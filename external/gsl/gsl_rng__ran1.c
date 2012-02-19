/* rng/ran1.c
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
   Recipe's ran1 generator.  It is MINSTD with a 32-element
   shuffle-box. */

static inline unsigned long int ran1_get (void *vstate);
static double ran1_get_double (void *vstate);
static void ran1_set (void *state, unsigned long int s);

static const long int m = 2147483647, a = 16807, q = 127773, r = 2836;

#define N_SHUFFLE 32
#define N_DIV (1 + 2147483646/N_SHUFFLE)

typedef struct
  {
    unsigned long int x;
    unsigned long int n;
    unsigned long int shuffle[N_SHUFFLE];
  }
ran1_state_t;

static inline unsigned long int
ran1_get (void *vstate)
{
  ran1_state_t *state = (ran1_state_t *) vstate;

  const unsigned long int x = state->x;

  const long int h = x / q;
  const long int t = a * (x - h * q) - h * r;

  if (t < 0)
    {
      state->x = t + m;
    }
  else
    {
      state->x = t;
    }

  {
    unsigned long int j = state->n / N_DIV;
    state->n = state->shuffle[j];
    state->shuffle[j] = state->x;
  }

  return state->n;
}

static double
ran1_get_double (void *vstate)
{
  float x_max = 1 - 1.2e-7f ; /* Numerical Recipes version of 1-FLT_EPS */

  float x = ran1_get (vstate) / 2147483647.0f ;
 
  if (x > x_max) 
    return x_max ;
  
  return x ;
}


static void
ran1_set (void *vstate, unsigned long int s)
{
  ran1_state_t *state = (ran1_state_t *) vstate;
  int i;

  if (s == 0)
    s = 1;      /* default seed is 1 */

  for (i = 0; i < 8; i++)
    {
      long int h = s / q;
      long int t = a * (s - h * q) - h * r;
      if (t < 0)
        t += m;
      s = t;
    }

  for (i = N_SHUFFLE - 1; i >= 0; i--)
    {
      long int h = s / q;
      long int t = a * (s - h * q) - h * r;
      if (t < 0)
        t += m;
      s = t;
      state->shuffle[i] = s;
    }

  state->x = s;
  state->n = s;

  return;
}

static const gsl_rng_type ran1_type =
{"ran1",                        /* name */
 2147483646,                    /* RAND_MAX */
 1,                             /* RAND_MIN */
 sizeof (ran1_state_t),
 &ran1_set,
 &ran1_get,
 &ran1_get_double};

const gsl_rng_type *gsl_rng_ran1 = &ran1_type;
