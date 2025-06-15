/* rng/ran3.c
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

/* This is an implementation of the algorithm used in Knuths's
   subtractive generator, with the Numerical Recipe's ran3 paramters.
   It is a subtractive lagged fibonnaci generator. */

static inline unsigned long int ran3_get (void *vstate);
static double ran3_get_double (void *vstate);
static void ran3_set (void *state, unsigned long int s);

#define M_BIG 1000000000
#define M_SEED 161803398

typedef struct
  {
    unsigned int x;
    unsigned int y;
    unsigned long int buffer[56];
  }
ran3_state_t;

static inline unsigned long int
ran3_get (void *vstate)
{
  ran3_state_t *state = (ran3_state_t *) vstate;
  long int j;

  state->x++;

  if (state->x == 56)
    state->x = 1;

  state->y++;

  if (state->y == 56)
    state->y = 1;

  j = state->buffer[state->x] - state->buffer[state->y];

  if (j < 0)
    j += M_BIG;

  state->buffer[state->x] = j;

  return j;
}

static double
ran3_get_double (void *vstate)
{
  return ran3_get (vstate) / (double) M_BIG ;
}

static void
ran3_set (void *vstate, unsigned long int s)
{
  ran3_state_t *state = (ran3_state_t *) vstate;
  int i, i1;
  long int j, k;

  if (s == 0)
    s = 1;      /* default seed is 1 */

  j = (M_SEED - s) % M_BIG;

  /* the zeroth element is never used, but we initialize it for
     consistency between states */

  state->buffer[0] = 0; 

  state->buffer[55] = j;

  k = 1;
  for (i = 1; i < 55; i++)
    {
      int n = (21 * i) % 55;
      state->buffer[n] = k;
      k = j - k;
      if (k < 0)
        k += M_BIG;
      j = state->buffer[n];

    }

  for (i1 = 0; i1 < 4; i1++)
    {
      for (i = 1; i < 56; i++)
        {
          long int t = state->buffer[i] - state->buffer[1 + (i + 30) % 55];
          if (t < 0)
            t += M_BIG;
          state->buffer[i] = t;
        }
    }

  state->x = 0;
  state->y = 31;

  return;
}

static const gsl_rng_type ran3_type =
{"ran3",                        /* name */
 M_BIG,                         /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ran3_state_t),
 &ran3_set,
 &ran3_get,
 &ran3_get_double};

const gsl_rng_type *gsl_rng_ran3 = &ran3_type;
