/* Praat_tests_enums.h
 *
 * Copyright (C) 2001-2012,2015,2016 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kPraatTests, 0)
	enums_add (kPraatTests, 0, _, U"_")
	enums_add (kPraatTests, 1, CHECK_RANDOM_1009_2009, U"CheckRandom1009_2009")
	enums_add (kPraatTests, 2, TIME_RANDOM_FRACTION, U"TimeRandomFraction")
	enums_add (kPraatTests, 3, TIME_RANDOM_GAUSS, U"TimeRandomGauss")
	enums_add (kPraatTests, 4, TIME_SORT, U"TimeSort")
	enums_add (kPraatTests, 5, TIME_INTEGER, U"TimeInteger")
	enums_add (kPraatTests, 6, TIME_FLOAT, U"TimeFloat")
	enums_add (kPraatTests, 7, TIME_FLOAT_TO_UNSIGNED_BUILTIN, U"TimeFloatToUnsigned_builtin")
	enums_add (kPraatTests, 8, TIME_FLOAT_TO_UNSIGNED_EXTERN, U"TimeFloatToUnsigned_extern")
	enums_add (kPraatTests, 9, TIME_UNSIGNED_TO_FLOAT_BUILTIN, U"TimeUnsignedToFloat_builtin")
	enums_add (kPraatTests, 10, TIME_UNSIGNED_TO_FLOAT_EXTERN, U"TimeUnsignedToFloat_extern")
	enums_add (kPraatTests, 11, TIME_STRING_MELDER_32, U"TimeStringMelder32")
	enums_add (kPraatTests, 12, TIME_STRING_MELDER_32_ALLOC, U"TimeStringMelder32_alloc")
	enums_add (kPraatTests, 13, TIME_STRING_CPP_S, U"TimeStringC++S")
	enums_add (kPraatTests, 14, TIME_STRING_CPP_C, U"TimeStringC++C")
	enums_add (kPraatTests, 15, TIME_STRING_CPP_WS, U"TimeStringC++WS")
	enums_add (kPraatTests, 16, TIME_STRING_CPP_WC, U"TimeStringC++WC")
	enums_add (kPraatTests, 17, TIME_STRING_CPP_32, U"TimeStringC++32")
	enums_add (kPraatTests, 18, TIME_STRING_CPP_U32STRING, U"TimeStringC++U32STRING")
	enums_add (kPraatTests, 19, TIME_STRCPY, U"TimeStrcpy")
	enums_add (kPraatTests, 20, TIME_WCSCPY, U"TimeWcscpy")
	enums_add (kPraatTests, 21, TIME_STR32CPY, U"TimeStr32cpy")
	enums_add (kPraatTests, 22, TIME_GRAPHICS_TEXT_TOP, U"TimeGraphicsTextTop")
	enums_add (kPraatTests, 23, THING_AUTO, U"ThingAuto")
enums_end (kPraatTests, 23, CHECK_RANDOM_1009_2009)

/* End of file Praat_tests_enums.h */
