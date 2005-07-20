/* Permutation.c
 *
 * Copyright (C) 2005 David Weenink
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
 djmw 20050715 info added.
*/

#include <time.h>
#include "Permutation.h"

#include "oo_DESTROY.h"
#include "Permutation_def.h"
#include "oo_COPY.h"
#include "Permutation_def.h"
#include "oo_EQUAL.h"
#include "Permutation_def.h"
#include "oo_WRITE_ASCII.h"
#include "Permutation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Permutation_def.h"
#include "oo_READ_BINARY.h"
#include "Permutation_def.h"
#include "oo_DESCRIPTION.h"
#include "Permutation_def.h"

int Permutation_validateValues (Permutation me)
{
	long i;
	Permutation thee = Data_copy (me);
	if (thee == NULL) return 0;
	
	NUMsort_l (thy n, thy p);
	for (i = 1; i <= my n; i++)
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
	Melder_info ("Number of elements: %d", my n);
}

static int readAscii (I, FILE *f)
{
	iam (Permutation);
	my n = ascgeti4 (f);
	if (my n < 1) return Melder_error ("(Permutation::readAscii:) Number of elements must be >= 1.");
	if (! (my p = NUMlvector_readAscii (1, my n, f, "p"))) return 0;
	if (! Permutation_validateValues (me)) return Melder_error
		("(Permutation::readAscii:) All values must be unique and in the [1, %d] range.", my n);
	return 1;
}

class_methods (Permutation, Data)
	class_method_local (Permutation, destroy)
	class_method_local (Permutation, copy)
	class_method_local (Permutation, equal)
	class_method_local (Permutation, writeAscii)
	class_method_local (Permutation, writeBinary)
	class_method (readAscii)
	class_method (info)
	class_method_local (Permutation, readBinary)
	class_method_local (Permutation, description)
class_methods_end


int Permutation_init (Permutation me, long n)
{
    my n = n;
    my p = NUMlvector (1, n);
    if (my p == NULL) return 0;
    Permutation_sort (me);
    return 1;
}

Permutation Permutation_create (long n)
{
    Permutation me = new (Permutation);
    if (me == NULL || ! Permutation_init (me, n)) forget (me);
    return me;
}

static int _Permutation_checkRange (Permutation me, long *from, long *to, long *n, char *proc)
{
	if ((*from < 0 || *from > my n) ||
		(*to < 0 || *to > my n)) return Melder_error ("%s: Range must be in [1, %d]", proc, my n);
	if (*from == 0) *from = 1;
	if (*to == 0) *to = my n;
	*n = *to - *from + 1;
	return 1;
}

Permutation Permutation_extractPart (Permutation me, long from, long to)
{
	long i, n;
	Permutation thee;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_extractPart")) return NULL;
	
	thee = Permutation_create (n);
	if (thee == NULL) return NULL;
	for (i = from; i <= to; i++)
	{
		thy p[i - from + 1] = my p[i];
	}
	return thee;
}

void Permutation_sort (Permutation me)
{
	long i;
	
    for (i = 1; i <= my n; i++)
    {
    	my p[i] = i;
    }
}

int Permutation_swapOnePair (Permutation me, long first, long second)
{
	long tmp;
	if (first < 0 || first > my n || second < 0 || second > my n) return Melder_error
		("Permutation_swapOnePair: Positions must be in [1,%d] range.", my n);
	tmp = my p[first]; my p[first] = my p[second]; my p[second] = tmp;
	return 1;
}

int Permutation_permuteRandomly_inline (Permutation me, long from, long to)
{
	long i, n;
	Permutation thee = NULL;
	
	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_permuteRandomly")) return 0;
	
	thee = Permutation_extractPart (me, from, to);
	if (thee == NULL) return 0;
	
	for (i = 1; i < thy n; i++)
	{
		long newpos = NUMrandomInteger (1, thy n);
		long pi = thy p[i];
		thy p[i] = thy p[newpos]; thy p[newpos] = pi;
	}
	
	for (i = from; i <= to; i++)
	{
		my p[i] = thy p[i - from + 1];
	}
	forget (thee);
	return 1;
}

Permutation Permutation_permuteRandomly (Permutation me, long from, long to)
{
    long i, n;
    Permutation thee;

	if (! _Permutation_checkRange (me, &from, &to, &n, "Permutation_permuteRandomly")) return NULL;
	
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
    if (n == 1) return thee;
	for (i = from; i < to; i++)
	{
		long newpos = NUMrandomInteger (from, to);
		long pi = thy p[i];
		thy p[i] = thy p[newpos]; thy p[newpos] = pi;
	}
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
    return i > 0 && i <= my n ? my p[i] : -1;
}

long Permutation_getIndexAtValue (Permutation me, long value)
{
	long i;

	for (i = 1; i <= my n; i++)
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
	for (i = 1; i <= my n; i++)
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
