#ifndef _Permutation_h_
#define _Permutation_h_
/* Permutation.h
 *
 * Copyright (C) 2005-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Collection.h"

#include "Permutation_def.h"
oo_CLASS_CREATE (Permutation, Data);

/*
	Class invariant: any permutation equals the identity permutation after all its elements are sorted ascendingly.
*/


void Permutation_init (Permutation me, long numberOfElements);

Permutation Permutation_create (long numberOfElements);
/*
	Create the Permutation data structure and fill
		with the identical permutation (1,2,..n)
*/

void Permutation_checkInvariant (Permutation me);
/* Check that the elements, if sorted ascendingly, are exactly equal to the identity (1,2,...). */


void Permutation_sort (Permutation me);
/* Set p[1..n]=1,..n */

void Permutation_permuteRandomly_inline (Permutation me, long from, long to);

Permutation Permutation_permuteRandomly (Permutation me, long from, long to);
/* Generate a new sequence by permuting the elements from..to */

Permutation Permutation_rotate (Permutation me, long from, long to, long step);

void Permutation_swapOneFromRange (Permutation me, long from, long to, long pos, int forbidsame);
/* Swap item at pos with one randomly chosen in interval [from,to]. If pos in [from,to]
	and forbidsame==true then new position may not be equal to pos. */

void Permutation_swapBlocks (Permutation me, long from, long to, long blocksize);
/* Swap two blocks */

void Permutation_swapPositions (Permutation me, long i1, long i2);
void Permutation_swapNumbers (Permutation me, long i1, long i2);
Permutation Permutation_interleave (Permutation me, long from, long to, long blocksize, long offset);

Permutation Permutation_permuteBlocksRandomly (Permutation me, long from, long to, long blocksize,
	int permuteWithinBlocks, int noDoublets);
/* Permute blocks of size blocksize randomly. If permuteWithinBlocks=true and noDoublets=true forbid that the last
	number in a block and the first number in the following block are 'equal modulo blocksize'. */

long Permutation_getValueAtIndex (Permutation me, long i);
/* return i > 0 && i < my n ? my p[i] : -1 */

long Permutation_getIndexAtValue (Permutation me, long value);
/* Find i for which p[i] = value */

Permutation Permutation_invert (Permutation me);
/*  */

Permutation Permutation_reverse (Permutation me, long from, long to);
/* (n1,n2,...nn) to (nn,...n2,n1) */

void Permutation_next_inline (Permutation me);
void Permutation_previous_inline (Permutation me);

Permutation Permutations_multiply2 (Permutation me, Permutation thee);
Permutation Permutations_multiply (Collection me);

#endif /* _Permutation_h_ */
