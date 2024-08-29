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
	integer errorNumber;
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
autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, const char *data, bool isStaticData, conststring32 path, conststring32 id);

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine);

/*
	File open and read emulations. The FILE * is internally used as an index of the file in the Set.
*/

FILE *FileInMemorySet_fopen (FileInMemorySet me, const char *fileName, const char *mode);

void FileInMemory_rewind (FILE *stream);

int FileInMemory_fclose (FILE *stream);

int FileInMemory_feof (FILE *stream);

integer FileInMemory_ftell (FILE *stream);

int FileInMemory_fseek (FILE *stream, integer offset, int origin);

char *FileInMemory_fgets (char *str, int num, FILE *stream);

size_t FileInMemory_fread (void *ptr, size_t size, size_t count, FILE *stream);

int FileInMemory_fgetc (FILE *stream);

int FileInMemory_fprintf (FILE *stream, const char *format, ... );
/* only returns number of bytes that would have been written or -1 in case of failure */

int FileInMemory_ungetc (int character, FILE *stream);

void test_FileInMemorySet_io (void);

/* End of file FileInMemory.h */
#endif
