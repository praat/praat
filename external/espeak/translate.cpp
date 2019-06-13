/*
 * Copyright (C) 2005 to 2014 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2017 Reece H. Dunn
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
//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "ucd.h"
#include "espeak_ng.h"
#include "encoding.h"
#include "speech.h"
#include "synthesize.h"
#include "translate.h"

Translator *translator = NULL; // the main translator
Translator *translator2 = NULL; // secondary translator for certain words
static char translator2_language[20] = { 0 };

FILE *f_trans = NULL; // phoneme output text
int option_tone2 = 0;
int option_tone_flags = 0; // bit 8=emphasize allcaps, bit 9=emphasize penultimate stress
int option_phonemes = 0;
int option_phoneme_events = 0;
int option_endpause = 0; // suppress pause after end of text
int option_capitals = 0;
int option_punctuation = 0;
int option_sayas = 0;
static int option_sayas2 = 0; // used in translate_clause()
static int option_emphasis = 0; // 0=normal, 1=normal, 2=weak, 3=moderate, 4=strong
int option_ssml = 0;
int option_phoneme_input = 0; // allow [[phonemes]] in input
int option_phoneme_variants = 0; // 0= don't display phoneme variant mnemonics
int option_wordgap = 0;

static int count_sayas_digits;
int skip_sentences;
int skip_words;
int skip_characters;
char skip_marker[N_MARKER_LENGTH];
int skipping_text; // waiting until word count, sentence count, or named marker is reached
int end_character_position;
int count_sentences;
int count_words;
int clause_start_char;
int clause_start_word;
int new_sentence;
static int word_emphasis = 0; // set if emphasis level 3 or 4
static int embedded_flag = 0; // there are embedded commands to be applied to the next phoneme, used in TranslateWord2()

static int prev_clause_pause = 0;
static int max_clause_pause = 0;
static int any_stressed_words;
int pre_pause;
ALPHABET *current_alphabet;

// these were previously in translator class
char word_phonemes[N_WORD_PHONEMES]; // a word translated into phoneme codes
int n_ph_list2;
PHONEME_LIST2 ph_list2[N_PHONEME_LIST]; // first stage of text->phonemes

wchar_t option_punctlist[N_PUNCTLIST] = { 0 };
char ctrl_embedded = '\001'; // to allow an alternative CTRL for embedded commands

// these are overridden by defaults set in the "speak" file
int option_linelength = 0;

#define N_EMBEDDED_LIST  250
static int embedded_ix;
static int embedded_read;
unsigned int embedded_list[N_EMBEDDED_LIST];

// the source text of a single clause (UTF8 bytes)
static char source[N_TR_SOURCE+40]; // extra space for embedded command & voice change info at end

int n_replace_phonemes;
REPLACE_PHONEMES replace_phonemes[N_REPLACE_PHONEMES];

// brackets, also 0x2014 to 0x021f which don't need to be in this list
static const unsigned short brackets[] = {
	'(', ')', '[', ']', '{', '}', '<', '>', '"', '\'', '`',
	0xab,   0xbb,   // double angle brackets
	0x300a, 0x300b, // double angle brackets (ideograph)
	0xe000+'<',     // private usage area
	0
};

// other characters which break a word, but don't produce a pause
static const unsigned short breaks[] = { '_', 0 };

// Tables of the relative lengths of vowels, depending on the
// type of the two phonemes that follow
// indexes are the "length_mod" value for the following phonemes

// use this table if vowel is not the last in the word
static unsigned char length_mods_en[100] = {
//	a    ,    t    s    n    d    z    r    N    <- next
	100, 120, 100, 105, 100, 110, 110, 100,  95, 100, // a  <- next2
	105, 120, 105, 110, 125, 130, 135, 115, 125, 100, // ,
	105, 120,  75, 100,  75, 105, 120,  85,  75, 100, // t
	105, 120,  85, 105,  95, 115, 120, 100,  95, 100, // s
	110, 120,  95, 105, 100, 115, 120, 100, 100, 100, // n
	105, 120, 100, 105,  95, 115, 120, 110,  95, 100, // d
	105, 120, 100, 105, 105, 122, 125, 110, 105, 100, // z
	105, 120, 100, 105, 105, 122, 125, 110, 105, 100, // r
	105, 120,  95, 105, 100, 115, 120, 110, 100, 100, // N
	100, 120, 100, 100, 100, 100, 100, 100, 100, 100
};

// as above, but for the last syllable in a word
static unsigned char length_mods_en0[100] = {
//	a    ,    t    s    n    d    z    r    N    <- next
	100, 150, 100, 105, 110, 115, 110, 110, 110, 100, // a  <- next2
	105, 150, 105, 110, 125, 135, 140, 115, 135, 100, // ,
	105, 150,  90, 105,  90, 122, 135, 100,  90, 100, // t
	105, 150, 100, 105, 100, 122, 135, 100, 100, 100, // s
	105, 150, 100, 105, 105, 115, 135, 110, 105, 100, // n
	105, 150, 100, 105, 105, 122, 130, 120, 125, 100, // d
	105, 150, 100, 105, 110, 122, 125, 115, 110, 100, // z
	105, 150, 100, 105, 105, 122, 135, 120, 105, 100, // r
	105, 150, 100, 105, 105, 115, 135, 110, 105, 100, // N
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100
};


static unsigned char length_mods_equal[100] = {
//	a    ,    t    s    n    d    z    r    N    <- next
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // a  <- next2
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // ,
	110, 120, 100, 110, 100, 110, 110, 110, 100, 110, // t
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // s
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // n
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // d
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // z
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // r
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110, // N
	110, 120, 100, 110, 110, 110, 110, 110, 110, 110
};

static unsigned char *length_mod_tabs[6] = {
	length_mods_en,
	length_mods_en,    // 1
	length_mods_en0,   // 2
	length_mods_equal, // 3
	length_mods_equal, // 4
	length_mods_equal  // 5
};

void SetLengthMods(Translator *tr, int value)
{
	int value2;

	tr->langopts.length_mods0 = tr->langopts.length_mods = length_mod_tabs[value % 100];
	if ((value2 = value / 100) != 0)
		tr->langopts.length_mods0 = length_mod_tabs[value2];
}

int IsAlpha(unsigned int c)
{
	// Replacement for iswalph() which also checks for some in-word symbols

	static const unsigned short extra_indic_alphas[] = {
		0xa70, 0xa71, // Gurmukhi: tippi, addak
		0
	};

	if (iswalpha(c))
		return 1;

	if (c < 0x300)
		return 0;

	if ((c >= 0x901) && (c <= 0xdf7)) {
		// Indic scripts: Devanagari, Tamil, etc
		if ((c & 0x7f) < 0x64)
			return 1;
		if (lookupwchar(extra_indic_alphas, c) != 0)
			return 1;
		if ((c >= 0xd7a) && (c <= 0xd7f))
			return 1; // malaytalam chillu characters

		return 0;
	}

	if ((c >= 0x5b0) && (c <= 0x5c2))
		return 1; // Hebrew vowel marks

	if (c == 0x0605)
		return 1;

	if ((c == 0x670) || ((c >= 0x64b) && (c <= 0x65e)))
		return 1; // arabic vowel marks

	if ((c >= 0x300) && (c <= 0x36f))
		return 1; // combining accents

	if ((c >= 0x780) && (c <= 0x7b1))
		return 1; // taani/divehi (maldives)

	if ((c >= 0xf40) && (c <= 0xfbc))
		return 1; // tibetan

	if ((c >= 0x1100) && (c <= 0x11ff))
		return 1; // Korean jamo

	if ((c >= 0x2800) && (c <= 0x28ff))
		return 1; // braille

	if ((c > 0x3040) && (c <= 0xa700))
		return 1; // Chinese/Japanese.  Should never get here, but Mac OS 10.4's iswalpha seems to be broken, so just make sure

	return 0;
}

int IsDigit09(unsigned int c)
{
	if ((c >= '0') && (c <= '9'))
		return 1;
	return 0;
}

int IsDigit(unsigned int c)
{
	if (iswdigit(c))
		return 1;

	if ((c >= 0x966) && (c <= 0x96f))
		return 1;

	return 0;
}

static int IsSpace(unsigned int c)
{
	if (c == 0)
		return 0;
	if ((c >= 0x2500) && (c < 0x25a0))
		return 1; // box drawing characters
	if ((c >= 0xfff9) && (c <= 0xffff))
		return 1; // unicode specials
	return iswspace(c);
}

int isspace2(unsigned int c)
{
	// can't use isspace() because on Windows, isspace(0xe1) gives TRUE !
	int c2;

	if (((c2 = (c & 0xff)) == 0) || (c > ' '))
		return 0;
	return 1;
}

void DeleteTranslator(Translator *tr)
{
	if (tr->data_dictlist != NULL)
		free(tr->data_dictlist);
	free(tr);
}

int lookupwchar(const unsigned short *list, int c)
{
	// Is the character c in the list ?
	int ix;

	for (ix = 0; list[ix] != 0; ix++) {
		if (list[ix] == c)
			return ix+1;
	}
	return 0;
}

int lookupwchar2(const unsigned short *list, int c)
{
	// Replace character c by another character.
	// Returns 0 = not found, 1 = delete character

	int ix;

	for (ix = 0; list[ix] != 0; ix += 2) {
		if (list[ix] == c)
			return list[ix+1];
	}
	return 0;
}

int IsBracket(int c)
{
	if ((c >= 0x2014) && (c <= 0x201f))
		return 1;
	return lookupwchar(brackets, c);
}

int utf8_nbytes(const char *buf)
{
	// Returns the number of bytes for the first UTF-8 character in buf

	unsigned char c = (unsigned char)buf[0];
	if (c < 0x80)
		return 1;
	if (c < 0xe0)
		return 2;
	if (c < 0xf0)
		return 3;
	return 4;
}

int utf8_in2(int *c, const char *buf, int backwards)
{
	// Reads a unicode characater from a UTF8 string
	// Returns the number of UTF8 bytes used.
	// c: holds integer representation of multibyte character
	// buf: position of buffer is moved, if character is read
	// backwards: set if we are moving backwards through the UTF8 string

	int c1;
	int n_bytes;
	int ix;
	static const unsigned char mask[4] = { 0xff, 0x1f, 0x0f, 0x07 };

	// find the start of the next/previous character
	while ((*buf & 0xc0) == 0x80) {
		// skip over non-initial bytes of a multi-byte utf8 character
		if (backwards)
			buf--;
		else
			buf++;
	}

	n_bytes = 0;

	if ((c1 = *buf++) & 0x80) {
		if ((c1 & 0xe0) == 0xc0)
			n_bytes = 1;
		else if ((c1 & 0xf0) == 0xe0)
			n_bytes = 2;
		else if ((c1 & 0xf8) == 0xf0)
			n_bytes = 3;

		c1 &= mask[n_bytes];
		for (ix = 0; ix < n_bytes; ix++)
			c1 = (c1 << 6) + (*buf++ & 0x3f);
	}
	*c = c1;
	return n_bytes+1;
}

#pragma GCC visibility push(default)
int utf8_in(int *c, const char *buf)
{
	/* Read a unicode characater from a UTF8 string
	 * Returns the number of UTF8 bytes used.
	 * buf: position of buffer is moved, if character is read
	 * c: holds integer representation of multibyte character by
	 * skipping UTF-8 header bits of bytes in following way:
	 * 2-byte character "ā":
	 * hex            binary
	 * c481           1100010010000001
	 *    |           11000100  000001
	 *    V              \    \ |    |
	 * 0101           0000000100000001
	 * 3-byte character "ꙅ":
	 * ea9985 111010101001100110000101
	 *            1010  011001  000101
	 *    |       +  +--.\   \  |    |
	 *    V        `--.  \`.  `.|    |
	 *   A645         0001001101000101
	 * 4-byte character "𠜎":
	 * f0a09c8e 11110000101000001001110010001110
	 *    V          000  100000  011100  001110
	 *   02070e         000000100000011100001110
	 */
	return utf8_in2(c, buf, 0);
}
#pragma GCC visibility pop

int utf8_out(unsigned int c, char *buf)
{
	// write a unicode character into a buffer as utf8
	// returns the number of bytes written

	int n_bytes;
	int j;
	int shift;
	static char unsigned code[4] = { 0, 0xc0, 0xe0, 0xf0 };

	if (c < 0x80) {
		buf[0] = c;
		return 1;
	}
	if (c >= 0x110000) {
		buf[0] = ' '; // out of range character code
		return 1;
	}
	if (c < 0x0800)
		n_bytes = 1;
	else if (c < 0x10000)
		n_bytes = 2;
	else
		n_bytes = 3;

	shift = 6*n_bytes;
	buf[0] = code[n_bytes] | (c >> shift);
	for (j = 0; j < n_bytes; j++) {
		shift -= 6;
		buf[j+1] = 0x80 + ((c >> shift) & 0x3f);
	}
	return n_bytes+1;
}

char *strchr_w(const char *s, int c)
{
	// return NULL for any non-ascii character
	if (c >= 0x80)
		return NULL;
	return strchr((char *)s, c); // (char *) is needed for Borland compiler
}
static char *SpeakIndividualLetters(Translator *tr, char *word, char *phonemes, int spell_word)
{
	int posn = 0;
	int capitals = 0;
	int non_initial = 0;

	if (spell_word > 2)
		capitals = 2; // speak 'capital'
	if (spell_word > 1)
		capitals |= 4; // speak charater code for unknown letters

	while ((*word != ' ') && (*word != 0)) {
		word += TranslateLetter(tr, word, phonemes, capitals | non_initial);
		posn++;
		non_initial = 1;
		if (phonemes[0] == phonSWITCH) {
			// change to another language in order to translate this word
			strcpy(word_phonemes, phonemes);
			return NULL;
		}
	}
	SetSpellingStress(tr, phonemes, spell_word, posn);
	return word;
}

static int CheckDottedAbbrev(char *word1)
{
	int wc;
	int count = 0;
	int nbytes;
	int ok;
	int ix;
	char *word;
	char *wbuf;
	char word_buf[80];

	word = word1;
	wbuf = word_buf;

	for (;;) {
		ok = 0;
		nbytes = utf8_in(&wc, word);
		if ((word[nbytes] == ' ') && IsAlpha(wc)) {
			if (word[nbytes+1] == '.') {
				if (word[nbytes+2] == ' ')
					ok = 1;
				else if (word[nbytes+2] == '\'') {
					nbytes += 2; // delete the final dot (eg. u.s.a.'s)
					ok = 2;
				}
			} else if ((count > 0) && (word[nbytes] == ' '))
				ok = 2;
		}

		if (ok == 0)
			break;

		for (ix = 0; ix < nbytes; ix++)
			*wbuf++ = word[ix];

		count++;

		if (ok == 2) {
			word += nbytes;
			break;
		}

		word += (nbytes + 3);
	}

	if (count > 1) {
		ix = wbuf - word_buf;
		memcpy(word1, word_buf, ix);
		while (&word1[ix] < word)
			word1[ix++] = ' ';
		dictionary_skipwords = (count - 1)*2;
	}
	return count;
}

extern char *phondata_ptr;

static int TranslateWord3(Translator *tr, char *word_start, WORD_TAB *wtab, char *word_out)
{
	// word1 is terminated by space (0x20) character

	char *word1;
	int word_length;
	int ix;
	char *p;
	int pfix;
	int n_chars;
	unsigned int dictionary_flags[2];
	unsigned int dictionary_flags2[2];
	int end_type = 0;
	int end_type1 = 0;
	int prefix_type = 0;
	int prefix_stress;
	char *wordx;
	char phonemes[N_WORD_PHONEMES];
	char phonemes2[N_WORD_PHONEMES];
	char prefix_phonemes[N_WORD_PHONEMES];
	char unpron_phonemes[N_WORD_PHONEMES];
	char end_phonemes[N_WORD_PHONEMES];
	char end_phonemes2[N_WORD_PHONEMES];
	char word_copy[N_WORD_BYTES];
	char word_copy2[N_WORD_BYTES];
	int word_copy_length;
	char prefix_chars[0x3f + 2];
	bool found = false;
	int end_flags;
	int c_temp; // save a character byte while we temporarily replace it with space
	int first_char;
	int last_char = 0;
	int prefix_flags = 0;
	int more_suffixes;
	int confirm_prefix;
	int spell_word;
	int emphasize_allcaps = 0;
	int wflags;
	int wmark;
	int was_unpronouncable = 0;
	int loopcount;
	int add_suffix_phonemes = 0;
	WORD_TAB wtab_null[8];

	// translate these to get pronunciations of plural 's' suffix (different forms depending on
	// the preceding letter
	static char word_zz[4] = { 0, 'z', 'z', 0 };
	static char word_iz[4] = { 0, 'i', 'z', 0 };
	static char word_ss[4] = { 0, 's', 's', 0 };

	if (wtab == NULL) {
		memset(wtab_null, 0, sizeof(wtab_null));
		wtab = wtab_null;
	}
	wflags = wtab->flags;
	wmark = wtab->wmark;

	dictionary_flags[0] = 0;
	dictionary_flags[1] = 0;
	dictionary_flags2[0] = 0;
	dictionary_flags2[1] = 0;
	dictionary_skipwords = 0;

	phonemes[0] = 0;
	unpron_phonemes[0] = 0;
	prefix_phonemes[0] = 0;
	end_phonemes[0] = 0;

	if (tr->data_dictlist == NULL) {
		// dictionary is not loaded
		word_phonemes[0] = 0;
		return 0;
	}

	// count the length of the word
	word1 = word_start;
	if (*word1 == ' ') word1++; // possibly a dot was replaced by space:  $dot
	wordx = word1;

	utf8_in(&first_char, wordx);
	word_length = 0;
	while ((*wordx != 0) && (*wordx != ' ')) {
		wordx += utf8_in(&last_char, wordx);
		word_length++;
	}

	word_copy_length = wordx - word_start;
	if (word_copy_length >= N_WORD_BYTES)
		word_copy_length = N_WORD_BYTES-1;
	memcpy(word_copy2, word_start, word_copy_length);

	spell_word = 0;

	if ((word_length == 1) && (wflags & FLAG_TRANSLATOR2)) {
		// retranslating a 1-character word using a different language, say its name
		utf8_in(&c_temp, wordx+1); // the next character
		if (!IsAlpha(c_temp) || (AlphabetFromChar(last_char) != AlphabetFromChar(c_temp)))
			spell_word = 1;
	}

	if (option_sayas == SAYAS_KEY) {
		if (word_length == 1)
			spell_word = 4;
		else {
			// is there a translation for this keyname ?
			word1--;
			*word1 = '_'; // prefix keyname with '_'
			found = LookupDictList(tr, &word1, phonemes, dictionary_flags, 0, wtab);
		}
	}

	// try an initial lookup in the dictionary list, we may find a pronunciation specified, or
	// we may just find some flags
	if (option_sayas & 0x10) {
		// SAYAS_CHAR, SAYAS_GYLPH, or SAYAS_SINGLE_CHAR
		spell_word = option_sayas & 0xf; // 2,3,4
	} else {
		if (!found)
			found = LookupDictList(tr, &word1, phonemes, dictionary_flags, FLAG_ALLOW_TEXTMODE, wtab);   // the original word

		if ((dictionary_flags[0] & (FLAG_ALLOW_DOT | FLAG_NEEDS_DOT)) && (wordx[1] == '.'))
			wordx[1] = ' '; // remove a Dot after this word

		if (dictionary_flags[0] & FLAG_TEXTMODE) {
			if (word_out != NULL)
				strcpy(word_out, word1);

			return dictionary_flags[0];
		} else if ((found == false) && (dictionary_flags[0] & FLAG_SKIPWORDS) && !(dictionary_flags[0] & FLAG_ABBREV)) {
			// grouped words, but no translation.  Join the words with hyphens.
			wordx = word1;
			ix = 0;
			while (ix < dictionary_skipwords) {
				if (*wordx == ' ') {
					*wordx = '-';
					ix++;
				}
				wordx++;
			}
		}

		if ((word_length == 1) && (dictionary_skipwords == 0)) {
			// is this a series of single letters separated by dots?
			if (CheckDottedAbbrev(word1)) {
				dictionary_flags[0] = 0;
				dictionary_flags[1] = 0;
				spell_word = 1;
				if (dictionary_skipwords)
					dictionary_flags[0] = FLAG_SKIPWORDS;
			}
		}

		if (phonemes[0] == phonSWITCH) {
			// change to another language in order to translate this word
			strcpy(word_phonemes, phonemes);
			return 0;
		}

		if ((wmark > 0) && (wmark < 8)) {
			// the stressed syllable has been specified in the text  (TESTING)
			dictionary_flags[0] = (dictionary_flags[0] & ~0xf) | wmark;
		}

		if (!found && (dictionary_flags[0] & FLAG_ABBREV)) {
			// the word has $abbrev flag, but no pronunciation specified.  Speak as individual letters
			spell_word = 1;
		}

		if (!found && iswdigit(first_char)) {
			Lookup(tr, "_0lang", word_phonemes);
			if (word_phonemes[0] == phonSWITCH)
				return 0;

			if ((tr->langopts.numbers2 & NUM2_ENGLISH_NUMERALS) && !(wtab->flags & FLAG_CHAR_REPLACED)) {
				// for this language, speak English numerals (0-9) with the English voice
				sprintf(word_phonemes, "%c", phonSWITCH);
				return 0;
			}

			found = TranslateNumber(tr, word1, phonemes, dictionary_flags, wtab, 0);
		}

		if (!found && ((wflags & FLAG_UPPERS) != FLAG_FIRST_UPPER)) {
			// either all upper or all lower case

			if ((tr->langopts.numbers & NUM_ROMAN) || ((tr->langopts.numbers & NUM_ROMAN_CAPITALS) && (wflags & FLAG_ALL_UPPER))) {
				if ((wflags & FLAG_LAST_WORD) || !(wtab[1].flags & FLAG_NOSPACE)) {
					// don't use Roman number if this word is not separated from the next word (eg. "XLTest")
					if ((found = TranslateRoman(tr, word1, phonemes, wtab)) != 0)
						dictionary_flags[0] |= FLAG_ABBREV; // prevent emphasis if capitals
				}
			}
		}

		if ((wflags & FLAG_ALL_UPPER) && (word_length > 1) && iswalpha(first_char)) {
			if ((option_tone_flags & OPTION_EMPHASIZE_ALLCAPS) && !(dictionary_flags[0] & FLAG_ABBREV)) {
				// emphasize words which are in capitals
				emphasize_allcaps = FLAG_EMPHASIZED;
			} else if (!found && !(dictionary_flags[0] &  FLAG_SKIPWORDS) && (word_length < 4) && (tr->clause_lower_count > 3)
			           && (tr->clause_upper_count <= tr->clause_lower_count)) {
				// An upper case word in a lower case clause. This could be an abbreviation.
				spell_word = 1;
			}
		}
	}

	if (spell_word > 0) {
		// Speak as individual letters
		phonemes[0] = 0;

		if (SpeakIndividualLetters(tr, word1, phonemes, spell_word) == NULL) {
			if (word_length > 1)
				return FLAG_SPELLWORD; // a mixture of languages, retranslate as individual letters, separated by spaces
			return 0;
		}
		strcpy(word_phonemes, phonemes);
		if (wflags & FLAG_TRANSLATOR2)
			return 0;
		return dictionary_flags[0] & FLAG_SKIPWORDS; // for "b.c.d"
	} else if (found == false) {
		// word's pronunciation is not given in the dictionary list, although
		// dictionary_flags may have ben set there

		int posn;
		int non_initial;
		int length;

		posn = 0;
		non_initial = 0;
		length = 999;
		wordx = word1;

		while (((length < 3) && (length > 0)) || (word_length > 1 && Unpronouncable(tr, wordx, posn))) {
			// This word looks "unpronouncable", so speak letters individually until we
			// find a remainder that we can pronounce.
			was_unpronouncable = FLAG_WAS_UNPRONOUNCABLE;
			emphasize_allcaps = 0;

			if (wordx[0] == '\'')
				break;

			if (posn > 0)
				non_initial = 1;

			wordx += TranslateLetter(tr, wordx, unpron_phonemes, non_initial);
			posn++;
			if (unpron_phonemes[0] == phonSWITCH) {
				// change to another language in order to translate this word
				strcpy(word_phonemes, unpron_phonemes);
				if (strcmp(&unpron_phonemes[1], "en") == 0)
					return FLAG_SPELLWORD; // _^_en must have been set in TranslateLetter(), not *_rules which uses only _^_
				return 0;
			}

			length = 0;
			while (wordx[length] != ' ') length++;
		}
		SetSpellingStress(tr, unpron_phonemes, 0, posn);

		// anything left ?
		if (*wordx != ' ') {
			if ((unpron_phonemes[0] != 0) && (wordx[0] != '\'')) {
				// letters which have been spoken individually from affecting the pronunciation of the pronuncable part
				wordx[-1] = ' ';
			}

			// Translate the stem
			end_type = TranslateRules(tr, wordx, phonemes, N_WORD_PHONEMES, end_phonemes, wflags, dictionary_flags);

			if (phonemes[0] == phonSWITCH) {
				// change to another language in order to translate this word
				strcpy(word_phonemes, phonemes);
				return 0;
			}

			if ((phonemes[0] == 0) && (end_phonemes[0] == 0)) {
				int wc;
				// characters not recognised, speak them individually
				// ?? should we say super/sub-script numbers and letters here?
				utf8_in(&wc, wordx);
				if ((word_length == 1) && (IsAlpha(wc) || IsSuperscript(wc))) {
					if ((wordx = SpeakIndividualLetters(tr, wordx, phonemes, spell_word)) == NULL)
						return 0;
					strcpy(word_phonemes, phonemes);
					return 0;
				}
			}

			c_temp = wordx[-1];

			found = false;
			confirm_prefix = 1;
			for (loopcount = 0; (loopcount < 50) && (end_type & SUFX_P); loopcount++) {
				// Found a standard prefix, remove it and retranslate
				// loopcount guards against an endless loop
				if (confirm_prefix && !(end_type & SUFX_B)) {
					int end2;
					char end_phonemes22[N_WORD_PHONEMES];

					// remove any standard suffix and confirm that the prefix is still recognised
					phonemes2[0] = 0;
					end2 = TranslateRules(tr, wordx, phonemes2, N_WORD_PHONEMES, end_phonemes22, wflags|FLAG_NO_PREFIX|FLAG_NO_TRACE, dictionary_flags);
					if (end2) {
						RemoveEnding(tr, wordx, end2, word_copy);
						end_type = TranslateRules(tr, wordx, phonemes, N_WORD_PHONEMES, end_phonemes, wflags|FLAG_NO_TRACE, dictionary_flags);
						memcpy(wordx, word_copy, strlen(word_copy));
						if ((end_type & SUFX_P) == 0) {
							// after removing the suffix, the prefix is no longer recognised.
							// Keep the suffix, but don't use the prefix
							end_type = end2;
							strcpy(phonemes, phonemes2);
							strcpy(end_phonemes, end_phonemes22);
							if (option_phonemes & espeakPHONEMES_TRACE) {
								DecodePhonemes(end_phonemes, end_phonemes22);
								fprintf(f_trans, "  suffix [%s]\n\n", end_phonemes22);
							}
						}
						confirm_prefix = 0;
						continue;
					}
				}

				prefix_type = end_type;

				if (prefix_type & SUFX_V)
					tr->expect_verb = 1; // use the verb form of the word

				wordx[-1] = c_temp;

				if ((prefix_type & SUFX_B) == 0) {
					for (ix = (prefix_type & 0xf); ix > 0; ix--) { // num. of characters to remove
						wordx++;
						while ((*wordx & 0xc0) == 0x80) wordx++; // for multibyte characters
					}
				} else {
					pfix = 1;
					prefix_chars[0] = 0;
					n_chars = prefix_type & 0x3f;

					for (ix = 0; ix < n_chars; ix++) { // num. of bytes to remove
						prefix_chars[pfix++] = *wordx++;

						if ((prefix_type & SUFX_B) && (ix == (n_chars-1)))
							prefix_chars[pfix-1] = 0; // discard the last character of the prefix, this is the separator character
					}
					prefix_chars[pfix] = 0;
				}
				c_temp = wordx[-1];
				wordx[-1] = ' ';
				confirm_prefix = 1;
				wflags |= FLAG_PREFIX_REMOVED;

				if (prefix_type & SUFX_B) {
					// SUFX_B is used for Turkish, tr_rules contains " ' (Pb"
					// examine the prefix part
					char *wordpf;
					char prefix_phonemes2[12];

					strncpy0(prefix_phonemes2, end_phonemes, sizeof(prefix_phonemes2));
					wordpf = &prefix_chars[1];
					strcpy(prefix_phonemes, phonemes);

					// look for stress marker or $abbrev
					found = LookupDictList(tr, &wordpf, phonemes, dictionary_flags, 0, wtab);
					if (found)
						strcpy(prefix_phonemes, phonemes);
					if (dictionary_flags[0] & FLAG_ABBREV) {
						prefix_phonemes[0] = 0;
						SpeakIndividualLetters(tr, wordpf, prefix_phonemes, 1);
					}
				} else
					strcat(prefix_phonemes, end_phonemes);
				end_phonemes[0] = 0;

				end_type = 0;
				found = LookupDictList(tr, &wordx, phonemes, dictionary_flags2, SUFX_P, wtab); // without prefix
				if (dictionary_flags[0] == 0) {
					dictionary_flags[0] = dictionary_flags2[0];
					dictionary_flags[1] = dictionary_flags2[1];
				} else
					prefix_flags = 1;
				if (found == false) {
					end_type = TranslateRules(tr, wordx, phonemes, N_WORD_PHONEMES, end_phonemes, wflags & (FLAG_HYPHEN_AFTER | FLAG_PREFIX_REMOVED), dictionary_flags);

					if (phonemes[0] == phonSWITCH) {
						// change to another language in order to translate this word
						wordx[-1] = c_temp;
						strcpy(word_phonemes, phonemes);
						return 0;
					}
				}
			}

			if ((end_type != 0) && !(end_type & SUFX_P)) {
				end_type1 = end_type;
				strcpy(phonemes2, phonemes);

				// The word has a standard ending, re-translate without this ending
				end_flags = RemoveEnding(tr, wordx, end_type, word_copy);
				more_suffixes = 1;

				while (more_suffixes) {
					more_suffixes = 0;
					phonemes[0] = 0;

					if (prefix_phonemes[0] != 0) {
						// lookup the stem without the prefix removed
						wordx[-1] = c_temp;
						found = LookupDictList(tr, &word1, phonemes, dictionary_flags2, end_flags, wtab);  // include prefix, but not suffix
						wordx[-1] = ' ';
						if (phonemes[0] == phonSWITCH) {
							// change to another language in order to translate this word
							memcpy(wordx, word_copy, strlen(word_copy));
							strcpy(word_phonemes, phonemes);
							return 0;
						}
						if (dictionary_flags[0] == 0) {
							dictionary_flags[0] = dictionary_flags2[0];
							dictionary_flags[1] = dictionary_flags2[1];
						}
						if (found)
							prefix_phonemes[0] = 0; // matched whole word, don't need prefix now

						if ((found == false) && (dictionary_flags2[0] != 0))
							prefix_flags = 1;
					}
					if (found == false) {
						found = LookupDictList(tr, &wordx, phonemes, dictionary_flags2, end_flags, wtab);  // without prefix and suffix
						if (phonemes[0] == phonSWITCH) {
							// change to another language in order to translate this word
							memcpy(wordx, word_copy, strlen(word_copy));
							strcpy(word_phonemes, phonemes);
							return 0;
						}

						if (dictionary_flags[0] == 0) {
							dictionary_flags[0] = dictionary_flags2[0];
							dictionary_flags[1] = dictionary_flags2[1];
						}
					}
					if (found == false) {
						if (end_type & SUFX_Q) {
							// don't retranslate, use the original lookup result
							strcpy(phonemes, phonemes2);
						} else {
							if (end_flags & FLAG_SUFX)
								wflags |= FLAG_SUFFIX_REMOVED;
							if (end_type & SUFX_A)
								wflags |= FLAG_SUFFIX_VOWEL;

							if (end_type & SUFX_M) {
								// allow more suffixes before this suffix
								strcpy(end_phonemes2, end_phonemes);
								end_type = TranslateRules(tr, wordx, phonemes, N_WORD_PHONEMES, end_phonemes, wflags, dictionary_flags);
								strcat(end_phonemes, end_phonemes2); // add the phonemes for the previous suffixes after this one

								if ((end_type != 0) && !(end_type & SUFX_P)) {
									// there is another suffix
									end_flags = RemoveEnding(tr, wordx, end_type, NULL);
									more_suffixes = 1;
								}
							} else {
								// don't remove any previous suffix
								TranslateRules(tr, wordx, phonemes, N_WORD_PHONEMES, NULL, wflags, dictionary_flags);
								end_type = 0;
							}

							if (phonemes[0] == phonSWITCH) {
								// change to another language in order to translate this word
								strcpy(word_phonemes, phonemes);
								memcpy(wordx, word_copy, strlen(word_copy));
								wordx[-1] = c_temp;
								return 0;
							}
						}
					}
				}


				if ((end_type1 & SUFX_T) == 0) {
					// the default is to add the suffix and then determine the word's stress pattern
					AppendPhonemes(tr, phonemes, N_WORD_PHONEMES, end_phonemes);
					end_phonemes[0] = 0;
				}
				memcpy(wordx, word_copy, strlen(word_copy));
			}

			wordx[-1] = c_temp;
		}
	}

	if (wflags & FLAG_HAS_PLURAL) {
		// s or 's suffix, append [s], [z] or [Iz] depending on previous letter
		if (last_char == 'f')
			TranslateRules(tr, &word_ss[1], phonemes, N_WORD_PHONEMES, NULL, 0, NULL);
		else if ((last_char == 0) || (strchr_w("hsx", last_char) == NULL))
			TranslateRules(tr, &word_zz[1], phonemes, N_WORD_PHONEMES, NULL, 0, NULL);
		else
			TranslateRules(tr, &word_iz[1], phonemes, N_WORD_PHONEMES, NULL, 0, NULL);
	}

	wflags |= emphasize_allcaps;

	// determine stress pattern for this word

	add_suffix_phonemes = 0;
	if (end_phonemes[0] != 0)
		add_suffix_phonemes = 2;

	prefix_stress = 0;
	for (p = prefix_phonemes; *p != 0; p++) {
		if ((*p == phonSTRESS_P) || (*p == phonSTRESS_P2))
			prefix_stress = *p;
	}
	if (prefix_flags || (prefix_stress != 0)) {
		if ((tr->langopts.param[LOPT_PREFIXES]) || (prefix_type & SUFX_T)) {
			char *p_local;
			// German, keep a secondary stress on the stem
			SetWordStress(tr, phonemes, dictionary_flags, 3, 0);

			// reduce all but the first primary stress
			ix = 0;
			for (p_local = prefix_phonemes; *p_local != 0; p_local++) {
				if (*p_local == phonSTRESS_P) {
					if (ix == 0)
						ix = 1;
					else
						*p_local = phonSTRESS_3;
				}
			}
			snprintf(word_phonemes, sizeof(word_phonemes), "%s%s%s", unpron_phonemes, prefix_phonemes, phonemes);
			word_phonemes[N_WORD_PHONEMES-1] = 0;
			SetWordStress(tr, word_phonemes, dictionary_flags, -1, 0);
		} else {
			// stress position affects the whole word, including prefix
			snprintf(word_phonemes, sizeof(word_phonemes), "%s%s%s", unpron_phonemes, prefix_phonemes, phonemes);
			word_phonemes[N_WORD_PHONEMES-1] = 0;
			SetWordStress(tr, word_phonemes, dictionary_flags, -1, 0);
		}
	} else {
		SetWordStress(tr, phonemes, dictionary_flags, -1, add_suffix_phonemes);
		snprintf(word_phonemes, sizeof(word_phonemes), "%s%s%s", unpron_phonemes, prefix_phonemes, phonemes);
		word_phonemes[N_WORD_PHONEMES-1] = 0;
	}

	if (end_phonemes[0] != 0) {
		// a suffix had the SUFX_T option set, add the suffix after the stress pattern has been determined
		ix = strlen(word_phonemes);
		end_phonemes[N_WORD_PHONEMES-1-ix] = 0; // ensure no buffer overflow
		strcpy(&word_phonemes[ix], end_phonemes);
	}

	if (wflags & FLAG_LAST_WORD) {
		// don't use $brk pause before the last word of a sentence
		// (but allow it for emphasis, see below
		dictionary_flags[0] &= ~FLAG_PAUSE1;
	}

	if ((wflags & FLAG_HYPHEN) && (tr->langopts.stress_flags & S_HYPEN_UNSTRESS))
		ChangeWordStress(tr, word_phonemes, 3);
	else if (wflags & FLAG_EMPHASIZED2) {
		// A word is indicated in the source text as stressed
		// Give it stress level 6 (for the intonation module)
		ChangeWordStress(tr, word_phonemes, 6);

		if (wflags & FLAG_EMPHASIZED)
			dictionary_flags[0] |= FLAG_PAUSE1; // precede by short pause
	} else if (wtab[dictionary_skipwords].flags & FLAG_LAST_WORD) {
		// the word has attribute to stress or unstress when at end of clause
		if (dictionary_flags[0] & (FLAG_STRESS_END | FLAG_STRESS_END2))
			ChangeWordStress(tr, word_phonemes, 4);
		else if ((dictionary_flags[0] & FLAG_UNSTRESS_END) && (any_stressed_words))
			ChangeWordStress(tr, word_phonemes, 3);
	}

	// dictionary flags for this word give a clue about which alternative pronunciations of
	// following words to use.
	if (end_type1 & SUFX_F) {
		// expect a verb form, with or without -s suffix
		tr->expect_verb = 2;
		tr->expect_verb_s = 2;
	}

	if (dictionary_flags[1] & FLAG_PASTF) {
		// expect perfect tense in next two words
		tr->expect_past = 3;
		tr->expect_verb = 0;
		tr->expect_noun = 0;
	} else if (dictionary_flags[1] & FLAG_VERBF) {
		// expect a verb in the next word
		tr->expect_verb = 2;
		tr->expect_verb_s = 0; // verb won't have -s suffix
		tr->expect_noun = 0;
	} else if (dictionary_flags[1] & FLAG_VERBSF) {
		// expect a verb, must have a -s suffix
		tr->expect_verb = 0;
		tr->expect_verb_s = 2;
		tr->expect_past = 0;
		tr->expect_noun = 0;
	} else if (dictionary_flags[1] & FLAG_NOUNF) {
		// not expecting a verb next
		tr->expect_noun = 2;
		tr->expect_verb = 0;
		tr->expect_verb_s = 0;
		tr->expect_past = 0;
	}

	if ((wordx[0] != 0) && (!(dictionary_flags[1] & FLAG_VERB_EXT))) {
		if (tr->expect_verb > 0)
			tr->expect_verb--;

		if (tr->expect_verb_s > 0)
			tr->expect_verb_s--;

		if (tr->expect_noun > 0)
			tr->expect_noun--;

		if (tr->expect_past > 0)
			tr->expect_past--;
	}

	if ((word_length == 1) && (tr->translator_name == L('e', 'n')) && iswalpha(first_char) && (first_char != 'i')) {
		// English Specific !!!!
		// any single letter before a dot is an abbreviation, except 'I'
		dictionary_flags[0] |= FLAG_ALLOW_DOT;
	}

	if ((tr->langopts.param[LOPT_ALT] & 2) && ((dictionary_flags[0] & (FLAG_ALT_TRANS | FLAG_ALT2_TRANS)) != 0))
		ApplySpecialAttribute2(tr, word_phonemes, dictionary_flags[0]);

	dictionary_flags[0] |= was_unpronouncable;
	memcpy(word_start, word_copy2, word_copy_length);
	return dictionary_flags[0];
}

int TranslateWord(Translator *tr, char *word_start, WORD_TAB *wtab, char *word_out)
{
	char words_phonemes[N_WORD_PHONEMES]; // a word translated into phoneme codes
	char *phonemes = words_phonemes;
	int available = N_WORD_PHONEMES;
	int first_word = 1;

	int flags = TranslateWord3(tr, word_start, wtab, word_out);
	if (flags & FLAG_TEXTMODE && word_out) {
		// Ensure that start of word rules match with the replaced text,
		// so that emoji and other characters are pronounced correctly.
		char word[N_WORD_BYTES+1];
		word[0] = 0;
		word[1] = ' ';
		strcpy(word+2, word_out);
		word_out = word+2;

		while (*word_out && available > 1) {
			int c;
			utf8_in(&c, word_out);
			if (iswupper(c)) {
				wtab->flags |= FLAG_FIRST_UPPER;
				utf8_out(tolower(c), word_out);
			} else {
				wtab->flags &= ~FLAG_FIRST_UPPER;
			}

			TranslateWord3(tr, word_out, wtab, NULL);

			int n;
			if (first_word) {
				n = snprintf(phonemes, available, "%s", word_phonemes);
				first_word = 0;
			} else {
				n = snprintf(phonemes, available, "%c%s", phonEND_WORD, word_phonemes);
			}

			available -= n;
			phonemes += n;

			// skip to the next word in a multi-word replacement
			while (!isspace(*word_out)) ++word_out;
			while (isspace(*word_out))  ++word_out;
		}
		snprintf(word_phonemes, sizeof(word_phonemes), "%s", words_phonemes);
	}
	return flags;
}

static void SetPlist2(PHONEME_LIST2 *p, unsigned char phcode)
{
	p->phcode = phcode;
	p->stresslevel = 0;
	p->tone_ph = 0;
	p->synthflags = embedded_flag;
	p->sourceix = 0;
	embedded_flag = 0;
}

static int CountSyllables(unsigned char *phonemes)
{
	int count = 0;
	int phon;
	while ((phon = *phonemes++) != 0) {
		if (phoneme_tab[phon]->type == phVOWEL)
			count++;
	}
	return count;
}

static void Word_EmbeddedCmd()
{
	// Process embedded commands for emphasis, sayas, and break
	int embedded_cmd;
	int value;

	do {
		embedded_cmd = embedded_list[embedded_read++];
		value = embedded_cmd >> 8;

		switch (embedded_cmd & 0x1f)
		{
		case EMBED_Y:
			option_sayas = value;
			break;

		case EMBED_F:
			option_emphasis = value;
			break;

		case EMBED_B:
			// break command
			if (value == 0)
				pre_pause = 0; // break=none
			else
				pre_pause += value;
			break;
		}
	} while (((embedded_cmd & 0x80) == 0) && (embedded_read < embedded_ix));
}

int SetTranslator2(const char *new_language)
{
	// Set translator2 to a second language
	int new_phoneme_tab;

	if ((new_phoneme_tab = SelectPhonemeTableName(new_language)) >= 0) {
		if ((translator2 != NULL) && (strcmp(new_language, translator2_language) != 0)) {
			// we already have an alternative translator, but not for the required language, delete it
			DeleteTranslator(translator2);
			translator2 = NULL;
		}

		if (translator2 == NULL) {
			translator2 = SelectTranslator(new_language);
			strcpy(translator2_language, new_language);

			if (LoadDictionary(translator2, translator2->dictionary_name, 0) != 0) {
				SelectPhonemeTable(voice->phoneme_tab_ix); // revert to original phoneme table
				new_phoneme_tab = -1;
				translator2_language[0] = 0;
			}
			translator2->phoneme_tab_ix = new_phoneme_tab;
		}
	}
	if (translator2 != NULL)
		translator2->phonemes_repeat[0] = 0;
	return new_phoneme_tab;
}

static int TranslateWord2(Translator *tr, char *word, WORD_TAB *wtab, int prepause)
{
	int flags = 0;
	int stress;
	int next_stress;
	int next_tone = 0;
	unsigned char *p;
	int srcix;
	int found_dict_flag;
	unsigned char ph_code;
	PHONEME_LIST2 *plist2;
	PHONEME_TAB *ph;
	int max_stress;
	int max_stress_ix = 0;
	int prev_vowel = -1;
	int pitch_raised = 0;
	int switch_phonemes = -1;
	int first_phoneme = 1;
	int source_ix;
	int len;
	int ix;
	int sylimit; // max. number of syllables in a word to be combined with a preceding preposition
	const char *new_language;
	int bad_phoneme;
	int word_flags;
	int word_copy_len;
	char word_copy[N_WORD_BYTES+1];
	char word_replaced[N_WORD_BYTES+1];
	char old_dictionary_name[40];

	len = wtab->length;
	if (len > 31) len = 31;
	source_ix = (wtab->sourceix & 0x7ff) | (len << 11); // bits 0-10 sourceix, bits 11-15 word length

	word_flags = wtab[0].flags;
	if (word_flags & FLAG_EMBEDDED) {
		wtab[0].flags &= ~FLAG_EMBEDDED; // clear it in case we call TranslateWord2() again for the same word
		embedded_flag = SFLAG_EMBEDDED;

		Word_EmbeddedCmd();
	}

	if ((word[0] == 0) || (word_flags & FLAG_DELETE_WORD)) {
		// nothing to translate.  Add a dummy phoneme to carry any embedded commands
		if (embedded_flag) {
			ph_list2[n_ph_list2].phcode = phonEND_WORD;
			ph_list2[n_ph_list2].stresslevel = 0;
			ph_list2[n_ph_list2].wordstress = 0;
			ph_list2[n_ph_list2].tone_ph = 0;
			ph_list2[n_ph_list2].synthflags = embedded_flag;
			ph_list2[n_ph_list2].sourceix = 0;
			n_ph_list2++;
			embedded_flag = 0;
		}
		word_phonemes[0] = 0;
		return 0;
	}

	// after a $pause word attribute, ignore a $pause attribute on the next two words
	if (tr->prepause_timeout > 0)
		tr->prepause_timeout--;

	if ((option_sayas & 0xf0) == 0x10) {
		if (!(word_flags & FLAG_FIRST_WORD)) {
			// SAYAS_CHARS, SAYAS_GLYPHS, or SAYAS_SINGLECHARS.  Pause between each word.
			prepause += 4;
		}
	}

	if (word_flags & FLAG_FIRST_UPPER) {
		if ((option_capitals > 2) && (embedded_ix < N_EMBEDDED_LIST-6)) {
			// indicate capital letter by raising pitch
			if (embedded_flag)
				embedded_list[embedded_ix-1] &= ~0x80; // already embedded command before this word, remove terminator
			if ((pitch_raised = option_capitals) == 3)
				pitch_raised = 20; // default pitch raise for capitals
			embedded_list[embedded_ix++] = EMBED_P+0x40+0x80 + (pitch_raised << 8); // raise pitch
			embedded_flag = SFLAG_EMBEDDED;
		}
	}

	p = (unsigned char *)word_phonemes;
	if (word_flags & FLAG_PHONEMES) {
		// The input is in phoneme mnemonics, not language text
		int c1;
		char lang_name[12];

		if (memcmp(word, "_^_", 3) == 0) {
			// switch languages
			word += 3;
			for (ix = 0;;) {
				c1 = *word++;
				if ((c1 == ' ') || (c1 == 0))
					break;
				lang_name[ix++] = tolower(c1);
			}
			lang_name[ix] = 0;

			if ((ix = LookupPhonemeTable(lang_name)) > 0) {
				SelectPhonemeTable(ix);
				word_phonemes[0] = phonSWITCH;
				word_phonemes[1] = ix;
				word_phonemes[2] = 0;
			}
		} else
			EncodePhonemes(word, word_phonemes, &bad_phoneme);
		flags = FLAG_FOUND;
	} else {
		int c2;
		ix = 0;
		while (((c2 = word_copy[ix] = word[ix]) != ' ') && (c2 != 0) && (ix < N_WORD_BYTES)) ix++;
		word_copy_len = ix;

		word_replaced[2] = 0;
		flags = TranslateWord(translator, word, wtab, &word_replaced[2]);

		if (flags & FLAG_SPELLWORD) {
			// re-translate the word as individual letters, separated by spaces
			memcpy(word, word_copy, word_copy_len);
			return flags;
		}

		if ((flags & FLAG_COMBINE) && !(wtab[1].flags & FLAG_PHONEMES)) {
			char *p2;
			int ok = 1;
			unsigned int flags2[2];
			int c_word2;
			char ph_buf[N_WORD_PHONEMES];

			flags2[0] = 0;
			sylimit = tr->langopts.param[LOPT_COMBINE_WORDS];

			// LANG=cs,sk
			// combine a preposition with the following word
			p2 = word;
			while (*p2 != ' ') p2++;

			utf8_in(&c_word2, p2+1); // first character of the next word;
			if (!iswalpha(c_word2))
				ok = 0;

			if (ok != 0) {
				strcpy(ph_buf, word_phonemes);

				flags2[0] = TranslateWord(translator, p2+1, wtab+1, NULL);
				if ((flags2[0] & FLAG_WAS_UNPRONOUNCABLE) || (word_phonemes[0] == phonSWITCH))
					ok = 0;

				if (sylimit & 0x100) {
					// only if the second word has $alt attribute
					if ((flags2[0] & FLAG_ALT_TRANS) == 0)
						ok = 0;
				}

				if ((sylimit & 0x200) && ((wtab+1)->flags & FLAG_LAST_WORD)) {
					// not if the next word is end-of-sentence
					ok = 0;
				}

				if (ok == 0)
					strcpy(word_phonemes, ph_buf);
			}

			if (ok) {
				*p2 = '-'; // replace next space by hyphen
				wtab[0].flags &= ~FLAG_ALL_UPPER; // prevent it being considered an abbreviation
				flags = TranslateWord(translator, word, wtab, NULL); // translate the combined word
				if ((sylimit > 0) && (CountSyllables(p) > (sylimit & 0x1f))) {
					// revert to separate words
					*p2 = ' ';
					flags = TranslateWord(translator, word, wtab, NULL);
				} else {
					if (flags == 0)
						flags = flags2[0]; // no flags for the combined word, so use flags from the second word eg. lang-hu "nem december 7-e"
					flags |= FLAG_SKIPWORDS;
					dictionary_skipwords = 1;
				}
			}
		}

		if (p[0] == phonSWITCH) {
			int switch_attempt;
			strcpy(old_dictionary_name, dictionary_name);
			for (switch_attempt = 0; switch_attempt < 2; switch_attempt++) {
				// this word uses a different language
				memcpy(word, word_copy, word_copy_len);

				new_language = (char *)(&p[1]);
				if (new_language[0] == 0)
					new_language = "en";

				switch_phonemes = SetTranslator2(new_language);

				if (switch_phonemes >= 0) {
					// re-translate the word using the new translator
					wtab[0].flags |= FLAG_TRANSLATOR2;
					if (word_replaced[2] != 0) {
						word_replaced[0] = 0; // byte before the start of the word
						word_replaced[1] = ' ';
						flags = TranslateWord(translator2, &word_replaced[1], wtab, NULL);
					} else
						flags = TranslateWord(translator2, word, wtab, &word_replaced[2]);
				}

				if (p[0] != phonSWITCH)
					break;
			}

			if (p[0] == phonSWITCH)
				return FLAG_SPELLWORD;

			if (switch_phonemes < 0) {
				// language code is not recognised or 2nd translator won't translate it
				p[0] = phonSCHWA; // just say something
				p[1] = phonSCHWA;
				p[2] = 0;
			}

			if (switch_phonemes == -1) {
				strcpy(dictionary_name, old_dictionary_name);
				SelectPhonemeTable(voice->phoneme_tab_ix);

				// leave switch_phonemes set, but use the original phoneme table number.
				// This will suppress LOPT_REGRESSIVE_VOICING
				switch_phonemes = voice->phoneme_tab_ix; // original phoneme table
			}
		}

		if (!(word_flags & FLAG_HYPHEN)) {
			if (flags & FLAG_PAUSE1) {
				if (prepause < 1)
					prepause = 1;
			}
			if ((flags & FLAG_PREPAUSE) && !(word_flags & (FLAG_LAST_WORD | FLAG_FIRST_WORD)) && !(wtab[-1].flags & FLAG_FIRST_WORD) && (tr->prepause_timeout == 0)) {
				// the word is marked in the dictionary list with $pause
				if (prepause < 4) prepause = 4;
				tr->prepause_timeout = 3;
			}
		}

		if ((option_emphasis >= 3) && (prepause < 1))
			prepause = 1;
	}

	stress = 0;
	next_stress = 1;
	srcix = 0;
	max_stress = -1;

	found_dict_flag = 0;
	if ((flags & FLAG_FOUND) && !(flags & FLAG_TEXTMODE))
		found_dict_flag = SFLAG_DICTIONARY;

	while ((prepause > 0) && (n_ph_list2 < N_PHONEME_LIST-4)) {
		// add pause phonemes here. Either because of punctuation (brackets or quotes) in the
		// text, or because the word is marked in the dictionary lookup as a conjunction
		if (prepause > 1) {
			SetPlist2(&ph_list2[n_ph_list2++], phonPAUSE);
			prepause -= 2;
		} else {
			SetPlist2(&ph_list2[n_ph_list2++], phonPAUSE_NOLINK);
			prepause--;
		}
		tr->end_stressed_vowel = 0; // forget about the previous word
		tr->prev_dict_flags[0] = 0;
		tr->prev_dict_flags[1] = 0;
	}
	plist2 = &ph_list2[n_ph_list2];

	if ((option_capitals == 1) && (word_flags & FLAG_FIRST_UPPER)) {
		SetPlist2(&ph_list2[n_ph_list2++], phonPAUSE_SHORT);
		SetPlist2(&ph_list2[n_ph_list2++], phonCAPITAL);
		if ((word_flags & FLAG_ALL_UPPER) && IsAlpha(word[1])) {
			// word > 1 letter and all capitals
			SetPlist2(&ph_list2[n_ph_list2++], phonPAUSE_SHORT);
			SetPlist2(&ph_list2[n_ph_list2++], phonCAPITAL);
		}
	}

	if (switch_phonemes >= 0) {
		if ((p[0] == phonPAUSE) && (p[1] == phonSWITCH)) {
			// the new word starts with a phoneme table switch, so there's no need to switch before it.
			if (ph_list2[n_ph_list2-1].phcode == phonSWITCH) {
				// previous phoneme is also a phonSWITCH, delete it
				n_ph_list2--;
			}
		} else {
			// this word uses a different phoneme table
			if (ph_list2[n_ph_list2-1].phcode == phonSWITCH) {
				// previous phoneme is also a phonSWITCH, just change its phoneme table number
				n_ph_list2--;
			} else
				SetPlist2(&ph_list2[n_ph_list2], phonSWITCH);
			ph_list2[n_ph_list2++].tone_ph = switch_phonemes; // temporary phoneme table number
		}
	}

	// remove initial pause from a word if it follows a hyphen
	if ((word_flags & FLAG_HYPHEN) && (phoneme_tab[*p]->type == phPAUSE))
		p++;

	if ((p[0] == 0) && (embedded_flag)) {
		// no phonemes.  Insert a very short pause to carry an embedded command
		p[0] = phonPAUSE_VSHORT;
		p[1] = 0;
	}

	while (((ph_code = *p++) != 0) && (n_ph_list2 < N_PHONEME_LIST-4)) {
		if (ph_code == 255)
			continue; // unknown phoneme

		// Add the phonemes to the first stage phoneme list (ph_list2)
		ph = phoneme_tab[ph_code];

		if (ph_code == phonSWITCH) {
			ph_list2[n_ph_list2].phcode = ph_code;
			ph_list2[n_ph_list2].sourceix = 0;
			ph_list2[n_ph_list2].synthflags = 0;
			ph_list2[n_ph_list2++].tone_ph = *p;
			SelectPhonemeTable(*p);
			p++;
		} else if (ph->type == phSTRESS) {
			// don't add stress phonemes codes to the list, but give their stress
			// value to the next vowel phoneme
			// std_length is used to hold stress number or (if >10) a tone number for a tone language
			if (ph->program == 0)
				next_stress = ph->std_length;
			else {
				// for tone languages, the tone number for a syllable follows the vowel
				if (prev_vowel >= 0)
					ph_list2[prev_vowel].tone_ph = ph_code;
				else
					next_tone = ph_code; // no previous vowel, apply to the next vowel
			}
		} else if (ph_code == phonSYLLABIC) {
			// mark the previous phoneme as a syllabic consonant
			prev_vowel = n_ph_list2-1;
			ph_list2[prev_vowel].synthflags |= SFLAG_SYLLABLE;
			ph_list2[prev_vowel].stresslevel = next_stress;
		} else if (ph_code == phonLENGTHEN)
			ph_list2[n_ph_list2-1].synthflags |= SFLAG_LENGTHEN;
		else if (ph_code == phonEND_WORD) {
			// a || symbol in a phoneme string was used to indicate a word boundary
			// Don't add this phoneme to the list, but make sure the next phoneme has
			// a newword indication
			srcix = source_ix+1;
		} else if (ph_code == phonX1) {
			// a language specific action
			if (tr->langopts.param[LOPT_IT_DOUBLING])
				flags |= FLAG_DOUBLING;
		} else {
			ph_list2[n_ph_list2].phcode = ph_code;
			ph_list2[n_ph_list2].tone_ph = 0;
			ph_list2[n_ph_list2].synthflags = embedded_flag | found_dict_flag;
			embedded_flag = 0;
			ph_list2[n_ph_list2].sourceix = srcix;
			srcix = 0;

			if (ph->type == phVOWEL) {
				stress = next_stress;
				next_stress = 1; // default is 'unstressed'

				if (stress >= 4)
					any_stressed_words = 1;

				if ((prev_vowel >= 0) && (n_ph_list2-1) != prev_vowel)
					ph_list2[n_ph_list2-1].stresslevel = stress; // set stress for previous consonant

				ph_list2[n_ph_list2].synthflags |= SFLAG_SYLLABLE;
				prev_vowel = n_ph_list2;

				if (stress > max_stress) {
					max_stress = stress;
					max_stress_ix = n_ph_list2;
				}
				if (next_tone != 0) {
					ph_list2[n_ph_list2].tone_ph = next_tone;
					next_tone = 0;
				}
			} else {
				if (first_phoneme && tr->langopts.param[LOPT_IT_DOUBLING]) {
					if (((tr->prev_dict_flags[0] & FLAG_DOUBLING) && (tr->langopts.param[LOPT_IT_DOUBLING] & 1)) ||
					    (tr->end_stressed_vowel && (tr->langopts.param[LOPT_IT_DOUBLING] & 2))) {
						// italian, double the initial consonant if the previous word ends with a
						// stressed vowel, or is marked with a flag
						ph_list2[n_ph_list2].synthflags |= SFLAG_LENGTHEN;
					}
				}
			}

			ph_list2[n_ph_list2].stresslevel = stress;
			n_ph_list2++;
			first_phoneme = 0;
		}
	}

	if (word_flags & FLAG_COMMA_AFTER)
		SetPlist2(&ph_list2[n_ph_list2++], phonPAUSE_CLAUSE);

	// don't set new-word if there is a hyphen before it
	if ((word_flags & FLAG_HYPHEN) == 0)
		plist2->sourceix = source_ix;

	tr->end_stressed_vowel = 0;
	if ((stress >= 4) && (phoneme_tab[ph_list2[n_ph_list2-1].phcode]->type == phVOWEL))
		tr->end_stressed_vowel = 1; // word ends with a stressed vowel

	if (switch_phonemes >= 0) {
		// this word uses a different phoneme table, now switch back
		strcpy(dictionary_name, old_dictionary_name);
		SelectPhonemeTable(voice->phoneme_tab_ix);
		SetPlist2(&ph_list2[n_ph_list2], phonSWITCH);
		ph_list2[n_ph_list2++].tone_ph = voice->phoneme_tab_ix; // original phoneme table number
	}


	if (pitch_raised > 0) {
		embedded_list[embedded_ix++] = EMBED_P+0x60+0x80 + (pitch_raised << 8); // lower pitch
		SetPlist2(&ph_list2[n_ph_list2], phonPAUSE_SHORT);
		ph_list2[n_ph_list2++].synthflags = SFLAG_EMBEDDED;
	}

	if (flags & FLAG_STRESS_END2) {
		// this's word's stress could be increased later
		ph_list2[max_stress_ix].synthflags |= SFLAG_PROMOTE_STRESS;
	}

	tr->prev_dict_flags[0] = flags;
	return flags;
}

static int EmbeddedCommand(unsigned int *source_index_out)
{
	// An embedded command to change the pitch, volume, etc.
	// returns number of commands added to embedded_list

	// pitch,speed,amplitude,expression,reverb,tone,voice,sayas
	const char *commands = "PSARHTIVYMUBF";
	int value = -1;
	int sign = 0;
	unsigned char c;
	char *p;
	int cmd;
	int source_index = *source_index_out;

	c = source[source_index];
	if (c == '+') {
		sign = 0x40;
		source_index++;
	} else if (c == '-') {
		sign = 0x60;
		source_index++;
	}

	if (IsDigit09(source[source_index])) {
		value = atoi(&source[source_index]);
		while (IsDigit09(source[source_index]))
			source_index++;
	}

	c = source[source_index++];
	if (embedded_ix >= (N_EMBEDDED_LIST - 2))
		return 0; // list is full

	if ((p = strchr_w(commands, c)) == NULL)
		return 0;
	cmd = (p - commands)+1;
	if (value == -1) {
		value = embedded_default[cmd];
		sign = 0;
	}

	if (cmd == EMBED_Y) {
		option_sayas2 = value;
		count_sayas_digits = 0;
	}
	if (cmd == EMBED_F) {
		if (value >= 3)
			word_emphasis = FLAG_EMPHASIZED;
		else
			word_emphasis = 0;
	}

	embedded_list[embedded_ix++] = cmd + sign + (value << 8);
	*source_index_out = source_index;
	return 1;
}

static int SubstituteChar(Translator *tr, unsigned int c, unsigned int next_in, int *insert, int *wordflags)
{
	int ix;
	unsigned int word;
	unsigned int new_c, c2, c_lower;
	int upper_case = 0;
	static int ignore_next = 0;
	const unsigned int *replace_chars;

	if (ignore_next) {
		ignore_next = 0;
		return 8;
	}
	if (c == 0) return 0;

	if ((replace_chars = tr->langopts.replace_chars) == NULL)
		return c;

	// there is a list of character codes to be substituted with alternative codes

	if (iswupper(c_lower = c)) {
		c_lower = towlower2(c);
		upper_case = 1;
	}

	new_c = 0;
	for (ix = 0; (word = replace_chars[ix]) != 0; ix += 2) {
		if (c_lower == (word & 0xffff)) {
			if ((word >> 16) == 0) {
				new_c = replace_chars[ix+1];
				break;
			}
			if ((word >> 16) == (unsigned int)towlower2(next_in)) {
				new_c = replace_chars[ix+1];
				ignore_next = 1;
				break;
			}
		}
	}

	if (new_c == 0)
		return c; // no substitution

	if (new_c & 0xffe00000) {
		// there is a second character to be inserted
		// don't convert the case of the second character unless the next letter is also upper case
		c2 = new_c >> 16;
		if (upper_case && iswupper(next_in))
			c2 = toupper(c2);
		*insert = c2;
		new_c &= 0xffff;
	}

	if (upper_case)
		new_c = toupper(new_c);

	*wordflags |= FLAG_CHAR_REPLACED;
	return new_c;
}

static int TranslateChar(Translator *tr, char *ptr, int prev_in, unsigned int c, unsigned int next_in, int *insert, int *wordflags)
{
	// To allow language specific examination and replacement of characters

	int code;
	int initial;
	int medial;
	int final;
	int next2;

	static const unsigned char hangul_compatibility[0x34] = {
		0,  0x00, 0x01, 0xaa, 0x02, 0xac, 0xad, 0x03,
		0x04, 0x05, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb4,
		0xb6, 0x06, 0x07, 0x08, 0xb9, 0x09, 0x0a, 0xbc,
		0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x61,
		0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
		0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71,
		0x72, 0x73, 0x74, 0x75
	};

	// check for Korean Hangul letters
	if (((code = c - 0xac00) >= 0) && (c <= 0xd7af)) {
		// break a syllable hangul into 2 or 3 individual jamo
		initial = (code/28)/21;
		medial = (code/28) % 21;
		final = code % 28;

		if (initial == 11) {
			// null initial
			c = medial + 0x1161;
			if (final > 0)
				*insert = final + 0x11a7;
		} else {
			// extact the initial and insert the remainder with a null initial
			c = initial + 0x1100;
			*insert = (11*28*21) + (medial*28) + final + 0xac00;
		}
		return c;
	} else if (((code = c - 0x3130) >= 0) && (code < 0x34)) {
		// Hangul compatibility jamo
		return hangul_compatibility[code] + 0x1100;
	}

	switch (tr->translator_name)
	{
	case L('a', 'f'):
	case L('n', 'l'):
		// look for 'n  and replace by a special character (unicode: schwa)

		if (!iswalpha(prev_in)) {
			utf8_in(&next2, &ptr[1]);

			if ((c == '\'') && IsSpace(next2)) {
				if ((next_in == 'n') && (tr->translator_name == L('a', 'f'))) {
					// n preceded by either apostrophe or U2019 "right single quotation mark"
					ptr[0] = ' '; // delete the n
					return 0x0259; // replace  '  by  unicode schwa character
				}
				if ((next_in == 'n') || (next_in == 't')) {
					// Dutch, [@n] and [@t]
					return 0x0259; // replace  '  by  unicode schwa character
				}
			}
		}
		break;
	}
	return SubstituteChar(tr, c, next_in, insert, wordflags);
}

static const char *UCase_ga[] = { "bp", "bhf", "dt", "gc", "hA", "mb", "nd", "ng", "ts", "tA", "nA", NULL };

static int UpperCaseInWord(Translator *tr, char *word, int c)
{
	int ix;
	int len;
	const char *p;

	if (tr->translator_name == L('g', 'a')) {
		// Irish
		for (ix = 0;; ix++) {
			if ((p = UCase_ga[ix]) == NULL)
				break;

			len = strlen(p);
			if ((word[-len] == ' ') && (memcmp(&word[-len+1], p, len-1) == 0)) {
				if ((c == p[len-1]) || ((p[len-1] == 'A') && IsVowel(tr, c)))
					return 1;
			}
		}
	}
	return 0;
}

void TranslateClause(Translator *tr, int *tone_out, char **voice_change)
{
	int ix;
	int c;
	int cc = 0;
	unsigned int source_index = 0;
	unsigned int prev_source_index = 0;
	int source_index_word = 0;
	int prev_in;
	int prev_out = ' ';
	int prev_out2;
	int prev_in_save = 0;
	int next_in;
	int next_in_nbytes;
	int char_inserted = 0;
	int clause_pause;
	int pre_pause_add = 0;
	int word_mark = 0;
	int all_upper_case = FLAG_ALL_UPPER;
	int finished;
	int single_quoted;
	int phoneme_mode = 0;
	int dict_flags = 0; // returned from dictionary lookup
	int word_flags; // set here
	int next_word_flags;
	int new_sentence2;
	int embedded_count = 0;
	int letter_count = 0;
	int space_inserted = 0;
	int syllable_marked = 0;
	int decimal_sep_count = 0;
	char *word;
	char *p;
	int j, k;
	int n_digits;
	int charix_top = 0;

	short charix[N_TR_SOURCE+4];
	WORD_TAB words[N_CLAUSE_WORDS];
	static char voice_change_name[40];
	int word_count = 0; // index into words

	char sbuf[N_TR_SOURCE];

	int terminator;
	int tone;
	int tone2;

	if (tr == NULL)
		return;

	embedded_ix = 0;
	embedded_read = 0;
	pre_pause = 0;
	any_stressed_words = 0;

	if ((clause_start_char = count_characters) < 0)
		clause_start_char = 0;
	clause_start_word = count_words + 1;

	for (ix = 0; ix < N_TR_SOURCE; ix++)
		charix[ix] = 0;
	terminator = ReadClause(tr, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name);

	charix[charix_top+1] = 0;
	charix[charix_top+2] = 0x7fff;
	charix[charix_top+3] = 0;

	clause_pause = (terminator & CLAUSE_PAUSE) * 10; // mS
	if (terminator & CLAUSE_PAUSE_LONG)
		clause_pause = clause_pause * 32; // pause value is *320mS not *10mS

	tone = (terminator & CLAUSE_INTONATION_TYPE) >> 12;
	if (tone2 != 0) {
		// override the tone type
		tone = tone2;
	}

	for (p = source; *p != 0; p++) {
		if (!isspace2(*p))
			break;
	}
	if (*p == 0) {
		// No characters except spaces. This is not a sentence.
		// Don't add this pause, just make up the previous pause to this value;
		clause_pause -= max_clause_pause;
		if (clause_pause < 0)
			clause_pause = 0;

		if (new_sentence)
			terminator |= CLAUSE_TYPE_SENTENCE; // carry forward an end-of-sentence indicator
		max_clause_pause += clause_pause;
		new_sentence2 = 0;
	} else {
		max_clause_pause = clause_pause;
		new_sentence2 = new_sentence;
	}
	tr->clause_terminator = terminator;

	if (new_sentence2) {
		count_sentences++;
		if (skip_sentences > 0) {
			skip_sentences--;
			if (skip_sentences == 0)
				skipping_text = 0;
		}
	}

	memset(&ph_list2[0], 0, sizeof(ph_list2[0]));
	ph_list2[0].phcode = phonPAUSE_SHORT;

	n_ph_list2 = 1;
	tr->prev_last_stress = 0;
	tr->prepause_timeout = 0;
	tr->expect_verb = 0;
	tr->expect_noun = 0;
	tr->expect_past = 0;
	tr->expect_verb_s = 0;
	tr->phonemes_repeat_count = 0;
	tr->end_stressed_vowel = 0;
	tr->prev_dict_flags[0] = 0;
	tr->prev_dict_flags[1] = 0;

	word_count = 0;
	single_quoted = 0;
	word_flags = 0;
	next_word_flags = 0;

	sbuf[0] = 0;
	sbuf[1] = ' ';
	sbuf[2] = ' ';
	ix = 3;
	prev_in = ' ';

	words[0].start = ix;
	words[0].flags = 0;
	finished = 0;

	for (j = 0; charix[j] <= 0; j++) ;
	words[0].sourceix = charix[j];
	k = 0;
	while (charix[j] != 0) {
		// count the number of characters (excluding multibyte continuation bytes)
		if (charix[j++] != -1)
			k++;
	}
	words[0].length = k;

	while (!finished && (ix < (int)sizeof(sbuf)) && (n_ph_list2 < N_PHONEME_LIST-4)) {
		prev_out2 = prev_out;
		utf8_in2(&prev_out, &sbuf[ix-1], 1);

		if (tr->langopts.tone_numbers && IsDigit09(prev_out) && IsAlpha(prev_out2)) {
			// tone numbers can be part of a word, consider them as alphabetic
			prev_out = 'a';
		}

		if (prev_in_save != 0) {
			prev_in = prev_in_save;
			prev_in_save = 0;
		} else if (source_index > 0)
			utf8_in2(&prev_in, &source[source_index-1], 1);

		prev_source_index = source_index;

		if (char_inserted) {
			c = char_inserted;
			char_inserted = 0;
		} else {
			source_index += utf8_in(&cc, &source[source_index]);
			c = cc;
		}
		next_in_nbytes = utf8_in(&next_in, &source[source_index]);

		if (c == 0) {
			finished = 1;
			c = ' ';
		}

		if ((c == CTRL_EMBEDDED) || (c == ctrl_embedded)) {
			// start of embedded command in the text
			int srcix = source_index-1;

			if (prev_in != ' ') {
				c = ' ';
				prev_in_save = c;
				source_index--;
			} else {
				embedded_count += EmbeddedCommand(&source_index);
				prev_in_save = prev_in;
				// replace the embedded command by spaces
				memset(&source[srcix], ' ', source_index-srcix);
				source_index = srcix;
				continue;
			}
		}

		if ((option_sayas2 == SAYAS_KEY) && (c != ' ')) {
			if ((prev_in == ' ') && (next_in == ' '))
				option_sayas2 = SAYAS_SINGLE_CHARS; // single character, speak its name
			c = towlower2(c);
		}


		if (phoneme_mode) {
			all_upper_case = FLAG_PHONEMES;

			if ((c == ']') && (next_in == ']')) {
				phoneme_mode = 0;
				source_index++;
				c = ' ';
			}
		} else if ((option_sayas2 & 0xf0) == SAYAS_DIGITS) {
			if (iswdigit(c)) {
				count_sayas_digits++;
				if (count_sayas_digits > (option_sayas2 & 0xf)) {
					// break after the specified number of digits
					c = ' ';
					space_inserted = 1;
					count_sayas_digits = 0;
				}
			} else {
				count_sayas_digits = 0;
				if (iswdigit(prev_out)) {
					c = ' ';
					space_inserted = 1;
				}
			}
		} else if ((option_sayas2 & 0x10) == 0) {
			// speak as words

			if ((c == 0x92) || (c == 0xb4) || (c == 0x2019) || (c == 0x2032))
				c = '\''; // 'microsoft' quote or sexed closing single quote, or prime - possibly used as apostrophe

			if (((c == 0x2018) || (c == '?')) && IsAlpha(prev_out) && IsAlpha(next_in)) {
				// ? between two letters may be a smart-quote replaced by ?
				c = '\'';
			}

			if (c == CHAR_EMPHASIS) {
				// this character is a marker that the previous word is the focus of the clause
				c = ' ';
				word_flags |= FLAG_FOCUS;
			}

			if (c == CHAR_COMMA_BREAK) {
				c = ' ';
				word_flags |= FLAG_COMMA_AFTER;
			}

			c = TranslateChar(tr, &source[source_index], prev_in, c, next_in, &char_inserted, &word_flags);  // optional language specific function
			if (c == 8)
				continue; // ignore this character

			if (char_inserted)
				next_in = char_inserted;

			// allow certain punctuation within a word (usually only apostrophe)
			if (!IsAlpha(c) && !IsSpace(c) && (wcschr(tr->punct_within_word, c) == 0)) {
				if (IsAlpha(prev_out)) {
					if (tr->langopts.tone_numbers && IsDigit09(c) && !IsDigit09(next_in)) {
						// allow a tone number as part of the word
					} else {
						c = ' '; // ensure we have an end-of-word terminator
						space_inserted = 1;
					}
				}
			}

			if (iswdigit(prev_out)) {
				if (!iswdigit(c) && (c != '.') && (c != ',') && (c != ' ')) {
					c = ' '; // terminate digit string with a space
					space_inserted = 1;
				}
			} else { // Prev output is not digit
				if (prev_in == ',') {
					// Workaround for several consecutive commas —
					// replace current character with space
					if (c == ',')
						c = ' ';
				} else {
					decimal_sep_count = 0;
				}
			}

			if (c == '[') {
				if ((next_in == '\002') || ((next_in == '[') && option_phoneme_input)) {
					//  "[\002" is used internally to start phoneme mode
					phoneme_mode = FLAG_PHONEMES;
					source_index++;
					continue;
				}
			}

			if (IsAlpha(c)) {
				if (!IsAlpha(prev_out) || (tr->langopts.ideographs && ((c > 0x3040) || (prev_out > 0x3040)))) {
					if (wcschr(tr->punct_within_word, prev_out) == 0)
						letter_count = 0; // don't reset count for an apostrophy within a word

					if ((prev_out != ' ') && (wcschr(tr->punct_within_word, prev_out) == 0)) {
						// start of word, insert space if not one there already
						c = ' ';
						space_inserted = 1;

						if (!IsBracket(prev_out)) // ?? perhaps only set FLAG_NOSPACE for . - /  (hyphenated words, URLs, etc)
							next_word_flags |= FLAG_NOSPACE;
					} else {
						if (iswupper(c))
							word_flags |= FLAG_FIRST_UPPER;

						if ((prev_out == ' ') && iswdigit(sbuf[ix-2]) && !iswdigit(prev_in)) {
							// word, following a number, but with a space between
							// Add an extra space, to distinguish "2 a" from "2a"
							sbuf[ix++] = ' ';
							words[word_count].start++;
						}
					}
				}

				if (c != ' ') {
					letter_count++;

					if (tr->letter_bits_offset > 0) {
						if (((c < 0x250) && (prev_out >= tr->letter_bits_offset)) ||
						    ((c >= tr->letter_bits_offset) && (letter_count > 1) && (prev_out < 0x250))) {
							// Don't mix native and Latin characters in the same word
							// Break into separate words
							if (IsAlpha(prev_out)) {
								c = ' ';
								space_inserted = 1;
								word_flags |= FLAG_HYPHEN_AFTER;
								next_word_flags |= FLAG_HYPHEN;
							}
						}
					}
				}

				if (iswupper(c)) {
					c = towlower2(c);

					if ((j = tr->langopts.param[LOPT_CAPS_IN_WORD]) > 0) {
						if ((j == 2) && (syllable_marked == 0)) {
							char_inserted = c;
							c = 0x2c8; // stress marker
							syllable_marked = 1;
						}
					} else {
						if (iswlower(prev_in)) {
							// lower case followed by upper case in a word
							if (UpperCaseInWord(tr, &sbuf[ix], c) == 1) {
								// convert to lower case and continue
								c = towlower2(c);
							} else {
								c = ' '; // lower case followed by upper case, treat as new word
								space_inserted = 1;
								prev_in_save = c;
							}
						} else if ((c != ' ') && iswupper(prev_in) && iswlower(next_in)) {
							int next2_in;
							utf8_in(&next2_in, &source[source_index + next_in_nbytes]);

							if ((tr->translator_name == L('n', 'l')) && (letter_count == 2) && (c == 'j') && (prev_in == 'I')) {
								// Dutch words may capitalise initial IJ, don't split
							} else if (IsAlpha(next2_in)) {
								// changing from upper to lower case, start new word at the last uppercase, if 3 or more letters
								c = ' ';
								space_inserted = 1;
								prev_in_save = c;
								next_word_flags |= FLAG_NOSPACE;
							}
						}
					}
				} else {
					if ((all_upper_case) && (letter_count > 2)) {
						if ((c == 's') && (next_in == ' ')) {
							c = ' ';
							all_upper_case |= FLAG_HAS_PLURAL;

							if (sbuf[ix-1] == '\'')
								sbuf[ix-1] = ' ';
						} else
							all_upper_case = 0; // current word contains lower case letters, not "'s"
					} else
						all_upper_case = 0;
				}
			} else if (c == '-') {
				if (!IsSpace(prev_in) && IsAlpha(next_in)) {
					if (prev_out != ' ') {
						// previous 'word' not yet ended (not alpha or numeric), start new word now.
						c = ' ';
						space_inserted = 1;
					} else {
						// '-' between two letters is a hyphen, treat as a space
						word_flags |= FLAG_HYPHEN;
						if (word_count > 0)
							words[word_count-1].flags |= FLAG_HYPHEN_AFTER;
						c = ' ';
					}
				} else if ((prev_in == ' ') && (next_in == ' ')) {
					// ' - ' dash between two spaces, treat as pause
					c = ' ';
					pre_pause_add = 4;
				} else if (next_in == '-') {
					// double hyphen, treat as pause
					source_index++;
					c = ' ';
					pre_pause_add = 4;
				} else if ((prev_out == ' ') && IsAlpha(prev_out2) && !IsAlpha(prev_in)) {
					// insert extra space between a word + space + hyphen, to distinguish 'a -2' from 'a-2'
					sbuf[ix++] = ' ';
					words[word_count].start++;
				}
			} else if (c == '.') {
				if (prev_out == '.') {
					// multiple dots, separate by spaces. Note >3 dots has been replaced by elipsis
					c = ' ';
					space_inserted = 1;
				} else if ((word_count > 0) && !(words[word_count-1].flags & FLAG_NOSPACE) && IsAlpha(prev_in)) {
					// dot after a word, with space following, probably an abbreviation
					words[word_count-1].flags |= FLAG_HAS_DOT;

					if (IsSpace(next_in) || (next_in == '-'))
						c = ' '; // remove the dot if it's followed by a space or hyphen, so that it's not pronounced
				}
			} else if (c == '\'') {
				if (((prev_in == '.') || iswalnum(prev_in)) && IsAlpha(next_in)) {
					// between two letters, or in an abbreviation (eg. u.s.a.'s). Consider the apostrophe as part of the word
					single_quoted = 0;
				} else if ((tr->langopts.param[LOPT_APOSTROPHE] & 1) && IsAlpha(next_in))
					single_quoted = 0; // apostrophe at start of word is part of the word
				else if ((tr->langopts.param[LOPT_APOSTROPHE] & 2) && IsAlpha(prev_in))
					single_quoted = 0; // apostrophe at end of word is part of the word
				else if ((wcschr(tr->char_plus_apostrophe, prev_in) != 0) && (prev_out2 == ' ')) {
					// consider single character plus apostrophe as a word
					single_quoted = 0;
					if (next_in == ' ')
						source_index++; // skip following space
				} else {
					if ((prev_out == 's') && (single_quoted == 0)) {
						// looks like apostrophe after an 's'
						c = ' ';
					} else {
						if (IsSpace(prev_out))
							single_quoted = 1;
						else
							single_quoted = 0;

						pre_pause_add = 4; // single quote
						c = ' ';
					}
				}
			} else if (lookupwchar(breaks, c) != 0)
				c = ' '; // various characters to treat as space
			else if (iswdigit(c)) {
				if (tr->langopts.tone_numbers && IsAlpha(prev_out) && !IsDigit(next_in)) {
				} else if ((prev_out != ' ') && !iswdigit(prev_out)) {
					if ((prev_out != tr->langopts.decimal_sep) || ((decimal_sep_count > 0) && (tr->langopts.decimal_sep == ','))) {
						c = ' ';
						space_inserted = 1;
					} else
						decimal_sep_count = 1;
				} else if ((prev_out == ' ') && IsAlpha(prev_out2) && !IsAlpha(prev_in)) {
					// insert extra space between a word and a number, to distinguish 'a 2' from 'a2'
					sbuf[ix++] = ' ';
					words[word_count].start++;
				}
			}
		}

		if (IsSpace(c)) {
			if (prev_out == ' ') {
				word_flags |= FLAG_MULTIPLE_SPACES;
				continue; // multiple spaces
			}

			if ((cc == 0x09) || (cc == 0x0a))
				next_word_flags |= FLAG_MULTIPLE_SPACES; // tab or newline, not a simple space

			if (space_inserted) {
				// count the number of characters since the start of the word
				j = 0;
				k = source_index - 1;
				while ((k >= source_index_word) && (charix[k] != 0)) {
					if (charix[k] > 0) // don't count initial bytes of multi-byte character
						j++;
					k--;
				}
				words[word_count].length = j;
			}

			source_index_word = source_index;

			// end of 'word'
			sbuf[ix++] = ' ';

			if ((word_count < N_CLAUSE_WORDS-1) && (ix > words[word_count].start)) {
				if (embedded_count > 0) {
					// there are embedded commands before this word
					embedded_list[embedded_ix-1] |= 0x80; // terminate list of commands for this word
					words[word_count].flags |= FLAG_EMBEDDED;
					embedded_count = 0;
				}
				words[word_count].pre_pause = pre_pause;
				words[word_count].flags |= (all_upper_case | word_flags | word_emphasis);
				words[word_count].wmark = word_mark;

				if (pre_pause > 0) {
					// insert an extra space before the word, to prevent influence from previous word across the pause
					for (j = ix; j > words[word_count].start; j--)
						sbuf[j] = sbuf[j-1];
					sbuf[j] = ' ';
					words[word_count].start++;
					ix++;
				}

				word_count++;
				words[word_count].start = ix;
				words[word_count].flags = 0;

				for (j = source_index; charix[j] <= 0; j++) // skip blanks
					;
				words[word_count].sourceix = charix[j];
				k = 0;
				while (charix[j] != 0) {
					// count the number of characters (excluding multibyte continuation bytes)
					if (charix[j++] != -1)
						k++;
				}
				words[word_count].length = k;

				word_flags = next_word_flags;
				next_word_flags = 0;
				pre_pause = 0;
				word_mark = 0;
				all_upper_case = FLAG_ALL_UPPER;
				syllable_marked = 0;
			}

			if (space_inserted) {
				source_index = prev_source_index; // rewind to the previous character
				char_inserted = 0;
				space_inserted = 0;
			}
		} else {
			if ((ix < (N_TR_SOURCE - 4)))
				ix += utf8_out(c, &sbuf[ix]);
		}
		if (pre_pause_add > pre_pause)
			pre_pause = pre_pause_add;
		pre_pause_add = 0;
	}

	if ((word_count == 0) && (embedded_count > 0)) {
		// add a null 'word' to carry the embedded command flag
		embedded_list[embedded_ix-1] |= 0x80;
		words[word_count].flags |= FLAG_EMBEDDED;
		word_count = 1;
	}

	tr->clause_end = &sbuf[ix-1];
	sbuf[ix] = 0;
	words[0].pre_pause = 0; // don't add extra pause at beginning of clause
	words[word_count].pre_pause = 8;
	if (word_count > 0) {
		ix = word_count-1;
		while ((ix > 0) && (IsBracket(sbuf[words[ix].start])))
			ix--; // the last word is a bracket, mark the previous word as last
		words[ix].flags |= FLAG_LAST_WORD;

		// FLAG_NOSPACE check to avoid recognizing  .mr  -mr
		if ((terminator & CLAUSE_DOT_AFTER_LAST_WORD) && !(words[word_count-1].flags & FLAG_NOSPACE))
			words[word_count-1].flags |= FLAG_HAS_DOT;
	}
	words[0].flags |= FLAG_FIRST_WORD;

	for (ix = 0; ix < word_count; ix++) {
		int nx;
		int c_temp;
		char *pn;
		char *pw;
		int nw;
		char number_buf[150];
		WORD_TAB num_wtab[50]; // copy of 'words', when splitting numbers into parts

		// start speaking at a specified word position in the text?
		count_words++;
		if (skip_words > 0) {
			skip_words--;
			if (skip_words == 0)
				skipping_text = 0;
		}
		if (skipping_text)
			continue;

		current_alphabet = NULL;

		// digits should have been converted to Latin alphabet ('0' to '9')
		word = pw = &sbuf[words[ix].start];

		if (iswdigit(word[0]) && (tr->langopts.break_numbers != BREAK_THOUSANDS)) {
			// Languages with 100000 numbers.  Remove thousands separators so that we can insert them again later
			pn = number_buf;
			while (pn < &number_buf[sizeof(number_buf)-20]) {
				if (iswdigit(*pw))
					*pn++ = *pw++;
				else if ((*pw == tr->langopts.thousands_sep) && (pw[1] == ' ')
				           && iswdigit(pw[2]) && (pw[3] != ' ') && (pw[4] != ' ')) { // don't allow only 1 or 2 digits in the final part
					pw += 2;
					ix++; // skip "word"
				} else {
					nx = pw - word;
					memset(word, ' ', nx);
					nx = pn - number_buf;
					memcpy(word, number_buf, nx);
					break;
				}
			}
			pw = word;
		}

		for (n_digits = 0; iswdigit(word[n_digits]); n_digits++) // count consecutive digits
			;

		if (n_digits > 4) {
			// word is entirely digits, insert commas and break into 3 digit "words"
			number_buf[0] = ' ';
			pn = &number_buf[1];
			nx = n_digits;
			nw = 0;

			if ((n_digits > tr->langopts.max_digits) || (word[0] == '0'))
				words[ix].flags |= FLAG_INDIVIDUAL_DIGITS;

			while (pn < &number_buf[sizeof(number_buf)-20]) {
				if (!IsDigit09(c = *pw++) && (c != tr->langopts.decimal_sep))
					break;

				*pn++ = c;
				nx--;
				if ((nx > 0) && (tr->langopts.break_numbers & (1 << nx))) {
					memcpy(&num_wtab[nw++], &words[ix], sizeof(WORD_TAB)); // copy the 'words' entry for each word of numbers

					if (tr->langopts.thousands_sep != ' ')
						*pn++ = tr->langopts.thousands_sep;
					*pn++ = ' ';

					if ((words[ix].flags & FLAG_INDIVIDUAL_DIGITS) == 0) {
						if (tr->langopts.break_numbers & (1 << (nx-1))) {
							// the next group only has 1 digits, make it three
							*pn++ = '0';
							*pn++ = '0';
						}
						if (tr->langopts.break_numbers & (1 << (nx-2))) {
							// the next group only has 2 digits (eg. Indian languages), make it three
							*pn++ = '0';
						}
					}
				}
			}
			pw--;
			memcpy(&num_wtab[nw], &words[ix], sizeof(WORD_TAB)*2); // the original number word, and the word after it

			for (j = 1; j <= nw; j++)
				num_wtab[j].flags &= ~(FLAG_MULTIPLE_SPACES | FLAG_EMBEDDED); // don't use these flags for subsequent parts when splitting a number

			// include the next few characters, in case there are an ordinal indicator or other suffix
			memcpy(pn, pw, 16);
			pn[16] = 0;
			nw = 0;

			for (pw = &number_buf[1]; pw < pn;) {
				// keep wflags for each part, for FLAG_HYPHEN_AFTER
				dict_flags = TranslateWord2(tr, pw, &num_wtab[nw++], words[ix].pre_pause);
				while (*pw++ != ' ')
					;
				words[ix].pre_pause = 0;
			}
		} else {
			pre_pause = 0;

			dict_flags = TranslateWord2(tr, word, &words[ix], words[ix].pre_pause);

			if (pre_pause > words[ix+1].pre_pause) {
				words[ix+1].pre_pause = pre_pause;
				pre_pause = 0;
			}

			if (dict_flags & FLAG_SPELLWORD) {
				// redo the word, speaking single letters
				for (pw = word; *pw != ' ';) {
					memset(number_buf, ' ', 9);
					nx = utf8_in(&c_temp, pw);
					memcpy(&number_buf[2], pw, nx);
					TranslateWord2(tr, &number_buf[2], &words[ix], 0);
					pw += nx;
				}
			}

			if ((dict_flags & (FLAG_ALLOW_DOT | FLAG_NEEDS_DOT)) && (ix == word_count - 1 - dictionary_skipwords) && (terminator & CLAUSE_DOT_AFTER_LAST_WORD)) {
				// probably an abbreviation such as Mr. or B. rather than end of sentence
				clause_pause = 10;
				tone = 4;
			}
		}

		if (dict_flags & FLAG_SKIPWORDS) {
			// dictionary indicates skip next word(s)
			while (dictionary_skipwords > 0) {
				words[ix+dictionary_skipwords].flags |= FLAG_DELETE_WORD;
				dictionary_skipwords--;
			}
		}
	}

	if (embedded_read < embedded_ix) {
		// any embedded commands not yet processed?
		Word_EmbeddedCmd();
	}

	for (ix = 0; ix < 2; ix++) {
		// terminate the clause with 2 PAUSE phonemes
		PHONEME_LIST2 *p2;
		p2 = &ph_list2[n_ph_list2 + ix];
		p2->phcode = phonPAUSE;
		p2->stresslevel = 0;
		p2->sourceix = source_index;
		p2->synthflags = 0;
	}
	n_ph_list2 += 2;

	if (count_words == 0)
		clause_pause = 0;
	if (Eof() && ((word_count == 0) || (option_endpause == 0)))
		clause_pause = 10;

	MakePhonemeList(tr, clause_pause, new_sentence2);
	phoneme_list[N_PHONEME_LIST].ph = NULL; // recognize end of phoneme_list array, in Generate()
	phoneme_list[N_PHONEME_LIST].sourceix = 1;

	if (embedded_count) { // ???? is this needed
		phoneme_list[n_phoneme_list-2].synthflags = SFLAG_EMBEDDED;
		embedded_list[embedded_ix-1] |= 0x80;
		embedded_list[embedded_ix] = 0x80;
	}

	prev_clause_pause = clause_pause;

	if (tone_out != NULL)
		*tone_out = tone;

	new_sentence = 0;
	if (terminator & CLAUSE_TYPE_SENTENCE)
		new_sentence = 1; // next clause is a new sentence

	if (voice_change != NULL) {
		// return new voice name if an embedded voice change command terminated the clause
		if (terminator & CLAUSE_TYPE_VOICE_CHANGE)
			*voice_change = voice_change_name;
		else
			*voice_change = NULL;
	}
}

void InitText(int control)
{
	count_sentences = 0;
	count_words = 0;
	end_character_position = 0;
	skip_sentences = 0;
	skip_marker[0] = 0;
	skip_words = 0;
	skip_characters = 0;
	skipping_text = 0;
	new_sentence = 1;

	prev_clause_pause = 0;

	option_sayas = 0;
	option_sayas2 = 0;
	option_emphasis = 0;
	word_emphasis = 0;
	embedded_flag = 0;

	InitText2();

	if ((control & espeakKEEP_NAMEDATA) == 0)
		InitNamedata();
}
