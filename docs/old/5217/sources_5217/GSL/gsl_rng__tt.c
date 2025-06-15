/* rng/tt.c
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

/* This is the TT800 twisted GSFR generator for 32 bit integers. It
   has been superceded by MT19937 (mt.c). The period is 2^800.

   This implementation is based on tt800.c, July 8th 1996 version by
   M. Matsumoto, email: matumoto@math.keio.ac.jp

   From: Makoto Matsumoto and Yoshiharu Kurita, "Twisted GFSR
   Generators II", ACM Transactions on Modelling and Computer
   Simulation, Vol. 4, No. 3, 1994, pages 254-266. */

static inline unsigned long int tt_get (void *vstate);
static double tt_get_double (void *vstate);
static void tt_set (void *state, unsigned long int s);

#define N 25
#define M 7

typedef struct
  {
    int n;
    unsigned long int x[N];
  }
tt_state_t;

static inline unsigned long int
tt_get (void *vstate)
{
  tt_state_t *state = (tt_state_t *) vstate;

  /* this is the magic vector, a */

  const unsigned long mag01[2] =
  {0x00000000, 0x8ebfd028UL};
  unsigned long int y;
  unsigned long int *const x = state->x;
  int n = state->n;

  if (n >= N)
    {
      int i;
      for (i = 0; i < N - M; i++)
        {
          x[i] = x[i + M] ^ (x[i] >> 1) ^ mag01[x[i] % 2];
        }
      for (; i < N; i++)
        {
          x[i] = x[i + (M - N)] ^ (x[i] >> 1) ^ mag01[x[i] % 2];
        };
      n = 0;
    }

  y = x[n];
  y ^= (y << 7) & 0x2b5b2500UL;         /* s and b, magic vectors */
  y ^= (y << 15) & 0xdb8b0000UL;        /* t and c, magic vectors */
  y &= 0xffffffffUL;    /* you may delete this line if word size = 32 */

  /* The following line was added by Makoto Matsumoto in the 1996
     version to improve lower bit's correlation.  Delete this line
     to use the code published in 1994.  */

  y ^= (y >> 16);       /* added to the 1994 version */

  state->n = n + 1;

  return y;
}

static double
tt_get_double (void * vstate)
{
  return tt_get (vstate) / 4294967296.0 ;
}

static void
tt_set (void *vstate, unsigned long int s)
{
  tt_state_t *state = (tt_state_t *) vstate;

  const tt_state_t init_state =
  {0,
   {0x95f24dabUL, 0x0b685215UL, 0xe76ccae7UL,
    0xaf3ec239UL, 0x715fad23UL, 0x24a590adUL,
    0x69e4b5efUL, 0xbf456141UL, 0x96bc1b7bUL,
    0xa7bdf825UL, 0xc1de75b7UL, 0x8858a9c9UL,
    0x2da87693UL, 0xb657f9ddUL, 0xffdc8a9fUL,
    0x8121da71UL, 0x8b823ecbUL, 0x885d05f5UL,
    0x4e20cd47UL, 0x5a9ad5d9UL, 0x512c0c03UL,
    0xea857ccdUL, 0x4cc1d30fUL, 0x8891a8a1UL,
    0xa6b7aadbUL}};


  if (s == 0)   /* default seed is given explicitly in the original code */
    {
      *state = init_state;
    }
  else
    {
      int i;

      state->n = 0;

      state->x[0] = s & 0xffffffffUL;

      for (i = 1; i < N; i++)
        state->x[i] = (69069 * state->x[i - 1]) & 0xffffffffUL;
    }

  return;
}

static const gsl_rng_type tt_type =
{"tt800",                       /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (tt_state_t),
 &tt_set,
 &tt_get,
 &tt_get_double};

const gsl_rng_type *gsl_rng_tt800 = &tt_type;
