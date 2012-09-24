/* melder_strings.cpp
 *
 * Copyright (C) 2006-2011 Paul Boersma
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
#define FREE_THRESHOLD_BYTES 10000

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0, totalDeallocationSize = 0;

void MelderString_free (MelderString *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	if (Melder_debug == 34) fprintf (stderr, "from MelderString_free\t%p\t%ld\t%ld\n", my string, my bufferSize, sizeof (wchar_t));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * sizeof (wchar_t);
	my bufferSize = 0;
	my length = 0;
}

void MelderString16_free (MelderString16 *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	if (Melder_debug == 34) fprintf (stderr, "from MelderString16_free\t%p\t%ld\t%ld\n", my string, my bufferSize, 2L);
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * sizeof (MelderUtf16);
	my bufferSize = 0;
	my length = 0;
}

#define expandIfNecessary(type) \
	if (sizeNeeded > my bufferSize) { \
		Melder_assert (my bufferSize >= 0); \
		Melder_assert (sizeNeeded >= 0); \
		sizeNeeded = 1.618034 * sizeNeeded + 100; \
		Melder_assert (sizeNeeded > 0); \
		if (my string) { \
			totalNumberOfDeallocations += 1; \
			totalDeallocationSize += my bufferSize * sizeof (type); \
		} \
		long bytesNeeded = sizeNeeded * sizeof (type); \
		Melder_assert (bytesNeeded > 0); \
		try { \
			if (Melder_debug == 34) fprintf (stderr, "from MelderString:expandIfNecessary\t%p\t%ld\t%ld\n", my string, sizeNeeded, sizeof (type)); \
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
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES) {
		MelderString_free (me);
	}
	unsigned long sizeNeeded = 1;
	expandIfNecessary (wchar_t)
	my string [0] = '\0';
	my length = 0;
}

void MelderString16_empty (MelderString16 *me) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES) {
		MelderString16_free (me);
	}
	unsigned long sizeNeeded = 1;
	expandIfNecessary (MelderUtf16)
	my string [0] = '\0';
	my length = 0;
}

void MelderString_copy (MelderString *me, const wchar_t *source) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string, source);
	my length = length;
}

void MelderString_ncopy (MelderString *me, const wchar_t *source, unsigned long n) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	if (length > n) length = n;
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (wchar_t)
	wcsncpy (my string, source, length);
	my string [length] = '\0';
	my length = length;
}

void MelderString_append (MelderString *me, const wchar_t *s1) {
	if (s1 == NULL) s1 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long sizeNeeded = my length + length1 + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, s1);
	my length += length1;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long sizeNeeded = my length + length1 + length2 + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
}

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, s1);
	my length += length1;
	wcscpy (my string + my length, s2);
	my length += length2;
	wcscpy (my string + my length, s3);
	my length += length3;
	wcscpy (my string + my length, s4);
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
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long length5 = wcslen (s5);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + 1;
	expandIfNecessary (wchar_t)
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

void MelderString_append (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
	const wchar_t *s5, const wchar_t *s6)
{
	if (s1 == NULL) s1 = L"";
	if (s2 == NULL) s2 = L"";
	if (s3 == NULL) s3 = L"";
	if (s4 == NULL) s4 = L"";
	if (s5 == NULL) s5 = L"";
	if (s6 == NULL) s6 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long length5 = wcslen (s5);
	unsigned long length6 = wcslen (s6);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + 1;
	expandIfNecessary (wchar_t)
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
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long length5 = wcslen (s5);
	unsigned long length6 = wcslen (s6);
	unsigned long length7 = wcslen (s7);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + 1;
	expandIfNecessary (wchar_t)
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
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long length5 = wcslen (s5);
	unsigned long length6 = wcslen (s6);
	unsigned long length7 = wcslen (s7);
	unsigned long length8 = wcslen (s8);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + 1;
	expandIfNecessary (wchar_t)
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
	unsigned long length1 = wcslen (s1);
	unsigned long length2 = wcslen (s2);
	unsigned long length3 = wcslen (s3);
	unsigned long length4 = wcslen (s4);
	unsigned long length5 = wcslen (s5);
	unsigned long length6 = wcslen (s6);
	unsigned long length7 = wcslen (s7);
	unsigned long length8 = wcslen (s8);
	unsigned long length9 = wcslen (s9);
	unsigned long sizeNeeded = my length + length1 + length2 + length3 + length4 + length5 + length6 + length7 + length8 + length9 + 1;
	expandIfNecessary (wchar_t)
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

void MelderString_appendCharacter (MelderString *me, wchar_t character) {
	unsigned long sizeNeeded = my length + 2;   // make room for character and null byte
	expandIfNecessary (wchar_t)
	my string [my length] = character;
	my length ++;
	my string [my length] = L'\0';
}

void MelderString16_appendCharacter (MelderString16 *me, wchar_t character) {
	unsigned long sizeNeeded = my length + 3;   // make room for character, potential surrogate character, and null byte
	expandIfNecessary (MelderUtf16)
	if (sizeof (wchar_t) == 2) {   // wchar_t is UTF-16?
		my string [my length] = character;
		my length ++;
	} else {   // wchar_t is UTF-32.
		MelderUtf32 kar = character;
		if (kar <= 0xFFFF) {
			my string [my length] = character;
			my length ++;
		} else if (kar <= 0x10FFFF) {
			kar -= 0x10000;
			my string [my length] = 0xD800 | (kar >> 10);
			my length ++;
			my string [my length] = 0xDC00 | (kar & 0x3FF);
			my length ++;
		} else {
			my string [my length] = UNICODE_REPLACEMENT_CHARACTER;
			my length ++;
		}
	}
	my string [my length] = '\0';
}

void MelderString_get (MelderString *me, wchar_t *destination) {
	if (my string) {
		wcscpy (destination, my string);
	} else {
		destination [0] = L'\0';
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
