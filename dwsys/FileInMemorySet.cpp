/* FileInMemorySet.cpp
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

#include "FileInMemorySet.h"

#include "oo_DESTROY.h"
#include "FileInMemorySet_def.h"
#include "oo_COPY.h"
#include "FileInMemorySet_def.h"
#include "oo_EQUAL.h"
#include "FileInMemorySet_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FileInMemorySet_def.h"
#include "oo_WRITE_TEXT.h"
#include "FileInMemorySet_def.h"
#include "oo_READ_TEXT.h"
#include "FileInMemorySet_def.h"
#include "oo_WRITE_BINARY.h"
#include "FileInMemorySet_def.h"
#include "oo_READ_BINARY.h"
#include "FileInMemorySet_def.h"
#include "oo_DESCRIPTION.h"
#include "FileInMemorySet_def.h"


Thing_implement (FileInMemorySet, SortedSet, 0);

void structFileInMemorySet :: v_info () {
	FileInMemorySet_Parent :: v_info ();
	MelderInfo_writeLine (U"Number of files: ", size);
	MelderInfo_writeLine (U"Total number of bytes: ", FileInMemorySet_getTotalNumberOfBytes (this));
}

integer FileInMemorySet_getTotalNumberOfBytes (FileInMemorySet me) {
	integer numberOfBytes = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = (FileInMemory) my at [ifile];
		numberOfBytes += fim -> d_numberOfBytes;
	}
	return numberOfBytes;
}

autoFileInMemorySet FileInMemorySet_create () {
	try {
		autoFileInMemorySet me = Thing_new (FileInMemorySet);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created.");
	}
}

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list) {
	try {
		autoFileInMemorySet thee = Data_copy (list.at[1]);
		for (integer iset = 1; iset <= list.size; iset ++)
			thy merge (list.at [iset]);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySets not merged.");
	}
}

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (conststring32 dirpath, conststring32 fileGlobber) {
	try {
		structMelderDir parent { };
		Melder_pathToDir (dirpath, & parent);
		autoStrings thee = Strings_createAsFileList (Melder_cat (dirpath, U"/", fileGlobber));
		Melder_require (thy numberOfStrings > 0,
			U"No files found.");

		autoFileInMemorySet me = FileInMemorySet_create ();
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			structMelderFile file { };
			MelderDir_getFile (& parent, thy strings [i].get(), & file);
			autoFileInMemory fim = FileInMemory_create (& file);
			my addItem_move (fim.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created from directory \"", dirpath, U"\" for files that match \"", fileGlobber, U"\".");
	}
}

autoFileInMemorySet FilesInMemory_to_FileInMemorySet (OrderedOf<structFileInMemory>& list) {
	try {
		autoFileInMemorySet thee = FileInMemorySet_create ();
		for (integer ifile = 1; ifile <= list.size; ifile ++) {
			autoFileInMemory fim = Data_copy (list.at [ifile]);
			thy addItem_move (fim.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FilesInMemory not collected in FileInMemorySet.");
	}
	
}

autoFileInMemorySet FileInMemorySet_extractFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = static_cast <FileInMemory> (my at [ifile]);
			if (Melder_stringMatchesCriterion (fim -> d_path.get(), which, criterion, true)) {
				autoFileInMemory item = Data_copy (fim);
				thy addItem_move (item.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

autoFileInMemorySet FileInMemorySet_listFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = static_cast<FileInMemory> (my at [ifile]);
			if (Melder_stringMatchesCriterion (fim -> d_path.get(), which, criterion, true))
				thy addItem_ref (fim);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

void FileInMemorySet_showAsCode (FileInMemorySet me, conststring32 name, integer numberOfBytesPerLine) {
	autoMelderString one_fim;
	MelderInfo_writeLine (U"#include \"FileInMemorySet.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"autoFileInMemorySet create_", name, U" () {");
	MelderInfo_writeLine (U"\ttry {");
	MelderInfo_writeLine (U"\t\tautoFileInMemorySet me = FileInMemorySet_create ();");
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = (FileInMemory) my at [ifile];
		MelderString_copy (& one_fim, name, ifile);
		FileInMemory_showAsCode (fim, one_fim.string, numberOfBytesPerLine);
		MelderInfo_writeLine (U"\t\tmy addItem_move (", one_fim.string, U".move());\n");
	}
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemorySet not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
}

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, conststring32 name, integer numberOfBytesPerLine)
{
	if (index < 1 || index > my size)
		return;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"static autoFileInMemory create_new_object () {");
	MelderInfo_writeLine (U"\ttry {");
	autoMelderString one_fim;
	const FileInMemory fim = (FileInMemory) my at [index];
	MelderString_append (& one_fim, name, index);
	FileInMemory_showAsCode (fim, U"me", numberOfBytesPerLine);
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
	MelderInfo_writeLine (U"autoFileInMemory ", name, U" = create_new_object ();");
}

integer FileInMemorySet_getIndexFromId (FileInMemorySet me, conststring32 id) {
	integer index = 0;
	for (integer i = 1; i <= my size; i ++) {
		const FileInMemory fim = (FileInMemory) my at [i];
		if (Melder_equ (id, fim -> d_id.get())) {
			index = i;
			break;
		}
	}
	return index;
}

integer FileInMemorySet_lookUp (FileInMemorySet me, conststring32 path) {
	integer index = 0;
	for (integer i = 1; i <= my size; i ++) {
		const FileInMemory fim = (FileInMemory) my at [i];
		if (Melder_equ (path, fim -> d_path.get())) {
			index = i;
			break;
		}
	}
	return index;
}

integer FileInMemorySet_findNumberOfMatches_path (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	integer numberOfMatches = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = static_cast <FileInMemory> (my at [ifile]);
		if (Melder_stringMatchesCriterion (fim -> d_path.get(), which, criterion, true))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

bool FileInMemorySet_hasDirectory (FileInMemorySet me, conststring32 name) {
	bool match = false;
	autoMelderString searchString;
	MelderString_append (& searchString, U"/", name, U"/");
	for (integer i = 1; i <= my size; i ++) {
		const FileInMemory fim = (FileInMemory) my at [i];
		if (str32str (fim -> d_path.get(), searchString.string)) {
			match = true;
			break;
		}
	}
	return match;
}

autoStrings FileInMemorySet_to_Strings_id (FileInMemorySet me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my size);
		thy numberOfStrings = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = (FileInMemory) my at [ifile];
			thy strings [ifile] = Melder_dup_f (fim -> d_id.get());
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Strings created from FilesinMemory.");
	}
}

autovector<unsigned char> FileInMemorySet_getCopyOfData (FileInMemorySet me, conststring32 id) {
	autovector<unsigned char> result;
	integer index = FileInMemorySet_getIndexFromId (me, id);
	if (index != 0) {
		const FileInMemory fim = (FileInMemory) my at [index];
		result = newvectorcopy (fim -> d_data.all());
	}
	return result;
}

const char * FileInMemorySet_getData (FileInMemorySet me, conststring32 id, integer *out_numberOfBytes) {
	if (out_numberOfBytes)
		*out_numberOfBytes = 0;
	const integer index = FileInMemorySet_getIndexFromId (me, id);
	if (index == 0)
		return nullptr;

	const FileInMemory fim = (FileInMemory) my at [index];
	if (out_numberOfBytes)
		*out_numberOfBytes = fim -> d_numberOfBytes;
	return reinterpret_cast<const char *> (fim -> d_data.asArgumentToFunctionThatExpectsZeroBasedArray ());
}


/* End of file FileInMemorySet.cpp */
