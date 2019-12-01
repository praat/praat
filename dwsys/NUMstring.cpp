/* NUMstring.cpp
 *
 * Copyright (C) 2012-2018 David Weenink
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

autoVEC VEC_createFromString (conststring32 s) {
	autostring32vector tokens = newSTRVECtokenize (s);
	if (tokens.size < 1)
		Melder_throw (U"Empty string.");
	autoVEC numbers = newVECraw (tokens.size);
	for (integer inum = 1; inum <= tokens.size; inum ++)
		Interpreter_numericExpression (nullptr, tokens [inum].get(), & numbers [inum]);
	return numbers;
}

char32 *strstr_regexp (conststring32 string, conststring32 search_regexp) {
	char32 *charp = nullptr;
	regexp *compiled_regexp = CompileRE_throwable (search_regexp, 0);

	if (ExecRE (compiled_regexp, nullptr, string, nullptr, false, U'\0', U'\0', nullptr, nullptr)) {
		charp = compiled_regexp -> startp [0];
	}

	free (compiled_regexp);
	return charp;
}

static autostring32vector string32vector_searchAndReplace_literal (conststring32vector me,
	conststring32 search, conststring32 replace, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	if (! search || ! replace)
		return autostring32vector();
	autostring32vector result (me.size);

	integer nmatches_sub = 0, nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		conststring32 string = ( me [i] ? me [i] : U"" );   // treat null as an empty string

		result [i] = newSTRreplace (string, search, replace, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

static autostring32vector string32vector_searchAndReplace_regexp (conststring32vector me,
	conststring32 searchRE, conststring32 replaceRE, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	if (! searchRE || ! replaceRE)
		return autostring32vector();

	integer nmatches_sub = 0;

	regexp *compiledRE = CompileRE_throwable (searchRE, 0);

	autostring32vector result (me.size);

	integer nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		conststring32 string = ( me [i] ? me [i] : U"" );   // treat null as an empty string
		result [i] = newSTRreplace_regex (string, compiledRE, replaceRE, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

autostring32vector string32vector_searchAndReplace (conststring32vector me,
	conststring32 search, conststring32 replace, int maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp)
{
	return use_regexp ?
		string32vector_searchAndReplace_regexp (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches) :
		string32vector_searchAndReplace_literal (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches);
}

/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static autoINTVEC getElementsOfRanges (conststring32 ranges, integer maximumElement, conststring32 elementType) {
	/*
		Count the elements.
	*/
	integer previousElement = 0;
	integer numberOfElements = 0;
	const char32 *p = & ranges [0];
	for (;;) {
		while (Melder_isHorizontalSpace (*p)) p ++;
		if (*p == U'\0')
			break;
		if (Melder_isAsciiDecimalNumber (*p)) {
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0,
				U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement,
				U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			numberOfElements += 1;
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else if (*p == ':') {
			Melder_require (previousElement != 0, U"The range should not start with a colon.");
			
			do { p ++; } while (Melder_isHorizontalSpace (*p));
			Melder_require (*p != U'\0',
				U"The range should not end with a colon.");
			Melder_require (Melder_isAsciiDecimalNumber (*p),
				U"End of range should be a positive whole number.");
			
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0,
				U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement,
				U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			if (currentElement > previousElement) {
				numberOfElements += currentElement - previousElement;
			} else {
				numberOfElements += previousElement - currentElement;
			}
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else {
			Melder_throw (U"Start of range should be a positive whole number.");
		}
	}

	/*
		Create room for the elements.
	*/
	
	if (numberOfElements == 0)
		Melder_throw (U"No element(s) found");
	autoINTVEC elements = newINTVECraw (numberOfElements);

	/*
		Store the elements.
	*/
	previousElement = 0;
	numberOfElements = 0;
	p = & ranges [0];
	for (;;) {
		while (Melder_isHorizontalSpace (*p)) p ++;
		if (*p == U'\0')
			break;
		if (Melder_isAsciiDecimalNumber (*p)) {
			integer currentElement = Melder_atoi (p);
			elements [++ numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else if (*p == U':') {
			do { p ++; } while (Melder_isHorizontalSpace (*p));
			integer currentElement = Melder_atoi (p);
			if (currentElement > previousElement) {
				for (integer ielement = previousElement + 1; ielement <= currentElement; ielement ++)
					elements [++ numberOfElements] = ielement;
			} else {
				for (integer ielement = previousElement - 1; ielement >= currentElement; ielement --)
					elements [++ numberOfElements] = ielement;
			}
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		}
	}
	return elements;
}

static autoINTVEC INTVEC_getUniqueNumbers (INTVEC & inout_numbers) {
	autoINTVEC sorted = newINTVECraw (inout_numbers.size);
	sorted.get () <<= inout_numbers;
	
	NUMsort_integer (inout_numbers.size, sorted.at); // TODO INTVEC_sortInplace

	integer numberOfUniques = 1;
	for (integer i = 2; i <= inout_numbers.size; i ++) {
		if (sorted [i] != sorted [i - 1])
			sorted [++ numberOfUniques] = sorted [i];
	}
	sorted.resize (numberOfUniques);
	return sorted;
}

autoINTVEC NUMstring_getElementsOfRanges (conststring32 ranges, integer maximumElement, conststring32 elementType, bool sortedUniques)
{
	autoINTVEC elements = getElementsOfRanges (ranges, maximumElement, elementType);
	if (sortedUniques) elements = INTVEC_getUniqueNumbers (elements);
	return elements;
}

char32 * NUMstring_timeNoDot (double time) {
	static char32 string [100];
	integer seconds = Melder_ifloor (time);
	integer ms = Melder_iround ((time - seconds) * 1000.0);
	Melder_sprint (string,100, U"_", seconds, U"_", ms);
	return string;
}

/* End of file NUMstring.cpp */
