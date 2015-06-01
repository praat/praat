/* melder_alloc.cpp
 *
 * Copyright (C) 1992-2011,2014,2015 Paul Boersma
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
 * pb 2014/12/17 made everything int64_t
 */

#include "melder.h"
#include <wctype.h>
#include <assert.h>

static double totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0,
	totalNumberOfMovingReallocs = 0, totalNumberOfReallocsInSitu = 0;

/*
 * The rainy-day fund.
 *
 * Typically, memory allocation for data is entirely checked by using the normal versions of the allocation routines,
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

#define theRainyDayFund_SIZE  3000000
static char *theRainyDayFund = NULL;

void Melder_alloc_init (void) {
	theRainyDayFund = (char *) malloc (theRainyDayFund_SIZE);   // called at application initialization, so cannot fail
	assert (theRainyDayFund != NULL);
}

void * _Melder_malloc (int64_t size) {
	if (size <= 0)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes.");
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	void *result = malloc ((size_t) size);
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room for another ", Melder_bigInteger (size), " bytes.");
	if (Melder_debug == 34) { Melder_casual ("Melder_malloc\t%p\t%ls\t1", result, Melder_bigInteger (size)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void * _Melder_malloc_f (int64_t size) {
	if (size <= 0)
		Melder_fatal ("(Melder_malloc_f:) Can never allocate %ls bytes.", Melder_bigInteger (size));
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_fatal ("(Melder_malloc_f:) Can never allocate %ls bytes.", Melder_bigInteger (size));
	void *result = malloc ((size_t) size);
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = malloc ((size_t) size);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room for another %ls bytes.", Melder_bigInteger (size));
		}
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void _Melder_free (void **ptr) {
	if (*ptr == NULL) return;
	free (*ptr);
	if (Melder_debug == 34) { Melder_casual ("Melder_free\t%p\t?\t?", *ptr); }
	*ptr = NULL;
	totalNumberOfDeallocations += 1;
}

void * Melder_realloc (void *ptr, int64_t size) {
	if (size <= 0)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes.");
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	void *result = realloc (ptr, (size_t) size);   // will not show in the statistics...
	if (result == NULL)
		Melder_throw ("Out of memory. Could not extend room to ", Melder_bigInteger (size), " bytes.");
	if (ptr == NULL) {   // is it like malloc?
		if (Melder_debug == 34) { Melder_casual ("Melder_realloc\t%p\t%ls\t1", result, Melder_bigInteger (size)); }
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

void * Melder_realloc_f (void *ptr, int64_t size) {
	void *result;
	if (size <= 0)
		Melder_fatal ("(Melder_realloc_f:) Can never allocate %ls bytes.", Melder_bigInteger (size));
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_fatal ("(Melder_realloc_f:) Can never allocate %ls bytes.", Melder_bigInteger (size));
	result = realloc (ptr, (size_t) size);   // will not show in the statistics...
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = realloc (ptr, (size_t) size);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory. Could not extend room to %ls bytes.", Melder_bigInteger (size));
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

void * _Melder_calloc (int64_t nelem, int64_t elsize) {
	void *result;
	if (nelem <= 0)
		Melder_throw ("Can never allocate ", Melder_bigInteger (nelem), " elements.");
	if (elsize <= 0)
		Melder_throw ("Can never allocate elements whose size is ", Melder_bigInteger (elsize), " bytes.");
	if (sizeof (size_t) < 8 && (double) nelem * (double) elsize > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (nelem * elsize), " bytes. Use a 64-bit edition of Praat instead?");
	result = calloc ((size_t) nelem, (size_t) elsize);
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room for ", Melder_bigInteger (nelem), " more elements whose sizes are ", elsize, " bytes each.");
	if (Melder_debug == 34) { Melder_casual ("Melder_calloc\t%p\t%ls\t%ls", result, Melder_bigInteger (nelem), Melder_bigInteger (elsize)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) nelem * (double) elsize;
	return result;
}

void * _Melder_calloc_f (int64_t nelem, int64_t elsize) {
	void *result;
	if (nelem <= 0)
		Melder_fatal ("(Melder_calloc_f:) Can never allocate %ls elements.", Melder_bigInteger (nelem));
	if (elsize <= 0)
		Melder_fatal ("(Melder_calloc_f:) Can never allocate elements whose size is %ls bytes.", Melder_bigInteger (elsize));
	if (sizeof (size_t) < 8 && (double) nelem * (double) elsize > SIZE_MAX)
		Melder_fatal ("(Melder_calloc_f:) Can never allocate %ls bytes.", Melder_double ((double) nelem * (double) elsize));
	result = calloc ((size_t) nelem, (size_t) elsize);
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = calloc ((size_t) nelem, (size_t) elsize);
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room for %ls more elements whose sizes are %ls bytes each.", Melder_bigInteger (nelem), Melder_bigInteger (elsize));
		}
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) nelem * (double) elsize;
	return result;
}

char * Melder_strdup (const char *string) {
	if (! string) return NULL;
	int64_t size = (int64_t) strlen (string) + 1;
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	char *result = (char *) malloc ((size_t) size * sizeof (char));
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", Melder_bigInteger (size - 1), " characters.");
	strcpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_strdup\t%p\t%ls\t1", result, Melder_bigInteger (size)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) size;
	return result;
}

char * Melder_strdup_f (const char *string) {
	if (! string) return NULL;
	int64_t size = (int64_t) strlen (string) + 1;
	if (sizeof (size_t) < 8 && (double) size > SIZE_MAX)
		Melder_fatal ("(Melder_strdup_f:) Can never allocate %ls bytes.", Melder_bigInteger (size));
	char *result = (char *) malloc ((size_t) size * sizeof (char));
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = (char *) malloc ((size_t) size * sizeof (char));
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room to duplicate a text of %ls characters.", Melder_bigInteger (size - 1));
		}
	}
	strcpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) size;
	return result;
}

wchar_t * Melder_wcsdup (const wchar_t *string) {
	if (! string) return NULL;
	int64_t size = (int64_t) wcslen (string) + 1;
	double allocationSize = (double) size * (double) sizeof (wchar_t);
	if (allocationSize > INT54_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. It is above the 9-petabyte limit.");
	if (sizeof (size_t) < 8 && allocationSize > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	wchar_t *result = (wchar_t *) malloc ((size_t) allocationSize);   // guarded conversion
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", Melder_bigInteger (size - 1), " characters.");
	wcscpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_wcsdup\t%p\t%ls\t4", result, Melder_bigInteger (size)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += allocationSize;
	return result;
}

char16 * Melder_str16dup (const char16 *string) {
	if (! string) return NULL;
	int64 size = (int64) str16len (string) + 1;
	double allocationSize = (double) size * (double) sizeof (char16);
	if (allocationSize > INT54_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. It is above the 9-petabyte limit.");
	if (sizeof (size_t) < 8 && allocationSize > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	char16 *result = (char16 *) malloc ((size_t) allocationSize);   // guarded conversion
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", Melder_bigInteger (size - 1), " 16-bit characters.");
	str16cpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_str16dup\t%p\t%ls\t4", result, Melder_bigInteger (size)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += allocationSize;
	return result;
}

char32 * Melder_str32dup (const char32 *string) {
	if (! string) return NULL;
	int64 size = (int64) str32len (string) + 1;
	double allocationSize = (double) size * (double) sizeof (char32);
	if (allocationSize > INT54_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. It is above the 9-petabyte limit.");
	if (sizeof (size_t) < 8 && allocationSize > SIZE_MAX)
		Melder_throw ("Can never allocate ", Melder_bigInteger (size), " bytes. Use a 64-bit edition of Praat instead?");
	char32 *result = (char32 *) malloc ((size_t) allocationSize);   // guarded conversion
	if (result == NULL)
		Melder_throw ("Out of memory: there is not enough room to duplicate a text of ", Melder_bigInteger (size - 1), " characters.");
	str32cpy (result, string);
	if (Melder_debug == 34) { Melder_casual ("Melder_str32dup\t%p\t%ls\t4", result, Melder_bigInteger (size)); }
	totalNumberOfAllocations += 1;
	totalAllocationSize += allocationSize;
	return result;
}

wchar_t * Melder_wcsdup_f (const wchar_t *string) {
	if (! string) return NULL;
	int64 size = (int64) wcslen (string) + 1;
	if (sizeof (size_t) < 8 && (double) size * (double) sizeof (wchar_t) > SIZE_MAX)
		Melder_fatal ("(Melder_wcsdup_f:) Can never allocate %ls characters.", Melder_bigInteger (size));
	wchar_t *result = (wchar_t *) malloc ((size_t) size * sizeof (wchar_t));
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = (wchar_t *) malloc ((size_t) size * sizeof (wchar_t));
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room to duplicate a text of %ls characters.", Melder_bigInteger (size - 1));
		}
	}
	wcscpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) size * sizeof (wchar_t);
	return result;
}
char32 * Melder_str32dup_f (const char32 *string) {
	if (! string) return NULL;
	int64 size = (int64) str32len (string) + 1;
	if (sizeof (size_t) < 8 && (double) size * (double) sizeof (char32) > SIZE_MAX)
		Melder_fatal ("(Melder_str32dup_f:) Can never allocate %ls characters.", Melder_bigInteger (size));
	char32 *result = (char32 *) malloc ((size_t) size * sizeof (char32));
	if (result == NULL) {
		if (theRainyDayFund != NULL) { free (theRainyDayFund); theRainyDayFund = NULL; }
		result = (char32 *) malloc ((size_t) size * sizeof (char32));
		if (result != NULL) {
			Melder_flushError ("Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		} else {
			Melder_fatal ("Out of memory: there is not enough room to duplicate a text of %ls characters.", Melder_bigInteger (size - 1));
		}
	}
	str32cpy (result, string);
	totalNumberOfAllocations += 1;
	totalAllocationSize += (double) size * sizeof (char32);
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

int Melder_str32cmp (const char32 *string1, const char32 *string2) {
	if (string1 == NULL) string1 = U"";
	if (string2 == NULL) string2 = U"";
	return str32cmp (string1, string2);
}

int Melder_strncmp (const char *string1, const char *string2, int64_t n) {
	if (string1 == NULL) string1 = "";
	if (string2 == NULL) string2 = "";
	return strncmp (string1, string2, n);
}

int Melder_str32ncmp (const char32 *string1, const char32 *string2, int64_t n) {
	if (string1 == NULL) string1 = U"";
	if (string2 == NULL) string2 = U"";
	return str32ncmp (string1, string2, n);
}

int Melder_wcscmp (const wchar_t *string1, const wchar_t *string2) {
	if (string1 == NULL) string1 = L"";
	if (string2 == NULL) string2 = L"";
	return wcscmp (string1, string2);
}

int Melder_wcsncmp (const wchar_t *string1, const wchar_t *string2, int64_t n) {
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

bool Melder_str32equ_firstCharacterCaseInsensitive (const char32 *string1, const char32 *string2) {
	if (string1 == NULL) string1 = U"";
	if (string2 == NULL) string2 = U"";
	if (*string1 == U'\0') return *string2 == U'\0';
	if (*string1 == *string2)
		return ! str32cmp (string1 + 1, string2 + 1);
	if (sizeof (wchar_t) == 2) {
		if (*string1 > 65536 || *string2 > 65536)
			return false;
		if (towlower ((char16) *string1) != towlower ((char16) *string2)) return false;
	} else {
		if (towlower ((int32) *string1) != towlower ((int32) *string2)) return false;
	}
	return ! str32cmp (string1 + 1, string2 + 1);
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
