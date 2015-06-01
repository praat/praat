/* melder_strings.cpp
 *
 * Copyright (C) 2006-2011,2015 Paul Boersma
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

#include "melder.h"
#include "UnicodeData.h"
#define my  me ->
#define FREE_THRESHOLD_BYTES 10000LL

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0, totalDeallocationSize = 0;

void MelderString_free (MelderString *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	if (Melder_debug == 34) fprintf (stderr, "from MelderString_free\t%p\t%lld\t%d\n", my string, (long long) my bufferSize, (int) sizeof (wchar_t));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64_t) sizeof (wchar_t);
	my bufferSize = 0;
	my length = 0;
}

void MelderString16_free (MelderString16 *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	if (Melder_debug == 34) fprintf (stderr, "from MelderString16_free\t%p\t%lld\t%d\n", my string, (long long) my bufferSize, 2);
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64_t) sizeof (char16_t);
	my bufferSize = 0;
	my length = 0;
}

void MelderString32_free (MelderString32 *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	if (Melder_debug == 34) fprintf (stderr, "from MelderString32_free\t%p\t%lld\t%d\n", my string, (long long) my bufferSize, 2);
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64_t) sizeof (char32);
	my bufferSize = 0;
	my length = 0;
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
			if (Melder_debug == 34) fprintf (stderr, "from MelderString:expandIfNecessary\t%p\t%lld\t%d\n", my string, (long long) sizeNeeded, (int) sizeof (type)); \
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

void MelderString_empty (MelderString *me) {
	if (my bufferSize * (int64) sizeof (wchar) >= FREE_THRESHOLD_BYTES) {
		MelderString_free (me);
	}
	int64 sizeNeeded = 1;
	expandIfNecessary (wchar)
	my string [0] = '\0';
	my length = 0;
}

void MelderString16_empty (MelderString16 *me) {
	if (my bufferSize * (int64) sizeof (char16) >= FREE_THRESHOLD_BYTES) {
		MelderString16_free (me);
	}
	int64_t sizeNeeded = 1;
	expandIfNecessary (char16)
	my string [0] = '\0';
	my length = 0;
}

void MelderString32_empty (MelderString32 *me) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES) {
		MelderString32_free (me);
	}
	int64_t sizeNeeded = 1;
	expandIfNecessary (char32)
	my string [0] = '\0';
	my length = 0;
}

void MelderString_copy (MelderString *me, const wchar_t *source) {
	if (my bufferSize * (int64) sizeof (wchar) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (source == NULL) source = L"";
	int64 length = (int64) wcslen (source);
	int64 sizeNeeded = length + 1;
	expandIfNecessary (wchar)
	wcscpy (my string, source);
	my length = length;
}

void MelderString32_copy (MelderString32 *me, const char32 *source) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString32_free (me);
	if (source == NULL) source = U"";
	int64 length = (int64) str32len (source);
	int64 sizeNeeded = length + 1;
	expandIfNecessary (char32)
	str32cpy (my string, source);
	my length = length;
}

void MelderString_ncopy (MelderString *me, const wchar_t *source, int64 n) {
	if (my bufferSize * (int64) sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (source == NULL) source = L"";
	int64 length = (int64) wcslen (source);
	if (length > n) length = n;
	int64 sizeNeeded = length + 1;
	expandIfNecessary (wchar)
	wcsncpy (my string, source, (size_t) length);
	my string [length] = '\0';
	my length = length;
}

void MelderString32_ncopy (MelderString32 *me, const char32 *source, int64 n) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString32_free (me);
	if (source == NULL) source = U"";
	int64 length = (int64) str32len (source);
	if (length > n) length = n;
	int64 sizeNeeded = length + 1;
	expandIfNecessary (char32)
	str32ncpy (my string, source, (size_t) length);
	my string [length] = '\0';
	my length = length;
}

void MelderString_append (MelderString *me, const wchar_t *s1) {
	if (s1 == NULL) s1 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 sizeNeeded = my length + length1 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
}

void MelderString32_append (MelderString32 *me, const char32 *s1) {
	if (s1 == NULL) s1 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 sizeNeeded = my length + length1 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 sizeNeeded = my length + length1 + length2 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2) {
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 sizeNeeded = my length + length1 + length2 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 sizeNeeded = my length + length1 + length2 + length3 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3) {
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 sizeNeeded = my length + length1 + length2 + length3 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4) {
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 length5 = (int64) wcslen (s5);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
	wcscpy (my string + my length, s5);
	my length += length5;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4,
	const char32 *s5)
{
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	if (s5 == NULL) s5 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 length5 = (int64) str32len (s5);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
	str32cpy (my string + my length, s5);
	my length += length5;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	if (s6 == NULL) s6 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 length5 = (int64) wcslen (s5);
	int64 length6 = (int64) wcslen (s6);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
	wcscpy (my string + my length, s5);
	my length += length5;
	wcscpy (my string + my length, s6);
	my length += length6;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4,
	const char32 *s5, const char32 *s6)
{
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	if (s5 == NULL) s5 = U"";
	if (s6 == NULL) s6 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 length5 = (int64) str32len (s5);
	int64 length6 = (int64) str32len (s6);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
	str32cpy (my string + my length, s5);
	my length += length5;
	str32cpy (my string + my length, s6);
	my length += length6;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	if (s6 == NULL) s6 = L"";
	if (s7 == NULL) s7 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 length5 = (int64) wcslen (s5);
	int64 length6 = (int64) wcslen (s6);
	int64 length7 = (int64) wcslen (s7);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
	wcscpy (my string + my length, s5);
	my length += length5;
	wcscpy (my string + my length, s6);
	my length += length6;
	wcscpy (my string + my length, s7);
	my length += length7;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4,
	const char32 *s5, const char32 *s6, const char32 *s7)
{
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	if (s5 == NULL) s5 = U"";
	if (s6 == NULL) s6 = U"";
	if (s7 == NULL) s7 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 length5 = (int64) str32len (s5);
	int64 length6 = (int64) str32len (s6);
	int64 length7 = (int64) str32len (s7);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
	str32cpy (my string + my length, s5);
	my length += length5;
	str32cpy (my string + my length, s6);
	my length += length6;
	str32cpy (my string + my length, s7);
	my length += length7;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	if (s6 == NULL) s6 = L"";
	if (s7 == NULL) s7 = L"";
	if (s8 == NULL) s8 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 length5 = (int64) wcslen (s5);
	int64 length6 = (int64) wcslen (s6);
	int64 length7 = (int64) wcslen (s7);
	int64 length8 = (int64) wcslen (s8);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
	wcscpy (my string + my length, s5);
	my length += length5;
	wcscpy (my string + my length, s6);
	my length += length6;
	wcscpy (my string + my length, s7);
	my length += length7;
	wcscpy (my string + my length, s8);
	my length += length8;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4,
	const char32 *s5, const char32 *s6, const char32 *s7, const char32 *s8)
{
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	if (s5 == NULL) s5 = U"";
	if (s6 == NULL) s6 = U"";
	if (s7 == NULL) s7 = U"";
	if (s8 == NULL) s8 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 length5 = (int64) str32len (s5);
	int64 length6 = (int64) str32len (s6);
	int64 length7 = (int64) str32len (s7);
	int64 length8 = (int64) str32len (s8);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
	str32cpy (my string + my length, s5);
	my length += length5;
	str32cpy (my string + my length, s6);
	my length += length6;
	str32cpy (my string + my length, s7);
	my length += length7;
	str32cpy (my string + my length, s8);
	my length += length8;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	if (s6 == NULL) s6 = L"";
	if (s7 == NULL) s7 = L"";
	if (s8 == NULL) s8 = L"";
	if (s9 == NULL) s9 = L"";
	int64 length1 = (int64) wcslen (s1);
	int64 length2 = (int64) wcslen (s2);
	int64 length3 = (int64) wcslen (s3);
	int64 length4 = (int64) wcslen (s4);
	int64 length5 = (int64) wcslen (s5);
	int64 length6 = (int64) wcslen (s6);
	int64 length7 = (int64) wcslen (s7);
	int64 length8 = (int64) wcslen (s8);
	int64 length9 = (int64) wcslen (s9);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9 + 1;
	expandIfNecessary (wchar)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
	my length += length4;
	wcscpy (my string + my length, s5);
	my length += length5;
	wcscpy (my string + my length, s6);
	my length += length6;
	wcscpy (my string + my length, s7);
	my length += length7;
	wcscpy (my string + my length, s8);
	my length += length8;
	wcscpy (my string + my length, s9);
	my length += length9;
}

void MelderString32_append (MelderString32 *me, const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4,
	const char32 *s5, const char32 *s6, const char32 *s7, const char32 *s8, const char32 *s9)
{
	if (s1 == NULL) s1 = U"";
	if (s2 == NULL) s2 = U"";
	if (s3 == NULL) s3 = U"";
	if (s4 == NULL) s4 = U"";
	if (s5 == NULL) s5 = U"";
	if (s6 == NULL) s6 = U"";
	if (s7 == NULL) s7 = U"";
	if (s8 == NULL) s8 = U"";
	if (s9 == NULL) s9 = U"";
	int64 length1 = (int64) str32len (s1);
	int64 length2 = (int64) str32len (s2);
	int64 length3 = (int64) str32len (s3);
	int64 length4 = (int64) str32len (s4);
	int64 length5 = (int64) str32len (s5);
	int64 length6 = (int64) str32len (s6);
	int64 length7 = (int64) str32len (s7);
	int64 length8 = (int64) str32len (s8);
	int64 length9 = (int64) str32len (s9);
	int64 sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9 + 1;
	expandIfNecessary (char32)
	str32cpy (my string + my length, s1);
	my length += length1;
	str32cpy (my string + my length, s2);
	my length += length2;
	str32cpy (my string + my length, s3);
	my length += length3;
	str32cpy (my string + my length, s4);
	my length += length4;
	str32cpy (my string + my length, s5);
	my length += length5;
	str32cpy (my string + my length, s6);
	my length += length6;
	str32cpy (my string + my length, s7);
	my length += length7;
	str32cpy (my string + my length, s8);
	my length += length8;
	str32cpy (my string + my length, s9);
	my length += length9;
}

void MelderString_appendCharacter (MelderString *me, wchar_t character) {
	int64 sizeNeeded = my length + 2;   // make room for character and null byte
	expandIfNecessary (wchar)
	my string [my length] = character;
	my length ++;
	my string [my length] = L'\0';
}

void MelderString16_appendCharacter (MelderString16 *me, wchar_t character) {
	int64 sizeNeeded = my length + 3;   // make room for character, potential surrogate character, and null character
	expandIfNecessary (char16)
	if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
		my string [my length] = (char16) character;   // guarded cast
		my length ++;
	} else {   // wchar_t is UTF-32.
		char32 kar = (char32) character;
		if (kar <= 0x00FFFF) {
			my string [my length] = (char16) character;   // guarded cast
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
	}
	my string [my length] = '\0';
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

void MelderString32_appendCharacter (MelderString32 *me, char32 character) {
	int64 sizeNeeded = my length + 2;   // make room for character and null character
	expandIfNecessary (char32)
	my string [my length] = character;
	my length ++;
	my string [my length] = U'\0';
}

void MelderString_get (MelderString *me, wchar_t *destination) {
	if (my string) {
		wcscpy (destination, my string);
	} else {
		destination [0] = L'\0';
	}
}

void MelderString32_get (MelderString32 *me, char32 *destination) {
	if (my string) {
		str32cpy (destination, my string);
	} else {
		destination [0] = U'\0';
	}
}

double MelderString_allocationCount (void) {
	return totalNumberOfAllocations;
}

double MelderString_deallocationCount (void) {
	return totalNumberOfDeallocations;
}

double MelderString_allocationSize (void) {
	return totalAllocationSize;
}

double MelderString_deallocationSize (void) {
	return totalDeallocationSize;
}

#define NUMBER_OF_BUFFERS  33
static MelderString32 buffer32 [NUMBER_OF_BUFFERS] = { { 0 } };
static int ibuffer32 = 0;

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4, s5);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4, s5, s6);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6, const char32 *s7) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4, s5, s6, s7);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6, const char32 *s7, const char32 *s8) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4, s5, s6, s7, s8);
	return buffer32 [ibuffer32].string;
}

const char32 * Melder_str32cat (const char32 *s1, const char32 *s2, const char32 *s3, const char32 *s4, const char32 *s5, const char32 *s6, const char32 *s7, const char32 *s8, const char32 *s9) {
	if (++ ibuffer32 == NUMBER_OF_BUFFERS) ibuffer32 = 0;
	MelderString32_empty (& buffer32 [ibuffer32]);
	MelderString32_append (& buffer32 [ibuffer32], s1, s2, s3, s4, s5, s6, s7, s8, s9);
	return buffer32 [ibuffer32].string;
}

static MelderString buffer [NUMBER_OF_BUFFERS] = { { 0 } };
static int ibuffer = 0;

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4, s5);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4, s5, s6);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4, s5, s6, s7);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4, s5, s6, s7, s8);
	return buffer [ibuffer].string;
}

const wchar_t * Melder_wcscat (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	if (++ ibuffer == NUMBER_OF_BUFFERS) ibuffer = 0;
	MelderString_empty (& buffer [ibuffer]);
	MelderString_append (& buffer [ibuffer], s1, s2, s3, s4, s5, s6, s7, s8, s9);
	return buffer [ibuffer].string;
}

/* End of file melder_strings.cpp */
