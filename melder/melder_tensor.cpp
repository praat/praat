/* melder_tensor.cpp
 *
 * Copyright (C) 1992-2012,2018-2020 Paul Boersma
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

byte * MelderTensor_generic (integer cellSize, integer numberOfCells, kTensorInitializationType initializationType) {
	try {
		if (numberOfCells <= 0)
			return nullptr;   // not an error
		byte *result = ( initializationType == kTensorInitializationType :: ZERO ?
				reinterpret_cast <byte *> (_Melder_calloc (numberOfCells, cellSize)) :
				reinterpret_cast <byte *> (_Melder_malloc (numberOfCells * cellSize)) );
		theTotalNumberOfArrays += 1;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Tensor of ", numberOfCells, U" cells not created.");
	}
}

void MelderTensor_free_generic (byte *cells) noexcept {
	if (! cells)
		return;   // not an error
	Melder_free (cells);
	theTotalNumberOfArrays -= 1;
}

/* End of file melder_tensor.cpp */
