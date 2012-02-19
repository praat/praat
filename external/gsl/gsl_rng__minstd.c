/* rng/minstd.c
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

/* MINSTD is Park and Miller's minimal standard generator (i.e. it's
   not particularly good).

   The sequence is

   x_{n+1} = (a x_n) mod m

   with a = 16807 and m = 2^31 - 1 = 2147483647. The seed specifies
   the initial value, x_1.  

   The theoretical value of x_{10001} is 1043618065, starting with a
   seed of x_1 = 1.

   The period of this generator is 2^31.

   It is used as the RNUN subroutine in the IMSL Library and the RAND
   function in MATLAB. The generator is sometimes known by the acronym
   "GGL" (I'm not sure what that stands for).

   From: Park and Miller, "Random Number Generators: Good ones are
   hard to find" Communications of the ACM, October 1988, Volume 31,
   No 10, pages 1192-1201. */

static inline unsigned long int minstd_get (void *vstate);
static double minstd_get_double (void *vstate);
static void minstd_set (void *state, unsigned long int s);

static const long int m = 2147483647, a = 16807, q = 127773, r = 2836;

typedef struct
  {
    unsigned long int x;
  }
minstd_state_t;

static inline unsigned long int
minstd_get (void *vstate)
{
  minstd_state_t *state = (minstd_state_t *) vstate;

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

  return state->x;
}

static double
minstd_get_double (void *vstate)
{
  return minstd_get (vstate) / 2147483647.0;
}

static void
minstd_set (void *vstate, unsigned long int s)
{
  minstd_state_t *state = (minstd_state_t *) vstate;

  if (s == 0)
    s = 1;      /* default seed is 1 */

  state->x = s;

  return;
}

static const gsl_rng_type minstd_type =
{"minstd",                      /* name */
 2147483646,                    /* RAND_MAX */
 1,                             /* RAND_MIN */
 sizeof (minstd_state_t),
 &minstd_set,
 &minstd_get,
 &minstd_get_double};

const gsl_rng_type *gsl_rng_minstd = &minstd_type;
