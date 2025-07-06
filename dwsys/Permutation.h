#ifndef _Permutation_h_
#define _Permutation_h_
/* Permutation.h
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

#include "melder.h"
#include "Collection.h"
#include "Graphics.h"
#include "NUMsorting.h"

#include "Permutation_def.h"

/*
	Class invariant: any permutation equals the identity permutation after all its elements are sorted ascendingly.
	Alternative: a permutation of size n contains all numbers from 1 to n.
*/

void Permutation_init (Permutation me, integer numberOfElements);

autoPermutation Permutation_createSimplePermutation (constINTVEC const& numbers);

void Permutation_tableJump_inline (Permutation me, integer jumpSize, integer first);

autoPermutation Permutation_create (integer numberOfElements, bool identity);
/*
	Create the Permutation data structure and fill
		with the identical permutation (1,2,..n) if identity is true
		else create a random permutation
*/

autoPermutation Permutation_createAsSortingIndex (constSTRVEC const& strvec, kStrings_sorting sorting);

void Permutation_drawAsLine (Permutation me, Graphics g, bool garnish);

void Permutation_checkInvariant (Permutation me);
/* Check that the elements, if sorted ascendingly, are exactly equal to the identity (1,2,...). */


void Permutation_sort (Permutation me);
/* Set p [1..n] = 1,..n */

integer Permutation_getNumberOfInversions (Permutation me);
/*
	Count the number of times when i < j && p [i] >  p [j].
	E.g. {1,2,3} -> 0 iversions; {3,2,1} -> 3; {1,3,2} -> 1.
 */

autoINTVEC Permutation_getRandomInversionIndices (Permutation me, integer maxNumberOfRandomInversions);
autoINTVEC Permutation_getAllInversionIndices (Permutation me);

autoMAT Permutation_getRandomInversions (Permutation me, integer maxNumberOfRandomInversions); /* convenience for interface */
autoMAT Permutation_getAllInversions (Permutation me); /* convenience for interface */

void Permutation_permuteRandomly_inplace (Permutation me, integer from, integer to);

autoPermutation Permutation_permuteRandomly (Permutation me, integer from, integer to);
/* Generate a new sequence by permuting the elements from..to */

autoPermutation Permutation_rotate (Permutation me, integer from, integer to, integer step);

void Permutation_swapOneFromRange (Permutation me, integer from, integer to, integer pos, bool forbidsame);
/* Swap item at pos with one randomly chosen in interval [from,to]. If pos in [from,to]
	and forbidsame==true then new position may not be equal to pos. */

void Permutation_swapBlocks (Permutation me, integer from, integer to, integer blocksize);
/* Swap two blocks */

void Permutation_swapPositions (Permutation me, integer i1, integer i2);

void Permutation_swapNumbers (Permutation me, integer i1, integer i2);

autoPermutation Permutation_interleave (Permutation me, integer from, integer to, integer blocksize, integer offset);

autoPermutation Permutation_permuteBlocksRandomly (Permutation me, integer from, integer to, integer blocksize, bool permuteWithinBlocks, bool noDoublets);
/* Permute blocks of size blocksize randomly. If permuteWithinBlocks=true and noDoublets=true forbid that the last
	number in a block and the first number in the following block are 'equal modulo blocksize'. */

integer Permutation_getValueAtIndex (Permutation me, integer i);
/* return i > 0 && i < my n ? my p [i] : -1 */

integer Permutation_getIndexAtValue (Permutation me, integer value);
/* Find i for which p [i] = value */

autoPermutation Permutation_invert (Permutation me);
void Permutation_invert_into (Permutation me, mutablePermutation result);
/*  */

void Permutation_reverse_inline (Permutation me, integer from, integer to);
autoPermutation Permutation_reverse (Permutation me, integer from, integer to);
/* (n1,n2,...nn) to (nn,...n2,n1) */

void Permutation_next_inplace (Permutation me);

void Permutation_previous_inplace (Permutation me);

autoPermutation Permutations_multiply2 (Permutation me, Permutation thee);
void Permutations_multiply2_into (Permutation me, Permutation thee, mutablePermutation result);

autoPermutation Permutations_multiply (OrderedOf<structPermutation>* me);

autoPermutation Permutation_permutePartByOther (Permutation me, integer startPosition, Permutation other);

void Permutation_permuteSubsetByOther_inout (Permutation me, constINTVEC const& subsetPositions, Permutation other);

void Permutation_permuteVEC_inout (Permutation me, VEC vec);
void Permutation_permuteINTVEC_inout (Permutation me, INTVEC vec);

void Permutation_permuteSTRVEC_inout (Permutation me, autoSTRVEC & vec); // special

autoPermutation Permutation_moveElementsToTheFront (Permutation me, constINTVEC const& subsetPositions);

void Permutations_swap (Permutation me, Permutation thee);

#endif /* _Permutation_h_ */
