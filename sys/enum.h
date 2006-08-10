#ifndef _enum_h_
#define _enum_h_
/* enum.h
 *
 * Copyright (C) 1994-2002 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 1997/09/25
 * pb 2002/03/07 GPL
 */

/*
Example in Booklet_enums.h:

	enum_begin (Booklet_CAT, NIL)   // Zero value is valid and is "NIL".
		enum (Noun)
		enum (Verb)
		enum (Adjective)
		enum (Adverb)
	enum_end (Booklet_CAT)

	enum_begin (Booklet_COLOUR, _)   // Zero value is invalid.
		enum (Red)
		enum (Green)
		enum (Blue)
	enum_end (Booklet_COLOUR)

In Booklet.h:

	// Start of file.
	#ifndef _Booklet_h_
	#define _Booklet_h_
	...
	#include "Booklet_enums.h"
	...
	#endif
	// End of file.

In Booklet.c:

	#include "Booklet.h"
	#include "enum_c.h"
	#include "Booklet_enums.h"

Usage:
	Declare as:
		enum1 category;   // 0..127
		enum2 adverb;   // 0..32767
	enumi (Booklet_CAT, Adjective) is an integer, equal to 3 (constant expression);
	enumstring (Booklet_CAT, 3) is a string which equals "Adjective" (single dereference);
	enumlength (Booklet_CAT) equals 4 (constant expression);
	enumstrings (Booklet_CAT) is an array of strings [1..4] (global variable);
	enumsearch (Booklet_CAT, "Adjective") is an integer, equal to 3 (string comparisons);
	enumname (enumstrings (Booklet_CAT)) is a string that equals "Booklet_CAT".

	enumstring (Booklet_CAT, 0) equals "nil";
	enumstring (Booklet_CAT, 5) equals NULL;
	enumstring (Booklet_CAT, -1) equals "<eof>";
	enumstring (Booklet_CAT, -2) equals "Booklet_CAT".

Limitations:
	The elements should consist of any sequence of letters, digits, and underscores,
	but not start with a digit (they will be the names of structure members),
	and they should not be called "_length", "_type", "_begin",
	"_end", or "_trailer" (compiler will object to double declaration).
	The name "Booklet_CAT" is used as a typedef.
	The name "enum_Booklet_CAT" refers to a global variable.
*/

typedef struct enum_ANY { int _length; const char *_type, *zero, *_end; int _trailer; } enum_ANY;

typedef signed char enum1;
typedef signed short enum2;
#define enumi(type,element)  (((int) & ((type *) 0) -> element - (int) & ((enum_ANY *) 0) -> zero) / (int) sizeof (char *))

#define enum_type(enumerated)  (((enum_ANY *) enumerated) -> _type)

#define enum_length(enumerated)  (((enum_ANY *) enumerated) -> _length)
#define enumlength(type)  (enumi (type, _end) - 1)

#define enum_string(enumerated,ielement)  (((char **) & ((enum_ANY *) enumerated) -> zero) [ielement])
#define enumstring(type,ielement)  enum_string (& enum_##type, ielement)

int enum_search (void *enumerated, const char *string);
#define enumsearch(type,string)  enum_search (& enum_##type, string)
/*
	If not found:
		1. Queue an error message like:
			"Yellow" is not a value of enumerated type "Booklet_COLOUR".
		2. Return -1 (sort of end-of-file).
	Usage:
		if ((my colour = enumsearch (Booklet_COLOUR, string)) < 0) return 0;
*/

/* The following definitions are for header files. */

#define enum_begin(type,zero)  typedef struct type { int _length; const char *_type, *zero;
#define enum(element)  const char *element;
#define enum_end(type)  const char *_end; int _trailer; } type; extern type enum_##type;

/* Technical detail:
	We would have liked to write the 'enumi' macro as follows:

		#define enumi(type,element)  ((char **) & ((type *) 0) -> element - (char **) & ((type *) 0) -> _type + 1)

	The SPARC compiler computes this correctly, but Think C would give the wrong result.
	Some examples of how Think C 6.0 computes compile-time expressions like these:

		char **a = (char **) 20;
		int result = a - (char **) 0;   // 5 (correct).

		char **b = 0;
		int result = (char **) 20 - b;   // 5 (correct).

		int result = (char **) 20 - (char **) 0;   // 20 (incorrect: SPARC compiler yields 5).
*/
/* End of file enum.h */
#endif

