/* rng/mrg.c
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

/* This is a fifth-order multiple recursive generator. The sequence is,

   x_n = (a_1 x_{n-1} + a_5 x_{n-5}) mod m

   with a_1 = 107374182, a_2 = a_3 = a_4 = 0, a_5 = 104480 and m = 2^31-1.

   We initialize the generator with x_n = s_n MOD m for n = 1..5,
   where s_n = (69069 * s_{n-1}) mod 2^32, and s_0 = s is the
   user-supplied seed.

   NOTE: According to the paper the seeds must lie in the range [0,
   2^31 - 2] with at least one non-zero value -- our seeding procedure
   satisfies these constraints.

   We then use 6 iterations of the generator to "warm up" the internal
   state.

   With this initialization procedure the theoretical value of
   z_{10006} is 2064828650 for s = 1. The subscript 10006 means (1)
   seed the generator with s = 1, (2) do the 6 warm-up iterations
   that are part of the seeding process, (3) then do 10000 actual
   iterations.

   The period of this generator is about 2^155.

   From: P. L'Ecuyer, F. Blouin, and R. Coutre, "A search for good
   multiple recursive random number generators", ACM Transactions on
   Modeling and Computer Simulation 3, 87-98 (1993). */

static inline unsigned long int mrg_get (void *vstate);
static double mrg_get_double (void *vstate);
static void mrg_set (void *state, unsigned long int s);

static const long int m = 2147483647;
static const long int a1 = 107374182, q1 = 20, r1 = 7;
static const long int a5 = 104480, q5 = 20554, r5 = 1727;

typedef struct
  {
    long int x1, x2, x3, x4, x5;
  }
mrg_state_t;

static inline unsigned long int
mrg_get (void *vstate)
{
  mrg_state_t *state = (mrg_state_t *) vstate;

  long int p1, h1, p5, h5;

  h5 = state->x5 / q5;
  p5 = a5 * (state->x5 - h5 * q5) - h5 * r5;
  if (p5 > 0)
    p5 -= m;

  h1 = state->x1 / q1;
  p1 = a1 * (state->x1 - h1 * q1) - h1 * r1;
  if (p1 < 0)
    p1 += m;

  state->x5 = state->x4;
  state->x4 = state->x3;
  state->x3 = state->x2;
  state->x2 = state->x1;

  state->x1 = p1 + p5;

  if (state->x1 < 0)
    state->x1 += m;

  return state->x1;
}

static double
mrg_get_double (void *vstate)
{
  return mrg_get (vstate) / 2147483647.0 ;
}


static void
mrg_set (void *vstate, unsigned long int s)
{
  /* An entirely adhoc way of seeding! This does **not** come from
     L'Ecuyer et al */

  mrg_state_t *state = (mrg_state_t *) vstate;

  if (s == 0)
    s = 1;      /* default seed is 1 */

#define LCG(n) ((69069 * n) & 0xffffffffUL)
  s = LCG (s);
  state->x1 = s % m;
  s = LCG (s);
  state->x2 = s % m;
  s = LCG (s);
  state->x3 = s % m;
  s = LCG (s);
  state->x4 = s % m;
  s = LCG (s);
  state->x5 = s % m;

  /* "warm it up" with at least 5 calls to go through
     all the x values */

  mrg_get (state);
  mrg_get (state);
  mrg_get (state);
  mrg_get (state);
  mrg_get (state);
  mrg_get (state);

  return;
}

static const gsl_rng_type mrg_type =
{"mrg",                         /* name */
 2147483646,                    /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (mrg_state_t),
 &mrg_set,
 &mrg_get,
 &mrg_get_double};

const gsl_rng_type *gsl_rng_mrg = &mrg_type;
