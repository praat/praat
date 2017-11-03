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
#include "espeak_ng.h"
#include "speech.h"
#include "voice.h"
#include <errno.h>

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

long espeak_io_ftell (FILE *stream) {
	return FileInMemoryManager_ftell (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fseek (FILE *stream, long offset, int origin) {
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

int espeak_io_fprintf (FILE * stream, ... ) {
	va_list arg;
	va_start (arg, stream);
	char *format = static_cast<char *> (va_arg (arg, void*));
	int result = FileInMemoryManager_fprintf (ESPEAK_FILEINMEMORYMANAGER, stream, format, arg);
	va_end (arg);
	return result;
}

int espeak_io_ungetc (int character, FILE * stream) {
	return FileInMemoryManager_ungetc (ESPEAK_FILEINMEMORYMANAGER, character,stream);
}
/* This mimics GetFileLength of espeak-ng */
int FileInMemoryManager_GetFileLength (FileInMemoryManager me, const char *filename) {
		integer index = FileInMemorySet_lookUp (my files.get(), Melder_peek8to32(filename));
		if (index > 0) {
			FileInMemory fim = static_cast<FileInMemory> (my files -> at [index]);
			return fim -> d_numberOfBytes;
		}
		// Directory ??
		if (FileInMemorySet_hasDirectory (my files.get(), Melder_peek8to32(filename))) {
			return -EISDIR;
		}
		return -1;
}

/* 
	espeak_io_GetFileLength: mimics GetFileLength of espeak-ng
	Returns the number of bytes in the file.
	If the filename is a directory it return -EISDIR
*/
int espeak_io_GetFileLength (const char *filename) {
	FileInMemorySet me = ESPEAK_FILEINMEMORYMANAGER -> files.get();
	integer index = FileInMemorySet_lookUp (me, Melder_peek8to32(filename));
	if (index > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my at [index]);
		return fim -> d_numberOfBytes;
	}
	// Directory ??
	if (FileInMemorySet_hasDirectory (me, Melder_peek8to32(filename))) {
		return -EISDIR;
	}
	return -1;
}

/* 
	espeak_io_GetVoices: mimics GetVoices of espeak-ng
	If is_languange_file == 0 then /voices/ else /lang/ 
	We know our voices are in /voices/ and our languages in /lang/
*/
void espeak_io_GetVoices (const char *path, int len_path_voices, int is_language_file) {
	(void) path;
	/*
		if is_languange_file == 0 then /voices/ else /lang/ 
		We know our voices are in /voices/!v/ and our languages in /lang/
	*/
	FileInMemoryManager me = ESPEAK_FILEINMEMORYMANAGER;
	const char32 *criterion = is_language_file ? U"/lang/" : U"/voices/";
	autoFileInMemorySet fileList = FileInMemorySet_listFiles (my files.get(), kMelder_string :: CONTAINS, criterion);
	for (long ifile = 1; ifile <= fileList -> size; ifile ++) {
		FileInMemory fim = static_cast<FileInMemory> (fileList -> at [ifile]);
		FILE *f_voice = FileInMemoryManager_fopen (me, Melder_peek32to8 (fim -> d_path), "r");
		char *fname = Melder_peek32to8 (fim -> d_path);
		espeak_VOICE *voice_data = ReadVoiceFile (f_voice, fname + len_path_voices, is_language_file);
		FileInMemoryManager_fclose (me, f_voice);
		if (voice_data) {
			voices_list [n_voices_list ++] = voice_data;
		} /*else {
			Melder_warning (U"Voice data for ", fname, U" could not be gathered.");
		}*/
	}
}

/* End of file espeak_io.cpp */
