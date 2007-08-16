/* melder_alloc.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2006/12/10 separated from melder.c
 * pb 2007/05/24 wcsdup, wcsToAscii, asciiToWcs
 * pb 2007/08/14 underscores for names _Melder_malloc and _Melder_calloc
 */

#include "melder.h"

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0;

#define TRACE_MALLOC  0

void * _Melder_malloc (unsigned long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_malloc:) Can never allocate %ld bytes.", size);
	result = malloc (size);
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room for another %ld bytes.", size);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	#if TRACE_MALLOC
		Melder_casual ("malloc %ld", size);
	#endif
	return result;
}

void _Melder_free (void **ptr) {
	if (*ptr == NULL) return;
	free (*ptr);
	*ptr = NULL;
	totalNumberOfDeallocations += 1;
	#if TRACE_MALLOC
		Melder_casual ("free");
	#endif
}

void * Melder_realloc (void *ptr, long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_realloc:) Can never allocate %ld bytes.", size);
	result = realloc (ptr, size);   /* Will not show in the statistics... */
	if (result == NULL)
		return Melder_errorp ("Out of memory. Could not extend room to %ld bytes.", size);
	if (ptr == NULL) {   /* Is it like malloc? */
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
	} else if (result != ptr) {   /* Did realloc do a malloc-and-free? */
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
		totalNumberOfDeallocations += 1;
	}
	#if TRACE_MALLOC
		Melder_casual ("realloc %ld", size);
	#endif
	return result;
}

void * _Melder_calloc (long nelem, long elsize) {
	void *result;
	if (nelem <= 0)
		return Melder_errorp ("(Melder_calloc:) "
			"Can never allocate %ld elements.", nelem);
	if (elsize <= 0)
		return Melder_errorp ("(Melder_calloc:) "
			"Can never allocate elements whose size is %ld bytes.", elsize);
	result = calloc (nelem, elsize);
	if (result == NULL)
		return Melder_errorp ("Out of memory: "
			"there is not enough room for %ld more elements whose sizes are %ld bytes each.", nelem, elsize);
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	#if TRACE_MALLOC
		Melder_casual ("calloc %ld %ld", nelem, elsize);
	#endif
	return result;
}

char * Melder_strdup (const char *string) {
	char *result;
	long size;
	if (! string) return NULL;
	size = strlen (string) + 1;
	result = malloc (size * sizeof (char));
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room to duplicate a text of %ld characters.", size - 1);
	strcpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	#if TRACE_MALLOC
		Melder_casual ("strdup %ld", size);
	#endif
	return result;
}

wchar_t * Melder_wcsdup (const wchar_t *string) {
	wchar_t *result;
	long size;
	if (! string) return NULL;
	size = wcslen (string) + 1;
	result = malloc (size * sizeof (wchar_t));
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room to duplicate a text of %ld characters.", size - 1);
	wcscpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	#if TRACE_MALLOC
		Melder_casual ("wcsdup %ld", size);
	#endif
	return result;
}

double Melder_allocationCount (void) {
	return totalNumberOfAllocations;
}

double Melder_deallocationCount (void) {
	return totalNumberOfDeallocations;
}

double Melder_allocationSize (void) {
	return totalAllocationSize;
}

int Melder_strcmp (const char *string1, const char *string2) {
	if (string1 == NULL) string1 = "";
	if (string2 == NULL) string2 = "";
	return strcmp (string1, string2);
}

int Melder_strncmp (const char *string1, const char *string2, unsigned long n) {
	if (string1 == NULL) string1 = "";
	if (string2 == NULL) string2 = "";
	return strncmp (string1, string2, n);
}

int Melder_wcscmp (const wchar_t *string1, const wchar_t *string2) {
	if (string1 == NULL) string1 = L"";
	if (string2 == NULL) string2 = L"";
	return wcscmp (string1, string2);
}

int Melder_wcsncmp (const wchar_t *string1, const wchar_t *string2, unsigned long n) {
	if (string1 == NULL) string1 = L"";
	if (string2 == NULL) string2 = L"";
	return wcsncmp (string1, string2, n);
}

/* End of file melder_alloc.c */
