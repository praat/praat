/* melder_tensor.cpp
 *
 * Copyright (C) 1992-2012,2018 Paul Boersma
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

static integer theTotalNumberOfArrays;

integer NUM_getTotalNumberOfArrays () { return theTotalNumberOfArrays; }

#pragma mark - Generic memory functions for vectors

byte * NUMvector_generic (integer elementSize, integer lo, integer hi, bool initializeToZero) {
	try {
		const int64 numberOfCells = hi - lo + 1;
		if (numberOfCells <= 0)
			return nullptr;   // not an error
		byte *result;
		for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
			result = initializeToZero ?
				reinterpret_cast <byte *> (_Melder_calloc (numberOfCells, elementSize)) :
				reinterpret_cast <byte *> (_Melder_malloc (numberOfCells * elementSize));
			const int64 offset = (int64) lo * elementSize;
			result -= offset;
			if (result != nullptr)   // it would be quite a coincidence if this failed
				break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + offset, 1);   // make "sure" that the second try will succeed (not *very* sure, because realloc might move memory even if it shrinks)
		}
		theTotalNumberOfArrays += 1;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Vector of elements not created.");
	}
}

void NUMvector_free_generic (integer elementSize, byte *vector, integer lo) noexcept {
	if (! vector)
		return;   // not an error
	byte *cells = & vector [lo * elementSize];
	Melder_free (cells);
	theTotalNumberOfArrays -= 1;
}

byte * NUMvector_copy_generic (integer elementSize, const byte *vector, integer lo, integer hi) {
	try {
		if (! vector)
			return nullptr;
		byte *result = NUMvector_generic (elementSize, lo, hi, false);
		const byte *p_cells = & vector [lo * elementSize];
		byte *p_resultCells = & result [lo * elementSize];
		integer numberOfBytesToCopy = (hi - lo + 1) * elementSize;
		memcpy (p_resultCells, p_cells, (size_t) numberOfBytesToCopy);
		return result;
	} catch (MelderError) {
		Melder_throw (U"Vector of elements not copied.");
	}
}

void NUMvector_copyElements_generic (integer elementSize, const byte *fromVector, byte *toVector, integer lo, integer hi) {
	Melder_assert (fromVector && toVector);
	const byte *p_fromCells = & fromVector [lo * elementSize];
	byte *p_toCells   = & toVector   [lo * elementSize];
	integer numberOfBytesToCopy = (hi - lo + 1) * elementSize;
	if (hi >= lo) memcpy (p_toCells, p_fromCells, (size_t) numberOfBytesToCopy);   // BUG this assumes contiguity
}

bool NUMvector_equal_generic (integer elementSize, const byte *vector1, const byte *vector2, integer lo, integer hi) {
	Melder_assert (vector1 && vector2);
	const byte *p_cells1 = & vector1 [lo * elementSize];
	const byte *p_cells2 = & vector2 [lo * elementSize];
	integer numberOfBytesToCompare = (hi - lo + 1) * elementSize;
	return memcmp (p_cells1, p_cells2, (size_t) numberOfBytesToCompare) == 0;
}

void NUMvector_append_generic (integer elementSize, byte **v, integer lo, integer *hi) {
	try {
		byte *result;
		if (! *v) {
			result = NUMvector_generic (elementSize, lo, lo, true);
			*hi = lo;
		} else {
			integer offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <byte *> (Melder_realloc ((char *) *v + offset, (*hi - lo + 2) * elementSize));
				if ((result -= offset) != nullptr)   // it would be quite a coincidence if this failed
					break;   // this will normally succeed at the first try
				(void) Melder_realloc_f (result + offset, 1);   // make "sure" that the second try will succeed
			}
			(*hi) ++;
			memset (result + *hi * elementSize, 0, elementSize);   // initialize the new element to zeroes
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw (U"Vector: element not appended.");
	}
}

void NUMvector_insert_generic (integer elementSize, byte **v, integer lo, integer *hi, integer position) {
	try {
		byte *result;
		if (! *v) {
			result = NUMvector_generic (elementSize, lo, lo, true);
			*hi = lo;
			Melder_assert (position == lo);
		} else {
			result = NUMvector_generic (elementSize, lo, *hi + 1, false);
			Melder_assert (position >= lo && position <= *hi + 1);
			NUMvector_copyElements_generic (elementSize, *v, result, lo, position - 1);
			memset (result + position * elementSize, 0, elementSize);
			NUMvector_copyElements_generic (elementSize, *v, result + elementSize, position, *hi);
			NUMvector_free_generic (elementSize, *v, lo);
			(*hi) ++;
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw (U"Vector: element not inserted.");
	}
}

/* End of file melder_tensor.cpp */
