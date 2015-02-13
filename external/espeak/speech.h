/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#ifndef SPEECH_H
#define SPEECH_H

#include <sys/types.h>
#include "espeakdata_FileInMemory.h"
#define DATA_FROM_SOURCECODE_FILES

#ifdef _WIN32
	wchar_t * Melder_peekUtf8ToWcs (const char *string);
	const uint16_t * Melder_peekWcsToUtf16 (const wchar_t *string);
#endif

// conditional compilation options
#define INCLUDE_KLATT
/* ppgb, 23 December 2011 */
#undef INCLUDE_MBROLA

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#define ARCH_BIG
#endif

#ifdef __QNX__
#define NEED_GETOPT
#define NO_VARIADIC_MACROS
#endif

/* ppgb, 23 December 2011 */
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS

#define PATHSEP  '/'
// USE_PORTAUDIO or USE_PULSEAUDIO are now defined in the makefile
//#define USE_PORTAUDIO
//#define USE_PULSEAUDIO
/* ppgb, 23 December 2011 */
#undef USE_NANOSLEEP
//#define ESPEAK_API  extern "C"

#ifdef LIBRARY
	#define USE_ASYNC
#endif

#ifdef _ESPEAKEDIT
	#define USE_PORTAUDIO
	#define USE_ASYNC
	#define LOG_FRAMES      // write keyframe info to log-espeakedit
#endif

// will look for espeak_data directory here, and also in user's home directory
#ifndef PATH_ESPEAK_DATA
   #define PATH_ESPEAK_DATA  "/usr/share/espeak-data"
#endif

typedef unsigned short USHORT;
typedef unsigned char  UCHAR;
typedef double DOUBLEX;
typedef uint64_t long64;   // use this for conversion between pointers and integers

template <size_t _align_to, typename T> T *align_address (T *p)
{
	union {
		T* ptr;
		size_t integer;
	};
	const size_t bit_mask = ~(_align_to - 1);
	ptr = p;
    Melder_assert (sizeof (size_t) == sizeof (void *));
	integer &= bit_mask;
	return ptr;
}


typedef struct {
   const char *mnem;
   int  value;
} MNEM_TAB;
int LookupMnem(MNEM_TAB *table, const char *string);


#ifdef PLATFORM_WINDOWS
	#define N_PATH_HOME  220
#else
	#define N_PATH_HOME  150
#endif

extern char path_home[N_PATH_HOME];    // this is the espeak-data directory

extern void strncpy0(char *to,const char *from, int size);
int  GetFileLength(const char *filename);
char *Alloc(int size);
void Free(void *ptr);

#endif // SPEECH_H
