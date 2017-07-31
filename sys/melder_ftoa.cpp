/* melder_ftoa.cpp
 *
 * Copyright (C) 1992-2011,2014,2015,2017 Paul Boersma
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

#include "melder.h"

/********** NUMBER TO STRING CONVERSION **********/

#define NUMBER_OF_BUFFERS  32
	/* = maximum number of arguments to a function call */
#define MAXIMUM_NUMERIC_STRING_LENGTH  400
	/* = sign + 324 + point + 60 + e + sign + 3 + null byte + ("·10^^" - "e") + 4 extra */

static char   buffers8  [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static char32 buffers32 [NUMBER_OF_BUFFERS] [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
static int ibuffer = 0;

#define CONVERT_BUFFER_TO_CHAR32 \
	char32 *q = buffers32 [ibuffer]; \
	while (*p != '\0') * q ++ = (char32) (char8) * p ++; /* change sign before extending (should be unnecessary, because all characters should be below 128) */ \
	*q = U'\0'; \
	return buffers32 [ibuffer];

const char * Melder8_integer (int64 value) noexcept {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (sizeof (long) == 8) {
		int n = snprintf (buffers8 [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH + 1, "%ld", (long) value);   // cast to identical type, to make compiler happy
		Melder_assert (n > 0);
		Melder_assert (n <= MAXIMUM_NUMERIC_STRING_LENGTH);
	} else if (sizeof (long long) == 8) {
		/*
		 * There are buggy platforms (namely 32-bit Mingw on Windows XP) that support long long and %lld but that convert
		 * the argument to a 32-bit long.
		 * There are also buggy platforms (namely 32-bit gcc on Linux) that support long long and %I64d but that convert
		 * the argument to a 32-bit long.
		 */
		static const char *formatString = nullptr;
		if (! formatString) {
			char tryBuffer [MAXIMUM_NUMERIC_STRING_LENGTH + 1];
			formatString = "%lld";
			sprintf (tryBuffer, formatString, 1000000000000LL);
			if (! strequ (tryBuffer, "1000000000000")) {
				formatString = "%I64d";
				sprintf (tryBuffer, formatString, 1000000000000LL);
				if (! strequ (tryBuffer, "1000000000000")) {
					Melder_fatal (U"Found no way to print 64-bit integers on this machine.");
				}
			}
		}
		int n = snprintf (buffers8 [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH + 1, formatString, value);
		Melder_assert (n > 0);
		Melder_assert (n <= MAXIMUM_NUMERIC_STRING_LENGTH);
	} else {
		Melder_fatal (U"Neither long nor long long is 8 bytes on this machine.");
	}
	return buffers8 [ibuffer];
}
const char32 * Melder_integer (int64 value) noexcept {
	const char *p = Melder8_integer (value);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_bigInteger (int64 value) noexcept {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	char *text = buffers8 [ibuffer];
	text [0] = '\0';
	if (value < 0) {
		sprintf (text, "-");
		value = - value;
	}
	int quintillions =  value / 1000000000000000000LL;
	value -=     quintillions * 1000000000000000000LL;
	int quadrillions =  value / 1000000000000000LL;
	value -=     quadrillions * 1000000000000000LL;
	int trillions =     value / 1000000000000LL;
	value -=        trillions * 1000000000000LL;
	int billions =      value / 1000000000LL;
	value -=         billions * 1000000000LL;
	int millions =      value / 1000000LL;
	value -=         millions * 1000000LL;
	int thousands =     value / 1000LL;
	value -=        thousands * 1000LL;
	int units = value;
	bool firstDigitPrinted = false;
	if (quintillions) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", quintillions);
		firstDigitPrinted = true;
	}
	if (quadrillions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", quadrillions);
		firstDigitPrinted = true;
	}
	if (trillions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", trillions);
		firstDigitPrinted = true;
	}
	if (billions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", billions);
		firstDigitPrinted = true;
	}
	if (millions || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", millions);
		firstDigitPrinted = true;
	}
	if (thousands || firstDigitPrinted) {
		sprintf (text + strlen (text), firstDigitPrinted ? "%03d," : "%d,", thousands);
		firstDigitPrinted = true;
	}
	sprintf (text + strlen (text), firstDigitPrinted ? "%03d" : "%d", units);
	return text;
}
const char32 * Melder_bigInteger (int64 value) noexcept {
	const char *p = Melder8_bigInteger (value);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_boolean (bool value) noexcept {
	return value ? "yes" : "no";
}
const char32 * Melder_boolean (bool value) noexcept {
	return value ? U"yes" : U"no";
}

const char * Melder8_double (double value) noexcept {
	if (isundef (value)) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers8 [ibuffer], "%.15g", value);
	if (strtod (buffers8 [ibuffer], nullptr) != value) {
		sprintf (buffers8 [ibuffer], "%.16g", value);
		if (strtod (buffers8 [ibuffer], nullptr) != value) {
			sprintf (buffers8 [ibuffer], "%.17g", value);
		}
	}
	return buffers8 [ibuffer];
}
const char32 * Melder_double (double value) noexcept {
	const char *p = Melder8_double (value);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_single (double value) noexcept {
	if (isundef (value)) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers8 [ibuffer], "%.9g", value);
	return buffers8 [ibuffer];
}
const char32 * Melder_single (double value) noexcept {
	const char *p = Melder8_single (value);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_half (double value) noexcept {
	if (isundef (value)) return "--undefined--";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers8 [ibuffer], "%.4g", value);
	return buffers8 [ibuffer];
}
const char32 * Melder_half (double value) noexcept {
	const char *p = Melder8_half (value);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_fixed (double value, int precision) noexcept {
	int minimumPrecision;
	if (isundef (value)) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	minimumPrecision = - (int) floor (log10 (fabs (value)));
	int n = snprintf (buffers8 [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH + 1, "%.*f",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	Melder_assert (n > 0);
	Melder_assert (n <= MAXIMUM_NUMERIC_STRING_LENGTH);
	return buffers8 [ibuffer];
}
const char32 * Melder_fixed (double value, int precision) noexcept {
	const char *p = Melder8_fixed (value, precision);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_fixedExponent (double value, int exponent, int precision) noexcept {
	double factor = pow (10, exponent);
	int minimumPrecision;
	if (isundef (value)) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value /= factor;
	minimumPrecision = - (int) floor (log10 (fabs (value)));
	int n = snprintf (buffers8 [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH + 1, "%.*fE%d",
		minimumPrecision > precision ? minimumPrecision : precision, value, exponent);
	Melder_assert (n > 0);
	Melder_assert (n <= MAXIMUM_NUMERIC_STRING_LENGTH);
	return buffers8 [ibuffer];
}
const char32 * Melder_fixedExponent (double value, int exponent, int precision) noexcept {
	const char *p = Melder8_fixedExponent (value, exponent, precision);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_percent (double value, int precision) noexcept {
	int minimumPrecision;
	if (isundef (value)) return "--undefined--";
	if (value == 0.0) return "0";
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (precision > 60) precision = 60;
	value *= 100.0;
	minimumPrecision = - (int) floor (log10 (fabs (value)));
	int n = snprintf (buffers8 [ibuffer], MAXIMUM_NUMERIC_STRING_LENGTH + 1, "%.*f%%",
		minimumPrecision > precision ? minimumPrecision : precision, value);
	Melder_assert (n > 0);
	Melder_assert (n <= MAXIMUM_NUMERIC_STRING_LENGTH);
	return buffers8 [ibuffer];
}
const char32 * Melder_percent (double value, int precision) noexcept {
	const char *p = Melder8_percent (value, precision);
	CONVERT_BUFFER_TO_CHAR32
}

const char32 * Melder_float (const char32 *number) noexcept {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	if (! str32chr (number, 'e')) {
		str32cpy (buffers32 [ibuffer], number);
	} else {
		char32 *b = buffers32 [ibuffer];
		const char32 *n = number;
		while (*n != U'e') *(b++) = *(n++); *b = U'\0';
		if (number [0] == '1' && number [1] == 'e') {
			str32cpy (buffers32 [ibuffer], U"10^^"); b = buffers32 [ibuffer] + 4;
		} else {
			str32cpy (buffers32 [ibuffer] + str32len (buffers32 [ibuffer]), U"·10^^"); b += 5;
		}
		Melder_assert (*n == U'e');
		if (*++n == U'+') n ++;   // ignore leading plus sign in exponent
		if (*n == U'-') *(b++) = *(n++);   // copy sign of negative exponent
		while (*n == U'0') n ++;   // ignore leading zeroes in exponent
		while (*n >= U'0' && *n <= U'9') *(b++) = *(n++);
		*(b++) = U'^';
		while (*n != U'\0') *(b++) = *(n++);
		*b = U'\0';
	}
	return buffers32 [ibuffer];
}

const char * Melder8_naturalLogarithm (double lnNumber) noexcept {
	//if (lnNumber == -INFINITY) return "0";   // this would have been nice, but cannot be relied upon
	if (isundef (lnNumber)) return "--undefined--";
	double log10Number = lnNumber * NUMlog10e;
	if (log10Number < -41.0) {
		if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
		long ceiling = (long) ceil (log10Number);
		double remainder = log10Number - ceiling;
		double remainder10 = pow (10.0, remainder);
		while (remainder10 < 1.0) {
			remainder10 *= 10.0;
			ceiling --;
		}
		sprintf (buffers8 [ibuffer], "%.15g", remainder10);
		if (strtod (buffers8 [ibuffer], nullptr) != remainder10) {
			sprintf (buffers8 [ibuffer], "%.16g", remainder10);
			if (strtod (buffers8 [ibuffer], nullptr) != remainder10) sprintf (buffers8 [ibuffer], "%.17g", remainder10);
		}
		sprintf (buffers8 [ibuffer] + strlen (buffers8 [ibuffer]), "e-%ld", ceiling);
	} else {
		return Melder8_double (exp (lnNumber));
	}
	return buffers8 [ibuffer];
}
const char32 * Melder_naturalLogarithm (double lnNumber) noexcept {
	const char *p = Melder8_naturalLogarithm (lnNumber);
	CONVERT_BUFFER_TO_CHAR32
}

const char * Melder8_pointer (void *pointer) noexcept {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	sprintf (buffers8 [ibuffer], "%p", pointer);
	return buffers8 [ibuffer];
}
const char32 * Melder_pointer (void *pointer) noexcept {
	const char *p = Melder8_pointer (pointer);
	CONVERT_BUFFER_TO_CHAR32
}

const char32 * Melder_character (char32 kar) noexcept {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	buffers32 [ibuffer] [0] = kar;
	buffers32 [ibuffer] [1] = U'\0';
	return buffers32 [ibuffer];
}

/********** TENSOR TO STRING CONVERSION **********/

#define NUMBER_OF_TENSOR_BUFFERS  3
static MelderString theTensorBuffers [NUMBER_OF_TENSOR_BUFFERS] { };
static int iTensorBuffer { 0 };

const char32 * Melder_numvec (numvec value) {
	if (++ iTensorBuffer == NUMBER_OF_TENSOR_BUFFERS) iTensorBuffer = 0;
	MelderString *string = & theTensorBuffers [iTensorBuffer];
	MelderString_empty (string);
	if (value.at) {
		for (long i = 1; i <= value.size; i ++) {
			MelderString_append (string, value [i], U'\n');
		}
	}
	return string -> string;
}
const char32 * Melder_nummat  (nummat value) {
	if (++ iTensorBuffer == NUMBER_OF_TENSOR_BUFFERS) iTensorBuffer = 0;
	MelderString *string = & theTensorBuffers [iTensorBuffer];
	MelderString_empty (string);
	if (value.at) {
		for (long irow = 1; irow <= value.nrow; irow ++) {
			for (long icol = 1; icol <= value.ncol; icol ++) {
				MelderString_append (string, value [irow] [icol]);
				if (icol < value.ncol) MelderString_appendCharacter (string, U' ');
			}
			if (irow < value.nrow) MelderString_appendCharacter (string, U'\n');
		}
	}
	return string -> string;
}

/********** STRING TO STRING CONVERSION **********/

static MelderString thePadBuffers [NUMBER_OF_BUFFERS];
static int iPadBuffer { 0 };

const char32 * Melder_pad (int64 width, const char32 *string) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooShort = width - length;
	if (tooShort <= 0) return string;
	MelderString_empty (& thePadBuffers [iPadBuffer]);
	for (int64 i = 0; i < tooShort; i ++)
		MelderString_appendCharacter (& thePadBuffers [iPadBuffer], U' ');
	MelderString_append (& thePadBuffers [iPadBuffer], string);
	return thePadBuffers [iPadBuffer]. string;
}

const char32 * Melder_pad (const char32 *string, int64 width) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooShort = width - length;
	if (tooShort <= 0) return string;
	MelderString_copy (& thePadBuffers [iPadBuffer], string);
	for (int64 i = 0; i < tooShort; i ++)
		MelderString_appendCharacter (& thePadBuffers [iPadBuffer], U' ');
	return thePadBuffers [iPadBuffer]. string;
}

const char32 * Melder_truncate (int64 width, const char32 *string) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooLong = length - width;
	if (tooLong <= 0) return string;
	MelderString_ncopy (& thePadBuffers [iPadBuffer], string + tooLong, width);
	return thePadBuffers [iPadBuffer]. string;
}

const char32 * Melder_truncate (const char32 *string, int64 width) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooLong = length - width;
	if (tooLong <= 0) return string;
	MelderString_ncopy (& thePadBuffers [iPadBuffer], string, width);
	return thePadBuffers [iPadBuffer]. string;
}

const char32 * Melder_padOrTruncate (int64 width, const char32 *string) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooLong = length - width;
	if (tooLong == 0) return string;
	if (tooLong < 0) {
		int64 tooShort = - tooLong;
		MelderString_empty (& thePadBuffers [iPadBuffer]);
		for (int64 i = 0; i < tooShort; i ++)
			MelderString_appendCharacter (& thePadBuffers [iPadBuffer], U' ');
		MelderString_append (& thePadBuffers [iPadBuffer], string);
	} else {
		MelderString_ncopy (& thePadBuffers [iPadBuffer], string + tooLong, width);
	}
	return thePadBuffers [iPadBuffer]. string;
}

const char32 * Melder_padOrTruncate (const char32 *string, int64 width) {
	if (++ iPadBuffer == NUMBER_OF_BUFFERS) iPadBuffer = 0;
	int64 length = str32len (string);
	int64 tooLong = length - width;
	if (tooLong == 0) return string;
	if (tooLong < 0) {
		int64 tooShort = - tooLong;
		MelderString_copy (& thePadBuffers [iPadBuffer], string);
		for (int64 i = 0; i < tooShort; i ++)
			MelderString_appendCharacter (& thePadBuffers [iPadBuffer], U' ');
	} else {
		MelderString_ncopy (& thePadBuffers [iPadBuffer], string, width);
	}
	return thePadBuffers [iPadBuffer]. string;
}

/* End of file melder_ftoa.cpp */
