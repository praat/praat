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

static char buffersA [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static int ibufferA = 0;

static wchar_t buffersW [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static int ibufferW = 0;

const char * Melder_integerA (long value) {
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	sprintf (buffersA [ibufferA], "%ld", value);
	return buffersA [ibufferA];
}

const wchar_t * Melder_integerW (long value) {
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%ld", value);
	return buffersW [ibufferW];
}

const char * Melder_bigIntegerA (double value) {
	char *text;
	int trillions, billions, millions, thousands, units, firstDigitPrinted = FALSE;
	if (fabs (value) > 1e15) return Melder_doubleA (value);
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	text = buffersA [ibufferA];
	text [0] = '\0';
	if (value < 0.0) {
		sprintf (text, "-");
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
		sprintf (text + strlen (text), "%d,", trillions);
		firstDigitPrinted = TRUE;
	}
	if (billions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", billions);
		firstDigitPrinted = TRUE;
	}
	if (millions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", millions);
		firstDigitPrinted = TRUE;
	}
	if (thousands || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", thousands);
		firstDigitPrinted = TRUE;
	}
	sprintf (text + strlen (text), firstDigitPrinted ? "%03d" : "%d", units);
	return text;
}

const wchar_t * Melder_bigIntegerW (double value) {
	wchar_t *text;
	int trillions, billions, millions, thousands, units, firstDigitPrinted = FALSE;
	if (fabs (value) > 1e15) return Melder_doubleW (value);
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	text = buffersW [ibufferW];
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

const char * Melder_booleanA (bool value) {
	return value ? "yes" : "no";
}

const wchar_t * Melder_booleanW (bool value) {
	return value ? L"yes" : L"no";
}

const char * Melder_doubleA (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	sprintf (buffersA [ibufferA], "%.15g", value);
	if (atof (buffersA [ibufferA]) != value) {
		sprintf (buffersA [ibufferA], "%.16g", value);
		if (atof (buffersA [ibufferA]) != value) sprintf (buffersA [ibufferA], "%.17g", value);
	}
	return buffersA [ibufferA];
}

const wchar_t * Melder_doubleW (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.15g", value);
	if (wcstod (buffersW [ibufferW], NULL) != value) {
		swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.16g", value);
		if (wcstod (buffersW [ibufferW], NULL) != value) swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.17g", value);
	}
	return buffersW [ibufferW];
}

const char * Melder_singleA (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	sprintf (buffersA [ibufferA], "%.9g", value);
	return buffersA [ibufferA];
}

const wchar_t * Melder_singleW (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.9g", value);
	return buffersW [ibufferW];
}

const char * Melder_halfA (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	sprintf (buffersA [ibufferA], "%.4g", value);
	return buffersA [ibufferA];
}

const wchar_t * Melder_halfW (double value) {
	if (value == NUMundefined) return L"--undefined--";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.4g", value);
	return buffersW [ibufferW];
}

const char * Melder_fixedA (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	if (precision > 60) precision = 60;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffersA [ibufferA], "%.*f",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffersA [ibufferA];
}

const wchar_t * Melder_fixedW (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	if (precision > 60) precision = 60;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*f",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffersW [ibufferW];
}

const char * Melder_fixedExponentA (double value, int exponent, int precision) {
	double factor = pow (10, exponent);
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	if (precision > 60) precision = 60;
	value /= factor;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffersA [ibufferA], "%.*fE%d",
		minimumPrecision > precision ? minimumPrecision : precision, value, exponent);
	return buffersA [ibufferA];
}

const wchar_t * Melder_fixedExponentW (double value, int exponent, int precision) {
	double factor = pow (10, exponent);
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	if (precision > 60) precision = 60;
	value /= factor;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*fE%d",
		minimumPrecision > precision ? minimumPrecision : precision, value, exponent);
	return buffersW [ibufferW];
}

const char * Melder_percentA (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibufferA == NUMBER_OF_BUFFERS) ibufferA = 0;
	if (precision > 60) precision = 60;
	value *= 100.0;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffersA [ibufferA], "%.*f%%",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffersA [ibufferA];
}

const wchar_t * Melder_percentW (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return L"--undefined--";
	if (value == 0.0) return L"0";
	if (++ ibufferW == NUMBER_OF_BUFFERS) ibufferW = 0;
	if (precision > 60) precision = 60;
	value *= 100.0;
	minimumPrecision = - (int) floor (log10 (value));
	swprintf (buffersW [ibufferW], MAXIMUM_NUMERIC_STRING_LENGTH, L"%.*f%%",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffersW [ibufferW];
}

/* End of file melder_ftoa.c */
