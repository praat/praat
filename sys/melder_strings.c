/* melder_strings.c
 *
 * Copyright (C) 2006 Paul Boersma
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
 */

#include "melder.h"
#define my  me ->

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0;

void MelderStringA_free (MelderStringA *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	totalNumberOfDeallocations += 1;
	my bufferSize = 0;
	my length = 0;
}

void MelderStringW_free (MelderStringW *me) {
	if (my string == NULL) return;
	Melder_free (my string);
	totalNumberOfDeallocations += 1;
	my bufferSize = 0;
	my length = 0;
}

void MelderStringA_empty (MelderStringA *me) {
	if (my string) {
		my string [0] = '\0';   // Optimization.
		my length = 0;
	} else {
		MelderStringA_copyA (me, "");
	}
}

void MelderStringW_empty (MelderStringW *me) {
	if (my string) {
		my string [0] = L'\0';   // Optimization.
		my length = 0;
	} else {
		MelderStringW_copyW (me, L"");
	}
}

#define expandIfNecessary(type) \
	if (sizeNeeded > my bufferSize) { \
		sizeNeeded = 1.618034 * sizeNeeded + 100; \
		if (my string) totalNumberOfDeallocations += 1; \
		my string = Melder_realloc (my string, sizeNeeded * sizeof (type)); cherror \
		totalNumberOfAllocations += 1; \
		totalAllocationSize += (sizeNeeded - my bufferSize) * sizeof (type); \
		my bufferSize = sizeNeeded; \
	}

bool MelderStringA_copyA (MelderStringA *me, const char *source) {
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

wchar_t * Melder_peekAsciiToWcs (const char *textA) {
	static MelderStringW buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringW_copyA (& buffers [ibuffer], textA);
	return buffers [ibuffer]. string;
}

char * Melder_peekWcsToAscii (const wchar_t *textW) {
	static MelderStringA buffers [11] = { { 0 } };
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderStringA_copyW (& buffers [ibuffer], textW);
	return buffers [ibuffer]. string;
}

/* End of file melder_strings.c */

