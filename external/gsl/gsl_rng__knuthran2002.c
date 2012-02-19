/* rng/knuthran2002.c
 * 
 * Copyright (C) 2007 Brian Gough
 * Copyright (C) 2001, 2007 Brian Gough, Carlo Perassi
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
 * Donald E. Knuth, The Art of Computer Programming, Volume 2, Section 3.6
 * Third Edition, Addison-Wesley, 
 * 
 * The modifications introduced in the 9th printing (2002) are
 * included here; there's no backwards compatibility with the
 * original.  [ see http://www-cs-faculty.stanford.edu/~knuth/taocp.html ] 
 * 
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

#define BUFLEN 1009             /* length of the buffer aa[] */
#define KK 100                  /* the long lag */
#define LL 37                   /* the short lag */
#define MM (1L << 30)           /* the modulus */
#define TT 70                   /* guaranteed separation between streams */

#define is_odd(x) ((x) & 1)                       /* the units bit of x */
#define mod_diff(x, y) (((x) - (y)) & (MM - 1))   /* (x - y) mod MM */

static inline void ran_array (long int aa[], unsigned int n,
                              long int ran_x[]);
static inline unsigned long int ran_get (void *vstate);
static double ran_get_double (void *vstate);
static void ran_set (void *state, unsigned long int s);

typedef struct
{
  unsigned int i;
  long int aa[BUFLEN]; 
  long int ran_x[KK];  /* the generator state */
}
ran_state_t;

static inline void
ran_array (long int aa[], unsigned int n, long int ran_x[])
{
  unsigned int i;
  unsigned int j;

  for (j = 0; j < KK; j++)
    aa[j] = ran_x[j];

  for (; j < n; j++)
    aa[j] = mod_diff (aa[j - KK], aa[j - LL]);

  for (i = 0; i < LL; i++, j++)
    ran_x[i] = mod_diff (aa[j - KK], aa[j - LL]);

  for (; i < KK; i++, j++)
    ran_x[i] = mod_diff (aa[j - KK], ran_x[i - LL]);
}

static inline unsigned long int
ran_get (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

  unsigned int i = state->i;
  unsigned long int v;

  if (i  == 0)
    {
      /* fill buffer with new random numbers */
      ran_array (state->aa, BUFLEN, state->ran_x);
    }

  v = state->aa[i];

  state->i = (i + 1) % KK;

  return v;
}

static double
ran_get_double (void *vstate)
{
  ran_state_t *state = (ran_state_t *) vstate;

  return ran_get (state) / 1073741824.0;        /* RAND_MAX + 1 */
}

static void
ran_set (void *vstate, unsigned long int s)
{
  ran_state_t *state = (ran_state_t *) vstate;

  long x[KK + KK - 1];          /* the preparation buffer */

  register int j;
  register int t;
  register long ss;

  if (s == 0 ) 
    s = 314159;                 /* default seed used by Knuth */

  ss = (s + 2)&(MM-2);

  for (j = 0; j < KK; j++)
    {
      x[j] = ss;                /* bootstrap the buffer */
      ss <<= 1;
      if (ss >= MM)             /* cyclic shift 29 bits */
        ss -= MM - 2;
    }
  x[1]++;                       /* make x[1] (and only x[1]) odd */

  ss = s & (MM - 1);
  t = TT - 1;
  while (t)
    {
      for (j = KK - 1; j > 0; j--)      /* square */
        {
          x[j + j] = x[j];
          x[j + j - 1] = 0;
        }

      for (j = KK + KK - 2; j >= KK; j--)
        {
          x[j - (KK - LL)] = mod_diff (x[j - (KK - LL)], x[j]);
          x[j - KK] = mod_diff (x[j - KK], x[j]);
        }

      if (is_odd (ss))
        {                       /* multiply by "z" */
          for (j = KK; j > 0; j--)
            {
              x[j] = x[j - 1];
            }
          x[0] = x[KK];         /* shift the buffer cyclically */
          x[LL] = mod_diff (x[LL], x[KK]);
        }

      if (ss)
        ss >>= 1;
      else
        t--;
    }

  for (j = 0; j < LL; j++)
    state->ran_x[j + KK - LL] = x[j];
  for (; j < KK; j++)
    state->ran_x[j - LL] = x[j];


  for (j = 0; j< 10; j++) 
    ran_array(x, KK+KK-1, state->ran_x);  /* warm things up */

  state->i = 0;

  return;
}

static const gsl_rng_type ran_type = {
  "knuthran2002",                /* name */
  0x3fffffffUL,                 /* RAND_MAX = (2 ^ 30) - 1 */
  0,                            /* RAND_MIN */
  sizeof (ran_state_t),
  &ran_set,
  &ran_get,
  &ran_get_double
};

const gsl_rng_type *gsl_rng_knuthran2002 = &ran_type;
