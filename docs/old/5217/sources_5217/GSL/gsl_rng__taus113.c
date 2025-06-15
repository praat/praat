/* rng/taus113.c
 * Copyright (C) 2002 Atakan Gurkan
 * Based on the file taus.c which has the notice
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* This is a maximally equidistributed combined, collision free 
   Tausworthe generator, with a period ~2^{113}. The sequence is,

   x_n = (z1_n ^ z2_n ^ z3_n ^ z4_n)  

   b = (((z1_n <<  6) ^ z1_n) >> 13)
   z1_{n+1} = (((z1_n & 4294967294) << 18) ^ b)
   b = (((z2_n <<  2) ^ z2_n) >> 27)
   z2_{n+1} = (((z2_n & 4294967288) <<  2) ^ b)
   b = (((z3_n << 13) ^ z3_n) >> 21)
   z3_{n+1} = (((z3_n & 4294967280) <<  7) ^ b)
   b = (((z4_n <<  3)  ^ z4_n) >> 12)
   z4_{n+1} = (((z4_n & 4294967168) << 13) ^ b)

   computed modulo 2^32. In the formulas above '^' means exclusive-or 
   (C-notation), not exponentiation. 
   The algorithm is for 32-bit integers, hence a bitmask is used to clear 
   all but least significant 32 bits, after left shifts, to make the code 
   work on architectures where integers are 64-bit.

   The generator is initialized with 
   zi = (69069 * z{i+1}) MOD 2^32 where z0 is the seed provided
   During initialization a check is done to make sure that the initial seeds 
   have a required number of their most significant bits set.
   After this, the state is passed through the RNG 10 times to ensure the
   state satisfies a recurrence relation.

   References:
   P. L'Ecuyer, "Tables of Maximally-Equidistributed Combined LFSR Generators",
   Mathematics of Computation, 68, 225 (1999), 261--269.
     http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps
   P. L'Ecuyer, "Maximally Equidistributed Combined Tausworthe Generators", 
   Mathematics of Computation, 65, 213 (1996), 203--213.
     http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme.ps
   the online version of the latter contains corrections to the print version.
*/

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

#define LCG(n) ((69069UL * n) & 0xffffffffUL)
#define MASK 0xffffffffUL

static inline unsigned long int taus113_get (void *vstate);
static double taus113_get_double (void *vstate);
static void taus113_set (void *state, unsigned long int s);

typedef struct
{
  unsigned long int z1, z2, z3, z4;
}
taus113_state_t;

static inline unsigned long
taus113_get (void *vstate)
{
  taus113_state_t *state = (taus113_state_t *) vstate;
  unsigned long b1, b2, b3, b4;

  b1 = ((((state->z1 << 6UL) & MASK) ^ state->z1) >> 13UL);
  state->z1 = ((((state->z1 & 4294967294UL) << 18UL) & MASK) ^ b1);

  b2 = ((((state->z2 << 2UL) & MASK) ^ state->z2) >> 27UL);
  state->z2 = ((((state->z2 & 4294967288UL) << 2UL) & MASK) ^ b2);

  b3 = ((((state->z3 << 13UL) & MASK) ^ state->z3) >> 21UL);
  state->z3 = ((((state->z3 & 4294967280UL) << 7UL) & MASK) ^ b3);

  b4 = ((((state->z4 << 3UL) & MASK) ^ state->z4) >> 12UL);
  state->z4 = ((((state->z4 & 4294967168UL) << 13UL) & MASK) ^ b4);

  return (state->z1 ^ state->z2 ^ state->z3 ^ state->z4);

}

static double
taus113_get_double (void *vstate)
{
  return taus113_get (vstate) / 4294967296.0;
}

static void
taus113_set (void *vstate, unsigned long int s)
{
  taus113_state_t *state = (taus113_state_t *) vstate;

  if (!s)
    s = 1UL;                    /* default seed is 1 */

  state->z1 = LCG (s);
  if (state->z1 < 2UL)
    state->z1 += 2UL;
  state->z2 = LCG (state->z1);
  if (state->z2 < 8UL)
    state->z2 += 8UL;
  state->z3 = LCG (state->z2);
  if (state->z3 < 16UL)
    state->z3 += 16UL;
  state->z4 = LCG (state->z3);
  if (state->z4 < 128UL)
    state->z4 += 128UL;

  /* Calling RNG ten times to satify recurrence condition */
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);
  taus113_get (state);

  return;
}

static const gsl_rng_type taus113_type = {
  "taus113",                    /* name */
  0xffffffffUL,                 /* RAND_MAX */
  0,                            /* RAND_MIN */
  sizeof (taus113_state_t),
  &taus113_set,
  &taus113_get,
  &taus113_get_double
};

const gsl_rng_type *gsl_rng_taus113 = &taus113_type;


/*  Rules for analytic calculations using GNU Emacs Calc:
    (used to find the values for the test program)

  [ LCG(n) := n * 69069 mod (2^32) ]
  
  [ b1(x) := rsh(xor(lsh(x, 6), x), 13),
  q1(x) := xor(lsh(and(x, 4294967294), 18), b1(x)),
  b2(x) := rsh(xor(lsh(x, 2), x), 27),
  q2(x) := xor(lsh(and(x, 4294967288), 2), b2(x)),
  b3(x) := rsh(xor(lsh(x, 13), x), 21),
  q3(x) := xor(lsh(and(x, 4294967280), 7), b3(x)),
  b4(x) := rsh(xor(lsh(x, 3), x), 12),
  q4(x) := xor(lsh(and(x, 4294967168), 13), b4(x))
  ]
  
  [ S([z1,z2,z3,z4]) := [q1(z1), q2(z2), q3(z3), q4(z4)] ]
*/
