#ifndef _espeak_io_h_
#define _espeak_io_h_

/* espeak_io.h
 * Copyright (C) David Weenink 2017
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

#include <stdio.h>
	
#define PATH_ESPEAK_DATA "/home/david/projects/espeak-ng/espeak-ng-data"
	
#define fopen(filename,mode) espeak_io_fopen (filename, mode)
#define fclose(stream) espeak_io_fclose (stream)
#define feof(stream) espeak_io_feof (stream)	
#define fseek(stream,offset,origin) espeak_io_fseek (stream, offset, origin)
#define ftell(stream) espeak_io_ftell (stream)
#define fgets(str,num,stream) espeak_io_fgets (str, num, stream)
#define fread(ptr,size,count,stream) espeak_io_fread (ptr, size, count,stream)
#define fgetc(stream) espeak_io_fgetc (stream)
#define ungetc(character,stream) espeak_io_ungetc (character, stream)
#define GetFileLength(filename) espeak_io_GetFileLength (filename)
#define GetVoices(path,len_path_voices,is_language_file) espeak_io_GetVoices (path, len_path_voices, is_language_file)

FILE *espeak_io_fopen (const char *filename, const char *mode);

void espeak_io_rewind (FILE *stream);

int espeak_io_fclose (FILE *stream);

int espeak_io_feof (FILE *stream);

long espeak_io_ftell (FILE *stream);

int espeak_io_fseek (FILE *stream, long offset, int origin);

char *espeak_io_fgets (char *str, int num, FILE *stream);

size_t espeak_io_fread (void *ptr, size_t size, size_t count, FILE *stream);

int espeak_io_fgetc (FILE *stream);

int espeak_io_ungetc (int character, FILE * stream);

int espeak_io_GetFileLength (const char *filename);

void espeak_io_GetVoices (const char *path, int len_path_voices, int is_language_file);

#endif
