/* Sequence.c
 *
 * Copyright (C) 1994-2002 David Weenink
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
 djmw 20020812 GPL header
*/

#include <time.h>
#include "Sequence.h"

#include "oo_DESTROY.h"
#include "Sequence_def.h"
#include "oo_COPY.h"
#include "Sequence_def.h"
#include "oo_EQUAL.h"
#include "Sequence_def.h"
#include "oo_WRITE_ASCII.h"
#include "Sequence_def.h"
#include "oo_WRITE_BINARY.h"
#include "Sequence_def.h"
#include "oo_READ_ASCII.h"
#include "Sequence_def.h"
#include "oo_READ_BINARY.h"
#include "Sequence_def.h"
#include "oo_DESCRIPTION.h"
#include "Sequence_def.h"

class_methods (Sequence, Data)
	class_method_local (Sequence, destroy)
	class_method_local (Sequence, copy)
	class_method_local (Sequence, equal)
	class_method_local (Sequence, writeAscii)
	class_method_local (Sequence, writeBinary)
	class_method_local (Sequence, readAscii)
	class_method_local (Sequence, readBinary)
	class_method_local (Sequence, description)
class_methods_end

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 2.3e-16
#define RNMX (1.0-EPS)

static double NUMran1 (Sequence me)
{
    long j, k; double ans;

    /* start here when not initializing */
    k = my seed / IQ;
    /* seed = (IA*seed) % IM without overflows by Schrage's method */
    my seed = IA * (my seed - k * IQ) - IR * k;
    if ( my seed < 0) my seed += IM;
    j = 1 + my iy / NDIV;
    my iy = my tab[j];
    my tab[j] = my seed;
    return ((ans = AM * my iy) > RNMX) ? RNMX : ans;
}

int Sequence_init (Sequence me, long n, int action, int seed)
{
    my n = n; my ntab = NTAB; my iy = 0; my tab = NULL;
    if (((my p = NUMlvector (1, n)) == NULL) ||
    	((my tab = NUMlvector (1, my ntab)) == NULL)) return 0;
    my action = action;
    my seed = seed;
    Sequence_restart (me, seed);
    return 1;
}

Sequence Sequence_create (long n, int action, int seed)
{
    Sequence me = new (Sequence);
    if ( ! me || ! Sequence_init (me, n, action, seed)) forget (me);
    return me;
}

void Sequence_restart (Sequence me, long seed)
{
	long j, k;
    if (seed <= 0) seed = time (NULL);
	for (j = my ntab + 8; j > 0; j--)
	{	/* initialize shuffle table of random generator */
		k = seed / IQ;
		seed = IA * (seed - k * IQ) - IR * k;
		if (seed < 0) seed += IM;
		if (j <= my ntab) my tab[j] = seed;
	}
	my iy = my tab[1];
	my seed = seed;
	(void) Sequence_newSequence (me);
}

void Sequence_newSequence (Sequence me)
{
    long i;
    if (my action == SEQUENCE_RANDOMIZE)
    {
    	for (i=1; i <= my n; i++) my p[i] = 1 + my n * NUMran1 (me);
    	return;
    }
    for (i=1; i <= my n; i++) my p[i] = i;
    if (my action == SEQUENCE_PERMUTE)
    {
		for (i=1; i < my n; i++)
		{	/* offset in range [0, my n - i] */
			long tmp, offset = (my n - i + 1) * NUMran1 (me);
			tmp = my p[i]; my p[i] = my p[i+offset]; my p[i+offset] = tmp;
		}
    }
}

long Sequence_ith (Sequence me, long i)
{
    Melder_assert (i > 0 && i <= my n); 
    return my p[i];
}

#undef NTAB
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NDIV
#undef EPS
#undef RNMX

/* End of Sequence.c */
