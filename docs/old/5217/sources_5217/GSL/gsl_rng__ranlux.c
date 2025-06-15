/* rng/ranlux.c
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

/* This is a lagged fibonacci generator with skipping developed by Luescher.
   The sequence is a series of 24-bit integers, x_n, 

   x_n = d_n + b_n

   where d_n = x_{n-10} - x_{n-24} - c_{n-1}, b_n = 0 if d_n >= 0 and
   b_n = 2^24 if d_n < 0, c_n = 0 if d_n >= 0 and c_n = 1 if d_n < 0,
   where after 24 samples a group of p integers are "skipped", to
   reduce correlations. By default p = 199, but can be increased to
   365.

   The period of the generator is around 10^171. 

   From: M. Luescher, "A portable high-quality random number generator
   for lattice field theory calculations", Computer Physics
   Communications, 79 (1994) 100-110.

   Available on the net as hep-lat/9309020 at http://xxx.lanl.gov/

   See also,

   F. James, "RANLUX: A Fortran implementation of the high-quality
   pseudo-random number generator of Luscher", Computer Physics
   Communications, 79 (1994) 111-114

   Kenneth G. Hamilton, F. James, "Acceleration of RANLUX", Computer
   Physics Communications, 101 (1997) 241-248

   Kenneth G. Hamilton, "Assembler RANLUX for PCs", Computer Physics
   Communications, 101 (1997) 249-253  */

static inline unsigned long int ranlux_get (void *vstate);
static double ranlux_get_double (void *vstate);
static void ranlux_set_lux (void *state, unsigned long int s, unsigned int luxury);
static void ranlux_set (void *state, unsigned long int s);
static void ranlux389_set (void *state, unsigned long int s);

static const unsigned long int mask_lo = 0x00ffffffUL;  /* 2^24 - 1 */
static const unsigned long int mask_hi = ~0x00ffffffUL;
static const unsigned long int two24 = 16777216;        /* 2^24 */

typedef struct
  {
    unsigned int i;
    unsigned int j;
    unsigned int n;
    unsigned int skip;
    unsigned int carry;
    unsigned long int u[24];
  }
ranlux_state_t;

static inline unsigned long int increment_state (ranlux_state_t * state);

static inline unsigned long int
increment_state (ranlux_state_t * state)
{
  unsigned int i = state->i;
  unsigned int j = state->j;
  long int delta = state->u[j] - state->u[i] - state->carry;

  if (delta & mask_hi)
    {
      state->carry = 1;
      delta &= mask_lo;
    }
  else
    {
      state->carry = 0;
    }

  state->u[i] = delta;

  if (i == 0)
    {
      i = 23;
    }
  else
    {
      i--;
    }

  state->i = i;

  if (j == 0)
    {
      j = 23;
    }
  else
    {
      j--;
    }

  state->j = j;

  return delta;
}

static inline unsigned long int
ranlux_get (void *vstate)
{
  ranlux_state_t *state = (ranlux_state_t *) vstate;
  const unsigned int skip = state->skip;
  unsigned long int r = increment_state (state);

  state->n++;

  if (state->n == 24)
    {
      unsigned int i;
      state->n = 0;
      for (i = 0; i < skip; i++)
        increment_state (state);
    }

  return r;
}

static double
ranlux_get_double (void *vstate)
{
  return ranlux_get (vstate) / 16777216.0;
}

static void
ranlux_set_lux (void *vstate, unsigned long int s, unsigned int luxury)
{
  ranlux_state_t *state = (ranlux_state_t *) vstate;
  int i;

  long int seed;

  if (s == 0)
    s = 314159265;      /* default seed is 314159265 */

  seed = s;

  /* This is the initialization algorithm of F. James, widely in use
     for RANLUX. */

  for (i = 0; i < 24; i++)
    {
      unsigned long int k = seed / 53668;
      seed = 40014 * (seed - k * 53668) - k * 12211;
      if (seed < 0)
        {
          seed += 2147483563;
        }
      state->u[i] = seed % two24;
    }

  state->i = 23;
  state->j = 9;
  state->n = 0;
  state->skip = luxury - 24;

  if (state->u[23] & mask_hi)
    {
      state->carry = 1;
    }
  else
    {
      state->carry = 0;
    }
}

static void
ranlux_set (void *vstate, unsigned long int s)
{
  ranlux_set_lux (vstate, s, 223);
}

static void
ranlux389_set (void *vstate, unsigned long int s)
{
  ranlux_set_lux (vstate, s, 389);
}


static const gsl_rng_type ranlux_type =
{"ranlux",                      /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranlux_state_t),
 &ranlux_set,
 &ranlux_get,
 &ranlux_get_double};

static const gsl_rng_type ranlux389_type =
{"ranlux389",                   /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranlux_state_t),
 &ranlux389_set,
 &ranlux_get,
 &ranlux_get_double};

const gsl_rng_type *gsl_rng_ranlux = &ranlux_type;
const gsl_rng_type *gsl_rng_ranlux389 = &ranlux389_type;
