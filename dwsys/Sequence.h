#ifndef _Sequence_h_
#define _Sequence_h_
/* Sequence.h
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
 djmw 19970507
 djmw 20020812 GPL header
*/

#define SEQUENCE_NATURALORDER 0
#define SEQUENCE_PERMUTE 1
#define SEQUENCE_RANDOMIZE 2

#ifndef _Data_h_
	#include "Data.h"
#endif

#include "Sequence_def.h"

#define Sequence_methods Data_methods    
oo_CLASS_CREATE (Sequence, Data)

int Sequence_init (Sequence me, long n, int action, int seed);
Sequence Sequence_create (long n, int action, int seed);
/*
	Create the Sequence data structure and a first sequence.
	action == SEQUENCE_NATURALORDER
		create a sequence 1..n.
	action == SEQUENCE_PERMUTE
		create a permutation of the numbers 1..n.
	action == SEQUENCE_RANDOMIZE
		create a sequence from random drawings (some numbers may occur 
		more than once).
*/

void Sequence_restart (Sequence me, long seed);
/*
	restart the random generator with seed
	seed > 0 generates a reproducible sequence
	seed == 0 generates a non-reproducible sequence
*/

void Sequence_newSequence (Sequence me);
/* Generate a new sequence */

long Sequence_ith (Sequence me, long i);
/* return my p[i] */

#endif /* _Sequence_h_ */
