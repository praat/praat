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

static int64 theTotalNumberOfAllocations = 0, theTotalNumberOfDeallocations = 0;
static int64 theTotalCellAllocationHistory = 0, theTotalCellDeallocationHistory = 0;

int64 MelderTensor_allocationCount () { return theTotalNumberOfAllocations; }
int64 MelderTensor_deallocationCount () { return theTotalNumberOfDeallocations; }
int64 MelderTensor_cellAllocationCount () { return theTotalCellAllocationHistory; }
int64 MelderTensor_cellDeallocationCount () { return theTotalCellDeallocationHistory; }

#pragma mark - Generic memory functions for vectors

byte * MelderTensor_generic (integer cellSize, integer numberOfCells, kTensorInitializationType initializationType) {
	try {
		if (numberOfCells <= 0)
			return nullptr;   // not an error
		byte *result = ( initializationType == kTensorInitializationType :: ZERO ?
				reinterpret_cast <byte *> (_Melder_calloc (numberOfCells, cellSize)) :
				reinterpret_cast <byte *> (_Melder_malloc (numberOfCells * cellSize)) );
		theTotalNumberOfAllocations += 1;
		theTotalCellAllocationHistory += numberOfCells;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Tensor of ", numberOfCells, U" cells not created.");
	}
}

void MelderTensor_free_generic (byte *cells, integer numberOfCells) noexcept {
	if (! cells)
		return;   // not an error
	Melder_free (cells);
	theTotalNumberOfDeallocations += 1;
	theTotalCellDeallocationHistory += numberOfCells;
}

/* End of file melder_tensor.cpp */
