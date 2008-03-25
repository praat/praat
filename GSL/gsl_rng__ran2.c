/* rng/ran2.c
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

/* This is an implementation of the algorithm used in Numerical
   Recipe's ran2 generator.  It is a L'Ecuyer combined recursive
   generator with a 32-element shuffle-box.

   As far as I can tell, in general the effects of adding a shuffle
   box cannot be proven theoretically, so the period of this generator
   is unknown. 

   The period of the underlying combined generator is O(2^60). */

static inline unsigned long int ran2_get (void *vstate);
static double ran2_get_double (void *vstate);
static void ran2_set (void *state, unsigned long int s);

static const long int m1 = 2147483563, a1 = 40014, q1 = 53668, r1 = 12211;
static const long int m2 = 2147483399, a2 = 40692, q2 = 52774, r2 = 3791;

#define N_SHUFFLE 32
#define N_DIV (1 + 2147483562/N_SHUFFLE)

typedef struct
  {
    unsigned long int x;
    unsigned long int y;
    unsigned long int n;
    unsigned long int shuffle[N_SHUFFLE];
  }
ran2_state_t;

static inline unsigned long int
ran2_get (void *vstate)
{
  ran2_state_t *state = (ran2_state_t *) vstate;

  const unsigned long int x = state->x;
  const unsigned long int y = state->y;

  long int h1 = x / q1;
  long int t1 = a1 * (x - h1 * q1) - h1 * r1;

  long int h2 = y / q2;
  long int t2 = a2 * (y - h2 * q2) - h2 * r2;

  if (t1 < 0)
    t1 += m1;

  if (t2 < 0)
    t2 += m2;

  state->x = t1;
  state->y = t2;

  {
    unsigned long int j = state->n / N_DIV;
    long int delta = state->shuffle[j] - t2;
    if (delta < 1)
      delta += m1 - 1;
    state->n = delta;
    state->shuffle[j] = t1;
  }

  return state->n;
}

static double
ran2_get_double (void *vstate)
{
  float x_max = 1 - 1.2e-7f ; /* Numerical Recipes version of 1-FLT_EPS */

  float x = ran2_get (vstate) / 2147483563.0f ;
 
  if (x > x_max) 
    return x_max ;
  
  return x ;
}

static void
ran2_set (void *vstate, unsigned long int s)
{
  ran2_state_t *state = (ran2_state_t *) vstate;
  int i;

  if (s == 0)
    s = 1;      /* default seed is 1 */

  state->y = s;

  for (i = 0; i < 8; i++)
    {
      long int h = s / q1;
      long int t = a1 * (s - h * q1) - h * r1;
      if (t < 0)
        t += m1;
      s = t;
    }

  for (i = N_SHUFFLE - 1; i >= 0; i--)
    {
      long int h = s / q1;
      long int t = a1 * (s - h * q1) - h * r1;
      if (t < 0)
        t += m1;
      s = t;
      state->shuffle[i] = s;
    }

  state->x = s;
  state->n = s;

  return;
}

static const gsl_rng_type ran2_type =
{"ran2",                        /* name */
 2147483562,                    /* RAND_MAX */
 1,                             /* RAND_MIN */
 sizeof (ran2_state_t),
 &ran2_set,
 &ran2_get,
 &ran2_get_double};

const gsl_rng_type *gsl_rng_ran2 = &ran2_type;
