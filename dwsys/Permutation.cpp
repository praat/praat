/* Permutation.c
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
#include "Permutation.h"

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

static long Permutation_checkRange (Permutation me, long *from, long *to)
{
	try {
		if ((*from < 0 || *from > my numberOfElements) || (*to < 0 || *to > my numberOfElements))
			Melder_throw (L"Range must be in [1, ", my numberOfElements, "].");
		if (*from == 0) *from = 1;
		if (*to == 0) *to = my numberOfElements;
		return *to - *from + 1;
	} catch (MelderError) { rethrowzero; }
}

int Permutation_checkInvariant (Permutation me)
{
	try {
		Permutation thee = (Permutation) Data_copy (me); therror
		NUMsort_l (thy numberOfElements, thy p);
		for (long i = 1; i <= my numberOfElements; i++)
		{
			if (thy p[i] != i) therror
		}
		forget (thee);
		return 1;
	} catch (MelderError) { rethrowmzero ("Not a valid permutation."); }
}

static void info (I)
{
	iam (Permutation);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of elements: ", Melder_integer (my numberOfElements));
}

static int readText (I, MelderReadText text)
{
	try {
		iam (Permutation);
		my numberOfElements = texgeti4 (text);
		if (my numberOfElements < 1) Melder_throw (L"(Permutation::readText:) Number of elements must be >= 1.");
		my p = NUMlvector_readText_i4 (1, my numberOfElements, text, "p"); therror
		Permutation_checkInvariant (me);
		return 1;
	} catch (MelderError) { rethrowzero; }
}

class_methods (Permutation, Data)
	class_method_local (Permutation, destroy)
	class_method_local (Permutation, copy)
	class_method_local (Permutation, equal)
	class_method_local (Permutation, canWriteAsEncoding)
	class_method_local (Permutation, writeText)
	class_method_local (Permutation, writeBinary)
	class_method (readText)
	class_method (info)
	class_method_local (Permutation, readBinary)
	class_method_local (Permutation, description)
class_methods_end

void Permutation_init (Permutation me, long numberOfElements)
{
	try {
		my numberOfElements = numberOfElements;
		my p = NUMvector<long> (1, numberOfElements);
		Permutation_sort (me);
	} catch (MelderError) { rethrow; }
}

Permutation Permutation_create (long numberOfElements)
{
	try {
		autoPermutation me = Thing_new (Permutation);
		Permutation_init (me.peek(), numberOfElements);
		return me.transfer();
	} catch (MelderError) { rethrowmzero ("Permulation not created."); }
}

void Permutation_sort (Permutation me)
{
    for (long i = 1; i <= my numberOfElements; i++)
    {
    	my p[i] = i;
    }
}

int Permutation_swapPositions (Permutation me, long i1, long i2)
{
	if (i1 < 1 || i1 > my numberOfElements ||
		i2 < 1 || i2 > my numberOfElements) return 0;
	long tmp = my p[i1];
	my p[i1] = my p[i2];
	my p[i2] = tmp;
	return 1;
}

int Permutation_swapNumbers (Permutation me, long i1, long i2)
{
	long ip = 0;
	if (i1 < 1 || i1 > my numberOfElements ||
		i2 < 1 || i2 > my numberOfElements) return 0;
	if (i1 == i2) return 1;
	for (long i = 1; i <= my numberOfElements; i++)
	{
		if (my p[i] == i1) { my p[i] = i2; ip++; }
		else if (my p[i] == i2) { my p[i] = i1; ip++; }
		if (ip == 2) break;
	}
	Melder_assert (ip == 2);
	return 1;
}

int Permutation_swapBlocks (Permutation me, long from, long to, long blocksize)
{
	try {
		if (blocksize < 1 || blocksize > my numberOfElements) Melder_throw
		("Blocksize must be in [1, %d] range.", my numberOfElements / 2);
		if (from < 0 || from + blocksize - 1 > my numberOfElements || to < 0 || to + blocksize - 1 > my numberOfElements)
		Melder_throw (L"Start and finish positions of the two blocks must be in [1,", my numberOfElements, "] range.");
		if (from == to) return 1;

		for (long i = 1; i <= blocksize; i++)
		{
			long tmp = my p[from + i - 1];
			my p[from + i - 1] = my p[to + i - 1];
			my p[to + i - 1] = tmp;
		}
		return 1;
	} catch (MelderError) { rethrowzero; }
}

int Permutation_permuteRandomly_inline (Permutation me, long from, long to)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror

		if (n == 1) return 1;
		for (long i = from; i < to; i++)
		{
			long newpos = NUMrandomInteger (from, to);
			long pi = my p[i];
			my p[i] = my p[newpos];
			my p[newpos] = pi;
		}
		return 1;
	} catch (MelderError) { rethrowzero; }
}

Permutation Permutation_permuteRandomly (Permutation me, long from, long to)
{
	try {
		autoPermutation thee = (Permutation) Data_copy (me); therror
		Permutation_permuteRandomly_inline (thee.peek(), from, to); therror
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Permutation not permuted."); };
}

Permutation Permutation_rotate (Permutation me, long from, long to, long step)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror
		step = (step - 1) % n + 1;

		autoPermutation thee = (Permutation) Data_copy (me);
		for (long i = from; i <= to; i++)
		{
			long ifrom = i + step;
			if (ifrom > to) ifrom -= n;
			if (ifrom < from) ifrom += n;
			thy p[ifrom] = my p[i];
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Permutation not rotated."); }
}

int Permutation_swapOneFromRange (Permutation me, long from, long to, long pos, int forbidsame)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror
		long newpos = NUMrandomInteger (from, to);
		if (newpos == pos && forbidsame)
		{
			if (n == 1) Melder_throw (L"Impossible to satisfy \"forbid same\" constraint within the chosen range.");
			while ((newpos = NUMrandomInteger (from, to)) == pos) ;
		}

		long tmp = my p[pos]; my p[pos] = my p[newpos]; my p[newpos] = tmp;
		return 1;
	} catch (MelderError) { rethrowzero; }
}


Permutation Permutation_permuteBlocksRandomly (Permutation me, long from, long to, long blocksize,
	int permuteWithinBlocks, int noDoublets)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror
		if (blocksize == 1 || (blocksize >= n && permuteWithinBlocks))
		{
			autoPermutation thee = Permutation_permuteRandomly (me, from, to);
			return thee.transfer();
		}
		autoPermutation thee = (Permutation) Data_copy (me);
		if (blocksize >= n) return thee.transfer();

		long nblocks  = n / blocksize, nrest = n % blocksize;
		if (nrest != 0) Melder_throw ("It is not possible to fit an integer number of blocks "
		"in the range.\n(The last block is only of size ", nrest, ").");

		autoPermutation pblocks = Permutation_create (nblocks);

		Permutation_permuteRandomly_inline (pblocks.peek(), 1, nblocks);
		long first = from;
		for (long iblock = 1; iblock <= nblocks; iblock++, first += blocksize)
		{
			/* (n1,n2,n3,...) means: move block n1 to position 1 etc... */
			long blocktomove = Permutation_getValueAtIndex (pblocks.peek(), iblock);

			for (long j = 1; j <= blocksize; j++)
			{
				thy p[first - 1 + j] = my p[from - 1 + (blocktomove - 1) * blocksize + j];
			}

			if (permuteWithinBlocks)
			{
				long last = first + blocksize - 1;
				Permutation_permuteRandomly_inline (thee.peek(), first, last); therror
				if (noDoublets && iblock > 0 && (thy p[first - 1] % blocksize) == (thy p[first] % blocksize))
				 Permutation_swapOneFromRange (thee.peek(), first+1, last, first, 0); therror
			}
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Permutation not permuted block randomly."); }
}

Permutation Permutation_interleave (Permutation me, long from, long to, long blocksize, long offset)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror
		long nblocks = n / blocksize;
		long nrest = n % blocksize;
		if (nrest != 0) Melder_throw ("There is not an integer number of blocks in the range.\n"
			"(The last block is only of size ", nrest, L" instead of ", blocksize, ").");
		if (offset >= blocksize) Melder_throw (L"Offset must be smaller than blocksize.");

		autoPermutation thee = (Permutation) Data_copy (me);

		if (nblocks == 1) return thee.transfer();

		autoNUMvector<long> occupied (1, blocksize);

		long posinblock = 1 - offset;
		for (long i = 1; i <= n; i++)
		{
			long index, rblock = (i - 1) % nblocks + 1;

			posinblock += offset;
			if (posinblock > blocksize) posinblock -= blocksize;

			if (i % nblocks == 1)
			{
				long count = blocksize;
				while (occupied[posinblock] == 1 && count > 0)
				{
					posinblock++; count--;
					if (posinblock > blocksize) posinblock -= blocksize;
				}
				occupied[posinblock] = 1;
			}
			index = from - 1 + (rblock - 1) * blocksize + posinblock;
			thy p[from - 1 + i] = my p[index];
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Permutation not interleaved."); }
}

long Permutation_getValueAtIndex (Permutation me, long i)
{
    return i > 0 && i <= my numberOfElements ? my p[i] : -1;
}

long Permutation_getIndexAtValue (Permutation me, long value)
{
	for (long i = 1; i <= my numberOfElements; i++)
	{
		if (my p[i] == value) return i;
	}
	return -1;
}

Permutation Permutation_invert (Permutation me)
{
	try {
		autoPermutation thee = (Permutation) Data_copy (me);
		for (long i = 1; i <= my numberOfElements; i++)
		{
			thy p[my p[i]] = i;
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Permutation not inverted."); }
}

Permutation Permutation_reverse (Permutation me, long from, long to)
{
	try {
		long n = Permutation_checkRange (me, &from, &to); therror
		autoPermutation thee = (Permutation) Data_copy (me);
		for (long i = 1; i <= n; i++)
		{
			thy p[from + i - 1] = my p[to - i + 1];
		}
		return thee.transfer();
		
	} catch (MelderError) { rethrowmzero ("Permutation not reversed."); }
}

/* Replaces p with the next permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/
int Permutation_next_inline (Permutation me)
{

	long size = my numberOfElements;
	long *p = & my p[1];

	if (size < 2) return 0;

	long i = size - 2;

	while ((p[i] > p[i + 1]) && (i != 0)) { i--; }

	if ((i == 0) && (p[0] > p[1])) return 0;

	long k = i + 1;

	for (long j = i + 2; j < size; j++ )
    {
		if ((p[j] > p[i]) && (p[j] < p[k])) { k = j; }
    }

	long tmp = p[i]; p[i] = p[k]; p[k] = tmp;

	for (long j = i + 1; j <= ((size + i) / 2); j++)
    {
		tmp = p[j];
		p[j] = p[size + i - j];
		p[size + i - j] = tmp;
    }

	return 1;
}

/* Replaces p with the previous permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/

int Permutation_previous_inline (Permutation me)
{
	long size = my numberOfElements;
	long *p = & my p[1];

	if (size < 2) return 0;

	long i = size - 2;

	while ((p[i] < p[i + 1]) && (i != 0)) { i--; }

	if ((i == 0) && (p[0] < p[1])) return 0;

	long k = i + 1;

	for (long j = i + 2; j < size; j++ )
    {
		if ((p[j] < p[i]) && (p[j] > p[k])) { k = j; }
    }

	long tmp = p[i]; p[i] = p[k]; p[k] = tmp;

	for (long j = i + 1; j <= ((size + i) / 2); j++)
    {
		tmp = p[j];
		p[j] = p[size + i - j];
		p[size + i - j] = tmp;
    }

	return 1;
}

/* End of Permutation.c */
