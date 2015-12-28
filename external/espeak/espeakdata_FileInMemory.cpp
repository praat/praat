/* espeakdata_FileInMemory.cpp
 *
 * Copyright (C) David Weenink 2012, 2015
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

#include "speech.h"
#include <wctype.h>

autoFileInMemorySet espeakdata_variants;
autoFileInMemorySet espeakdata_dicts;
autoFileInMemorySet espeakdata_phons;
autoFileInMemorySet espeakdata_voices;
autoStrings espeakdata_voices_names;
autoStrings espeakdata_voices_names_short;
autoStrings espeakdata_variants_names;

static void FileInMemorySet_and_Strings_changeIds (FileInMemorySet me, Strings thee) {
	try {
		if (my size() != thy numberOfStrings) return; // do nothing
		for (long i = 1; i <= my size(); i ++) {
			FileInMemory_setId (my _item [i], thy strings [i]);
		}
	} catch (MelderError) {
		Melder_throw (me, U"Ids not changed.");
	}
}

static autoStrings espeak_voices_sort () {
	try {
		autoTable names = espeakdata_voices_to_Table (espeakdata_voices.get());
		autoStrings fullnames = espeakdata_voices_getNames (names.peek(), 2);
		FileInMemorySet_and_Strings_changeIds (espeakdata_voices.get(), fullnames.peek());
		espeakdata_voices -> d_sortKey = 1;   // sort id's
		espeakdata_voices -> sort ();
		Table_sortRows_string (names.peek(), U"name"); //They hopefully sort the same way
		autoStrings neworder = espeakdata_voices_getNames (names.peek(), 2);
		autoStrings names_short = espeakdata_voices_getNames (names.peek(), 1);
		FilesInMemory_and_Strings_changeIds (espeakdata_voices.get(), names_short.peek());
		return neworder;
	} catch (MelderError) {
		Melder_throw (U"Espeak voices not sorted.");
	}
}

static autoStrings Strings_insertAndExpand (Strings me, long position, const char32 *newstring) {
	try {
		if (position == 0) position = my numberOfStrings + 1;
		Melder_assert (position >= 1);
		Melder_assert (position <= my numberOfStrings + 1);
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<char32 *> (1, my numberOfStrings + 1);
		for (long i = 1, from = 1; i <= my numberOfStrings + 1; i++, from++) {
			const char32 *to_copy = my strings[from];
			if (i == position) {
				to_copy = newstring; from--;
			}
			thy strings[i] = Melder_dup (to_copy);
		}
		thy numberOfStrings = my numberOfStrings + 1;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" not expanded.");
	}
}

void espeakdata_praat_init () {
	try {
		espeakdata_variants = create_espeakdata_variants ();
		autoStrings vnames = FilesInMemory_to_Strings_id (espeakdata_variants.get());
		espeakdata_variants_names = Strings_insertAndExpand (vnames.peek(), 1, U"default");
		espeakdata_dicts = create_espeakdata_dicts ();
		espeakdata_phons = create_espeakdata_phons ();
		espeakdata_voices = create_espeakdata_voices ();
		espeakdata_voices_names = espeak_voices_sort ();
		autoTable names_table = espeakdata_voices_to_Table (espeakdata_voices.get());
	} catch (MelderError) {
		Melder_throw (U"Espeakdata initialization not performed.");
	}
}

#define ESPEAK_ISSPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')

// imitates fgets_strip for file in memory
const char * espeakdata_get_voicedata (const char *data, long ndata, char *buf, long nbuf, long *index) {
	if (ndata <= 0 || nbuf <= 0 || *index >= ndata) {
		return 0;
	}
	long i = 0;
	while (i < nbuf && *index < ndata && ((buf[i] = data[i]) != '\n')) {
		i++; (*index)++;
	}
	(*index)++;   // ppgb 20151020 fix
	long idata = i + 1;
	buf[i] = '\0';
	while (--i >= 0 && ESPEAK_ISSPACE (buf[i])) {
		buf[i] = 0;
	}
	char *p = strstr (buf, "//");
	if (p != 0) {
		*p = 0;
	}
	return &data[idata];
}

autoTable espeakdata_voices_to_Table (FilesInMemory me) {
	try {
		autoTable thee = Table_createWithColumnNames (my size, U"id name");
		for (long ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = (FileInMemory) my item [ifile];
			Table_setStringValue (thee.peek(), ifile, 1, fim -> d_id);
			const char *p = strstr (fim -> d_data, "name");
			if (p == NULL) continue;
			// copy the name part to the following new line
			char buf[40], *bufp = buf;
			long len = 0;
			while ((*bufp ++ = *p ++) != '\n' && len < 39) { len ++; }
			// remove trailing white space
			*bufp = 0;
			while (ESPEAK_ISSPACE (buf[len]) && len > 0) {
				buf[len] = 0; len--;
			}
			// skip leading white space
			bufp = & buf[4];
			while (ESPEAK_ISSPACE (*bufp)) { *bufp ++; }
			Table_setStringValue (thee.peek(), ifile, 2, Melder_peek8to32 (bufp));
			TableRow row = thy rows [ifile];
			wint_t c0 = row -> cells [2]. string [0];
			row -> cells [2]. string [0] = towupper (c0);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Espeakdata: voice table not initialized.");
	}
}

autoStrings espeakdata_voices_getNames (Table me, long column) {
	try {
		if (column < 0 || column > 2) {
			Melder_throw (U"Illegal columnn.");
		}
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, my rows.size());
		thy numberOfStrings = 0;
		for (long irow = 1; irow <= my rows.size(); irow ++) {
			thy strings [irow] = Melder_dup (Table_getStringValue_Assert (me, irow, column));
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Espeakdata: voices not initialized.");
	}
}

char * espeakdata_get_dict_data (const char *name, unsigned int *size) {
	long lsize;
	char *data = FilesInMemory_getCopyOfData (espeakdata_dicts.get(), Melder_peek8to32 (name), &lsize);
	*size = (unsigned int) lsize;
	return data;
}


const char * espeakdata_get_voice (const char *vname, long *numberOfBytes) {
	return FilesInMemory_getData (espeakdata_voices.get(), Melder_peek8to32 (vname), numberOfBytes);
}

const char * espeakdata_get_voiceVariant (const char *vname, long *numberOfBytes) {
	char *plus = strstr ((char *) vname, "+"); // prototype says: strstr (const char *, const char *)
	const char *name = plus != NULL ? ++plus : vname;
	return FilesInMemory_getData (espeakdata_variants.get(), Melder_peek8to32 (name), numberOfBytes);;
}

/* End of file espeakdata_FileInMemory.cpp */
