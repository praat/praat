/* espeakdata_FileInMemory.cpp
 *
 * Copyright (C) David Weenink 2012, 2015-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// The glue between Praat and espeak

#include "NUM2.h"
#include "espeak_ng.h"
#include "speech.h"
#include <wctype.h>

#include "espeakdata_FileInMemory.h"

autoTable Table_createAsEspeakLanguagesIdAndNamePairs ();
autoTable Table_createAsEspeakVoicesProperties ();
autoStrings Table_column_to_Strings (Table me, integer column);

autoFileInMemoryManager espeak_ng_FileInMemoryManager;

autoFileInMemorySet espeakdata_languages;
autoFileInMemorySet espeakdata_dicts;
autoFileInMemorySet espeakdata_phons;
autoFileInMemorySet espeakdata_voices;
autoFileInMemorySet espeak_ng_data_allFilesInMemory;
autoTable espeakdata_languages_idAndNameTable;
autoTable espeakdata_voices_propertiesTable;
autoStrings espeakdata_voices_names;
autoStrings espeakdata_languages_names;

static void FileInMemorySet_and_Strings_changeIds (FileInMemorySet me, Strings thee) {
	try {
		if (my size != thy numberOfStrings) return; // do nothing
		for (integer i = 1; i <= my size; i ++) {
			FileInMemory_setId ((FileInMemory) my at [i], thy strings [i]);
		}
	} catch (MelderError) {
		Melder_throw (me, U"Ids not changed.");
	}
}

integer Table_findStringInColumn (Table me, const char32 *string, integer icol) {
	integer row = 0;
	if (icol > 0 && icol <= my numberOfColumns) {
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			if (Melder_equ (myRow -> cells [icol]. string, string)) {
				return irow;
			}
		}
	}
	return row;
}

void espeakdata_praat_init () {
	try {
		espeak_ng_FileInMemoryManager = create_espeak_ng_FileInMemoryManager ();
		espeak_ng_data_allFilesInMemory = create_espeak_ng_data_allFilesInMemory ();
		espeakdata_dicts = create_espeakdata_dicts ();
		espeakdata_languages = create_espeakdata_languages ();
		espeakdata_voices = create_espeakdata_voices ();
		espeakdata_phons = create_espeakdata_phons ();
		espeakdata_languages_idAndNameTable = Table_createAsEspeakLanguagesIdAndNamePairs ();
		espeakdata_voices_propertiesTable = Table_createAsEspeakVoicesProperties ();
		espeakdata_languages_names = Table_column_to_Strings (espeakdata_languages_idAndNameTable.get(), 2);
		Strings_sort (espeakdata_languages_names.get());
		//espeakdata_languages_names = espeakdata_languages_sortById ();
		espeakdata_voices_names = FileInMemorySet_to_Strings_id (espeakdata_voices.get());
	} catch (MelderError) {
		Melder_throw (U"Espeakdata initialization not performed.");
	}
}

#define ESPEAK_ISSPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')

// imitates fgets_strip for file in memory
const char * espeakdata_get_voicedata (const char *data, integer ndata, char *buf, integer nbuf, integer *index) {
	if (ndata <= 0 || nbuf <= 0 || *index >= ndata) {
		return 0;
	}
	integer i = 0;
	while (i < nbuf && *index < ndata && ((buf [i] = data [i]) != '\n')) {
		i ++;
		(*index) ++;
	}
	(*index) ++;   // ppgb 20151020 fix
	integer idata = i + 1;
	buf[i] = '\0';
	while (--i >= 0 && ESPEAK_ISSPACE (buf[i])) {
		buf[i] = 0;
	}
	char *p = strstr (buf, "//");
	if (p != 0) {
		*p = 0;
	}
	return & data[idata];
}


static const char32 * get_wordAfterPrecursor_u8 (const unsigned char *text8, const char32 *precursor) {
	static char32 word [100];
	/*
		1. Find (first occurence of) 'precursor' at the start of a line (with optional leading whitespace).
		2. Get the word after 'precursor' (skip leading and trailing whitespace).
	*/
	autoMelderString regex;
	const char32 *text = Melder_peek8to32 (reinterpret_cast<const char *> (text8));
	MelderString_append (& regex, U"^\\s*", precursor, U"\\s+");
	char32 *p = nullptr;
	const char32 *pmatch = strstr_regexp (text, regex.string);
	if (pmatch) {
		while (*pmatch == U' ' || *pmatch ++ == U'\t'); // skip whitespace before 'precursor'
		pmatch += str32len (precursor); // skip 'precursor'
		while (*pmatch == U' ' || *pmatch ++ == U'\t'); // skip whitespace after 'precursor'
		pmatch --;
		p = word;
		char32 *p_end = p + 99;
		while ((*p = *pmatch ++) && *p != U' ' && *p != U'\t' && *p != U'\n' && p < p_end) { p ++; };
		*p = 0;
		p = word;
	}
	return p;
}

autoTable Table_createAsEspeakVoicesProperties () {
	try {
		FileInMemorySet me = espeakdata_voices.get();
		autoTable thee = Table_createWithColumnNames (my size, U"id name gender age variant");
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = (FileInMemory) my at [ifile];
			Table_setStringValue (thee.get(), ifile, 1, fim -> d_id);
			const char32 *word = get_wordAfterPrecursor_u8 (fim -> d_data, U"name");
			Table_setStringValue (thee.get(), ifile, 2, (word ?word : fim -> d_id));
			word = get_wordAfterPrecursor_u8 (fim -> d_data, U"gender");
			Table_setStringValue (thee.get(), ifile, 3, (word ? word : U"0"));
			word = get_wordAfterPrecursor_u8 (fim -> d_data, U"age");
			Table_setStringValue (thee.get(), ifile, 4, (word ? word : U"0"));
			word = get_wordAfterPrecursor_u8 (fim -> d_data, U"variant");
			Table_setStringValue (thee.get(), ifile, 5, (word ? word : U"0"));

		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Table with espeak-ng voice properties not created.");
	}
}

autoTable Table_createAsEspeakLanguagesIdAndNamePairs () {
	try {
		char32 *criterion = U"/lang/";
		FileInMemorySet me = espeak_ng_FileInMemoryManager -> files.get();
		integer numberOfMatches = FileInMemorySet_findNumberOfMatches_path (me, kMelder_string :: CONTAINS, criterion);
		
		autoTable thee = Table_createWithColumnNames (numberOfMatches, U"id name");
		integer irow = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = (FileInMemory) my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> d_path, kMelder_string :: CONTAINS, criterion)) {
				irow ++;
				Table_setStringValue (thee.get(), irow, 1, fim -> d_id);
				const char32 *word = get_wordAfterPrecursor_u8 (fim -> d_data, U"name");
				Table_setStringValue (thee.get(), irow, 2, (word ? word : fim -> d_id));
			}
		}
		Melder_assert (numberOfMatches == irow);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Table with espeak-ng languages not created.");
	}
}

autoStrings Table_column_to_Strings (Table me, integer column) {
	try {
		if (column < 0 || column > 2) {
			Melder_throw (U"Illegal columnn.");
		}
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, my rows.size);
		thy numberOfStrings = 0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			thy strings [irow] = Melder_dup (Table_getStringValue_Assert (me, irow, column));
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Espeakdata: voices not initialized.");
	}
}

char * espeakdata_get_dict_data (const char *name, unsigned int *size) {
	integer lsize;
	char *data = FileInMemorySet_getCopyOfData (espeakdata_dicts.get(), Melder_peek8to32 (name), &lsize);
	*size = (unsigned int) lsize;
	return data;
}


const char * espeakdata_get_languageData (const char *languageName, integer *numberOfBytes) { // TODO
	return FileInMemorySet_getData (espeakdata_languages.get(), Melder_peek8to32 (languageName), numberOfBytes);
}

const char * espeakdata_get_voiceData (const char *voiceName, integer *numberOfBytes) { //TODO 
	char *plus = strstr ((char *) voiceName, "+"); // prototype says: strstr (const char *, const char *)
	const char *name = ( plus ? ++ plus : voiceName );
	return FileInMemorySet_getData (espeakdata_voices.get(), Melder_peek8to32 (name), numberOfBytes);;
}

/* End of file espeakdata_FileInMemory.cpp */
