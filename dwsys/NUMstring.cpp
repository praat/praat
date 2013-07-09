/* NUMstring.cpp
*
* Copyright (C) 2012 David Weenink
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at
* your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 djmw 20121005 First version
*/

#include <ctype.h>
#include <wctype.h>
#include "Interpreter.h"
#include "NUM2.h"


int NUMstring_containsPrintableCharacter (const wchar_t *s) {
	long len;
	if (s == NULL || ( (len = wcslen (s)) == 0)) {
		return 0;
	}
	for (long i = 0; i < len; i++) {
		int c = s[i];
		if (isgraph (c)) {
			return 1;
		}
	}
	return 0;
}

void NUMstring_chopWhiteSpaceAtExtremes_inline (wchar_t *string) {
	long start = 0;
	while (iswspace (string[start]) != 0) {
		start++;
	}
	long end = wcslen (string);
	while (end != start && iswspace(string[end - 1]) != 0) {
		end--;
	}
	long n = end - start;
	memmove (string, string + start, n * sizeof (wchar_t));
	string[n] = 0;
}

double *NUMstring_to_numbers (const wchar_t *s, long *numbers_found) {
	*numbers_found = Melder_countTokens (s);
	if (*numbers_found < 1) {
		Melder_throw ("Empty string.");
	}
	autoNUMvector<double> numbers (1, *numbers_found);
	long inum = 1;
	for (wchar_t *token = Melder_firstToken (s); token != 0; token = Melder_nextToken (), inum++) {
		Interpreter_numericExpression (0, token, &numbers[inum]);
	}
	return numbers.transfer();
}

void NUMstrings_copyElements (wchar_t **from, wchar_t **to, long lo, long hi) {
	for (long i = lo; i <= hi; i++) {
		Melder_free (to[i]);
		if (from[i]) {
			to[i] = Melder_wcsdup (from[i]);
		}
	}
}

void NUMstrings_free (wchar_t **s, long lo, long hi) {
	if (s == NULL) {
		return;
	}
	for (long i = lo; i <= hi; i++) {
		Melder_free (s[i]);
	}
	NUMvector_free<wchar_t *> (s, lo);
}

wchar_t **NUMstrings_copy (wchar_t **from, long lo, long hi) {
	autoNUMvector<wchar_t *> to (lo, hi);
	NUMstrings_copyElements (from, to.peek(), lo, hi);
	return to.transfer();
}

static wchar_t *appendNumberToString (const wchar_t *s, long number, int asArray) {
	wchar_t buf[30];
	long ncharb, nchars = 0;
	ncharb = swprintf (buf, 29, (asArray == 0 ? L"%ld" : asArray == 1 ? L"[%ld]" : L"(%ld)"), number);
	if (s != NULL) {
		nchars = wcslen (s);
	}
	wchar_t *newc = Melder_calloc (wchar_t, nchars + ncharb + 1);
	if (nchars > 0) {
		wcsncpy (newc, s, nchars);
	}
	wcsncpy (newc + nchars, buf, ncharb + 1);
	return newc;
}

int NUMstrings_setSequentialNumbering (wchar_t **s, long lo, long hi, const wchar_t *pre, long number, long increment, int asArray) {
	for (long i = lo; i <= hi; i++, number += increment) {
		wchar_t *newc = appendNumberToString (pre, number, asArray);
		if (newc == NULL) {
			return 0;
		}
		Melder_free (s[i]);
		s[i] = newc;
	}
	return 1;
}

#define HIGHBYTE(x) ((unsigned char) ((x) & 0xFF))
#define LOWBYTE(x)  ((unsigned char) ((x) >> 8 & 0xFF))

/* a+b=c in radix 256 */
void NUMstring_add (unsigned char *a, unsigned char *b, unsigned char *c, long n);
void NUMstring_add (unsigned char *a, unsigned char *b, unsigned char *c, long n) {
	int j;
	unsigned short reg = 0;

	for (j = n; j > 1; j--) {
		reg = a[j] + b[j] + HIGHBYTE (reg);
		c[j + 1] = LOWBYTE (reg);
	}
}

wchar_t *strstr_regexp (const wchar_t *string, const wchar_t *search_regexp) {
	wchar_t *charp = 0;
	const wchar_t *compileMsg;
	regexp *compiled_regexp = CompileRE ( (regularExp_CHAR *) search_regexp, &compileMsg, 0);

	if (compiled_regexp == 0) {
		Melder_throw ("No regexp");
	}

	if (ExecRE (compiled_regexp, NULL, (regularExp_CHAR *) string, NULL, 0, '\0', '\0', NULL, NULL, NULL)) {
		charp = (wchar *) compiled_regexp -> startp[0];
	}

	free (compiled_regexp);
	return charp;
}

wchar_t *str_replace_literal (const wchar_t *string, const wchar_t *search, const wchar_t *replace,
                              long maximumNumberOfReplaces, long *nmatches) {
	if (string == 0 || search == 0 || replace == 0) {
		return NULL;
	}


	int len_string = wcslen (string);
	if (len_string == 0) {
		maximumNumberOfReplaces = 1;
	}
	int len_search = wcslen (search);
	if (len_search == 0) {
		maximumNumberOfReplaces = 1;
	}

	/*
		To allocate memory for 'result' only once, we have to know how many
		matches will occur.
	*/

	const wchar_t *pos = string; //current position / start of current match
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
			while ( (pos = wcsstr (pos, search)) && *nmatches < maximumNumberOfReplaces) {
				pos += len_search;
				(*nmatches) ++;
			}
		}
	}

	int len_replace = wcslen (replace);
	int len_result = len_string + *nmatches * (len_replace - len_search);
	wchar_t *result = Melder_malloc (wchar_t, (len_result + 1) * sizeof (wchar_t));
	result[len_result] = '\0';

	const wchar_t *posp = pos = string;
	int nchar = 0, result_nchar = 0;
	for (long i = 1; i <= *nmatches; i++) {
		pos = wcsstr (pos, search);

		/*
			Copy gap between end of previous match and start of current.
		*/

		nchar = (pos - posp);
		if (nchar > 0) {
			wcsncpy (result + result_nchar, posp, nchar);
			result_nchar += nchar;
		}

		/*
			Insert the replace string in result.
		*/

		wcsncpy (result + result_nchar, replace, len_replace);
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
		wcsncpy (result + result_nchar, posp, nchar);
	}
	return result;
}

wchar_t *str_replace_regexp (const wchar_t *string, regexp *compiledSearchRE,
                             const wchar_t *replaceRE, long maximumNumberOfReplaces, long *nmatches) {
	int buf_nchar = 0;				/* # characters in 'buf' */
	int gap_copied = 0;
	int nchar, reverse = 0;
	int errorType;
	wchar_t prev_char = '\0';
	const wchar_t *pos; 	/* current position in 'string' / start of current match */
	const wchar_t *posp; /* end of previous match */
	autostring buf;

	*nmatches = 0;
	if (string == 0 || compiledSearchRE == 0 || replaceRE == 0) {
		return 0;
	}

	int string_length = wcslen (string);
	//int replace_length = wcslen (replaceRE);
	if (string_length == 0) {
		maximumNumberOfReplaces = 1;
	}

	long i = maximumNumberOfReplaces > 0 ? 0 : - string_length;

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
	while (ExecRE (compiledSearchRE, 0, (regularExp_CHAR *) pos, 0, reverse, prev_char, '\0', 0, 0, 0) && i++ < maximumNumberOfReplaces) {
		/*
			Copy gap between the end of the previous match and the start
			of the current match.
			Check buffer overflow. pos == posp ? '\0' : pos[-1],
		*/

		pos = (wchar_t *) compiledSearchRE -> startp[0];
		nchar = pos - posp;
		if (nchar > 0 && ! gap_copied) {
			if (buf_nchar + nchar + 1 > buf_size) {
				buf_size *= 2;
				buf.resize (buf_size);
			}
			wcsncpy (buf.peek() + buf_nchar, posp, nchar);
			buf_nchar += nchar;
		}

		gap_copied = 1;

		/*
			Do the substitution. We can only check afterwards for buffer
			overflow. SubstituteRE puts null byte at last replaced position and signals when overflow.
		*/

		if ( (SubstituteRE (compiledSearchRE, (regularExp_CHAR *) replaceRE, (regularExp_CHAR *) buf.peek() + buf_nchar, buf_size - buf_nchar, &errorType)) == false) {
			if (errorType == 1) { // not enough memory
				buf_size *= 2;
				buf.resize (buf_size);
				Melder_clearError ();
				i--; // retry
				continue;
			}
			Melder_throw ("Error during substitution.");
		}

		// Buffer is not full, get number of characters added;

		nchar = wcslen (buf.peek() + buf_nchar);
		buf_nchar += nchar;

		// Update next start position in search string.

		posp = pos;
		pos = (wchar_t *) compiledSearchRE -> endp[0];
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

	wcsncpy (buf.peek() + buf_nchar, pos, nchar);
	buf[buf_size - 1] = '\0';
	return buf.transfer();
}

static wchar_t **strs_replace_literal (wchar_t **from, long lo, long hi, const wchar_t *search,
	const wchar_t *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches) {
	if (search == NULL || replace == NULL) {
		return NULL;
	}
	autostringvector result (lo, hi);
	try {
		long nmatches_sub = 0;
		*nmatches = 0; *nstringmatches = 0;
		for (long i = lo; i <= hi; i++) {
			/* Treat a NULL as an empty string */
			const wchar_t *string = from[i] == NULL ? L"" : from[i];

			result[i] = str_replace_literal (string, search, replace, maximumNumberOfReplaces, &nmatches_sub);
			if (nmatches_sub > 0) {
				*nmatches += nmatches_sub;
				(*nstringmatches) ++;
			}
		}
		return result.transfer();
	} catch (MelderError) {
		return 0;
	}
}

static wchar_t **strs_replace_regexp (wchar_t **from, long lo, long hi, const wchar_t *searchRE,
	const wchar_t *replaceRE, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches) {
	if (searchRE == NULL || replaceRE == NULL) {
		return NULL;
	}
	autostringvector result;
	try {
		regexp *compiledRE;
		const wchar_t *compileMsg;
		long nmatches_sub = 0;

		compiledRE = CompileRE ( (regularExp_CHAR *) searchRE, &compileMsg, 0);
		if (compiledRE == NULL) {
			Melder_throw ("No regexp ");
		}

		result.reset (lo, hi);

		*nmatches = 0; *nstringmatches = 0;
		for (long i = lo; i <= hi; i++) {
			/* Treat a NULL as an empty string */
			const wchar_t *string = from[i] == NULL ? L"" : from[i];
			result [i] = str_replace_regexp (string, compiledRE, replaceRE,
			                                 maximumNumberOfReplaces, &nmatches_sub);
			if (nmatches_sub > 0) {
				*nmatches += nmatches_sub;
				(*nstringmatches) ++;
			}
		}
		return result.transfer();
	} catch (MelderError) {
		return 0;
	}
}

wchar_t **strs_replace (wchar_t **from, long lo, long hi, const wchar_t *search, const wchar_t *replace,
                        int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	if (use_regexp) return strs_replace_regexp (from, lo, hi, search,
		                       replace, maximumNumberOfReplaces, nmatches, nstringmatches);
	else return strs_replace_literal (from, lo, hi, search, replace,
		                                  maximumNumberOfReplaces, nmatches, nstringmatches);
}


/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static long *getElementsOfRanges (const wchar_t *ranges, long maximumElement, long *numberOfElements, const wchar_t *elementType) {
	/*
	 * Count the elements.
	 */
	long previousElement = 0;
	*numberOfElements = 0;
	const wchar_t *p = & ranges [0];
	for (;;) {
		while (*p == ' ' || *p == '\t') p ++;
		if (*p == '\0') break;
		if (isdigit (*p)) {
			long currentElement = wcstol (p, NULL, 10);
			if (currentElement == 0)
				Melder_throw ("No such ", elementType, L": 0 (minimum is 1).");
			if (currentElement > maximumElement)
				Melder_throw ("No such ", elementType, ": ", currentElement, " (maximum is ", maximumElement, ").");
			*numberOfElements += 1;
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else if (*p == ':') {
			if (previousElement == 0)
				Melder_throw ("Cannot start range with colon.");
			do { p ++; } while (*p == ' ' || *p == '\t');
			if (*p == '\0')
				Melder_throw ("Cannot end range with colon.");
			if (! isdigit (*p))
				Melder_throw ("End of range should be a positive whole number.");
			long currentElement = wcstol (p, NULL, 10);
			if (currentElement == 0)
				Melder_throw ("No such ", elementType, ": 0 (minimum is 1).");
			if (currentElement > maximumElement)
				Melder_throw ("No such ", elementType, ": ", currentElement, " (maximum is ", maximumElement, ").");
			if (currentElement > previousElement) {
				*numberOfElements += currentElement - previousElement;
			} else {
				*numberOfElements += previousElement - currentElement;
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else {
			Melder_throw ("Start of range should be a positive whole number.");
		}
	}
	/*
	 * Create room for the elements.
	 */
	autoNUMvector <long> elements (1, *numberOfElements);
	/*
	 * Store the elements.
	 */
	previousElement = 0;
	*numberOfElements = 0;
	p = & ranges [0];
	for (;;) {
		while (*p == ' ' || *p == '\t') p ++;
		if (*p == '\0') break;
		if (isdigit (*p)) {
			long currentElement = wcstol (p, NULL, 10);
			elements [++ *numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else if (*p == ':') {
			do { p ++; } while (*p == ' ' || *p == '\t');
			long currentElement = wcstol (p, NULL, 10);
			if (currentElement > previousElement) {
				for (long ielement = previousElement + 1; ielement <= currentElement; ielement ++) {
					elements [++ *numberOfElements] = ielement;
				}
			} else {
				for (long ielement = previousElement - 1; ielement >= currentElement; ielement --) {
					elements [++ *numberOfElements] = ielement;
				}
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		}
	}
	return elements.transfer();
}

static void NUMlvector_getUniqueNumbers (long *numbers, long *numberOfElements, long *numberOfMultiples) {

	autoNUMvector<long> sorted (NUMvector_copy<long> (numbers, 1, *numberOfElements), 1);
	NUMsort_l (*numberOfElements, sorted.peek());
	if (numberOfMultiples != 0) {
		*numberOfMultiples = 0;
	}
	numbers[1] = sorted[1];
	long i = 2, n = 1;
	while (i <= *numberOfElements) {
		if (sorted[i] != sorted[i - 1]) {
			numbers[++n] = sorted[i];
		} else {
			if ((i > 2 && sorted[i - 1] != sorted[i - 1]) || i == 2) {
				if (numberOfMultiples) {
					(*numberOfMultiples)++;
				}
			}
		}
		i++;
	}
	*numberOfElements = n;
}

long *NUMstring_getElementsOfRanges (const wchar_t *ranges, long maximumElement, long *numberOfElements, long *numberOfMultiples, const wchar_t *elementType, bool sortedUniques) {
	autoNUMvector<long> elements (getElementsOfRanges (ranges, maximumElement, numberOfElements, elementType), 1);
	if (sortedUniques) {
		NUMlvector_getUniqueNumbers (elements.peek(), numberOfElements, numberOfMultiples);
	}
	return elements.transfer();
}

wchar_t * NUMstring_timeNoDot (double time) {
	static wchar_t string[100];
	long seconds = time;
	long ms = round((double)((time - seconds) * 1000.0));
	swprintf (string, 99, L"_%ld_%ld", seconds, ms);
	return string;
}

/* End of file NUMstring.cpp */
