/* rng/rand48.c
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
#include <math.h>
#include <stdlib.h>
#include "gsl_sys.h"
#include "gsl_rng.h"

/* This is the Unix rand48() generator. The generator returns the
   upper 32 bits from each term of the sequence,

   x_{n+1} = (a x_n + c) mod m 

   using 48-bit unsigned arithmetic, with a = 0x5DEECE66D , c = 0xB
   and m = 2^48. The seed specifies the upper 32 bits of the initial
   value, x_1, with the lower 16 bits set to 0x330E.

   The theoretical value of x_{10001} is 244131582646046.

   The period of this generator is ? FIXME (probably around 2^48). */

static inline void rand48_advance (void *vstate);
static unsigned long int rand48_get (void *vstate);
static double rand48_get_double (void *vstate);
static void rand48_set (void *state, unsigned long int s);

static const unsigned short int a0 = 0xE66D ;
static const unsigned short int a1 = 0xDEEC ;
static const unsigned short int a2 = 0x0005 ;

static const unsigned short int c0 = 0x000B ;

typedef struct
  {
    unsigned short int x0, x1, x2;
  }
rand48_state_t;

static inline void
rand48_advance (void *vstate)
{
  rand48_state_t *state = (rand48_state_t *) vstate;

  /* work with unsigned long ints throughout to get correct integer
     promotions of any unsigned short ints */

  const unsigned long int x0 = (unsigned long int) state->x0 ;
  const unsigned long int x1 = (unsigned long int) state->x1 ;
  const unsigned long int x2 = (unsigned long int) state->x2 ;

  unsigned long int a ;
  
  a = a0 * x0 + c0 ;
  state->x0 = (a & 0xFFFF) ;
 
  a >>= 16 ;

  /* although the next line may overflow we only need the top 16 bits
     in the following stage, so it does not matter */

  a += a0 * x1 + a1 * x0 ; 
  state->x1 = (a & 0xFFFF) ;

  a >>= 16 ;
  a += a0 * x2 + a1 * x1 + a2 * x0 ;
  state->x2 = (a & 0xFFFF) ;
}

static unsigned long int 
rand48_get (void *vstate)
{
  unsigned long int x1, x2;

  rand48_state_t *state = (rand48_state_t *) vstate;
  rand48_advance (state) ;

  x2 = (unsigned long int) state->x2;
  x1 = (unsigned long int) state->x1;

  return (x2 << 16) + x1;
}

static double
rand48_get_double (void * vstate)
{
  rand48_state_t *state = (rand48_state_t *) vstate;

  rand48_advance (state) ;  

  return (ldexp((double) state->x2, -16)
          + ldexp((double) state->x1, -32) 
          + ldexp((double) state->x0, -48)) ;
}

static void
rand48_set (void *vstate, unsigned long int s)
{
  rand48_state_t *state = (rand48_state_t *) vstate;

  if (s == 0)  /* default seed */
    {
      state->x0 = 0x330E ;
      state->x1 = 0xABCD ;
      state->x2 = 0x1234 ;
    }
  else 
    {
      state->x0 = 0x330E ;
      state->x1 = s & 0xFFFF ;
      state->x2 = (s >> 16) & 0xFFFF ;
    }

  return;
}

static const gsl_rng_type rand48_type =
{"rand48",                      /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (rand48_state_t),
 &rand48_set,
 &rand48_get,
 &rand48_get_double
};

const gsl_rng_type *gsl_rng_rand48 = &rand48_type;
