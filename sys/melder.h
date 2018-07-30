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
#include <functional>

#pragma mark - ASSERTION

void Melder_assert_ (const char *fileName, int lineNumber, const char *condition);
	/* Call Melder_fatal with a message based on the following template: */
	/*    "Assertion failed in file <fileName> on line <lineNumber>: <condition>" */
#ifdef NDEBUG
	#define Melder_assert(x)   ((void) 0)
#else
	#define Melder_assert(x)   ((x) ? (void) (0) : (Melder_assert_ (__FILE__, __LINE__, #x), abort ()))
#endif

#pragma mark - INTEGERS
/*
 * The following two lines are for obsolete (i.e. C99) versions of stdint.h
 */
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
using byte = unsigned char;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using integer = intptr_t;   // the default size of an integer (a "long" is only 32 bits on 64-bit Windows)
using long_not_integer = long;   // for cases where we explicitly need the type "long", such as when printfing to %ld
using uinteger = uintptr_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
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
#define INTEGER_MAX  ( sizeof (integer) == 4 ? INT32_MAX : INT64_MAX )
#define INTEGER_MIN  ( sizeof (integer) == 4 ? INT32_MIN : INT64_MIN )
/*
	The bounds of the contiguous set of integers that in a "double" can represent only themselves.
*/
#ifndef INT54_MAX
	#define INT54_MAX   9007199254740991LL
	#define INT54_MIN  -9007199254740991LL
#endif

/*
	We assume that the types "integer" and "uinteger" are both large enough to contain
	any possible value that Praat wants to assign to them.
	This entails that we assume that these types can be converted to each other without bounds checking.
	We therefore crash Praat if this second assumption is not met.
*/
inline static uinteger integer_to_uinteger (integer n) {
	Melder_assert (n >= 0);
	return (uinteger) n;
}
inline static integer uinteger_to_integer (uinteger n) {
	Melder_assert (n <= INTEGER_MAX);
	return (integer) n;
}

#pragma mark - NULL

#ifndef NULL
	#define NULL  ((void *) 0)
#endif

#pragma mark - REALS
/*
	The following are checked in praat.h.
*/
using longdouble = long double;   // typically 80 bits ("extended") precision, but stored in 96 or 128 bits; on some platforms only 64 bits
#include "complex.h"

#pragma mark - LAW OF DEMETER FOR CLASS FUNCTIONS DEFINED OUTSIDE CLASS DEFINITION

#define our  this ->
/* The single most useful macro in Praat: */
#define my  me ->
#define thy  thee ->
#define your  you ->
#define his  him ->
#define her  she ->
#define iam(klas)  klas me = (klas) void_me

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

#pragma mark - STRINGS

using char8 = unsigned char;
using char16 = char16_t;
using char32 = char32_t;

/* USAGE: Constant strings.

	For whole null-terminated strings whose contents will not be changed, use conststring32:
		void writeString (conststring32 text);

	For a pointer to a character in a conststring32, use "const char32 *".
	This is appropriate if you need to cycle later:
		const char32 *p = & text [0];
		...
		p ++;
	This is also appropriate for character searches:
		const char32 *semicolonLocation = str32chr (text, U';');

	For an array of characters, use "const char32 []":
		void displayCharacters (const char32 characters []);
	Such an array may or may not be null-terminated.
*/
using conststring8 = const char *;
using conststring16 = const char16 *;
using conststringW = const wchar_t *;
using conststring32 = const char32 *;

/* USAGE: Mutable strings.

	For whole null-terminated strings whose contents will be changed, use mutablestring32:
		void changeCase (mutablestring32 string);

	For a pointer to a character in a mutablestring32, use "char32 *".
	This is appropriate if you need to cycle later:
		char32 *p = & string [0];
		...
		p ++;
	This is also appropriate for character searches:
		char32 *semicolonLocation = str32chr (string, U';');

	For an array of characters that will be changed, use "char32 []":
		void modifyCodes (char32 codes []);
	Such an array may or may not be null-terminated.
*/
using mutablestring8 = char *;
using mutablestring16 = char16 *;
using mutablestringW = wchar_t *;
using mutablestring32 = char32 *;

#define strequ  ! strcmp
#define strnequ  ! strncmp

template <class T>
class _autostring {
	T *ptr;
public:
	#if 1
	_autostring () : ptr (nullptr) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: zero constructor");
	}
	#else
	_autostring () = default;   // explicit default, so that it can be used in a union
	#endif
	_autostring (integer length, bool f = false) {
		our ptr = ( f ? Melder_malloc_f (T, length + 1) : Melder_malloc (T, length + 1) );
		our ptr [0] = '\0';
		our ptr [length] = '\0';
	}
	//_autostring (T *string) : ptr (string) {
		//if (Melder_debug == 39) Melder_casual (U"autostring: constructor from C-string ", Melder_pointer (ptr));
	//}
	~_autostring () {
		//if (Melder_debug == 39) Melder_casual (U"autostring: entering destructor ptr = ", Melder_pointer (ptr));
		if (our ptr) Melder_free (our ptr);
		//if (Melder_debug == 39) Melder_casual (U"autostring: leaving destructor");
	}
	template <class U> T& operator[] (U i) {
		return our ptr [i];
	}
	T * get () const {
		return our ptr;
	}
	#if 0
	operator T* () const {
		return our ptr;
	}
	#endif
	/*T ** operator& () {
		return & our ptr;
	}*/
	T * transfer () {
		T *tmp = our ptr;
		our ptr = nullptr;
		return tmp;
	}
	void reset () {
		if (our ptr) Melder_free (our ptr);
	}
	void resize (int64 newLength) {
		T *tmp = (T *) Melder_realloc (our ptr, (newLength + 1) * (int64) sizeof (T));
		our ptr = tmp;
		our ptr [newLength] = '\0';
	}
	_autostring& operator= (const _autostring&) = delete;   // disable copy assignment
	_autostring (_autostring &) = delete;   // disable copy constructor
	template <class Y> _autostring (_autostring<Y> &) = delete;   // disable copy constructor
	explicit operator bool () const { return !! our ptr; }
	/*
		Enable moving.
	*/
	_autostring (_autostring&& other) noexcept {   // enable move constructor
		our ptr = other.ptr;
		other.ptr = nullptr;
	}
	_autostring& operator= (_autostring&& other) noexcept {   // enable move assignment
		if (& other != this) {
			if (our ptr) Melder_free (our ptr);
			our ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}
	_autostring&& move () noexcept {
		return static_cast <_autostring&&> (*this);
	}
	void _zero_asInUnion () {
		our ptr = nullptr;
	}
};

typedef _autostring <char> autostring8;
typedef _autostring <char16> autostring16;
typedef _autostring <wchar_t> autostringW;
typedef _autostring <char32> autostring32;

autostring32 Melder_dup (conststring32 string /* cattable */);
autostring32 Melder_dup_f (conststring32 string /* cattable */);

#pragma mark - CHARACTER PROPERTIES

#define kUCD_TOP_OF_ASCII  127
#define kUCD_TOP_OF_LIST  0x2FA1D
#define kUCD_UNASSIGNED  0

enum {
	mUCD_UPPERCASE_LETTER = (1 << 0),
	mUCD_LOWERCASE_LETTER = (1 << 1),
	mUCD_TITLECASE_LETTER = (1 << 2),
	mUCD_CASED_LETTER = (mUCD_UPPERCASE_LETTER | mUCD_LOWERCASE_LETTER | mUCD_TITLECASE_LETTER),
	mUCD_MODIFIER_LETTER = (1 << 3),
	mUCD_OTHER_LETTER = (1 << 4),
	mUCD_LETTER = (mUCD_CASED_LETTER | mUCD_MODIFIER_LETTER | mUCD_OTHER_LETTER),

	mUCD_NONSPACING_MARK = (1 << 5),
	mUCD_SPACING_MARK = (1 << 6),
	mUCD_ENCLOSING_MARK = (1 << 7),
	mUCD_MARK = (mUCD_NONSPACING_MARK | mUCD_SPACING_MARK | mUCD_ENCLOSING_MARK),

	mUCD_DECIMAL_NUMBER = (1 << 8),
	mUCD_LETTER_NUMBER = (1 << 9),
	mUCD_OTHER_NUMBER = (1 << 10),
	mUCD_NUMBER = (mUCD_DECIMAL_NUMBER | mUCD_LETTER_NUMBER | mUCD_OTHER_NUMBER),

	mUCD_CONNECTOR_PUNCTUATION = (1 << 11),
	mUCD_DASH_PUNCTUATION = (1 << 12),
	mUCD_OPEN_PUNCTUATION = (1 << 13),
	mUCD_CLOSE_PUNCTUATION = (1 << 14),
	mUCD_INITIAL_PUNCTUATION = (1 << 15),
	mUCD_FINAL_PUNCTUATION = (1 << 16),
	mUCD_OTHER_PUNCTUATION = (1 << 17),
	mUCD_PUNCTUATION = (mUCD_CONNECTOR_PUNCTUATION | mUCD_DASH_PUNCTUATION | mUCD_OPEN_PUNCTUATION | mUCD_CLOSE_PUNCTUATION | mUCD_INITIAL_PUNCTUATION | mUCD_FINAL_PUNCTUATION | mUCD_OTHER_PUNCTUATION),

	mUCD_MATH_SYMBOL = (1 << 18),
	mUCD_CURRENCY_SYMBOL = (1 << 19),
	mUCD_MODIFIER_SYMBOL = (1 << 20),
	mUCD_OTHER_SYMBOL = (1 << 21),
	mUCD_SYMBOL = (mUCD_MATH_SYMBOL | mUCD_CURRENCY_SYMBOL | mUCD_MODIFIER_SYMBOL | mUCD_OTHER_SYMBOL),

	mUCD_BREAKING_SPACE = (1 << 22),
	mUCD_NON_BREAKING_SPACE = (1 << 23),
	mUCD_SPACE_SEPARATOR = (mUCD_BREAKING_SPACE | mUCD_NON_BREAKING_SPACE),
	mUCD_LINE_SEPARATOR = (1 << 24),
	mUCD_PARAGRAPH_SEPARATOR = (1 << 25),
	mUCD_NEWLINE = (mUCD_LINE_SEPARATOR | mUCD_PARAGRAPH_SEPARATOR),
	mUCD_SEPARATOR = (mUCD_SPACE_SEPARATOR | mUCD_NEWLINE),

	mUCD_CONTROL = (1 << 26),
	mUCD_FORMAT = (1 << 27),
	mUCD_PRIVATE_USE = (1 << 28),

	mUCD_WORD_CHARACTER = (1 << 29),
	mUCD_NULL = (1 << 30),

	mUCD_ALPHANUMERIC = (mUCD_LETTER | mUCD_NUMBER),
	mUCD_END_OF_INK = (mUCD_SEPARATOR | mUCD_NULL),
	mUCD_END_OF_LINE = (mUCD_NEWLINE | mUCD_NULL),
};

struct UCD_CodePointInfo {
	uint32 features;
	char32 upperCase, lowerCase, titleCase;
	char first, second;
};
extern UCD_CodePointInfo theUnicodeDatabase [1+kUCD_TOP_OF_LIST];
enum class kMelder_charset { ASCII_, UNICODE_ };

/*
	Praat is an internationalized program, which means it has to work in the same way
	wherever on earth it is used. This means that Praat has to be blind to localized settings,
	such as what counts as a space and what combinations of characters
	count as pairs of lower case and upper case.

	To be able to use Praat all over the world, we therefore define one single
	"international locale", which is simply based on the Unicode features of each code point.
*/

/*
	Internationalize std::isblank ():
*/
inline static bool Melder_isHorizontalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_SPACE_SEPARATOR) != 0;
}
inline static void Melder_skipHorizontalSpace (char32 **p_text) {
	while (Melder_isHorizontalSpace (**p_text)) (*p_text) ++;
}
inline static char32 * Melder_findEndOfHorizontalSpace (char32 *p) {
	while (Melder_isHorizontalSpace (*p)) p ++;
	return p;
}
inline static const char32 * Melder_findEndOfHorizontalSpace (const char32 *p) {
	while (Melder_isHorizontalSpace (*p)) p ++;
	return p;
}

inline static bool Melder_isAsciiHorizontalSpace (char32 kar) {
	return kar == U'\t' || kar == U' ';
}

inline static bool Melder_isVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_NEWLINE) != 0;
}
inline static bool Melder_isAsciiVerticalSpace (char32 kar) {
	return kar >= 10 && kar <= 13;   // \n, \v, \f, \r
}

/*
	Internationalize std::isspace ():
*/
inline static bool Melder_isHorizontalOrVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_SEPARATOR) != 0;
}
inline static bool Melder_isHorizontalOrVerticalSpace (char32 kar, kMelder_charset charset) {
	const char32 top = charset == kMelder_charset::ASCII_ ? kUCD_TOP_OF_ASCII : kUCD_TOP_OF_LIST;
	return kar <= top && (theUnicodeDatabase [kar]. features & mUCD_SEPARATOR) != 0;
}
inline static bool Melder_isAsciiHorizontalOrVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_SEPARATOR) != 0;
}

inline static bool Melder_isEndOfInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_INK) != 0;
}
inline static bool Melder_isEndOfLine (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_LINE) != 0;
}
inline static bool Melder_isEndOfText (char32 kar) {
	return kar == U'\0';
}
inline static bool Melder_staysWithinInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_INK) == 0;
}
inline static bool Melder_staysWithinLine (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_LINE) == 0;
}
inline static void Melder_skipToEndOfLine (char32 **p_text) {
	while (Melder_staysWithinLine (**p_text)) (*p_text) ++;
}
inline static char32 * Melder_findEndOfInk (char32 *p) {
	while (Melder_staysWithinInk (*p)) p ++;
	return p;
}
inline static const char32 * Melder_findEndOfInk (const char32 *p) {
	while (Melder_staysWithinInk (*p)) p ++;
	return p;
}
inline static char32 * Melder_findEndOfLine (char32 *p) {
	while (Melder_staysWithinLine (*p)) p ++;
	return p;
}
inline static const char32 * Melder_findEndOfLine (const char32 *p) {
	while (Melder_staysWithinLine (*p)) p ++;
	return p;
}
/*
	Internationalize std::isalpha ():
*/
inline static bool Melder_isLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_LETTER) != 0;
}
inline static bool Melder_isAsciiLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_LETTER) != 0;
}

/*
	Internationalize std::isupper ():
*/
inline static bool Melder_isUpperCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_UPPERCASE_LETTER) != 0;
}
inline static bool Melder_isAsciiUpperCaseLetter (char32 kar) {
	return kar >= U'A' && kar <= U'Z';
}

/*
	Internationalize std::islower ():
*/
inline static bool Melder_isLowerCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_LOWERCASE_LETTER) != 0;
}
inline static bool Melder_isAsciiLowerCaseLetter (char32 kar) {
	return kar >= U'a' && kar <= U'z';
}

inline static bool Melder_isTitleCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_TITLECASE_LETTER) != 0;
}
inline static bool Melder_isAsciiTitleCaseLetter (char32 kar) {
	return kar >= U'A' && kar <= U'Z';
}

/*
	Internationalize std::isdigit ():
*/
inline static bool Melder_isDecimalNumber (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_DECIMAL_NUMBER) != 0;
}
inline static bool Melder_isAsciiDecimalNumber (char32 kar) {
	return kar >= U'0' && kar <= U'9';
}

/*
	We cannot really internationalize std::isxdigit ():
*/
inline static bool Melder_isHexadecimalDigit (char32 kar) {
	return kar >= U'0' && kar <= U'9' || kar >= U'A' && kar <= U'Z' || kar >= U'a' && kar <= U'z';
}

/*
	Internationalize std::isalnum ():
*/
inline static bool Melder_isAlphanumeric (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_ALPHANUMERIC) != 0;
}
inline static bool Melder_isAsciiAlphanumeric (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_ALPHANUMERIC) != 0;
}

inline static bool Melder_isWordCharacter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_WORD_CHARACTER) != 0;
}
inline static bool Melder_isAsciiWordCharacter (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_WORD_CHARACTER) != 0;
}

/*
	The standard library further contains std::ispunct (), std::iscntrl (), std::isprint (), std::isgraph ().
	These have very little use nowadays, so only for completeness do we include versions of them here,
	which are correct at least for ASCII arguments.
	Of these four functions, Melder_hasInk () is not yet correct for all Unicode points,
	as approximately one half of the mUCD_FORMAT points are inkless as well.
*/
inline static bool Melder_isPunctuationOrSymbol (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & (mUCD_PUNCTUATION | mUCD_SYMBOL)) != 0;
}
inline static bool Melder_isAsciiPunctuationOrSymbol (char32 kar) {   // same as std::ispunct() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & (mUCD_PUNCTUATION | mUCD_SYMBOL)) != 0;
}
inline static bool Melder_isControl (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) != 0;
}
inline static bool Melder_isAsciiControl (char32 kar) {   // same as std::iscntrl() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) != 0;
}
inline static bool Melder_isPrintable (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) == 0;
}
inline static bool Melder_isAsciiPrintable (char32 kar) {   // same as std::isprint() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) == 0;
}
inline static bool Melder_hasInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & (mUCD_CONTROL | mUCD_SEPARATOR)) == 0;
}
inline static bool Melder_hasAsciiInk (char32 kar) {   // same as std::isgraph() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & (mUCD_CONTROL | mUCD_SEPARATOR)) == 0;
}

/*
	Internationalize std::toupper () and std::tolower ():
*/
inline static char32 Melder_toUpperCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. upperCase : kar;
}
inline static char32 Melder_toLowerCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. lowerCase : kar;
}
inline static char32 Melder_toTitleCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. titleCase : kar;
}

inline static integer str16len (conststring16 string) noexcept {
	const char16 *p = & string [0];
	while (*p != u'\0') ++ p;
	return p - string;
}
inline static mutablestring16 str16cpy (mutablestring16 target, conststring16 source) noexcept {
	char16 *p = & target [0];
	while (* source != u'\0') * p ++ = * source ++;
	*p = u'\0';
	return target;
}

inline static integer str32len (conststring32 string) noexcept {
	const char32 *p = & string [0];
	while (*p != U'\0') ++ p;
	return p - string;
}
inline static mutablestring32 str32cpy (mutablestring32 target, conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}
inline static char32 * stp32cpy (mutablestring32 target, conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return p;
}
inline static mutablestring32 str32ncpy (mutablestring32 target, conststring32 source, integer n) noexcept {
	char32 *p = & target [0];
	for (; n > 0 && *source != U'\0'; -- n) * p ++ = * source ++;
	for (; n > 0; -- n) * p ++ = U'\0';
	return target;
}

inline static int str32cmp (conststring32 string1, conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
inline static int str32cmp_caseInsensitive (conststring32 string1, conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) Melder_toLowerCase (*string1) - (int32) Melder_toLowerCase (*string2);
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
inline static int str32cmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, bool caseSensitive) noexcept {
	return caseSensitive ? str32cmp (string1, string2) : str32cmp_caseInsensitive (string1, string2);
}
inline static int str32ncmp (conststring32 string1, conststring32 string2, integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
inline static int str32ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) Melder_toLowerCase (*string1) - (int32) Melder_toLowerCase (*string2);
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
inline static int str32ncmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, integer n, bool caseSensitive) noexcept {
	return caseSensitive ? str32ncmp (string1, string2, n) : str32ncmp_caseInsensitive (string1, string2, n);
}

int Melder_cmp (conststring32 string1, conststring32 string2);   // regards null string as empty string
int Melder_cmp_caseInsensitive (conststring32 string1, conststring32 string2);
int Melder_ncmp (conststring32 string1, conststring32 string2, integer n);
int Melder_ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n);

#define str32equ  ! str32cmp
#define str32nequ  ! str32ncmp
#define Melder_equ  ! Melder_cmp
#define str32equ_caseInsensitive  ! str32cmp_caseInsensitive
#define str32nequ_caseInsensitive  ! str32ncmp_caseInsensitive
#define Melder_equ_caseInsensitive  ! Melder_cmp_caseInsensitive
#define str32equ_optionallyCaseSensitive  ! str32cmp_optionallyCaseSensitive
#define str32nequ_optionallyCaseSensitive  ! str32ncmp_optionallyCaseSensitive
bool Melder_equ_firstCharacterCaseInsensitive (conststring32 string1, conststring32 string2);
#define Melder_nequ  ! Melder_ncmp
#define Melder_nequ_caseInsensitive  ! Melder_ncmp_caseInsensitive

inline static char32 * str32chr (conststring32 string, char32 kar) noexcept {
	for (; *string != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline static char32 * str32chr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	for (; Melder_toLowerCase (*string) != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline static char32 * str32rchr (conststring32 string, char32 kar) noexcept {
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (*string == kar) result = (char32 *) string;
	}
	return result;
}
inline static char32 * str32rchr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (Melder_toLowerCase (*string) == kar) result = (char32 *) string;
	}
	return result;
}
inline static char32 * str32str (conststring32 string, conststring32 find) noexcept {
	integer length = str32len (find);
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
inline static char32 * str32str_caseInsensitive (conststring32 string, conststring32 find) noexcept {
	integer length = str32len (find);
	if (length == 0) return (char32 *) string;
	char32 firstCharacter = Melder_toLowerCase (* find ++);   // optimization
	do {
		char32 kar;
		do {
			kar = Melder_toLowerCase (* string ++);
			if (kar == U'\0') return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp_caseInsensitive (string, find, length - 1));
	return (char32 *) (string - 1);
}
inline static char32 * str32str_optionallyCaseSensitive (conststring32 string, conststring32 find, bool caseSensitive) noexcept {
	return caseSensitive ? str32str (string, find) : str32str_caseInsensitive (string, find);
}
inline static integer str32spn (conststring32 string1, conststring32 string2) noexcept {
	const char32 *p = & string1 [0];
	char32 kar1, kar2;
cont:
	kar1 = * p ++;
	for (const char32 *q = & string2 [0]; (kar2 = * q ++) != U'\0';)
		if (kar2 == kar1)
			goto cont;
	return p - 1 - string1;
}

char32 * Melder_tok (char32 *string, conststring32 delimiter);

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

/*
	The following functions return a static string, chosen from a circularly used set of 32 buffers.
	You can call at most 32 of them in one Melder_casual call, for instance.
*/

conststring32 Melder_integer (int64 value) noexcept;
conststring8 Melder8_integer (int64 value) noexcept;

conststring32 Melder_bigInteger (int64 value) noexcept;
conststring8 Melder8_bigInteger (int64 value) noexcept;

conststring32 Melder_boolean (bool value) noexcept;
conststring8 Melder8_boolean (bool value) noexcept;
	// "yes" or "no"

/**
	Format a double value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats.
*/
conststring32 Melder_double (double value) noexcept;
conststring8 Melder8_double (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.9g" format.
*/
conststring32 Melder_single (double value) noexcept;
conststring8 Melder8_single (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.4g" format.
*/
conststring32 Melder_half (double value) noexcept;
conststring8 Melder8_half (double value) noexcept;

/**
	Format a double value as "--undefined--" or something in the "%.*f" format.
*/
conststring32 Melder_fixed (double value, integer precision) noexcept;
conststring8 Melder8_fixed (double value, integer precision) noexcept;

/**
	Format a double value with a specified precision. If exponent is -2 and precision is 2, you get things like 67E-2 or 0.00024E-2.
*/
conststring32 Melder_fixedExponent (double value, integer exponent, integer precision) noexcept;
conststring8 Melder8_fixedExponent (double value, integer exponent, integer precision) noexcept;

/**
	Format a double value as a percentage. If precision is 3, you get things like "0" or "34.400%" or "0.014%" or "0.001%" or "0.0000007%".
*/
conststring32 Melder_percent (double value, integer precision) noexcept;
conststring8 Melder8_percent (double value, integer precision) noexcept;

/**
	Format an integer as a hexadecimal number. If precision is 4, you get things like "0000" or "1A3C" or "107FFFF".
*/
conststring8 Melder8_hexadecimal (integer value, integer precision) noexcept;
conststring32 Melder_hexadecimal (integer value, integer precision) noexcept;

/**
	Format a dcomplex value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats,
	separated without spaces by "+" or "-" and followed by "i".
*/
conststring32 Melder_dcomplex (dcomplex value) noexcept;
conststring8 Melder8_dcomplex (dcomplex value) noexcept;

/**
	Format a dcomplex value as "--undefined--" or something in the "%.9g" format,
	separated without spaces by "+" or "-" and followed by "i".
*/
conststring32 Melder_scomplex (dcomplex value) noexcept;
conststring8 Melder8_scomplex (dcomplex value) noexcept;

/**
	Convert a formatted floating-point string to something suitable for visualization with the Graphics library.
	For instance, "1e+4" is turned into "10^^4", and "-1.23456e-78" is turned into "-1.23456\.c10^^-78".
*/
conststring32 Melder_float (conststring32 number) noexcept;

/**
	Format the number that is specified by its natural logarithm.
	For instance, -10000 is formatted as "1.135483865315339e-4343", which is a floating-point representation of exp(-10000).
*/
conststring32 Melder_naturalLogarithm (double lnNumber) noexcept;
conststring8 Melder8_naturalLogarithm (double lnNumber) noexcept;

conststring32 Melder_pointer (void *pointer) noexcept;
conststring8 Melder8_pointer (void *pointer) noexcept;

conststring32 Melder_character (char32 kar) noexcept;
conststring8 Melder8_character (char32 kar) noexcept;

conststring32 Melder_pad (int64 width, conststring32 string);   // will append spaces to the left of 'string' until 'width' is reached; no truncation
conststring32 Melder_pad (conststring32 string, int64 width);   // will append spaces to the right of 'string' until 'width' is reached; no truncation
conststring32 Melder_truncate (int64 width, conststring32 string);   // will cut away the left of 'string' until 'width' is reached; no padding
conststring32 Melder_truncate (conststring32 string, int64 width);   // will cut away the right of 'string' until 'width' is reached; no padding
conststring32 Melder_padOrTruncate (int64 width, conststring32 string);   // will cut away, or append spaces to, the left of 'string' until 'width' is reached
conststring32 Melder_padOrTruncate (conststring32 string, int64 width);   // will cut away, or append spaces to, the right of 'string' until 'width' is reached

#pragma mark - CONSOLE

void Melder_writeToConsole (conststring32 message, bool useStderr);

/**
 * Text encodings.
 */
void Melder_textEncoding_prefs ();
void Melder_setInputEncoding (kMelder_textInputEncoding encoding);
kMelder_textInputEncoding Melder_getInputEncoding ();
void Melder_setOutputEncoding (kMelder_textOutputEncoding encoding);
kMelder_textOutputEncoding Melder_getOutputEncoding ();

/*
 * Some other encodings. Although not used in the above set/get functions,
 * these constants should stay separate from the above encoding constants
 * because they occur in the same fields of struct MelderFile.
 */
//const uint32 kMelder_textInputEncoding_FLAC = 0x464C4143;
const uint32 kMelder_textOutputEncoding_ASCII = 0x41534349;
const uint32 kMelder_textOutputEncoding_ISO_LATIN1 = 0x4C415401;
const uint32 kMelder_textOutputEncoding_FLAC = 0x464C4143;

bool Melder_isValidAscii (conststring32 string);
bool Melder_str8IsValidUtf8 (const char *string);
bool Melder_isEncodable (conststring32 string, int outputEncoding);
extern char32 Melder_decodeMacRoman [256];
extern char32 Melder_decodeWindowsLatin1 [256];

integer Melder_killReturns_inplace (mutablestring32 text);
integer Melder_killReturns_inplace (mutablestring8 text);
/*
	 Replaces all bare returns (old Mac) or return-plus-linefeed sequences (Win) with bare linefeeds
	 (generic: Unix and modern Mac).
	 Returns new length of string (equal to or less than old length).
*/

size_t str32len_utf8  (conststring32 string, bool nativizeNewlines);
size_t str32len_utf16 (conststring32 string, bool nativizeNewlines);

extern "C" conststring32 Melder_peek8to32 (conststring8 string);
void Melder_8to32_inplace (conststring8 source, mutablestring32 target, kMelder_textInputEncoding inputEncoding);
	// errors: Text is not valid UTF-8.
autostring32 Melder_8to32 (conststring8 string, kMelder_textInputEncoding inputEncoding);
	// errors: Out of memory; Text is not valid UTF-8.
autostring32 Melder_8to32 (conststring8 string);
	// errors: Out of memory; Text is not valid UTF-8.

conststring32 Melder_peek16to32 (conststring16 text);
autostring32 Melder_16to32 (conststring16 text);

extern "C" conststring8 Melder_peek32to8 (conststring32 string);
void Melder_32to8_inplace (conststring32 string, mutablestring8 utf8);
autostring8 Melder_32to8 (conststring32 string);
autostring16 Melder_32to16 (conststring32 string);
	// errors: Out of memory.

conststring16 Melder_peek32to16 (conststring32 text, bool nativizeNewlines);
extern "C" conststring16 Melder_peek32to16 (conststring32 string);

#ifdef _WIN32
	inline static conststringW Melder_peek32toW (conststring32 string) { return (conststringW) Melder_peek32to16 (string); }
	autostringW Melder_32toW (conststring32 string);
	inline static conststring32 Melder_peekWto32 (conststringW string) { return Melder_peek16to32 ((conststring16) string); }
	inline static autostring32 Melder_Wto32 (conststringW string) { return Melder_16to32 ((conststring16) string); }
#endif

void Melder_str32To8bitFileRepresentation_inplace (conststring32 string, mutablestring8 utf8);
void Melder_8bitFileRepresentationToStr32_inplace (conststring8 utf8, mutablestring32 string);
const void * Melder_peek32toCfstring (conststring32 string);
void Melder_fwrite32to8 (conststring32 string, FILE *f);

#pragma mark - STRING TO NUMBER CONVERSION

bool Melder_isStringNumeric (conststring32 string) noexcept;
double Melder_a8tof (conststring8 string) noexcept;
double Melder_atof (conststring32 string) noexcept;
int64 Melder_atoi (conststring32 string) noexcept;
	/*
	 * "3.14e-3" -> 3.14e-3
	 * "15.6%" -> 0.156
	 * "fghfghj" -> undefined
	 */

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
	uint32 outputEncoding;
	int indent;
	struct FLAC__StreamEncoder *flacEncoder;
};
typedef struct structMelderFile *MelderFile;

struct structMelderDir {
	char32 path [kMelder_MAXPATH+1];
};
typedef struct structMelderDir *MelderDir;

conststring32 MelderFile_name (MelderFile file);
conststring32 MelderDir_name (MelderDir dir);
void Melder_pathToDir (conststring32 path, MelderDir dir);
void Melder_pathToFile (conststring32 path, MelderFile file);
void Melder_relativePathToFile (conststring32 path, MelderFile file);
conststring32 Melder_dirToPath (MelderDir dir);
	/* Returns a pointer internal to 'dir', like "/u/paul/praats" or "D:\Paul\Praats" */
conststring32 Melder_fileToPath (MelderFile file);
void MelderFile_copy (MelderFile file, MelderFile copy);
void MelderDir_copy (MelderDir dir, MelderDir copy);
bool MelderFile_equal (MelderFile file1, MelderFile file2);
bool MelderDir_equal (MelderDir dir1, MelderDir dir2);
void MelderFile_setToNull (MelderFile file);
bool MelderFile_isNull (MelderFile file);
void MelderDir_setToNull (MelderDir dir);
bool MelderDir_isNull (MelderDir dir);
void MelderDir_getFile (MelderDir parent, conststring32 fileName, MelderFile file);
void MelderDir_relativePathToFile (MelderDir dir, conststring32 path, MelderFile file);
void MelderFile_getParentDir (MelderFile file, MelderDir parent);
void MelderDir_getParentDir (MelderDir file, MelderDir parent);
bool MelderDir_isDesktop (MelderDir dir);
void MelderDir_getSubdir (MelderDir parent, conststring32 subdirName, MelderDir subdir);
void Melder_rememberShellDirectory ();
conststring32 Melder_getShellDirectory ();
void Melder_getHomeDir (MelderDir homeDir);
void Melder_getPrefDir (MelderDir prefDir);
void Melder_getTempDir (MelderDir tempDir);

bool MelderFile_exists (MelderFile file);
bool MelderFile_readable (MelderFile file);
integer MelderFile_length (MelderFile file);
void MelderFile_delete (MelderFile file);

/* The following two should be combined with each other and with Windows extension setting: */
FILE * Melder_fopen (MelderFile file, const char *type);
void Melder_fclose (MelderFile file, FILE *stream);
void Melder_files_cleanUp ();

/* Use the following functions to pass unchanged text or file names to Melder_* functions. */
/* Backslashes are replaced by "\bs". */
/* The trick is that they return one of 11 cyclically used static strings, */
/* so you can use up to 11 strings in a single Melder_* call. */
char32 * Melder_peekExpandBackslashes (conststring32 message);
conststring32 MelderFile_messageName (MelderFile file);   // calls Melder_peekExpandBackslashes ()

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

#define undefined  (0.0/0.0)   /* NaN */

/*
	isdefined() shall capture not only `undefined`, but all infinities and NaNs.
	This can be done with a single test for the set bits in 0x7FF0000000000000,
	at least for 64-bit IEEE implementations. The correctness of this assumption is checked in sys/praat.cpp.
	The portable version of isdefined() involves both isinf() and isnan(), or perhaps just isfinite(),
	but that would be slower (as tested in fon/Praat_tests.cpp)
	and it would also get into problems on some platforms whenever both <cmath> and <math.h> are included,
	as in dwsys/NUMcomplex.cpp.
*/
//inline static bool isdefined (double x) { return ! isinf (x) && ! isnan (x); }   /* portable */
inline static bool isdefined (double x) { return ((* (uint64 *) & x) & 0x7FF0000000000000) != 0x7FF0000000000000; }
inline static bool isundef (double x) { return ((* (uint64 *) & x) & 0x7FF0000000000000) == 0x7FF0000000000000; }

/********** Arrays with one index (NUMarrays.cpp) **********/

byte * NUMvector_generic (integer elementSize, integer lo, integer hi, bool zero);
/*
	Function:
		create a vector [lo...hi]; if `zero`, then all values are initialized to 0.
	Preconditions:
		hi >= lo;
*/

void NUMvector_free_generic (integer elementSize, byte *v, integer lo) noexcept;
/*
	Function:
		destroy a vector v that was created with NUMvector.
	Preconditions:
		lo must have the same values as with the creation of the vector.
*/

byte * NUMvector_copy_generic (integer elementSize, byte *v, integer lo, integer hi);
/*
	Function:
		copy (part of) a vector v, which need not have been created with NUMvector, to a new one.
	Preconditions:
		if v != nullptr, the values v [lo..hi] must exist.
*/

void NUMvector_copyElements_generic (integer elementSize, byte *v, byte *to, integer lo, integer hi);
/*
	copy the vector elements v [lo..hi] to those of a vector 'to'.
	These vectors need not have been created by NUMvector.
*/

bool NUMvector_equal_generic (integer elementSize, byte *v1, byte *v2, integer lo, integer hi);
/*
	return true if the vector elements v1 [lo..hi] are equal
	to the corresponding elements of the vector v2; otherwise, return false.
	The vectors need not have been created by NUMvector.
*/

void NUMvector_append_generic (integer elementSize, byte **v, integer lo, integer *hi);
void NUMvector_insert_generic (integer elementSize, byte **v, integer lo, integer *hi, integer position);
/*
	add one element to the vector *v.
	The new element is initialized to zero.
	On success, *v points to the new vector, and *hi is incremented by 1.
	On failure, *v and *hi are not changed.
*/

/********** Arrays with two indices (NUMarrays.cpp) **********/

void * NUMmatrix (integer elementSize, integer row1, integer row2, integer col1, integer col2, bool zero);
/*
	Function:
		create a matrix [row1...row2] [col1...col2]; if `zero`, then all values are initialized to 0.
	Preconditions:
		row2 >= row1;
		col2 >= col1;
*/

void NUMmatrix_free_ (integer elementSize, byte **m, integer row1, integer col1) noexcept;
/*
	Function:
		destroy a matrix m created with NUM...matrix.
	Preconditions:
		if m != nullptr: row1 and col1
		must have the same value as with the creation of the matrix.
*/

void * NUMmatrix_copy (integer elementSize, void *m, integer row1, integer row2, integer col1, integer col2);
/*
	Function:
		copy (part of) a matrix m, wich does not have to be created with NUMmatrix, to a new one.
	Preconditions:
		if m != nullptr: the values m [rowmin..rowmax] [colmin..colmax] must exist.
*/

void NUMmatrix_copyElements_ (integer elementSize, char **mfrom, char **mto, integer row1, integer row2, integer col1, integer col2);
/*
	copy the matrix elements m [r1..r2] [c1..c2] to those of a matrix 'to'.
	These matrices need not have been created by NUMmatrix.
*/

bool NUMmatrix_equal (integer elementSize, void *m1, void *m2, integer row1, integer row2, integer col1, integer col2);
/*
	return 1 if the matrix elements m1 [r1..r2] [c1..c2] are equal
	to the corresponding elements of the matrix m2; otherwise, return 0.
	The matrices need not have been created by NUM...matrix.
*/

integer NUM_getTotalNumberOfArrays ();   // for debugging

/********** Special functions (NUM.cpp) **********/

double NUMlnGamma (double x);
double NUMbeta (double z, double w);
double NUMbesselI (integer n, double x);   // precondition: n >= 0
double NUMbessel_i0_f (double x);
double NUMbessel_i1_f (double x);
double NUMbesselK (integer n, double x);   // preconditions: n >= 0 && x > 0.0
double NUMbessel_k0_f (double x);
double NUMbessel_k1_f (double x);
double NUMbesselK_f (integer n, double x);
double NUMsigmoid (double x);   // correct also for large positive or negative x
double NUMinvSigmoid (double x);
double NUMerfcc (double x);
double NUMgaussP (double z);
double NUMgaussQ (double z);
double NUMincompleteGammaP (double a, double x);
double NUMincompleteGammaQ (double a, double x);
double NUMchiSquareP (double chiSquare, double degreesOfFreedom);
double NUMchiSquareQ (double chiSquare, double degreesOfFreedom);
double NUMcombinations (integer n, integer k);
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

/********** Interpolation and optimization (NUM.cpp) **********/

// Special values for interpolationDepth:
#define NUM_VALUE_INTERPOLATE_NEAREST  0
#define NUM_VALUE_INTERPOLATE_LINEAR  1
#define NUM_VALUE_INTERPOLATE_CUBIC  2
// Higher values than 2 yield a true sinc interpolation. Here are some examples:
#define NUM_VALUE_INTERPOLATE_SINC70  70
#define NUM_VALUE_INTERPOLATE_SINC700  700
double NUM_interpolate_sinc (double y [], integer nx, double x, integer interpolationDepth);

#define NUM_PEAK_INTERPOLATE_NONE  0
#define NUM_PEAK_INTERPOLATE_PARABOLIC  1
#define NUM_PEAK_INTERPOLATE_CUBIC  2
#define NUM_PEAK_INTERPOLATE_SINC70  3
#define NUM_PEAK_INTERPOLATE_SINC700  4

double NUMimproveExtremum (double *y, integer nx, integer ixmid, int interpolation, double *ixmid_real, int isMaximum);
double NUMimproveMaximum (double *y, integer nx, integer ixmid, int interpolation, double *ixmid_real);
double NUMimproveMinimum (double *y, integer nx, integer ixmid, int interpolation, double *ixmid_real);

void NUM_viterbi (
	integer numberOfFrames, integer maxnCandidates,
	integer (*getNumberOfCandidates) (integer iframe, void *closure),
	double (*getLocalCost) (integer iframe, integer icand, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, void *closure),
	void (*putResult) (integer iframe, integer place, void *closure),
	void *closure);

void NUM_viterbi_multi (
	integer nframe, integer ncand, integer ntrack,
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure),
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure),
	void *closure);

/********** Metrics (NUM.cpp) **********/

int NUMrotationsPointInPolygon
	(double x0, double y0, integer n, double x [], double y []);
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

integer NUMrandomInteger (integer lowest, integer highest);

bool NUMrandomBernoulli (double probability);
double NUMrandomBernoulli_real (double probability);

double NUMrandomGauss (double mean, double standardDeviation);
double NUMrandomGauss_mt (int threadNumber, double mean, double standardDeviation);

double NUMrandomPoisson (double mean);

uint32 NUMhashString (conststring32 string);

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2);
void NUMfilterSecondOrderSection_a (double x [], integer n, double a1, double a2);
void NUMfilterSecondOrderSection_fb (double x [], integer n, double dt, double formant, double bandwidth);
double NUMftopreemphasis (double f, double dt);
void NUMpreemphasize_a (double x [], integer n, double preemphasis);
void NUMdeemphasize_a (double x [], integer n, double preemphasis);
void NUMpreemphasize_f (double x [], integer n, double dt, double frequency);
void NUMdeemphasize_f (double x [], integer n, double dt, double frequency);
void NUMautoscale (double x [], integer n, double scale);

typedef struct structNUMlinprog *NUMlinprog;
void NUMlinprog_delete (NUMlinprog me);
NUMlinprog NUMlinprog_new (bool maximize);
void NUMlinprog_addVariable (NUMlinprog me, double lowerBound, double upperBound, double coeff);
void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound);
void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient);
void NUMlinprog_run (NUMlinprog me);
double NUMlinprog_getPrimalValue (NUMlinprog me, integer ivar);

template <class T>
T* NUMvector (integer from, integer to) {
	T* result = reinterpret_cast <T*> (NUMvector_generic (sizeof (T), from, to, true));
	return result;
}

template <class T>
T* NUMvector (integer from, integer to, bool initializeToZero) {
	T* result = reinterpret_cast <T*> (NUMvector_generic (sizeof (T), from, to, initializeToZero));
	return result;
}

template <class T>
void NUMvector_free (T* ptr, integer from) noexcept {
	NUMvector_free_generic (sizeof (T), reinterpret_cast <byte *> (ptr), from);
}

template <class T>
T* NUMvector_copy (T* ptr, integer lo, integer hi) {
	T* result = reinterpret_cast <T*> (NUMvector_copy_generic (sizeof (T), reinterpret_cast <byte *> (ptr), lo, hi));
	return result;
}

template <class T>
bool NUMvector_equal (T* v1, T* v2, integer lo, integer hi) {
	return NUMvector_equal_generic (sizeof (T), reinterpret_cast <byte *> (v1), reinterpret_cast <byte *> (v2), lo, hi);
}

template <class T>
void NUMvector_copyElements (T* vfrom, T* vto, integer lo, integer hi) {
	NUMvector_copyElements_generic (sizeof (T), reinterpret_cast <byte *> (vfrom), reinterpret_cast <byte *> (vto), lo, hi);
}

template <class T>
void NUMvector_append (T** v, integer lo, integer *hi) {
	NUMvector_append_generic (sizeof (T), reinterpret_cast <byte **> (v), lo, hi);
}

template <class T>
void NUMvector_insert (T** v, integer lo, integer *hi, integer position) {
	NUMvector_insert_generic (sizeof (T), reinterpret_cast <byte **> (v), lo, hi, position);
}

template <class T>
class autoNUMvector {
	T* d_ptr;
	integer d_from;
public:
	autoNUMvector<T> (integer from, integer to) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, true);
	}
	autoNUMvector<T> (integer from, integer to, bool zero) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, zero);
	}
	autoNUMvector (T *ptr, integer from) : d_ptr (ptr), d_from (from) {
	}
	autoNUMvector () : d_ptr (nullptr), d_from (1) {
	}
	~autoNUMvector<T> () {
		if (d_ptr) NUMvector_free (d_ptr, d_from);
	}
	T& operator[] (integer i) {
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
	void reset (integer from, integer to) {
		if (d_ptr) {
			NUMvector_free (d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, true);
	}
	void reset (integer from, integer to, bool zero) {
		if (d_ptr) {
			NUMvector_free (d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, zero);
	}
};

template <class T>
T** NUMmatrix (integer row1, integer row2, integer col1, integer col2) {
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2, true));
	return result;
}

template <class T>
T** NUMmatrix (integer row1, integer row2, integer col1, integer col2, bool zero) {
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2, zero));
	return result;
}

template <class T>
void NUMmatrix_free (T** ptr, integer row1, integer col1) noexcept {
	NUMmatrix_free_ (sizeof (T), reinterpret_cast <byte **> (ptr), row1, col1);
}

template <class T>
T** NUMmatrix_copy (T** ptr, integer row1, integer row2, integer col1, integer col2) {
	#if 1
	T** result = static_cast <T**> (NUMmatrix_copy (sizeof (T), ptr, row1, row2, col1, col2));
	#else
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2));
	for (integer irow = row1; irow <= row2; irow ++)
		for (integer icol = col1; icol <= col2; icol ++)
			result [irow] [icol] = ptr [irow] [icol];
	#endif
	return result;
}

template <class T>
bool NUMmatrix_equal (T** m1, T** m2, integer row1, integer row2, integer col1, integer col2) {
	return NUMmatrix_equal (sizeof (T), m1, m2, row1, row2, col1, col2);
}

template <class T>
void NUMmatrix_copyElements (T** mfrom, T** mto, integer row1, integer row2, integer col1, integer col2) {
	NUMmatrix_copyElements_ (sizeof (T), reinterpret_cast <char **> (mfrom), reinterpret_cast <char **> (mto), row1, row2, col1, col2);
}

template <class T>
class autoNUMmatrix {
	T** d_ptr;
	integer d_row1, d_col1;
public:
	autoNUMmatrix (integer row1, integer row2, integer col1, integer col2) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	autoNUMmatrix (integer row1, integer row2, integer col1, integer col2, bool zero) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
	autoNUMmatrix (T **ptr, integer row1, integer col1) : d_ptr (ptr), d_row1 (row1), d_col1 (col1) {
	}
	autoNUMmatrix () : d_ptr (nullptr), d_row1 (0), d_col1 (0) {
	}
	~autoNUMmatrix () {
		if (d_ptr) NUMmatrix_free_ (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
	}
	T*& operator[] (integer row) {
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
	void reset (integer row1, integer row2, integer col1, integer col2) {
		if (d_ptr) {
			NUMmatrix_free_ (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	void reset (integer row1, integer row2, integer col1, integer col2, bool zero) {
		if (d_ptr) {
			NUMmatrix_free_ (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
};

template <typename T>
class _stringvector {
public:
	T** at;
	integer size;
	T* & operator[] (integer i) {
		return our at [i];
	}
};
typedef _stringvector <char32> string32vector;
typedef _stringvector <char> string8vector;

template <class T>
class _autostringvector {
	_autostring <T> * _ptr;
public:
	integer size;
	_autostringvector () {
		our _ptr = nullptr;
		our size = 0;
	}
	_autostringvector<T> (integer initialSize) {
		our _ptr = NUMvector <_autostring <T>> (1, initialSize, true);
		our size = initialSize;
	}
	_autostringvector (const _autostringvector &) = delete;
	_autostringvector (_autostringvector&& other) {
		our _ptr = other. _ptr;
		our size = other. size;
		other. _ptr = nullptr;
		other. size = 0;
	}
	_autostringvector& operator= (const _autostringvector &) = delete;   // disable copy assignment
	_autostringvector& operator= (_autostringvector&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our reset ();
			our _ptr = other. _ptr;
			our size = other. size;
			other. _ptr = nullptr;
			other. size = 0;
		}
		return *this;
	}
	~ _autostringvector<T> () {
		our reset ();
	}
	explicit operator bool () const { return !! our _ptr; }
	_autostring <T> & operator[] (integer i) {
		return our _ptr [i];
	}
	_stringvector<T> get () const {
		return _stringvector<T> { (T**) our _ptr, our size };
	}
	T** peek2 () const {
		return (T**) our _ptr;
	}
	_autostring <T> * transfer () {
		_autostring <T> * tmp = our _ptr;
		our _ptr = nullptr;   // make the pointer non-automatic again
		our size = 0;
		return tmp;
	}
	T** transfer2 () {
		T** tmp = (T**) our _ptr;
		our _ptr = nullptr;   // make the pointer non-automatic again
		our size = 0;
		return tmp;
	}
	void reset () {
		if (our _ptr) {
			for (integer i = 1; i <= our size; i ++) {
				our _ptr [i]. reset ();
			}
			NUMvector_free (our _ptr, 1);
			our _ptr = nullptr;
			our size = 0;
		}
	}
	void copyFrom (_autostringvector& other) {
		our reset ();
		our _ptr = NUMvector <_autostring <T>> (1, other. size, true);
		our size = other. size;
		for (integer i = 1; i <= our size; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
	void copyElementsFrom (_autostringvector& other) {
		Melder_assert (other. size == our size);
		for (integer i = 1; i <= our size; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
	void copyElementsFrom_upTo (_autostringvector& other, integer to) {
		Melder_assert (to <= other. size && to <= our size);
		for (integer i = 1; i <= to; i ++) {
			our _ptr [i] = Melder_dup (other. _ptr [i].get());
		}
	}
};

typedef _autostringvector <char32> autostring32vector;
typedef _autostringvector <char> autostring8vector;

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

enum class kTensorInitializationType { RAW = 0, ZERO = 1 };

class numvec {
public:
	double *at;
	integer size;
public:
	numvec () = default;   // for use in a union
	numvec (double *givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	numvec (const numvec& other) = default;
	numvec (const autonumvec& other) = delete;
	numvec& operator= (const numvec&) = default;
	numvec& operator= (const autonumvec&) = delete;
	double& operator[] (integer i) {
		return our at [i];
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autonumvec)
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our size = 0;
	}
protected:
	void _initAt (integer givenSize, kTensorInitializationType initializationType);
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
	autonumvec (integer givenSize, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		our _initAt (givenSize, initializationType);
		our size = givenSize;
	}
	~autonumvec () {   // destroy the payload (if any)
		if (our at) our _freeAt ();
	}
	numvec get () const { return { our at, our size }; }   // let the public use the payload (they may change the values of the elements but not the at-pointer or the size)
	void adoptFromAmbiguousOwner (numvec given) {   // buy the payload from a non-autonumvec
		our reset();
		our at = given.at;
		our size = given.size;
	}
	numvec releaseToAmbiguousOwner () {   // sell the payload to a non-autonumvec
		double *oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return { oldAt, our size };
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
	integer nrow, ncol;
public:
	nummat () = default;   // for use in a union
	nummat (double **givenAt, integer givenNrow, integer givenNcol): at (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	nummat (const nummat& other) = default;
	nummat (const autonummat& other) = delete;
	nummat& operator= (const nummat&) = default;
	nummat& operator= (const autonummat&) = delete;
	double *& operator[] (integer i) {
		return our at [i];
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autonummat)
		if (our at) {
			our _freeAt ();
			our at = nullptr;
		}
		our nrow = 0;
		our ncol = 0;
	}
protected:
	void _initAt (integer givenNrow, integer givenNcol, kTensorInitializationType initializationType);
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
	autonummat (integer givenNrow, integer givenNcol, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		our _initAt (givenNrow, givenNcol, initializationType);
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	~autonummat () {   // destroy the payload (if any)
		if (our at) our _freeAt ();
	}
	nummat get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	void adoptFromAmbiguousOwner (nummat given) {   // buy the payload from a non-autonummat
		our reset();
		our at = given.at;
		our nrow = given.nrow;
		our ncol = given.ncol;
	}
	nummat releaseToAmbiguousOwner () {   // sell the payload to a non-autonummat
		double **oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return { oldAt, our nrow, our ncol };
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

/********** Sorting (NUMsort.cpp) **********/

void NUMsort_d (integer n, double ra []);   // heap sort
void NUMsort_i (integer n, int ra []);
void NUMsort_integer (integer n, integer ra []);
void NUMsort_str (string32vector a);
void NUMsort_p (integer n, void *a [], int (*compare) (const void *, const void *));

double NUMquantile (integer n, double a [], double factor);
/*
	An estimate of the quantile 'factor' (between 0 and 1) of the distribution
	from which the set 'a [1..n]' is a sorted array of random samples.
	For instance, if 'factor' is 0.5, this function returns an estimate of
	the median of the distribution underlying the sorted set a [].
	If your array has not been sorted, first sort it with NUMsort (n, a).
*/

#pragma mark - ARGUMENTS

conststring32 Melder_numvec (numvec value);
conststring32 Melder_nummat (nummat value);
typedef class structThing *Thing;   // forward declaration
conststring32 Thing_messageName (Thing me);
struct MelderArg {
	conststring32 _arg;
	/*
		The types of arguments that never involve memory allocation:
	*/
	MelderArg (conststring32             arg) : _arg (arg) { }
	MelderArg (const double              arg) : _arg (Melder_double          (arg)) { }
	MelderArg (const          long long  arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned long long  arg) : _arg (Melder_integer         ((int64) arg)) { }
	MelderArg (const          long       arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned long       arg) : _arg (Melder_integer         ((int64) arg)) { }   // ignore ULL above 2^63
	MelderArg (const          int        arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned int        arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const          short      arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const unsigned short      arg) : _arg (Melder_integer         (arg)) { }
	MelderArg (const dcomplex            arg) : _arg (Melder_dcomplex        (arg)) { }
	MelderArg (const char32              arg) : _arg (Melder_character       (arg)) { }
	/*
		The types of arguments that sometimes involve memory allocation:
	*/
	MelderArg (numvec                    arg) : _arg (Melder_numvec          (arg)) { }
	MelderArg (nummat                    arg) : _arg (Melder_nummat          (arg)) { }
	MelderArg (Thing                     arg) : _arg (Thing_messageName      (arg)) { }
	MelderArg (MelderFile                arg) : _arg (MelderFile_messageName (arg)) { }
	/*
		There could be more types of arguments, but those are rare;
		you have to use explicit conversion to one of the types above.
		For instance, you can write a char* string by using Melder_peek8to32()
		(which sometimes involves memory allocation),
		and you can write a void* by using Melder_pointer()
		(which never involves memory allocation).
	*/
};

inline static integer MelderArg__length (const MelderArg& arg) {
	return arg._arg ? str32len (arg._arg) : 0;
}
template <typename... Args>
integer MelderArg__length (const MelderArg& first, Args... rest) {
	integer length = MelderArg__length (first);
	length += MelderArg__length (rest...);
	return length;
}

void Melder_tracingToFile (MelderFile file);
void Melder_setTracing (bool tracing);
extern bool Melder_isTracing;

namespace MelderTrace {
	extern structMelderFile _file;
	FILE * _open (conststring8 sourceCodeFileName, int lineNumber, conststring8 functionName);
	void _close (FILE *f);
	conststring8  _peek32to8  (conststring32 string);
	conststring16 _peek32to16 (conststring32 string);
};

inline static void _recursiveTemplate_Melder_trace (FILE *f, const MelderArg& arg) {
	if (arg._arg)
		fprintf (f, "%s", MelderTrace::_peek32to8 (arg. _arg));
}
template <typename... Args>
void _recursiveTemplate_Melder_trace (FILE *f, const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_trace (f, first);
	_recursiveTemplate_Melder_trace (f, rest...);
}

template <typename... Args>
void Melder_trace (conststring8 sourceCodeFileName, int lineNumber, conststring8 functionName, const MelderArg& first, Args... rest) {
	if (! Melder_isTracing || MelderFile_isNull (& MelderTrace::_file))
		return;
	FILE *f = MelderTrace::_open (sourceCodeFileName, lineNumber, functionName);
	_recursiveTemplate_Melder_trace (f, first, rest...);
	MelderTrace::_close (f);
}

#ifdef NDEBUG
	#define trace(x)   ((void) 0)
#else
	#define trace(...)   (! Melder_isTracing ? (void) 0 : Melder_trace (__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#endif

/* These will be the future replacements for Melder_fopen, as soon as we rid of text files: */
MelderFile MelderFile_open (MelderFile file);
MelderFile MelderFile_append (MelderFile file);
MelderFile MelderFile_create (MelderFile file);
void * MelderFile_read (MelderFile file, integer nbytes);
char * MelderFile_readLine (MelderFile file);
void MelderFile_writeCharacter (MelderFile file, wchar_t kar);
void MelderFile_writeCharacter (MelderFile file, char32 kar);

void _MelderFile_write (MelderFile file, conststring32 string);

inline static void _recursiveTemplate_MelderFile_write (MelderFile file, const MelderArg& arg) {
	_MelderFile_write (file, arg. _arg);
}
template <typename... Args>
void _recursiveTemplate_MelderFile_write (MelderFile file, const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderFile_write (file, first);
	_recursiveTemplate_MelderFile_write (file, rest...);
}

template <typename... Args>
void MelderFile_write (MelderFile file, const MelderArg& first, Args... rest) {
	if (! file -> filePointer)
		return;
	_recursiveTemplate_MelderFile_write (file, first, rest...);
}

void MelderFile_rewind (MelderFile file);
void MelderFile_seek (MelderFile file, integer position, int direction);
integer MelderFile_tell (MelderFile file);
void MelderFile_close (MelderFile file);
void MelderFile_close_nothrow (MelderFile file);

/* Read and write whole text files. */
autostring32 MelderFile_readText (MelderFile file);
void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding);
void MelderFile_appendText (MelderFile file, conststring32 text);

void Melder_createDirectory (MelderDir parent, conststring32 subdirName, int mode);

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
void MelderString_ncopy (MelderString *me, conststring32 source, int64 n);

inline static void _recursiveTemplate_MelderString_append (MelderString *me, const MelderArg& arg) {
	if (arg._arg) {
		const char32 *newEndOfStringLocation = stp32cpy (& my string [my length], arg._arg);
		my length = newEndOfStringLocation - & my string [0];
	}
}
template <typename... Args>
void _recursiveTemplate_MelderString_append (MelderString *me, const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderString_append (me, first);
	_recursiveTemplate_MelderString_append (me, rest...);
}

template <typename... Args>
void MelderString_append (MelderString *me, const MelderArg& first, Args... rest) {
	integer extraLength = MelderArg__length (first, rest...);
	integer sizeNeeded = my length + extraLength + 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand (me, sizeNeeded);
	_recursiveTemplate_MelderString_append (me, first, rest...);
}

template <typename... Args>
void MelderString_copy (MelderString *me, const MelderArg& first, Args... rest) {
	constexpr int64 FREE_THRESHOLD_BYTES = 10000;
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	integer length = MelderArg__length (first, rest...);
	integer sizeNeeded = length + 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand (me, sizeNeeded);
	my length = 0;
	_recursiveTemplate_MelderString_append (me, first, rest...);
}

void MelderString16_appendCharacter (MelderString16 *me, char32 character);
void MelderString_appendCharacter (MelderString *me, char32 character);
void MelderString_get (MelderString *me, char32 *destination);   // performs no boundary checking
int64 MelderString_allocationCount ();
int64 MelderString_deallocationCount ();
int64 MelderString_allocationSize ();
int64 MelderString_deallocationSize ();

namespace MelderCat {
	constexpr int _k_NUMBER_OF_BUFFERS = 33;
	extern MelderString _buffers [_k_NUMBER_OF_BUFFERS];
	extern int _bufferNumber;
};

template <typename... Args>
conststring32 Melder_cat (Args... args) {
	if (++ MelderCat::_bufferNumber == MelderCat::_k_NUMBER_OF_BUFFERS)
		MelderCat::_bufferNumber = 0;
	MelderString_copy (& MelderCat::_buffers [MelderCat::_bufferNumber], args...);
	return MelderCat::_buffers [MelderCat::_bufferNumber].string;
}

inline static void _recursiveTemplate_Melder_sprint (char32 **inout_pointer, const MelderArg& arg) {
	if (arg._arg) {
		char32 *newEndOfStringLocation = stp32cpy (*inout_pointer, arg._arg);
		*inout_pointer = newEndOfStringLocation;
	}
}
template <typename... Args>
void _recursiveTemplate_Melder_sprint (char32 **inout_pointer, const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_sprint (inout_pointer, first);
	_recursiveTemplate_Melder_sprint (inout_pointer, rest...);
}

template <typename... Args>
void Melder_sprint (mutablestring32 buffer, int64 bufferSize, const MelderArg& first, Args... rest) {
	integer length = MelderArg__length (first, rest...);
	if (length >= bufferSize) {
		for (int64 i = 0; i < bufferSize; i ++)
			buffer [i] = U'?';
		if (bufferSize > 0)
			buffer [bufferSize - 1] = U'\0';
		return;
	}
	char32 *p = & buffer [0];
	_recursiveTemplate_Melder_sprint (& p, first, rest...);
}

/********** NUMBER AND STRING COMPARISON **********/

bool Melder_numberMatchesCriterion (double value, kMelder_number which, double criterion);
bool Melder_stringMatchesCriterion (conststring32 value, kMelder_string which, conststring32 criterion, bool caseSensitive);

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

integer Melder_countTokens (conststring32 string);
char32 *Melder_firstToken (conststring32 string);
char32 *Melder_nextToken ();
char32 ** Melder_getTokens (conststring32 string, integer *n);
void Melder_freeTokens (char32 ***tokens);
integer Melder_searchToken (conststring32 string, char32 **tokens, integer n);

/********** MESSAGING ROUTINES **********/

/**
	Function:
		Sends a message without user interference.
	Behaviour:
		Writes to stderr on Unix, otherwise to a special window.
*/

inline static void _recursiveTemplate_Melder_casual (const MelderArg& arg) {
	Melder_writeToConsole (arg._arg, true);
}
template <typename... Args>
void _recursiveTemplate_Melder_casual (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_casual (first);
	_recursiveTemplate_Melder_casual (rest...);
}

template <typename... Args>
void Melder_casual (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_casual (first, rest...);
	Melder_writeToConsole (U"\n", true);
}

void MelderCasual_memoryUse (integer message = 0);

/*
	Give information to stdout (batch), or to an "Info" window (interactive), or to a diverted string.
*/

namespace MelderInfo {
	using Proc = void (*) (conststring32 message);
	void _defaultProc (conststring32 message);
	extern Proc _p_currentProc;
	extern MelderString _foregroundBuffer, *_p_currentBuffer;
};

void MelderInfo_open ();   // clear the Info window in the background
void MelderInfo_close ();   // drain the background info to the Info window, making sure there is a line break
void MelderInfo_drain ();   // drain the background info to the Info window, without adding any extra line break

inline static void _recursiveTemplate_MelderInfo_write (const MelderArg& arg) {
	Melder_writeToConsole (arg._arg, false);
}
template <typename... Args>
void _recursiveTemplate_MelderInfo_write (const MelderArg& first, Args... rest) {
	_recursiveTemplate_MelderInfo_write (first);
	_recursiveTemplate_MelderInfo_write (rest...);
}

template <typename... Args>
void MelderInfo_write (const MelderArg& first, Args... rest) {
	MelderString_append (MelderInfo::_p_currentBuffer, first, rest...);
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer)
		_recursiveTemplate_MelderInfo_write (first, rest...);
}

template <typename... Args>
void MelderInfo_writeLine (const MelderArg& first, Args... rest) {
	MelderString_append (MelderInfo::_p_currentBuffer, first, rest...);
	MelderString_appendCharacter (MelderInfo::_p_currentBuffer, U'\n');
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer) {
		_recursiveTemplate_MelderInfo_write (first, rest...);
		Melder_writeToConsole (U"\n", false);
	}
}

template <typename... Args>
void Melder_information (const MelderArg& first, Args... rest) {
	MelderString_copy (MelderInfo::_p_currentBuffer, first, rest...);
	if (MelderInfo::_p_currentProc == & MelderInfo::_defaultProc && MelderInfo::_p_currentBuffer == & MelderInfo::_foregroundBuffer)
		_recursiveTemplate_MelderInfo_write (first, rest...);
	MelderInfo_close ();
}

void Melder_informationReal (double value, conststring32 units);   // %.17g or --undefined--; units may be null

void Melder_divertInfo (MelderString *p_buffer);   // nullptr = back to normal

class autoMelderDivertInfo {
	public:
		autoMelderDivertInfo (MelderString *p_buffer) { Melder_divertInfo (p_buffer); }
		~autoMelderDivertInfo () { Melder_divertInfo (nullptr); }
};

void Melder_clearInfo ();   // clear the Info window
conststring32 Melder_getInfo ();
void Melder_help (conststring32 query);

void Melder_search ();
	
void Melder_beep ();

extern int Melder_debug;

/* The following trick uses Melder_debug only because it is the only plain variable known to exist at the moment. */
#define Melder_offsetof(klas,member) (int) ((char *) & ((klas) & Melder_debug) -> member - (char *) & Melder_debug)

/********** ERROR **********/

class MelderError {
public:
	static void _append (conststring32 message);
};

void Melder_appendError_noLine (const MelderArg& arg1);

/**
	The usual error reporting function is Melder_throw. However,
	you may sometimes want to prepend other messages before Melder_throw,
	without jumping away from the error-generating location.
	In such a special case you can use Melder_appendError.

	Melder_appendError() has to be followed by one of these:
	- Melder_throw() (or just `throw`) to prepend the error to a normal exception;
	- Melder_flushError() to show the error in the GUI
	  (this is where a trail of Melder_throw will usually end up as well);
	- Melder_clearError() to ignore the error.
	
	If you don't do this, the error will linger in your error buffer until
	the next, probably unrelated, error is generated,
	and your prepended error text will be shown to the user out of context,
	which is wrong.
*/

inline static void _recursiveTemplate_Melder_appendError (const MelderArg& arg) {
	MelderError::_append (arg._arg);
}
template <typename... Args>
void _recursiveTemplate_Melder_appendError (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_appendError (first);
	_recursiveTemplate_Melder_appendError (rest...);
}

template <typename... Args>
void Melder_appendError (const MelderArg& first, Args... rest) {
	_recursiveTemplate_Melder_appendError (first, rest...);
	MelderError::_append (U"\n");
}

#define Melder_throw(...)  do { Melder_appendError (__VA_ARGS__); throw MelderError (); } while (false)
#define Melder_require(condition, ...)  do { if (! (condition)) Melder_throw (__VA_ARGS__); } while (false)

void Melder_flushError ();

template <typename... Args>
void Melder_flushError (const MelderArg& first, Args... rest) {
	Melder_appendError (first, rest...);
	Melder_flushError ();
}
	/* Send all deferred error messages to stderr (batch) or to an "Error" dialog, */
	/* including, if there are arguments, the error message generated by this routine. */

bool Melder_hasError ();
bool Melder_hasError (conststring32 partialError);
	/* Returns 1 if there is an error message in store, otherwise 0. */

void Melder_clearError ();
	/* Cancel all stored error messages. */

conststring32 Melder_getError ();
	/* Returns the error string. Mainly used with str32str. */

/********** WARNING: give warning to stderr (batch) or to a "Warning" dialog **********/

namespace MelderWarning {
	extern int _depth;
	extern MelderString _buffer;
	using Proc = void (*) (conststring32 message);
	void _defaultProc (conststring32 message);
	extern Proc _p_currentProc;
}

template <typename... Args>
void Melder_warning (const MelderArg& first, Args... rest);

template <typename... Args>
void Melder_warning (const MelderArg& first, Args... rest) {
	if (MelderWarning::_depth < 0)
		return;
	MelderString_copy (& MelderWarning::_buffer, first, rest...);
	MelderWarning::_p_currentProc (MelderWarning::_buffer.string);
}

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
void Melder_fatal (const MelderArg&,
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U"",
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U"",
	const MelderArg& = U"", const MelderArg& = U"", const MelderArg& = U""
);

#pragma mark - PROGRESS

namespace MelderProgress {
	extern int _depth;
	using ProgressProc = void (*) (double progress, conststring32 message);
	using MonitorProc = void * (*) (double progress, conststring32 message);
	extern ProgressProc _p_progressProc;
	extern MonitorProc _p_monitorProc;
	void _doProgress (double progress, conststring32 message);
	void * _doMonitor (double progress, conststring32 message);
	extern MelderString _buffer;
}

void Melder_progressOff ();
void Melder_progressOn ();

inline static void Melder_progress (double progress) {
	MelderProgress::_doProgress (progress, U"");
}
template <typename... Args>
void Melder_progress (double progress, const MelderArg& first, Args... rest) {
	MelderString_copy (& MelderProgress::_buffer, first, rest...);
	MelderProgress::_doProgress (progress, MelderProgress::_buffer.string);
}
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
	autoMelderProgress (conststring32 message) {
		Melder_progress (0.0, message);
	}
	~autoMelderProgress () {
		Melder_progress (1.0);
	}
};

inline static void * Melder_monitor (double progress) {
	return MelderProgress::_doMonitor (progress, U"");
}
template <typename... Args>
void * Melder_monitor (double progress, const MelderArg& first, Args... rest) {
	MelderString_copy (& MelderProgress::_buffer, first, rest...);
	return MelderProgress::_doMonitor (progress, MelderProgress::_buffer.string);
}
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
	autoMelderMonitor (conststring32 message) {
		_graphics = (Graphics) Melder_monitor (0.0, message);
	}
	~autoMelderMonitor () {
		Melder_monitor (1.0);
	}
	Graphics graphics () { return _graphics; }
};

#pragma mark - REAL TO INTEGER CONVERSION

inline static double Melder_roundDown (double x) {
	return floor (x);
}

inline static integer Melder_iroundDown (double x) {
	double xround = Melder_roundDown (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,   // this formulation handles NaN correctly
		U"When rounding down the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_ifloor  Melder_iroundDown

inline static double Melder_roundUp (double x) {
	return ceil (x);
}

inline static integer Melder_iroundUp (double x) {
	double xround = Melder_roundUp (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding up the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_iceiling  Melder_iroundUp

inline static double Melder_roundTowardsZero (double x) {
	return x >= 0.0 ? Melder_roundDown (x) : Melder_roundUp (x);
}

inline static integer Melder_iroundTowardsZero (double x) {
	Melder_require (x >= (double) INTEGER_MIN && x <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U" towards zero, the result cannot be represented in an integer.");
	return (integer) x;
}

inline static double Melder_roundAwayFromZero (double x) {
	return x >= 0.0 ? Melder_roundUp (x) : Melder_roundDown (x);
}

inline static integer Melder_iroundAwayFromZero (double x) {
	double xround = Melder_roundAwayFromZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U" away from zero, the result cannot be represented in an integer.");
	return (integer) xround;
}

inline static double Melder_round_tieUp (double x) {
	return Melder_roundDown (x + 0.5);
}

inline static integer Melder_iround_tieUp (double x) {
	double xround = Melder_round_tieUp (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_iround  Melder_iround_tieUp

inline static double Melder_round_tieDown (double x) {
	return Melder_roundUp (x - 0.5);
}

inline static integer Melder_iround_tieDown (double x) {
	double xround = Melder_round_tieDown (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

inline static double Melder_round_tieTowardsZero (double x) {
	return x >= 0.0 ? Melder_round_tieDown (x) : Melder_round_tieUp (x);
}

inline static integer Melder_iround_tieTowardsZero (double x) {
	double xround = Melder_round_tieTowardsZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

inline static double Melder_round_tieAwayFromZero (double x) {
	return x >= 0.0 ? Melder_round_tieUp (x) : Melder_round_tieDown (x);
}

inline static integer Melder_iround_tieAwayFromZero (double x) {
	double xround = Melder_round_tieAwayFromZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

#pragma mark - RECORD AND PLAY FUNCTIONS

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
integer MelderAudio_getOutputBestSampleRate (integer fsamp);

extern bool MelderAudio_isPlaying;
void MelderAudio_play16 (int16 *buffer, integer sampleRate, integer numberOfSamples, int numberOfChannels,
	bool (*playCallback) (void *playClosure, integer numberOfSamplesPlayed),   // return true to continue, false to stop
	void *playClosure);
bool MelderAudio_stopPlaying (bool isExplicit);   // returns true if sound was playing
#define MelderAudio_IMPLICIT  false
#define MelderAudio_EXPLICIT  true
integer MelderAudio_getSamplesPlayed ();
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
conststring32 Melder_audioFileTypeString (int audioFileType);   // "AIFF", "AIFC", "WAV", "NeXT/Sun", "NIST", "FLAC", "MP3"
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
void MelderFile_writeAudioFileHeader (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFileTrailer (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFile (MelderFile file, int audioFileType, const short *buffer, integer sampleRate, integer numberOfSamples, int numberOfChannels, int numberOfBitsPerSamplePoint);

int MelderFile_checkSoundFile (MelderFile file, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples);
/* Returns information about a just opened audio file.
 * The return value is the audio file type, or 0 if it is not a sound file or in case of error.
 * The data start at 'startOfData' bytes from the start of the file.
 */
int Melder_bytesPerSamplePoint (int encoding);
void Melder_readAudioToFloat (FILE *f, integer numberOfChannels, int encoding, double **buffer, integer numberOfSamples);
/* Reads channels into buffer [ichannel], which are base-1.
 */
void Melder_readAudioToShort (FILE *f, integer numberOfChannels, int encoding, short *buffer, integer numberOfSamples);
/* If stereo, buffer will contain alternating left and right values.
 * Buffer is base-0.
 */
void MelderFile_writeFloatToAudio (MelderFile file, integer numberOfChannels, int encoding, double **buffer, integer numberOfSamples, int warnIfClipped);
void MelderFile_writeShortToAudio (MelderFile file, integer numberOfChannels, int encoding, const short *buffer, integer numberOfSamples);

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
	#include <memory>
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
double Melder_clock ();   // seconds since 1969

struct autoMelderProgressOff {
	autoMelderProgressOff () { Melder_progressOff (); }
	~autoMelderProgressOff () { Melder_progressOn (); }
};

struct autoMelderString : MelderString {
	autoMelderString () { length = 0; bufferSize = 0; string = nullptr; }
	~autoMelderString () { MelderString_free (this); }
};

class autofile {
	FILE *ptr;
public:
	autofile (FILE *f) : ptr (f) {
	}
	autofile () : ptr (nullptr) {
	}
	~autofile () {
		if (ptr)
			fclose (ptr);   // no error checking, because this is a destructor, only called after a throw, because otherwise you'd use f.close(file)
	}
	operator FILE * () {
		return ptr;
	}
	void reset (FILE *f) {
		if (ptr)
			fclose (ptr);   // BUG: not a normal closure
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
		if (_file)
			MelderFile_close_nothrow (_file);
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
	integer numberOfTokens;
public:
	autoMelderTokens () {
		tokens = nullptr;
	}
	autoMelderTokens (conststring32 string) {
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
	~autoMelderTokens () {
		if (tokens) {
			for (integer itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
	}
	char32*& operator[] (integer i) {
		return tokens [i];
	}
	char32 ** peek () const {
		return tokens;
	}
	integer count () const {
		return numberOfTokens;
	}
	void reset (conststring32 string) {
		if (tokens) {
			for (integer itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
};

class autoMelderAudioSaveMaximumAsynchronicity {
	bool _disowned;
	enum kMelder_asynchronicityLevel _savedAsynchronicity;
public:
	autoMelderAudioSaveMaximumAsynchronicity () {
		our _savedAsynchronicity = MelderAudio_getOutputMaximumAsynchronicity ();
		trace (U"value was ", (int) our _savedAsynchronicity);
		our _disowned = false;
	}
	~autoMelderAudioSaveMaximumAsynchronicity () {
		MelderAudio_setOutputMaximumAsynchronicity (our _savedAsynchronicity);
		trace (U"value set to ", (int) our _savedAsynchronicity);
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
