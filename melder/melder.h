#ifndef _melder_h_
#define _melder_h_
/* melder.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>
#ifdef __MINGW32__
	#include <sys/types.h>   // for off_t
#endif
#include <stdbool.h>
#include <functional>   // std::move, std::forward
#include <memory>   // unique_ptr
#include <new>   // placement new

#include "melder_assert.h"   // Melder_assert
#include "melder_int.h"   // <stdint.h>, int64, integer_to_uinteger (requires Melder_assert)
#include "melder_pointer.h"   // NULL
#include "melder_real.h"   // longdouble
#include "complex.h"   // dcomplex

/*
	Law of Demeter for class functions defined outside class definition.
*/
#define our  this ->
/* The single most useful macro in Praat: */
#define my  me ->
#define thy  thee ->
#define your  you ->
#define his  him ->
#define her  she ->
#define iam(klas)  klas me = (klas) void_me

#include "melder_alloc.h"   // Melder_malloc (requires int64), Melder_free, Melder_strdup
#include "melder_string32.h"   // char32, conststring32, autostring32 (requires Melder_malloc, our), Melder_dup
#include "melder_kar.h"   // Melder_hasInk (requires char32), Melder_toLowerCase
#include "melder_str32.h"   // str32len, str32cpy, str32cmp_caseInsensitive (requires Melder_toLowerCase)

#include "enums.h"
#include "melder_enums.h"

/*
 * Operating system version control.
 */
#define ALLOW_GDK_DRAWING  (gtk && 1)   /* change to (gtk && 0) if you want to try out GTK 3 */
/* */

typedef struct { double red, green, blue, transparency; } double_rgbt;

#include "melder_ftoa.h"   // Melder_double, Melder_pad (require dcomplex, conststring32)
#include "melder_console.h"   // MelderConsole (requires conststring32)
#include "melder_textencoding.h"   // str32len_utf8, Melder_32to8
#include "melder_atof.h"
#include "melder_files.h"   // Melder_fopen, MelderFile, MelderDir
#include "melder_vector.h"   // NUMvector, autoNUMmatrix, VEC, autoMAT, Melder_VEC
#include "melder_strvec.h"   // string32vector, autostring32vector (requires NUMvector)
#include "melder_sort.h"   // VECsort_inplace (requires VEC), NUMsort_str (requires string32vector)

#include "MelderArg.h"   // MelderArg (requires Melder_double, MelderFile, Melder_VEC)
#include "melder_debug.h"   // trace (requires MelderFile, MelderArg), Melder_debug
#include "MelderFile.h"   // MelderFile_open (requires MelderFile), MelderFile_write (requires MelderArg)
#include "MelderString.h"   // MelderString_append (requires MelderArg)
#include "melder_cat.h"   // Melder_cat (requires MelderArg)
#include "melder_sprint.h"   // Melder_sprint (requires MelderArg)
#include "melder_search.h"
#include "melder_casual.h"
#include "melder_info.h"
#include "melder_error.h"   // Melder_throw (requires MelderArg)
#include "melder_warning.h"
#include "melder_fatal.h"
#include "melder_progress.h"
#include "melder_play.h"   // Melder_record, Melder_play, Melder_beep
#include "melder_help.h"
#include "melder_ftoi.h"
#include "melder_time.h"   // stopwatch, sleep, clock
#include "melder_audio.h"
#include "melder_audiofiles.h"

/* The following trick uses Melder_debug only because it is the only plain variable known to exist at the moment. */
#define Melder_offsetof(klas,member) (int) ((char *) & ((klas) & Melder_debug) -> member - (char *) & Melder_debug)

/********** SYSTEM VERSION **********/

/*
	For MacOS, this is set in Melder_init.
	TODO: change to inline variable once C++17 is implemented completely on all platforms.
*/
extern int32 Melder_systemVersion;

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

/*
	Set in praat_init.
	True if run from the batch or from an interactive command-line interface.
	TODO: change to inline variable once C++17 is implemented completely on all platforms.
*/
extern bool Melder_batch;

/*
	True if running a script.
	TODO: change to inline variable once C++17 is implemented completely on all platforms.
*/
extern bool Melder_backgrounding;

void Melder_init ();   // inits NUmrandom, alloc, message, Melder_systemVersion

#include "melder_quantity.h"

#include "MelderReadText.h"
#include "melder_vectorio.h"   // requires MelderReadText
#include "melder_sysenv.h"
#include "abcio_enums.h"
#include "abcio.h"   // requires MelderReadText

#include "melder_templates.h"   // Melder_ENABLE_IF_ISA, MelderCallback, MelderCompareHook

#include "NUMmath.h"   // <math.h>, NUMpi, undefined
#include "NUMspecfunc.h"
#include "NUMear.h"
#include "NUMinterpol.h"
#include "NUMmetrics.h"
#include "NUMrandom.h"
#include "NUMfilter.h"
#include "NUMlinprog.h"

#include "regularExp.h"
#include "PAIRWISE_SUM.h"

#include "NUM.h"
#include "STR.h"
#include "VEC.h"
#include "MAT.h"
#include "STRVEC.h"

/* End of file melder.h */
#endif
