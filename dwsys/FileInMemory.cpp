/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2013, 2015 David Weenink
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

#include "FileInMemory.h"
#include "Strings_.h"

Thing_implement (FileInMemory, Daata, 0);

void structFileInMemory :: v_copy (thou) {
	thouart (FileInMemory);
	FileInMemory_Parent :: v_copy (thee);
	thy d_path = Melder_dup (d_path);
	thy d_id = Melder_dup (d_id);
	thy d_numberOfBytes = d_numberOfBytes;
	thy d_data = NUMvector<char> (0, d_numberOfBytes);
	memcpy (thy d_data, d_data, d_numberOfBytes+1);
}

void structFileInMemory :: v_destroy () {
	Melder_free (d_path);
	Melder_free (d_id);
	NUMvector_free <char>  (d_data, 0);
	FileInMemory_Parent :: v_destroy ();
}

void structFileInMemory :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"File name: ", d_path);
	MelderInfo_writeLine (U"Id: ", d_id);
	MelderInfo_writeLine (U"Number of bytes: ", d_numberOfBytes);
}

FileInMemory FileInMemory_create (MelderFile file) {
	try {
		if (! MelderFile_readable (file)) {
			Melder_throw (U"File not readable.");
		}
		long length = MelderFile_length (file);
		if (length <= 0) {
			Melder_throw (U"File is empty.");
		}
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (file -> path);
		my d_id = Melder_dup (MelderFile_name (file));
		my d_numberOfBytes = length;
		my d_data = NUMvector <char> (0, my d_numberOfBytes); // one extra for 0-byte at end if text
		MelderFile_open (file);
		for (long i = 0; i < my d_numberOfBytes; i++) {
			unsigned int number = bingetu1 (file -> filePointer);
			my d_data[i] = number;
		}
		my d_data[my d_numberOfBytes] = 0; // one extra
		MelderFile_close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from \"", Melder_fileToPath (file), U"\".");
	}
}

FileInMemory FileInMemory_createWithData (long numberOfBytes, const char *data, const char32 *path, const char32 *id) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_dup (path);
		my d_id = Melder_dup (id);
		my d_numberOfBytes = numberOfBytes;
		my d_data = const_cast<char *> (data); // copy pointer to data only
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not create from data.");
	}
}

void FileInMemory_setId (FileInMemory me, const char32 *newId) {
	Melder_free (my d_id);
	my d_id = Melder_dup (newId);
}

void FileInMemory_showAsCode (FileInMemory me, const char32 *name, long numberOfBytesPerLine)
{
	if (numberOfBytesPerLine <= 0) numberOfBytesPerLine = 20;
	// autoNUMvector<unsigned char> data (0, my d_numberOfBytes); ????
	MelderInfo_writeLine (U"\t\tstatic unsigned char ", name, U"_data[", my d_numberOfBytes+1, U"] = {");
	for (long i = 0; i < my d_numberOfBytes; i++) {
		unsigned char number = my d_data[i];
		MelderInfo_write ((i % numberOfBytesPerLine == 0 ? U"\t\t\t" : U""), number, U",",
			((i % numberOfBytesPerLine  == (numberOfBytesPerLine - 1)) ? U"\n" : U" "));
	}
	MelderInfo_writeLine ((my d_numberOfBytes - 1) % numberOfBytesPerLine == (numberOfBytesPerLine - 1) ? U"\t\t\t0};" : U"0};");
	MelderInfo_write (U"\t\tautoFileInMemory ", name, U" = FileInMemory_createWithData (");
	MelderInfo_writeLine (my d_numberOfBytes, U", reinterpret_cast<const char *> (&", name, U"_data), \n\t\t\tU\"", my d_path, U"\", \n\t\t\tU\"", my d_id, U"\");");
}

Thing_implement (FilesInMemory, SortedSet, 0);

int structFilesInMemory :: s_compare_name (I, thou) {
	iam (FileInMemory); thouart (FileInMemory);
	return Melder_cmp (my d_path, thy d_path);
}

int structFilesInMemory :: s_compare_id (I, thou) {
	iam (FileInMemory); thouart (FileInMemory);
	return Melder_cmp (my d_id, thy d_id);
}

FilesInMemory FilesInMemory_create () {
	try {
		autoFilesInMemory me = Thing_new (FilesInMemory);
		Collection_init (me.peek(), classFileInMemory, 30);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"FilesInMemory not created.");
	}
}

FilesInMemory FilesInMemory_createFromDirectoryContents (const char32 *dirpath, const char32 *fileGlobber) {
	try {
		structMelderDir parent = { { 0 } };
		Melder_pathToDir (dirpath, &parent);
		autoStrings thee = Strings_createAsFileList (Melder_cat (dirpath, U"/", fileGlobber));
		if (thy numberOfStrings < 1) {
			Melder_throw (U"No files found.");
		}
		autoFilesInMemory me = FilesInMemory_create ();
		for (long i = 1; i <= thy numberOfStrings; i++) {
			structMelderFile file = { 0 };
			MelderDir_getFile (&parent, thy strings[i], &file);
			autoFileInMemory fim = FileInMemory_create (&file);
			Collection_addItem (me.peek(), fim.transfer());
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"FilesInMemory not created from directory \"", dirpath, U"\" for files that match \"",
		fileGlobber, U"\".");
	}
}

void FilesInMemory_showAsCode (FilesInMemory me, const char32 *name, long numberOfBytesPerLine) {
	autoMelderString one_fim;
	MelderInfo_writeLine (U"#include \"Collection.h\"");
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"FilesInMemory create_", name, U" () {");
	MelderInfo_writeLine (U"\ttry {");
	MelderInfo_writeLine (U"\t\tautoFilesInMemory me = FilesInMemory_create ();");
	for (long ifile = 1; ifile <= my size; ifile++) {
		FileInMemory fim = (FileInMemory) my item[ifile];
		MelderString_copy (&one_fim, name, ifile);
		FileInMemory_showAsCode (fim, one_fim.string, numberOfBytesPerLine);
		MelderInfo_writeLine (U"\t\tCollection_addItem (me.peek(), ", one_fim.string, U".transfer());\n");
	}
	MelderInfo_writeLine (U"\t\treturn me.transfer();");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (L\"FilesInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
}

void FilesInMemory_showOneFileAsCode (FilesInMemory me, long index, const char32 *name, long numberOfBytesPerLine)
{
	if (index < 1 || index > my size) return;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"static FileInMemory create_new_object () {");
	MelderInfo_writeLine (U"\ttry {");
	autoMelderString one_fim;
	FileInMemory fim = (FileInMemory) my item[index];
	MelderString_append (&one_fim, name, index);
	FileInMemory_showAsCode (fim, U"me", numberOfBytesPerLine);
	MelderInfo_writeLine (U"\t\treturn me.transfer();");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (L\"FileInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
	MelderInfo_writeLine (U"FileInMemory ", name, U" = create_new_object ();");
}

long FilesInMemory_getIndexFromId (FilesInMemory me, const char32 *id) {
	long index = 0;
	for (long i = 1; i <= my size; i++) {
		FileInMemory fim = (FileInMemory) my item[i];
		if (Melder_cmp (id, fim -> d_id) == 0) {
			index = i; break;
		}
	}
	return index;
}

Strings FilesInMemory_to_Strings_id (FilesInMemory me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, my size);
		thy numberOfStrings = 0;
		for (long ifile = 1; ifile <= my size; ifile++) {
			FileInMemory fim = (FileInMemory) my item[ifile];
			thy strings[ifile] = Melder_dup_f (fim -> d_id);
			thy numberOfStrings++;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (U"No Strings created from FilesinMemory.");
	}
}

char * FilesInMemory_getCopyOfData (FilesInMemory me, const char32 *id, long *numberOfBytes) {
	*numberOfBytes = 0;
	long index = FilesInMemory_getIndexFromId (me, id);
	if (index == 0) return 0;
	FileInMemory fim = (FileInMemory) my item[index];
	char *data = (char *) _Melder_malloc (fim -> d_numberOfBytes);
	if (data == 0 || ! memcpy (data, fim -> d_data, fim -> d_numberOfBytes)) {
		//Melder_appendError (U"No memory for dictionary.");
		return nullptr;
	}
	*numberOfBytes = fim -> d_numberOfBytes;
	return data;
}

const char * FilesInMemory_getData (FilesInMemory me, const char32 *id, long *numberOfBytes) {
	*numberOfBytes = 0;
	long index = FilesInMemory_getIndexFromId (me, id);
	if (index == 0) return 0;
	FileInMemory fim = (FileInMemory) my item[index];
	*numberOfBytes = fim -> d_numberOfBytes;
	return fim -> d_data;
}

/* End of file FileInMemory.cpp */
