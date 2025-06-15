/* rng/ranmar.c
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

/* This is the RANMAR lagged fibonacci generator of Marsaglia, Zaman
   and Tsang.  The sequence is a series of 24-bit integers, x_n,

   x_n = (y_n - c_n + 2^24) mod 2^24

   where,

   y_n = (y_{n-97) - y_{n-33} + 2^24) mod 2^24
   c_n = (c_{n-1} - 7654321 + 2^24 - 3) mod (2^24 - 3)

   The period of this generator is 2^144.

   The generator provides about 900 million different subsequences
   each of length O(10^30). Thus each seed up to 900,000,000 gives an
   independent sequence.

   Although it was good in its day this generator now has known
   statistical defects and has been superseded by RANLUX.

   From: F. James, "A Review of Pseudorandom number generators",
   Computer Physics Communications 60, 329 (1990).

   G. Marsaglia, A. Zaman and W.W. Tsang, Stat. Prob. Lett. 9, 35 (1990)  */

static inline unsigned long int ranmar_get (void *vstate);
static double ranmar_get_double (void *vstate);
static void ranmar_set (void *state, unsigned long int s);

static const unsigned long int two24 = 16777216;        /* 2^24 */

typedef struct
  {
    unsigned int i;
    unsigned int j;
    long int carry;
    unsigned long int u[97];
  }
ranmar_state_t;

static inline unsigned long int
ranmar_get (void *vstate)
{
  ranmar_state_t *state = (ranmar_state_t *) vstate;

  unsigned int i = state->i;
  unsigned int j = state->j;
  long int carry = state->carry;

  long int delta = state->u[i] - state->u[j];

  if (delta < 0)
    delta += two24 ;
  
  state->u[i] = delta;

  if (i == 0)
    {
      i = 96;
    }
  else
    {
      i--;
    }

  state->i = i;

  if (j == 0)
    {
      j = 96;
    }
  else
    {
      j--;
    }

  state->j = j;

  carry += - 7654321 ;
  
  if (carry < 0)
    carry += two24 - 3;
  
  state->carry = carry ;

  delta += - carry ;
  
  if (delta < 0)
    delta += two24 ;

  return delta;
}

static double
ranmar_get_double (void *vstate)
{
  return ranmar_get (vstate) / 16777216.0 ;
}

static void
ranmar_set (void *vstate, unsigned long int s)
{
  ranmar_state_t *state = (ranmar_state_t *) vstate;
  
  unsigned long int ij = s / 30082 ;
  unsigned long int kl = s % 30082 ;
  
  int i = (ij / 177) % 177 + 2 ;
  int j = (ij % 177) + 2 ;
  int k = (kl / 169) % 178 + 1 ;
  int l = (kl % 169) ;

  int a, b;
  
  for (a = 0; a < 97; a++)
    {
      unsigned long int sum = 0 ;
      unsigned long int t = two24 ;

      for (b = 0; b < 24; b++)
        {
          unsigned long int m = (((i * j) % 179) * k) % 179 ;
          i = j ;
          j = k ;
          k = m ;
          l = (53 * l + 1) % 169 ;
          t >>= 1 ;
          
          if ((l * m) % 64 >= 32)
            sum += t ;
        }

      state->u[a] = sum ;
    }

  state->i = 96;
  state->j = 32;
  state->carry = 362436 ;

}

static const gsl_rng_type ranmar_type =
{"ranmar",                      /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranmar_state_t),
 &ranmar_set,
 &ranmar_get,
 &ranmar_get_double};

const gsl_rng_type *gsl_rng_ranmar = &ranmar_type;
