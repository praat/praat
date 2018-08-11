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

#include "melder_alloc.h"   // Melder_malloc (requires int64), Melder_free
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
#include "melder_vector.h"   // NUMvector, autoNUMmatrix, numvec, autonummat, Melder_numvec
#include "melder_strvec.h"   // string32vector, autostring32vector (requires NUMvector)
#include "melder_sort.h"   // NUMsort_d (requires numvec), NUMsort_str (requires string32vector)

#include "MelderArg.h"   // MelderArg (requires Melder_double, MelderFile, Melder_numvec)
#include "melder_debug.h"   // trace (requires MelderFile, MelderArg), Melder_debug
#include "MelderFile.h"   // MelderFile_open (requires MelderFile), MelderFile_write (requires MelderArg)
#include "MelderString.h"   // MelderString_append (requires MelderArg)
#include "melder_cat.h"   // Melder_cat (requires MelderArg)
#include "melder_sprint.h"   // Melder_sprint (requires MelderArg)
#include "melder_search.h"
#include "melder_token.h"
#include "melder_casual.h"
#include "melder_info.h"
#include "melder_error.h"   // Melder_throw (requires MelderArg)
#include "melder_warning.h"
#include "melder_fatal.h"
#include "melder_progress.h"
#include "melder_ftoi.h"
#include "melder_time.h"   // stopwatch, sleep, clock
#include "melder_audio.h"
#include "melder_audiofiles.h"

/* The following trick uses Melder_debug only because it is the only plain variable known to exist at the moment. */
#define Melder_offsetof(klas,member) (int) ((char *) & ((klas) & Melder_debug) -> member - (char *) & Melder_debug)

/********** SYSTEM VERSION **********/

/*
	For Macintosh, this is set in praat_init.
	TODO: change to inline variable once C++17 is implemented completely on all platforms.
*/
extern int32 Melder_systemVersion;

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

void MelderGui_create (/* GuiWindow */ void *parent);
/*
	'parent' is the top-level widget returned by GuiAppInitialize.
*/

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

/********** OVERRIDE DEFAULT BEHAVIOUR **********/

/* Procedures to override default message methods. */
/* They may chage the string arguments. */
/* Many of these routines are called by MelderGui_create(). */

void Melder_setCasualProc (void (*casualProc) (conststring32 message));
void Melder_setProgressProc (int (*progressProc) (double progress, conststring32 message));
void Melder_setMonitorProc (void * (*monitorProc) (double progress, conststring32 message));
void Melder_setInformationProc (MelderInfo::Proc informationProc);
void Melder_setHelpProc (void (*help) (conststring32 query));
void Melder_setSearchProc (void (*search) ());
void Melder_setWarningProc (void (*warningProc) (conststring32 message));
void Melder_setProgressProc (void (*progress) (double, conststring32));
void Melder_setMonitorProc (void * (*monitor) (double, conststring32));
void Melder_setErrorProc (void (*errorProc) (conststring32 message));
void Melder_setFatalProc (void (*fatalProc) (conststring32 message));

#include "melder_quantity.h"

#include "melder_readText.h"
#include "melder_writeText.h"
#include "melder_sysenv.h"
#include "abcio_enums.h"
#include "abcio.h"

/* The following ANSI-C power trick generates the declarations of 88 functions. */
#define FUNCTION(type,storage)  \
	void NUMvector_writeText_##storage (const type *v, integer lo, integer hi, MelderFile file, conststring32 name); \
	void NUMvector_writeBinary_##storage (const type *v, integer lo, integer hi, FILE *f); \
	type * NUMvector_readText_##storage (integer lo, integer hi, MelderReadText text, const char *name); \
	type * NUMvector_readBinary_##storage (integer lo, integer hi, FILE *f); \
	void NUMmatrix_writeText_##storage (type **v, integer r1, integer r2, integer c1, integer c2, MelderFile file, conststring32 name); \
	void NUMmatrix_writeBinary_##storage (type **v, integer r1, integer r2, integer c1, integer c2, FILE *f); \
	type ** NUMmatrix_readText_##storage (integer r1, integer r2, integer c1, integer c2, MelderReadText text, const char *name); \
	type ** NUMmatrix_readBinary_##storage (integer r1, integer r2, integer c1, integer c2, FILE *f);
FUNCTION (signed char, i8)
FUNCTION (int, i16)
FUNCTION (long, i32)
FUNCTION (integer, integer32BE)
FUNCTION (unsigned char, u8)
FUNCTION (unsigned int, u16)
FUNCTION (unsigned long, u32)
FUNCTION (double, r32)
FUNCTION (double, r64)
FUNCTION (dcomplex, c64)
FUNCTION (dcomplex, c128)
#undef FUNCTION

/*
void NUMvector_writeBinary_r64 (const double *v, integer lo, integer hi, FILE *f);   // etc
	write the vector elements v [lo..hi] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUM...vector.
double * NUMvector_readText_r64 (integer lo, integer hi, MelderReadText text, const char *name);   // etc
	create and read a vector as text.
	Throw an error message if anything went wrong.
	Every element is supposed to be on the beginning of a line.
double * NUMvector_readBinary_r64 (integer lo, integer hi, FILE *f);   // etc
	create and read a vector as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
void NUMvector_writeText_r64 (const double *v, integer lo, integer hi, MelderFile file, conststring32 name);   // etc
	write the vector elements v [lo..hi] as text to the open file,
	each element on its own line, preceded by "name [index]: ".
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUMvector.
void NUMmatrix_writeText_r64 (double **m, integer r1, integer r2, integer c1, integer c2, MelderFile file, conststring32 name);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as text to the open file.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
void NUMmatrix_writeBinary_r64 (double **m, integer r1, integer r2, integer c1, integer c2, FILE *f);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
double ** NUMmatrix_readText_r64 (integer r1, integer r2, integer c1, integer c2, MelderReadText text, const char *name);   // etc
	create and read a matrix as text.
	Throw an error message if anything went wrong.
double ** NUMmatrix_readBinary_r64 (integer r1, integer r2, integer c1, integer c2, FILE *f);   // etc
	create and read a matrix as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
*/


//#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if<std::is_base_of<B,A>::value>::type
#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if_t<std::is_base_of<B,A>::value>
//#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if_v<std::is_base_of<B,A>>

template <typename Ret, typename T, typename... Args>
class MelderCallback {
	public:
		using FunctionType = Ret* (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T), typename Ret2  Melder_ENABLE_IF_ISA(Ret2,Ret)>
			MelderCallback (Ret2* (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		Ret* operator () (T* data, Args ... args) { return _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <void, T, Args...> {   // specialization
	public:
		using FunctionType = void (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCallback (void (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		void operator () (T* data, Args ... args) { _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <int, T, Args...> {   // specialization
	public:
		using FunctionType = int (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCallback (int (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		int operator () (T* data, Args ... args) { return _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const { return !! _f; }
	private:
		FunctionType _f;
};

template <typename T>
class MelderCompareHook {
	public:
		typedef int (*FunctionType) (T*, T*);
		MelderCompareHook (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCompareHook (int (*f) (T2*, T2*)) : _f (reinterpret_cast<FunctionType> (f)) { };
		int operator () (T* data1, T* data2) noexcept { return _f (data1, data2); }
		explicit operator bool () const { return !! _f; }
		FunctionType get () { return _f; }
	private:
		FunctionType _f;
};

#include "NUMmath.h"   // <math.h>, NUMpow, NUMpi, undefined
#include "NUMspecfunc.h"
#include "NUMear.h"
#include "NUMinterpol.h"
#include "NUMmetrics.h"
#include "NUMrandom.h"
#include "NUMfilter.h"
#include "NUMlinprog.h"

#include "regularExp.h"

#include "xxStr.h"
#include "tensor.h"

/* End of file melder.h */
#endif
