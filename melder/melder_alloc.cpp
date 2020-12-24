/* melder_alloc.cpp
 *
 * Copyright (C) 1992-2007,2009,2011,2012,2014-2020 Paul Boersma
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
#include <wctype.h>
#include <assert.h>

static int64 totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0,
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

#define theRainyDayFund_SIZE  3'000'000
static char *theRainyDayFund = nullptr;

void Melder_alloc_init () {
	theRainyDayFund = (char *) malloc (theRainyDayFund_SIZE);   // called at application initialization, so cannot fail
	assert (theRainyDayFund);
}

/*
	The following functions take int64 arguments even on 32-bit machines.
	This is because it is easy for the user to request objects that do not fit in memory
	on 32-bit machines, in which case an appropriate error message is required.
*/

void * _Melder_malloc (int64 size) {
	if (size <= 0)
		Melder_throw (U"Can never allocate ", Melder_bigInteger (size), U" bytes.");
	if (sizeof (size_t) < 8 && size > SIZE_MAX)
		Melder_throw (U"Can never allocate ", Melder_bigInteger (size), U" bytes. Use a 64-bit edition of Praat instead?");
	void *result = malloc ((size_t) size);   // guarded cast
	if (! result)
		Melder_throw (U"Out of memory: there is not enough room for another ", Melder_bigInteger (size), U" bytes.");
	if (Melder_debug == 34)
		Melder_casual (U"Melder_malloc\t", Melder_pointer (result), U"\t", Melder_bigInteger (size), U"\t1");
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void * _Melder_malloc_f (int64 size) {
	if (size <= 0)
		Melder_fatal (U"(Melder_malloc_f:) Can never allocate ", Melder_bigInteger (size), U" bytes.");
	if (sizeof (size_t) < 8 && size > SIZE_MAX)
		Melder_fatal (U"(Melder_malloc_f:) Can never allocate ", Melder_bigInteger (size), U" bytes.");
	void *result = malloc ((size_t) size);
	if (! result) {
		if (theRainyDayFund) {
			free (theRainyDayFund);
			theRainyDayFund = nullptr;
		}
		result = malloc ((size_t) size);
		if (result)
			Melder_flushError (U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		else
			Melder_fatal (U"Out of memory: there is not enough room for another ", Melder_bigInteger (size), U" bytes.");
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += size;
	return result;
}

void _Melder_free (void **ptr) noexcept {
	if (! *ptr)
		return;
	if (Melder_debug == 34)
		Melder_casual (U"Melder_free\t", Melder_pointer (*ptr), U"\t?\t?");
	free (*ptr);
	*ptr = nullptr;
	totalNumberOfDeallocations += 1;
}

void * Melder_realloc (void *ptr, int64 size) {
	if (size <= 0)
		Melder_throw (U"Can never allocate ", Melder_bigInteger (size), U" bytes.");
	if (sizeof (size_t) < 8 && size > SIZE_MAX)
		Melder_throw (U"Can never allocate ", Melder_bigInteger (size), U" bytes. Use a 64-bit edition of Praat instead?");
	void *result = realloc (ptr, (size_t) size);   // will not show in the statistics...
	if (! result)
		Melder_throw (U"Out of memory. Could not extend room to ", Melder_bigInteger (size), U" bytes.");
	if (! ptr) {   // is it like malloc?
		if (Melder_debug == 34)
			Melder_casual (U"Melder_realloc\t", Melder_pointer (result), U"\t", Melder_bigInteger (size), U"\t1");
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

void * Melder_realloc_f (void *ptr, int64 size) {
	if (size <= 0)
		Melder_fatal (U"(Melder_realloc_f:) Can never allocate ", Melder_bigInteger (size), U" bytes.");
	if (sizeof (size_t) < 8 && size > SIZE_MAX)
		Melder_fatal (U"(Melder_realloc_f:) Can never allocate ", Melder_bigInteger (size), U" bytes.");
	void *result = realloc (ptr, (size_t) size);   // will not show in the statistics...
	if (! result) {
		if (theRainyDayFund) {
			free (theRainyDayFund);
			theRainyDayFund = nullptr;
		}
		result = realloc (ptr, (size_t) size);
		if (result)
			Melder_flushError (U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		else
			Melder_fatal (U"Out of memory. Could not extend room to ", Melder_bigInteger (size), U" bytes.");
	}
	if (! ptr) {   // is it like malloc?
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

void * _Melder_calloc (int64 nelem, int64 elsize) {
	if (nelem <= 0)
		Melder_throw (U"Can never allocate ", Melder_bigInteger (nelem), U" elements.");
	if (elsize <= 0)
		Melder_throw (U"Can never allocate elements whose size is ", Melder_bigInteger (elsize), U" bytes.");
	if ((uint64) nelem > SIZE_MAX / (uint64) elsize)   // guarded casts to unsigned
		Melder_throw (U"Can never allocate ", Melder_bigInteger (nelem), U" elements whose sizes are ", Melder_bigInteger (elsize), U" bytes each.",
			sizeof (size_t) < 8 ? U" Use a 64-bit edition of Praat instead?" : nullptr);
	void *result = calloc ((size_t) nelem, (size_t) elsize);
	if (! result)
		Melder_throw (U"Out of memory: there is not enough room for ", Melder_bigInteger (nelem), U" more elements whose sizes are ", elsize, U" bytes each.");
	if (Melder_debug == 34)
		Melder_casual (U"Melder_calloc\t", Melder_pointer (result), U"\t", Melder_bigInteger (nelem), U"\t", Melder_bigInteger (elsize));
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	return result;
}

void * _Melder_calloc_f (int64 nelem, int64 elsize) {
	if (nelem <= 0)
		Melder_fatal (U"(Melder_calloc_f:) Can never allocate ", Melder_bigInteger (nelem), U" elements.");
	if (elsize <= 0)
		Melder_fatal (U"(Melder_calloc_f:) Can never allocate elements whose size is ", Melder_bigInteger (elsize), U" bytes.");
	if ((uint64) nelem > SIZE_MAX / (uint64) elsize)
		Melder_fatal (U"(Melder_calloc_f:) Can never allocate ", Melder_bigInteger (nelem), U" elements whose sizes are ", Melder_bigInteger (elsize), U" bytes each.");
	void *result = calloc ((size_t) nelem, (size_t) elsize);
	if (! result) {
		if (theRainyDayFund) {
			free (theRainyDayFund);
			theRainyDayFund = nullptr;
		}
		result = calloc ((size_t) nelem, (size_t) elsize);
		if (result)
			Melder_flushError (U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
		else
			Melder_fatal (U"Out of memory: there is not enough room for ", Melder_bigInteger (nelem),
				U" more elements whose sizes are ", Melder_bigInteger (elsize), U" bytes each.");
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += nelem * elsize;
	return result;
}

autostring32 Melder_dup (conststring32 string /* cattable */) {
	if (! string)
		return autostring32();
	int64 size = (int64) str32len (string) + 1;   // guaranteed to be positive
	if (sizeof (size_t) < 8 && size > SIZE_MAX / sizeof (char32))
		Melder_throw (U"Can never allocate ", Melder_bigInteger (size), U" characters. Use a 64-bit edition of Praat instead?");
	autostring32 result (size, false);   // guarded conversion
	str32cpy (result.get(), string);
	if (Melder_debug == 34)
		Melder_casual (U"Melder_dup\t", Melder_pointer (result.get()), U"\t", Melder_bigInteger (size), U"\t", sizeof (char32));
	return result;
}

autostring32 Melder_dup_f (conststring32 string /* cattable */) {
	if (! string)
		return autostring32();
	int64 size = (int64) str32len (string) + 1;
	if (sizeof (size_t) < 8 && size > SIZE_MAX / sizeof (char32))
		Melder_fatal (U"(Melder_dup_f:) Can never allocate ", Melder_bigInteger (size), U" characters.");
	autostring32 result (size, true);
	str32cpy (result.get(), string);
	return result;
}

int64 Melder_allocationCount () {
	return totalNumberOfAllocations;
}

int64 Melder_deallocationCount () {
	return totalNumberOfDeallocations;
}

int64 Melder_allocationSize () {
	return totalAllocationSize;
}

int64 Melder_reallocationsInSituCount () {
	return totalNumberOfReallocsInSitu;
}

int64 Melder_movingReallocationsCount () {
	return totalNumberOfMovingReallocs;
}

int Melder_cmp (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp (string1, string2);
}

int Melder_cmp_caseInsensitive (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp_caseInsensitive (string1, string2);
}

int Melder_ncmp (conststring32 string1, conststring32 string2, integer n) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp (string1, string2, n);
}

int Melder_ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp_caseInsensitive (string1, string2, n);
}

bool Melder_equ_firstCharacterCaseInsensitive (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	if (string1 [0] == U'\0')
		return string2 [0] == U'\0';
	if (Melder_toLowerCase (string1 [0]) != Melder_toLowerCase (string2 [0]))
		return false;
	return str32equ (string1 + 1, string2 + 1);
}

#pragma mark - Generic memory functions for vectors and matrices

namespace MelderArray { // reopen
	int64 allocationCount = 0, deallocationCount = 0;
	int64 cellAllocationCount = 0, cellDeallocationCount = 0;
}

int64 MelderArray_allocationCount () { return MelderArray :: allocationCount; }
int64 MelderArray_deallocationCount () { return MelderArray :: deallocationCount; }
int64 MelderArray_cellAllocationCount () { return MelderArray :: cellAllocationCount; }
int64 MelderArray_cellDeallocationCount () { return MelderArray :: cellDeallocationCount; }

byte * MelderArray:: _alloc_generic (integer cellSize, integer numberOfCells, kInitializationType initializationType) {
	try {
		if (numberOfCells <= 0)
			return nullptr;   // not an error
		byte *result = ( initializationType == kInitializationType :: ZERO ?
				reinterpret_cast <byte *> (_Melder_calloc (numberOfCells, cellSize)) :
				reinterpret_cast <byte *> (_Melder_malloc (numberOfCells * cellSize)) );
		MelderArray::allocationCount += 1;
		MelderArray::cellAllocationCount += numberOfCells;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Tensor of ", numberOfCells, U" cells not created.");
	}
}

void MelderArray:: _free_generic (byte *cells, integer numberOfCells) noexcept {
	if (! cells)
		return;   // not an error
	Melder_free (cells);
	MelderArray::deallocationCount += 1;
	MelderArray::cellDeallocationCount += numberOfCells;
}

/* End of file melder_alloc.cpp */
