/* Permutation.cpp
 *
 * Copyright (C) 2005-2012 David Weenink
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

Thing_implement (Permutation, Data, 0);

static long Permutation_checkRange (Permutation me, long *from, long *to) {
	if ( (*from < 0 || *from > my numberOfElements) || (*to < 0 || *to > my numberOfElements)) {
		Melder_throw (L"Range must be in [1, ", my numberOfElements, "].");
	}
	if (*from == 0) {
		*from = 1;
	}
	if (*to == 0) {
		*to = my numberOfElements;
	}
	return *to - *from + 1;
}

void Permutation_checkInvariant (Permutation me) {
	autoPermutation thee = Data_copy (me);
	NUMsort_l (thy numberOfElements, thy p);
	for (long i = 1; i <= my numberOfElements; i++) {
		if (thy p[i] != i) {
			Melder_throw (me, ":not a valid permutation.");
		}
	}
}

void structPermutation :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of elements: ", Melder_integer (numberOfElements));
}

void structPermutation :: v_readText (MelderReadText text) {
	numberOfElements = texgeti4 (text);
	if (numberOfElements < 1) {
		Melder_throw (L"Found a negative mumber of elements during reading.");
	}
	p = NUMvector_readText_i4 (1, numberOfElements, text, "p");
	Permutation_checkInvariant (this);
}

void Permutation_init (Permutation me, long numberOfElements) {
	my numberOfElements = numberOfElements;
	my p = NUMvector<long> (1, numberOfElements);
	Permutation_sort (me);
}

Permutation Permutation_create (long numberOfElements) {
	try {
		autoPermutation me = Thing_new (Permutation);
		Permutation_init (me.peek(), numberOfElements);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Permulation not created.");
	}
}

void Permutation_sort (Permutation me) {
	for (long i = 1; i <= my numberOfElements; i++) {
		my p[i] = i;
	}
}

void Permutation_swapPositions (Permutation me, long i1, long i2) {
	try {
		if (i1 < 1 || i1 > my numberOfElements || i2 < 1 || i2 > my numberOfElements) {
			Melder_throw ("Invalid positions.");
		}
		long tmp = my p[i1];
		my p[i1] = my p[i2];
		my p[i2] = tmp;
	} catch (MelderError) {
		Melder_throw (me, ":positions not swapped.");
	}
}

void Permutation_swapNumbers (Permutation me, long i1, long i2) {
	try {
		long ip = 0;
		if (i1 < 1 || i1 > my numberOfElements || i2 < 1 || i2 > my numberOfElements) {
			Melder_throw ("");
		}
		if (i1 == i2) {
			return;
		}
		for (long i = 1; i <= my numberOfElements; i++) {
			if (my p[i] == i1) {
				my p[i] = i2;
				ip++;
			} else if (my p[i] == i2) {
				my p[i] = i1;
				ip++;
			}
			if (ip == 2) {
				break;
			}
		}
		Melder_assert (ip == 2);
	} catch (MelderError) {
		Melder_throw (me, ": numbers not swapped.");
	}
}

void Permutation_swapBlocks (Permutation me, long from, long to, long blocksize) {
	try {
		if (blocksize < 1 || blocksize > my numberOfElements) Melder_throw
			("Blocksize must be in [1, %d] range.", my numberOfElements / 2);
		if (from < 0 || from + blocksize - 1 > my numberOfElements || to < 0 || to + blocksize - 1 > my numberOfElements) {
			Melder_throw (L"Start and finish positions of the two blocks must be in [1,", my numberOfElements, "] range.");
		}
		if (from == to) {
			return;
		}

		for (long i = 1; i <= blocksize; i++) {
			long tmp = my p[from + i - 1];
			my p[from + i - 1] = my p[to + i - 1];
			my p[to + i - 1] = tmp;
		}
	} catch (MelderError) {
		Melder_throw (me, ": blocks not swapped.");
	}
}

void Permutation_permuteRandomly_inline (Permutation me, long from, long to) {
	try {
		long n = Permutation_checkRange (me, &from, &to);

		if (n == 1) {
			return;
		}
		for (long i = from; i < to; i++) {
			long newpos = NUMrandomInteger (from, to);
			long pi = my p[i];
			my p[i] = my p[newpos];
			my p[newpos] = pi;
		}
	} catch (MelderError) {
		Melder_throw (me, ": not permuted randomly.");
	}
}

Permutation Permutation_permuteRandomly (Permutation me, long from, long to) {
	try {
		autoPermutation thee = Data_copy (me);
		Permutation_permuteRandomly_inline (thee.peek(), from, to);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not permuted.");
	}
}

Permutation Permutation_rotate (Permutation me, long from, long to, long step) {
	try {
		long n = Permutation_checkRange (me, &from, &to);
		step = (step - 1) % n + 1;

		autoPermutation thee = Data_copy (me);
		for (long i = from; i <= to; i++) {
			long ifrom = i + step;
			if (ifrom > to) {
				ifrom -= n;
			}
			if (ifrom < from) {
				ifrom += n;
			}
			thy p[ifrom] = my p[i];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not rotated.");
	}
}

void Permutation_swapOneFromRange (Permutation me, long from, long to, long pos, int forbidsame) {
	try {
		long n = Permutation_checkRange (me, &from, &to);
		long newpos = NUMrandomInteger (from, to);
		if (newpos == pos && forbidsame) {
			if (n == 1) {
				Melder_throw (L"Impossible to satisfy \"forbid same\" constraint within the chosen range.");
			}
			while ( (newpos = NUMrandomInteger (from, to)) == pos) {
				;
			}
		}

		long tmp = my p[pos]; my p[pos] = my p[newpos]; my p[newpos] = tmp;
	} catch (MelderError) {
		Melder_throw (me, ": one from range not swapped.");
	}
}


Permutation Permutation_permuteBlocksRandomly (Permutation me, long from, long to, long blocksize,
        int permuteWithinBlocks, int noDoublets) {
	try {
		long n = Permutation_checkRange (me, &from, &to);
		if (blocksize == 1 || (blocksize >= n && permuteWithinBlocks)) {
			autoPermutation thee = Permutation_permuteRandomly (me, from, to);
			return thee.transfer();
		}
		autoPermutation thee = Data_copy (me);
		if (blocksize >= n) {
			return thee.transfer();
		}

		long nblocks  = n / blocksize, nrest = n % blocksize;
		if (nrest != 0) Melder_throw ("It is not possible to fit an integer number of blocks "
			                              "in the range.\n(The last block is only of size ", nrest, ").");

		autoPermutation pblocks = Permutation_create (nblocks);

		Permutation_permuteRandomly_inline (pblocks.peek(), 1, nblocks);
		long first = from;
		for (long iblock = 1; iblock <= nblocks; iblock++, first += blocksize) {
			/* (n1,n2,n3,...) means: move block n1 to position 1 etc... */
			long blocktomove = Permutation_getValueAtIndex (pblocks.peek(), iblock);

			for (long j = 1; j <= blocksize; j++) {
				thy p[first - 1 + j] = my p[from - 1 + (blocktomove - 1) * blocksize + j];
			}

			if (permuteWithinBlocks) {
				long last = first + blocksize - 1;
				Permutation_permuteRandomly_inline (thee.peek(), first, last);
				if (noDoublets && iblock > 0 && (thy p[first - 1] % blocksize) == (thy p[first] % blocksize)) {
					Permutation_swapOneFromRange (thee.peek(), first + 1, last, first, 0);
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not permuted block randomly.");
	}
}

Permutation Permutation_interleave (Permutation me, long from, long to, long blocksize, long offset) {
	try {
		long n = Permutation_checkRange (me, &from, &to);
		long nblocks = n / blocksize;
		long nrest = n % blocksize;
		if (nrest != 0) Melder_throw ("There is not an integer number of blocks in the range.\n"
			                              "(The last block is only of size ", nrest, L" instead of ", blocksize, ").");
		if (offset >= blocksize) {
			Melder_throw (L"Offset must be smaller than blocksize.");
		}

		autoPermutation thee = Data_copy (me);

		if (nblocks == 1) {
			return thee.transfer();
		}

		autoNUMvector<long> occupied (1, blocksize);

		long posinblock = 1 - offset;
		for (long i = 1; i <= n; i++) {
			long index, rblock = (i - 1) % nblocks + 1;

			posinblock += offset;
			if (posinblock > blocksize) {
				posinblock -= blocksize;
			}

			if (i % nblocks == 1) {
				long count = blocksize;
				while (occupied[posinblock] == 1 && count > 0) {
					posinblock++; count--;
					if (posinblock > blocksize) {
						posinblock -= blocksize;
					}
				}
				occupied[posinblock] = 1;
			}
			index = from - 1 + (rblock - 1) * blocksize + posinblock;
			thy p[from - 1 + i] = my p[index];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not interleaved.");
	}
}

long Permutation_getValueAtIndex (Permutation me, long i) {
	return i > 0 && i <= my numberOfElements ? my p[i] : -1;
}

long Permutation_getIndexAtValue (Permutation me, long value) {
	for (long i = 1; i <= my numberOfElements; i++) {
		if (my p[i] == value) {
			return i;
		}
	}
	return -1;
}

Permutation Permutation_invert (Permutation me) {
	try {
		autoPermutation thee = Data_copy (me);
		for (long i = 1; i <= my numberOfElements; i++) {
			thy p[my p[i]] = i;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not inverted.");
	}
}

Permutation Permutation_reverse (Permutation me, long from, long to) {
	try {
		long n = Permutation_checkRange (me, &from, &to);
		autoPermutation thee = Data_copy (me);
		for (long i = 1; i <= n; i++) {
			thy p[from + i - 1] = my p[to - i + 1];
		}
		return thee.transfer();

	} catch (MelderError) {
		Melder_throw (me, ": not reversed.");
	}
}

/* Replaces p with the next permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/
void Permutation_next_inline (Permutation me) {
	long size = my numberOfElements;
	long *p = & my p[1];

	if (size < 2) {
		Melder_throw ("Only one element.");
	}

	long i = size - 2;

	while ( (p[i] > p[i + 1]) && (i != 0)) {
		i--;
	}

	if ( (i == 0) && (p[0] > p[1])) {
		Melder_throw ("No next.");
	}

	long k = i + 1;

	for (long j = i + 2; j < size; j++) {
		if ( (p[j] > p[i]) && (p[j] < p[k])) {
			k = j;
		}
	}

	long tmp = p[i]; p[i] = p[k]; p[k] = tmp;

	for (long j = i + 1; j <= ( (size + i) / 2); j++) {
		tmp = p[j];
		p[j] = p[size + i - j];
		p[size + i - j] = tmp;
	}
}

/* Replaces p with the previous permutation (in the standard lexicographical ordering.
   Adapted from the GSL library
*/

void Permutation_previous_inline (Permutation me) {
	long size = my numberOfElements;
	long *p = & my p[1];

	if (size < 2) {
		Melder_throw ("Only one element.");
	}

	long i = size - 2;

	while ( (p[i] < p[i + 1]) && (i != 0)) {
		i--;
	}

	if ( (i == 0) && (p[0] < p[1])) {
		Melder_throw ("No previous");
	}

	long k = i + 1;

	for (long j = i + 2; j < size; j++) {
		if ( (p[j] < p[i]) && (p[j] > p[k])) {
			k = j;
		}
	}

	long tmp = p[i]; p[i] = p[k]; p[k] = tmp;

	for (long j = i + 1; j <= ( (size + i) / 2); j++) {
		tmp = p[j];
		p[j] = p[size + i - j];
		p[size + i - j] = tmp;
	}
}

Permutation Permutations_multiply2 (Permutation me, Permutation thee) {
	try {
		if (my numberOfElements != thy numberOfElements) {
			Melder_throw ("Number of elements must be equal.");
		}
		autoPermutation him = Data_copy (me);
		for (long i = 1; i <= my numberOfElements; i++) {
			his p[i] = my p[thy p[i]];
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, " not multiplied.");
	}
}

Permutation Permutations_multiply (Collection me) {
	try {
		if (my size < 2) {
			Melder_throw ("There must be at least 2 Permutations in the set.");
		}
		autoPermutation thee = Permutations_multiply2 ( (Permutation) my item[1], (Permutation) my item[2]);
		for (long i = 3; i <= my size; i++) {
			thee.reset (Permutations_multiply2 (thee.peek(), (Permutation) my item[i]));
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Permutations not multiplied.");
	}
}

/* End of Permutation.c */
