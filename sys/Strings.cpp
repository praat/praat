/* Strings.cpp
 *
 * Copyright (C) 1992-2008,2011-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Strings_.h"
#include "../kar/longchar.h"

#include "oo_DESTROY.h"
#include "Strings_def.h"
#include "oo_COPY.h"
#include "Strings_def.h"
#include "oo_EQUAL.h"
#include "Strings_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Strings_def.h"
#include "oo_WRITE_TEXT.h"
#include "Strings_def.h"
#include "oo_READ_TEXT.h"
#include "Strings_def.h"
#include "oo_WRITE_BINARY.h"
#include "Strings_def.h"
#include "oo_READ_BINARY.h"
#include "Strings_def.h"
#include "oo_DESCRIPTION.h"
#include "Strings_def.h"

Thing_implement (Strings, Daata, 0);

static double Strings_minimumLength (Strings me) {
	return NUMminimumLength (my strings.get());
}
static double Strings_maximumLength (Strings me) {
	return NUMmaximumLength (my strings.get());
}
static double Strings_totalLength (Strings me) {
	return NUMtotalLength (my strings.get());
}

void structStrings :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of strings: ", our numberOfStrings);
	MelderInfo_writeLine (U"Total length: ", Strings_totalLength (this), U" characters");
	MelderInfo_writeLine (U"Shortest string: ", Strings_minimumLength (this), U" characters");
	MelderInfo_writeLine (U"Longest string: ", Strings_maximumLength (this), U" characters");
}

conststring32 structStrings :: v_getVectorStr (integer icol) {
	if (icol < 1 || icol > our numberOfStrings)
		return U"";
	char32 *stringValue = strings [icol].get();
	return stringValue ? stringValue : U"";
}

autoStrings Strings_createAsFileList (conststring32 path /* cattable */) {
	try {
		autoStrings me = Thing_new (Strings);
		my strings = fileNames_STRVEC (path);
		my maintainInvariants ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings object not created as file list.");
	}
}

autoStrings Strings_createAsFolderList (conststring32 path /* cattable */) {
	try {
		autoStrings me = Thing_new (Strings);
		my strings = folderNames_STRVEC (path);
		my maintainInvariants ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings object not created as folder list.");
	}
}

autoStrings Strings_readFromRawTextFile (MelderFile file) {
	try {
		autoStrings me = Thing_new (Strings);
		my strings = readLinesFromFile_STRVEC (file);
		my maintainInvariants ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings not read from raw text file ", file, U".");
	}
}

void Strings_writeToRawTextFile (Strings me, MelderFile file) {
	try {
		my assertInvariants ();
		autoMelderString buffer;
		for (integer i = 1; i <= my numberOfStrings; i ++)
			MelderString_append (& buffer, my strings [i].get(), U"\n");
		MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
	} catch (MelderError) {
		Melder_throw (U"Strings not written to raw text file ", file, U".");
	}
}

void Strings_randomize (Strings me) {
	for (integer i = 1; i < my numberOfStrings; i ++) {
		integer other = NUMrandomInteger (i, my numberOfStrings);
		std::swap (my strings [other], my strings [i]);
	}
}

void Strings_genericize (Strings me) {
	if (my numberOfStrings == 0)
		return;
	autostring32 buffer (Melder_iround (Strings_maximumLength (me)) * 3);
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		const conststring32 string = my strings [i].get();
		const char32 *p = & string [0];
		while (*p) {
			if (*p > 126) {   // backslashes are not converted, i.e. genericize^2 == genericize
				Longchar_genericize (string, buffer.get());
				my strings [i] = Melder_dup (buffer.get());
				break;
			}
			p ++;
		}
	}
}

void Strings_nativize (Strings me) {
	if (my numberOfStrings == 0)
		return;
	autostring32 buffer (Melder_iround (Strings_maximumLength (me)));
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		Longchar_nativize (my strings [i].get(), buffer.get(), false);
		my strings [i] = Melder_dup (buffer.get());
	}
}

void Strings_sort (Strings me) {
	sort_STRVEC_inout (my strings.get());
}

void Strings_remove (Strings me, integer position) {
	try {
		my checkStringNumber (position);
		my strings. remove (position);
		my maintainInvariants ();
	} catch (MelderError) {
		Melder_throw (me, U": string ", position, U" not removed.");
	}
}

void Strings_replace (Strings me, integer stringNumber, conststring32 text) {
	try {
		my checkStringNumber (stringNumber);
		if (Melder_equ (my strings [stringNumber].get(), text))
			return;   // nothing to change
		/*
			Create without change.
		*/
		autostring32 newString = Melder_dup (text);
		/*
			Change without error.
		*/
		my strings [stringNumber] = newString. move();
	} catch (MelderError) {
		Melder_throw (me, U": string ", stringNumber, U" not replaced.");
	}
}

void Strings_insert (Strings me, integer position, conststring32 text) {
	try {
		if (position == 0)
			position = my numberOfStrings + 1;
		Melder_require (position >= 1,
			U"The element number should be at least 1, not ", position, U".");
		Melder_require (position <= my numberOfStrings + 1,
			U"The element number should be at most the number of elements plus 1 (", my numberOfStrings + 1, U"), not", position, U".");
		my strings. insert (position, text);   // size changes only on success
		my maintainInvariants ();
	} catch (MelderError) {
		Melder_throw (me, U": no string inserted at position ", position, U".");
	}
}

/* End of file Strings.cpp */
