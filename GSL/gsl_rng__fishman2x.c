/* rng/fishman2x.c
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
 * It is called "Fishman - L'Ecuyer"
 *
 * This implementation copyright (C) 2001 Carlo Perassi
 * and (C) 2003 Heiko Bauke.
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

/* Fishman */
#define AAA_F 48271UL
#define MMM_F 0x7fffffffUL      /* 2 ^ 31 - 1 */
#define QQQ_F 44488UL
#define RRR_F 3399UL

/* L'Ecuyer */
#define AAA_L 40692UL
#define MMM_L 0x7fffff07UL      /* 2 ^ 31 - 249 */
#define QQQ_L 52774UL
#define RRR_L 3791UL

static inline unsigned long int ran_get (void *vstate);
static double ran_get_double (void *vstate);
static void ran_set (void *state, unsigned long int s);

typedef struct
{
  unsigned long int x;
  unsigned long int y;
  unsigned long int z;
}
ran_state_t;

static inline unsigned long int
ran_get (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

  long int y, r;

  r = RRR_F * (state->x / QQQ_F);
  y = AAA_F * (state->x % QQQ_F) - r;
  if (y < 0)
    y += MMM_F;
  state->x = y;

  r = RRR_L * (state->y / QQQ_L);
  y = AAA_L * (state->y % QQQ_L) - r;
  if (y < 0)
    y += MMM_L;
  state->y = y;

  state->z = (state->x > state->y) ? (state->x - state->y) :
    MMM_F + state->x - state->y;

  return state->z;
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

  if ((s % MMM_F) == 0 || (s % MMM_L) == 0)
    s = 1;                      /* default seed is 1 */

  state->x = s % MMM_F;
  state->y = s % MMM_L;
  state->z = (state->x > state->y) ? (state->x - state->y) :
    MMM_F + state->x - state->y;

  return;
}

static const gsl_rng_type ran_type = {
  "fishman2x",                  /* name */
  MMM_F - 1,                    /* RAND_MAX */
  0,                            /* RAND_MIN */
  sizeof (ran_state_t),
  &ran_set,
  &ran_get,
  &ran_get_double
};

const gsl_rng_type *gsl_rng_fishman2x = &ran_type;
