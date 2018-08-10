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
#include "melder_math.h"   // <math.h>, NUMpow, NUMpi, undefined
#include "melder_vector.h"   // NUMvector, autoNUMmatrix, numvec, autonummat, Melder_numvec
#include "melder_strvec.h"   // string32vector, autostring32vector (requires NUMvector)
#include "melder_sort.h"   // NUMsort_d (requires numvec), NUMsort_str (requires string32vector)

#include "melder_specfunc.h"
#include "melder_ear.h"
#include "melder_interpol.h"
#include "melder_metrics.h"
#include "melder_random.h"
#include "melder_filter.h"
#include "melder_linprog.h"

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

inline int32 Melder_systemVersion;
/*
	For Macintosh, this is set in praat_init.
*/

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

void MelderGui_create (/* GuiWindow */ void *parent);
/*
	'parent' is the top-level widget returned by GuiAppInitialize.
*/

inline bool Melder_batch;   // true if run from the batch or from an interactive command-line interface
inline bool Melder_backgrounding;   // true if running a script
inline bool Melder_asynchronous;   // true if specified by the "asynchronous" directive in a script

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

#pragma mark - QUANTITY

#define MelderQuantity_NONE  0
#define MelderQuantity_TIME_SECONDS  1
#define MelderQuantity_FREQUENCY_HERTZ  2
#define MelderQuantity_FREQUENCY_BARK  3
#define MelderQuantity_DISTANCE_FROM_GLOTTIS_METRES  4
#define MelderQuantity_NUMBER_OF_QUANTITIES  4
conststring32 MelderQuantity_getText (int quantity);   // e.g. "Time"
conststring32 MelderQuantity_getWithUnitText (int quantity);   // e.g. "Time (s)"
conststring32 MelderQuantity_getLongUnitText (int quantity);   // e.g. "seconds"
conststring32 MelderQuantity_getShortUnitText (int quantity);   // e.g. "s"

#pragma mark - READING TEXT

struct structMelderReadText {
	autostring32 string32;
	char32 *readPointer32;
	autostring8 string8;
	char *readPointer8;
	kMelder_textInputEncoding input8Encoding;
	structMelderReadText () : readPointer32 (nullptr), readPointer8 (nullptr) {
		/*
			Check that C++ default initialization has worked.
		*/
		Melder_assert (! our string32);
		Melder_assert (! our string8);
	}
};
typedef struct structMelderReadText *MelderReadText;

#if 1
	using autoMelderReadText = std::unique_ptr<structMelderReadText>;
#else
struct autoMelderReadText {
	MelderReadText text;
	autoMelderReadText () {
		our text = new structMelderReadText;
	}
	~ autoMelderReadText () {
		delete (our text);
	}
	MelderReadText operator-> () const {   // as r-value
		return our text;
	}
	MelderReadText get () const {
		return our text;
	}
	autoMelderReadText (const autoMelderReadText&) = delete;   // disable copy constructor
	autoMelderReadText (autoMelderReadText&& other) noexcept {   // enable move constructor
		our text = other.text;
		other.text = nullptr;
	}
	autoMelderReadText& operator= (const autoMelderReadText&) = delete;   // disable copy assignment
	autoMelderReadText& operator= (autoMelderReadText&& other) noexcept {   // enable move assignment
		if (& other != this) {
			delete (our text);
			our text = other.text;
			other.text = nullptr;
		}
		return *this;
	}
	autoMelderReadText&& move () noexcept { return static_cast <autoMelderReadText&&> (*this); }
	explicit operator bool () const { return !! our text; }
};
#endif

autoMelderReadText MelderReadText_createFromFile (MelderFile file);
char32 MelderReadText_getChar (MelderReadText text);
mutablestring32 MelderReadText_readLine (MelderReadText text);
int64 MelderReadText_getNumberOfLines (MelderReadText me);
conststring32 MelderReadText_getLineNumber (MelderReadText text);

#include "../sys/abcio.h"

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

#pragma mark - MISCELLANEOUS

conststring32 Melder_getenv (conststring32 variableName);
void Melder_system (conststring32 command);   // spawn a system command
void Melder_execv (conststring32 executableFileName, integer narg, char32 **args);   // spawn a subprocess



class autoMelderAsynchronous {
	bool _disowned;
	bool _savedAsynchronicity;
public:
	autoMelderAsynchronous () {
		our _savedAsynchronicity = Melder_asynchronous;
		Melder_asynchronous = true;
		our _disowned = false;
	}
	~autoMelderAsynchronous () {
		if (! _disowned) {
			Melder_asynchronous = _savedAsynchronicity;
		}
	}
	/*
		Disable copying.
	*/
	autoMelderAsynchronous (const autoMelderAsynchronous&) = delete;   // disable copy constructor
	autoMelderAsynchronous& operator= (const autoMelderAsynchronous&) = delete;   // disable copy assignment
	/*
		Enable moving.
	*/
	autoMelderAsynchronous (autoMelderAsynchronous&& other) noexcept {   // enable move constructor
		our _disowned = other._disowned;
		our _savedAsynchronicity = other._savedAsynchronicity;
		other._disowned = true;
	}
	autoMelderAsynchronous& operator= (autoMelderAsynchronous&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our _disowned = other._disowned;
			our _savedAsynchronicity = other._savedAsynchronicity;
			other._disowned = true;   // needed only if you insist on keeping the source in a valid state
		}
		return *this;
	}
	autoMelderAsynchronous&& move () noexcept { return static_cast <autoMelderAsynchronous&&> (*this); }
	void releaseToAmbiguousOwner () {
		our _disowned = true;
	}
};

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

#include "../dwsys/regularExp.h"

#include "xxStr.h"

/* End of file melder.h */
#endif
