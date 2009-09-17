/* melder_alloc.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2007/12/05 Melder_wcsequ_firstCharacterCaseInsensitive
 * pb 2009/03/14 counting reallocs moving and in situ
 * pb 2009/07/31 tracing by Melder_debug 34
 */

#include "melder.h"
#include <wctype.h>
#ifdef macintosh
	#include <execinfo.h>
#endif

static void backTrace (void) {
	#ifdef macintoshxxx
		void *callstack [9];
		int frames = backtrace (callstack, 9);
		char **strs = backtrace_symbols (callstack, frames);
		for (int i = 0; i < frames; i ++) { fprintf (stderr, " %s", strs [i]); }
		free (strs);
 	#endif
}

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0,
	totalNumberOfMovingReallocs = 0, totalNumberOfReallocsInSitu = 0;

void * _Melder_malloc (unsigned long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_malloc:) Can never allocate %ld bytes.", size);
	result = malloc (size);
	if (result == NULL)
		return Melder_errorp ("Out of memory: there is not enough room for another %ld bytes.", size);
	if (Melder_debug == 34) { Melder_casual ("Melder_malloc\t%ld\t%ld\t1", result, size); backTrace (); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void _Melder_free (void **ptr) {
	if (*ptr == NULL) return;
	free (*ptr);
	if (Melder_debug == 34) { Melder_casual ("Melder_free\t%ld\t?\t?", *ptr); backTrace (); }
	*ptr = NULL;
	totalNumberOfDeallocations += 1;
}

void * Melder_realloc (void *ptr, long size) {
	void *result;
	if (size <= 0)
		return Melder_errorp ("(Melder_realloc:) Can never allocate %ld bytes.", size);
	result = realloc (ptr, size);   /* Will not show in the statistics... */
	if (result == NULL)
		return Melder_errorp ("Out of memory. Could not extend room to %ld bytes.", size);
	if (ptr == NULL) {   /* Is it like malloc? */
		if (Melder_debug == 34) { Melder_casual ("Melder_realloc\t%ld\t%ld\t1", result, size); backTrace (); }
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
	} else if (result != ptr) {   /* Did realloc do a malloc-and-free? */
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
		totalNumberOfDeallocations += 1;
		totalNumberOfMovingReallocs += 1;
	} else {
		totalNumberOfReallocsInSitu += 1;
	}
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
	if (Melder_debug == 34) { Melder_casual ("Melder_calloc\t%ld\t%ld\t%ld", result, nelem, elsize); backTrace (); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
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
	if (Melder_debug == 34) { Melder_casual ("Melder_strdup\t%ld\t%ld\t1", result, size); backTrace (); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
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
	if (Melder_debug == 34) { Melder_casual ("Melder_wcsdup\t%ld\t%ld\t4", result, size); backTrace (); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size * sizeof (wchar_t);
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

double Melder_reallocationsInSituCount (void) {
	return totalNumberOfReallocsInSitu;
}

double Melder_movingReallocationsCount (void) {
	return totalNumberOfMovingReallocs;
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

bool Melder_wcsequ_firstCharacterCaseInsensitive (const wchar_t *string1, const wchar_t *string2) {
	if (string1 == NULL) string1 = L"";
	if (string2 == NULL) string2 = L"";
	if (towlower (*string1) != towlower (*string2)) return false;
	if (*string1 == '\0') return true;
	return ! wcscmp (string1 + 1, string2 + 1);
}

wchar_t * Melder_wcstok (wchar_t *string, const wchar_t *delimiter, wchar_t **last) {
	#if defined (__MINGW32__)
		(void) last;
		return wcstok (string, delimiter);
	#else
		return wcstok (string, delimiter, last);
	#endif
}

/* End of file melder_alloc.c */
