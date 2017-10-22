/* melder_strings.cpp
 *
 * Copyright (C) 2006-2012,2014-2017 Paul Boersma
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
#include "UnicodeData.h"
#define FREE_THRESHOLD_BYTES 10000LL

static int64 totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0, totalDeallocationSize = 0;

void MelderString16_free (MelderString16 *me) {
	if (! my string) return;
	Melder_free (my string);
	if (Melder_debug == 34)
		Melder_casual (U"from MelderString16_free\t", Melder_pointer (my string), U"\t", my bufferSize, U"\t", sizeof (char16));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64) sizeof (char16_t);
	my bufferSize = 0;
	my length = 0;
}

void MelderString_free (MelderString *me) {
	if (! my string) return;
	Melder_free (my string);
	if (Melder_debug == 34)
		Melder_casual (U"from MelderString32_free\t", Melder_pointer (my string), U"\t", my bufferSize, U"\t", sizeof (char32));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64) sizeof (char32);
	my bufferSize = 0;
	my length = 0;
}

void MelderString_expand (MelderString *me, int64 sizeNeeded) {
	Melder_assert (my bufferSize >= 0);
	Melder_assert (sizeNeeded >= 0);
	sizeNeeded = (int64) (1.618034 * sizeNeeded) + 100;
	Melder_assert (sizeNeeded > 0);
	if (my string) {
		totalNumberOfDeallocations += 1;
		totalDeallocationSize += my bufferSize * (int64) sizeof (char32);
	}
	int64 bytesNeeded = sizeNeeded * (int64) sizeof (char32);
	Melder_assert (bytesNeeded > 0);
	try {
		if (Melder_debug == 34)
			Melder_casual (U"from MelderString:expandIfNecessary\t", Melder_pointer (my string), U"\t", sizeNeeded, U"\t", sizeof (char32));
		my string = (char32 *) Melder_realloc (my string, bytesNeeded);
	} catch (MelderError) {
		my bufferSize = 0;
		my length = 0;
		throw;
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += bytesNeeded;
	my bufferSize = sizeNeeded;
}

#define expandIfNecessary(type) \
	if (sizeNeeded > my bufferSize) { \
		Melder_assert (my bufferSize >= 0); \
		Melder_assert (sizeNeeded >= 0); \
		sizeNeeded = (int64) (1.618034 * sizeNeeded) + 100; \
		Melder_assert (sizeNeeded > 0); \
		if (my string) { \
			totalNumberOfDeallocations += 1; \
			totalDeallocationSize += my bufferSize * (int64) sizeof (type); \
		} \
		int64 bytesNeeded = sizeNeeded * (int64) sizeof (type); \
		Melder_assert (bytesNeeded > 0); \
		try { \
			if (Melder_debug == 34) \
				Melder_casual (U"from MelderString:expandIfNecessary\t", Melder_pointer (my string), U"\t", sizeNeeded, U"\t", sizeof (type)); \
			my string = (type *) Melder_realloc (my string, bytesNeeded); \
		} catch (MelderError) { \
			my bufferSize = 0; \
			my length = 0; \
			throw; \
		} \
		totalNumberOfAllocations += 1; \
		totalAllocationSize += bytesNeeded; \
		my bufferSize = sizeNeeded; \
	}

void MelderString16_empty (MelderString16 *me) {
	if (my bufferSize * (int64) sizeof (char16) >= FREE_THRESHOLD_BYTES) {
		MelderString16_free (me);
	}
	int64 sizeNeeded = 1;
	expandIfNecessary (char16)
	my string [0] = '\0';
	my length = 0;
}

void MelderString_empty (MelderString *me) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) {
		MelderString_free (me);
	}
	int64 sizeNeeded = 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	my string [0] = '\0';
	my length = 0;
}

void MelderString_copy (MelderString *me, Melder_1_ARG) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	int64 sizeNeeded = length1 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
}
void MelderString_copy (MelderString *me, Melder_2_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	int64 sizeNeeded = length1 + length2 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
}
void MelderString_copy (MelderString *me, Melder_3_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	int64 sizeNeeded = length1 + length2 + length3 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
}
void MelderString_copy (MelderString *me, Melder_4_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
}
void MelderString_copy (MelderString *me, Melder_5_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
}
void MelderString_copy (MelderString *me, Melder_6_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
}
void MelderString_copy (MelderString *me, Melder_7_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
}
void MelderString_copy (MelderString *me, Melder_8_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
}
void MelderString_copy (MelderString *me, Melder_9_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
}
void MelderString_copy (MelderString *me, Melder_10_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
}
void MelderString_copy (MelderString *me, Melder_11_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
}
void MelderString_copy (MelderString *me, Melder_13_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
}
void MelderString_copy (MelderString *me, Melder_15_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
	str32cpy (my string + my length, s14);  my length += length14;
	str32cpy (my string + my length, s15);  my length += length15;
}
void MelderString_copy (MelderString *me, Melder_19_ARGS) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) MelderString_free (me);
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	const char32 *s16 = arg16._arg ? arg16._arg : U"";  int64 length16 = str32len (s16);
	const char32 *s17 = arg17._arg ? arg17._arg : U"";  int64 length17 = str32len (s17);
	const char32 *s18 = arg18._arg ? arg18._arg : U"";  int64 length18 = str32len (s18);
	const char32 *s19 = arg19._arg ? arg19._arg : U"";  int64 length19 = str32len (s19);
	int64 sizeNeeded = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15 + length16 + length17 + length18 + length19 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string, s1);  my length = length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
	str32cpy (my string + my length, s14);  my length += length14;
	str32cpy (my string + my length, s15);  my length += length15;
	str32cpy (my string + my length, s16);  my length += length16;
	str32cpy (my string + my length, s17);  my length += length17;
	str32cpy (my string + my length, s18);  my length += length18;
	str32cpy (my string + my length, s19);  my length += length19;
}

void MelderString_ncopy (MelderString *me, const char32 *source, int64 n) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (! source) source = U"";
	int64 length = str32len (source);
	if (length > n) length = n;
	int64 sizeNeeded = length + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32ncpy (my string, source, length);
	my string [length] = '\0';
	my length = length;
}

/*void MelderString_append (MelderString *me, Melder_1_ARG) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = _str32len (s1);
	int64 sizeNeeded = my length + length1 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	_str32cpy (my string + my length, s1);   my length += length1;
}*/
void MelderString_append (MelderString *me, Melder_2_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	int64 sizeNeeded = my length + length1 + length2 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
}
void MelderString_append (MelderString *me, Melder_3_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	int64 sizeNeeded = my length + length1 + length2 + length3 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
}
void MelderString_append (MelderString *me, Melder_4_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
}
void MelderString_append (MelderString *me, Melder_5_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
}
void MelderString_append (MelderString *me, Melder_6_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
}
void MelderString_append (MelderString *me, Melder_7_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
}
void MelderString_append (MelderString *me, Melder_8_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
}
void MelderString_append (MelderString *me, Melder_9_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
}
void MelderString_append (MelderString *me, Melder_10_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
}
void MelderString_append (MelderString *me, Melder_11_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
}
void MelderString_append (MelderString *me, Melder_13_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
}
void MelderString_append (MelderString *me, Melder_15_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
	str32cpy (my string + my length, s14);  my length += length14;
	str32cpy (my string + my length, s15);  my length += length15;
}
void MelderString_append (MelderString *me, Melder_19_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	const char32 *s16 = arg16._arg ? arg16._arg : U"";  int64 length16 = str32len (s16);
	const char32 *s17 = arg17._arg ? arg17._arg : U"";  int64 length17 = str32len (s17);
	const char32 *s18 = arg18._arg ? arg18._arg : U"";  int64 length18 = str32len (s18);
	const char32 *s19 = arg19._arg ? arg19._arg : U"";  int64 length19 = str32len (s19);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15 + length16 + length17 + length18 + length19 + 1;
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	str32cpy (my string + my length, s1);   my length += length1;
	str32cpy (my string + my length, s2);   my length += length2;
	str32cpy (my string + my length, s3);   my length += length3;
	str32cpy (my string + my length, s4);   my length += length4;
	str32cpy (my string + my length, s5);   my length += length5;
	str32cpy (my string + my length, s6);   my length += length6;
	str32cpy (my string + my length, s7);   my length += length7;
	str32cpy (my string + my length, s8);   my length += length8;
	str32cpy (my string + my length, s9);   my length += length9;
	str32cpy (my string + my length, s10);  my length += length10;
	str32cpy (my string + my length, s11);  my length += length11;
	str32cpy (my string + my length, s12);  my length += length12;
	str32cpy (my string + my length, s13);  my length += length13;
	str32cpy (my string + my length, s14);  my length += length14;
	str32cpy (my string + my length, s15);  my length += length15;
	str32cpy (my string + my length, s16);  my length += length16;
	str32cpy (my string + my length, s17);  my length += length17;
	str32cpy (my string + my length, s18);  my length += length18;
	str32cpy (my string + my length, s19);  my length += length19;
}

void MelderString16_appendCharacter (MelderString16 *me, char32 kar) {
	int64 sizeNeeded = my length + 3;   // make room for character, potential surrogate character, and null character
	expandIfNecessary (char16)
	if (kar <= 0x00FFFF) {
		my string [my length] = (char16) kar;   // guarded cast
		my length ++;
	} else if (kar <= 0x10FFFF) {
		kar -= 0x010000;
		my string [my length] = (char16) (0x00D800 | (kar >> 10));
		my length ++;
		my string [my length] = (char16) (0x00DC00 | (kar & 0x0003FF));
		my length ++;
	} else {
		my string [my length] = UNICODE_REPLACEMENT_CHARACTER;
		my length ++;
	}
	my string [my length] = '\0';
}

void MelderString_appendCharacter (MelderString *me, char32 character) {
	int64 sizeNeeded = my length + 2;   // make room for character and null character
	if (sizeNeeded > my bufferSize) MelderString_expand (me, sizeNeeded);
	my string [my length] = character;
	my length ++;
	my string [my length] = U'\0';
}

void MelderString_get (MelderString *me, char32 *destination) {
	if (my string) {
		str32cpy (destination, my string);
	} else {
		destination [0] = U'\0';
	}
}

int64 MelderString_allocationCount () {
	return totalNumberOfAllocations;
}

int64 MelderString_deallocationCount () {
	return totalNumberOfDeallocations;
}

int64 MelderString_allocationSize () {
	return totalAllocationSize;
}

int64 MelderString_deallocationSize () {
	return totalDeallocationSize;
}

#define NUMBER_OF_CAT_BUFFERS  33
static MelderString theCatBuffers [NUMBER_OF_CAT_BUFFERS] { };
static int iCatBuffer = 0;

const char32 * Melder_cat (Melder_2_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_2_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_3_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_3_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_4_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_4_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_5_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_5_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_6_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_6_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_7_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_7_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_8_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_8_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_9_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_9_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_10_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_10_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_11_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_11_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_13_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_13_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_15_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_15_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}
const char32 * Melder_cat (Melder_19_ARGS) {
	if (++ iCatBuffer == NUMBER_OF_CAT_BUFFERS) iCatBuffer = 0;
	MelderString_copy (& theCatBuffers [iCatBuffer], Melder_19_ARGS_CALL);
	return theCatBuffers [iCatBuffer].string;
}

#define Melder_sprint_HANDLE_OVERFLOW  \
	if (totalLength >= bufferSize) { \
		for (int64 i = 0; i < bufferSize; i ++) \
			buffer [i] = U'?'; \
		if (bufferSize > 0) buffer [bufferSize - 1] = U'\0'; \
		return; \
	}

void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_1_ARG) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";
	int64 totalLength = str32len (s1);
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_2_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";
	int64 totalLength = length1 + str32len (s2);
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);
	str32cpy (buffer + length1, s2);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_3_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	int64 totalLength = length1 + length2 + length3;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_4_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	int64 totalLength = length1 + length2 + length3 + length4;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_5_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	int64 totalLength = length1 + length2 + length3 + length4 + length5;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_6_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_7_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_8_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_9_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_10_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);   length += length9;
	str32cpy (buffer + length, s10);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_11_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);   length += length9;
	str32cpy (buffer + length, s10);  length += length10;
	str32cpy (buffer + length, s11);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_13_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);   length += length9;
	str32cpy (buffer + length, s10);  length += length10;
	str32cpy (buffer + length, s11);  length += length11;
	str32cpy (buffer + length, s12);  length += length12;
	str32cpy (buffer + length, s13);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_15_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);   length += length9;
	str32cpy (buffer + length, s10);  length += length10;
	str32cpy (buffer + length, s11);  length += length11;
	str32cpy (buffer + length, s12);  length += length12;
	str32cpy (buffer + length, s13);  length += length13;
	str32cpy (buffer + length, s14);  length += length14;
	str32cpy (buffer + length, s15);
}
void Melder_sprint (char32 *buffer, int64 bufferSize, Melder_19_ARGS) {
	const char32 *s1  = arg1._arg  ? arg1._arg  : U"";  int64 length1  = str32len (s1);
	const char32 *s2  = arg2._arg  ? arg2._arg  : U"";  int64 length2  = str32len (s2);
	const char32 *s3  = arg3._arg  ? arg3._arg  : U"";  int64 length3  = str32len (s3);
	const char32 *s4  = arg4._arg  ? arg4._arg  : U"";  int64 length4  = str32len (s4);
	const char32 *s5  = arg5._arg  ? arg5._arg  : U"";  int64 length5  = str32len (s5);
	const char32 *s6  = arg6._arg  ? arg6._arg  : U"";  int64 length6  = str32len (s6);
	const char32 *s7  = arg7._arg  ? arg7._arg  : U"";  int64 length7  = str32len (s7);
	const char32 *s8  = arg8._arg  ? arg8._arg  : U"";  int64 length8  = str32len (s8);
	const char32 *s9  = arg9._arg  ? arg9._arg  : U"";  int64 length9  = str32len (s9);
	const char32 *s10 = arg10._arg ? arg10._arg : U"";  int64 length10 = str32len (s10);
	const char32 *s11 = arg11._arg ? arg11._arg : U"";  int64 length11 = str32len (s11);
	const char32 *s12 = arg12._arg ? arg12._arg : U"";  int64 length12 = str32len (s12);
	const char32 *s13 = arg13._arg ? arg13._arg : U"";  int64 length13 = str32len (s13);
	const char32 *s14 = arg14._arg ? arg14._arg : U"";  int64 length14 = str32len (s14);
	const char32 *s15 = arg15._arg ? arg15._arg : U"";  int64 length15 = str32len (s15);
	const char32 *s16 = arg16._arg ? arg16._arg : U"";  int64 length16 = str32len (s16);
	const char32 *s17 = arg17._arg ? arg17._arg : U"";  int64 length17 = str32len (s17);
	const char32 *s18 = arg18._arg ? arg18._arg : U"";  int64 length18 = str32len (s18);
	const char32 *s19 = arg19._arg ? arg19._arg : U"";  int64 length19 = str32len (s19);
	int64 totalLength = length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9
		+ length10 + length11 + length12 + length13 + length14 + length15 + length16 + length17 + length18 + length19;
	Melder_sprint_HANDLE_OVERFLOW
	str32cpy (buffer, s1);  int64 length = length1;
	str32cpy (buffer + length, s2);   length += length2;
	str32cpy (buffer + length, s3);   length += length3;
	str32cpy (buffer + length, s4);   length += length4;
	str32cpy (buffer + length, s5);   length += length5;
	str32cpy (buffer + length, s6);   length += length6;
	str32cpy (buffer + length, s7);   length += length7;
	str32cpy (buffer + length, s8);   length += length8;
	str32cpy (buffer + length, s9);   length += length9;
	str32cpy (buffer + length, s10);  length += length10;
	str32cpy (buffer + length, s11);  length += length11;
	str32cpy (buffer + length, s12);  length += length12;
	str32cpy (buffer + length, s13);  length += length13;
	str32cpy (buffer + length, s14);  length += length14;
	str32cpy (buffer + length, s15);  length += length15;
	str32cpy (buffer + length, s16);  length += length16;
	str32cpy (buffer + length, s17);  length += length17;
	str32cpy (buffer + length, s18);  length += length18;
	str32cpy (buffer + length, s19);
}

/* End of file melder_strings.cpp */
