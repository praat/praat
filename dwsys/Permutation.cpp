/* Permutation.cpp
 *
 * Copyright (C) 2005-2022, 2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20050706
 djmw 20050722 Latest modification.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20100521 Next and Previous
 djmw 20100818 Permutation_permuteTwoItems/Numbers
 djmw 20110304 Thing_new
*/

#include <time.h>
#include "BinaryIndexedTree.h"
#include "Permutation.h"
#include "NUM2.h"
#include "PermutationInversionCounter.h"

#include "oo_DESTROY.h"
#include "Permutation_def.h"
#include "oo_COPY.h"
#include "Permutation_def.h"
#include "oo_EQUAL.h"
#include "Permutation_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Permutation_def.h"
#include "oo_WRITE_TEXT.h"
#include "Permutation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Permutation_def.h"
#include "oo_READ_BINARY.h"
#include "Permutation_def.h"
#include "oo_DESCRIPTION.h"
#include "Permutation_def.h"

Thing_implement (Permutation, Daata, 0);

static integer Permutation_checkRange (Permutation me, integer *from, integer *to) {
	if (*from == 0)
		*from = 1;
	if (*to == 0)
		*to = my numberOfElements;
	Melder_require (*from > 0 && *from <= my numberOfElements && *to > 0 && *to <= my numberOfElements,
		U"Range should be in [1, ", my numberOfElements, U"].");
	return *to - *from + 1;
}

void Permutation_checkInvariant (Permutation me) {
	/*
		All element numbers should be in the interval [1, numberOfElements].
	*/
	MelderIntegerRange irange = NUMextrema_e (my p.get());
	Melder_require (irange.first == 1,
		U"Your minimum number should be 1 (it is ", irange.first, U").");
	Melder_require (irange.last == my numberOfElements,
		U"Your maximum number (", irange.last, U") should not be larger than the number of elements you supplied (", my numberOfElements, U").");
	/*
		All element numbers should occur exactly once
	*/
	autoINTVEC p = zero_INTVEC (my numberOfElements);
	for (integer i = 1; i <= my numberOfElements; i ++)
		p [my p [i]] ++;
	for (integer i = 1; i <= my numberOfElements; i ++)
		Melder_require (p [i] == 1,
			U"All numbers from 1 to ", my numberOfElements, U" should occur exactly once, e.g. the value ", i, U" occurs ", p [i], U" times.");
}

void structPermutation :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Number of elements: ", numberOfElements);
}

void structPermutation :: v1_readText (MelderReadText text, int /*formatVersion*/) {
	numberOfElements = texgeti32 (text);
	Melder_require (numberOfElements > 0,
		U"Number of elements should be greater than zero.");
	p = vector_readText_integer32BE (numberOfElements, text, "p");
	Permutation_checkInvariant (this);
}

void Permutation_init (Permutation me, integer numberOfElements) {
	my numberOfElements = numberOfElements;
	my p = to_INTVEC (numberOfElements);
}

autoPermutation Permutation_createSimplePermutation (constINTVEC const& numbers) {
	try {
		Melder_require (numbers.size > 0,
			U"There should be at least one element in a Permutation.");
		autoPermutation me = Permutation_create (numbers.size, true);
		my p.get()  <<=  numbers;
		Permutation_checkInvariant (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"The permutation could not be created.");
	}
}

void Permutation_drawAsLine (Permutation me, Graphics g, bool garnish) {
	const double xmin = 0.0, xmax = my numberOfElements + 1.0;
	const double ymin = xmin, ymax = xmax;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	autoVEC x = raw_VEC (my numberOfElements), y = raw_VEC (my numberOfElements);
	for (integer i = 1; i <= my numberOfElements; i ++) {
		x [i] = i;
		y [i] = my p [i];
	}
	Graphics_polyline (g, my numberOfElements, & x [1], & y [1]);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markLeft (g, 1, true, true, false, U"");
		Graphics_markLeft (g, my numberOfElements, true, true, false, U"");
		Graphics_markBottom (g, 1, true, true, false, U"");
		Graphics_markBottom (g, my numberOfElements, true, true, false, U"");
	}
}

void Permutation_tableJump_inline (Permutation me, integer jumpSize, integer first) {
	if (jumpSize >= my numberOfElements || first > my numberOfElements)
		return;
	autoINTVEC p_copy = copy_INTVEC (my p.get());

	integer index = first, column = 1;
	if (first > 1)
		column = (first - 1) % jumpSize + 1;
	for (integer i = 1; i <= my numberOfElements; i ++) {
		my p [i] = p_copy [index];
		index += jumpSize;
		if (index > my numberOfElements) {
			if (++ column > jumpSize)
				column = 1;
			index = column;
		}
	}
}

autoPermutation Permutation_create (integer numberOfElements, bool identity) {
	try {
		autoPermutation me = Thing_new (Permutation);
		Permutation_init (me.get(), numberOfElements);
		if (! identity)
			Permutation_permuteRandomly_inplace (me.get(), 1, numberOfElements);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Permutation not created.");
	}
}

void Permutation_sort (Permutation me) {
	to_INTVEC_out (my p.get());
}

void Permutation_swapPositions (Permutation me, integer i1, integer i2) {
	try {
		Melder_require (i1 > 0 && i1 <= my numberOfElements && i2 > 0 && i2 <= my numberOfElements, 
			U"Positions should be within the range [1, ",  my numberOfElements, U"].");
		std::swap (my p [i1], my p [i2]);
	} catch (MelderError) {
		Melder_throw (me, U": positions not swapped.");
	}
}

void Permutation_swapNumbers (Permutation me, integer i1, integer i2) {
	try {
		integer ip = 0;
		Melder_require (i1 > 0 && i1 <= my numberOfElements && i2 > 0 && i2 <= my numberOfElements, 
			U"Positions should be within the range [1, ",  my numberOfElements, U"].");
		
		if (i1 == i2)
			return;
		for (integer i = 1; i <= my numberOfElements; i ++) {
			if (my p [i] == i1) {
				my p [i] = i2;
				ip ++;
			} else if (my p [i] == i2) {
				my p [i] = i1;
				ip ++;
			}
			if (ip == 2)
				break;
		}
		Melder_assert (ip == 2);
	} catch (MelderError) {
		Melder_throw (me, U": numbers not swapped.");
	}
}

void Permutation_swapBlocks (Permutation me, integer from, integer to, integer blockSize) {
	try {
		Melder_require (blockSize > 0 && blockSize <= my numberOfElements / 2,
			U"The block size should be in the [1, ", my numberOfElements / 2, U"] range.");
		Melder_require (from > 0 && to > 0 && from + blockSize <= my numberOfElements && to + blockSize <= my numberOfElements,
			U"Start and finish positions of the two blocks should be in the [1,", my numberOfElements, U"] range.");

		if (from == to)
			return;
		for (integer i = 1; i <= blockSize; i ++)
			std::swap (my p [from + i - 1], my p [to + i - 1]);
	} catch (MelderError) {
		Melder_throw (me, U": blocks not swapped.");
	}
}

autoPermutation Permutation_permutePartByOther (Permutation me, integer startPosition, Permutation other) {
	Melder_require (startPosition > 0 && startPosition <= my numberOfElements,
		U"The start position should be in the range from 1 to ", my numberOfElements, U".");
	const integer endPosition = startPosition + other -> numberOfElements - 1;
	Melder_require (endPosition <= my numberOfElements,
		U"Start at position ", startPosition, U" requires the permuter to have maximally ",
		my numberOfElements - startPosition + 1, U" elements, however it has ", other -> numberOfElements,
		U" elements. "
	);
	autoPermutation thee = Data_copy (me);
	for (integer ipos = 1; ipos <= other -> numberOfElements; ipos ++)
		thy p [startPosition + ipos - 1] = my p [startPosition + other -> p [ipos] - 1];
	return thee;
}

void Permutation_permuteRandomly_inplace (Permutation me, integer from, integer to) {
	try {
		const integer n = Permutation_checkRange (me, & from, & to);
		if (n == 1)
			return;
		for (integer i = from; i < to; i ++) {
			const integer newpos = NUMrandomInteger (from, to);
			std::swap (my p [i], my p [newpos]);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not permuted randomly.");
	}
}

autoPermutation Permutation_permuteRandomly (Permutation me, integer from, integer to) {
	try {
		autoPermutation thee = Data_copy (me);
		Permutation_permuteRandomly_inplace (thee.get(), from, to);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not permuted.");
	}
}

autoPermutation Permutation_rotate (Permutation me, integer from, integer to, integer step) {
	try {
		const integer n = Permutation_checkRange (me, & from, & to);
		step = (step - 1) % n + 1;

		autoPermutation thee = Data_copy (me);
		for (integer i = from; i <= to; i ++) {
			integer ifrom = i + step;
			if (ifrom > to)
				ifrom -= n;
			if (ifrom < from)
				ifrom += n;
			thy p [ifrom] = my p [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not rotated.");
	}
}

void Permutation_swapOneFromRange (Permutation me, integer from, integer to, integer pos, bool forbidsame) {
	try {
		const integer n = Permutation_checkRange (me, & from, & to);
		integer newpos = NUMrandomInteger (from, to);
		if (newpos == pos && forbidsame) {
			Melder_require (n != 1,
				U"Impossible to satisfy \"forbid same\" constraint within the chosen range.");
			
			while ((newpos = NUMrandomInteger (from, to)) == pos) {
				;
			}
		}
		std::swap (my p [pos], my p [newpos]);
	} catch (MelderError) {
		Melder_throw (me, U": one from range not swapped.");
	}
}

autoPermutation Permutation_permuteBlocksRandomly (Permutation me, integer from, integer to, integer blockSize, bool permuteWithinBlocks, bool noDoublets) {
	try {
		const integer n = Permutation_checkRange (me, & from, & to);
		if (blockSize == 1 || (blockSize >= n && permuteWithinBlocks)) {
			autoPermutation thee = Permutation_permuteRandomly (me, from, to);
			return thee;
		}
		autoPermutation thee = Data_copy (me);
		if (blockSize >= n)
			return thee;

		const integer nblocks  = n / blockSize, nrest = n % blockSize;
		Melder_require (nrest == 0,
			U"There should fit an integer number of blocks in the range.\n(The last block is only of size ", nrest, U").");
		
		autoPermutation pblocks = Permutation_create (nblocks, true);

		integer first = from;
		for (integer iblock = 1; iblock <= nblocks; iblock ++, first += blockSize) {
			/* (n1,n2,n3,...) means: move block n1 to position 1 etc... */
			const integer blocktomove = Permutation_getValueAtIndex (pblocks.get(), iblock);

			for (integer j = 1; j <= blockSize; j ++)
				thy p [first - 1 + j] = my p [from - 1 + (blocktomove - 1) * blockSize + j];

			if (permuteWithinBlocks) {
				const integer last = first + blockSize - 1;
				Permutation_permuteRandomly_inplace (thee.get(), first, last);
				if (noDoublets && iblock > 0 && thy p [first - 1] % blockSize == thy p [first] % blockSize)
					Permutation_swapOneFromRange (thee.get(), first + 1, last, first, 0);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not permuted block randomly.");
	}
}

autoPermutation Permutation_interleave (Permutation me, integer from, integer to, integer blockSize, integer offset) {
	try {
		Melder_require (offset < blockSize,
			U"Offset should be smaller than block size.");
		const integer n = Permutation_checkRange (me, & from, & to);
		const integer nblocks = n / blockSize, nrest = n % blockSize;
		Melder_require (nrest == 0,
			U"There should fit an integer number of blocks in the range.\n"
			U"(The last block is only of size ", nrest, U" instead of ", blockSize, U").");
		
		autoPermutation thee = Data_copy (me);

		if (nblocks > 1) {
			autoBOOLVEC occupied = zero_BOOLVEC (blockSize);

			integer posinblock = 1 - offset;
			for (integer i = 1; i <= n; i ++) {
				const integer rblock = (i - 1) % nblocks + 1;

				posinblock += offset;
				if (posinblock > blockSize)
					posinblock -= blockSize;

				if (i % nblocks == 1) {
					integer count = blockSize;
					while (occupied [posinblock] && count > 0) {
						posinblock ++;
						count --;
						if (posinblock > blockSize)
							posinblock -= blockSize;
					}
					occupied [posinblock] = true;
				}
				const integer index = from - 1 + (rblock - 1) * blockSize + posinblock;
				thy p [from - 1 + i] = my p [index];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not interleaved.");
	}
}

integer Permutation_getValueAtIndex (Permutation me, integer i) {
	return i > 0 && i <= my numberOfElements ? my p [i] : -1;
}

integer Permutation_getIndexAtValue (Permutation me, integer value) {
	for (integer i = 1; i <= my numberOfElements; i ++) {
		if (my p [i] == value)
			return i;
	}
	return -1;
}

autoPermutation Permutation_invert (Permutation me) {
	try {
		autoPermutation thee = Data_copy (me);
		Permutation_invert_into (me, thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not inverted.");
	}
}

void Permutation_invert_into (Permutation me, mutablePermutation result) {
	Melder_assert (my numberOfElements == result -> numberOfElements);
	for (integer i = 1; i <= my numberOfElements; i ++)
		result -> p [my p [i]] = i;
}

void Permutation_reverse_inline (Permutation me, integer from, integer to) {
	const integer n = Permutation_checkRange (me, & from, & to);
	for (integer i = 1; i <= n / 2; i ++)
		std::swap (my p [from + i - 1], my p [to - i + 1] );
}

autoPermutation Permutation_reverse (Permutation me, integer from, integer to) {
	try {
		(void) Permutation_checkRange (me, & from, & to);
		autoPermutation thee = Data_copy (me);
		Permutation_reverse_inline (thee.get(), from, to);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not reversed.");
	}
}

/* Replaces p with the next permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/
void Permutation_next_inplace (Permutation me) {
	const integer size = my numberOfElements;
	Melder_require (size > 1,
		U"The permutation should have more than one element.");

	integer *p = & my p [1];
	integer i = size - 2;
	while (p [i] > p [i + 1] && i != 0)
		i --;
	if (i == 0 && p [0] > p [1])
		Melder_throw (U"No next element.");
	integer k = i + 1;
	for (integer j = i + 2; j < size; j ++) {
		if (p [j] > p [i] && p [j] < p [k])
			k = j;
	}
	std::swap (p [i], p [k]);
	for (integer j = i + 1; j <= (size + i) / 2; j ++)
		std::swap (p [j], p [size + i - j]);
}

/* Replaces p with the previous permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/
void Permutation_previous_inplace (Permutation me) {
	const integer size = my numberOfElements;
	Melder_require (size > 1,
		U"The permutation should have more than one element.");

	integer *p = & my p [1];
	integer i = size - 2;
	while ((p [i] < p [i + 1]) && (i != 0))
		i --;
	Melder_require (! (i == 0 && p [0] < p [1]),
		U"No previous element.");
	integer k = i + 1;
	for (integer j = i + 2; j < size; j ++) {
		if (p [j] < p [i] && p [j] > p [k])
			k = j;
	}
	std::swap (p [i], p [k]);
	for (integer j = i + 1; j <= (size + i) / 2; j ++)
		std::swap (p [j], p [size + i - j]);
}

autoPermutation Permutations_multiply2 (Permutation me, Permutation thee) {
	try {
		Melder_require (my numberOfElements == thy numberOfElements,
			U"Number of elements should be equal.");
		autoPermutation him = Data_copy (me);
		Permutations_multiply2_into (me, thee, him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U" not multiplied.");
	}
}

void Permutations_multiply2_into (Permutation me, Permutation thee, mutablePermutation result) {
	Melder_assert (my numberOfElements == thy numberOfElements && my numberOfElements == result -> numberOfElements);
	for (integer i = 1; i <= my numberOfElements; i ++)
		result -> p [i] = my p [thy p [i]];
}

autoPermutation Permutations_multiply (OrderedOf<structPermutation>* me) {
	try {
		Melder_require (my size > 1,
			U"There should be at least two Permutations to multiply.");
		autoPermutation thee = Permutations_multiply2 (my at [1], my at [2]);
		for (integer i = 3; i <= my size; i ++) {
			autoPermutation him = Permutations_multiply2 (thee.get(), my at [i]);
			thee = him.move();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Permutations not multiplied.");
	}
}

static void checkUniqueAndInInterval (constINTVEC const& vec, integer maximum) {
	autoINTVEC sorted = sort_INTVEC (vec);
	Melder_require (sorted [1] >= 1 && vec [sorted.size] <= maximum,
		U"The subset numbers should  be in the interval from 1 to ", maximum, U".");
	for (integer i = 2; i <= sorted.size; i ++)
		if (sorted [i] == sorted [i - 1])
			Melder_throw (U"All numbers in the set should be unique, number ", sorted [i], U" isn't.");
}

void Permutation_permuteVEC_inout (Permutation me, VEC vec) {
		try {
		Melder_require (my numberOfElements == vec.size,
			U"The sizes of the vector and the Permutation should be equal.");
		autoVEC save = copy_VEC (vec);
		for (integer i = 1; i <= my numberOfElements; i ++)
			vec [i] = save [my p [i]];
	} catch (MelderError) {
		Melder_throw (U"Vector not permuted.");
	}

}

void Permutation_permuteINTVEC_inout (Permutation me, INTVEC vec){
		try {
		Melder_require (my numberOfElements == vec.size,
			U"The sizes of the vector and the Permutation should be equal.");
		autoINTVEC save = copy_INTVEC (vec);
		for (integer i = 1; i <= my numberOfElements; i ++)
			vec [i] = save [my p [i]];
	} catch (MelderError) {
		Melder_throw (U"Vector not permuted.");
	}
}

void Permutation_permuteSTRVEC_inout (Permutation me, autoSTRVEC & vec) {
	try {
		Melder_require (my numberOfElements == vec.size,
			U"The sizes of the vector and the Permutation should be equal.");
		autoSTRVEC save = copy_STRVEC (vec.get());
		for (integer i = 1; i <= my numberOfElements; i ++)
			vec [i] = save [my p [i]].move();
	} catch (MelderError) {
		Melder_throw (U"STRVEC not permuted.");
	}
}

void Permutation_permuteSubsetByOther_inout (Permutation me, constINTVEC const& subsetPositions, Permutation other) {
	try {
		autoPermutation thee = Data_copy (me);
		Melder_require (subsetPositions.size == other -> numberOfElements,
			U"The subset and the other Permutation should have the same size.");
		checkUniqueAndInInterval (subsetPositions, my numberOfElements);
		autoINTVEC save = raw_INTVEC (subsetPositions.size);
		for (integer i = 1; i <= subsetPositions.size; i ++)
			save [i] = my p [subsetPositions [i]];
		
		for (integer i = 1; i <= subsetPositions.size; i ++)
			my p [subsetPositions [i]] =  save [other -> p [i]];
	} catch (MelderError) {
		Melder_throw (me, U": not permuted by subset");
	}
}

autoPermutation Permutation_moveElementsToTheFront (Permutation me, constINTVEC const& subsetPositions) {
	try {
		if (subsetPositions.size == 0) {
			return Data_copy (me);
		}
		checkUniqueAndInInterval (subsetPositions, my numberOfElements);
		autoPermutation thee = Data_copy (me);
		for (integer i = 1; i <= subsetPositions.size; i++)
			thy p [i] = my p [subsetPositions [i]];
		integer nextPos = subsetPositions.size;
		// remove the duplicates
		for (integer i = 1; i <= my numberOfElements; i ++) {
			bool inSubset = false;
			for (integer j = 1; j <= subsetPositions.size; j++)
				if (my p [i] == thy p [j]) {
				thy p [++ nextPos] = my p [i];
					inSubset = true;
					break;
				}
			if (! inSubset)
				thy p [++ nextPos] = my p [i];
		}
		Melder_assert (nextPos == my numberOfElements);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not permuted by subset");
	}
}

autoPermutation Permutation_createAsSortingIndex (constSTRVEC const& strvec, kStrings_sorting sorting) {
	try {
		autoPermutation me = Permutation_create (strvec.size, true);
		if (sorting == kStrings_sorting::ALPHABETICAL) 
			INTVECindex_inout (my p.get(), strvec);
		else if (sorting == kStrings_sorting::NUMBER_AWARE)
			INTVECindex_numberAware_inout (my p.get(), strvec);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create Permutation from STRVEC.");
	}	
}

integer Permutation_getNumberOfInversions (Permutation me) {
	autoPermutationInversionCounter pic = PermutationInversionCounter_create (my numberOfElements);
	return pic -> getNumberOfInversions (me);
}

// count the number of permutations between the two
integer Permutations_getNumberOfInversions (Permutation me, Permutation thee) {
	autoPermutation invert = Permutation_invert (me);
	autoPermutation mult = Permutations_multiply2 (invert.get(), thee);
	return Permutation_getNumberOfInversions (mult.get());
}

autoINTVEC Permutation_getAllInversionIndices (Permutation me) {
	try {
		const integer maximumNumberOfInversions = Permutation_getNumberOfInversions (me);
		autoPermutationInversionCounter pic = PermutationInversionCounter_create (my numberOfElements);
		autoINTVEC inversionIndices = raw_INTVEC (2 * maximumNumberOfInversions);
		const integer numberOfInversions = pic -> getInversions (me, inversionIndices.get());
		Melder_assert (numberOfInversions == maximumNumberOfInversions);
		return inversionIndices;
	} catch (MelderError) {
		Melder_throw (me, U"Could not determine all inversions.");
	}
}

autoINTVEC Permutation_getRandomInversionIndices (Permutation me, integer numberOfRandomInversions) {
	try {
		const integer maximumNumberOfInversions = Permutation_getNumberOfInversions (me);
		autoPermutationInversionCounter pic = PermutationInversionCounter_create (my numberOfElements);
		autoINTVEC inversionIndices = raw_INTVEC (2 * numberOfRandomInversions);
		autoINTVEC sortedRandomInversionNumbers = raw_INTVEC (numberOfRandomInversions);
		for (integer i = 1; i <= numberOfRandomInversions; i ++)
			sortedRandomInversionNumbers [i] = NUMrandomInteger (1, maximumNumberOfInversions);
		sort_INTVEC_inout (sortedRandomInversionNumbers.get());
		const integer numberOfInversions = pic -> getSelectedInversions (
			me, sortedRandomInversionNumbers.get(), inversionIndices.get()
		);
		Melder_assert (numberOfInversions == maximumNumberOfInversions);
		return inversionIndices;
	} catch (MelderError) {
		Melder_throw (me, U"Could not determine random inversion indices.");
	}
}

autoMAT Permutation_getRandomInversions (Permutation me, integer numberOfRandomInversions) {
	try {
		autoMAT inversions;
		autoINTVEC inversionIndices = Permutation_getRandomInversionIndices (me, numberOfRandomInversions);
		if (inversionIndices.size > 0) {
			const integer numberOfPairs = inversionIndices.size / 2;
			inversions = raw_MAT (numberOfPairs, 2_integer);
			for (integer i = 1; i <= numberOfPairs; i ++) {
				inversions [i] [1] = inversionIndices [2 * i];		// ihigh
				inversions [i] [2] = inversionIndices [2 * i - 1];	// ilow
				if (inversions [i] [1] < inversions [i] [2])
					std::swap (inversions [i] [1], inversions [i] [2]);
			}
		}
		return inversions;
	} catch (MelderError) {
		Melder_throw (me, U"Could not determine random inversions.");
	}
}

autoMAT Permutation_getAllInversions (Permutation me) {
	try {
		autoMAT inversions;
		autoINTVEC inversionIndices = Permutation_getAllInversionIndices (me);
		if (inversionIndices.size > 0) {
			const integer numberOfPairs = inversionIndices.size / 2;
			autoINTVEC columnNumbers {1,2};
			inversions = raw_MAT (numberOfPairs, 2_integer);
			for (integer i = 1; i <= numberOfPairs; i ++) {
				inversions [i] [1] = inversionIndices [2 * i];		// ihigh
				inversions [i] [2] = inversionIndices [2 * i - 1];	// ilow
				if (inversions [i] [1] < inversions [i] [2])
					std::swap (inversions [i] [1], inversions [i] [2]);
			}
			autoMAT sorted = sortRows_MAT (inversions.get(), columnNumbers.get());
			inversions.get()  <<=  sorted.get();
		}
		return inversions;
	} catch (MelderError) {
		Melder_throw (me, U": Could not determine inversions.");
	}
}

void Permutations_swap (Permutation me, Permutation thee) {
	Melder_assert (my numberOfElements == thy numberOfElements);
	INTVEC tmp1 = my p.releaseToAmbiguousOwner ();
	INTVEC tmp2 = thy p.releaseToAmbiguousOwner ();
	my p.adoptFromAmbiguousOwner (tmp2);
	thy p.adoptFromAmbiguousOwner (tmp1);
}

/* End of Permutation.cpp */
