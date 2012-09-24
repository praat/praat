/* melder_alloc.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2010/12/28 split into _e and _f versions, and created a rainy-day fund
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include <wctype.h>
#include <assert.h>

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0,
	totalNumberOfMovingReallocs = 0, totalNumberOfReallocsInSitu = 0;

/*
 * The rainy-day fund.
 *
 * Typically, memory allocation for data is entirely checked by using the _e versions of the allocation routines,
 * which will call Melder_error if they are out of memory.
 * When data allocation is indeed out of memory,
 * the application will present an error message to the user saying that the data could not be created.
 *
 * By contrast, it is not practical to check the allocation of user interface elements,
 * because the application cannot perform correctly if an interface element is missing or incorrect.
 * For such situations, the application will typically use the _f versions of the allocation routines,
 * which, if out of memory, will free a "rainy-day fund" and retry.
 * In this way, the interface element can usually still be allocated;
 * the application will present an error message telling the user to save her work and quit the application.
 * If the user doesn't do that, the application will crash upon the next failing allocation of a _f routine.
 */

#define theRainyDayFund_SIZE  300000
static char *theRainyDayFund = NULL;

void Melder_alloc_init (void) {
	theRainyDayFund = (char *) malloc (theRainyDayFund_SIZE);   // called at application initialization, so cannot fail
	assert (theRainyDayFund != NULL);
}

void * _Melder_malloc (unsigned long size) {
	if (size <= 0)
		Melder_throw ("Can never allocate ", size, " bytes.");
	void *result = malloc (size);
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room for another ", size, " bytes.");
	if (Melder_debug == 34) { Melder_casual ("Melder_malloc\t%ld\t%ld\t1", result, size); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void * _Melder_malloc_f (unsigned long size) {
	if (size <= 0)
		Melder_fatal ("(Melder_malloc_f:) Can never allocate %ld bytes.", size);
	void *result = malloc (size);
	if (result == NULL) {
		if (theRainyDayFund != NULL) free (theRainyDayFund);
		result = malloc (size);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room for another %ld bytes.", size);
		}
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void _Melder_free (void **ptr) {
	if (*ptr == NULL) return;
	free (*ptr);
	if (Melder_debug == 34) { Melder_casual ("Melder_free\t%ld\t?\t?", *ptr); }
	*ptr = NULL;
	totalNumberOfDeallocations += 1;
}

void * Melder_realloc (void *ptr, long size) {
	if (size <= 0)
		Melder_throw ("Can never allocate ", size, " bytes.");
	void *result = realloc (ptr, size);   // will not show in the statistics...
	if (result == NULL)
		Melder_throw ("Out of memory. Could not extend room to ", size, " bytes.");
	if (ptr == NULL) {   // is it like malloc?
		if (Melder_debug == 34) { Melder_casual ("Melder_realloc\t%ld\t%ld\t1", result, size); }
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
	} else if (result != ptr) {   // did realloc do a malloc-and-free?
		totalNumberOfAllocations += 1;
		totalAllocationSize += size;
		totalNumberOfDeallocations += 1;
		totalNumberOfMovingReallocs += 1;
	} else {
		totalNumberOfReallocsInSitu += 1;
	}
	return result;
}

void * Melder_realloc_f (void *ptr, long size) {
	void *result;
	if (size <= 0)
		Melder_fatal ("(Melder_realloc_f:) Can never allocate %ld bytes.", size);
	result = realloc (ptr, size);   /* Will not show in the statistics... */
	if (result == NULL) {
		if (theRainyDayFund != NULL) free (theRainyDayFund);
		result = realloc (ptr, size);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory. Could not extend room to %ld bytes.", size);
		}
	}
	if (ptr == NULL) {   /* Is it like malloc? */
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
		Melder_throw ("Can never allocate ", nelem, " elements.");
	if (elsize <= 0)
		Melder_throw ("Can never allocate elements whose size is ", elsize, " bytes.");
	result = calloc (nelem, elsize);
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room for ", nelem, " more elements whose sizes are ", elsize, " bytes each.");
	if (Melder_debug == 34) { Melder_casual ("Melder_calloc\t%ld\t%ld\t%ld", result, nelem, elsize); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	return result;
}

void * _Melder_calloc_f (long nelem, long elsize) {
	void *result;
	if (nelem <= 0)
		Melder_fatal ("(Melder_calloc_f:) Can never allocate %ld elements.", nelem);
	if (elsize <= 0)
		Melder_fatal ("(Melder_calloc_f:) Can never allocate elements whose size is %ld bytes.", elsize);
	result = calloc (nelem, elsize);
	if (result == NULL) {
		if (theRainyDayFund != NULL) free (theRainyDayFund);
		result = calloc (nelem, elsize);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room for %ld more elements whose sizes are %ld bytes each.", nelem, elsize);
		}
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	return result;
}

char * Melder_strdup (const char *string) {
	if (! string) return NULL;
	long size = strlen (string) + 1;
	char *result = (char *) malloc (size * sizeof (char));
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", size - 1, " characters.");
	strcpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_strdup\t%ld\t%ld\t1", result, size); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

char * Melder_strdup_f (const char *string) {
	if (! string) return NULL;
	long size = strlen (string) + 1;
	char *result = (char *) malloc (size * sizeof (char));
	if (result == NULL) {
		if (theRainyDayFund != NULL) free (theRainyDayFund);
		result = (char *) malloc (size * sizeof (char));
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room to duplicate a text of %ld characters.", size - 1);
		}
	}
	strcpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

wchar_t * Melder_wcsdup (const wchar_t *string) {
	if (! string) return NULL;
	long size = wcslen (string) + 1;
	wchar_t *result = (wchar_t *) malloc (size * sizeof (wchar_t));
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", size - 1, " characters.");
	wcscpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_wcsdup\t%ld\t%ld\t4", result, size); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size * sizeof (wchar_t);
	return result;
}

wchar_t * Melder_wcsdup_f (const wchar_t *string) {
	if (! string) return NULL;
	long size = wcslen (string) + 1;
	wchar_t *result = (wchar_t *) malloc (size * sizeof (wchar_t));
	if (result == NULL) {
		if (theRainyDayFund != NULL) free (theRainyDayFund);
		result = (wchar_t *) malloc (size * sizeof (wchar_t));
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room to duplicate a text of %ld characters.", size - 1);
		}
	}
	wcscpy (result, string);
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

/* End of file melder_alloc.cpp */
