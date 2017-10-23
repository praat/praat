/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2013, 2015-2016 David Weenink, 2017 Paul Boersma
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

/*
void structFileInMemory :: v_copy (Daata thee_Daata) {
	FileInMemory thee = static_cast <FileInMemory> (thee_Daata);
	our FileInMemory_Parent :: v_copy (thee);
	thy d_path = Melder_dup (our d_path);
	thy d_id = Melder_dup (our d_id);
	thy d_numberOfBytes = our d_numberOfBytes;
	thy ownData = our ownData;
	thy d_data = NUMvector<char> (0, our d_numberOfBytes);
	memcpy (thy d_data, our d_data, our d_numberOfBytes + 1);
}

void structFileInMemory :: v_destroy () noexcept {
	Melder_free (our d_path);
	Melder_free (our d_id);
	if (our ownData)
		NUMvector_free <char> (our d_data, 0);
	our FileInMemory_Parent :: v_destroy ();
}
*/

void structFileInMemory :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"File name: ", our d_path);
	MelderInfo_writeLine (U"Id: ", our d_id);
	MelderInfo_writeLine (U"Number of bytes: ", our d_numberOfBytes);
}

autoFileInMemory FileInMemory_create (MelderFile file) {
	try {
		if (! MelderFile_readable (file)) {
			Melder_throw (U"File not readable.");
		}
		integer length = MelderFile_length (file);
		if (length <= 0) {
			Melder_throw (U"File is empty.");
		}
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (file -> path);
		my d_id = Melder_dup (MelderFile_name (file));
		my d_numberOfBytes = length;
		my ownData = true;
		my d_data = NUMvector <unsigned char> (0, my d_numberOfBytes);   // includes room for a final null byte in case the file happens to contain text
		MelderFile_open (file);
		for (integer i = 0; i < my d_numberOfBytes; i++) {
			unsigned int number = bingetu8 (file -> filePointer);
			my d_data[i] = number;
		}
		my d_data[my d_numberOfBytes] = 0;   // one extra
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from \"", Melder_fileToPath (file), U"\".");
	}
}

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, const char32 *path, const char32 *id) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (path);
		my d_id = Melder_dup (id);
		my d_numberOfBytes = numberOfBytes;
		my ownData = false;
		my d_data =  NUMvector <unsigned char> (0L, numberOfBytes);
		NUMvector_copyElements <unsigned char> (reinterpret_cast<unsigned char *> (const_cast<char *> (data)), my d_data, 0L, numberOfBytes);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not create from data.");
	}
}

void FileInMemory_setId (FileInMemory me, const char32 *newId) {
	Melder_free (my d_id);
	my d_id = Melder_dup (newId);
}

void FileInMemory_showAsCode (FileInMemory me, const char32 *name, integer numberOfBytesPerLine)
{
	if (numberOfBytesPerLine <= 0) {
		numberOfBytesPerLine = 20;
	}
	MelderInfo_writeLine (U"\t\tstatic unsigned char ", name, U"_data[", my d_numberOfBytes+1, U"] = {");
	for (integer i = 0; i < my d_numberOfBytes; i++) {
		unsigned char number = my d_data[i];
		MelderInfo_write ((i % numberOfBytesPerLine == 0 ? U"\t\t\t" : U""), number, U",",
			((i % numberOfBytesPerLine == (numberOfBytesPerLine - 1)) ? U"\n" : U" "));
	}
	MelderInfo_writeLine ((my d_numberOfBytes - 1) % numberOfBytesPerLine == (numberOfBytesPerLine - 1) ? U"\t\t\t0};" : U"0};");
	MelderInfo_write (U"\t\tautoFileInMemory ", name, U" = FileInMemory_createWithData (");
	MelderInfo_writeLine (my d_numberOfBytes, U", reinterpret_cast<const char *> (&", name, U"_data), \n\t\t\tU\"", my d_path, U"\", \n\t\t\tU\"", my d_id, U"\");");
}

/* End of file FileInMemory.cpp */
