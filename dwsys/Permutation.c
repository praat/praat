/* Permutation.c
 *
 * Copyright (C) 2005-2007 David Weenink
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
*/

#include <time.h>
#include "Permutation.h"

#include "oo_DESTROY.h"
#include "Permutation_def.h"
#include "oo_COPY.h"
#include "Permutation_def.h"
#include "oo_EQUAL.h"
#include "Permutation_def.h"
#include "oo_WRITE_TEXT.h"
#include "Permutation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Permutation_def.h"
#include "oo_READ_BINARY.h"
#include "Permutation_def.h"
#include "oo_DESCRIPTION.h"
#include "Permutation_def.h"

static int _Permutation_checkRange (Permutation me, long *from, long *to, long *n, char *proc)
{
	if ((*from < 0 || *from > my numberOfElements) ||
		(*to < 0 || *to > my numberOfElements)) return Melder_error ("%s: Range must be in [1, %d]", proc, my numberOfElements);
	if (*from == 0) *from = 1;
	if (*to == 0) *to = my numberOfElements;
	*n = *to - *from + 1;
	return 1;
}

int Permutation_checkInvariant (Permutation me)
{
	long i;
	Permutation thee = Data_copy (me);
	if (thee == NULL) return 0;
	
	NUMsort_l (thy numberOfElements, thy p);
	for (i = 1; i <= my numberOfElements; i++)
	{
		if (thy p[i] != i) return 0;
	}
	forget (thee);
	return 1;
}

static void info (I)
{
	iam (Permutation);
	classData -> info (me);
	MelderInfo_writeLine2 ("Number of elements: ", Melder_integer (my numberOfElements));
}

static int readText (I, MelderFile file)
{
	iam (Permutation);
	my numberOfElements = texgeti4 (file);
	if (my numberOfElements < 1) return Melder_error ("(Permutation::readText:) Number of elements must be >= 1.");
	if (! (my p = NUMlvector_readText (1, my numberOfElements, file, "p"))) return 0;
	if (! Permutation_checkInvariant (me)) return Melder_error
		("(Permutation::readText:) All values must be unique and in the [1, %d] range.", my numberOfElements);
	return 1;
}

class_methods (Permutation, Data)
	class_method_local (Permutation, destroy)
	class_method_local (Permutation, copy)
	class_method_local (Permutation, equal)
	class_method_local (Permutation, writeText)
	class_method_local (Permutation, writeBinary)
	class_method (readText)
	class_method (info)
	class_method_local (Permutation, readBinary)
	class_method_local (Permutation, description)
class_methods_end

int Permutation_init (Permutation me, long numberOfElements)
{
    my numberOfElements = numberOfElements;
    my p = NUMlvector (1, numberOfElements);
    if (my p == NULL) return 0;
    Permutation_sort (me);
    return 1;
}

Permutation Permutation_create (long numberOfElements)
{
    Permutation me = new (Permutation);
    if (me == NULL || ! Permutation_init (me, numberOfElements)) forget (me);
    return me;
}

void Permutation_sort (Permutation me)
{
	long i;
	
    for (i = 1; i <= my numberOfElements; i++)
    {
    	my p[i] = i;
    }
}

int Permutation_swapBlocks (Permutation me, long from, long to, long blocksize)
{
	char *proc = "Permutation_swap";
	long i;

	if (blocksize < 1 || blocksize > my numberOfElements) return Melder_error
		("%s: Blocksize must be in [1, %d] range.", proc, my numberOfElements / 2);
	if (from < 0 || from + blocksize - 1 > my numberOfElements || to < 0 || to + blocksize - 1 > my numberOfElements)
		return Melder_error	("%s: Start and finish positions of the two blocks must be in [1,%d] range.",
		proc, my numberOfElements);
	if (from == to) return 1;
	
	for (i = 1; i <= blocksize; i++)
	{
		long tmp = my p[from + i - 1];
		my p[from + i - 1] = my p[to + i - 1];
		my p[to + i - 1] = tmp;
	}
	return 1;
}

int Permutation_permuteRandomly_inline (Permutation me, long from, long to)
{
	long i, n;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_permuteRandomly")) return 0;
	
	if (n == 1) return 1;
	for (i = from; i < to; i++)
	{
		long newpos = NUMrandomInteger (from, to);
		long pi = my p[i];
		my p[i] = my p[newpos];
		my p[newpos] = pi;
	}
	return 1;
}

Permutation Permutation_permuteRandomly (Permutation me, long from, long to)
{
    Permutation thee = Data_copy (me);

	if (thee == NULL) return NULL;
	if (! Permutation_permuteRandomly_inline (thee, from, to)) forget (thee);
    return thee;
}

Permutation Permutation_rotate (Permutation me, long from, long to, long step)
{
	char *proc = "Permutation_rotate";
	long i, n;
	Permutation thee;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, proc)) return NULL;
	step = (step - 1) % n + 1;
	
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
	for (i = from; i <= to; i++)
	{
		long ifrom = i + step;
		if (ifrom > to) ifrom -= n;
		if (ifrom < from) ifrom += n;
		thy p[ifrom] = my p[i];
	}
	return thee;
}

int Permutation_swapOneFromRange (Permutation me, long from, long to, long pos, int forbidsame)
{
	long tmp, newpos, n;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_swapOneFromRange")) return 0;
	
	newpos = NUMrandomInteger (from, to);
	if (newpos == pos && forbidsame)
	{
		if (n == 1)
		{
			return Melder_error ("Permutation_swapOneFromRange: Impossible to satisfy \"forbid same\" constraint "
				"within the chosen range.");
		}
		while ((newpos = NUMrandomInteger (from, to)) == pos) ;
	}
	
	tmp = my p[pos]; my p[pos] = my p[newpos]; my p[newpos] = tmp;
	return 1;
}


Permutation Permutation_permuteBlocksRandomly (Permutation me, long from, long to, long blocksize,
	int permuteWithinBlocks, int noDoublets)
{
	char *proc = "Permutation_permuteBlocksRandomly";
	long iblock, first, j, nrest, n, nblocks;
	Permutation thee, pblocks = NULL;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, proc)) return NULL;

	if (blocksize == 1 || (blocksize >= n && permuteWithinBlocks))
	{
		thee = Permutation_permuteRandomly (me, from, to);
		return thee;
	}
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
	if (blocksize >= n) return thee;
	
	nblocks  = n / blocksize;
	if ((nrest = n % blocksize) != 0) return Melder_errorp ("%s: It is not possible to fit an integer number of blocks "
		"in the range.\n(The last block is only of size %d).", proc, nrest);
	
	pblocks = Permutation_create (nblocks);
	if (pblocks == NULL) goto end;
	
	if (! Permutation_permuteRandomly_inline (pblocks, 1, nblocks)) goto end;
	
	for (first = from, iblock = 1; iblock <= nblocks; iblock++, first += blocksize)
	{
		/* (n1,n2,n3,...) means: move block n1 to position 1 etc... */
		long blocktomove = Permutation_getValueAtIndex (pblocks, iblock);
		
		for (j = 1; j <= blocksize; j++)
		{
			thy p[first - 1 + j] = my p[from - 1 + (blocktomove - 1) * blocksize + j];
		}
		
		if (permuteWithinBlocks)
		{
			long last = first + blocksize - 1;
			if (! Permutation_permuteRandomly_inline (thee, first, last)) goto end;
			if (noDoublets && iblock > 0 && (thy p[first - 1] % blocksize) == (thy p[first] % blocksize))
			{
				if (! Permutation_swapOneFromRange (thee, first+1, last, first, 0)) goto end;
			}
		}
	}
end:
	forget (pblocks);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Permutation Permutation_interleave (Permutation me, long from, long to, long blocksize, long offset)
{
	char *proc = "Permutation_interleave";
	long i, n, *occupied = NULL, nblocks, nrest, posinblock;
	Permutation thee = NULL;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, proc)) return NULL;
	nblocks = n / blocksize;
	if ((nrest = n % blocksize) != 0) return Melder_errorp ("%s: There is not an integer number of blocks in the range.\n"
			"(The last block is only of size %d instead of %d).", proc, nrest, blocksize);
	if (offset >= blocksize) return Melder_errorp ("%s: Offset must be smaller than blocksize.", proc);
	
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
	
	if (nblocks == 1) return thee;
	
	occupied = NUMlvector (1, blocksize);
	if (occupied == NULL) goto end;


	posinblock = 1 - offset;
	for (i = 1; i <= n; i++)
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
end:
	NUMlvector_free (occupied, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

long Permutation_getValueAtIndex (Permutation me, long i)
{ 
    return i > 0 && i <= my numberOfElements ? my p[i] : -1;
}

long Permutation_getIndexAtValue (Permutation me, long value)
{
	long i;

	for (i = 1; i <= my numberOfElements; i++)
	{
		if (my p[i] == value) return i;
	}
	return -1;
}

Permutation Permutation_invert (Permutation me)
{
	long i;
	Permutation thee = Data_copy (me);
	if (thee == NULL) return NULL;
	for (i = 1; i <= my numberOfElements; i++)
	{
		thy p[my p[i]] = i;
	}
	return thee;
}

Permutation Permutation_reverse (Permutation me, long from, long to)
{
	long i, n;
	Permutation thee = NULL;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_reverse")) return NULL;
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
	for (i = 1; i <= n; i++)
	{
		thy p[from + i - 1] = my p[to - i + 1];
	}
	return thee;
}

/* End of Permutation.c */
