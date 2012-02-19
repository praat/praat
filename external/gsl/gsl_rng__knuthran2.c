/* rng/knuthran2.c
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
 * This implementation  copyright (C) 2001 Carlo Perassi
 * and (C) 2003 Heiko Bauke.
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

#include "gsl_rng__schrage.c"

#define AA1      271828183UL
#define AA2     1833324378UL    /* = -314159269 mod (2 ^ 31 -1) */
#define MM      0x7fffffffUL    /* 2 ^ 31 - 1 */
#define CEIL_SQRT_MM 46341UL    /* sqrt(2 ^ 31 - 1) */

static inline unsigned long int ran_get (void *vstate);
static double ran_get_double (void *vstate);
static void ran_set (void *state, unsigned long int s);

typedef struct
{
  unsigned long int x0;
  unsigned long int x1;
}
ran_state_t;

static inline unsigned long int
ran_get (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

  const unsigned long int xtmp = state->x1;
  state->x1 = schrage_mult (AA1, state->x1, MM, CEIL_SQRT_MM)
    + schrage_mult (AA2, state->x0, MM, CEIL_SQRT_MM);

  if (state->x1 >= MM)
    state->x1 -= MM;

  state->x0 = xtmp;

  return state->x1;
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

  if ((s % MM) == 0)
    s = 1;                      /* default seed is 1 */

  state->x0 = s % MM;
  state->x1 = s % MM;

  return;
}

static const gsl_rng_type ran_type = {
  "knuthran2",                  /* name */
  MM - 1L,                      /* RAND_MAX */
  0,                            /* RAND_MIN */
  sizeof (ran_state_t),
  &ran_set,
  &ran_get,
  &ran_get_double
};

const gsl_rng_type *gsl_rng_knuthran2 = &ran_type;
