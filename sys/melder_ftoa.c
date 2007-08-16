/* melder_ftoa.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 */

#include "melder.h"
#include "NUM.h"

/********** NUMBER TO STRING CONVERSION **********/

#define NUMBER_OF_BUFFERS  32
	/* = maximum number of arguments to a function call */
#define MAXIMUM_NUMERIC_STRING_LENGTH  380
	/* = sign + 308 + point + 60 + E + sign + 3 + null byte + 4 extra */

static wchar_t buffers [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static int ibuffer = 0;

const wchar_t * Melder_integer (long value) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%ld", value);
	return buffers [ibuffer];
}

const wchar_t * Melder_bigInteger (double value) {
	wchar_t *text;
	int trillions, billions, millions, thousands, units, firstDigitPrinted = FALSE;
	if (fabs (value) > 1e15) return Melder_double (value);
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	text = buffers [ibuffer];
	text [0] = L'\0';
	if (value < 0.0) {
		swprintf (text, MAXIMUM_NUMERIC_STRING_LENGTH, L"-");
		value = - value;
	}
	trillions = floor (value / 1e12);
	value -= trillions * 1e12;
	billions = floor (value / 1e9);
	value -= billions * 1e9;
	millions = floor (value / 1e6);
	value -= millions * 1e6;
	thousands = floor (value / 1e3);
	value -= thousands * 1e3;
	units = value;
	if (trillions) {
		swprintf (text + wcslen (text), MAXIMUM_NUMERIC_STRING_LENGTH, L"%d,", trillions);
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
	swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.15g", value);
	if (wcstod (buffers [ibuffer], NULL) != value) {
		swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.16g", value);
		if (wcstod (buffers [ibuffer], NULL) != value) swprintf (buffers [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.17g", value);
	}
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

/* End of file melder_ftoa.c */
