#ifndef _melder_h_
#define _melder_h_
/* melder.h
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016,2017 Paul Boersma
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
#include <functional>

#pragma mark - INTEGERS
/*
 * The following two lines are for obsolete (i.e. C99) versions of stdint.h
 */
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef intptr_t integer;   // the default size of an integer (a "long" is only 32 bits on 64-bit Windows)
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#ifndef INT12_MAX
	#define INT12_MAX   2047
	#define INT12_MIN  -2048
#endif
#ifndef UINT12_MAX
	#define UINT12_MAX   4096
#endif
#ifndef INT24_MAX
	#define INT24_MAX   8388607
	#define INT24_MIN  -8388608
#endif
#ifndef UINT24_MAX
	#define UINT24_MAX   16777216
#endif
/*
	The bounds of the contiguous set of integers that in a "double" can represent only themselves.
*/
#ifndef INT54_MAX
	#define INT54_MAX   9007199254740991LL
	#define INT54_MIN  -9007199254740991LL
#endif

#pragma mark - BOOLEANS

#ifndef TRUE
	#define TRUE  1
#endif
#ifndef FALSE
	#define FALSE  0
#endif
#ifndef NULL
	#define NULL  ((void *) 0)
#endif

#pragma mark - REALS
/*
	The following are checked in praat.h.
*/
typedef float real32;
typedef double real64;
typedef long double real80;   // at least 80 bits ("extended") precision, but stored in 96 or 128 bits
typedef double real;

#pragma mark - LAW OF DEMETER FOR CLASS FUNCTIONS DEFINED OUTSIDE CLASS DEFINITION

#define our  this ->
/* The single most useful macro in Praat: */
#define my  me ->
#define thy  thee ->
#define your  you ->
#define his  him ->
#define her  she ->
#define iam(klas)  klas me = (klas) void_me

#pragma mark - DEBUGGING

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition);
	/* Call Melder_fatal with a message based on the following template: */
	/*    "Assertion failed in file <fileName> on line <lineNumber>: <condition>" */

void Melder_setTracing (bool tracing);
extern bool Melder_isTracing;

#pragma mark - STRINGS

typedef unsigned char char8;
typedef char16_t char16;
typedef char32_t char32;

#define strequ  ! strcmp
#define strnequ  ! strncmp

inline static int64 str16len (const char16 *string) noexcept {
	const char16 *p = string;
	while (*p != u'\0') ++ p;
	return (int64) (p - string);
}
inline static char16 * str16cpy (char16 *target, const char16 *source) noexcept {
	char16 *p = target;
	while (* source != u'\0') * p ++ = * source ++;
	*p = u'\0';
	return target;
}

inline static int64 str32len (const char32 *string) noexcept {
	const char32 *p = string;
	while (*p != U'\0') ++ p;
	return (int64) (p - string);
}
inline static char32 * str32cpy (char32 *target, const char32 *source) noexcept {
	char32 *p = target;
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}
inline static char32 * str32ncpy (char32 *target, const char32 *source, int64 n) noexcept {
	char32 *p = target;
	for (; n > 0 && *source != U'\0'; -- n) * p ++ = * source ++;
	for (; n > 0; -- n) * p ++ = U'\0';
	return target;
}

inline static int str32cmp (const char32 *string1, const char32 *string2) noexcept {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
inline static int str32ncmp (const char32 *string1, const char32 *string2, int64 n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
int Melder_cmp (const char32 *string1, const char32 *string2);   // regards null string as empty string
int Melder_ncmp (const char32 *string1, const char32 *string2, int64 n);

#define str32equ  ! str32cmp
#define str32nequ  ! str32ncmp
#define Melder_equ  ! Melder_cmp
bool Melder_equ_firstCharacterCaseInsensitive (const char32 *string1, const char32 *string2);
#define Melder_nequ  ! Melder_ncmp

inline static char32 * str32chr (const char32 *string, char32 kar) noexcept {
	for (; *string != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline static char32 * str32rchr (const char32 *string, char32 kar) noexcept {
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (*string == kar) result = (char32 *) string;
	}
	return result;
}
inline static char32 * str32str (const char32 *string, const char32 *find) noexcept {
	int64 length = str32len (find);
	if (length == 0) return (char32 *) string;
	char32 firstCharacter = * find ++;   // optimization
	do {
		char32 kar;
		do {
			kar = * string ++;
			if (kar == U'\0') return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp (string, find, length - 1));
	return (char32 *) (string - 1);
}
inline static int64 str32spn (const char32 *string1, const char32 *string2) noexcept {
	const char32 *p = string1;
	char32 kar1, kar2;
cont:
	kar1 = * p ++;
	for (const char32 * q = string2; (kar2 = * q ++) != U'\0';)
		if (kar2 == kar1)
			goto cont;
	return p - 1 - string1;
}
inline static bool islower32 (char32 kar) { return iswlower ((int) kar); }
inline static bool isupper32 (char32 kar) { return iswupper ((int) kar); }
inline static char32 tolower32 (char32 kar) { return (char32) towlower ((int) kar); }
inline static char32 toupper32 (char32 kar) { return (char32) towupper ((int) kar); }

char32 * Melder_tok (char32 *string, const char32 *delimiter);

#pragma mark - ENUMERATED TYPES

#include "enums.h"
#include "melder_enums.h"

/*
 * Operating system version control.
 */
#define ALLOW_GDK_DRAWING  (gtk && 1)   /* change to (gtk && 0) if you want to try out GTK 3 */
/* */

typedef struct { double red, green, blue, transparency; } double_rgbt;

#pragma mark - NUMBER TO STRING CONVERSION

/**
	The following routines return a static string, chosen from a circularly used set of 32 buffers.
	You can call at most 32 of them in one Melder_casual call, for instance.
*/

const  char32 * Melder_integer  (int64 value) noexcept;
const  char   * Melder8_integer (int64 value) noexcept;

const  char32 * Melder_bigInteger  (int64 value) noexcept;
const  char   * Melder8_bigInteger (int64 value) noexcept;

const  char32 * Melder_boolean  (bool value) noexcept;
const  char   * Melder8_boolean (bool value) noexcept;
	// "yes" or "no"

/**
	Format a double value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats.
*/
const  char32 * Melder_double  (double value) noexcept;
const  char   * Melder8_double (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.9g" format.
*/
const  char32 * Melder_single  (double value) noexcept;
const  char   * Melder8_single (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.4g" format.
*/
const  char32 * Melder_half  (double value) noexcept;
const  char   * Melder8_half (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.*f" format.
*/
const  char32 * Melder_fixed  (double value, int precision) noexcept;
const  char   * Melder8_fixed (double value, int precision) noexcept;

/**
	Format a double value with a specified precision. If exponent is -2 and precision is 2, you get things like 67E-2 or 0.00024E-2.
*/
const  char32 * Melder_fixedExponent  (double value, int exponent, int precision) noexcept;
const  char   * Melder8_fixedExponent (double value, int exponent, int precision) noexcept;

/**
	Format a double value as a percentage. If precision is 3, you get things like "0" or "34.400%" or "0.014%" or "0.001%" or "0.0000007%".
*/
const  char32 * Melder_percent  (double value, int precision) noexcept;
const  char   * Melder8_percent (double value, int precision) noexcept;

/**
	Convert a formatted floating-point string to something suitable for visualization with the Graphics library.
	For instance, "1e+4" is turned into "10^^4", and "-1.23456e-78" is turned into "-1.23456\.c10^^-78".
*/
const char32 * Melder_float (const char32 *number) noexcept;

/**
	Format the number that is specified by its natural logarithm.
	For instance, -10000 is formatted as "1.135483865315339e-4343", which is a floating-point representation of exp(-10000).
*/
const  char32 * Melder_naturalLogarithm  (double lnNumber) noexcept;
const  char   * Melder8_naturalLogarithm (double lnNumber) noexcept;

const  char32 * Melder_pointer  (void *pointer) noexcept;
const  char   * Melder8_pointer (void *pointer) noexcept;

const  char32 * Melder_character  (char32_t kar) noexcept;
const  char   * Melder8_character (char32_t kar) noexcept;

const char32 * Melder_pad (int64 width, const char32 *string);   // will append spaces to the left of 'string' until 'width' is reached; no truncation
const char32 * Melder_pad (const char32 *string, int64 width);   // will append spaces to the right of 'string' until 'width' is reached; no truncation
const char32 * Melder_truncate (int64 width, const char32 *string);   // will cut away the left of 'string' until 'width' is reached; no padding
const char32 * Melder_truncate (const char32 *string, int64 width);   // will cut away the right of 'string' until 'width' is reached; no padding
const char32 * Melder_padOrTruncate (int64 width, const char32 *string);   // will cut away, or append spaces to, the left of 'string' until 'width' is reached
const char32 * Melder_padOrTruncate (const char32 *string, int64 width);   // will cut away, or append spaces to, the right of 'string' until 'width' is reached

/********** CONSOLE **********/

void Melder_writeToConsole (const char32 *message, bool useStderr);

#pragma mark - MEMORY ALLOCATION

/* These routines call malloc, free, realloc, and calloc. */
/* If out of memory, the non-f versions throw an error message (like "Out of memory"); */
/* the f versions open up a rainy day fund or crash Praat. */
/* These routines also maintain a count of the total number of blocks allocated. */

void Melder_alloc_init ();   // to be called around program start-up
void Melder_message_init ();   // to be called around program start-up
void * _Melder_malloc (int64 size);
#define Melder_malloc(type,numberOfElements)  (type *) _Melder_malloc ((numberOfElements) * (int64) sizeof (type))
void * _Melder_malloc_f (int64 size);
#define Melder_malloc_f(type,numberOfElements)  (type *) _Melder_malloc_f ((numberOfElements) * (int64) sizeof (type))
void * Melder_realloc (void *pointer, int64 size);
void * Melder_realloc_f (void *pointer, int64 size);
void * _Melder_calloc (int64 numberOfElements, int64 elementSize);
#define Melder_calloc(type,numberOfElements)  (type *) _Melder_calloc (numberOfElements, sizeof (type))
void * _Melder_calloc_f (int64 numberOfElements, int64 elementSize);
#define Melder_calloc_f(type,numberOfElements)  (type *) _Melder_calloc_f (numberOfElements, sizeof (type))
char * Melder_strdup (const char *string);
char * Melder_strdup_f (const char *string);

#define Melder_free(pointer)  _Melder_free ((void **) & (pointer))
void _Melder_free (void **pointer) noexcept;
/*
	Preconditions:
		none (*pointer may be null).
	Postconditions:
		*pointer == nullptr;
*/

int64 Melder_allocationCount ();
/*
	Returns the total number of successful calls to
	Melder_malloc, Melder_realloc (if 'ptr' is null), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_deallocationCount ();
/*
	Returns the total number of successful calls to Melder_free,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_allocationSize ();
/*
	Returns the total number of bytes allocated in calls to
	Melder_malloc, Melder_realloc (if moved), Melder_calloc, and Melder_strdup,
	since the start of the process. Mainly for debugging purposes.
*/

int64 Melder_reallocationsInSituCount ();
int64 Melder_movingReallocationsCount ();

/**
 * Text encodings.
 */
void Melder_textEncoding_prefs ();
void Melder_setInputEncoding (enum kMelder_textInputEncoding encoding);
int Melder_getInputEncoding ();
void Melder_setOutputEncoding (enum kMelder_textOutputEncoding encoding);
enum kMelder_textOutputEncoding Melder_getOutputEncoding ();

/*
 * Some other encodings. Although not used in the above set/get functions,
 * these constants should stay separate from the above encoding constants
 * because they occur in the same fields of struct MelderFile.
 */
const uint32 kMelder_textInputEncoding_FLAC = 0x464C4143;
const uint32 kMelder_textOutputEncoding_ASCII = 0x41534349;
const uint32 kMelder_textOutputEncoding_ISO_LATIN1 = 0x4C415401;
const uint32 kMelder_textOutputEncoding_FLAC = 0x464C4143;

bool Melder_isValidAscii (const char32 *string);
bool Melder_str8IsValidUtf8 (const char *string);
bool Melder_isEncodable (const char32 *string, int outputEncoding);
extern char32 Melder_decodeMacRoman [256];
extern char32 Melder_decodeWindowsLatin1 [256];

long Melder_killReturns_inline (char32 *text);
long Melder_killReturns_inline (char *text);
/*
	 Replaces all bare returns (old Mac) or return-plus-linefeed sequences (Win) with bare linefeeds
	 (generic: Unix and modern Mac).
	 Returns new length of string (equal to or less than old length).
*/

size_t str32len_utf8  (const char32 *string, bool nativizeNewlines);
size_t str32len_utf16 (const char32 *string, bool nativizeNewlines);

extern "C" char32 * Melder_peek8to32 (const char *string);
void Melder_8to32_inline (const char *source, char32 *target, int inputEncoding);
	// errors: Text is not valid UTF-8.
char32 * Melder_8to32 (const char *string, int inputEncoding);
	// errors: Out of memory; Text is not valid UTF-8.
char32 * Melder_8to32 (const char *string);
	// errors: Out of memory; Text is not valid UTF-8.

char32 * Melder_peek16to32 (const char16 *text);
char32 * Melder_16to32 (const char16 *text);

extern "C" char * Melder_peek32to8 (const char32 *string);
void Melder_32to8_inline (const char32 *string, char *utf8);
char * Melder_32to8 (const char32 *string);
char16 * Melder_32to16 (const char32 *string);
	// errors: Out of memory.

char16 * Melder_peek32to16 (const char32 *text, bool nativizeNewlines);
extern "C" char16 * Melder_peek32to16 (const char32 *string);

#ifdef _WIN32
	inline static wchar_t * Melder_peek32toW (const char32 *string) { return (wchar_t *) Melder_peek32to16 (string); }
	inline static wchar_t * Melder_32toW (const char32 *string) { return (wchar_t *) Melder_32to16 (string); }
	inline static char32 * Melder_peekWto32 (const wchar_t *string) { return Melder_peek16to32 ((const char16 *) string); }
	inline static char32 * Melder_Wto32 (const wchar_t *string) { return Melder_16to32 ((const char16 *) string); }
#endif

void Melder_str32To8bitFileRepresentation_inline (const char32 *string, char *utf8);
void Melder_8bitFileRepresentationToStr32_inline (const char *utf8, char32 *string);
const void * Melder_peek32toCfstring (const char32 *string);
void Melder_fwrite32to8 (const char32 *ptr, FILE *f);

#pragma mark - STRING TO NUMBER CONVERSION

bool Melder_isStringNumeric_nothrow (const char32 *string);
double Melder_a8tof (const char *string);
double Melder_atof (const char32 *string);
int64 Melder_atoi (const char32 *string);
	/*
	 * "3.14e-3" -> 3.14e-3
	 * "15.6%" -> 0.156
	 * "fghfghj" -> undefined
	 */
inline static long a32tol (const char32 *string) {
	if (sizeof (wchar_t) == 4) {
		return wcstol ((const wchar_t *) string, nullptr, 10);
	} else {
		return atol (Melder_peek32to8 (string));
	}
}

/********** FILES **********/

#if defined (_WIN32)
	#define Melder_DIRECTORY_SEPARATOR  '\\'
#else
	#define Melder_DIRECTORY_SEPARATOR  '/'
#endif

struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;

#define kMelder_MAXPATH 1023   /* excluding the null byte */

struct structMelderFile {
	FILE *filePointer;
	char32 path [kMelder_MAXPATH+1];
	enum class Format { none = 0, binary = 1, text = 2 } format;
	bool openForReading, openForWriting, verbose, requiresCRLF;
	unsigned long outputEncoding;
	int indent;
	struct FLAC__StreamEncoder *flacEncoder;
};
typedef struct structMelderFile *MelderFile;

struct structMelderDir {
	char32 path [kMelder_MAXPATH+1];
};
typedef struct structMelderDir *MelderDir;

const char32 * MelderFile_name (MelderFile file);
const char32 * MelderDir_name (MelderDir dir);
void Melder_pathToDir (const char32 *path, MelderDir dir);
void Melder_pathToFile (const char32 *path, MelderFile file);
void Melder_relativePathToFile (const char32 *path, MelderFile file);
const char32 * Melder_dirToPath (MelderDir dir);
	/* Returns a pointer internal to 'dir', like "/u/paul/praats" or "D:\Paul\Praats" */
const char32 * Melder_fileToPath (MelderFile file);
void MelderFile_copy (MelderFile file, MelderFile copy);
void MelderDir_copy (MelderDir dir, MelderDir copy);
bool MelderFile_equal (MelderFile file1, MelderFile file2);
bool MelderDir_equal (MelderDir dir1, MelderDir dir2);
void MelderFile_setToNull (MelderFile file);
bool MelderFile_isNull (MelderFile file);
void MelderDir_setToNull (MelderDir dir);
bool MelderDir_isNull (MelderDir dir);
void MelderDir_getFile (MelderDir parent, const char32 *fileName, MelderFile file);
void MelderDir_relativePathToFile (MelderDir dir, const char32 *path, MelderFile file);
void MelderFile_getParentDir (MelderFile file, MelderDir parent);
void MelderDir_getParentDir (MelderDir file, MelderDir parent);
bool MelderDir_isDesktop (MelderDir dir);
void MelderDir_getSubdir (MelderDir parent, const char32 *subdirName, MelderDir subdir);
void Melder_rememberShellDirectory ();
const char32 * Melder_getShellDirectory ();
void Melder_getHomeDir (MelderDir homeDir);
void Melder_getPrefDir (MelderDir prefDir);
void Melder_getTempDir (MelderDir tempDir);

bool MelderFile_exists (MelderFile file);
bool MelderFile_readable (MelderFile file);
long MelderFile_length (MelderFile file);
void MelderFile_delete (MelderFile file);

/* The following two should be combined with each other and with Windows extension setting: */
FILE * Melder_fopen (MelderFile file, const char *type);
void Melder_fclose (MelderFile file, FILE *stream);
void Melder_files_cleanUp ();

/* Use the following functions to pass unchanged text or file names to Melder_* functions. */
/* Backslashes are replaced by "\bs". */
/* The trick is that they return one of 11 cyclically used static strings, */
/* so you can use up to 11 strings in a single Melder_* call. */
char32 * Melder_peekExpandBackslashes (const char32 *message);
const char32 * MelderFile_messageName (MelderFile file);   // calls Melder_peekExpandBackslashes ()

struct structMelderReadText {
	char32 *string32, *readPointer32;
	char *string8, *readPointer8;
	unsigned long input8Encoding;
};
typedef struct structMelderReadText *MelderReadText;

MelderReadText MelderReadText_createFromFile (MelderFile file);
MelderReadText MelderReadText_createFromString (const char32 *string);
char32 MelderReadText_getChar (MelderReadText text);
char32 * MelderReadText_readLine (MelderReadText text);
wchar_t * MelderReadText_readLineW (MelderReadText text);
int64 MelderReadText_getNumberOfLines (MelderReadText me);
const char32 * MelderReadText_getLineNumber (MelderReadText text);
void MelderReadText_delete (MelderReadText text);

/* "NUM" = "NUMerics" */
/* More mathematical and numerical things than there are in <math.h>. */

/********** Inherit all the ANSI routines from math.h **********/

/* On the sgi, math.h declares some bessel functions. */
/* The following statements suppress these declarations */
/* so that the compiler will give no warnings */
/* when you redeclare y0 etc. in your code. */
#ifdef sgi
	#define y0 sgi_y0
	#define y1 sgi_y1
	#define yn sgi_yn
	#define j0 sgi_j0
	#define j1 sgi_j1
	#define jn sgi_jn
#endif
#include <math.h>
#ifdef sgi
	#undef y0
	#undef y1
	#undef yn
	#undef j0
	#undef j1
	#undef jn
#endif
#include <stdio.h>
#include <wchar.h>
#include "../sys/abcio.h"
#define NUMlog2(x)  (log (x) * NUMlog2e)

void NUMinit ();

double NUMpow (double base, double exponent);   /* Zero for non-positive base. */
void NUMshift (double *x, double xfrom, double xto);
void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto);

/********** Constants **********
 * Forty-digit constants computed by e.g.:
 *    bc -l
 *       scale=42
 *       print e(1)
 * Then rounding away the last two digits.
 */
//      print e(1)
#define NUMe  2.7182818284590452353602874713526624977572
//      print 1/l(2)
#define NUMlog2e  1.4426950408889634073599246810018921374266
//      print l(10)/l(2)
#define NUMlog2_10  3.3219280948873623478703194294893901758648
//      print 1/l(10)
#define NUMlog10e  0.4342944819032518276511289189166050822944
//      print l(2)/l(10)
#define NUMlog10_2  0.3010299956639811952137388947244930267682
//      print l(2)
#define NUMln2  0.6931471805599453094172321214581765680755
//      print l(10)
#define NUMln10  2.3025850929940456840179914546843642076011
//      print a(1)*8
#define NUM2pi  6.2831853071795864769252867665590057683943
//      print a(1)*4
#define NUMpi  3.1415926535897932384626433832795028841972
//      print a(1)*2
#define NUMpi_2  1.5707963267948966192313216916397514420986
//      print a(1)
#define NUMpi_4  0.7853981633974483096156608458198757210493
//      print 0.25/a(1)
#define NUM1_pi  0.3183098861837906715377675267450287240689
//      print 0.5/a(1)
#define NUM2_pi  0.6366197723675813430755350534900574481378
//      print sqrt(a(1)*4)
#define NUMsqrtpi  1.7724538509055160272981674833411451827975
//      print sqrt(a(1)*8)
#define NUMsqrt2pi  2.5066282746310005024157652848110452530070
//      print 1/sqrt(a(1)*8)
#define NUM1_sqrt2pi  0.3989422804014326779399460599343818684759
//      print 1/sqrt(a(1))
#define NUM2_sqrtpi  1.1283791670955125738961589031215451716881
//      print l(a(1)*4)
#define NUMlnpi  1.1447298858494001741434273513530587116473
//      print sqrt(2)
#define NUMsqrt2  1.4142135623730950488016887242096980785697
//      print sqrt(0.5)
#define NUMsqrt1_2  0.7071067811865475244008443621048490392848
//      print sqrt(3)
#define NUMsqrt3  1.7320508075688772935274463415058723669428
//      print sqrt(5)
#define NUMsqrt5  2.2360679774997896964091736687312762354406
//      print sqrt(6)
#define NUMsqrt6  2.4494897427831780981972840747058913919659
//      print sqrt(7)
#define NUMsqrt7  2.6457513110645905905016157536392604257102
//      print sqrt(8)
#define NUMsqrt8  2.8284271247461900976033774484193961571393
//      print sqrt(10)
#define NUMsqrt10  3.1622776601683793319988935444327185337196
//      print sqrt(5)/2-0.5
#define NUM_goldenSection  0.6180339887498948482045868343656381177203
// The Euler-Mascheroni constant cannot be computed by bc.
// Instead we use the 40 digits computed by Johann von Soldner in 1809.
#define NUM_euler  0.5772156649015328606065120900824024310422

/*
	Ideally, `undefined` should be #defined as NAN (or 0.0/0.0),
	because that would make sure that
		1.0 / undefined
	evaluates as undefined.
	However, we cannot do that as long as Praat contains any instances of
		if (x == undefined) { ... }
	because that condition would evaluate as false even if x were undefined
	(because NAN is unequal to NAN).
	Therefore, we must define, for the moment, `undefined` as positive infinity,
	because positive infinity can be compared to itself
	(i.e. Inf is equal to Inf). The drawback is that
		1.0 / undefined
	will evaluate as 0.0, i.e. this version of `undefined` does not propagate properly.
*/
#define undefined  (0.0/0.0)
//#define undefined  NAN   /* a future definition? */
//#define undefined  (0.0/0.0)   /* an alternative future definition */

/*
	Ideally, isdefined() should capture not only `undefined`, but all infinities and nans.
	This can be done with a single test for the set bits in 0x7FF0000000000000,
	at least for 64-bit IEEE implementations. The correctness of this assumption is checked in sys/praat.cpp.
	The portable version of isdefined() would involve both isinf() and isnan(),
	but that would be slower (as tested in fon/Praat_tests.cpp)
	and it would also get into problems on some platforms whenever both <cmath> and <math.h> are included,
	as in dwsys/NUMcomplex.cpp.
*/
//#define isdefined(x)  ((x) != NUMundefined)   /* an old definition, not good at capturing nans */
//inline static bool isdefined (double x) { return ! isinf (x) && ! isnan (x); }   /* portable */
inline static bool isdefined (double x) { return ((* (uint64_t *) & x) & 0x7FF0000000000000) != 0x7FF0000000000000; }
inline static bool isundef (double x) { return ((* (uint64_t *) & x) & 0x7FF0000000000000) == 0x7FF0000000000000; }

/********** Arrays with one index (NUMarrays.cpp) **********/

void * NUMvector (long elementSize, long lo, long hi, bool zero);
/*
	Function:
		create a vector [lo...hi]; if `zero`, then all values are initialized to 0.
	Preconditions:
		hi >= lo;
*/

void NUMvector_free (long elementSize, void *v, long lo) noexcept;
/*
	Function:
		destroy a vector v that was created with NUMvector.
	Preconditions:
		lo must have the same values as with the creation of the vector.
*/

void * NUMvector_copy (long elementSize, void *v, long lo, long hi);
/*
	Function:
		copy (part of) a vector v, which need not have been created with NUMvector, to a new one.
	Preconditions:
		if v != nullptr, the values v [lo..hi] must exist.
*/

void NUMvector_copyElements (long elementSize, void *v, void *to, long lo, long hi);
/*
	copy the vector elements v [lo..hi] to those of a vector 'to'.
	These vectors need not have been created by NUMvector.
*/

bool NUMvector_equal (long elementSize, void *v1, void *v2, long lo, long hi);
/*
	return true if the vector elements v1 [lo..hi] are equal
	to the corresponding elements of the vector v2; otherwise, return false.
	The vectors need not have been created by NUMvector.
*/

void NUMvector_append (long elementSize, void **v, long lo, long *hi);
void NUMvector_insert (long elementSize, void **v, long lo, long *hi, long position);
/*
	add one element to the vector *v.
	The new element is initialized to zero.
	On success, *v points to the new vector, and *hi is incremented by 1.
	On failure, *v and *hi are not changed.
*/

/********** Arrays with two indices (NUMarrays.cpp) **********/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2, bool zero);
/*
	Function:
		create a matrix [row1...row2] [col1...col2]; if `zero`, then all values are initialized to 0.
	Preconditions:
		row2 >= row1;
		col2 >= col1;
*/

void NUMmatrix_free (long elementSize, void *m, long row1, long col1) noexcept;
/*
	Function:
		destroy a matrix m created with NUM...matrix.
	Preconditions:
		if m != nullptr: row1 and col1
		must have the same value as with the creation of the matrix.
*/

void * NUMmatrix_copy (long elementSize, void *m, long row1, long row2, long col1, long col2);
/*
	Function:
		copy (part of) a matrix m, wich does not have to be created with NUMmatrix, to a new one.
	Preconditions:
		if m != nullptr: the values m [rowmin..rowmax] [colmin..colmax] must exist.
*/

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2);
/*
	copy the matrix elements m [r1..r2] [c1..c2] to those of a matrix 'to'.
	These matrices need not have been created by NUMmatrix.
*/

bool NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2);
/*
	return 1 if the matrix elements m1 [r1..r2] [c1..c2] are equal
	to the corresponding elements of the matrix m2; otherwise, return 0.
	The matrices need not have been created by NUM...matrix.
*/

long NUM_getTotalNumberOfArrays ();   // for debugging

/********** Special functions (NUM.cpp) **********/

double NUMlnGamma (double x);
double NUMbeta (double z, double w);
double NUMbesselI (long n, double x);   // precondition: n >= 0
double NUMbessel_i0_f (double x);
double NUMbessel_i1_f (double x);
double NUMbesselK (long n, double x);   // preconditions: n >= 0 && x > 0.0
double NUMbessel_k0_f (double x);
double NUMbessel_k1_f (double x);
double NUMbesselK_f (long n, double x);
double NUMsigmoid (double x);   // correct also for large positive or negative x
double NUMinvSigmoid (double x);
double NUMerfcc (double x);
double NUMgaussP (double z);
double NUMgaussQ (double z);
double NUMincompleteGammaP (double a, double x);
double NUMincompleteGammaQ (double a, double x);
double NUMchiSquareP (double chiSquare, double degreesOfFreedom);
double NUMchiSquareQ (double chiSquare, double degreesOfFreedom);
double NUMcombinations (long n, long k);
double NUMincompleteBeta (double a, double b, double x);   // incomplete beta function Ix(a,b). Preconditions: a, b > 0; 0 <= x <= 1
double NUMbinomialP (double p, double k, double n);
double NUMbinomialQ (double p, double k, double n);
double NUMinvBinomialP (double p, double k, double n);
double NUMinvBinomialQ (double p, double k, double n);

/********** Auditory modelling (NUMear.cpp) **********/

double NUMhertzToBark (double hertz);
double NUMbarkToHertz (double bark);
double NUMphonToDifferenceLimens (double phon);
double NUMdifferenceLimensToPhon (double ndli);
double NUMsoundPressureToPhon (double soundPressure, double bark);
double NUMhertzToMel (double hertz);
double NUMmelToHertz (double mel);
double NUMhertzToSemitones (double hertz);
double NUMsemitonesToHertz (double semitones);
double NUMerb (double f);
double NUMhertzToErb (double hertz);
double NUMerbToHertz (double erb);

/********** Sorting (NUMsort.cpp) **********/

void NUMsort_d (long n, double ra []);   // heap sort
void NUMsort_i (long n, int ra []);
void NUMsort_l (long n, long ra []);
void NUMsort_str (long n, char32 *a []);
void NUMsort_p (long n, void *a [], int (*compare) (const void *, const void *));

double NUMquantile (long n, double a [], double factor);
/*
	An estimate of the quantile 'factor' (between 0 and 1) of the distribution
	from which the set 'a [1..n]' is a sorted array of random samples.
	For instance, if 'factor' is 0.5, this function returns an estimate of
	the median of the distribution underlying the sorted set a [].
	If your array has not been sorted, first sort it with NUMsort (n, a).
*/

/********** Interpolation and optimization (NUM.cpp) **********/

// Special values for interpolationDepth:
#define NUM_VALUE_INTERPOLATE_NEAREST  0
#define NUM_VALUE_INTERPOLATE_LINEAR  1
#define NUM_VALUE_INTERPOLATE_CUBIC  2
// Higher values than 2 yield a true sinc interpolation. Here are some examples:
#define NUM_VALUE_INTERPOLATE_SINC70  70
#define NUM_VALUE_INTERPOLATE_SINC700  700
double NUM_interpolate_sinc (double y [], long nx, double x, long interpolationDepth);

#define NUM_PEAK_INTERPOLATE_NONE  0
#define NUM_PEAK_INTERPOLATE_PARABOLIC  1
#define NUM_PEAK_INTERPOLATE_CUBIC  2
#define NUM_PEAK_INTERPOLATE_SINC70  3
#define NUM_PEAK_INTERPOLATE_SINC700  4

double NUMimproveExtremum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real, int isMaximum);
double NUMimproveMaximum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);
double NUMimproveMinimum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);

void NUM_viterbi (
	long numberOfFrames, long maxnCandidates,
	long (*getNumberOfCandidates) (long iframe, void *closure),
	double (*getLocalCost) (long iframe, long icand, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, void *closure),
	void (*putResult) (long iframe, long place, void *closure),
	void *closure);

void NUM_viterbi_multi (
	long nframe, long ncand, int ntrack,
	double (*getLocalCost) (long iframe, long icand, int itrack, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, int itrack, void *closure),
	void (*putResult) (long iframe, long place, int itrack, void *closure),
	void *closure);

/********** Metrics (NUM.cpp) **********/

int NUMrotationsPointInPolygon
	(double x0, double y0, long n, double x [], double y []);
/*
	Returns the number of times that the closed polygon
	(x [1], y [1]), (x [2], y [2]),..., (x [n], y [n]), (x [1], y [1]) encloses the point (x0, y0).
	The result is positive if the polygon encloses the point in the
	anti-clockwise direction, and negative if the direction is clockwise.
	The result is 0 if the point is outside the polygon.
	If the point is on the polygon, the result is unpredictable.
*/

/********** Random numbers (NUMrandom.cpp) **********/

void NUMrandom_init ();   // automatically called by NUMinit ();

double NUMrandomFraction ();
double NUMrandomFraction_mt (int threadNumber);

double NUMrandomUniform (double lowest, double highest);

long NUMrandomInteger (long lowest, long highest);

bool NUMrandomBernoulli (double probability);
double NUMrandomBernoulli_real (double probability);

double NUMrandomGauss (double mean, double standardDeviation);
double NUMrandomGauss_mt (int threadNumber, double mean, double standardDeviation);

double NUMrandomPoisson (double mean);

uint32 NUMhashString (const char32 *string);

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2);
void NUMfilterSecondOrderSection_a (double x [], long n, double a1, double a2);
void NUMfilterSecondOrderSection_fb (double x [], long n, double dt, double formant, double bandwidth);
double NUMftopreemphasis (double f, double dt);
void NUMpreemphasize_a (double x [], long n, double preemphasis);
void NUMdeemphasize_a (double x [], long n, double preemphasis);
void NUMpreemphasize_f (double x [], long n, double dt, double frequency);
void NUMdeemphasize_f (double x [], long n, double dt, double frequency);
void NUMautoscale (double x [], long n, double scale);

/* The following ANSI-C power trick generates the declarations of 156 functions. */
#define FUNCTION(type,storage)  \
	void NUMvector_writeText_##storage (const type *v, long lo, long hi, MelderFile file, const char32 *name); \
	void NUMvector_writeBinary_##storage (const type *v, long lo, long hi, FILE *f); \
	type * NUMvector_readText_##storage (long lo, long hi, MelderReadText text, const char *name); \
	type * NUMvector_readBinary_##storage (long lo, long hi, FILE *f); \
	void NUMmatrix_writeText_##storage (type **v, long r1, long r2, long c1, long c2, MelderFile file, const char32 *name); \
	void NUMmatrix_writeBinary_##storage (type **v, long r1, long r2, long c1, long c2, FILE *f); \
	type ** NUMmatrix_readText_##storage (long r1, long r2, long c1, long c2, MelderReadText text, const char *name); \
	type ** NUMmatrix_readBinary_##storage (long r1, long r2, long c1, long c2, FILE *f);
FUNCTION (signed char, i8)
FUNCTION (int, i16)
FUNCTION (long, i32)
FUNCTION (unsigned char, u8)
FUNCTION (unsigned int, u16)
FUNCTION (unsigned long, u32)
FUNCTION (double, r32)
FUNCTION (double, r64)
FUNCTION (fcomplex, c64)
FUNCTION (dcomplex, c128)
#undef FUNCTION

/*
void NUMvector_writeBinary_r64 (const double *v, long lo, long hi, FILE *f);   // etc
	write the vector elements v [lo..hi] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUM...vector.
double * NUMvector_readText_r64 (long lo, long hi, MelderReadText text, const char *name);   // etc
	create and read a vector as text.
	Throw an error message if anything went wrong.
	Every element is supposed to be on the beginning of a line.
double * NUMvector_readBinary_r64 (long lo, long hi, FILE *f);   // etc
	create and read a vector as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
void NUMvector_writeText_r64 (const double *v, long lo, long hi, MelderFile file, const char32 *name);   // etc
	write the vector elements v [lo..hi] as text to the open file,
	each element on its own line, preceded by "name [index]: ".
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUMvector.
void NUMmatrix_writeText_r64 (double **m, long r1, long r2, long c1, long c2, MelderFile file, const char32 *name);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as text to the open file.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
void NUMmatrix_writeBinary_r64 (double **m, long r1, long r2, long c1, long c2, FILE *f);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
double ** NUMmatrix_readText_r64 (long r1, long r2, long c1, long c2, MelderReadText text, const char *name);   // etc
	create and read a matrix as text.
	Throw an error message if anything went wrong.
double ** NUMmatrix_readBinary_r64 (long r1, long r2, long c1, long c2, FILE *f);   // etc
	create and read a matrix as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
*/

typedef struct structNUMlinprog *NUMlinprog;
void NUMlinprog_delete (NUMlinprog me);
NUMlinprog NUMlinprog_new (bool maximize);
void NUMlinprog_addVariable (NUMlinprog me, double lowerBound, double upperBound, double coeff);
void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound);
void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient);
void NUMlinprog_run (NUMlinprog me);
double NUMlinprog_getPrimalValue (NUMlinprog me, long ivar);

template <class T>
T* NUMvector (long from, long to) {
	T* result = static_cast <T*> (NUMvector (sizeof (T), from, to, true));
	return result;
}

template <class T>
T* NUMvector (long from, long to, bool zero) {
	T* result = static_cast <T*> (NUMvector (sizeof (T), from, to, zero));
	return result;
}

template <class T>
void NUMvector_free (T* ptr, long from) noexcept {
	NUMvector_free (sizeof (T), ptr, from);
}

template <class T>
T* NUMvector_copy (T* ptr, long lo, long hi) {
	T* result = static_cast <T*> (NUMvector_copy (sizeof (T), ptr, lo, hi));
	return result;
}

template <class T>
bool NUMvector_equal (T* v1, T* v2, long lo, long hi) {
	return NUMvector_equal (sizeof (T), v1, v2, lo, hi);
}

template <class T>
void NUMvector_copyElements (T* vfrom, T* vto, long lo, long hi) {
	NUMvector_copyElements (sizeof (T), vfrom, vto, lo, hi);
}

template <class T>
void NUMvector_append (T** v, long lo, long *hi) {
	NUMvector_append (sizeof (T), (void**) v, lo, hi);
}

template <class T>
void NUMvector_insert (T** v, long lo, long *hi, long position) {
	NUMvector_insert (sizeof (T), (void**) v, lo, hi, position);
}

template <class T>
class autoNUMvector {
	T* d_ptr;
	long d_from;
public:
	autoNUMvector<T> (long from, long to) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, true);
	}
	autoNUMvector<T> (long from, long to, bool zero) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, zero);
	}
	autoNUMvector (T *ptr, long from) : d_ptr (ptr), d_from (from) {
	}
	autoNUMvector () : d_ptr (nullptr), d_from (1) {
	}
	~autoNUMvector<T> () {
		if (d_ptr) NUMvector_free (sizeof (T), d_ptr, d_from);
	}
	T& operator[] (long i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = nullptr;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long from, long to) {
		if (d_ptr) {
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, true);
	}
	void reset (long from, long to, bool zero) {
		if (d_ptr) {
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, zero);
	}
};

template <class T>
T** NUMmatrix (long row1, long row2, long col1, long col2) {
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2, true));
	return result;
}

template <class T>
T** NUMmatrix (long row1, long row2, long col1, long col2, bool zero) {
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2, zero));
	return result;
}

template <class T>
void NUMmatrix_free (T** ptr, long row1, long col1) noexcept {
	NUMmatrix_free (sizeof (T), ptr, row1, col1);
}

template <class T>
T** NUMmatrix_copy (T** ptr, long row1, long row2, long col1, long col2) {
	#if 1
	T** result = static_cast <T**> (NUMmatrix_copy (sizeof (T), ptr, row1, row2, col1, col2));
	#else
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2));
	for (long irow = row1; irow <= row2; irow ++)
		for (long icol = col1; icol <= col2; icol ++)
			result [irow] [icol] = ptr [irow] [icol];
	#endif
	return result;
}

template <class T>
bool NUMmatrix_equal (T** m1, T** m2, long row1, long row2, long col1, long col2) {
	return NUMmatrix_equal (sizeof (T), m1, m2, row1, row2, col1, col2);
}

template <class T>
void NUMmatrix_copyElements (T** mfrom, T** mto, long row1, long row2, long col1, long col2) {
	NUMmatrix_copyElements (sizeof (T), mfrom, mto, row1, row2, col1, col2);
}

template <class T>
class autoNUMmatrix {
	T** d_ptr;
	long d_row1, d_col1;
public:
	autoNUMmatrix (long row1, long row2, long col1, long col2) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	autoNUMmatrix (long row1, long row2, long col1, long col2, bool zero) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
	autoNUMmatrix (T **ptr, long row1, long col1) : d_ptr (ptr), d_row1 (row1), d_col1 (col1) {
	}
	autoNUMmatrix () : d_ptr (nullptr), d_row1 (0), d_col1 (0) {
	}
	~autoNUMmatrix () {
		if (d_ptr) NUMmatrix_free (sizeof (T), d_ptr, d_row1, d_col1);
	}
	T*& operator[] (long row) {
		return d_ptr [row];
	}
	T** peek () const {
		return d_ptr;
	}
	T** transfer () {
		T** temp = d_ptr;
		d_ptr = nullptr;
		return temp;
	}
	void reset (long row1, long row2, long col1, long col2) {
		if (d_ptr) {
			NUMmatrix_free (sizeof (T), d_ptr, d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	void reset (long row1, long row2, long col1, long col2, bool zero) {
		if (d_ptr) {
			NUMmatrix_free (sizeof (T), d_ptr, d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
};

template <class T>
class autodatavector {
	T* d_ptr;
	long d_from, d_to;
public:
	autodatavector<T> (long from, long to) : d_from (from), d_to (to) {
		d_ptr = NUMvector<T> (from, to, true);
	}
	autodatavector<T> (long from, long to, bool zero) : d_from (from), d_to (to) {
		d_ptr = NUMvector<T> (from, to, zero);
	}
	autodatavector (T *ptr, long from, long to) : d_ptr (ptr), d_from (from), d_to (to) {
	}
	autodatavector () : d_ptr (nullptr), d_from (1), d_to (0) {
	}
	~autodatavector<T> () {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				Melder_free (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
		}
	}
	T& operator[] (long i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = nullptr;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long from, long to) {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				Melder_free (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;   // this assignment is safe, because d_ptr is null
		d_to = to;
		d_ptr = NUMvector<T> (from, to, true);
	}
	void reset (long from, long to, bool zero) {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				Melder_free (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;   // this assignment is safe, because d_ptr is null
		d_to = to;
		d_ptr = NUMvector<T> (from, to, zero);
	}
};

typedef autodatavector <char32 *> autostring32vector;
typedef autodatavector <char *> autostring8vector;

#pragma mark - TENSOR
/*
	numvec and nummat: the type declarations are in melder.h, the function declarations in tensor.h

	Initialization (tested in praat.cpp):
		numvec x;                  // does not initialize x
		numvec x { };              // initializes x.at to nullptr and x.size to 0
		numvec x { 100, false };   // initializes x to 100 uninitialized values
		numvec x { 100, true };    // initializes x to 100 zeroes
		NUMvector<double> a (1, 100);
		numvec x { a, 100 };       // initializes x to 100 values from a base-1 array

		autonumvec y;                  // initializes y.at to nullptr and y.size to 0
		autonumvec y { 100, false };   // initializes y to 100 uninitialized values, having ownership
		autonumvec y { 100, true };    // initializes y to 100 zeroes, having ownership
		autonumvec y { x };            // initializes y to the content of x, taking ownership (explicit, so not "y = x")
		numvec z = releaseToAmbiguousOwner();   // releases ownership, x.at becoming nullptr
		"}"                            // end of scope destroys x.at if not nullptr
		autonumvec z = y.move()        // moves the content of y to z, emptying y

	To return an autonumvec from a function, transfer ownership like this:
		autonumvec foo () {
			autonumvec x { 100, false };
			... // fill in the 100 values
			return x;
		}
*/

class autonumvec;   // forward declaration, needed in the declaration of numvec

class numvec {
public:
	double *at;
	long size;
public:
	numvec () = default;   // for use in a union
	numvec (double *givenAt, long givenSize): at (givenAt), size (givenSize) { }
	numvec (long givenSize, bool zero) {
		our _initAt (givenSize, zero);
		our size = givenSize;
	}
	numvec (const numvec& other) = default;
	numvec (const autonumvec& other) = delete;
	numvec& operator= (const numvec&) = default;
	numvec& operator= (const autonumvec&) = delete;
	double& operator[] (long i) {
		return our at [i];
	}
	void reset () noexcept {
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our size = 0;
	}
protected:
	void _initAt (long givenSize, bool zero);
	void _freeAt () noexcept;
};

#define empty_numvec  numvec { nullptr, 0 }

/*
	An autonumvec is the sole owner of its payload, which is a numvec.
	When the autonumvec ends its life (goes out of scope),
	it should destroy its payload (if it has not sold it),
	because keeping a payload alive when the owner is dead
	would continue to use some of the computer's resources (namely, memory).
*/
class autonumvec : public numvec {
public:
	autonumvec (): numvec (nullptr, 0) { }   // come into existence without a payload
	autonumvec (long givenSize, bool zero): numvec (givenSize, zero) { }   // come into existence and manufacture a payload
	autonumvec (double *givenAt, long givenSize): numvec (givenAt, givenSize) { }   // come into existence and buy a payload from a non-autonumvec
	explicit autonumvec (numvec x): numvec (x.at, x.size) { }   // come into existence and buy a payload from a non-autonumvec (disable implicit conversion)
	~autonumvec () {   // destroy the payload (if any)
		if (our at) our _freeAt ();
	}
	numvec get () const { return { our at, our size }; }   // let the public use the payload (they may change the values of the elements but not the at-pointer or the size)
	numvec releaseToAmbiguousOwner () {   // sell the payload to a non-autonumvec
		double *oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return { oldAt, our size };
	}
	void reset () {   // destroy the current payload (if any) and have no new payload
		our numvec :: reset ();
	}
	void reset (long newSize, bool zero) {   // destroy the current payload (if any) and manufacture a new payload
		our numvec :: reset ();   // exception guarantee: leave *this in a reasonable state...
		our _initAt (newSize, zero);   // ...in case this line throws an exception
		our size = newSize;
	}
	void reset (double *newAt, long newSize) {   // destroy the current payload (if any) and buy a new payload
		if (our at) our _freeAt ();
		our at = newAt;
		our size = newSize;
	}
	void reset (numvec newX) {   // destroy the current payload (if any) and buy a new payload
		if (our at) our _freeAt ();
		our at = newX.at;
		our size = newX.size;
	}
	/*
		Disable copying via construction or assignment (which would violate unique ownership of the payload).
	*/
	autonumvec (const autonumvec&) = delete;   // disable copy constructor
	autonumvec& operator= (const autonumvec&) = delete;   // disable copy assignment
	/*
		Enable moving of temporaries or (for variables) via an explicit move().
		This implements buying a payload from another autonumvec (which involves destroying our current payload).
	*/
	autonumvec (autonumvec&& other) noexcept : numvec { other.get() } {   // enable move constructor for r-values (temporaries)
		other.at = nullptr;   // disown source
	}
	autonumvec& operator= (autonumvec&& other) noexcept {   // enable move assignment for r-values (temporaries)
		if (other.at != our at) {
			if (our at) our _freeAt ();
			our at = other.at;
			our size = other.size;
			other.at = nullptr;   // disown source
			other.size = 0;   // needed only if you insist on keeping the source in a valid state
		}
		return *this;
	}
	autonumvec&& move () noexcept { return static_cast <autonumvec&&> (*this); }   // enable constriction and assignment for l-values (variables) via explicit move()
};

class autonummat;   // forward declaration, needed in the declaration of nummat

class nummat {
public:
	double **at;
	long nrow, ncol;
public:
	nummat () = default;   // for use in a union
	nummat (double **givenAt, long givenNrow, long givenNcol): at (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	nummat (long givenNrow, long givenNcol, bool zero) {
		our _initAt (givenNrow, givenNcol, zero);
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	nummat (const nummat& other) = default;
	nummat (const autonummat& other) = delete;
	nummat& operator= (const nummat&) = default;
	nummat& operator= (const autonummat&) = delete;
	double *& operator[] (long i) {
		return our at [i];
	}
	void reset () noexcept {
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our nrow = 0;
		our ncol = 0;
	}
protected:
	void _initAt (long givenNrow, long givenNcol, bool zero);
	void _freeAt () noexcept;
};

#define empty_nummat  nummat { nullptr, 0, 0 }

/*
	An autonummat is the sole owner of its payload, which is a nummat.
	When the autonummat ends its life (goes out of scope),
	it should destroy its payload (if it has not sold it),
	because keeping a payload alive when the owner is dead
	would continue to use some of the computer's resources (namely, memory).
*/
class autonummat : public nummat {
public:
	autonummat (): nummat { nullptr, 0, 0 } { }   // come into existence without a payload
	autonummat (long givenNrow, long givenNcol, bool zero): nummat { givenNrow, givenNcol, zero } { }   // come into existence and manufacture a payload
	autonummat (double **givenAt, long givenNrow, long givenNcol): nummat (givenAt, givenNrow, givenNcol) { }   // come into existence and buy a payload from a non-autonummat
	explicit autonummat (nummat x): nummat (x.at, x.nrow, x.ncol) { }   // come into existence and buy a payload from a non-autonummat (disable implicit conversion)
	~autonummat () {   // destroy the payload (if any)
		if (our at) our _freeAt ();
	}
	nummat get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	nummat releaseToAmbiguousOwner () {   // sell the payload to a non-autonummat
		double **oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return { oldAt, our nrow, our ncol };
	}
	void reset () {   // destroy the current payload (if any) and have no new payload
		our nummat :: reset ();
	}
	void reset (long newNrow, long newNcol, bool zero) {   // destroy the current payload (if any) and manufacture a new payload
		our nummat :: reset ();   // exception guarantee: leave *this in a reasonable state...
		our _initAt (newNrow, newNcol, zero);   // ...in case this line throws an exception
		our nrow = newNrow;
		our ncol = newNcol;
	}
	void reset (double **newAt, long newNrow, long newNcol) {   // destroy the current payload (if any) and buy a new payload
		if (our at) our _freeAt ();
		our at = newAt;
		our nrow = newNrow;
		our ncol = newNcol;
	}
	void reset (nummat newX) {   // destroy the current payload (if any) and buy a new payload
		if (our at) our _freeAt ();
		our at = newX.at;
		our nrow = newX.nrow;
		our ncol = newX.ncol;
	}
	/*
		Disable copying via construction or assignment (which would violate unique ownership of the payload).
	*/
	autonummat (const autonummat&) = delete;   // disable copy constructor
	autonummat& operator= (const autonummat&) = delete;   // disable copy assignment
	/*
		Enable moving of temporaries or (for variables) via an explicit move().
		This implements buying a payload from another autonummat (which involves destroying our current payload).
	*/
	autonummat (autonummat&& other) noexcept : nummat { other.get() } {   // enable move constructor for r-values (temporaries)
		other.at = nullptr;   // disown source
	}
	autonummat& operator= (autonummat&& other) noexcept {   // enable move assignment for r-values (temporaries)
		if (other.at != our at) {
			if (our at) our _freeAt ();
			our at = other.at;
			our nrow = other.nrow;
			our ncol = other.ncol;
			other.at = nullptr;   // disown source
			other.nrow = 0;   // needed only if you insist on keeping the source in a valid state
			other.ncol = 0;
		}
		return *this;
	}
	autonummat&& move () noexcept { return static_cast <autonummat&&> (*this); }
};

#pragma mark - ARGUMENTS

const  char32 * Melder_numvec  (numvec value);
const  char32 * Melder_nummat  (nummat value);
typedef class structThing *Thing;   // forward declaration
const char32 * Thing_messageName (Thing me);
struct MelderArg {
	const char32 *_arg;
	/*
		The types of arguments that never involve memory allocation:
	*/
	MelderArg (const char32 *            arg) : _arg (arg) { }
	//MelderArg (const char   *            arg) : _arg (Melder_peek8to32 (arg)) { }
	MelderArg (const double              arg) : _arg (Melder_double          (arg)) { }
	MelderArg (const          long long  arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned long long  arg) : _arg (Melder_integer         ((int64) arg)) { }
	MelderArg (const          long       arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned long       arg) : _arg (Melder_integer         ((int64) arg)) { }   // ignore ULL above 2^63
	MelderArg (const          int        arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned int        arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const          short      arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned short      arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const char32_t            arg) : _arg (Melder_character       (arg)) { }
	MelderArg (void *                    arg) : _arg (Melder_integer         ((int64) arg)) { }
	/*
		The types of arguments that sometimes involve memory allocation:
	*/
	MelderArg (numvec                    arg) : _arg (Melder_numvec          (arg)) { }
	MelderArg (nummat                    arg) : _arg (Melder_nummat          (arg)) { }
	MelderArg (Thing                     arg) : _arg (Thing_messageName      (arg)) { }
	MelderArg (MelderFile                arg) : _arg (MelderFile_messageName (arg)) { }
};

#define Melder_1_ARG \
	const MelderArg& arg1
#define Melder_2_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2
#define Melder_3_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3
#define Melder_4_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4
#define Melder_5_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5
#define Melder_6_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6
#define Melder_7_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7
#define Melder_8_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8
#define Melder_9_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9
#define Melder_10_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10
#define Melder_11_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11
#define Melder_12_OR_13_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13 = U""
#define Melder_13_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13
#define Melder_14_OR_15_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15 = U""
#define Melder_15_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15
#define Melder_16_TO_19_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15, const MelderArg& arg16, \
	const MelderArg& arg17 = U"", const MelderArg& arg18 = U"", const MelderArg& arg19 = U""
#define Melder_19_ARGS \
	const MelderArg& arg1,  const MelderArg& arg2,  const MelderArg& arg3,  const MelderArg& arg4, \
	const MelderArg& arg5,  const MelderArg& arg6,  const MelderArg& arg7,  const MelderArg& arg8, \
	const MelderArg& arg9,  const MelderArg& arg10, const MelderArg& arg11, const MelderArg& arg12, \
	const MelderArg& arg13, const MelderArg& arg14, const MelderArg& arg15, const MelderArg& arg16, \
	const MelderArg& arg17, const MelderArg& arg18, const MelderArg& arg19

#define Melder_1_ARG_CALL \
	arg1._arg
#define Melder_2_ARGS_CALL \
	arg1._arg, arg2._arg
#define Melder_3_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg
#define Melder_4_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg
#define Melder_5_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg
#define Melder_6_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg
#define Melder_7_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg
#define Melder_8_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg
#define Melder_9_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg
#define Melder_10_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg, arg10._arg
#define Melder_11_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg, arg10._arg, \
	arg11._arg
#define Melder_13_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg, arg10._arg, \
	arg11._arg, arg12._arg, arg13._arg
#define Melder_15_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg, arg10._arg, \
	arg11._arg, arg12._arg, arg13._arg, arg14._arg, arg15._arg
#define Melder_19_ARGS_CALL \
	arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg, arg6._arg, arg7._arg, arg8._arg, arg9._arg, arg10._arg, \
	arg11._arg, arg12._arg, arg13._arg, arg14._arg, arg15._arg, arg16._arg, arg17._arg, arg18._arg, arg19._arg

void Melder_tracingToFile (MelderFile file);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_1_ARG);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_2_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_3_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_4_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_5_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_6_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_7_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_8_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_9_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_10_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_11_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_12_OR_13_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_14_OR_15_ARGS);
void Melder_trace (const char *fileName, int lineNumber, const char *functionName, Melder_16_TO_19_ARGS);
#ifdef NDEBUG
	#define Melder_assert(x)   ((void) 0)
	#define trace(x)   ((void) 0)
#else
	#define Melder_assert(x)   ((x) ? (void) (0) : (Melder_assert_ (__FILE__, __LINE__, #x), abort ()))
	#define trace(...)   (! Melder_isTracing ? (void) 0 : Melder_trace (__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#endif

/* So these will be the future replacements for the above, as soon as we rid of text files: */
MelderFile MelderFile_open (MelderFile file);
MelderFile MelderFile_append (MelderFile file);
MelderFile MelderFile_create (MelderFile file);
void * MelderFile_read (MelderFile file, long nbytes);
char * MelderFile_readLine (MelderFile file);
void MelderFile_writeCharacter (MelderFile file, wchar_t kar);
void MelderFile_writeCharacter (MelderFile file, char32 kar);
void MelderFile_write (MelderFile file, Melder_1_ARG);
void MelderFile_write (MelderFile file, Melder_2_ARGS);
void MelderFile_write (MelderFile file, Melder_3_ARGS);
void MelderFile_write (MelderFile file, Melder_4_ARGS);
void MelderFile_write (MelderFile file, Melder_5_ARGS);
void MelderFile_write (MelderFile file, Melder_6_ARGS);
void MelderFile_write (MelderFile file, Melder_7_ARGS);
void MelderFile_write (MelderFile file, Melder_8_ARGS);
void MelderFile_write (MelderFile file, Melder_9_ARGS);
void MelderFile_write (MelderFile file, Melder_10_ARGS);
void MelderFile_write (MelderFile file, Melder_11_ARGS);
void MelderFile_write (MelderFile file, Melder_12_OR_13_ARGS);
void MelderFile_write (MelderFile file, Melder_14_OR_15_ARGS);
void MelderFile_write (MelderFile file, Melder_16_TO_19_ARGS);
void MelderFile_rewind (MelderFile file);
void MelderFile_seek (MelderFile file, long position, int direction);
long MelderFile_tell (MelderFile file);
void MelderFile_close (MelderFile file);
void MelderFile_close_nothrow (MelderFile file);

/* Read and write whole text files. */
char32 * MelderFile_readText (MelderFile file);
void MelderFile_writeText (MelderFile file, const char32 *text, enum kMelder_textOutputEncoding outputEncoding);
void MelderFile_appendText (MelderFile file, const char32 *text);

void Melder_createDirectory (MelderDir parent, const char32 *subdirName, int mode);

void Melder_getDefaultDir (MelderDir dir);
void Melder_setDefaultDir (MelderDir dir);
void MelderFile_setDefaultDir (MelderFile file);

/*
 * Some often used characters.
 */
#define U_SPACE  U" "
#define U_TAB  U"\t"
#define U_NEWLINE  U"\n"
#define U_COMMA  U","
#define U_COMMA_  U", "
#define U_PERIOD  U"."
#define U_LEFT_SINGLE_QUOTE  U"\u2018"
#define U_RIGHT_SINGLE_QUOTE  U"\u2019"
#define U_LEFT_DOUBLE_QUOTE  U"\u201c"
#define U_RIGHT_DOUBLE_QUOTE  U"\u201d"
#define U_LEFT_GUILLEMET  U"\u00ab"
#define U_RIGHT_GUILLEMET  U"\u00bb"

/********** STRINGS **********/

/* These are routines for never having to check string boundaries again. */

typedef struct {
	int64 length;
	int64 bufferSize;
	char16 *string;   // a growing buffer, rarely shrunk (can only be freed by MelderString16_free)
} MelderString16;
typedef struct {
	int64 length;
	int64 bufferSize;
	char32 *string;   // a growing buffer, rarely shrunk (can only be freed by MelderString32_free)
} MelderString;

void MelderString16_free (MelderString16 *me);   // frees the buffer (and sets other attributes to zero)
void MelderString_free (MelderString *me);   // frees the buffer (and sets other attributes to zero)
void MelderString16_empty (MelderString16 *me);   // sets to empty string (buffer shrunk if very large)
void MelderString_empty (MelderString *me);   // sets to empty string (buffer shrunk if very large)
void MelderString_expand (MelderString *me, int64 sizeNeeded);   // increases the buffer size; there's normally no need to call this
void MelderString_copy (MelderString *me, Melder_1_ARG);
void MelderString_copy (MelderString *me, Melder_2_ARGS);
void MelderString_copy (MelderString *me, Melder_3_ARGS);
void MelderString_copy (MelderString *me, Melder_4_ARGS);
void MelderString_copy (MelderString *me, Melder_5_ARGS);
void MelderString_copy (MelderString *me, Melder_6_ARGS);
void MelderString_copy (MelderString *me, Melder_7_ARGS);
void MelderString_copy (MelderString *me, Melder_8_ARGS);
void MelderString_copy (MelderString *me, Melder_9_ARGS);
void MelderString_copy (MelderString *me, Melder_10_ARGS);
void MelderString_copy (MelderString *me, Melder_11_ARGS);
void MelderString_copy (MelderString *me, Melder_12_OR_13_ARGS);
void MelderString_copy (MelderString *me, Melder_14_OR_15_ARGS);
void MelderString_copy (MelderString *me, Melder_16_TO_19_ARGS);
void MelderString_ncopy (MelderString *me, const char32 *source, int64 n);

inline static void MelderString_append (MelderString *me, Melder_1_ARG) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	int64 sizeNeeded = me -> length + length1 + 1;
	if (sizeNeeded > me -> bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (me -> string + me -> length, s1);   me -> length += length1;
}
/*void MelderString_append (MelderString *me, Melder_1_ARG);*/
void MelderString_append (MelderString *me, Melder_2_ARGS);
void MelderString_append (MelderString *me, Melder_3_ARGS);
void MelderString_append (MelderString *me, Melder_4_ARGS);
void MelderString_append (MelderString *me, Melder_5_ARGS);
void MelderString_append (MelderString *me, Melder_6_ARGS);
void MelderString_append (MelderString *me, Melder_7_ARGS);
void MelderString_append (MelderString *me, Melder_8_ARGS);
void MelderString_append (MelderString *me, Melder_9_ARGS);
void MelderString_append (MelderString *me, Melder_10_ARGS);
void MelderString_append (MelderString *me, Melder_11_ARGS);
void MelderString_append (MelderString *me, Melder_12_OR_13_ARGS);
void MelderString_append (MelderString *me, Melder_14_OR_15_ARGS);
void MelderString_append (MelderString *me, Melder_16_TO_19_ARGS);
void MelderString16_appendCharacter (MelderString16 *me, char32 character);
void MelderString_appendCharacter (MelderString *me, char32 character);
void MelderString_get (MelderString *me, char32 *destination);   // performs no boundary checking
int64 MelderString_allocationCount ();
int64 MelderString_deallocationCount ();
int64 MelderString_allocationSize ();
int64 MelderString_deallocationSize ();

const char32 * Melder_cat (Melder_2_ARGS);
const char32 * Melder_cat (Melder_3_ARGS);
const char32 * Melder_cat (Melder_4_ARGS);
const char32 * Melder_cat (Melder_5_ARGS);
const char32 * Melder_cat (Melder_6_ARGS);
const char32 * Melder_cat (Melder_7_ARGS);
const char32 * Melder_cat (Melder_8_ARGS);
const char32 * Melder_cat (Melder_9_ARGS);
const char32 * Melder_cat (Melder_10_ARGS);
const char32 * Melder_cat (Melder_11_ARGS);
const char32 * Melder_cat (Melder_12_OR_13_ARGS);
const char32 * Melder_cat (Melder_14_OR_15_ARGS);
const char32 * Melder_cat (Melder_16_TO_19_ARGS);

char32 * Melder_dup (const char32 *string /* cattable */);
char32 * Melder_dup_f (const char32 *string /* cattable */);

void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_1_ARG);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_2_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_3_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_4_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_5_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_6_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_7_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_8_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_9_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_10_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_11_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_12_OR_13_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_14_OR_15_ARGS);
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_16_TO_19_ARGS);

/********** NUMBER AND STRING COMPARISON **********/

bool Melder_numberMatchesCriterion (double value, int which_kMelder_number, double criterion);
bool Melder_stringMatchesCriterion (const char32 *value, int which_kMelder_string, const char32 *criterion);

/********** STRING PARSING **********/

/*
	These functions regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
	Typical use:
		for (token = Melder_firstToken (string); token != nullptr; token = Melder_nextToken ()) {
			... do something with the token ...
		}
*/

long Melder_countTokens (const char32 *string);
char32 *Melder_firstToken (const char32 *string);
char32 *Melder_nextToken ();
char32 ** Melder_getTokens (const char32 *string, long *n);
void Melder_freeTokens (char32 ***tokens);
long Melder_searchToken (const char32 *string, char32 **tokens, long n);

/********** MESSAGING ROUTINES **********/

/**
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/
void Melder_casual (Melder_1_ARG);
void Melder_casual (Melder_2_ARGS);
void Melder_casual (Melder_3_ARGS);
void Melder_casual (Melder_4_ARGS);
void Melder_casual (Melder_5_ARGS);
void Melder_casual (Melder_6_ARGS);
void Melder_casual (Melder_7_ARGS);
void Melder_casual (Melder_8_ARGS);
void Melder_casual (Melder_9_ARGS);
void Melder_casual (Melder_10_ARGS);
void Melder_casual (Melder_11_ARGS);
void Melder_casual (Melder_12_OR_13_ARGS);
void Melder_casual (Melder_14_OR_15_ARGS);
void Melder_casual (Melder_16_TO_19_ARGS);

/**
	Give information to stdout (batch), or to an "Info" window (interactive), or to a diverted string.
*/
void MelderInfo_open ();   // clear the Info window in the background

void MelderInfo_write (Melder_1_ARG);
void MelderInfo_write (Melder_2_ARGS);
void MelderInfo_write (Melder_3_ARGS);
void MelderInfo_write (Melder_4_ARGS);
void MelderInfo_write (Melder_5_ARGS);
void MelderInfo_write (Melder_6_ARGS);
void MelderInfo_write (Melder_7_ARGS);
void MelderInfo_write (Melder_8_ARGS);
void MelderInfo_write (Melder_9_ARGS);
void MelderInfo_write (Melder_10_ARGS);
void MelderInfo_write (Melder_11_ARGS);
void MelderInfo_write (Melder_12_OR_13_ARGS);
void MelderInfo_write (Melder_14_OR_15_ARGS);
void MelderInfo_write (Melder_16_TO_19_ARGS);

void MelderInfo_writeLine (Melder_1_ARG);
void MelderInfo_writeLine (Melder_2_ARGS);
void MelderInfo_writeLine (Melder_3_ARGS);
void MelderInfo_writeLine (Melder_4_ARGS);
void MelderInfo_writeLine (Melder_5_ARGS);
void MelderInfo_writeLine (Melder_6_ARGS);
void MelderInfo_writeLine (Melder_7_ARGS);
void MelderInfo_writeLine (Melder_8_ARGS);
void MelderInfo_writeLine (Melder_9_ARGS);
void MelderInfo_writeLine (Melder_10_ARGS);
void MelderInfo_writeLine (Melder_11_ARGS);
void MelderInfo_writeLine (Melder_12_OR_13_ARGS);
void MelderInfo_writeLine (Melder_14_OR_15_ARGS);
void MelderInfo_writeLine (Melder_16_TO_19_ARGS);

void MelderInfo_close ();   // drain the background info to the Info window, making sure there is a line break
void MelderInfo_drain ();   // drain the background info to the Info window, without adding any extra line break

void Melder_information (Melder_1_ARG);
void Melder_information (Melder_2_ARGS);
void Melder_information (Melder_3_ARGS);
void Melder_information (Melder_4_ARGS);
void Melder_information (Melder_5_ARGS);
void Melder_information (Melder_6_ARGS);
void Melder_information (Melder_7_ARGS);
void Melder_information (Melder_8_ARGS);
void Melder_information (Melder_9_ARGS);
void Melder_information (Melder_10_ARGS);
void Melder_information (Melder_11_ARGS);
void Melder_information (Melder_12_OR_13_ARGS);
void Melder_information (Melder_14_OR_15_ARGS);
void Melder_information (Melder_16_TO_19_ARGS);

void Melder_informationReal (double value, const char32 *units);   // %.17g or --undefined--; units may be null

void Melder_divertInfo (MelderString *buffer);   // nullptr = back to normal

class autoMelderDivertInfo {
	public:
		autoMelderDivertInfo (MelderString *buffer) { Melder_divertInfo (buffer); }
		~autoMelderDivertInfo () { Melder_divertInfo (nullptr); }
};

void Melder_clearInfo ();   // clear the Info window
const char32 * Melder_getInfo ();
void Melder_help (const char32 *query);

void Melder_search ();
	
void Melder_beep ();

extern int Melder_debug;

/* The following trick uses Melder_debug only because it is the only plain variable known to exist at the moment. */
#define Melder_offsetof(klas,member) (int) ((char *) & ((klas) & Melder_debug) -> member - (char *) & Melder_debug)

/********** ERROR **********/

class MelderError { };

void Melder_appendError_noLine (const MelderArg& arg1);

void Melder_appendError (Melder_1_ARG);
void Melder_appendError (Melder_2_ARGS);
void Melder_appendError (Melder_3_ARGS);
void Melder_appendError (Melder_4_ARGS);
void Melder_appendError (Melder_5_ARGS);
void Melder_appendError (Melder_6_ARGS);
void Melder_appendError (Melder_7_ARGS);
void Melder_appendError (Melder_8_ARGS);
void Melder_appendError (Melder_9_ARGS);
void Melder_appendError (Melder_10_ARGS);
void Melder_appendError (Melder_11_ARGS);
void Melder_appendError (Melder_12_OR_13_ARGS);
void Melder_appendError (Melder_14_OR_15_ARGS);
void Melder_appendError (Melder_16_TO_19_ARGS);
#define Melder_throw(...)  do { Melder_appendError (__VA_ARGS__); throw MelderError (); } while (false)

void Melder_flushError ();
void Melder_flushError (Melder_1_ARG);
void Melder_flushError (Melder_2_ARGS);
void Melder_flushError (Melder_3_ARGS);
void Melder_flushError (Melder_4_ARGS);
void Melder_flushError (Melder_5_ARGS);
void Melder_flushError (Melder_6_ARGS);
void Melder_flushError (Melder_7_ARGS);
void Melder_flushError (Melder_8_ARGS);
void Melder_flushError (Melder_9_ARGS);
void Melder_flushError (Melder_10_ARGS);
void Melder_flushError (Melder_11_ARGS);
void Melder_flushError (Melder_12_OR_13_ARGS);
void Melder_flushError (Melder_14_OR_15_ARGS);
void Melder_flushError (Melder_16_TO_19_ARGS);
	/* Send all deferred error messages to stderr (batch) or to an "Error" dialog, */
	/* including, if there are arguments, the error message generated by this routine. */

bool Melder_hasError ();
bool Melder_hasError (const char32 *partialError);
	/* Returns 1 if there is an error message in store, otherwise 0. */

void Melder_clearError ();
	/* Cancel all stored error messages. */

char32 * Melder_getError ();
	/* Returns the error string. Mainly used with str32str. */

/********** WARNING: give warning to stderr (batch) or to a "Warning" dialog **********/

void Melder_warning (Melder_1_ARG);
void Melder_warning (Melder_2_ARGS);
void Melder_warning (Melder_3_ARGS);
void Melder_warning (Melder_4_ARGS);
void Melder_warning (Melder_5_ARGS);
void Melder_warning (Melder_6_ARGS);
void Melder_warning (Melder_7_ARGS);
void Melder_warning (Melder_8_ARGS);
void Melder_warning (Melder_9_ARGS);
void Melder_warning (Melder_10_ARGS);
void Melder_warning (Melder_11_ARGS);
void Melder_warning (Melder_12_OR_13_ARGS);
void Melder_warning (Melder_14_OR_15_ARGS);
void Melder_warning (Melder_16_TO_19_ARGS);

void Melder_warningOff ();
void Melder_warningOn ();

class autoMelderWarningOff {
public:
	autoMelderWarningOff () { Melder_warningOff (); }
	~autoMelderWarningOff () { Melder_warningOn (); }
};

/********** FATAL: Praat crashes because of a programming error **********/

/**
	Give error message, abort program.
	Should only be caused by programming errors.
*/
int Melder_fatal (Melder_1_ARG);
int Melder_fatal (Melder_2_ARGS);
int Melder_fatal (Melder_3_ARGS);
int Melder_fatal (Melder_4_ARGS);
int Melder_fatal (Melder_5_ARGS);
int Melder_fatal (Melder_6_ARGS);
int Melder_fatal (Melder_7_ARGS);
int Melder_fatal (Melder_8_ARGS);
int Melder_fatal (Melder_9_ARGS);
int Melder_fatal (Melder_10_ARGS);
int Melder_fatal (Melder_11_ARGS);
int Melder_fatal (Melder_12_OR_13_ARGS);
int Melder_fatal (Melder_14_OR_15_ARGS);
int Melder_fatal (Melder_16_TO_19_ARGS);

#pragma mark - PROGRESS

void Melder_progress (double progress);
void Melder_progress (double progress, Melder_1_ARG);
void Melder_progress (double progress, Melder_2_ARGS);
void Melder_progress (double progress, Melder_3_ARGS);
void Melder_progress (double progress, Melder_4_ARGS);
void Melder_progress (double progress, Melder_5_ARGS);
void Melder_progress (double progress, Melder_6_ARGS);
void Melder_progress (double progress, Melder_7_ARGS);
void Melder_progress (double progress, Melder_8_ARGS);
void Melder_progress (double progress, Melder_9_ARGS);
void Melder_progress (double progress, Melder_10_ARGS);
void Melder_progress (double progress, Melder_11_ARGS);
void Melder_progress (double progress, Melder_12_OR_13_ARGS);
void Melder_progress (double progress, Melder_14_OR_15_ARGS);
void Melder_progress (double progress, Melder_16_TO_19_ARGS);

void Melder_progressOff ();
void Melder_progressOn ();
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		Any of 'arg1' through 'arg19' may be null.
	Batch behaviour:
		Does nothing, always returns 1.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button, and return 1;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return 0 if user interrupts, else return 1;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts:
		      (void) Melder_progress (0.0, U"Starting work...");
		- at every turn in your loop, call with 'progress' between 0.0 and 1.0:
		      Melder_progress (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
		  an exception is thrown if the user clicks Cancel; if you don't want that, catch it:
		      try {
		          Melder_progress (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
		      } catch (MelderError) {
		          Melder_clearError ();
		          break;
		      }
		- after the process has finished, call with 'progress' = 1.0:
		      (void) Melder_progress (1.0);
		- the first and third steps can be automated by autoMelderProgress:
		      autoMelderProgress progress (U"Starting work...");
*/
class autoMelderProgress {
public:
	autoMelderProgress (const char32 *message) {
		Melder_progress (0.0, message);
	}
	~autoMelderProgress () {
		Melder_progress (1.0);
	}
};

void * Melder_monitor (double progress);
void * Melder_monitor (double progress, Melder_1_ARG);
void * Melder_monitor (double progress, Melder_2_ARGS);
void * Melder_monitor (double progress, Melder_3_ARGS);
void * Melder_monitor (double progress, Melder_4_ARGS);
void * Melder_monitor (double progress, Melder_5_ARGS);
void * Melder_monitor (double progress, Melder_6_ARGS);
void * Melder_monitor (double progress, Melder_7_ARGS);
void * Melder_monitor (double progress, Melder_8_ARGS);
void * Melder_monitor (double progress, Melder_9_ARGS);
void * Melder_monitor (double progress, Melder_10_ARGS);
void * Melder_monitor (double progress, Melder_11_ARGS);
void * Melder_monitor (double progress, Melder_12_OR_13_ARGS);
void * Melder_monitor (double progress, Melder_14_OR_15_ARGS);
void * Melder_monitor (double progress, Melder_16_TO_19_ARGS);
/*
	Function:
		Show the progress of a time-consuming process.
	Arguments:
		Any of 'arg1' through 'arg19' may be null.
	Batch behaviour:
		Does nothing, returns null if 'progress' <= 0.0 and a non-null pointer otherwise.
	Interactive behaviour:
		Shows the progress of a time-consuming process:
		- if 'progress' <= 0.0, show a window with text and a Cancel button and
		  room for a square drawing, and return a Graphics;
		- if 0.0 < 'progress' < 1.0, show text and a partially filled progress bar,
		  and return nullptr if user interrupts, else return a non-null pointer;
		- if 'progress' >= 1, hide the window.
	Usage:
		- call with 'progress' = 0.0 before the process starts.
		- assign the return value to a Graphics:
		      Graphics graphics = Melder_monitor (0.0, U"Starting work...");
		- at every turn of your loop, draw something in the Graphics:
		      if (graphics) {   // always check; might be batch
		          Graphics_clearWs (graphics);   // only if you redraw all every time
		          Graphics_polyline (graphics, ...);
		          Graphics_text (graphics, ...);
		      }
		- immediately after this in your loop, call with 'progress' between 0.0 and 1.0:
		      Melder_monitor (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
		  an exception is thrown if the user clicks Cancel; if you don't want that, catch it:
		      try {
		          Melder_monitor (i / (n + 1.0), U"Working on part ", i, U" out of ", n, U"...");
		      } catch (MelderError) {
		          Melder_clearError ();
		          break;
		      }
		- after the process has finished, call with 'progress' = 1.0:
		      (void) Melder_monitor (1.0, nullptr);
		- showing and hiding can be automated by autoMelderMonitor:
		      autoMelderMonitor monitor ("Starting work...");
		      if (monitor.graphics()) {   // always check; might be batch
		          Graphics_clearWs (monitor.graphics());   // only if you redraw all every time
		          Graphics_polyline (monitor.graphics(), ...);
		          Graphics_text (monitor.graphics(), ...);
		      }
*/
typedef class structGraphics *Graphics;
class autoMelderMonitor {
	Graphics _graphics;
public:
	autoMelderMonitor (const char32 *message) {
		_graphics = (Graphics) Melder_monitor (0.0, message);
	}
	~autoMelderMonitor () {
		Melder_monitor (1.0);
	}
	Graphics graphics () { return _graphics; }
};

/********** RECORD AND PLAY ROUTINES **********/

int Melder_record (double duration);
int Melder_recordFromFile (MelderFile file);
void Melder_play ();
void Melder_playReverse ();
int Melder_publishPlayed ();

/********** SYSTEM VERSION **********/

extern int32 Melder_systemVersion;
/*
	For Macintosh, this is set in praat_init.
*/

/********** ENFORCE INTERACTIVE BEHAVIOUR **********/

/* Procedures to enforce interactive behaviour of the Melder_XXXXXX routines. */

void MelderGui_create (/* GuiWindow */ void *parent);
/*
	'parent' is the top-level widget returned by GuiAppInitialize.
*/

extern bool Melder_batch;   // true if run from the batch or from an interactive command-line interface
extern bool Melder_backgrounding;   // true if running a script
extern bool Melder_consoleIsAnsi;
extern bool Melder_asynchronous;   // true if specified by the "asynchronous" directive in a script

/********** OVERRIDE DEFAULT BEHAVIOUR **********/

/* Procedures to override default message methods. */
/* They may chage the string arguments. */
/* Many of these routines are called by MelderMotif_create and MelderXvt_create. */

void Melder_setCasualProc (void (*casualProc) (const char32 *message));
void Melder_setProgressProc (int (*progressProc) (double progress, const char32 *message));
void Melder_setMonitorProc (void * (*monitorProc) (double progress, const char32 *message));
void Melder_setInformationProc (void (*informationProc) (const char32 *message));
void Melder_setHelpProc (void (*help) (const char32 *query));
void Melder_setSearchProc (void (*search) ());
void Melder_setWarningProc (void (*warningProc) (const char32 *message));
void Melder_setProgressProc (void (*progress) (double, const char32 *));
void Melder_setMonitorProc (void * (*monitor) (double, const char32 *));
void Melder_setErrorProc (void (*errorProc) (const char32 *message));
void Melder_setFatalProc (void (*fatalProc) (const char32 *message));
void Melder_setRecordProc (int (*record) (double));
void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile));
void Melder_setPlayProc (void (*play) ());
void Melder_setPlayReverseProc (void (*playReverse) ());
void Melder_setPublishPlayedProc (int (*publishPlayed) ());

double Melder_stopwatch ();
void Melder_sleep (double duration);

#pragma mark - AUDIO

void MelderAudio_setInputSoundSystem (enum kMelder_inputSoundSystem inputSoundSystem);
enum kMelder_inputSoundSystem MelderAudio_getInputSoundSystem ();

void MelderAudio_setOutputSoundSystem (enum kMelder_outputSoundSystem outputSoundSystem);
enum kMelder_outputSoundSystem MelderAudio_getOutputSoundSystem ();

#if defined (_WIN32)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.1
		// in order to get rid of the click on some cards
#elif defined (macintosh)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
		// in order to switch off the BOING caused by the automatic gain control
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.0
		// in order to reduce the BOING caused by the automatic gain control when the user replays immediately after a sound has finished
#elif defined (linux)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.1
		// in order to get rid of double playing of a sounding buffer (?)
#else
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.0
#endif
void MelderAudio_setOutputSilenceBefore (double silenceBefore);
double MelderAudio_getOutputSilenceBefore ();
void MelderAudio_setOutputSilenceAfter (double silenceAfter);
double MelderAudio_getOutputSilenceAfter ();

void MelderAudio_setUseInternalSpeaker (bool useInternalSpeaker);   // for HP-UX and Sun
bool MelderAudio_getUseInternalSpeaker ();
void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity);
enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity ();
long MelderAudio_getOutputBestSampleRate (long fsamp);

extern bool MelderAudio_isPlaying;
void MelderAudio_play16 (int16_t *buffer, long sampleRate, long numberOfSamples, int numberOfChannels,
	bool (*playCallback) (void *playClosure, long numberOfSamplesPlayed),   // return true to continue, false to stop
	void *playClosure);
bool MelderAudio_stopPlaying (bool isExplicit);   // returns true if sound was playing
#define MelderAudio_IMPLICIT  false
#define MelderAudio_EXPLICIT  true
long MelderAudio_getSamplesPlayed ();
bool MelderAudio_stopWasExplicit ();

void Melder_audio_prefs ();   // in init file

#pragma mark - AUDIO FILES

/* Audio file types. */
#define Melder_AIFF  1
#define Melder_AIFC  2
#define Melder_WAV  3
#define Melder_NEXT_SUN  4
#define Melder_NIST  5
#define Melder_FLAC 6
#define Melder_MP3 7
#define Melder_NUMBER_OF_AUDIO_FILE_TYPES  7
const char32 * Melder_audioFileTypeString (int audioFileType);   // "AIFF", "AIFC", "WAV", "NeXT/Sun", "NIST", "FLAC", "MP3"
/* Audio encodings. */
#define Melder_LINEAR_8_SIGNED  1
#define Melder_LINEAR_8_UNSIGNED  2
#define Melder_LINEAR_16_BIG_ENDIAN  3
#define Melder_LINEAR_16_LITTLE_ENDIAN  4
#define Melder_LINEAR_24_BIG_ENDIAN  5
#define Melder_LINEAR_24_LITTLE_ENDIAN  6
#define Melder_LINEAR_32_BIG_ENDIAN  7
#define Melder_LINEAR_32_LITTLE_ENDIAN  8
#define Melder_MULAW  9
#define Melder_ALAW  10
#define Melder_SHORTEN  11
#define Melder_POLYPHONE  12
#define Melder_IEEE_FLOAT_32_BIG_ENDIAN  13
#define Melder_IEEE_FLOAT_32_LITTLE_ENDIAN  14
#define Melder_FLAC_COMPRESSION_16 15
#define Melder_FLAC_COMPRESSION_24 16
#define Melder_FLAC_COMPRESSION_32 17
#define Melder_MPEG_COMPRESSION_16 18
#define Melder_MPEG_COMPRESSION_24 19
#define Melder_MPEG_COMPRESSION_32 20
int Melder_defaultAudioFileEncoding (int audioFileType, int numberOfBitsPerSamplePoint);   /* BIG_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN */
void MelderFile_writeAudioFileHeader (MelderFile file, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFileTrailer (MelderFile file, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFile (MelderFile file, int audioFileType, const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);

int MelderFile_checkSoundFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, int32 *numberOfSamples);
/* Returns information about a just opened audio file.
 * The return value is the audio file type, or 0 if it is not a sound file or in case of error.
 * The data start at 'startOfData' bytes from the start of the file.
 */
int Melder_bytesPerSamplePoint (int encoding);
void Melder_readAudioToFloat (FILE *f, int numberOfChannels, int encoding, double **buffer, long numberOfSamples);
/* Reads channels into buffer [ichannel], which are base-1.
 */
void Melder_readAudioToShort (FILE *f, int numberOfChannels, int encoding, short *buffer, long numberOfSamples);
/* If stereo, buffer will contain alternating left and right values.
 * Buffer is base-0.
 */
void MelderFile_writeFloatToAudio (MelderFile file, int numberOfChannels, int encoding, double **buffer, long numberOfSamples, int warnIfClipped);
void MelderFile_writeShortToAudio (MelderFile file, int numberOfChannels, int encoding, const short *buffer, long numberOfSamples);

/********** QUANTITY **********/

#define MelderQuantity_NONE  0
#define MelderQuantity_TIME_SECONDS  1
#define MelderQuantity_FREQUENCY_HERTZ  2
#define MelderQuantity_FREQUENCY_BARK  3
#define MelderQuantity_DISTANCE_FROM_GLOTTIS_METRES  4
#define MelderQuantity_NUMBER_OF_QUANTITIES  4
const char32 * MelderQuantity_getText (int quantity);   // e.g. "Time"
const char32 * MelderQuantity_getWithUnitText (int quantity);   // e.g. "Time (s)"
const char32 * MelderQuantity_getLongUnitText (int quantity);   // e.g. "seconds"
const char32 * MelderQuantity_getShortUnitText (int quantity);   // e.g. "s"

/********** MISCELLANEOUS **********/

char32 * Melder_getenv (const char32 *variableName);
void Melder_system (const char32 *command);   // spawn a system command
void Melder_execv (const char32 *executableFileName, int narg, char32 **args);   // spawn a subprocess
double Melder_clock ();   // seconds since 1969

struct autoMelderProgressOff {
	autoMelderProgressOff () { Melder_progressOff (); }
	~autoMelderProgressOff () { Melder_progressOn (); }
};

struct autoMelderString : MelderString {
	autoMelderString () { length = 0; bufferSize = 0; string = nullptr; }
	~autoMelderString () { MelderString_free (this); }
};

struct autoMelderReadText {
	MelderReadText text;
	autoMelderReadText (MelderReadText a_text) : text (a_text) {
	}
	~autoMelderReadText () {
		if (text) MelderReadText_delete (text);
	}
	MelderReadText operator-> () const {   // as r-value
		return text;
	}
	MelderReadText peek () const {
		return text;
	}
	MelderReadText transfer () {
		MelderReadText tmp = text;
		text = nullptr;
		return tmp;
	}
};

class autofile {
	FILE *ptr;
public:
	autofile (FILE *f) : ptr (f) {
	}
	autofile () : ptr (nullptr) {
	}
	~autofile () {
		if (ptr) fclose (ptr);   // no error checking, because this is a destructor, only called after a throw, because otherwise you'd use f.close(file)
	}
	operator FILE * () {
		return ptr;
	}
	void reset (FILE *f) {
		if (ptr) fclose (ptr);   // BUG: not a normal closure
		ptr = f;
	}
	void close (MelderFile file) {
		if (ptr) {
			FILE *tmp = ptr;
			ptr = nullptr;
			Melder_fclose (file, tmp);
		}
	}
};

class autoMelderFile {
	MelderFile _file;
public:
	autoMelderFile (MelderFile file) : _file (file) {
	}
	~autoMelderFile () {
		if (_file) MelderFile_close_nothrow (_file);
	}
	void close () {
		if (_file && _file -> filePointer) {
			MelderFile tmp = _file;
			_file = nullptr;
			MelderFile_close (tmp);
		}
	}
	MelderFile transfer () {
		MelderFile tmp = _file;
		_file = nullptr;
		return tmp;
	}
};

class autoMelderSaveDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderSaveDefaultDir () {
		Melder_getDefaultDir (& our _savedDir);
	}
	~autoMelderSaveDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderSaveDefaultDir (const autoMelderSaveDefaultDir&) = delete;   // disable copy constructor
	autoMelderSaveDefaultDir& operator= (const autoMelderSaveDefaultDir&) = delete;   // disable copy assignment
};

class autoMelderSetDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderSetDefaultDir (MelderDir dir) {
		Melder_getDefaultDir (& our _savedDir);
		Melder_setDefaultDir (dir);
	}
	~autoMelderSetDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderSetDefaultDir (const autoMelderSetDefaultDir&) = delete;   // disable copy constructor
	autoMelderSetDefaultDir& operator= (const autoMelderSetDefaultDir&) = delete;   // disable copy assignment
};

class autoMelderFileSetDefaultDir {
	structMelderDir _savedDir;
public:
	autoMelderFileSetDefaultDir (MelderFile file) {
		Melder_getDefaultDir (& our _savedDir);
		MelderFile_setDefaultDir (file);
	}
	~autoMelderFileSetDefaultDir () {
		Melder_setDefaultDir (& our _savedDir);
	}
	/*
		Disable copying.
	*/
	autoMelderFileSetDefaultDir (const autoMelderFileSetDefaultDir&) = delete;   // disable copy constructor
	autoMelderFileSetDefaultDir& operator= (const autoMelderFileSetDefaultDir&) = delete;   // disable copy assignment
};

class autoMelderTokens {
	char32 **tokens;
	long numberOfTokens;
public:
	autoMelderTokens () {
		tokens = nullptr;
	}
	autoMelderTokens (const char32 *string) {
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
	~autoMelderTokens () {
		if (tokens) {
			for (long itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
	}
	char32*& operator[] (long i) {
		return tokens [i];
	}
	char32 ** peek () const {
		return tokens;
	}
	long count () const {
		return numberOfTokens;
	}
	void reset (const char32 *string) {
		if (tokens) {
			for (long itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
};

template <class T>
class _autostring {
	T *ptr;
public:
	_autostring (T *string) : ptr (string) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: constructor from C-string ", Melder_pointer (ptr));
	}
	_autostring () : ptr (nullptr) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: zero constructor");
	}
	~_autostring () {
		//if (Melder_debug == 39) Melder_casual (U"autostring: entering destructor ptr = ", Melder_pointer (ptr));
		if (ptr) Melder_free (ptr);
		//if (Melder_debug == 39) Melder_casual (U"autostring: leaving destructor");
	}
	#if 0
	void operator= (T *string) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: entering assignment from C-string; old = ", Melder_pointer (ptr));
		if (ptr) Melder_free (ptr);
		ptr = string;
		//if (Melder_debug == 39) Melder_casual (U"autostring: leaving assignment from C-string; new = ", Melder_pointer (ptr));
	}
	#endif
	template <class U> T& operator[] (U i) {
		return ptr [i];
	}
	T * peek () const {
		return ptr;
	}
	T ** operator& () {
		return & ptr;
	}
	T * transfer () {
		T *tmp = ptr;
		ptr = nullptr;
		return tmp;
	}
	void reset (T *string) {
		if (ptr) Melder_free (ptr);
		ptr = string;
	}
	void resize (int64 new_size) {
		T *tmp = (T *) Melder_realloc (ptr, new_size * (int64) sizeof (T));
		ptr = tmp;
	}
	_autostring& operator= (const _autostring&) = delete;   // disable copy assignment
	//_autostring (_autostring &) = delete;   // disable copy constructor (trying it this way also disables good things like autostring s1 = str32dup(U"hello");)
	template <class Y> _autostring (_autostring<Y> &) = delete;   // disable copy constructor
};

typedef _autostring <char> autostring8;
typedef _autostring <char16> autostring16;
typedef _autostring <char32> autostring32;

class autoMelderAudioSaveMaximumAsynchronicity {
	bool _disowned;
	enum kMelder_asynchronicityLevel _savedAsynchronicity;
public:
	autoMelderAudioSaveMaximumAsynchronicity () {
		our _savedAsynchronicity = MelderAudio_getOutputMaximumAsynchronicity ();
		trace (U"value was ", our _savedAsynchronicity);
		our _disowned = false;
	}
	~autoMelderAudioSaveMaximumAsynchronicity () {
		MelderAudio_setOutputMaximumAsynchronicity (our _savedAsynchronicity);
		trace (U"value set to ", our _savedAsynchronicity);
	}
	/*
		Disable copying.
	*/
	autoMelderAudioSaveMaximumAsynchronicity (const autoMelderAudioSaveMaximumAsynchronicity&) = delete;   // disable copy constructor
	autoMelderAudioSaveMaximumAsynchronicity& operator= (const autoMelderAudioSaveMaximumAsynchronicity&) = delete;   // disable copy assignment
	/*
		Enable moving.
	*/
	autoMelderAudioSaveMaximumAsynchronicity (autoMelderAudioSaveMaximumAsynchronicity&& other) noexcept {   // enable move constructor
		our _disowned = other._disowned;
		our _savedAsynchronicity = other._savedAsynchronicity;
		other._disowned = true;
	}
	autoMelderAudioSaveMaximumAsynchronicity& operator= (autoMelderAudioSaveMaximumAsynchronicity&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our _disowned = other._disowned;
			our _savedAsynchronicity = other._savedAsynchronicity;
			other._disowned = true;   // needed only if you insist on keeping the source in a valid state
		}
		return *this;
	}
	autoMelderAudioSaveMaximumAsynchronicity&& move () noexcept { return static_cast <autoMelderAudioSaveMaximumAsynchronicity&&> (*this); }
	void releaseToAmbiguousOwner () {
		our _disowned = true;
	}
};

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

#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if<std::is_base_of<B,A>::value>::type

template <typename Ret, typename T, typename... Args>
class MelderCallback {
	public:
		typedef Ret* (*FunctionType) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T), typename Ret2  Melder_ENABLE_IF_ISA(Ret2,Ret)>
			MelderCallback (Ret2* (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		Ret* operator () (T* data, Args ... args) { return _f (data, args...); }
		explicit operator bool () const { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <void, T, Args...> {   // specialization
	public:
		typedef void (*FunctionType) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCallback (void (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		void operator () (T* data, Args ... args) { _f (data, args...); }
		explicit operator bool () const { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <int, T, Args...> {   // specialization
	public:
		typedef int (*FunctionType) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCallback (int (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) { };
		int operator () (T* data, Args ... args) { return _f (data, args...); }
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

/* End of file melder.h */
#endif
