/* melder_ftoa.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2002/11/30 Melder_fixed
 * pb 2002/12/01 Melder_single, Melder_half
 * pb 2003/05/13 Melder_percent
 * pb 2003/05/19 Melder_fixed: include a minimum precision of 1 digit
 * pb 2004/04/04 Melder_bigInteger
 * pb 2006/04/16 separated from melder.c
 * pb 2006/12/10 A and W versions
 * pb 2006/12/29 removed future bug
 * pb 2007/11/30 Melder_float
 * pb 2008/01/06 Mac: use strtod instead of wcstod for speed
 * pb 2010/10/16 Melder_naturalLogarithm
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include "NUM.h"

/********** NUMBER TO STRING CONVERSION **********/

#define NUMBER_OF_BUFFERS  32
	/* = maximum number of arguments to a function call */
#define MAXIMUM_NUMERIC_STRING_LENGTH  386
	/* = sign + 308 + point + 60 + e + sign + 3 + null byte + (\.c10^^ - 1) + 4 extra */

static wchar_t buffers [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static int ibuffer = 0;

const wchar_t * Melder_integer (long value) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%ld", value);
	return buffers [ibuffer];
}

const wchar_t * Melder_bigInteger (long long value) {
	wchar_t *text;
	int quintillions, quadrillions, trillions, billions, millions, thousands, units;
	bool firstDigitPrinted = false;
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	text = buffers [ibuffer];
	text [0] = L'\0';
	if (value < 0) {
		swprintf (text, MAXIMUM_NUMERIC_STRING_LENGTH, L"-");
		value = - value;
	}
	quintillions =  value / 1000000000000000000LL;
	value -= quintillions * 1000000000000000000LL;
	quadrillions =  value / 1000000000000000LL;
	value -= quadrillions * 1000000000000000LL;
	trillions =     value / 1000000000000LL;
	value -=    trillions * 1000000000000LL;
	billions =      value / 1000000000LL;
	value -=     billions * 1000000000LL;
	millions =      value / 1000000LL;
	value -=     millions * 1000000LL;
	thousands =     value / 1000LL;
	value -=    thousands * 1000LL;
	units = value;
	if (quintillions) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", quintillions);
		firstDigitPrinted = TRUE;
	}
	if (quadrillions || firstDigitPrinted) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", quadrillions);
		firstDigitPrinted = TRUE;
	}
	if (trillions || firstDigitPrinted) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", trillions);
		firstDigitPrinted = TRUE;
	}
	if (billions || firstDigitPrinted) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", billions);
		firstDigitPrinted = TRUE;
	}
	if (millions || firstDigitPrinted) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", millions);
		firstDigitPrinted = TRUE;
	}
	if (thousands || firstDigitPrinted) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d," : L"%d,", thousands);
		firstDigitPrinted = TRUE;
	}
	swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, firstDigitPrinted ? L"%03d" : L"%d", units);
	return text;
}

const wchar_t * Melder_boolean (bool value) {
	return value ? L"yes" : L"no";
}

const wchar_t * Melder_double (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	#if defined (macintosh)
		/*
		 * OPTIMIZATION: strtod may be 100 times faster than wcstod on the Mac. 20080106
		 */
		static char buffer [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
		sprintf (buffer, "%.15g", value);
		if (strtod (buffer, NULL) != value) {
			sprintf (buffer, "%.16g", value);
			if (strtod (buffer, NULL) != value) {
				sprintf (buffer, "%.17g", value);
			}
		}
		#if 0
			wchar_t *to = & buffers [ibuffer] [0];
			char *from = & buffer [0];
			for (; (*to++ = *from++) != '\0';) ;
			*to = '\0';
		#else
			Melder_8bitToWcs_inline (buffer, buffers [ibuffer], kMelder_textInputEncoding_UTF8);   // guaranteed not to fail
		#endif
	#else
		swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.15g", value);
		if (wcstod (buffers [ibuffer], NULL) != value) {
			swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.16g", value);
			if (wcstod (buffers [ibuffer], NULL) != value) {
				swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.17g", value);
			}
		}
	#endif
	return buffers [ibuffer];
}

const wchar_t * Melder_single (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.9g", value);
	return buffers [ibuffer];
}

const wchar_t * Melder_half (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.4g", value);
	return buffers [ibuffer];
}

const wchar_t * Melder_fixed (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*f",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffers [ibuffer];
}

const wchar_t * Melder_fixedExponent (double value, int exponent, int precision) {
	double factor = pow (10, exponent);
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value /= factor;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*fE%d",
		minimumPrecision > precision ? minimumPrecision : precision, value, exponent);
	return buffers [ibuffer];
}

const wchar_t * Melder_percent (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value *= 100.0;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*f%%",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffers [ibuffer];
}

const wchar_t * Melder_float (const wchar_t *number) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (wcschr (number, 'e') == NULL) {
		wcscpy (buffers [ibuffer], number);
	} else {
		wchar_t *b = buffers [ibuffer];
		const wchar_t *n = number;
		while (*n != 'e') *(b++) = *(n++); *b = '\0';
		if (number [0] == '1' && number [1] == 'e') {
			wcscpy (buffers [ibuffer], L"10^^"); b = buffers [ibuffer] + 4;
		} else {
			wcscat (buffers [ibuffer], L"\\.c10^^"); b += 7;
		}
		Melder_assert (*n == 'e');
		if (*++n == '+') n ++;   /* Ignore leading plus sign in exponent. */
		if (*n == '-') *(b++) = *(n++);   /* Copy sign of negative exponent. */
		while (*n == '0') n ++;   /* Ignore leading zeroes in exponent. */
		while (*n >= '0' && *n <= '9') *(b++) = *(n++);
		*(b++) = '^';
		while (*n != '\0') *(b++) = *(n++); *b = '\0';
	}
	return buffers [ibuffer];
}

const wchar_t * Melder_naturalLogarithm (double lnNumber) {
	if (lnNumber == NUMundefined) return L"--undefined--";
	if (lnNumber == -INFINITY) return L"0";
	double log10Number = lnNumber * NUMlog10e;
	if (log10Number < -41) {
		if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
		long ceiling = ceil (log10Number);
		double remainder = log10Number - ceiling;
		double remainder10 = pow (10, remainder);
		while (remainder10 < 1.0) {
			remainder10 *= 10;
			ceiling --;
		}
		swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.15g", remainder10);
		if (wcstod (buffers [ibuffer], NULL) != remainder10) {
			swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.16g", remainder10);
			if (wcstod (buffers [ibuffer], NULL) != remainder10) swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.17g", remainder10);
		}
		swprintf (buffers [ibuffer] + wcslen (buffers [ibuffer]), 100, L"e-%ld", ceiling);
	} else {
		return Melder_double (exp (lnNumber));
	}
	return buffers [ibuffer];
}

/* End of file melder_ftoa.cpp */