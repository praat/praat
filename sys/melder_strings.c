/* melder_strings.c
 *
 * Copyright (C) 2006-2007 Paul Boersma
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
 * pb 2006/12/10
 * pb 2007/06/02 utf8 <-> wcs
 * pb 2007/08/07 MelderUtf16
 */

#include "melder.h"
#define my  me ->
#define FREE_THRESHOLD_BYTES 10000

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0, totalDeallocationSize = 0;

void MelderStringA_free (MelderStringA *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * sizeof (char);
	my bufferSize = 0;
	my length = 0;
}

void MelderStringW_free (MelderStringW *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * sizeof (wchar_t);
	my bufferSize = 0;
	my length = 0;
}

void MelderString16_free (MelderString16 *me) {
	if (my string == NULL) return;
	Melder_free (my string);
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
		my string = Melder_realloc (my string, bytesNeeded); \
		if (my string == NULL) { my bufferSize = 0; goto end; } \
		totalNumberOfAllocations += 1; \
		totalAllocationSize += bytesNeeded; \
		my bufferSize = sizeNeeded; \
	}

void MelderStringA_empty (MelderStringA *me) {
	if (my bufferSize * sizeof (char) >= FREE_THRESHOLD_BYTES) {
		MelderStringA_free (me);
	}
	unsigned long sizeNeeded = 1;
	expandIfNecessary (char)
	my string [0] = '\0';
end:
	my length = 0;
}

void MelderStringW_empty (MelderStringW *me) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES) {
		MelderStringW_free (me);
	}
	unsigned long sizeNeeded = 1;
	expandIfNecessary (wchar_t)
	my string [0] = '\0';
end:
	my length = 0;
}

void MelderString16_empty (MelderString16 *me) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES) {
		MelderString16_free (me);
	}
	unsigned long sizeNeeded = 1;
	expandIfNecessary (MelderUtf16)
	my string [0] = '\0';
end:
	my length = 0;
}

bool MelderStringA_copyA (MelderStringA *me, const char *source) {
	if (my bufferSize * sizeof (char) >= FREE_THRESHOLD_BYTES)
		MelderStringA_free (me);
	if (source == NULL) source = "";
	unsigned long length = strlen (source);
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (char)
	strcpy (my string, source);
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringW_copyA (MelderStringW *me, const char *source) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderStringW_free (me);
	if (source == NULL) source = "";
	unsigned long length = strlen (source);
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (wchar_t)
	const char *from = & source [0];
	wchar_t *to = & my string [0];
	for (; *from != '\0'; from ++, to ++) { *to = (unsigned char) *from; } *to = L'\0';
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringA_copyW (MelderStringA *me, const wchar_t *source) {
	if (my bufferSize * sizeof (char) >= FREE_THRESHOLD_BYTES)
		MelderStringA_free (me);
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (char)
	const wchar_t *from = & source [0];
	char *to = & my string [0];
	for (; *from != L'\0'; from ++, to ++) { *to = *from; /* Truncate */ } *to = '\0';
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringW_copyW (MelderStringW *me, const wchar_t *source) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderStringW_free (me);
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string, source);
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringA_ncopyA (MelderStringA *me, const char *source, unsigned long n) {
	if (my bufferSize * sizeof (char) >= FREE_THRESHOLD_BYTES)
		MelderStringA_free (me);
	if (source == NULL) source = "";
	unsigned long length = strlen (source);
	if (length > n) length = n;
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (char)
	strncpy (my string, source, length);
	my string [length] = '\0';
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringW_ncopyW (MelderStringW *me, const wchar_t *source, unsigned long n) {
	if (my bufferSize * sizeof (wchar_t) >= FREE_THRESHOLD_BYTES)
		MelderStringW_free (me);
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	if (length > n) length = n;
	unsigned long sizeNeeded = length + 1;
	expandIfNecessary (wchar_t)
	wcsncpy (my string, source, length);
	my string [length] = '\0';
	my length = length;
end:
	iferror return false;
	return true;
}

bool MelderStringA_appendA (MelderStringA *me, const char *source) {
	if (source == NULL) source = "";
	unsigned long length = strlen (source);
	unsigned long sizeNeeded = my length + length + 1;
	expandIfNecessary (char)
	strcpy (my string + my length, source);
	my length += length;
end:
	iferror return false;
	return true;
}

bool MelderStringW_appendW (MelderStringW *me, const wchar_t *source) {
	if (source == NULL) source = L"";
	unsigned long length = wcslen (source);
	unsigned long sizeNeeded = my length + length + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, source);
	my length += length;
end:
	iferror return false;
	return true;
}

bool MelderStringW_append1 (MelderStringW *me, const wchar_t *s1) {
	if (s1 == NULL) s1 = L"";
	unsigned long length1 = wcslen (s1);
	unsigned long sizeNeeded = my length + length1 + 1;
	expandIfNecessary (wchar_t)
	wcscpy (my string + my length, s1);
	my length += length1;
end:
	iferror return false;
	return true;
}

bool MelderStringW_append2 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2) {
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append3 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append4 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append5 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append6 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append7 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append8 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
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
end:
	iferror return false;
	return true;
}

bool MelderStringW_append9 (MelderStringW *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
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
end:
	iferror return false;
	return true;
}

bool MelderStringA_appendCharacter (MelderStringA *me, char character) {
	unsigned long sizeNeeded = my length + 1;
	expandIfNecessary (char)
	my string [my length] = character;
	my length ++;
	my string [my length] = '\0';
end:
	iferror return false;
	return true;
}

bool MelderStringW_appendCharacter (MelderStringW *me, wchar_t character) {
	unsigned long sizeNeeded = my length + 1;
	expandIfNecessary (wchar_t)
	my string [my length] = character;
	my length ++;
	my string [my length] = L'\0';
end:
	iferror return false;
	return true;
}

bool MelderString16_appendCharacter (MelderString16 *me, wchar_t character) {
	unsigned long sizeNeeded = my length + 2;   // Allow for surrogate characters.
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
			my string [my length] = 0xDC00 | (kar & 0x3ff);
			my length ++;
		} else {
			my string [my length] = '?';
			my length ++;
		}
	}
	my string [my length] = '\0';
end:
	iferror return false;
	return true;
}

bool MelderStringW_appendA (MelderStringW *me, const char *source) {
	if (source == NULL) source = "";
	unsigned long length = strlen (source);
	unsigned long sizeNeeded = my length + length + 1;
	expandIfNecessary (wchar_t)
	const char *from = & source [0];
	wchar_t *to = & my string [my length];
	for (; *from != '\0'; from ++, to ++) { *to = (unsigned char) *from; } *to = L'\0';
	my length += length;
end:
	iferror return false;
	return true;
}

bool MelderStringA_getA (MelderStringA *me, char *destination) {
	if (my string) {
		strcpy (destination, my string);
	} else {
		destination [0] = '\0';
	}
	return true;
}

bool MelderStringW_getA (MelderStringW *me, char *destination) {
	if (my string) {
		wchar_t *from = & my string [0];
		char *to = & destination [0];
		for (; *from != L'\0'; from ++, to ++) {
			*to = *from;
		}
		*to = '\0';
	} else {
		destination [0] = '\0';
	}
	return true;
}

bool MelderStringW_getW (MelderStringW *me, wchar_t *destination) {
	if (my string) {
		wcscpy (destination, my string);
	} else {
		destination [0] = L'\0';
	}
	return true;
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

wchar_t * MelderReadString_readLine (MelderReadString *text) {
	if (text -> readPointer == NULL) return NULL;
	if (*text -> readPointer == '\0') {   // Tried to read past end of file.
		text -> readPointer = NULL;
		return NULL;
	}
	wchar_t *result = text -> readPointer;
	wchar_t *newline = wcschr (result, '\n');
	if (newline != NULL) {
		*newline = '\0';
		text -> readPointer = newline + 1;
	} else {
		text -> readPointer += wcslen (result);
	}
	return result;
}

const wchar_t * MelderReadString_getLineNumber (MelderReadString *text) {
	wchar_t *p = text -> string;
	long result = 1;
	while (text -> readPointer - p > 0) {
		if (*p == '\0' || *p == '\n') result ++;
		p ++;
	}
	return Melder_integer (result);
}

/* End of file melder_strings.c */
