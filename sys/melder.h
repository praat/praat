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
	#define strequ  ! strcmp
	#define strnequ  ! strncmp
	#define Melder_equ  ! Melder_cmp
	#define Melder_nequ  ! Melder_ncmp
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>
#ifdef __MINGW32__
	#include <sys/types.h>   // for off_t
#endif
#include <stdbool.h>
#include <functional>
/*
 * The following two lines are for obsolete (i.e. C99) versions of stdint.h
 */
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#ifndef INT54_MAX
	#define INT54_MAX   9007199254740991LL
	#define INT54_MIN  -9007199254740991LL
#endif

typedef unsigned char char8;
typedef char16_t char16;
typedef char32_t char32;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

bool Melder_str32equ_firstCharacterCaseInsensitive (const char32 *string1, const char32 *string2);

#include "enums.h"

#include "melder_enums.h"

#ifndef TRUE
	#define TRUE  1
#endif
#ifndef FALSE
	#define FALSE  0
#endif
#ifndef NULL
	#define NULL  ((void *) 0)
#endif

/*
 * Debugging.
 */

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition);
	/* Call Melder_fatal with a message based on the following template: */
	/*    "Assertion failed in file <fileName> on line <lineNumber>: <condition>" */

void Melder_setTracing (bool tracing);
extern bool Melder_isTracing;

/*
 * char16 handling.
 */
inline static int64 str16len (const char16 *string) {
	const char16 *p = string;
	while (*p != u'\0') ++ p;
	return (int64) (p - string);
}
inline static char16 * str16cpy (char16 *target, const char16 *source) {
	char16 *p = target;
	while (* source != u'\0') * p ++ = * source ++;
	*p = u'\0';
	return target;
}
/*
 * char32 handling.
 */
inline static int64 str32len (const char32 *string) {
	const char32 *p = string;
	while (*p != U'\0') ++ p;
	return (int64) (p - string);
}
inline static char32 * str32cpy (char32 *target, const char32 *source) {
	char32 *p = target;
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}
inline static char32 * str32ncpy (char32 *target, const char32 *source, int64 n) {
	char32 *p = target;
	for (; n > 0 && *source != U'\0'; -- n) * p ++ = * source ++;
	for (; n > 0; -- n) * p ++ = U'\0';
	return target;
}
inline static int str32cmp (const char32 *string1, const char32 *string2) {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
#define str32equ  ! str32cmp
inline static int str32ncmp (const char32 *string1, const char32 *string2, int64 n) {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
#define str32nequ  ! str32ncmp
inline static char32 * str32chr (const char32 *string, char32 kar) {
	for (; *string != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline static char32 * str32rchr (const char32 *string, char32 kar) {
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (*string == kar) result = (char32 *) string;
	}
	return result;
}
inline static char32 * str32str (const char32 *string, const char32 *find) {
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
inline static int64 str32spn (const char32 *string1, const char32 *string2) {
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
extern "C" char * Melder_peek32to8 (const char32 *string);
inline static long a32tol (const char32 *string) {
	if (sizeof (wchar_t) == 4) {
		return wcstol ((const wchar_t *) string, nullptr, 10);
	} else {
		return atol (Melder_peek32to8 (string));
	}
}

/*
 * Operating system version control.
 */
#define ALLOW_GDK_DRAWING  (gtk && 1)   /* change to (gtk && 0) if you want to try out GTK 3 */
/* */

typedef struct { double red, green, blue, transparency; } double_rgbt;

/********** NUMBER TO STRING CONVERSION **********/

/**
	The following routines return a static string, chosen from a circularly used set of 11 buffers.
	You can call at most 11 of them in one Melder_casual call, for instance.
*/

const  char32 * Melder_integer  (int64 value);
const  char   * Melder8_integer (int64 value);

const  char32 * Melder_bigInteger  (int64 value);
const  char   * Melder8_bigInteger (int64 value);

const  char32 * Melder_boolean  (bool value);
const  char   * Melder8_boolean (bool value);
	// "yes" or "no"

/**
	Format a double value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats.
*/
const  char32 * Melder_double  (double value);
const  char   * Melder8_double (double value);

/**
	Format a double value as "--undefined--" or something in the "%.9g" format.
*/
const  char32 * Melder_single  (double value);
const  char   * Melder8_single (double value);

/**
	Format a double value as "--undefined--" or something in the "%.4g" format.
*/
const  char32 * Melder_half  (double value);
const  char   * Melder8_half (double value);

/**
	Format a double value as "--undefined--" or something in the "%.*f" format.
*/
const  char32 * Melder_fixed  (double value, int precision);
const  char   * Melder8_fixed (double value, int precision);

/**
	Format a double value with a specified precision. If exponent is -2 and precision is 2, you get things like 67E-2 or 0.00024E-2.
*/
const  char32 * Melder_fixedExponent  (double value, int exponent, int precision);
const  char   * Melder8_fixedExponent (double value, int exponent, int precision);

/**
	Format a double value as a percentage. If precision is 3, you get things like "0" or "34.400%" or "0.014%" or "0.001%" or "0.0000007%".
*/
const  char32 * Melder_percent  (double value, int precision);
const  char   * Melder8_percent (double value, int precision);

/**
	Convert a formatted floating-point string to something suitable for visualization with the Graphics library.
	For instance, "1e+4" is turned into "10^^4", and "-1.23456e-78" is turned into "-1.23456\.c10^^-78".
*/
const char32 * Melder_float (const char32 *number);

/**
	Format the number that is specified by its natural logarithm.
	For instance, -10000 is formatted as "1.135483865315339e-4343", which is a floating-point representation of exp(-10000).
*/
const  char32 * Melder_naturalLogarithm  (double lnNumber);
const  char   * Melder8_naturalLogarithm (double lnNumber);

const  char32 * Melder_pointer  (void *pointer);
const  char   * Melder8_pointer (void *pointer);

const  char32 * Melder_character  (char32_t kar);
const  char   * Melder8_character (char32_t kar);

const char32 * Melder_pad (int64 width, const char32 *string);   // will append spaces to the left of 'string' until 'width' is reached; no truncation
const char32 * Melder_pad (const char32 *string, int64 width);   // will append spaces to the right of 'string' until 'width' is reached; no truncation
const char32 * Melder_truncate (int64 width, const char32 *string);   // will cut away the left of 'string' until 'width' is reached; no padding
const char32 * Melder_truncate (const char32 *string, int64 width);   // will cut away the right of 'string' until 'width' is reached; no padding
const char32 * Melder_padOrTruncate (int64 width, const char32 *string);   // will cut away, or append spaces to, the left of 'string' until 'width' is reached
const char32 * Melder_padOrTruncate (const char32 *string, int64 width);   // will cut away, or append spaces to, the right of 'string' until 'width' is reached


/********** STRING TO NUMBER CONVERSION **********/

bool Melder_isStringNumeric_nothrow (const char32 *string);
double Melder_a8tof (const char *string);
double Melder_atof (const char32 *string);
int64 Melder_atoi (const char32 *string);
	/*
	 * "3.14e-3" -> 3.14e-3
	 * "15.6%" -> 0.156
	 * "fghfghj" -> NUMundefined
	 */

/********** CONSOLE **********/

void Melder_writeToConsole (const char32 *message, bool useStderr);

/********** MEMORY ALLOCATION ROUTINES **********/

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

int Melder_cmp (const char32 *string1, const char32 *string2);   // regards null string as empty string
int Melder_ncmp (const char32 *string1, const char32 *string2, int64 n);
char32 * Melder_tok (char32 *string, const char32 *delimiter);

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
	 Replaces all bare returns (Mac) or return / linefeed sequences (Win) with bare linefeeds (generic = Unix).
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
const char32 * MelderFile_messageName (MelderFile file);   // Calls Melder_peekExpandBackslashes ().

/* The arguments to all messaging functions. */

typedef class structThing *Thing;
char32 *Thing_messageName (Thing me);
struct MelderArg {
	const char32 *_arg;
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
	MelderArg (Thing                     arg) : _arg (Thing_messageName      (arg)) { }
	MelderArg (MelderFile                arg) : _arg (MelderFile_messageName (arg)) { }
	//MelderArg (void *                    arg) : _arg (Melder_integer         ((int64) arg)) { }
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

#define Melder_free(pointer)  _Melder_free ((void **) & (pointer))
void _Melder_free (void **pointer);
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

/********** PROGRESS ROUTINES **********/

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
		- at every turn in your loop, call with 'progress' between 0 and 1:
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
		- immediately after this in your loop, call with 'progress' between 0 and 1:
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

/********** AUDIO **********/

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

/********** AUDIO FILES **********/

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
	structMelderDir saveDir;
public:
	autoMelderSaveDefaultDir () {
		Melder_getDefaultDir (& saveDir);
	}
	~autoMelderSaveDefaultDir () {
		Melder_setDefaultDir (& saveDir);
	}
};

class autoMelderSetDefaultDir {
	structMelderDir saveDir;
public:
	autoMelderSetDefaultDir (MelderDir dir) {
		Melder_getDefaultDir (& saveDir);
		Melder_setDefaultDir (dir);
	}
	~autoMelderSetDefaultDir () {
		Melder_setDefaultDir (& saveDir);
	}
};

class autoMelderFileSetDefaultDir {
	structMelderDir saveDir;
public:
	autoMelderFileSetDefaultDir (MelderFile file) {
		Melder_getDefaultDir (& saveDir);
		MelderFile_setDefaultDir (file);
	}
	~autoMelderFileSetDefaultDir () {
		Melder_setDefaultDir (& saveDir);
	}
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
private:
	_autostring& operator= (const _autostring&);   // disable copy assignment
	//_autostring (_autostring &);   // disable copy constructor (trying it this way also disables good things like autostring s1 = str32dup(U"hello");)
	template <class Y> _autostring (_autostring<Y> &);   // disable copy constructor
};

typedef _autostring <char> autostring8;
typedef _autostring <char16> autostring16;
typedef _autostring <char32> autostring32;

class autoMelderAudioSaveMaximumAsynchronicity {
	enum kMelder_asynchronicityLevel d_saveAsynchronicity;
public:
	autoMelderAudioSaveMaximumAsynchronicity () {
		d_saveAsynchronicity = MelderAudio_getOutputMaximumAsynchronicity ();
		trace (U"value was ", d_saveAsynchronicity);
	}
	~autoMelderAudioSaveMaximumAsynchronicity () {
		MelderAudio_setOutputMaximumAsynchronicity (d_saveAsynchronicity);
		trace (U"value set to ", d_saveAsynchronicity);
	}
};

struct autoMelderAsynchronous {
	autoMelderAsynchronous () { Melder_asynchronous = true; }
	~autoMelderAsynchronous () { Melder_asynchronous = false; }
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
