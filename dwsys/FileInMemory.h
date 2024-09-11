#ifndef _FileInMemory_h_
#define _FileInMemory_h_
/* FileInMemory.h
 *
 * Copyright (C) 2011-2020 David Weenink, 2015,2018,2024 Paul Boersma
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

#include "Collection.h"
#include "Strings_.h"

#include "FileInMemory_def.h"

Collection_define (FileInMemorySet, SortedSetOfStringOf, FileInMemory) {
	void v1_info ()
		override;
};

autoFileInMemory FileInMemory_create (MelderFile file);

/*
	If the FileInMemory is created from static data we do not need to copy the data into the object but only
	create a link to the data. This implies that the data cannot be deleted if the FileInMemory object gets
	destroyed. However, if the FileInMemory object is copied, for example, then we also have to copy the data,
	and the copied object has to become owner of these copied data. This is automatically guaranteed by our
	implementation, because _dontOwnData is default-initialised as false.
	Only if we create a FileInMemory object from data we have to be explicit about ownership.
*/
autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, uint8 *data, bool isStaticData, conststring32 path);

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine);

/*
	File open and read emulations. The FILE * is internally used as an index of the file in the Set.
*/

FileInMemory FileInMemorySet_fopen (FileInMemorySet me, const char *fileName, const char *mode);

void FileInMemory_rewind (FileInMemory stream);

int FileInMemory_fclose (FileInMemory stream);

int FileInMemory_feof (FileInMemory stream);

int FileInMemory_ferror (FileInMemory stream);

integer FileInMemory_ftell (FileInMemory stream);

int FileInMemory_fseek (FileInMemory stream, integer offset, int origin);

char *FileInMemory_fgets (char *str, int num, FileInMemory stream);

size_t FileInMemory_fread (void *ptr, size_t size, size_t count, FileInMemory stream);

int FileInMemory_fgetc (FileInMemory stream);

int FileInMemory_ungetc (int character, FileInMemory stream);

int FileInMemorySet_stat (FileInMemorySet me, const char *path, struct stat *buf);

/*
	FileInMemorySet bookkeeping.
*/

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (conststring32 dirpath, conststring32 file);

autoFileInMemorySet FilesInMemory_to_FileInMemorySet (OrderedOf<structFileInMemory>& list);

autoFileInMemorySet FileInMemorySet_extractFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion);
autoFileInMemorySet FileInMemorySet_removeFiles  (FileInMemorySet me, kMelder_string which, conststring32 criterion);

/* Only creates references to files in memory. Once me is deleted, the references are not valid any more!! */
autoFileInMemorySet FileInMemorySet_listFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion);

integer FileInMemorySet_getTotalNumberOfBytes (FileInMemorySet me);

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list);

void FileInMemorySet_showAsCode (FileInMemorySet me, conststring32 name, integer numberOfBytesPerLine);

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, conststring32 name, integer numberOfBytesPerLine);

integer FileInMemorySet_findNumberOfMatches_path (FileInMemorySet me, kMelder_string which, conststring32 criterion);

bool FileInMemorySet_hasDirectory (FileInMemorySet me, conststring32 name);

autoStrings FileInMemorySet_to_Strings_path (FileInMemorySet me);

/*
	Testing.
*/
void test_FileInMemory_io (void);


/* End of file FileInMemory.h */
#endif
