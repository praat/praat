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

Thing_implement (FileStatus, Daata, 0);

autoFileStatus FileStatus_create (integer index, integer fileSizeInBytes, integer position, const char *data) {
	try {
		autoFileStatus me = Thing_new (FileStatus);
		my d_index = index;
		my d_fileSizeInBytes = fileSizeInBytes;
		my d_position = position;
		my d_data = data;
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileStatus not created.");
	}
}

Thing_implement (OpenFilesSet, SortedSet, 0);

int structOpenFilesSet :: index_compare (FileStatus me, FileStatus thee) {
	return my d_index - thy d_index;
}

integer OpenFilesSet_getMatchingIndex (OpenFilesSet me, integer index) {
	integer indexFound = 0;
	for (integer i = 1; i <= my size; i++) {
		FileStatus fileStatus = my at[i];
		if (fileStatus -> d_index == index) {
			indexFound = i;
			break;
		}
	}
	return indexFound;
}

Thing_implement (FileInMemory, Daata, 0);

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
		my d_data = NUMvector <char> (0, my d_numberOfBytes);   // includes room for a final null byte in case the file happens to contain text
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
		my d_data = const_cast<char *> (data); // copy pointer to data only
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
	// autoNUMvector<unsigned char> data (0, my d_numberOfBytes); ????
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

Thing_implement (FileInMemorySet, SortedSet, 0);

int structFileInMemorySet :: s_compare_name (FileInMemory me, FileInMemory thee) {
	return Melder_cmp (my d_path, thy d_path);
}

int structFileInMemorySet :: s_compare_id (FileInMemory me, FileInMemory thee) {
	return Melder_cmp (my d_id, thy d_id);
}

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list) {
	try {
		autoFileInMemorySet thee = Data_copy (list.at[1]);
		for (integer iset = 1; iset <= list.size; iset ++) {
			thy merge (list.at [iset]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySets not merged.");
	}
}

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (const char32 *dirpath, const char32 *fileGlobber) {
	try {
		structMelderDir parent { };
		Melder_pathToDir (dirpath, &parent);
		autoStrings thee = Strings_createAsFileList (Melder_cat (dirpath, U"/", fileGlobber));
		if (thy numberOfStrings < 1) {
			Melder_throw (U"No files found.");
		}
		autoFileInMemorySet me = FileInMemorySet_create ();
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			structMelderFile file { };
			MelderDir_getFile (& parent, thy strings [i], & file);
			autoFileInMemory fim = FileInMemory_create (& file);
			my addItem_move (fim.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created from directory \"", dirpath, U"\" for files that match \"", fileGlobber, U"\".");
	}
}

void FileInMemorySet_showAsCode (FileInMemorySet me, const char32 *name, integer numberOfBytesPerLine) {
	autoMelderString one_fim;
	MelderInfo_writeLine (U"#include \"Collection.h\"");
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"autoFileInMemorySet create_", name, U" () {");
	MelderInfo_writeLine (U"\ttry {");
	MelderInfo_writeLine (U"\t\tautoFileInMemorySet me = FileInMemorySet_create ();");
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		FileInMemory fim = my at [ifile];
		MelderString_copy (& one_fim, name, ifile);
		FileInMemory_showAsCode (fim, one_fim.string, numberOfBytesPerLine);
		MelderInfo_writeLine (U"\t\tCollection_addItem_move (me.get(), ", one_fim.string, U".move());\n");
	}
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemorySet not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
}

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, const char32 *name, integer numberOfBytesPerLine)
{
	if (index < 1 || index > my size) return;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"");
	MelderInfo_writeLine (U"#include \"melder.h\"\n");
	MelderInfo_writeLine (U"static autoFileInMemory create_new_object () {");
	MelderInfo_writeLine (U"\ttry {");
	autoMelderString one_fim;
	FileInMemory fim = my at [index];
	MelderString_append (& one_fim, name, index);
	FileInMemory_showAsCode (fim, U"me", numberOfBytesPerLine);
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
	MelderInfo_writeLine (U"autoFileInMemory ", name, U" = create_new_object ();");
}

integer FileInMemorySet_getIndexFromId (FileInMemorySet me, const char32 *id) {
	integer index = 0;
	for (integer i = 1; i <= my size; i ++) {
		FileInMemory fim = my at [i];
		if (Melder_equ (id, fim -> d_id)) {
			index = i;
			break;
		}
	}
	return index;
}

integer FileInMemorySet_getIndexFromPathName (FileInMemorySet me, const char32 *path) {
	integer index = 0;
	for (integer i = 1; i <= my size; i ++) {
		FileInMemory fim = my at [i];
		if (Melder_equ (path, fim -> d_path)) {
			index = i;
			break;
		}
	}
	return index;
}

bool FileInMemorySet_isDirectoryName (FileInMemorySet me, const char32 *name) {
	bool match = false;
	autoMelderString regex;
	for (integer i = 1; i <= my size; i ++) {
		FileInMemory fim = my at [i];
		MelderString_append (& regex, U"/", name, U"/]"); 
		if (Melder_stringMatchesCriterion (fim -> d_path, kMelder_string :: MATCH_REGEXP, regex.string)) {
			match = true;
			break;
		}
		MelderString_empty (& regex);
	}
	return match;
}

autoStrings FileInMemorySet_to_Strings_id (FileInMemorySet me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, my size);
		thy numberOfStrings = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = my at [ifile];
			thy strings [ifile] = Melder_dup_f (fim -> d_id);
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Strings created from FilesinMemory.");
	}
}

char * FileInMemorySet_getCopyOfData (FileInMemorySet me, const char32 *id, integer *numberOfBytes) {
	*numberOfBytes = 0;
	integer index = FileInMemorySet_getIndexFromId (me, id);
	if (index == 0) {
		return nullptr;
	}
	FileInMemory fim = my at [index];
	char *data = (char *) _Melder_malloc (fim -> d_numberOfBytes + 1);
	if (! data || ! memcpy (data, fim -> d_data, fim -> d_numberOfBytes)) {
		//Melder_appendError (U"No memory for dictionary.");
		return nullptr;
	}
	data [fim -> d_numberOfBytes] = '\0';
	*numberOfBytes = fim -> d_numberOfBytes;
	return data;
}

const char * FileInMemorySet_getData (FileInMemorySet me, const char32 *id, integer *numberOfBytes) {
	*numberOfBytes = 0;
	integer index = FileInMemorySet_getIndexFromId (me, id);
	if (index == 0) {
		return nullptr;
	}
	FileInMemory fim = my at [index];
	*numberOfBytes = fim -> d_numberOfBytes;
	return fim -> d_data;
}

integer FileInMemorySet_getIndexInOpenFilesSet (FileInMemorySet me, integer index) {
	for (integer i = 1; i <= my openFilesSet -> size; i++) {
		FileStatus fileStatus = my openFilesSet -> at[i];
		if (fileStatus -> d_index == index) {
			index = i;
			break;
		}
	}
	return index;
}

/*
	File open and read emulations. The FILE * is internally used as an pointer to the index of the file in the Set.
	List of open files has to contain per file: index, position, length (bytes), pointer to data
*/

FILE *FileInMemorySet_fopen (FileInMemorySet me, const char *filename, const char *mode) {
	try {
		integer *filePtr = nullptr;
		if (*mode == 'r') {
			integer index = FileInMemorySet_getIndexFromPathName (me, Melder_peek8to32(filename));
			if (index > 0) {
				FileInMemory fim = my at [index];
				autoFileStatus fileStatus = FileStatus_create (index, fim -> d_numberOfBytes, 0, fim -> d_data);
				filePtr = & (fileStatus -> d_index);
				// check if already opened ?
				my openFilesSet -> addItem_move (fileStatus.move());
			}
		} 
		return reinterpret_cast<FILE *> (filePtr);
	} catch (MelderError) {
		Melder_throw (U"File ", Melder_peek8to32(filename), U" cannot be opended.");
	}
}

int FileInMemorySet_fclose (FileInMemorySet me, FILE *stream) {
	integer *filePtr = reinterpret_cast<integer *> (stream);
	integer index = OpenFilesSet_getMatchingIndex (my openFilesSet, *filePtr);
	if (index > 0) {
		my openFilesSet -> removeItem (index);
	}
	return 0; // always ok
}

int FileInMemorySet_feof (FileInMemorySet me, FILE *stream) {
	integer *filePtr = reinterpret_cast<integer *> (stream);
	integer index = OpenFilesSet_getMatchingIndex (my openFilesSet, *filePtr);
	int eof = 0;
	if (index > 0) {
		FileStatus fs = my openFilesSet -> at [index];
		if (fs ->d_position >= fs -> d_fileSizeInBytes) {
			eof = 1;
		}
	}
	return eof;
}

int FileInMemorySet_fseek (FileInMemorySet me, FILE *stream, integer offset, int origin) {
	integer *filePtr = reinterpret_cast<integer *> (stream);
	integer index = OpenFilesSet_getMatchingIndex (my openFilesSet, *filePtr);
	int errval = 0;
	if (index > 0) {
		FileStatus fs = my openFilesSet -> at [index];
		if (origin == SEEK_SET) {
			if (offset > 0 && offset <= fs -> d_fileSizeInBytes) {
				fs -> d_position = offset;
				errval = 0;
			}
		} else if (origin == SEEK_CUR) {
			integer newPosition = fs -> d_position + offset;
			if (newPosition >= 0 && newPosition <= fs -> d_fileSizeInBytes) {
				fs -> d_position = newPosition;
				errval = 0;
			}
		} else if (origin == SEEK_END) {
			integer newPosition = fs -> d_fileSizeInBytes + offset;
			if (newPosition >= 0 && newPosition <= fs -> d_fileSizeInBytes) {
				fs -> d_position = newPosition;
				errval = 0;
			}
		} else {
			// We should not be here 
		}
	}
	return errval;
}


char *FileInMemorySet_fgets (FileInMemorySet me, char *str, int num, FILE *stream) {
	integer *filePtr = reinterpret_cast<integer *> (stream);
	integer index = OpenFilesSet_getMatchingIndex (my openFilesSet, *filePtr);
	if (index > 0) {
		FileStatus fs = my openFilesSet -> at [index];
		integer i = 0, startOffset = fs -> d_position;
		integer endOffset = startOffset + num;
		endOffset = endOffset <= fs -> d_fileSizeInBytes ? endOffset : fs -> d_fileSizeInBytes;
		const char * p = fs -> d_data + startOffset;
		while (i < endOffset) {
			str [i ++] = *p;
			if (*p ++ == '\n') {
				break;
			}
		}
		str [i] = '\0';
		fs -> d_position = endOffset;
		return str;
	} else {
		Melder_throw (me, U": File is not open.");
	}
}

int FileInMemorySet_fgetc (FileInMemorySet me, FILE *stream) {
	char str[4];
	FileInMemorySet_fgets (me, str, 1, stream); 
	return FileInMemorySet_feof (me, stream) ? EOF : static_cast<int> (*str);
}

size_t FileInMemorySet_fread (FileInMemorySet me, void *ptr, size_t size, size_t nmemb, FILE *stream) {
	integer *filePtr = reinterpret_cast<integer *> (stream);
	integer index = OpenFilesSet_getMatchingIndex (my openFilesSet, *filePtr);
	if (index > 0) {
		FileStatus fs = my openFilesSet -> at [index];
		integer i = 0, startOffset = fs -> d_position;
		integer numberOfBytes = nmemb * size; // 
		integer endOffset = startOffset + numberOfBytes;
		if (endOffset > fs -> d_fileSizeInBytes) {
			endOffset = fs -> d_fileSizeInBytes;
			endOffset -= endOffset % size;
		}
		nmemb = (endOffset - startOffset) / size;
		const char * p = fs -> d_data + startOffset;
		char * str = static_cast<char *> (ptr);
		while (i < endOffset) {
			str [i ++] = *p ++;
		}
		fs -> d_position = endOffset;
		return nmemb;
	} else {
		Melder_throw (me, U": File is not open.");
	}
}

/* This mimics GetFileLength of espeak-ng */
int FileInMemorySet_GetFileLength (FileInMemorySet me, const char *filename) {
		integer index = FileInMemorySet_getIndexFromPathName (me, Melder_peek8to32(filename));
		if (index > 0) {
			FileInMemory fim = my at [index];
			return fim -> d_numberOfBytes;
		}
		// Directory ??
		if (FileInMemorySet_isDirectoryName (me, Melder_peek8to32(filename))) {
			return -EISDIR;
		}
		return -1;
}

/* End of file FileInMemory.cpp */
