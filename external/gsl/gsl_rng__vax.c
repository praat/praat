/* rng/vax.c
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

/* This is the old vax generator MTH$RANDOM. The sequence is,

   x_{n+1} = (a x_n + c) mod m

   with a = 69069, c = 1 and m = 2^32. The seed specifies the initial
   value, x_1.

   The theoretical value of x_{10001} is 3051034865.

   The period of this generator is 2^32. */

static inline unsigned long int vax_get (void *vstate);
static double vax_get_double (void *vstate);
static void vax_set (void *state, unsigned long int s);

typedef struct
  {
    unsigned long int x;
  }
vax_state_t;

static inline unsigned long int
vax_get (void *vstate)
{
  vax_state_t *state = (vax_state_t *) vstate;

  state->x = (69069 * state->x + 1) & 0xffffffffUL;

  return state->x;
}

static double
vax_get_double (void *vstate)
{
  return vax_get (vstate) / 4294967296.0 ;
}

static void
vax_set (void *vstate, unsigned long int s)
{
  vax_state_t *state = (vax_state_t *) vstate;

  /* default seed is 0. The constant term c stops the series from
     collapsing to 0,0,0,0,0,... */

  state->x = s;

  return;
}

static const gsl_rng_type vax_type =
{"vax",                         /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (vax_state_t),
 &vax_set,
 &vax_get,
 &vax_get_double};

const gsl_rng_type *gsl_rng_vax = &vax_type;
