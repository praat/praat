#ifndef _melder_ftoa_h_
#define _melder_ftoa_h_
/* melder_ftoa.h
 *
 * Copyright (C) 1992-2019,2021 Paul Boersma
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

/*
	The following functions return a static string, chosen from a circularly used set of 32 buffers.
	You can call at most 32 of them in one Melder_casual call, for instance.
*/

conststring32 Melder_integer (int64 value);
conststring8 Melder8_integer (int64 value);

conststring32 Melder_bigInteger (int64 value);
conststring8 Melder8_bigInteger (int64 value);

conststring32 Melder_boolean (bool value);
conststring8 Melder8_boolean (bool value);
	// "yes" or "no"

conststring32 Melder_onoff (bool value);
conststring8 Melder8_onoff (bool value);

conststring32 Melder_kleenean (kleenean value);
conststring8 Melder8_kleenean (kleenean value);
	// "unknown" (< 0), "yes" (> 0), or "no" (0)

/*
	Format a double value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats.
*/
conststring32 Melder_double (double value);
conststring8 Melder8_double (double value);
conststring32 Melder_double_overtlyReal (double value);
conststring8 Melder8_double_overtlyReal (double value);

/*
	Format a double value as "--undefined--" or something in the "%.9g" format.
*/
conststring32 Melder_single (double value);
conststring8 Melder8_single (double value);

/*
	Format a double value as "--undefined--" or something in the "%.4g" format.
*/
conststring32 Melder_half (double value);
conststring8 Melder8_half (double value);

/*
	Format a double value as "--undefined--" or something in the "%.*f" format.
*/
conststring32 Melder_fixed (double value, integer precision);
conststring8 Melder8_fixed (double value, integer precision);

/*
	Format a double value with a specified precision. If exponent is -2 and precision is 2, you get things like 67E-2 or 0.00024E-2.
*/
conststring32 Melder_fixedExponent (double value, integer exponent, integer precision);
conststring8 Melder8_fixedExponent (double value, integer exponent, integer precision);

/*
	Format a double value as a percentage. If precision is 3, you get things like "0" or "34.400%" or "0.014%" or "0.001%" or "0.0000007%".
*/
conststring32 Melder_percent (double value, integer precision);
conststring8 Melder8_percent (double value, integer precision);

/*
	Format an integer as a hexadecimal number. If precision is 4, you get things like "0000" or "1A3C" or "107FFFF".
*/
conststring8 Melder8_hexadecimal (integer value, integer precision);
conststring32 Melder_hexadecimal (integer value, integer precision);

/*
	Format a dcomplex value as "--undefined--" or something in the "%.15g", "%.16g", or "%.17g" formats,
	separated without spaces by "+" or "-" and followed by "i".
*/
conststring32 Melder_dcomplex (dcomplex value);
conststring8 Melder8_dcomplex (dcomplex value);

/*
	Format a dcomplex value as "--undefined--" or something in the "%.9g" format,
	separated without spaces by "+" or "-" and followed by "i".
*/
conststring32 Melder_scomplex (dcomplex value);
conststring8 Melder8_scomplex (dcomplex value);

/*
	Convert a formatted floating-point string to something suitable for visualization with the Graphics library.
	For instance, "1e+4" is turned into "10^^4", and "-1.23456e-78" is turned into "-1.23456\.c10^^-78".
*/
conststring32 Melder_float (conststring32 number);

/*
	Format the number that is specified by its natural logarithm.
	For instance, -10000 is formatted as "1.135483865315339e-4343", which is a floating-point representation of exp(-10000).
*/
conststring32 Melder_naturalLogarithm (double lnNumber);
conststring8 Melder8_naturalLogarithm (double lnNumber);

conststring32 Melder_pointer (const void *pointer);
conststring8 Melder8_pointer (const void *pointer);

conststring32 Melder_character (char32 kar);
conststring8 Melder8_character (char32 kar);

conststring32 Melder_colour (MelderColour colour);
conststring8 Melder8_colour (MelderColour colour);

/*
	Append padding to the left of `string` until the result is at least as long as `width`.
	No truncation takes place, so the resulting string may be longer than `width`.
	The left edge of `pad` is left-aligned to the resulting string,
	so that the result will look good if padding is used for visualizing columns.
*/
conststring32 Melder_padLeft (conststring32 string, integer width, conststring32 pad = U" ");

/*
	Append padding to the right of `string` until the result is at least as long as `width`.
	No truncation takes place, so the resulting string may be longer than `width`.
	The right edge of `pad` is right-aligned to the resulting string,
	so that the result will look good if padding is used for visualizing columns.
*/
conststring32 Melder_padRight (conststring32 string, integer width, conststring32 pad = U" ");

/*
	Cut away the left of `string` until the result is at least as short as `width` (which must be non-negative).
	No padding takes place, so the resulting string may be shorter than `width`.
*/
conststring32 Melder_truncateLeft (conststring32 string, integer width);

/*
	Cut away the right of `string` until the result is at least as short as `width` (which must be non-negative).
	No padding takes place, so the resulting string may be shorter than `width`.
*/
conststring32 Melder_truncateRight (conststring32 string, integer width);

/*
	Cut away, or prepend padding to, the left of 'string' until the length of the result is 'width' (which must be non-negative).
	The resulting string will always have `width` characters.
	The left edge of `pad` is left-aligned to the resulting string,
	so that the result will look good if padding is used for visualizing columns.
*/
conststring32 Melder_padOrTruncateLeft (conststring32 string, integer width, conststring32 pad = U" ");

/*
	Cut away, or append padding to, the right of 'string' until the length of the result is 'width' (which must be non-negative).
	The resulting string will always have `width` characters.
	The right edge of `pad` is right-aligned to the resulting string,
	so that the result will look good if padding is used for visualizing columns.
*/
conststring32 Melder_padOrTruncateRight (conststring32 string, integer width, conststring32 pad = U" ");

/* End of file melder_ftoa.h */
#endif
