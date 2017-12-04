/* NUMstring.cpp
 *
 * Copyright (C) 2012-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20121005 First version
*/

#include <ctype.h>
#include <wctype.h>
#include "Interpreter.h"
#include "NUM2.h"


int NUMstring_containsPrintableCharacter (const char32 *s) {
	integer len;
	if (s == NULL || ( (len = str32len (s)) == 0)) {
		return 0;
	}
	for (integer i = 0; i < len; i ++) {
		if (isgraph ((int) s [i])) {
			return 1;
		}
	}
	return 0;
}

void NUMstring_chopWhiteSpaceAtExtremes_inplace (char32 *string) {
	int64 start = 0;
	while (iswspace ((int) string [start]) != 0) {
		start ++;
	}
	int64 end = str32len (string);
	while (end != start && iswspace((int) string [end - 1]) != 0) {
		end --;
	}
	int64 n = end - start;
	memmove (string, string + start, (size_t) (n * (int64) sizeof (char32)));
	string [n] = 0;
}

real *NUMstring_to_numbers (const char32 *s, integer *p_numbers_found) {
	integer numbers_found = Melder_countTokens (s);
	if (numbers_found < 1) {
		Melder_throw (U"Empty string.");
	}
	autoNUMvector <real> numbers (1, numbers_found);
	integer inum = 1;
	for (char32 *token = Melder_firstToken (s); token; token = Melder_nextToken (), inum ++) {
		Interpreter_numericExpression (0, token, & numbers [inum]);
	}
	if (p_numbers_found) {
		*p_numbers_found = numbers_found;
	}
	return numbers.transfer();
}

void NUMstrings_copyElements (char32 **from, char32 **to, integer lo, integer hi) {
	for (integer i = lo; i <= hi; i ++) {
		Melder_free (to [i]);
		if (from [i]) {
			to [i] = Melder_dup (from [i]);
		}
	}
}

void NUMstrings_free (char32 **s, integer lo, integer hi) {
	if (! s) {
		return;
	}
	for (integer i = lo; i <= hi; i ++) {
		Melder_free (s [i]);
	}
	NUMvector_free <char32 *> (s, lo);
}

char32 **NUMstrings_copy (char32 **from, integer lo, integer hi) {
	autoNUMvector <char32 *> to (lo, hi);
	NUMstrings_copyElements (from, to.peek(), lo, hi);
	return to.transfer();
}

static char32 *appendNumberToString (const char32 *s, integer number, int asArray) {
	return Melder_dup (
		asArray == 0 ? Melder_cat (s, number) :
		asArray == 1 ? Melder_cat (s, U"[", number, U"]") :
		Melder_cat (s, U"(", number, U")"));
}

int NUMstrings_setSequentialNumbering (char32 **s, integer lo, integer hi, const char32 *pre, integer number, integer increment, int asArray) {
	for (integer i = lo; i <= hi; i ++, number += increment) {
		char32 *newc = appendNumberToString (pre, number, asArray);
		if (newc == NULL) {
			return 0;
		}
		Melder_free (s [i]);
		s [i] = newc;
	}
	return 1;
}

#define HIGHBYTE(x) ((unsigned char) ((x) & 0xFF))
#define LOWBYTE(x)  ((unsigned char) ((x) >> 8 & 0xFF))

/* a+b=c in radix 256 */
void NUMstring_add (unsigned char *a, unsigned char *b, unsigned char *c, integer n);
void NUMstring_add (unsigned char *a, unsigned char *b, unsigned char *c, integer n) {
	int j;
	unsigned short reg = 0;

	for (j = n; j > 1; j --) {
		reg = a [j] + b [j] + HIGHBYTE (reg);
		c [j + 1] = LOWBYTE (reg);
	}
}

char32 *strstr_regexp (const char32 *string, const char32 *search_regexp) {
	char32 *charp = nullptr;
	regexp *compiled_regexp = CompileRE_throwable (search_regexp, 0);

	if (ExecRE (compiled_regexp, NULL, string, NULL, 0, '\0', '\0', NULL, NULL, NULL)) {
		charp = compiled_regexp -> startp[0];
	}

	free (compiled_regexp);
	return charp;
}

char32 *str_replace_literal (const char32 *string, const char32 *search, const char32 *replace,
                              integer maximumNumberOfReplaces, integer *nmatches) {
	if (string == 0 || search == 0 || replace == 0) {
		return NULL;
	}

	int len_string = str32len (string);
	if (len_string == 0) {
		maximumNumberOfReplaces = 1;
	}
	int len_search = str32len (search);
	if (len_search == 0) {
		maximumNumberOfReplaces = 1;
	}

	/*
		To allocate memory for 'result' only once, we have to know how many
		matches will occur.
	*/

	const char32 *pos = string; //current position / start of current match
	*nmatches = 0;
	if (maximumNumberOfReplaces <= 0) {
		maximumNumberOfReplaces = LONG_MAX;
	}

	if (len_search == 0) { /* Search is empty string... */
		if (len_string == 0) {
			*nmatches = 1;    /* ...only matches empty string */
		}
	} else {
		if (len_string != 0) { /* Because empty string always matches */
			while ( (pos = str32str (pos, search)) && *nmatches < maximumNumberOfReplaces) {
				pos += len_search;
				(*nmatches) ++;
			}
		}
	}

	integer len_replace = str32len (replace);
	integer len_result = len_string + *nmatches * (len_replace - len_search);
	char32 *result = Melder_malloc (char32, (len_result + 1) * (integer) sizeof (char32));
	result[len_result] = '\0';

	const char32 *posp = pos = string;
	integer nchar = 0, result_nchar = 0;
	for (integer i = 1; i <= *nmatches; i ++) {
		pos = str32str (pos, search);

		/*
			Copy gap between end of previous match and start of current.
		*/

		nchar = (pos - posp);
		if (nchar > 0) {
			str32ncpy (result + result_nchar, posp, nchar);
			result_nchar += nchar;
		}

		/*
			Insert the replace string in result.
		*/

		str32ncpy (result + result_nchar, replace, len_replace);
		result_nchar += len_replace;

		/*
			Next search starts after the match.
		*/

		pos += len_search;
		posp = pos;
	}

	/*
		Copy gap between end of match and end of string.
	*/

	pos = string + len_string;
	nchar = pos - posp;
	if (nchar > 0) {
		str32ncpy (result + result_nchar, posp, nchar);
	}
	return result;
}

char32 *str_replace_regexp (const char32 *string, regexp *compiledSearchRE, const char32 *replaceRE, integer maximumNumberOfReplaces, integer *nmatches) {
	int buf_nchar = 0;				/* # characters in 'buf' */
	int gap_copied = 0;
	int nchar, reverse = 0;
	int errorType;
	char32 prev_char = U'\0';
	const char32 *pos; 	/* current position in 'string' / start of current match */
	const char32 *posp; /* end of previous match */
	autostring32 buf;

	*nmatches = 0;
	if (string == 0 || compiledSearchRE == 0 || replaceRE == 0) {
		return 0;
	}

	int string_length = str32len (string);
	//int replace_length = str32len (replaceRE);
	if (string_length == 0) {
		maximumNumberOfReplaces = 1;
	}

	integer i = maximumNumberOfReplaces > 0 ? 0 : - string_length;

	/*
		We do not know the size of the replaced string in advance,
		therefor, we allocate a replace buffer twice the size of the
		original string. After all replaces have taken place we do a
		final realloc to the then exactly known size.
		If during the replace, the size of the buffer happens to be too
		small (this is signalled by the replaceRE function),
		we double its size and restart the replace.
	*/

	int buf_size = 2 * string_length;
	buf_size = buf_size < 100 ? 100 : buf_size;
	buf.resize (buf_size);

	pos = posp = string;
	while (ExecRE (compiledSearchRE, 0, pos, 0, reverse, prev_char, '\0', 0, 0, 0) && i++ < maximumNumberOfReplaces) {
		/*
			Copy gap between the end of the previous match and the start
			of the current match.
			Check buffer overflow. pos == posp ? '\0' : pos[-1],
		*/

		pos = compiledSearchRE -> startp[0];
		nchar = pos - posp;
		if (nchar > 0 && ! gap_copied) {
			if (buf_nchar + nchar + 1 > buf_size) {
				buf_size *= 2;
				buf.resize (buf_size);
			}
			str32ncpy (buf.peek() + buf_nchar, posp, nchar);
			buf_nchar += nchar;
		}

		gap_copied = 1;

		/*
			Do the substitution. We can only check afterwards for buffer
			overflow. SubstituteRE puts null byte at last replaced position and signals when overflow.
		*/

		if ( (SubstituteRE (compiledSearchRE, replaceRE, buf.peek() + buf_nchar, buf_size - buf_nchar, &errorType)) == false) {
			if (errorType == 1) { // not enough memory
				buf_size *= 2;
				buf.resize (buf_size);
				Melder_clearError ();
				i--; // retry
				continue;
			}
			Melder_throw (U"Error during substitution.");
		}

		// Buffer is not full, get number of characters added;

		nchar = str32len (buf.peek() + buf_nchar);
		buf_nchar += nchar;

		// Update next start position in search string.

		posp = pos;
		pos = (char32 *) compiledSearchRE -> endp[0];
		if (pos != posp) {
			prev_char = pos[-1];
		}
		gap_copied = 0;
		posp = pos; //pb 20080121
		(*nmatches) ++;
		// at end of string?
		// we need this because .* matches at end of a string
		if (pos - string == string_length) {
			break;
		}
	}

	// Copy last part of string to destination string

	nchar = (string + string_length) - pos;
	buf_size = buf_nchar + nchar + 1;
	buf.resize (buf_size);

	str32ncpy (buf.peek() + buf_nchar, pos, nchar);
	buf[buf_size - 1] = '\0';
	return buf.transfer();
}

static char32 **strs_replace_literal (char32 **from, integer lo, integer hi, const char32 *search,
	const char32 *replace, int maximumNumberOfReplaces, integer *p_nmatches, integer *p_nstringmatches) {
	if (search == NULL || replace == NULL) {
		return NULL;
	}
	autostring32vector result (lo, hi);

	integer nmatches_sub = 0, nmatches = 0, nstringmatches = 0;
	for (integer i = lo; i <= hi; i ++) {
		const char32 *string = ( from [i] ? from [i] : U"" );   // treat null as an empty string

		result [i] = str_replace_literal (string, search, replace, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (p_nmatches) {
		*p_nmatches = nmatches;
	}
	if (p_nstringmatches) {
		*p_nstringmatches = nstringmatches;
	}
	return result.transfer();
}

static char32 **strs_replace_regexp (char32 **from, integer lo, integer hi, const char32 *searchRE,
	const char32 *replaceRE, int maximumNumberOfReplaces, integer *p_nmatches, integer *p_nstringmatches) {
	if (searchRE == NULL || replaceRE == NULL) {
		return NULL;
	}
	autostring32vector result;

	integer nmatches_sub = 0;

	regexp *compiledRE = CompileRE_throwable (searchRE, 0);

	result.reset (lo, hi);

	integer nmatches = 0, nstringmatches = 0;
	for (integer i = lo; i <= hi; i ++) {
		const char32 *string = ( from [i] ? from [i] : U"" );   // treat null as an empty string

		result [i] = str_replace_regexp (string, compiledRE, replaceRE, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (p_nmatches) {
		*p_nmatches = nmatches;
	}
	if (p_nstringmatches) {
		*p_nstringmatches = nstringmatches;
	}
	return result.transfer();
}

char32 **strs_replace (char32 **from, integer lo, integer hi, const char32 *search, const char32 *replace, int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp) {
	return use_regexp ? strs_replace_regexp (from, lo, hi, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches) :
		strs_replace_literal (from, lo, hi, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches);
}


/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static integer *getElementsOfRanges (const char32 *ranges, integer maximumElement, integer *numberOfElements, const char32 *elementType) {
	/*
		Count the elements.
	 */
	integer previousElement = 0;
	*numberOfElements = 0;
	const char32 *p = & ranges [0];
	for (;;) {
		while (*p == U' ' || *p == U'\t') p ++;
		if (*p == U'\0') break;
		if (isdigit ((int) *p)) {
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0, U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement, U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			*numberOfElements += 1;
			previousElement = currentElement;
			do { p ++; } while (isdigit ((int) *p));
		} else if (*p == ':') {
			Melder_require (previousElement != 0, U"Cannot start range with colon.");
			
			do { p ++; } while (*p == U' ' || *p == U'\t');
			Melder_require (*p != U'\0', U"Cannot end range with colon.");
			Melder_require (isdigit ((int) *p), U"End of range should be a positive whole number.");
			
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0, U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement, U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			if (currentElement > previousElement) {
				*numberOfElements += currentElement - previousElement;
			} else {
				*numberOfElements += previousElement - currentElement;
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit ((int) *p));
		} else {
			Melder_throw (U"Start of range should be a positive whole number.");
		}
	}
	/*
		Create room for the elements.
	 */
	if (*numberOfElements == 0) {
		return nullptr;
	}
	autoNUMvector <integer> elements (1, *numberOfElements);
	
	/*
		Store the elements.
	 */
	
	previousElement = 0;
	*numberOfElements = 0;
	p = & ranges [0];
	for (;;) {
		while (*p == U' ' || *p == U'\t') p ++;
		if (*p == U'\0') break;
		if (isdigit ((int) *p)) {
			integer currentElement = Melder_atoi (p);
			elements [++ *numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (isdigit ((int) *p));
		} else if (*p == U':') {
			do { p ++; } while (*p == U' ' || *p == U'\t');
			integer currentElement = Melder_atoi (p);
			if (currentElement > previousElement) {
				for (integer ielement = previousElement + 1; ielement <= currentElement; ielement ++) {
					elements [++ *numberOfElements] = ielement;
				}
			} else {
				for (integer ielement = previousElement - 1; ielement >= currentElement; ielement --) {
					elements [++ *numberOfElements] = ielement;
				}
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit ((int) *p));
		}
	}
	return elements.transfer();
}

static void NUMlvector_getUniqueNumbers (integer *numbers, integer *numberOfElements, integer *p_numberOfMultiples) {
	Melder_assert (numberOfElements);
	autoNUMvector< integer> sorted (NUMvector_copy <integer> (numbers, 1, *numberOfElements), 1);
	NUMsort_integer (*numberOfElements, sorted.peek());
	integer numberOfMultiples = 0;
	
	numbers [1] = sorted [1];
	integer numberOfUniques = 1;
	for (integer i = 2; i <= *numberOfElements; i ++) {
		if (sorted [i] != sorted [i - 1]) {
			numbers [++numberOfUniques] = sorted [i];
		} else {
			numberOfMultiples ++;
		}
	}
	*numberOfElements = numberOfUniques;
	if (p_numberOfMultiples) {
		*p_numberOfMultiples = numberOfMultiples;
	}
}

integer *NUMstring_getElementsOfRanges (const char32 *ranges, integer maximumElement, integer *numberOfElements, integer *numberOfMultiples, const char32 *elementType, bool sortedUniques) {
	autoNUMvector<integer> elements (getElementsOfRanges (ranges, maximumElement, numberOfElements, elementType), 1);
	if (sortedUniques && *numberOfElements > 0) {
		NUMlvector_getUniqueNumbers (elements.peek(), numberOfElements, numberOfMultiples);
	}
	return elements.transfer();
}

char32 * NUMstring_timeNoDot (double time) {
	static char32 string[100];
	integer seconds = Melder_ifloor (time);
	integer ms = Melder_iround ((time - seconds) * 1000.0);
	Melder_sprint (string,100, U"_", seconds, U"_", ms);
	return string;
}

/* End of file NUMstring.cpp */
