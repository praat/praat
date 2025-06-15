/* rng/r250.c
 * 
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

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

/* This is a shift-register random number generator. The sequence is

   x_n = x_{n-103} ^ x_{n-250}        ("^" means XOR)

   defined on 32-bit words.

   BJG: Note that this implementation actually uses the sequence, x_n
   = x_{n-147} ^ x_{n-250} which generates the outputs in
   time-reversed order but is otherwise completely equivalent.

   The first 250 elements x_1 .. x_250 are first initialized as x_n =
   s_n, where s_n = (69069*s_{n-1}) mod 2^32 and s_0=s is the
   user-supplied seed. To ensure that the sequence does not lie on a
   subspace we force 32 of the entries to be linearly independent.  We
   take the 32 elements x[3], x[10], x[17], x[24], ..., 213 and apply
   the following operations,

   x[3]   &= 11111111111111111111111111111111
   x[3]   |= 10000000000000000000000000000000 
   x[10]  &= 01111111111111111111111111111111
   x[10]  |= 01000000000000000000000000000000 
   x[17]  &= 00111111111111111111111111111111
   x[17]  |= 00100000000000000000000000000000 
   ....      ...
   x[206] &= 00000000000000000000000000000111
   x[206] |= 00000000000000000000000000000100 
   x[213] &= 00000000000000000000000000000011
   x[213] |= 00000000000000000000000000000010 
   x[220] &= 00000000000000000000000000000001
   x[220] |= 00000000000000000000000000000001 

   i.e. if we consider the bits of the 32 elements as forming a 32x32
   array then we are setting the diagonal bits of the array to one and
   masking the lower triangle below the diagonal to zero.

   With this initialization procedure the theoretical value of
   x_{10001} is 1100653588 for s = 1 (Actually I got this by running
   the original code). The subscript 10001 means (1) seed the
   generator with s = 1 and then do 10000 actual iterations.

   The period of this generator is about 2^250.

   The algorithm works for any number of bits. It is implemented here
   for 32 bits.

   From: S. Kirkpatrick and E. Stoll, "A very fast shift-register
   sequence random number generator", Journal of Computational Physics,
   40, 517-526 (1981). */

static inline unsigned long int r250_get (void *vstate);
static double r250_get_double (void *vstate);
static void r250_set (void *state, unsigned long int s);

typedef struct
  {
    int i;
    unsigned long x[250];
  }
r250_state_t;

static inline unsigned long int
r250_get (void *vstate)
{
  r250_state_t *state = (r250_state_t *) vstate;
  unsigned long int k;
  int j;

  int i = state->i;

  if (i >= 147)
    {
      j = i - 147;
    }
  else
    {
      j = i + 103;
    }

  k = state->x[i] ^ state->x[j];
  state->x[i] = k;

  if (i >= 249)
    {
      state->i = 0;
    }
  else
    {
      state->i = i + 1;
    }

  return k;
}

static double 
r250_get_double (void *vstate)
{
  return r250_get (vstate) /  4294967296.0 ;
}

static void
r250_set (void *vstate, unsigned long int s)
{
  r250_state_t *state = (r250_state_t *) vstate;

  int i;

  if (s == 0)
    s = 1;      /* default seed is 1 */

  state->i = 0;

#define LCG(n) ((69069 * n) & 0xffffffffUL)

  for (i = 0; i < 250; i++)     /* Fill the buffer  */
    {
      s = LCG (s);
      state->x[i] = s;
    }

  {
    /* Masks for turning on the diagonal bit and turning off the
       leftmost bits */

    unsigned long int msb = 0x80000000UL;
    unsigned long int mask = 0xffffffffUL;

    for (i = 0; i < 32; i++)
      {
        int k = 7 * i + 3;      /* Select a word to operate on        */
        state->x[k] &= mask;    /* Turn off bits left of the diagonal */
        state->x[k] |= msb;     /* Turn on the diagonal bit           */
        mask >>= 1;
        msb >>= 1;
      }
  }

  return;
}

static const gsl_rng_type r250_type =
{"r250",                        /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (r250_state_t),
 &r250_set,
 &r250_get,
 &r250_get_double};

const gsl_rng_type *gsl_rng_r250 = &r250_type;
