/*
 * Copyright (C) 2005 to 2007 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2015 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#ifndef SPEECH_H
#define SPEECH_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#define ARCH_BIG
#endif

#ifdef __QNX__
#define NO_VARIADIC_MACROS
#endif

#if defined(_WIN32) || defined(_WIN64) // Windows

#define PLATFORM_WINDOWS
#define PATHSEP '/'
#define N_PATH_HOME  230
#define NO_VARIADIC_MACROS

#else

#define PLATFORM_POSIX
#define PATHSEP  '/'
#define N_PATH_HOME  160
#define USE_NANOSLEEP
#define __cdecl

#endif

// used in synthesize.h and voice.h	
#define N_PEAKS   9
#define N_PEAKS2  9 // plus Notch and Fill (not yet implemented)

// will look for espeak_data directory here, and also in user's home directory
#ifndef PATH_ESPEAK_DATA
   #define PATH_ESPEAK_DATA  "/usr/share/espeak-ng-data"
#endif

#ifdef DATA_FROM_SOURCECODE_FILES
	#include "FileInMemoryManager.h"
	#define ESPEAK_FILEINMEMORYSET espeak-ng-data-allFilesInMemory
	#define PATH_ESPEAK_DATA "/home/david/projects/espeak-ng/espeak-ng-data"
	#define fopen(filename,mode) FileInMemoryManager_fopen (ESPEAK_FILEINMEMORYSET, filename, mode)
	#define fclose(stream) FileInMemoryManager_fclose (ESPEAK_FILEINMEMORYSET, stream)
	#define feof(stream) FileInMemoryManager_feof (ESPEAK_FILEINMEMORYSET, stream)	
	#define fseek(stream,offset,origin) FileInMemoryManager_fseek (ESPEAK_FILEINMEMORYSET, stream, offset, origin)
	#define fgets(str,num,stream) FileInMemoryManager_fgets (ESPEAK_FILEINMEMORYSET, str, num, stream)
	#define fgetc(stream) FileInMemoryManager_fgetc (ESPEAK_FILEINMEMORYSET, stream);
	#define GetFileLength(filename) FileInMemoryManager_GetFileLength (ESPEAK_FILEINMEMORYSET, filename)	
#endif
	
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef double DOUBLEX;

typedef struct {
	const char *mnem;
	int value;
} MNEM_TAB;
int LookupMnem(MNEM_TAB *table, const char *string);
const char *LookupMnemName(MNEM_TAB *table, const int value);

void cancel_audio(void);

extern char path_home[N_PATH_HOME];    // this is the espeak-ng-data directory

extern ESPEAK_NG_API void strncpy0(char *to, const char *from, int size);
extern ESPEAK_NG_API int  GetFileLength(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // SPEECH_H
