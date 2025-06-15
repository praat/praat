/* rng/zuf.c
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

/* It is crucial that m == n-273 mod 607 at all times;
   For speed of execution, however, this is never enforced.
   Instead is is set in the initializer: note 607-273=334
   Note also that the state.u[607] is not initialized */

static inline unsigned long int zuf_get (void *vstate);
static double zuf_get_double (void *vstate);
static void zuf_set (void *state, unsigned long int s);

static const unsigned long int zuf_randmax = 16777216;  /* 2^24 */

typedef struct
  {
    int n;
    unsigned long int u[607];
  }
zuf_state_t;

/* The zufall package was implemented with float's, which is to say 24
   bits of precision.  Since I'm using long's instead, my RANDMAX
   reflects this. */

static inline unsigned long int
zuf_get (void *vstate)
{
  zuf_state_t *state = (zuf_state_t *) vstate;
  const int n = state->n;
  const int m = (n - 273 + 607) % 607;
  unsigned long int t = state->u[n] + state->u[m];

  while (t > zuf_randmax)
    t -= zuf_randmax;

  state->u[n] = t;

  if (n == 606)
    {
      state->n = 0;
    }
  else
    {
      state->n = n + 1;
    }

  return t;
}

static double
zuf_get_double (void *vstate)
{
  return zuf_get (vstate) / 16777216.0 ;
}

static void
zuf_set (void *vstate, unsigned long int s)
{
  /* A very elaborate seeding procedure is provided with the
     zufall package; this is virtually a copy of that procedure */

  /* Initialized data */

  long int kl = 9373;
  long int ij = 1802;

  /* Local variables */
  long int i, j, k, l, m;
  double x, y;
  long int ii, jj;

  zuf_state_t *state = (zuf_state_t *) vstate;

  state->n = 0;

/*  generates initial seed buffer by linear congruential */
/*  method. Taken from Marsaglia, FSU report FSU-SCRI-87-50 */
/*  variable seed should be 0 < seed <31328 */

  if (s == 0)
    s = 1802;   /* default seed is 1802 */

  ij = s;

  i = ij / 177 % 177 + 2;
  j = ij % 177 + 2;
  k = kl / 169 % 178 + 1;
  l = kl % 169;
  for (ii = 0; ii < 607; ++ii)
    {
      x = 0.0;
      y = 0.5;
      /* 24 bits?? */
      for (jj = 1; jj <= 24; ++jj)
        {
          m = i * j % 179 * k % 179;
          i = j;
          j = k;
          k = m;
          l = (l * 53 + 1) % 169;
          if (l * m % 64 >= 32)
            {
              x += y;
            }
          y *= 0.5;
        }
      state->u[ii] = (unsigned long int) (x * zuf_randmax);
    }
}

static const gsl_rng_type zuf_type =
{"zuf",                         /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (zuf_state_t),
 &zuf_set,
 &zuf_get,
 &zuf_get_double};

const gsl_rng_type *gsl_rng_zuf = &zuf_type;
