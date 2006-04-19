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
 * pb 2006/04/16 moved from melder.c
 */

#include "melder.h"
#include "NUM.h"

/********** NUMBER TO STRING CONVERSION **********/

#define NUMBER_OF_BUFFERS  32
	/* = maximum number of arguments to a function call */
#define MAXIMUM_NUMERIC_STRING_LENGTH  380
	/* = sign + 308 + point + 60 + E + sign + 3 + null byte + 4 extra */

static char buffers [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH];
static int ibuffer = 0;

const char * Melder_integer (long value) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers [ibuffer], "%ld", value);
	return buffers [ibuffer];
}

const char * Melder_bigInteger (double value) {
	char *text;
	int trillions, billions, millions, thousands, units, firstDigitPrinted = FALSE;
	if (fabs (value) > 1e15) return Melder_double (value);
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	text = buffers [ibuffer];
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

const char * Melder_boolean (int value) {
	return value ? "yes" : "no";
}

const char * Melder_double (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers [ibuffer], "%.15g", value);
	if (atof (buffers [ibuffer]) != value) {
		sprintf (buffers [ibuffer], "%.16g", value);
		if (atof (buffers [ibuffer]) != value) sprintf (buffers [ibuffer], "%.17g", value);
	}
	return buffers [ibuffer];
}

const char * Melder_single (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers [ibuffer], "%.8g", value);
	return buffers [ibuffer];
}

const char * Melder_half (double value) {
	if (value == NUMundefined) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers [ibuffer], "%.4g", value);
	return buffers [ibuffer];
}

const char * Melder_fixed (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffers [ibuffer], "%.*f",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffers [ibuffer];
}

const char * Melder_fixedExponent (double value, int exponent, int precision) {
	double factor = pow (10, exponent);
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value /= factor;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffers [ibuffer], "%.*fE%d",
		minimumPrecision > precision ? minimumPrecision : precision, value, exponent);
	return buffers [ibuffer];
}

const char * Melder_percent (double value, int precision) {
	int minimumPrecision;
	if (value == NUMundefined) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value *= 100.0;
	minimumPrecision = - (int) floor (log10 (value));
	sprintf (buffers [ibuffer], "%.*f%%",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	return buffers [ibuffer];
}

/* End of file melder_ftoa.c */
