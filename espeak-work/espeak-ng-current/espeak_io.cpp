/* espeak_io.cpp
 *
//  * Copyright (C) 2017 David Weenink
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

/*
	djmw 20171024
*/

#include "espeak_io.h"
#include "espeakdata_FileInMemory.h"

extern autoFileInMemoryManager espeak_ng_FileInMemoryManager;
#define ESPEAK_FILEINMEMORYMANAGER espeak_ng_FileInMemoryManager.get()

FILE *espeak_io_fopen (const char * filename, const char * mode) {
	return FileInMemoryManager_fopen (ESPEAK_FILEINMEMORYMANAGER, filename, mode);
}

void espeak_io_rewind (FILE *stream) {
	FileInMemoryManager_rewind (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fclose (FILE *stream) {
	return FileInMemoryManager_fclose (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_feof (FILE *stream) {
	return FileInMemoryManager_feof (ESPEAK_FILEINMEMORYMANAGER, stream);
}

integer espeak_io_ftell (FILE *stream) {
	return FileInMemoryManager_ftell (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fseek (FILE *stream, integer offset, int origin) {
	return FileInMemoryManager_fseek (ESPEAK_FILEINMEMORYMANAGER, stream, offset, origin);
}

char *espeak_io_fgets (char *str, int num, FILE *stream) {
	return FileInMemoryManager_fgets (ESPEAK_FILEINMEMORYMANAGER, str, num, stream);
}

size_t espeak_io_fread (void *ptr, size_t size, size_t count, FILE *stream) {
	return FileInMemoryManager_fread (ESPEAK_FILEINMEMORYMANAGER, ptr, size, count, stream);
}

int espeak_io_fgetc (FILE *stream) {
	return FileInMemoryManager_fgetc (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_ungetc (int character, FILE * stream) {
	return FileInMemoryManager_ungetc (ESPEAK_FILEINMEMORYMANAGER, character,stream);
}

int espeak_io_GetFileLength (const char *filename) {
	return FileInMemoryManager_GetFileLength (ESPEAK_FILEINMEMORYMANAGER, filename);
}

void espeak_io_GetVoices (const char *path, int len_path_voices, int is_language_file) {
	FileInMemoryManager_GetVoices (ESPEAK_FILEINMEMORYMANAGER, path, len_path_voices, is_language_file);
}

/* End of file espeak_io.cpp */
