/* Praat_tests_enums.h
 *
 * Copyright (C) 2001-2005,2009,2013-2018,2020 Paul Boersma
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
	enums_add (kPraatTests, 23, TIME_UNDEFINED_NUMUNDEFINED, U"TimeUndefinedNUMundefined")
	enums_add (kPraatTests, 24, TIME_UNDEFINED_ISINF_OR_ISNAN, U"TimeUndefinedIsinfOrIsnan")
	enums_add (kPraatTests, 25, TIME_UNDEFINED_0x7FF, U"TimeUndefined0x7FF")
	enums_add (kPraatTests, 26, TIME_INNER, U"TimeInner")
	enums_add (kPraatTests, 27, TIME_OUTER_NUMMAT, U"TimeOuter##")
	enums_add (kPraatTests, 28, CHECK_INVFISHERQ, U"invFisherQ(0.003,1,100000)")
	enums_add (kPraatTests, 29, TIME_AUTOSTRING, U"TimeAutostring")
	enums_add (kPraatTests, 30, TIME_CHAR32, U"TimeChar32")
	enums_add (kPraatTests, 31, TIME_SUM, U"TimeSum")
	enums_add (kPraatTests, 32, TIME_MEAN, U"TimeMean")
	enums_add (kPraatTests, 33, TIME_STDEV, U"TimeStdev")
	enums_add (kPraatTests, 34, TIME_ALLOC, U"TimeAlloc")
	enums_add (kPraatTests, 35, TIME_ALLOC0, U"TimeAlloc0")
	enums_add (kPraatTests, 36, TIME_ZERO, U"TimeZero")
	enums_add (kPraatTests, 37, TIME_MALLOC, U"TimeMalloc")
	enums_add (kPraatTests, 38, TIME_CALLOC, U"TimeCalloc")
	enums_add (kPraatTests, 39, TIME_ADD, U"TimeAdd")
	enums_add (kPraatTests, 40, TIME_SIN, U"TimeSin")
	enums_add (kPraatTests, 41, TIME_VECADD, U"TimeVecAdd")
	enums_add (kPraatTests, 42, TIME_MATMUL, U"TimeMatMul")
	enums_add (kPraatTests, 43, THING_AUTO, U"ThingAuto")
	enums_add (kPraatTests, 44, FILEINMEMORYMANAGER_IO, U"FileInMemoryManager_io")
enums_end (kPraatTests, 45, CHECK_RANDOM_1009_2009)

/* End of file Praat_tests_enums.h */
