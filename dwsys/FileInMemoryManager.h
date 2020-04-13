#ifndef _FileInMemoryManager_h_
#define _FileInMemoryManager_h_
/* FileInMemoryManager.h
 *
 * Copyright (C) 2017-2020 David Weenink
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
#include "Strings_.h"
#include "Table.h"

#include "FileInMemoryManager_def.h"

autoFileInMemoryManager FileInMemoryManager_create (FileInMemorySet files);

autoFileInMemory FileInMemoryManager_createFile (FileInMemoryManager me, MelderFile file);

/* Generates the set with ownership */
autoFileInMemorySet FileInMemoryManager_extractFiles (FileInMemoryManager me, kMelder_string which, conststring32 criterion);

/*
	File open and read emulations. The FILE * is internally used as an index of the file in the Set.
*/

bool FileInMemoryManager_hasDirectory (FileInMemoryManager me, conststring32 name);

FILE *FileInMemoryManager_fopen (FileInMemoryManager me, const char *filename, const char *mode);

void FileInMemoryManager_rewind (FileInMemoryManager me, FILE *stream);

int FileInMemoryManager_fclose (FileInMemoryManager me, FILE *stream);

int FileInMemoryManager_feof (FileInMemoryManager me, FILE *stream);

integer FileInMemoryManager_ftell (FileInMemoryManager me, FILE *stream);

int FileInMemoryManager_fseek (FileInMemoryManager me, FILE *stream, integer offset, int origin);

char *FileInMemoryManager_fgets (FileInMemoryManager me, char *str, int num, FILE *stream);

size_t FileInMemoryManager_fread (FileInMemoryManager me, void *ptr, size_t size, size_t count, FILE *stream);

int FileInMemoryManager_fgetc (FileInMemoryManager me, FILE *stream);

int FileInMemoryManager_fprintf (FileInMemoryManager me, FILE * stream, const char * format, ... );
/* only returns number of bytes that would have been written or -1 in case of failure */

int FileInMemoryManager_ungetc (FileInMemoryManager me, int character, FILE * stream);

void test_FileInMemoryManager_io (void);

#endif // _FileInMemoryManager_h_
