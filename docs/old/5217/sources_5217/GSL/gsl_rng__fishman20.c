/* rng/fishman20.c
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

/*
 * This generator is taken from
 *
 * Donald E. Knuth
 * The Art of Computer Programming
 * Volume 2
 * Third Edition
 * Addison-Wesley
 * Page 108
 *
 * It is called "Fishman"
 *
 * This implementation copyright (C) 2001 Carlo Perassi
 * and (C) 2003 Heiko Bauke.
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

static inline unsigned long int ran_get (void *vstate);
static double ran_get_double (void *vstate);
static void ran_set (void *state, unsigned long int s);

static const long int m = 2147483647, a = 48271, q = 44488, r = 3399;

typedef struct
{
  unsigned long int x;
}
ran_state_t;

static inline unsigned long int
ran_get (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

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
ran_get_double (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

  return ran_get (state) / 2147483647.0;
}

static void
ran_set (void *vstate, unsigned long int s)
{
  ran_state_t *state = (ran_state_t *) vstate;

  if ((s%m) == 0)
    s = 1;                      /* default seed is 1 */

  state->x = s & m;

  return;
}

static const gsl_rng_type ran_type = {
  "fishman20",                  /* name */
  2147483646,                   /* RAND_MAX */
  1,                            /* RAND_MIN */
  sizeof (ran_state_t),
  &ran_set,
  &ran_get,
  &ran_get_double
};

const gsl_rng_type *gsl_rng_fishman20 = &ran_type;
