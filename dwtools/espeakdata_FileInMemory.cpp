/* espeakdata_FileInMemory.cpp
 *
 * Copyright (C) 2012-2021 David Weenink, 2024 Paul Boersma
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
#include "FileInMemory.h"
#include "speech.h"
#include "voice.h"
#include "Strings_extensions.h"
#include "Table_and_Strings.h"

#include "espeak_praat.h"
#include "espeakdata_FileInMemory.h"

#if 0
static integer Table_getRownumberOfStringInColumn (Table me, conststring32 string, integer icol) {
	integer row = 0;
	if (icol > 0 && icol <= my numberOfColumns) {
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			if (Melder_equ (myRow -> cells [icol]. string.get(), string)) {
				return irow;
			}
		}
	}
	return row;
}
#endif

FileInMemorySet theEspeakPraatFileInMemorySet() {
	static autoFileInMemorySet singleton;
	if (! singleton)
		singleton = create_espeak_all_FileInMemorySet ();
	return singleton.get();
}

void espeakdata_praat_init () {
	try {
		(void) theEspeakPraatFileInMemorySet();   // create the singleton now (not strictly necessary)
		espeakdata_languages_propertiesTable = Table_createAsEspeakLanguagesProperties ();
		espeakdata_voices_propertiesTable = Table_createAsEspeakVoicesProperties ();
		espeakdata_languages_names = Table_column_to_Strings (espeakdata_languages_propertiesTable.get(), 2);
		espeakdata_voices_names = Table_column_to_Strings (espeakdata_voices_propertiesTable.get(), 2);
		constexpr int test = 1;
		//if (* ((char *) & test) != 1)   // simple endian test
			espeak_ng_data_to_bigendian ();
	} catch (MelderError) {
		Melder_throw (U"Espeakdata initialization not performed.");
	}
}

#define ESPEAK_ISSPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')

// imitates fgets_strip for file in memory
const char * espeakdata_get_voicedata (const char *data, integer ndata, char *buf, integer nbuf, integer *index) {
	if (ndata <= 0 || nbuf <= 0 || *index >= ndata)
		return 0;

	integer i = 0;
	while (i < nbuf && *index < ndata && ((buf [i] = data [i]) != '\n')) {
		i ++;
		(*index) ++;
	}
	(*index) ++;   // ppgb 20151020 fix
	const integer idata = i + 1;
	buf [i] = '\0';
	while (-- i >= 0 && ESPEAK_ISSPACE (buf [i]))
		buf [i] = 0;

	char *p = strstr (buf, "//");
	if (p)
		*p = '\0';
	return & data [idata];
}


static conststring32 get_wordAfterPrecursor_u8 (constvector<unsigned char> const& text8, conststring32 precursor) {
	static char32 word [100];
	/*
		1. Find (first occurrence of) 'precursor' at the start of a line (with optional leading whitespace).
		2. Get the words after 'precursor' (skip leading and trailing whitespace).
	*/
	autoMelderString regex;
	const conststring32 text = Melder_peek8to32 (reinterpret_cast<const char *> (text8.asArgumentToFunctionThatExpectsZeroBasedArray()));
	MelderString_append (& regex, U"^\\s*", precursor, U"\\s+");
	char32 *p = nullptr;
	const char32 *pmatch = strstr_regexp (text, regex.string);
	if (pmatch) {
		pmatch += Melder_length (precursor); // skip 'precursor'
		while (*pmatch == U' ' || *pmatch == U'\t')
			pmatch ++; // skip whitespace after 'precursor'
		p = word;
		char32 *p_end = p + 99;
		while ((*p = *pmatch ++) && *p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\r' && p < p_end)
			p ++;
		*p = U'\0';
		p = word;
	}
	return p;
}

static conststring32 get_stringAfterPrecursor_u8 (constvector<unsigned char> const& text8, conststring32 precursor) {
	static char32 word [100];
	/*
		1. Find (first occurrence of) 'precursor' at the start of a line (with optional leading whitespace).
		2. Get the words after 'precursor' (skip leading and trailing whitespace).
	*/
	autoMelderString regex;
	const conststring32 text = Melder_peek8to32 (reinterpret_cast<const char *> (text8.asArgumentToFunctionThatExpectsZeroBasedArray()));
	//const conststring32 text = Melder_peek8to32 ((const char *) & (text8.cells[1]));
	MelderString_append (& regex, U"^\\s*", precursor, U"\\s+");
	char32 *p = nullptr;
	const char32 *pmatch = strstr_regexp (text, regex.string);
	if (pmatch) {
		pmatch += Melder_length (precursor); // skip 'precursor'
		while (*pmatch == U' ' || *pmatch == U'\t')
			pmatch ++; // skip whitespace after 'precursor'
		//pmatch --;
		p = word;
		char32 *p_end = p + 99;
		// also discard text after comment '//'
		while ((*p = *pmatch ++) && *p != U'\n' && *p != U'\r' && *p != U'/' && *(p+1) != U'/' && p < p_end)
			p ++; // copy to end of line
		while (*p == U' ' || *p == U'\t' || *p == U'\n' || *p == U'\r')
			p --; // remove trailing white space
		*(++ p) = U'\0';
		p = word;
	}
	return p;
}

autoTable Table_createAsEspeakVoicesProperties () {
	try {
		constexpr conststring32 criterion = U"/voices/!v/";
		FileInMemorySet me = theEspeakPraatFileInMemorySet();
		const integer numberOfMatches = FileInMemorySet_findNumberOfMatches_path (me, kMelder_string :: CONTAINS, criterion);
		const conststring32 columnNames [] = { U"id", U"name", U"index", U"gender", U"age", U"variant" };
		autoTable thee = Table_createWithColumnNames (numberOfMatches, ARRAY_TO_STRVEC (columnNames));
		integer irow = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), kMelder_string :: CONTAINS, criterion, true)) {
				irow ++;
				Table_setStringValue (thee.get(), irow, 1, str32rchr (fim -> string.get(), U'/') + 1);
				const char32 *name = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"name");
				// The first character of name must be upper case
				if (name) {
					autoMelderString capitalFirst;
					MelderString_copy (& capitalFirst, name); // we cannot modify original
					capitalFirst.string [0] = Melder_toUpperCase (name [0]);
					Table_setStringValue (thee.get(), irow, 2, capitalFirst.string);
				} else {
					Table_setStringValue (thee.get(), irow, 2, str32rchr (fim -> string.get(), U'/') + 1);
				}
				Table_setNumericValue (thee.get(), irow, 3, ifile);
				conststring32 word = get_wordAfterPrecursor_u8 (fim -> d_data.get(), U"gender");
				Table_setStringValue (thee.get(), irow, 4, (word ? word : U"0"));
				word = get_wordAfterPrecursor_u8 (fim -> d_data.get(), U"age");
				Table_setStringValue (thee.get(), irow, 5, (word ? word : U"0"));
				word = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"variant");
				Table_setStringValue (thee.get(), irow, 6, (word ? word : U"0"));
			}
		}
		Melder_assert (irow == numberOfMatches);
		Table_sortRows (thee.get(),
				autoSTRVEC ({ U"name" }).get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Table with espeak-ng voice properties not created.");
	}
}

autoTable Table_createAsEspeakLanguagesProperties () {
	try {
		constexpr conststring32 criterion = U"./data/lang/";   // 12 characters
		FileInMemorySet me = theEspeakPraatFileInMemorySet();
		const integer numberOfMatches = FileInMemorySet_findNumberOfMatches_path (me, kMelder_string :: CONTAINS, criterion);
		const conststring32 columnNames [] = { U"id", U"name", U"index" };
		autoTable thee = Table_createWithColumnNames (numberOfMatches, ARRAY_TO_STRVEC (columnNames)); // old: Default English
		integer irow = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), kMelder_string :: CONTAINS, criterion, true)) {
				irow ++;
				Table_setStringValue (thee.get(), irow, 1, & fim -> string [12]);
				const char32 *word = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"name");
				Table_setStringValue (thee.get(), irow, 2, ( word ? word : & fim -> string[12] ));
				Table_setNumericValue (thee.get(), irow, 3, ifile);
			}
		}
		Melder_assert (irow == numberOfMatches);
		Table_sortRows (thee.get(),
				autoSTRVEC ({ U"name" }).get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Table with espeak-ng languages not created.");
	}
}

void espeakdata_getIndices (conststring32 language_string, conststring32 voice_string, int *p_languageIndex, int *p_voiceIndex) {
	if (p_languageIndex) {
		integer languageIndex = Strings_findString (espeakdata_languages_names.get(), language_string);
		if (languageIndex == 0) {
			if (Melder_equ (language_string, U"Default") || Melder_equ (language_string, U"English")) {
				languageIndex = Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)");
				Melder_warning (U"Language \"", language_string, U"\" is deprecated. Please use \"",
						espeakdata_languages_names -> strings [languageIndex].get(), U"\".");
			} else {
				languageIndex = Table_searchColumn (espeakdata_languages_propertiesTable.get(), 1, language_string);
				if (languageIndex == 0)
					Melder_throw (U"Language \"", language_string, U"\" is not a valid option.");
			}
		}
		*p_languageIndex = languageIndex;
	}
	if (p_voiceIndex) {
		integer voiceIndex = Strings_findString (espeakdata_voices_names.get(), voice_string);
		*p_voiceIndex = voiceIndex;
		if (voiceIndex == 0) {
			if (Melder_equ (voice_string, U"default")) {
				voiceIndex = Strings_findString (espeakdata_voices_names.get(), U"Male1");
			} else if (Melder_equ (voice_string, U"f1")) {
				voiceIndex = Strings_findString (espeakdata_voices_names.get(), U"Female1");
			} else {
				// Try the bare file names
				voiceIndex = Table_searchColumn (espeakdata_voices_propertiesTable.get(), 1, voice_string);
				if (voiceIndex == 0) {
					Melder_throw (U"Voice variant ", voice_string, U" is not a valid option.");
				}
			}
		}
		if (voiceIndex != *p_voiceIndex) {
			*p_voiceIndex = voiceIndex;
			Melder_casual (U"Voice \"", voice_string, U"\" is deprecated. Please use \"",
					espeakdata_voices_names -> strings [*p_voiceIndex].get(), U"\".");
		} else {
			// unknown voice, handled by interface
		}
	}
}

/* End of file espeakdata_FileInMemory.cpp */
