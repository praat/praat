/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2013 David Weenink
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

Thing_implement (FileInMemory, Data, 0);

void structFileInMemory :: v_copy (thou) {
	thouart (FileInMemory);
	FileInMemory_Parent :: v_copy (thee);
	thy d_path = Melder_wcsdup (d_path);
	thy d_id = Melder_wcsdup (d_id);
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
	structData :: v_info ();
	MelderInfo_writeLine (L"File name: ", d_path);
	MelderInfo_writeLine (L"Id: ", d_id);
	MelderInfo_writeLine (L"Number of bytes: ", Melder_integer (d_numberOfBytes));
}

FileInMemory FileInMemory_create (MelderFile file) {
	try {
		if (! MelderFile_readable (file)) {
			Melder_throw ("File not readable.");
		}
		long length = MelderFile_length (file);
		if (length <= 0) {
			Melder_throw ("File is empty.");
		}
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_wcsdup (file -> path);
		my d_id = Melder_wcsdup (MelderFile_name (file));
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
		Melder_throw ("FileInMemory not created from \"", Melder_fileToPath (file), "\".");
	}
}

FileInMemory FileInMemory_createWithData (long numberOfBytes, const char *data, const wchar_t *path, const wchar_t *id) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my d_path = Melder_wcsdup (path);
		my d_id = Melder_wcsdup (id);
		my d_numberOfBytes = numberOfBytes;
		my d_data = const_cast<char *> (data); // copy pointer to data only
		return me.transfer ();
	} catch (MelderError) {
		Melder_throw ("FileInMemory not create from data.");
	}
}

void FileInMemory_setId (FileInMemory me, const wchar_t *newId) {
	Melder_free (my d_id);
	my d_id = Melder_wcsdup (newId);
}

void FileInMemory_showAsCode (FileInMemory me, const wchar_t *name, long numberOfBytesPerLine)
{
	if (numberOfBytesPerLine <= 0) numberOfBytesPerLine = 20;
	// autoNUMvector<unsigned char> data (0, my d_numberOfBytes); ????
	MelderInfo_writeLine (L"\t\tstatic unsigned char ", name, L"_data[", Melder_integer (my d_numberOfBytes+1), L"] = {");
	for (long i = 0; i < my d_numberOfBytes; i++) {
		unsigned char number = my d_data[i];
		MelderInfo_write ((i % numberOfBytesPerLine == 0 ? L"\t\t\t" : L""), Melder_integer (number), L",",
			((i % numberOfBytesPerLine  == (numberOfBytesPerLine - 1)) ? L"\n" : L" "));
	}
	MelderInfo_writeLine ((my d_numberOfBytes - 1) % numberOfBytesPerLine == (numberOfBytesPerLine - 1) ? L"\t\t\t0};" : L"0};");
	MelderInfo_write (L"\t\tautoFileInMemory ", name, L" = FileInMemory_createWithData (");
	MelderInfo_writeLine (Melder_integer (my d_numberOfBytes), L", reinterpret_cast<const char *> (&", name, L"_data), \n\t\t\tL\"", my d_path, L"\", \n\t\t\tL\"", my d_id, L"\");");
}

Thing_implement (FilesInMemory, SortedSet, 0);

int structFilesInMemory :: s_compare_name (I, thou) {
	iam (FileInMemory); thouart (FileInMemory);
	return Melder_wcscmp (my d_path, thy d_path);
}

int structFilesInMemory :: s_compare_id (I, thou) {
	iam (FileInMemory); thouart (FileInMemory);
	return Melder_wcscmp (my d_id, thy d_id);
}

FilesInMemory FilesInMemory_create () {
	try {
		autoFilesInMemory me = Thing_new (FilesInMemory);
		Collection_init (me.peek(), classFileInMemory, 30);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FilesInMemory not created.");
	}
}

FilesInMemory FilesInMemory_createFromDirectoryContents (const wchar_t *dirpath, const wchar *fileGlobber) {
	try {
		structMelderDir parent = { { 0 } };
		Melder_pathToDir (dirpath, &parent);
		autoMelderString path;
		MelderString_append (&path, dirpath, L"/", fileGlobber);
		autoStrings thee = Strings_createAsFileList (path.string);
		if (thy numberOfStrings < 1) {
			Melder_throw ("No files found.");
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
		Melder_throw ("FilesInMemory not created from directory \"", dirpath, "\" for files that match \"",
		fileGlobber, "\".");
	}
}

void FilesInMemory_showAsCode (FilesInMemory me, const wchar_t *name, long numberOfBytesPerLine) {
	autoMelderString one_fim;
	autoMelderString all_fims;
	MelderInfo_writeLine (L"#include \"Collection.h\"");
	MelderInfo_writeLine (L"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (L"#include \"melder.h\"\n");
	MelderInfo_writeLine (L"FilesInMemory create_", name, L" () {");
	MelderInfo_writeLine (L"\ttry {");
	MelderInfo_writeLine (L"\t\tautoFilesInMemory me = FilesInMemory_create ();");
	for (long ifile = 1; ifile <= my size; ifile++) {
		FileInMemory fim = (FileInMemory) my item[ifile];
		MelderString_append (&one_fim, name, Melder_integer (ifile));
		FileInMemory_showAsCode (fim, one_fim.string, numberOfBytesPerLine);
		MelderInfo_writeLine (L"\t\tCollection_addItem (me.peek(), ", one_fim.string, L".transfer());\n");
		MelderString_empty (&one_fim);
	}
	MelderInfo_writeLine (L"\t\treturn me.transfer();");
	MelderInfo_writeLine (L"\t} catch (MelderError) {");
	MelderInfo_writeLine (L"\t\tMelder_throw (L\"FilesInMemory not created.\");");
	MelderInfo_writeLine (L"\t}");
	MelderInfo_writeLine (L"}\n\n");
}

void FilesInMemory_showOneFileAsCode (FilesInMemory me, long index, const wchar_t *name, long numberOfBytesPerLine)
{
	if (index < 1 || index > my size) return;
	MelderInfo_writeLine (L"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (L"#include \"melder.h\"\n");
	MelderInfo_writeLine (L"static FileInMemory create_new_object () {");
	MelderInfo_writeLine (L"\ttry {");
	autoMelderString one_fim;
	FileInMemory fim = (FileInMemory) my item[index];
	MelderString_append (&one_fim, name, Melder_integer (index));
	FileInMemory_showAsCode (fim, L"me", numberOfBytesPerLine);
	MelderInfo_writeLine (L"\t\treturn me.transfer();");
	MelderInfo_writeLine (L"\t} catch (MelderError) {");
	MelderInfo_writeLine (L"\t\tMelder_throw (L\"FileInMemory not created.\");");
	MelderInfo_writeLine (L"\t}");
	MelderInfo_writeLine (L"}\n\n");
	MelderInfo_writeLine (L"FileInMemory ", name, L" = create_new_object ();");
}

long FilesInMemory_getIndexFromId (FilesInMemory me, const wchar_t *id) {
	long index = 0;
	for (long i = 1; i <= my size; i++) {
		FileInMemory fim = (FileInMemory) my item[i];
		if (Melder_wcscmp (id, fim -> d_id) == 0) {
			index = i; break;
		}
	}
	return index;
}

Strings FilesInMemory_to_Strings_id (FilesInMemory me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <wchar *> (1, my size);
		thy numberOfStrings = 0;
		for (long ifile = 1; ifile <= my size; ifile++) {
			FileInMemory fim = (FileInMemory) my item[ifile];
			thy strings[ifile] = Melder_wcsdup_f (fim -> d_id);
			thy numberOfStrings++;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("No Strings created from FilesinMemory.");
	}
}

char * FilesInMemory_getCopyOfData (FilesInMemory me, const wchar_t *id, long *numberOfBytes) {
	*numberOfBytes = 0;
	long index = FilesInMemory_getIndexFromId (me, id);
	if (index == 0) return 0;
	FileInMemory fim = (FileInMemory) my item[index];
	char *data = (char *) _Melder_malloc (fim -> d_numberOfBytes);
	if (data == 0 ||
		(memcpy (data, fim -> d_data, fim -> d_numberOfBytes) == NULL)) {
		//Melder_error_ (L"No memory for dictionary.");
		return 0;
	}
	*numberOfBytes = fim -> d_numberOfBytes;
	return data;
}

const char * FilesInMemory_getData (FilesInMemory me, const wchar_t *id, long *numberOfBytes) {
	*numberOfBytes = 0;
	long index = FilesInMemory_getIndexFromId (me, id);
	if (index == 0) return 0;
	FileInMemory fim = (FileInMemory) my item[index];
	*numberOfBytes = fim -> d_numberOfBytes;
	return fim -> d_data;
}

/* End of file FileInMemory.cpp */
