/*
 * Copyright (C) 2005 to 2014 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2017 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "espeak_ng.h"
#include "speak_lib.h"
#include "encoding.h"

#include "error.h"
#include "speech.h"
#include "synthesize.h"
#include "translate.h"

typedef struct {
	unsigned int value;
	char *name;
} NAMETAB;

NAMETAB *manifest = NULL;
int n_manifest;
char phsrc[sizeof(path_home)+40]; // Source: path to the 'phonemes' source file.

extern ESPEAK_NG_API int utf8_in(int *c, const char *buf);


typedef struct {
	const char *mnem;
	int type;
	int data;
} keywtab_t;

#define k_AND     1
#define k_OR      2
#define k_THEN    3
#define k_NOT     4

#define kTHISSTRESS 0x800

// keyword types
enum {
	tPHONEME_TYPE = 1,
	tPHONEME_FLAG,
	tTRANSITION,
	tSTATEMENT,
	tINSTRN1,
	tWHICH_PHONEME,
	tTEST,
};

static keywtab_t k_conditions[] = {
	{ "AND",  0, k_AND },
	{ "OR",   0, k_OR },
	{ "THEN", 0, k_THEN },
	{ "NOT",  0, k_NOT },

	{ "prevPh",    tWHICH_PHONEME,  0 },
	{ "thisPh",    tWHICH_PHONEME,  1 },
	{ "nextPh",    tWHICH_PHONEME,  2 },
	{ "next2Ph",   tWHICH_PHONEME,  3 },
	{ "nextPhW",   tWHICH_PHONEME,  4 },
	{ "prevPhW",   tWHICH_PHONEME,  5 },
	{ "next2PhW",  tWHICH_PHONEME,  6 },
	{ "nextVowel", tWHICH_PHONEME,  7 },
	{ "prevVowel", tWHICH_PHONEME,  8 },
	{ "next3PhW",  tWHICH_PHONEME,  9 },
	{ "prev2PhW",  tWHICH_PHONEME, 10 },

	{ "PreVoicing",  tTEST, 0xf01 },
	{ "KlattSynth",  tTEST, 0xf02 },
	{ "MbrolaSynth", tTEST, 0xf03 },

	{ NULL, 0, 0 }
};

static keywtab_t k_properties[] = {
	{ "isPause",      0, CONDITION_IS_PHONEME_TYPE | phPAUSE },
	{ "isVowel",      0, CONDITION_IS_PHONEME_TYPE | phVOWEL },
	{ "isNasal",      0, CONDITION_IS_PHONEME_TYPE | phNASAL },
	{ "isLiquid",     0, CONDITION_IS_PHONEME_TYPE | phLIQUID },
	{ "isUStop",      0, CONDITION_IS_PHONEME_TYPE | phSTOP },
	{ "isVStop",      0, CONDITION_IS_PHONEME_TYPE | phVSTOP },
	{ "isVFricative", 0, CONDITION_IS_PHONEME_TYPE | phVFRICATIVE },

	{ "isPalatal",  0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_PALATAL },
	{ "isLong",     0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_LONG },
	{ "isRhotic",   0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_RHOTIC },
	{ "isSibilant", 0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_SIBILANT },
	{ "isFlag1",    0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_FLAG1 },
	{ "isFlag2",    0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_FLAG2 },

	{ "isVelar", 0, CONDITION_IS_PLACE_OF_ARTICULATION | phPLACE_VELAR },

	{ "isDiminished",  0, CONDITION_IS_OTHER | isDiminished },
	{ "isUnstressed",  0, CONDITION_IS_OTHER | isUnstressed },
	{ "isNotStressed", 0, CONDITION_IS_OTHER | isNotStressed },
	{ "isStressed",    0, CONDITION_IS_OTHER | isStressed },
	{ "isMaxStress",   0, CONDITION_IS_OTHER | isMaxStress },

	{ "isPause2",           0, CONDITION_IS_OTHER | isBreak },
	{ "isWordStart",        0, CONDITION_IS_OTHER | isWordStart },
	{ "isWordEnd",          0, CONDITION_IS_OTHER | isWordEnd },
	{ "isAfterStress",      0, CONDITION_IS_OTHER | isAfterStress },
	{ "isNotVowel",         0, CONDITION_IS_OTHER | isNotVowel },
	{ "isFinalVowel",       0, CONDITION_IS_OTHER | isFinalVowel },
	{ "isVoiced",           0, CONDITION_IS_OTHER | isVoiced },
	{ "isFirstVowel",       0, CONDITION_IS_OTHER | isFirstVowel },
	{ "isSecondVowel",      0, CONDITION_IS_OTHER | isSecondVowel },
	{ "isTranslationGiven", 0, CONDITION_IS_OTHER | isTranslationGiven },

	{ NULL, 0, 0 }
};

enum {
	kPHONEMESTART = 1,
	kUTF8_BOM,
	kPROCEDURE,
	kENDPHONEME,
	kENDPROCEDURE,
	kPHONEMETABLE,
	kINCLUDE,
	kIMPORT_PH,

	kSTARTTYPE,
	kENDTYPE,
	kSTRESSTYPE,
	kVOICINGSWITCH,

	kIF,
	kELSE,
	kELIF,
	kENDIF,
	kCALLPH,

	kSWITCH_PREVVOWEL,
	kSWITCH_NEXTVOWEL,
	kENDSWITCH,

	kFMT,
	kWAV,
	kVOWELSTART,
	kVOWELENDING,
	kANDWAV,

	kVOWELIN,
	kVOWELOUT,
	kTONESPEC,

	kRETURN,
	kCONTINUE,
};

enum {
	kTUNE = 1,
	kENDTUNE,
	kTUNE_PREHEAD,
	kTUNE_ONSET,
	kTUNE_HEAD,
	kTUNE_HEADENV,
	kTUNE_HEADEXTEND,
	kTUNE_HEADLAST,
	kTUNE_NUCLEUS0,
	kTUNE_NUCLEUS1,
	kTUNE_SPLIT,
};

static unsigned const char utf8_bom[] = { 0xef, 0xbb, 0xbf, 0 };

static keywtab_t k_intonation[] = {
	{ "tune",       0, kTUNE },
	{ "endtune",    0, kENDTUNE },
	{ "prehead",    0, kTUNE_PREHEAD },
	{ "onset",      0, kTUNE_ONSET },
	{ "head",       0, kTUNE_HEAD },
	{ "headenv",    0, kTUNE_HEADENV },
	{ "headextend", 0, kTUNE_HEADEXTEND },
	{ "headlast",   0, kTUNE_HEADLAST },
	{ "nucleus0",   0, kTUNE_NUCLEUS0 },
	{ "nucleus",    0, kTUNE_NUCLEUS1 },
	{ "split",      0, kTUNE_SPLIT },

	{ NULL, 0, -1 }
};

static keywtab_t keywords[] = {
	{ "liquid",  tPHONEME_TYPE, phLIQUID },
	{ "pause",   tPHONEME_TYPE, phPAUSE },
	{ "stress",  tPHONEME_TYPE, phSTRESS },
	{ "virtual", tPHONEME_TYPE, phVIRTUAL },
	{ "delete_phoneme", tPHONEME_TYPE, phDELETED },

	// keywords
	{ "phonemetable",         tSTATEMENT, kPHONEMETABLE },
	{ "include",              tSTATEMENT, kINCLUDE },
	{ (const char *)utf8_bom, tSTATEMENT, kUTF8_BOM },

	{ "phoneme",        tSTATEMENT, kPHONEMESTART },
	{ "procedure",      tSTATEMENT, kPROCEDURE },
	{ "endphoneme",     tSTATEMENT, kENDPHONEME },
	{ "endprocedure",   tSTATEMENT, kENDPROCEDURE },
	{ "import_phoneme", tSTATEMENT, kIMPORT_PH },
	{ "stress_type",    tSTATEMENT, kSTRESSTYPE },
	{ "starttype",      tSTATEMENT, kSTARTTYPE },
	{ "endtype",        tSTATEMENT, kENDTYPE },
	{ "voicingswitch",  tSTATEMENT, kVOICINGSWITCH },

	{ "IF",     tSTATEMENT, kIF },
	{ "ELSE",   tSTATEMENT, kELSE },
	{ "ELIF",   tSTATEMENT, kELIF },
	{ "ELSEIF", tSTATEMENT, kELIF },  // same as ELIF
	{ "ENDIF",  tSTATEMENT, kENDIF },
	{ "CALL",   tSTATEMENT, kCALLPH },
	{ "RETURN", tSTATEMENT, kRETURN },

	{ "PrevVowelEndings", tSTATEMENT, kSWITCH_PREVVOWEL },
	{ "NextVowelStarts",  tSTATEMENT, kSWITCH_NEXTVOWEL },
	{ "EndSwitch",        tSTATEMENT, kENDSWITCH },

	{ "Tone",        tSTATEMENT, kTONESPEC },
	{ "FMT",         tSTATEMENT, kFMT },
	{ "WAV",         tSTATEMENT, kWAV },
	{ "VowelStart",  tSTATEMENT, kVOWELSTART },
	{ "VowelEnding", tSTATEMENT, kVOWELENDING },
	{ "addWav",      tSTATEMENT, kANDWAV },

	{ "Vowelin",  tSTATEMENT, kVOWELIN },
	{ "Vowelout", tSTATEMENT, kVOWELOUT },
	{ "Continue", tSTATEMENT, kCONTINUE },

	{ "ChangePhoneme",       tINSTRN1, i_CHANGE_PHONEME },
	{ "ChangeNextPhoneme",   tINSTRN1, i_REPLACE_NEXT_PHONEME },
	{ "InsertPhoneme",       tINSTRN1, i_INSERT_PHONEME },
	{ "AppendPhoneme",       tINSTRN1, i_APPEND_PHONEME },
	{ "IfNextVowelAppend",   tINSTRN1, i_APPEND_IFNEXTVOWEL },
	{ "ChangeIfDiminished",  tINSTRN1, i_CHANGE_IF | isDiminished },
	{ "ChangeIfUnstressed",  tINSTRN1, i_CHANGE_IF | isUnstressed },
	{ "ChangeIfNotStressed", tINSTRN1, i_CHANGE_IF | isNotStressed },
	{ "ChangeIfStressed",    tINSTRN1, i_CHANGE_IF | isStressed },

	{ "PauseBefore", tINSTRN1, i_PAUSE_BEFORE },
	{ "PauseAfter",  tINSTRN1, i_PAUSE_AFTER },
	{ "length",      tINSTRN1, i_SET_LENGTH },
	{ "LongLength",  tINSTRN1, i_LONG_LENGTH },
	{ "LengthAdd",   tINSTRN1, i_ADD_LENGTH },
	{ "lengthmod",   tINSTRN1, i_LENGTH_MOD },
	{ "ipa",         tINSTRN1, i_IPA_NAME },

	// flags
	{ "unstressed",   tPHONEME_FLAG, phUNSTRESSED },
	{ "nolink",       tPHONEME_FLAG, phNOLINK },
	{ "brkafter",     tPHONEME_FLAG, phBRKAFTER },
	{ "rhotic",       tPHONEME_FLAG, phRHOTIC },
	{ "lengthenstop", tPHONEME_FLAG, phLENGTHENSTOP },
	{ "nopause",      tPHONEME_FLAG, phNOPAUSE },
	{ "prevoice",     tPHONEME_FLAG, phPREVOICE },

	{ "flag1", tPHONEME_FLAG, phFLAG1 },
	{ "flag2", tPHONEME_FLAG, phFLAG2 },

	// vowel transition attributes
	{ "len=",   tTRANSITION,  1 },
	{ "rms=",   tTRANSITION,  2 },
	{ "f1=",    tTRANSITION,  3 },
	{ "f2=",    tTRANSITION,  4 },
	{ "f3=",    tTRANSITION,  5 },
	{ "brk",    tTRANSITION,  6 },
	{ "rate",   tTRANSITION,  7 },
	{ "glstop", tTRANSITION,  8 },
	{ "lenadd", tTRANSITION,  9 },
	{ "f4",     tTRANSITION, 10 },
	{ "gpaus",  tTRANSITION, 11 },
	{ "colr=",  tTRANSITION, 12 },
	{ "amp=",   tTRANSITION, 13 },  // set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)

	{ NULL, 0, -1 }
};

static keywtab_t *keyword_tabs[] = {
	keywords, k_conditions, k_properties, k_intonation
};

static PHONEME_TAB *phoneme_out;

static int n_phcodes_list[N_PHONEME_TABS];
static PHONEME_TAB_LIST phoneme_tab_list2[N_PHONEME_TABS];
static PHONEME_TAB *phoneme_tab2;

#define N_PROCS 50
int n_procs;
int proc_addr[N_PROCS];
char proc_names[40][N_PROCS];

#define MAX_PROG_BUF 2000
USHORT *prog_out;
USHORT *prog_out_max;
USHORT prog_buf[MAX_PROG_BUF+20];

static espeak_ng_STATUS ReadPhondataManifest(espeak_ng_ERROR_CONTEXT *context)
{
	// Read the phondata-manifest file
	FILE *f;
	int n_lines = 0;
	int ix;
	char *p;
	unsigned int value;
	char buf[sizeof(path_home)+40];
	char name[120];

	sprintf(buf, "%s%c%s", path_home, PATHSEP, "phondata-manifest");
	if ((f = fopen(buf, "r")) == NULL)
		return create_file_error_context(context, static_cast<espeak_ng_STATUS> (errno), buf);

	while (fgets(buf, sizeof(buf), f) != NULL)
		n_lines++;

	rewind(f);

	if (manifest != NULL) {
		for (ix = 0; ix < n_manifest; ix++)
			free(manifest[ix].name);
	}

	if (n_lines == 0) {
		fclose(f);
		return ENS_EMPTY_PHONEME_MANIFEST;
	}

	NAMETAB *new_manifest = (NAMETAB *)realloc(manifest, n_lines * sizeof(NAMETAB));
	if (new_manifest == NULL) {
		fclose(f);
		free(manifest);
		return static_cast<espeak_ng_STATUS> (ENOMEM);
	} else
		manifest = new_manifest;

	n_manifest = 0;
	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (!isalpha(buf[0]))
			continue;

		if (sscanf(&buf[2], "%x %s", &value, name) == 2) {
			if ((p = (char *)malloc(strlen(name)+1)) != NULL) {
				strcpy(p, name);
				manifest[n_manifest].value = value;
				manifest[n_manifest].name = p;
				n_manifest++;
			}
		}
	}
	fclose(f);

	return ENS_OK;
}

static int n_phoneme_tabs;
static int n_phcodes;

// outout files
static FILE *f_phdata;
static FILE *f_phindex;
static FILE *f_phtab;
static FILE *f_phcontents;
static FILE *f_errors = NULL;
static FILE *f_prog_log = NULL;
static FILE *f_report;

static FILE *f_in;
static int f_in_linenum;
static int f_in_displ;

static int linenum;
static int count_references = 0;
static int duplicate_references = 0;
static int count_frames = 0;
static int error_count = 0;
static int resample_count = 0;
static int resample_fails = 0;
static int then_count = 0;
static int after_if = 0;

static char current_fname[80];

static int markers_used[8];

typedef struct {
	void *link;
	int value;
	int ph_mnemonic;
	short ph_table;
	char string[1];
} REF_HASH_TAB;

static REF_HASH_TAB *ref_hash_tab[256];

#define N_ENVELOPES  30
int n_envelopes = 0;
char envelope_paths[N_ENVELOPES][80];
unsigned char envelope_dat[N_ENVELOPES][ENV_LEN];

typedef struct {
	FILE *file;
	int linenum;
	char fname[80];
} STACK;

#define N_STACK  12
int stack_ix;
STACK stack[N_STACK];

#define N_IF_STACK 12
int if_level;
typedef struct {
	USHORT *p_then;
	USHORT *p_else;
	int returned;
} IF_STACK;
IF_STACK if_stack[N_IF_STACK];

enum {
	tENDFILE = 1,
	tSTRING,
	tNUMBER,
	tSIGNEDNUMBER,
	tPHONEMEMNEM,
	tOPENBRACKET,
	tKEYWORD,
	tCONDITION,
	tPROPERTIES,
	tINTONATION,
};

int item_type;
int item_terminator;
#define N_ITEM_STRING 256
char item_string[N_ITEM_STRING];

static int ref_sorter(char **a, char **b)
{
	int ix;

	REF_HASH_TAB *p1 = (REF_HASH_TAB *)(*a);
	REF_HASH_TAB *p2 = (REF_HASH_TAB *)(*b);

	ix = strcoll(p1->string, p2->string);
	if (ix != 0)
		return ix;

	ix = p1->ph_table - p2->ph_table;
	if (ix != 0)
		return ix;

	return p1->ph_mnemonic - p2->ph_mnemonic;
}

static void CompileReport(void)
{
	int ix;
	int hash;
	int n;
	REF_HASH_TAB *p;
	REF_HASH_TAB **list;
	const char *data_path;
	int prev_table;
	int procedure_num;
	int prev_mnemonic;

	if (f_report == NULL)
		return;

	// make a list of all the references and sort it
	list = (REF_HASH_TAB **)malloc((count_references)* sizeof(REF_HASH_TAB *));
	if (list == NULL)
		return;

	fprintf(f_report, "\n%d phoneme tables\n", n_phoneme_tabs);
	fprintf(f_report, "          new total\n");
	for (ix = 0; ix < n_phoneme_tabs; ix++)
		fprintf(f_report, "%8s %3d %4d\n", phoneme_tab_list2[ix].name, phoneme_tab_list2[ix].n_phonemes, n_phcodes_list[ix]+1);
	fputc('\n', f_report);

	fprintf(f_report, "Data file      Used by\n");
	ix = 0;
	for (hash = 0; (hash < 256) && (ix < count_references); hash++) {
		p = ref_hash_tab[hash];
		while (p != NULL) {
			list[ix++] = p;
			p = (REF_HASH_TAB *)(p->link);
		}
	}
	n = ix;
	qsort((void *)list, n, sizeof(REF_HASH_TAB *), (int (*)(const void *, const void *))ref_sorter);

	data_path = "";
	prev_mnemonic = 0;
	prev_table = 0;
	for (ix = 0; ix < n; ix++) {
		int j = 0;

		if (strcmp(list[ix]->string, data_path) != 0) {
			data_path = list[ix]->string;
			j = strlen(data_path);
			fprintf(f_report, "%s", data_path);
		} else if ((list[ix]->ph_table == prev_table) && (list[ix]->ph_mnemonic == prev_mnemonic))
			continue; // same phoneme, don't list twice

		while (j < 14) {
			fputc(' ', f_report); // pad filename with spaces
			j++;
		}

		prev_mnemonic = list[ix]->ph_mnemonic;
		if ((prev_mnemonic >> 24) == 'P') {
			// a procedure, not a phoneme
			procedure_num = atoi(WordToString(prev_mnemonic));
			fprintf(f_report, "  %s  %s", phoneme_tab_list2[prev_table = list[ix]->ph_table].name, proc_names[procedure_num]);
		} else
			fprintf(f_report, "  [%s] %s", WordToString(prev_mnemonic), phoneme_tab_list2[prev_table = list[ix]->ph_table].name);
		fputc('\n', f_report);
	}

	for (ix = 0; ix < n; ix++) {
		free(list[ix]);
		list[ix] = NULL;
	}

	free(list);
	list = NULL;
}

static void error(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	fprintf(f_errors, "%s(%d): ", current_fname, linenum-1);
	vfprintf(f_errors, format, args);
	fprintf(f_errors, "\n");
	error_count++;

	va_end(args);
}

static void error_from_status(espeak_ng_STATUS status, const char *context)
{
	char message[512];
	espeak_ng_GetStatusCodeMessage(status, message, sizeof(message));
	if (context)
		error("%s: '%s'.", message, context);
	else
		error("%s.", message);
}

static unsigned int StringToWord(const char *string)
{
	// Pack 4 characters into a word
	int ix;
	unsigned char c;
	unsigned int word;

	if (string == NULL)
		return 0;

	word = 0;
	for (ix = 0; ix < 4; ix++) {
		if (string[ix] == 0) break;
		c = string[ix];
		word |= (c << (ix*8));
	}
	return word;
}

static MNEM_TAB reserved_phonemes[] = {
	{ "_\001",  phonCONTROL },      // NOT USED
	{ "%",      phonSTRESS_U },
	{ "%%",     phonSTRESS_D },
	{ ",",      phonSTRESS_2 },
	{ ",,",     phonSTRESS_3 },
	{ "'",      phonSTRESS_P },
	{ "''",     phonSTRESS_P2 },
	{ "=",      phonSTRESS_PREV },  //  stress previous syllable
	{ "_:",     phonPAUSE },        //  pause
	{ "_",      phonPAUSE_SHORT },  //  short pause
	{ "_!",     phonPAUSE_NOLINK }, //  short pause, no link
	{ ":",      phonLENGTHEN },
	{ "@",      phonSCHWA },
	{ "@-",     phonSCHWA_SHORT },
	{ "||",     phonEND_WORD },
	{ "1",      phonDEFAULTTONE },  // (numeral 1)  default tone (for tone language)
	{ "#X1",    phonCAPITAL },      // capital letter indication
	{ "?",      phonGLOTTALSTOP },  // glottal stop
	{ "-",      phonSYLLABIC },     // syllabic consonant
	{ "_^_",    phonSWITCH },       //  Change language
	{ "_X1",    phonX1 },           // a language specific action
	{ "_|",     phonPAUSE_VSHORT }, // very short pause
	{ "_::",    phonPAUSE_LONG },   // long pause
	{ "t#",     phonT_REDUCED },    // reduced version of [t]
	{ "'!",     phonSTRESS_TONIC }, // stress - emphasized
	{ "_;_",    phonPAUSE_CLAUSE }, // clause pause

	{ "#@",     phonVOWELTYPES },   // vowel type groups, these must be consecutive
	{ "#a",     phonVOWELTYPES+1 },
	{ "#e",     phonVOWELTYPES+2 },
	{ "#i",     phonVOWELTYPES+3 },
	{ "#o",     phonVOWELTYPES+4 },
	{ "#u",     phonVOWELTYPES+5 },

	{ NULL, 0 }
};

static void ReservePhCodes()
{
	// Reserve phoneme codes which have fixed numbers so that they can be
	// referred to from the program code.
	unsigned int word;
	MNEM_TAB *p;

	p = reserved_phonemes;
	while (p->mnem != NULL) {
		word = StringToWord(p->mnem);
		phoneme_tab2[p->value].mnemonic = word;
		phoneme_tab2[p->value].code = p->value;
		if (n_phcodes <= p->value)
			n_phcodes = p->value+1;
		p++;
	}
}

static int LookupPhoneme(const char *string, int control)
{
	// control = 0   explicit declaration
	// control = 1   declare phoneme if not found
	// control = 2   start looking after control & stress phonemes

	int ix;
	int start;
	int use;
	unsigned int word;

	if (strcmp(string, "NULL") == 0)
		return 1;

	ix = strlen(string);
	if ((ix == 0) || (ix > 4))
		error("Bad phoneme name '%s'", string);
	word = StringToWord(string);

	// don't use phoneme number 0, reserved for string terminator
	start = 1;

	if (control == 2) {
		// don't look for control and stress phonemes (allows these characters to be
		// used for other purposes)
		start = 8;
	}

	use = 0;
	for (ix = start; ix < n_phcodes; ix++) {
		if (phoneme_tab2[ix].mnemonic == word)
			return ix;

		if ((use == 0) && (phoneme_tab2[ix].mnemonic == 0))
			use = ix;
	}

	if (use == 0) {
		if (control == 0)
			return -1;
		if (n_phcodes >= N_PHONEME_TAB-1)
			return -1; // phoneme table is full
		use = n_phcodes++;
	}

	// add this phoneme to the phoneme table
	phoneme_tab2[use].mnemonic = word;
	phoneme_tab2[use].type = phINVALID;
	phoneme_tab2[use].program = linenum; // for error report if the phoneme remains undeclared
	return use;
}

static unsigned int get_char()
{
	unsigned int c;
	c = fgetc(f_in);
	if (c == '\n')
		linenum++;
	return c;
}

static void unget_char(unsigned int c)
{
	ungetc(c, f_in);
	if (c == '\n')
		linenum--;
}

static int CheckNextChar()
{
	int c;
	while (((c = get_char()) == ' ') || (c == '\t'))
		;
	unget_char(c);
	return c;
}

static int NextItem(int type)
{
	int acc;
	unsigned char c = 0;
	unsigned char c2;
	int ix;
	int sign;
	char *p;
	keywtab_t *pk;

	item_type = -1;

	f_in_displ = ftell(f_in);
	f_in_linenum = linenum;

	while (!feof(f_in)) {
		c = get_char();
		if (c == '/') {
			if ((c2 = get_char()) == '/') {
				// comment, ignore to end of line
				while (!feof(f_in) && ((c = get_char()) != '\n'))
					;
			} else
				unget_char(c2);
		}
		if (!isspace(c))
			break;
	}
	if (feof(f_in))
		return -2;

	if (c == '(') {
		if (type == tOPENBRACKET)
			return 1;
		return -1;
	}

	ix = 0;
	while (!feof(f_in) && !isspace(c) && (c != '(') && (c != ')') && (c != ',')) {
		if (c == '\\')
			c = get_char();
		item_string[ix++] = c;
		c = get_char();
		if (feof(f_in))
			break;
		if (item_string[ix-1] == '=')
			break;
	}
	item_string[ix] = 0;

	while (isspace(c))
		c = get_char();

	item_terminator = ' ';
	if ((c == ')') || (c == '(') || (c == ','))
		item_terminator = c;

	if ((c == ')') || (c == ','))
		c = ' ';

	if (!feof(f_in))
		unget_char(c);

	if (type == tSTRING)
		return 0;

	if ((type == tNUMBER) || (type == tSIGNEDNUMBER)) {
		acc = 0;
		sign = 1;
		p = item_string;

		if ((*p == '-') && (type == tSIGNEDNUMBER)) {
			sign = -1;
			p++;
		}
		if (!isdigit(*p)) {
			if ((type == tNUMBER) && (*p == '-'))
				error("Expected an unsigned number");
			else
				error("Expected a number");
		}
		while (isdigit(*p)) {
			acc *= 10;
			acc += (*p - '0');
			p++;
		}
		return acc * sign;
	}

	if ((type >= tKEYWORD) && (type <= tINTONATION)) {
		pk = keyword_tabs[type-tKEYWORD];
		while (pk->mnem != NULL) {
			if (strcmp(item_string, pk->mnem) == 0) {
				item_type = pk->type;
				return pk->data;
			}
			pk++;
		}
		item_type = -1;
		return -1; // keyword not found
	}
	if (type == tPHONEMEMNEM)
		return LookupPhoneme(item_string, 2);
	return -1;
}

static int NextItemMax(int max)
{
	// Get a number, but restrict value to max
	int value;

	value = NextItem(tNUMBER);
	if (value > max) {
		error("Value %d is greater than maximum %d", value, max);
		value = max;
	}
	return value;
}

static int NextItemBrackets(int type, int control)
{
	// Expect a parameter inside parantheses
	// control: bit 0  0= need (
	//          bit 1  1= allow comma

	int value;

	if ((control & 1) == 0) {
		if (!NextItem(tOPENBRACKET))
			error("Expected '('");
	}

	value = NextItem(type);
	if ((control & 2) && (item_terminator == ','))
		return value;

	if (item_terminator != ')')
		error("Expected ')'");
	return value;
}

static void UngetItem()
{
	fseek(f_in, f_in_displ, SEEK_SET);
	linenum = f_in_linenum;
}

static int Range(int value, int divide, int min, int max)
{
	if (value < 0)
		value -= divide/2;
	else
		value += divide/2;
	value = value / divide;

	if (value > max)
		value = max;
	if (value < min)
		value = min;
	return value - min;
}

static int CompileVowelTransition(int which)
{
	// Compile a vowel transition
	int key;
	int len = 0;
	int rms = 0;
	int f1 = 0;
	int f2 = 0;
	int f2_min = 0;
	int f2_max = 0;
	int f3_adj = 0;
	int f3_amp = 0;
	int flags = 0;
	int vcolour = 0;
	int x;
	int instn = i_VOWELIN;
	int word1;
	int word2;

	if (which == 1) {
		len = 50 / 2; // defaults for transition into vowel
		rms = 25 / 2;

		if (phoneme_out->type == phSTOP) {
			len = 42 / 2; // defaults for transition into vowel
			rms = 30 / 2;
		}
	} else if (which == 2) {
		instn = i_VOWELOUT;
		len = 36 / 2; // defaults for transition out of vowel
		rms = 16 / 2;
	}

	for (;;) {
		key = NextItem(tKEYWORD);
		if (item_type != tTRANSITION) {
			UngetItem();
			break;
		}

		switch (key & 0xf)
		{
		case 1:
			len = Range(NextItem(tNUMBER), 2, 0, 63) & 0x3f;
			flags |= 1;
			break;
		case 2:
			rms = Range(NextItem(tNUMBER), 2, 0, 31) & 0x1f;
			flags |= 1;
			break;
		case 3:
			f1 = NextItem(tNUMBER);
			break;
		case 4:
			f2 = Range(NextItem(tNUMBER), 50, 0, 63) & 0x3f;
			f2_min = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f2_max = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			if (f2_min > f2_max) {
				x = f2_min;
				f2_min = f2_max;
				f2_max = x;
			}
			break;
		case 5:
			f3_adj = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f3_amp = Range(NextItem(tNUMBER), 8, 0, 15) & 0x1f;
			break;
		case 6:
			flags |= 2; // break
			break;
		case 7:
			flags |= 4; // rate
			break;
		case 8:
			flags |= 8; // glstop
			break;
		case 9:
			flags |= 16; // lenadd
			break;
		case 10:
			flags |= 32;  // f4
			break;
		case 11:
			flags |= 64;  // pause
			break;
		case 12:
			vcolour = NextItem(tNUMBER);
			break;
		case 13:
			// set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)
			rms = (Range(NextItem(tNUMBER), 1, 0, 31) & 0x1f) | 0x20;
			flags |= 1;
			break;
		}
	}
	word1 = len + (rms << 6) + (flags << 12);
	word2 =  f2 + (f2_min << 6) + (f2_max << 11) + (f3_adj << 16) + (f3_amp << 21) + (f1 << 26) + (vcolour << 29);
	prog_out[0] = instn + ((word1 >> 16) & 0xff);
	prog_out[1] = word1;
	prog_out[2] = word2 >> 16;
	prog_out[3] = word2;
	prog_out += 4;

	return 0;
}

static int LoadWavefile(FILE *f, const char *fname)
{
	int displ;
	unsigned char c1;
	unsigned char c3;
	int c2;
	int sample;
	int sample2;
	float x;
	int max = 0;
	int length;
	int sr1, sr2;
	int failed;
	int len;
	int resample_wav = 0;
	const char *fname2;
	char fname_temp[100];
	char msg[120];
	int scale_factor = 0;

	fseek(f, 24, SEEK_SET);
	sr1 = Read4Bytes(f);
	sr2 = Read4Bytes(f);
	fseek(f, 40, SEEK_SET);

	if ((sr1 != samplerate_native) || (sr2 != sr1*2)) {
		char command[sizeof(path_home)+250];

		failed = 0;

#ifdef HAVE_MKSTEMP
		int fd_temp;
		strcpy(fname_temp, "/tmp/espeakXXXXXX");
		if ((fd_temp = mkstemp(fname_temp)) >= 0)
			close(fd_temp);
#else
		strcpy(fname_temp, tmpnam(NULL));
#endif

		fname2 = fname;
		len = strlen(fname);
		if (strcmp(&fname[len-4], ".wav") == 0) {
			strcpy(msg, fname);
			msg[len-4] = 0;
			fname2 = msg;
		}

		sprintf(command, "sox \"%s/%s.wav\" -r %d -c1 -t wav %s\n", phsrc, fname2, samplerate_native, fname_temp);
		if (system(command) != 0)
			failed = 1;

		if (failed || (GetFileLength(fname_temp) <= 0)) {
			if (resample_fails < 2)
				error("Resample command failed: %s", command);
			resample_fails++;

			if (sr1 != samplerate_native)
				error("Can't resample (%d to %d): %s", sr1, samplerate_native, fname);
			else
				error("WAV file is not mono: %s", fname);
			remove(fname_temp);
			return 0;
		}

		f = fopen(fname_temp, "rb");
		if (f == NULL) {
			error("Can't read temp file: %s", fname_temp);
			return 0;
		}
		if (f_report != NULL)
			fprintf(f_report, "resampled  %s\n", fname);
		resample_count++;
		resample_wav = 1;
		fseek(f, 40, SEEK_SET); // skip past the WAV header, up to before "data length"
	}

	displ = ftell(f_phdata);

	// data contains:  4 bytes of length (n_samples * 2), followed by 2-byte samples (lsb byte first)
	length = Read4Bytes(f);

	while (true) {
		int c;

		if ((c = fgetc(f)) == EOF)
			break;
		c1 = (unsigned char)c;

		if ((c = fgetc(f)) == EOF)
			break;
		c3 = (unsigned char)c;

		c2 = c3 << 24;
		c2 = c2 >> 16; // sign extend

		sample = (c1 & 0xff) + c2;

		if (sample > max)
			max = sample;
		else if (sample < -max)
			max = -sample;
	}

	scale_factor = (max / 127) + 1;

	#define MIN_FACTOR   -1 // was 6, disable use of 16 bit samples
	if (scale_factor > MIN_FACTOR) {
		length = length/2 + (scale_factor << 16);
	}

	Write4Bytes(f_phdata, length);
	fseek(f, 44, SEEK_SET);

	while (!feof(f)) {
		c1 = fgetc(f);
		c3 = fgetc(f);
		c2 = c3 << 24;
		c2 = c2 >> 16; // sign extend

		sample = (c1 & 0xff) + c2;

		if (feof(f)) break;

		if (scale_factor <= MIN_FACTOR) {
			fputc(sample & 0xff, f_phdata);
			fputc(sample >> 8, f_phdata);
		} else {
			x = ((float)sample / scale_factor) + 0.5;
			sample2 = (int)x;
			if (sample2 > 127)
				sample2 = 127;
			if (sample2 < -128)
				sample2 = -128;
			fputc(sample2, f_phdata);
		}
	}

	length = ftell(f_phdata);
	while ((length & 3) != 0) {
		// pad to a multiple of 4 bytes
		fputc(0, f_phdata);
		length++;
	}

	if (resample_wav != 0) {
		fclose(f);
		remove(fname_temp);
	}
	return displ | 0x800000; // set bit 23 to indicate a wave file rather than a spectrum
}

static espeak_ng_STATUS LoadEnvelope(FILE *f, const char *fname, int *displ)
{
	char buf[128];

	if (displ)
		*displ = ftell(f_phdata);

	if (fseek(f, 12, SEEK_SET) == -1)
		return static_cast<espeak_ng_STATUS> (errno);

	if (fread(buf, 128, 1, f) != 128)
		return static_cast<espeak_ng_STATUS> (errno);
	fwrite(buf, 128, 1, f_phdata);

	if (n_envelopes < N_ENVELOPES) {
		strncpy0(envelope_paths[n_envelopes], fname, sizeof(envelope_paths[0]));
		memcpy(envelope_dat[n_envelopes], buf, sizeof(envelope_dat[0]));
		n_envelopes++;
	}

	return ENS_OK;
}

// Generate a hash code from the specified string
static int Hash8(const char *string)
{
	int c;
	int chars = 0;
	int hash = 0;

	while ((c = *string++) != 0) {
		c = tolower(c) - 'a';
		hash = hash * 8 + c;
		hash = (hash & 0x1ff) ^ (hash >> 8); // exclusive or
		chars++;
	}

	return (hash+chars) & 0xff;
}

static int LoadEnvelope2(FILE *f, const char *fname)
{
	int ix, ix2;
	int n;
	int x, y;
	int displ;
	int n_points;
	double yy;
	char line_buf[128];
	float env_x[20];
	float env_y[20];
	int env_lin[20];
	unsigned char env[ENV_LEN];

	n_points = 0;
	(void) fgets(line_buf, sizeof(line_buf), f); // skip first line
	while (!feof(f)) {
		if (fgets(line_buf, sizeof(line_buf), f) == NULL)
			break;

		env_lin[n_points] = 0;
		n = sscanf(line_buf, "%f %f %d", &env_x[n_points], &env_y[n_points], &env_lin[n_points]);
		if (n >= 2) {
			env_x[n_points] *= (float)1.28; // convert range 0-100 to 0-128
			n_points++;
		}
	}
	if (n_points > 0) {
		env_x[n_points] = env_x[n_points-1];
		env_y[n_points] = env_y[n_points-1];
	}

	ix = -1;
	ix2 = 0;
	if (n_points > 0) for (x = 0; x < ENV_LEN; x++) {
		if (n_points > 3 && x > env_x[ix+4])
			ix++;
		if (n_points > 2 && x >= env_x[ix2+1])
			ix2++;

		if (env_lin[ix2] > 0) {
			yy = env_y[ix2] + (env_y[ix2+1] - env_y[ix2]) * ((float)x - env_x[ix2]) / (env_x[ix2+1] - env_x[ix2]);
			y = (int)(yy * 2.55);
		} else if (n_points > 3)
			y = (int)(polint(&env_x[ix], &env_y[ix], 4, x) * 2.55); // convert to range 0-255
		else
			y = (int)(polint(&env_x[ix], &env_y[ix], 3, x) * 2.55);
		if (y < 0) y = 0;
		if (y > 255) y = 255;
		env[x] = y;
	}

	if (n_envelopes < N_ENVELOPES) {
		strncpy0(envelope_paths[n_envelopes], fname, sizeof(envelope_paths[0]));
		memcpy(envelope_dat[n_envelopes], env, ENV_LEN);
		n_envelopes++;
	}

	displ = ftell(f_phdata);
	fwrite(env, 1, 128, f_phdata);

	return displ;
}

/*
   Condition
   bits 14,15   1
   bit 13       1 = AND, 0 = OR
   bit 12       spare
   bit 8-11
   =0-3       p,t,n,n2   data=phoneme code
   =4-7       p,t,n,n2   data=(bits5-7: phtype, place, property, special) (bits0-4: data)
   =8         data = stress bitmap
   =9         special tests
 */
static int CompileIf(int elif)
{
	int key;
	int finish = 0;
	int word = 0;
	int word2;
	int data;
	int bitmap;
	int brackets;
	int not_flag;
	USHORT *prog_last_if = NULL;

	then_count = 2;
	after_if = 1;

	while (!finish) {
		not_flag = 0;
		word2 = 0;
		if (prog_out >= prog_out_max) {
			error("Phoneme program too large");
			return 0;
		}

		if ((key = NextItem(tCONDITION)) < 0)
			error("Expected a condition, not '%s'", item_string);

		if ((item_type == 0) && (key == k_NOT)) {
			not_flag = 1;
			if ((key = NextItem(tCONDITION)) < 0)
				error("Expected a condition, not '%s'", item_string);
		}

		if (item_type == tWHICH_PHONEME) {
			// prevPh(), thisPh(), nextPh(), next2Ph() etc
			if (key >= 6) {
				// put the 'which' code in the next instruction
				word2 = key;
				key = 6;
			}
			key = key << 8;

			data = NextItemBrackets(tPROPERTIES, 0);
			if (data >= 0)
				word = key + data + 0x700;
			else {
				data = LookupPhoneme(item_string, 2);
				word = key + data;
			}
		} else if (item_type == tTEST) {
			if (key == kTHISSTRESS) {
				bitmap = 0;
				brackets = 2;
				do {
					data = NextItemBrackets(tNUMBER, brackets);
					if (data > 7)
						error("Expected list of stress levels");
					bitmap |= (1 << data);

					brackets = 3;
				} while (item_terminator == ',');
				word = i_StressLevel | bitmap;
			} else
				word = key;
		} else {
			error("Unexpected keyword '%s'", item_string);

			if ((strcmp(item_string, "phoneme") == 0) || (strcmp(item_string, "endphoneme") == 0))
				return -1;
		}

		// output the word
		prog_last_if = prog_out;
		*prog_out++ = word | i_CONDITION;

		if (word2 != 0)
			*prog_out++ = word2;
		if (not_flag)
			*prog_out++ = i_NOT;

		// expect AND, OR, THEN
		switch (NextItem(tCONDITION))
		{
		case k_AND:
			break;
		case k_OR:
			if (prog_last_if != NULL)
				*prog_last_if |=  i_OR;
			break;
		case k_THEN:
			finish = 1;
			break;
		default:
			error("Expected AND, OR, THEN");
			break;
		}
	}

	if (elif == 0) {
		if_level++;
		if_stack[if_level].p_else = NULL;
	}

	if_stack[if_level].returned = 0;
	if_stack[if_level].p_then = prog_out;
	*prog_out++ = i_JUMP_FALSE;

	return 0;
}

static void FillThen(int add)
{
	USHORT *p;
	int offset;

	p = if_stack[if_level].p_then;
	if (p != NULL) {
		offset = prog_out - p + add;

		if ((then_count == 1) && (if_level == 1)) {
			// The THEN part only contains one statement, we can remove the THEN jump
			// and the interpreter will implicitly skip the statement.
			while (p < prog_out) {
				p[0] = p[1];
				p++;
			}
			prog_out--;
		} else {
			if (offset > MAX_JUMP)
				error("IF block is too long");
			*p = i_JUMP_FALSE + offset;
		}
		if_stack[if_level].p_then = NULL;
	}

	then_count = 0;
}

static int CompileElse(void)
{
	USHORT *ref;
	USHORT *p;

	if (if_level < 1) {
		error("ELSE not expected");
		return 0;
	}

	if (if_stack[if_level].returned == 0)
		FillThen(1);
	else
		FillThen(0);

	if (if_stack[if_level].returned == 0) {
		ref = prog_out;
		*prog_out++ = 0;

		if ((p = if_stack[if_level].p_else) != NULL)
			*ref = ref - p; // backwards offset to the previous else
		if_stack[if_level].p_else = ref;
	}

	return 0;
}

static int CompileElif(void)
{
	if (if_level < 1) {
		error("ELIF not expected");
		return 0;
	}

	CompileElse();
	CompileIf(1);
	return 0;
}

static int CompileEndif(void)
{
	USHORT *p;
	int chain;
	int offset;

	if (if_level < 1) {
		error("ENDIF not expected");
		return 0;
	}

	FillThen(0);

	if ((p = if_stack[if_level].p_else) != NULL) {
		do {
			chain = *p; // a chain of previous else links

			offset = prog_out - p;
			if (offset > MAX_JUMP)
				error("IF block is too long");
			*p = i_JUMP + offset;

			p -= chain;
		} while (chain > 0);
	}

	if_level--;
	return 0;
}

static int CompileSwitch(int type)
{
	// Type 0:  EndSwitch
	//      1:  SwitchPrevVowelType
	//      2:  SwitchNextVowelType

	if (type == 0) {
		// check the instructions in the Switch
		return 0;
	}

	if (type == 1)
		*prog_out++ = i_SWITCH_PREVVOWEL+6;
	if (type == 2)
		*prog_out++ = i_SWITCH_NEXTVOWEL+6;
	return 0;
}

static PHONEME_TAB_LIST *FindPhonemeTable(const char *string)
{
	int ix;

	for (ix = 0; ix < n_phoneme_tabs; ix++) {
		if (strcmp(phoneme_tab_list2[ix].name, string) == 0)
			return &phoneme_tab_list2[ix];
	}
	error("Unknown phoneme table: '%s'", string);
	return NULL;
}

static PHONEME_TAB *FindPhoneme(const char *string)
{
	PHONEME_TAB_LIST *phtab = NULL;
	int ix;
	unsigned int mnem;
	char *phname;
	char buf[200];

	// is this the name of a phoneme which is in scope
	if ((strlen(string) <= 4) && ((ix = LookupPhoneme(string, 0)) != -1))
		return &phoneme_tab2[ix];

	// no, treat the name as phonemetable/phoneme
	strcpy(buf, string);
	if ((phname = strchr(buf, '/')) != 0)
		*phname++ = 0;

	phtab = FindPhonemeTable(buf);
	if (phtab == NULL)
		return NULL; // phoneme table not found

	mnem = StringToWord(phname);
	for (ix = 1; ix < 256; ix++) {
		if (mnem == phtab->phoneme_tab_ptr[ix].mnemonic)
			return &phtab->phoneme_tab_ptr[ix];
	}

	error("Phoneme reference not found: '%s'", string);
	return NULL;
}

static void ImportPhoneme(void)
{
	unsigned int ph_mnem;
	unsigned int ph_code;
	PHONEME_TAB *ph;

	NextItem(tSTRING);

	if ((ph = FindPhoneme(item_string)) == NULL) {
		error("Cannot find phoneme '%s' to import.", item_string);
		return;
	}

	if (phoneme_out->phflags != 0 ||
	    phoneme_out->type != phINVALID ||
	    phoneme_out->start_type != 0 ||
	    phoneme_out->end_type != 0 ||
	    phoneme_out->std_length != 0 ||
	    phoneme_out->length_mod != 0) {
		error("Phoneme import will override set properties.");
	}

	ph_mnem = phoneme_out->mnemonic;
	ph_code = phoneme_out->code;
	memcpy(phoneme_out, ph, sizeof(PHONEME_TAB));
	phoneme_out->mnemonic = ph_mnem;
	phoneme_out->code = ph_code;
	if (phoneme_out->type != phVOWEL)
		phoneme_out->end_type = 0;  // voicingswitch, this must be set later to refer to a local phoneme
}

static void CallPhoneme(void)
{
	PHONEME_TAB *ph;
	int ix;
	int addr = 0;

	NextItem(tSTRING);

	// first look for a procedure name
	for (ix = 0; ix < n_procs; ix++) {
		if (strcmp(proc_names[ix], item_string) == 0) {
			addr = proc_addr[ix];
			break;
		}
	}
	if (ix == n_procs) {
		// procedure not found, try a phoneme name
		if ((ph = FindPhoneme(item_string)) == NULL)
			return;
		addr = ph->program;

		if (phoneme_out->type == phINVALID) {
			// Phoneme type has not been set. Copy it from the called phoneme
			phoneme_out->type = ph->type;
			phoneme_out->start_type = ph->start_type;
			phoneme_out->end_type = ph->end_type;
			phoneme_out->std_length = ph->std_length;
			phoneme_out->length_mod = ph->length_mod;
		}
	}

	*prog_out++ = i_CALLPH + (addr >> 16);
	*prog_out++ = addr;
}

static void DecThenCount()
{
	if (then_count > 0)
		then_count--;
}

static void WritePhonemeTables()
{
	int ix;
	int j;
	int n;
	int value;
	int count;
	PHONEME_TAB *p;

	value = n_phoneme_tabs;
	fputc(value, f_phtab);
	fputc(0, f_phtab);
	fputc(0, f_phtab);
	fputc(0, f_phtab);

	for (ix = 0; ix < n_phoneme_tabs; ix++) {
		p = phoneme_tab_list2[ix].phoneme_tab_ptr;
		n = n_phcodes_list[ix];
		memset(&p[n], 0, sizeof(p[n]));
		p[n].mnemonic = 0; // terminate the phoneme table

		// count number of locally declared phonemes
		count = 0;
		for (j = 0; j < n; j++) {
			if (ix == 0)
				p[j].phflags |= phLOCAL; // write all phonemes in the base phoneme table

			if (p[j].phflags & phLOCAL)
				count++;
		}
		phoneme_tab_list2[ix].n_phonemes = count+1;

		fputc(count+1, f_phtab);
		fputc(phoneme_tab_list2[ix].includes, f_phtab);
		fputc(0, f_phtab);
		fputc(0, f_phtab);

		fwrite(phoneme_tab_list2[ix].name, 1, N_PHONEME_TAB_NAME, f_phtab);

		for (j = 0; j < n; j++) {
			if (p[j].phflags & phLOCAL) {
				// this bit is set temporarily to incidate a local phoneme, declared in
				// in the current phoneme file
				p[j].phflags &= ~phLOCAL;
				fwrite(&p[j], sizeof(PHONEME_TAB), 1, f_phtab);
			}
		}
		fwrite(&p[n], sizeof(PHONEME_TAB), 1, f_phtab); // include the extra list-terminator phoneme entry
		free(p);
	}
}

static void EndPhonemeTable()
{
	int ix;

	if (n_phoneme_tabs == 0)
		return;

	// check that all referenced phonemes have been declared
	for (ix = 0; ix < n_phcodes; ix++) {
		if (phoneme_tab2[ix].type == phINVALID) {
			error("Phoneme [%s] not declared, referenced at line %d",
			      WordToString(phoneme_tab2[ix].mnemonic), (int)(phoneme_tab2[ix].program));
			error_count++;
			phoneme_tab2[ix].type = 0; // prevent the error message repeating
		}
	}

	n_phcodes_list[n_phoneme_tabs-1] = n_phcodes;
}

static void StartPhonemeTable(const char *name)
{
	int ix;
	int j;
	PHONEME_TAB *p;

	if (n_phoneme_tabs >= N_PHONEME_TABS-1) {
		error("Too many phonemetables");
		return;
	}
	p = (PHONEME_TAB *)calloc(sizeof(PHONEME_TAB), N_PHONEME_TAB);

	if (p == NULL) {
		error("Out of memory");
		return;
	}

	memset(&phoneme_tab_list2[n_phoneme_tabs], 0, sizeof(PHONEME_TAB_LIST));
	phoneme_tab_list2[n_phoneme_tabs].phoneme_tab_ptr = phoneme_tab2 = p;
	memset(phoneme_tab_list2[n_phoneme_tabs].name, 0, sizeof(phoneme_tab_list2[n_phoneme_tabs].name));
	strncpy0(phoneme_tab_list2[n_phoneme_tabs].name, name, N_PHONEME_TAB_NAME);
	n_phcodes = 1;
	phoneme_tab_list2[n_phoneme_tabs].includes = 0;

	if (n_phoneme_tabs > 0) {
		NextItem(tSTRING); // name of base phoneme table
		for (ix = 0; ix < n_phoneme_tabs; ix++) {
			if (strcmp(item_string, phoneme_tab_list2[ix].name) == 0) {
				phoneme_tab_list2[n_phoneme_tabs].includes = ix+1;

				// initialise the new phoneme table with the contents of this one
				memcpy(phoneme_tab2, phoneme_tab_list2[ix].phoneme_tab_ptr, sizeof(PHONEME_TAB)*N_PHONEME_TAB);
				n_phcodes = n_phcodes_list[ix];

				// clear "local phoneme" bit"
				for (j = 0; j < n_phcodes; j++)
					phoneme_tab2[j].phflags &= ~phLOCAL;
				break;
			}
		}
		if (ix == n_phoneme_tabs && strcmp(item_string, "_") != 0)
			error("Can't find base phonemetable '%s'", item_string);
	} else
		ReservePhCodes();

	n_phoneme_tabs++;
}

#pragma GCC visibility push(default)

#pragma GCC visibility pop

static const char *preset_tune_names[] = {
	"s1", "c1", "q1", "e1", NULL
};

static const TUNE default_tune = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 40, 24, 8, 0, 0, 0, 0 },
	46, 57, PITCHfall, 16, 0, 0,
	255, 78, 50, 255,
	3, 5,
	{ -7, -7, -7 }, { -7, -7, -7 },
	PITCHfall, 64, 8,
	PITCHfall, 70, 18, 24, 12,
	PITCHfall, 70, 18, 24, 12, 0,
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, 0
};

#define N_TUNE_NAMES  100

MNEM_TAB envelope_names[] = {
	{ "fall", 0 },
	{ "rise", 2 },
	{ "fall-rise", 4 },
	{ "fall-rise2", 6 },
	{ "rise-fall",  8 },
	{ "fall-rise3", 10 },
	{ "fall-rise4", 12 },
	{ "fall2", 14 },
	{ "rise2", 16 },
	{ "rise-fall-rise", 18 },
	{ NULL, -1 }
};

static int LookupEnvelopeName(const char *name)
{
	return LookupMnem(envelope_names, name);
}

#pragma GCC visibility push(default)

espeak_ng_STATUS espeak_ng_CompileIntonation(FILE *log, espeak_ng_ERROR_CONTEXT *context)
{
	if (!log) log = stderr;

	int ix;
	char *p;
	char c;
	int keyword;
	int n_tune_names = 0;
	int done_split = 0;
	int done_onset = 0;
	int done_last = 0;
	int n_preset_tunes = 0;
	int found = 0;
	int tune_number = 0;
	FILE *f_out;
	TUNE *tune_data;
	TUNE new_tune;

	char name[12];
	char tune_names[N_TUNE_NAMES][12];
	char buf[sizeof(path_home)+150];

	error_count = 0;
	f_errors = log;

	sprintf(buf, "%s/../phsource/intonation.txt", path_home);
	if ((f_in = fopen(buf, "r")) == NULL) {
		sprintf(buf, "%s/../phsource/intonation", path_home);
		if ((f_in = fopen(buf, "r")) == NULL) {
			int error = errno;
			fclose(f_errors);
			return create_file_error_context(context, static_cast<espeak_ng_STATUS> (error), buf);
		}
	}

	for (ix = 0; preset_tune_names[ix] != NULL; ix++)
		strcpy(tune_names[ix], preset_tune_names[ix]);
	n_tune_names = ix;
	n_preset_tunes = ix;

	// make a list of the tune names
	while (!feof(f_in)) {
		if (fgets(buf, sizeof(buf), f_in) == NULL)
			break;

		if ((memcmp(buf, "tune", 4) == 0) && isspace(buf[4])) {
			p = &buf[5];
			while (isspace(*p)) p++;

			ix = 0;
			while ((ix < (int)(sizeof(name) - 1)) && !isspace(*p))
				name[ix++] = *p++;
			name[ix] = 0;

			found = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(name, tune_names[ix]) == 0) {
					found = 1;
					break;
				}
			}

			if (found == 0) {
				strncpy0(tune_names[n_tune_names++], name, sizeof(name));

				if (n_tune_names >= N_TUNE_NAMES)
					break;
			}
		}
	}
	rewind(f_in);
	linenum = 1;

	tune_data = (n_tune_names == 0) ? NULL : (TUNE *)calloc(n_tune_names, sizeof(TUNE));
	if (tune_data == NULL) {
		fclose(f_in);
		fclose(f_errors);
		return static_cast<espeak_ng_STATUS> (ENOMEM);
	}

	sprintf(buf, "%s/intonations", path_home);
	f_out = fopen(buf, "wb");
	if (f_out == NULL) {
		int error = errno;
		fclose(f_in);
		fclose(f_errors);
		free(tune_data);
		return create_file_error_context(context, static_cast<espeak_ng_STATUS> (error), buf);
	}

	while (!feof(f_in)) {
		keyword = NextItem(tINTONATION);

		switch (keyword)
		{
		case kTUNE:
			done_split = 0;

			memcpy(&new_tune, &default_tune, sizeof(TUNE));
			NextItem(tSTRING);
			strncpy0(new_tune.name, item_string, sizeof(new_tune.name));

			found = 0;
			tune_number = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(new_tune.name, tune_names[ix]) == 0) {
					found = 1;
					tune_number = ix;

					if (tune_data[ix].name[0] != 0)
						found = 2;
					break;
				}
			}
			if (found == 2)
				error("Duplicate tune name: '%s'", new_tune.name);
			if (found == 0)
				error("Bad tune name: '%s;", new_tune.name);
			break;
		case kENDTUNE:
			if (!found) continue;
			if (done_onset == 0) {
				new_tune.unstr_start[0] = new_tune.unstr_start[1];
				new_tune.unstr_end[0] = new_tune.unstr_end[1];
			}
			if (done_last == 0) {
				new_tune.unstr_start[2] = new_tune.unstr_start[1];
				new_tune.unstr_end[2] = new_tune.unstr_end[1];
			}
			memcpy(&tune_data[tune_number], &new_tune, sizeof(TUNE));
			break;
		case kTUNE_PREHEAD:
			new_tune.prehead_start = NextItem(tNUMBER);
			new_tune.prehead_end = NextItem(tNUMBER);
			break;
		case kTUNE_ONSET:
			new_tune.onset = NextItem(tNUMBER);
			new_tune.unstr_start[0] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[0] = NextItem(tSIGNEDNUMBER);
			done_onset = 1;
			break;
		case kTUNE_HEADLAST:
			new_tune.head_last = NextItem(tNUMBER);
			new_tune.unstr_start[2] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[2] = NextItem(tSIGNEDNUMBER);
			done_last = 1;
			break;
		case kTUNE_HEADENV:
			NextItem(tSTRING);
			if ((ix = LookupEnvelopeName(item_string)) < 0)
				error("Bad envelope name: '%s'", item_string);
			else
				new_tune.stressed_env = ix;
			new_tune.stressed_drop = NextItem(tNUMBER);
			break;
		case kTUNE_HEAD:
			new_tune.head_max_steps = NextItem(tNUMBER);
			new_tune.head_start = NextItem(tNUMBER);
			new_tune.head_end = NextItem(tNUMBER);
			new_tune.unstr_start[1] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[1] = NextItem(tSIGNEDNUMBER);
			break;
		case kTUNE_HEADEXTEND:
			// up to 8 numbers
			for (ix = 0; ix < (int)(sizeof(new_tune.head_extend)); ix++) {
				if (!isdigit(c = CheckNextChar()) && (c != '-'))
					break;

				new_tune.head_extend[ix] = (NextItem(tSIGNEDNUMBER) * 64) / 100; // convert from percentage to 64ths
			}
			new_tune.n_head_extend = ix; // number of values
			break;
		case kTUNE_NUCLEUS0:
			NextItem(tSTRING);
			if ((ix = LookupEnvelopeName(item_string)) < 0) {
				error("Bad envelope name: '%s'", item_string);
				break;
			}
			new_tune.nucleus0_env = ix;
			new_tune.nucleus0_max = NextItem(tNUMBER);
			new_tune.nucleus0_min = NextItem(tNUMBER);
			break;
		case kTUNE_NUCLEUS1:
			NextItem(tSTRING);
			if ((ix = LookupEnvelopeName(item_string)) < 0) {
				error("Bad envelope name: '%s'", item_string);
				break;
			}
			new_tune.nucleus1_env = ix;
			new_tune.nucleus1_max = NextItem(tNUMBER);
			new_tune.nucleus1_min = NextItem(tNUMBER);
			new_tune.tail_start = NextItem(tNUMBER);
			new_tune.tail_end = NextItem(tNUMBER);

			if (!done_split) {
				// also this as the default setting for 'split'
				new_tune.split_nucleus_env = ix;
				new_tune.split_nucleus_max = new_tune.nucleus1_max;
				new_tune.split_nucleus_min = new_tune.nucleus1_min;
				new_tune.split_tail_start = new_tune.tail_start;
				new_tune.split_tail_end = new_tune.tail_end;
			}
			break;
		case kTUNE_SPLIT:
			NextItem(tSTRING);
			if ((ix = LookupEnvelopeName(item_string)) < 0) {
				error("Bad envelope name: '%s'", item_string);
				break;
			}
			done_split = 1;
			new_tune.split_nucleus_env = ix;
			new_tune.split_nucleus_max = NextItem(tNUMBER);
			new_tune.split_nucleus_min = NextItem(tNUMBER);
			new_tune.split_tail_start = NextItem(tNUMBER);
			new_tune.split_tail_end = NextItem(tNUMBER);
			NextItem(tSTRING);
			item_string[12] = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(item_string, tune_names[ix]) == 0)
					break;
			}

			if (ix == n_tune_names)
				error("Tune '%s' not found", item_string);
			else
				new_tune.split_tune = ix;
			break;
		default:
			error("Unexpected: '%s'", item_string);
			break;
		}
	}

	for (ix = 0; ix < n_preset_tunes; ix++) {
		if (tune_data[ix].name[0] == 0)
			error("Tune '%s' not defined", preset_tune_names[ix]);
	}
	fwrite(tune_data, n_tune_names, sizeof(TUNE), f_out);
	free(tune_data);
	fclose(f_in);
	fclose(f_out);

	fprintf(log, "Compiled %d intonation tunes: %d errors.\n", n_tune_names, error_count);

	LoadPhData(NULL, NULL);

	return error_count > 0 ? ENS_COMPILE_ERROR : ENS_OK;
}

#pragma GCC visibility pop
