/* rng/ranf.c
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
#include "gsl_rng.h"

/* This is the CRAY RANF generator. The generator returns the
   upper 32 bits from each term of the sequence,

   x_{n+1} = (a x_n) mod m 

   using 48-bit unsigned arithmetic, with a = 0x2875A2E7B175 and m =
   2^48. The seed specifies the lower 32 bits of the initial value,
   x_1, with the lowest bit set (to prevent the seed taking an even
   value), and the upper 16 bits set to 0.

   There is a subtlety in the implementation of the seed. The initial
   state is put one step back by multiplying by the modular inverse of
   a mod m. This is done for compatibility with the original CRAY
   implementation.

   Note, you can only seed the generator with integers up to 2^32,
   while the CRAY uses wide integers which can cover all 2^48 states
   of the generator.

   The theoretical value of x_{10001} is 141091827447341.

   The period of this generator is 2^{46}. */

static inline void ranf_advance (void *vstate);
static unsigned long int ranf_get (void *vstate);
static double ranf_get_double (void *vstate);
static void ranf_set (void *state, unsigned long int s);

static const unsigned short int a0 = 0xB175 ;
static const unsigned short int a1 = 0xA2E7 ;
static const unsigned short int a2 = 0x2875 ;

typedef struct
  {
    unsigned short int x0, x1, x2;
  }
ranf_state_t;

static inline void
ranf_advance (void *vstate)
{
  ranf_state_t *state = (ranf_state_t *) vstate;

  const unsigned long int x0 = (unsigned long int) state->x0 ;
  const unsigned long int x1 = (unsigned long int) state->x1 ;
  const unsigned long int x2 = (unsigned long int) state->x2 ;

  unsigned long int r ;
  
  r = a0 * x0 ;
  state->x0 = (r & 0xFFFF) ;
 
  r >>= 16 ;
  r += a0 * x1 + a1 * x0 ;
  state->x1 = (r & 0xFFFF) ;
  
  r >>= 16 ;
  r += a0 * x2 + a1 * x1 + a2 * x0 ;
  state->x2 = (r & 0xFFFF) ;
}

static unsigned long int 
ranf_get (void *vstate)
{
  unsigned long int x1, x2;

  ranf_state_t *state = (ranf_state_t *) vstate;
  ranf_advance (state) ;  

  x1 = (unsigned long int) state->x1;
  x2 = (unsigned long int) state->x2;
  
  return (x2 << 16) + x1;
}

static double
ranf_get_double (void * vstate)
{
  ranf_state_t *state = (ranf_state_t *) vstate;

  ranf_advance (state) ; 

  return (ldexp((double) state->x2, -16)
          + ldexp((double) state->x1, -32) 
          + ldexp((double) state->x0, -48)) ;
}

static void
ranf_set (void *vstate, unsigned long int s)
{
  ranf_state_t *state = (ranf_state_t *) vstate;

  unsigned short int x0, x1, x2 ;
  unsigned long int r ;

  unsigned long int b0 = 0xD6DD ;
  unsigned long int b1 = 0xB894 ;
  unsigned long int b2 = 0x5CEE ;

  if (s == 0)  /* default seed */
    {
      x0 = 0x9CD1 ;
      x1 = 0x53FC ;
      x2 = 0x9482 ;
    }
  else 
    {
      x0 = (s | 1) & 0xFFFF ;
      x1 = s >> 16 & 0xFFFF ;
      x2 = 0 ;
    }

  r = b0 * x0 ;
  state->x0 = (r & 0xFFFF) ;
 
  r >>= 16 ;
  r += b0 * x1 + b1 * x0 ;
  state->x1 = (r & 0xFFFF) ;
  
  r >>= 16 ;
  r += b0 * x2 + b1 * x1 + b2 * x0 ;
  state->x2 = (r & 0xFFFF) ;

  return;
}

static const gsl_rng_type ranf_type =
{"ranf",                        /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranf_state_t),
 &ranf_set,
 &ranf_get,
 &ranf_get_double
};

const gsl_rng_type *gsl_rng_ranf = &ranf_type;
