/* rng/transputer.c
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

/* This is the INMOS Transputer Development System generator. The sequence is,

   x_{n+1} = (a x_n) mod m

   with a = 1664525 and m = 2^32. The seed specifies the initial
   value, x_1.

   The theoretical value of x_{10001} is 1244127297.

   The period of this generator is 2^30. */

static inline unsigned long int transputer_get (void *vstate);
static double transputer_get_double (void *vstate);
static void transputer_set (void *state, unsigned long int s);

typedef struct
  {
    unsigned long int x;
  }
transputer_state_t;

static unsigned long int
transputer_get (void *vstate)
{
  transputer_state_t *state = (transputer_state_t *) vstate;

  state->x = (1664525 * state->x) & 0xffffffffUL;

  return state->x;
}

static double
transputer_get_double (void *vstate)
{
  return transputer_get (vstate) / 4294967296.0 ;
}

static void
transputer_set (void *vstate, unsigned long int s)
{
  transputer_state_t *state = (transputer_state_t *) vstate;

  if (s == 0)
    s = 1 ;   /* default seed is 1. */

  state->x = s;

  return;
}

static const gsl_rng_type transputer_type =
{"transputer",                          /* name */
 0xffffffffUL,                  /* RAND_MAX */
 1,                             /* RAND_MIN */
 sizeof (transputer_state_t),
 &transputer_set,
 &transputer_get,
 &transputer_get_double};

const gsl_rng_type *gsl_rng_transputer = &transputer_type;
