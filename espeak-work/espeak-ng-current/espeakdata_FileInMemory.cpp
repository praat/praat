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

#include "espeak_ng.h"
#include "speech.h"
#include <wctype.h>

#include "espeakdata_FileInMemory.h"

autoTable Table_createAsEspeakLanguagesIdAndNamePairs ();
autoStrings Table_column_to_Strings (Table me, integer column);


autoFileInMemorySet espeakdata_languages;
autoFileInMemorySet espeakdata_dicts;
autoFileInMemorySet espeakdata_phons;
autoFileInMemorySet espeakdata_voices;
autoTable espeakdata_languages_idAndNameTable;
autoStrings espeakdata_voices_names;
autoStrings espeakdata_languages_names;

static void FileInMemorySet_and_Strings_changeIds (FileInMemorySet me, Strings thee) {
	try {
		if (my size != thy numberOfStrings) return; // do nothing
		for (integer i = 1; i <= my size; i ++) {
			FileInMemory_setId (my at [i], thy strings [i]);
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
		espeakdata_dicts = create_espeakdata_dicts ();
		espeakdata_languages = create_espeakdata_languages ();
		espeakdata_voices = create_espeakdata_voices ();
		espeakdata_phons = create_espeakdata_phons ();
		espeakdata_languages_idAndNameTable = Table_createAsEspeakLanguagesIdAndNamePairs ();
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

autoTable Table_createAsEspeakLanguagesIdAndNamePairs () {
	try {
		FileInMemorySet me = espeakdata_languages.get();
		autoTable thee = Table_createWithColumnNames (my size, U"id name");
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = my at [ifile];
			Table_setStringValue (thee.get(), ifile, 1, fim -> d_id);
			const char *p = strstr (fim -> d_data, "name");
			if (p) {
			// copy the name part to the following new line
			char buf [100], *bufp = buf;
			integer len = 0;
			while ((*bufp ++ = *p ++) != '\n' && len < 99) { len ++; }
			// remove trailing white space
			*bufp = 0;
			while (ESPEAK_ISSPACE (buf[len]) && len > 0) {
				buf [len] = 0;
				len --;
			}
			// skip leading white space after "name"
			bufp = & buf[4];
			while (ESPEAK_ISSPACE (*bufp)) { bufp ++; }
			
			Table_setStringValue (thee.get(), ifile, 2, Melder_peek8to32 (bufp));
			TableRow row = thy rows.at [ifile];
			} else {
				// probably an error.
				Table_setStringValue (thee.get(), ifile, 2, fim -> d_id);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Espeakdata-languages: language table not initialized.");
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
