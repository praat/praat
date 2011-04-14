/* melder_atof.cpp
 *
 * Copyright (C) 2003-2011 Paul Boersma
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
 * pb 2003/05/19 Melder_atof
 * pb 2006/04/16 moved Melder_atof from melder.c
 * pb 2006/04/16 moved Melder_isStringNumeric from Table.c
 * pb 2006/12/08 guard against null strings
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include "NUM.h"

static const wchar_t *findEndOfNumericString_nothrow (const wchar_t *string) {
	const wchar_t *p = & string [0];
	/*
	 * Leading white space is OK.
	 */
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p ++;
	/*
	 * Next we accept an optional leading plus or minus.
	 */
	if (*p == '+' || *p == '-') p ++;
	/*
	 * The next character must be a decimal digit.
	 * So we don't allow things like ".5".
	 */
	if (*p < '0' || *p > '9') return NULL;   /* String is not numeric. */
	p ++;
	/*
	 * Then we accept any number of decimal digits.
	 */
	while (*p >= '0' && *p <= '9') p ++;
	/*
	 * Next we accept an optional decimal point.
	 */
	if (*p == '.') {
		p ++;
		/*
		 * We accept any number of (even zero) decimal digits after the decimal point.
		 */
		while (*p >= '0' && *p <= '9') p ++;
	}
	/*
	 * Next we accept an optional exponential E or e.
	 */
	if (*p == 'e' || *p == 'E') {
		p ++;
		/*
		 * In the exponent we accept an optional leading plus or minus.
		 */
		if (*p == '+' || *p == '-') p ++;
		/*
		 * The exponent must contain a decimal digit.
		 * So we don't allow things like "+2.1E".
		 */
		if (*p < '0' || *p > '9') return NULL;   /* String is not numeric. */
		p ++;
		/*
		 * Then we accept any number of decimal digits.
		 */
		while (*p >= '0' && *p <= '9') p ++;
	}
	/*
	 * Next we accept an optional percent sign.
	 */
	if (*p == '%') p ++;
	/*
	 * We have found the end of the numeric string.
	 */
	return p;
}

int Melder_isStringNumeric_nothrow (const wchar_t *string) {
	if (string == NULL) return false;
	const wchar_t *p = findEndOfNumericString_nothrow (string);
	if (p == NULL) return FALSE;
	/*
	 * We accept only white space after the numeric string.
	 */
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p ++;
	return *p == '\0';
}

double Melder_atof (const wchar_t *string) {
	if (string == NULL) return NUMundefined;
	const wchar_t *p = findEndOfNumericString_nothrow (string);
	if (p == NULL) return NUMundefined;
	Melder_assert (p - string > 0);
	#if defined (macintosh)
		/* strtod may be 100 times faster than wcstod: */
		return p [-1] == '%' ? 0.01 * strtod (Melder_peekWcsToUtf8 (string), NULL) : strtod (Melder_peekWcsToUtf8 (string), NULL);
	#else
		return p [-1] == '%' ? 0.01 * wcstod (string, NULL) : wcstod (string, NULL);
	#endif
}

/* End of file melder_atof.cpp */
