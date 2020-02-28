/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2020 David Weenink, 2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FileInMemory.h"
#include "Strings_.h"

#include "oo_DESTROY.h"
#include "FileInMemory_def.h"
#include "oo_COPY.h"
#include "FileInMemory_def.h"
#include "oo_EQUAL.h"
#include "FileInMemory_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_READ_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_READ_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_DESCRIPTION.h"
#include "FileInMemory_def.h"

Thing_implement (FileInMemory, Daata, 0);

void structFileInMemory :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"File name: ", our d_path.get());
	MelderInfo_writeLine (U"Id: ", our d_id.get());
	MelderInfo_writeLine (U"Number of bytes: ", our d_numberOfBytes);
}

autoFileInMemory FileInMemory_create (MelderFile file) {
	try {
		Melder_require (MelderFile_readable (file),
			U"File is not readable.");
		const integer length = MelderFile_length (file);
		Melder_require (length > 0,
			U"File should not be empty.");
		
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (file -> path);
		my d_id = Melder_dup (MelderFile_name (file));
		my d_numberOfBytes = length;
		my _dontOwnData = false;
		my d_data = newvectorzero <byte> (my d_numberOfBytes + 1);   // includes room for a final null byte in case the file happens to contain only text
		MelderFile_open (file);
		for (integer i = 1; i <= my d_numberOfBytes; i++) {
			const unsigned int number = bingetu8 (file -> filePointer);
			my d_data [i] = number;
		}
		my d_data [my d_numberOfBytes + 1] = 0;   // "extra" null byte
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from \"", Melder_fileToPath (file), U"\".");
	}
}

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, bool isStaticData, conststring32 path, conststring32 id) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (path);
		my d_id = Melder_dup (id);
		my d_numberOfBytes = numberOfBytes;
		if (isStaticData) {
			my _dontOwnData = true; // we cannot dispose of the data!
			/*
				djmw 20200226:
				We changed d_data from type vector to autovector and cannot share the data anynmore.
				Therefore make an explicit copy until we find a solution.
			*/
			//my d_data.at = reinterpret_cast<unsigned char *> (const_cast<char *> (data))-1; // ... just a link
			//my d_data.size = numberOfBytes + 1;
			my _dontOwnData = false; // we can dispose of the data!
			my d_data = newvectorraw <unsigned char> (numberOfBytes + 1);
			memcpy (my d_data.asArgumentToFunctionThatExpectsZeroBasedArray (), data, (size_t) numberOfBytes + 1);
		} else {
			my _dontOwnData = false;
			my d_data = newvectorraw <unsigned char> (numberOfBytes + 1);
			memcpy (my d_data.asArgumentToFunctionThatExpectsZeroBasedArray (), data, (size_t) numberOfBytes + 1);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from data.");
	}
}

void FileInMemory_setId (FileInMemory me, conststring32 newId) {
	my d_id = Melder_dup (newId);
}

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine) {
	if (numberOfBytesPerLine < 1)
		numberOfBytesPerLine = 20;

	MelderInfo_writeLine (U"\t\tstatic unsigned char ", name, U"_data[", my d_numberOfBytes+1, U"] = {");
	for (integer i = 0; i < my d_numberOfBytes; i++) {
		const unsigned char number = my d_data [i];
		MelderInfo_write ((i % numberOfBytesPerLine == 0 ? U"\t\t\t" : U""), number, U",",
				i % numberOfBytesPerLine == numberOfBytesPerLine - 1 ? U"\n" : U" ");
	}
	MelderInfo_writeLine ((my d_numberOfBytes - 1) % numberOfBytesPerLine == (numberOfBytesPerLine - 1) ? U"\t\t\t0};" : U"0};");
	MelderInfo_write (U"\t\tautoFileInMemory ", name, U" = FileInMemory_createWithData (");
	MelderInfo_writeLine (my d_numberOfBytes, U", reinterpret_cast<const char *> (&",
		name, U"_data), true, \n\t\t\tU\"", my d_path.get(), U"\", \n\t\t\tU\"", my d_id.get(), U"\");");
}

/* End of file FileInMemory.cpp */
