/* rng/ranlxs.c
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

/* This is an implementation of M. Luescher's second generation
   version of the RANLUX generator.

   Thanks to Martin Luescher for providing information on this
   generator.

 */

static unsigned long int ranlxs_get (void *vstate);
static inline double ranlxs_get_double (void *vstate);
static void ranlxs_set_lux (void *state, unsigned long int s, unsigned int luxury);
static void ranlxs0_set (void *state, unsigned long int s);
static void ranlxs1_set (void *state, unsigned long int s);
static void ranlxs2_set (void *state, unsigned long int s);

static const int next[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0};
static const int snext[24] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0};

static const double sbase = 16777216.0;         /* 2^24 */
static const double sone_bit = 1.0 / 16777216.0;        /* 1/2^24 */
static const double one_bit = 1.0 / 281474976710656.0;  /* 1/2^48 */

static const double shift = 268435456.0;        /* 2^28 */

#define RANLUX_STEP(x1,x2,i1,i2,i3)      \
          x1=xdbl[i1] - xdbl[i2];        \
          if (x2 < 0)                    \
          {                              \
            x1-=one_bit;                 \
            x2+=1;                       \
          }                              \
          xdbl[i3]=x2

typedef struct
  {
    double xdbl[12], ydbl[12];  /* doubles first so they are 8-byte aligned */
    double carry;
    float xflt[24];
    unsigned int ir;
    unsigned int jr;
    unsigned int is;
    unsigned int is_old;
    unsigned int pr;
  }
ranlxs_state_t;

static void increment_state (ranlxs_state_t * state);

static void
increment_state (ranlxs_state_t * state)
{
  int k, kmax, m;
  double x, y1, y2, y3;

  float *xflt = state->xflt;
  double *xdbl = state->xdbl;
  double *ydbl = state->ydbl;
  double carry = state->carry;
  unsigned int ir = state->ir;
  unsigned int jr = state->jr;

  for (k = 0; ir > 0; ++k)
    {
      y1 = xdbl[jr] - xdbl[ir];
      y2 = y1 - carry;
      if (y2 < 0)
        {
          carry = one_bit;
          y2 += 1;
        }
      else
        {
          carry = 0;
        }
      xdbl[ir] = y2;
      ir = next[ir];
      jr = next[jr];
    }

  kmax = state->pr - 12;

  for (; k <= kmax; k += 12)
    {
      y1 = xdbl[7] - xdbl[0];
      y1 -= carry;

      RANLUX_STEP (y2, y1, 8, 1, 0);
      RANLUX_STEP (y3, y2, 9, 2, 1);
      RANLUX_STEP (y1, y3, 10, 3, 2);
      RANLUX_STEP (y2, y1, 11, 4, 3);
      RANLUX_STEP (y3, y2, 0, 5, 4);
      RANLUX_STEP (y1, y3, 1, 6, 5);
      RANLUX_STEP (y2, y1, 2, 7, 6);
      RANLUX_STEP (y3, y2, 3, 8, 7);
      RANLUX_STEP (y1, y3, 4, 9, 8);
      RANLUX_STEP (y2, y1, 5, 10, 9);
      RANLUX_STEP (y3, y2, 6, 11, 10);

      if (y3 < 0)
        {
          carry = one_bit;
          y3 += 1;
        }
      else
        {
          carry = 0;
        }
      xdbl[11] = y3;
    }

  kmax = state->pr;

  for (; k < kmax; ++k)
    {
      y1 = xdbl[jr] - xdbl[ir];
      y2 = y1 - carry;
      if (y2 < 0)
        {
          carry = one_bit;
          y2 += 1;
        }
      else
        {
          carry = 0;
        }
      xdbl[ir] = y2;
      ydbl[ir] = y2 + shift;
      ir = next[ir];
      jr = next[jr];
    }

  ydbl[ir] = xdbl[ir] + shift;

  for (k = next[ir]; k > 0;)
    {
      ydbl[k] = xdbl[k] + shift;
      k = next[k];
    }

  for (k = 0, m = 0; k < 12; ++k)
    {
      x = xdbl[k];
      y2 = ydbl[k] - shift;
      if (y2 > x)
        y2 -= sone_bit;
      y1 = (x - y2) * sbase;

      xflt[m++] = (float) y1;
      xflt[m++] = (float) y2;
    }

  state->ir = ir;
  state->is = 2 * ir;
  state->is_old = 2 * ir;
  state->jr = jr;
  state->carry = carry;
}


static inline double
ranlxs_get_double (void *vstate)
{
  ranlxs_state_t *state = (ranlxs_state_t *) vstate;

  const unsigned int is = snext[state->is];

  state->is = is;

  if (is == state->is_old)
    increment_state (state);

  return state->xflt[state->is];
}

static unsigned long int
ranlxs_get (void *vstate)
{
  return ranlxs_get_double (vstate) * 16777216.0;       /* 2^24 */
}

static void
ranlxs_set_lux (void *vstate, unsigned long int s, unsigned int luxury)
{
  ranlxs_state_t *state = (ranlxs_state_t *) vstate;

  int ibit, jbit, i, k, m, xbit[31];
  double x, y;

  long int seed;

  if (s == 0)
    s = 1;                      /* default seed is 1 */

  seed = s;

  i = seed & 0xFFFFFFFFUL;

  for (k = 0; k < 31; ++k)
    {
      xbit[k] = i % 2;
      i /= 2;
    }

  ibit = 0;
  jbit = 18;

  for (k = 0; k < 12; ++k)
    {
      x = 0;

      for (m = 1; m <= 48; ++m)
        {
          y = (double) xbit[ibit];
          x += x + y;
          xbit[ibit] = (xbit[ibit] + xbit[jbit]) % 2;
          ibit = (ibit + 1) % 31;
          jbit = (jbit + 1) % 31;
        }
      state->xdbl[k] = one_bit * x;
    }

  state->carry = 0;
  state->ir = 0;
  state->jr = 7;
  state->is = 23;
  state->is_old = 0;
  state->pr = luxury;
}

static void
ranlxs0_set (void *vstate, unsigned long int s)
{
  ranlxs_set_lux (vstate, s, 109);
}

void
ranlxs1_set (void *vstate, unsigned long int s)
{
  ranlxs_set_lux (vstate, s, 202);
}

static void
ranlxs2_set (void *vstate, unsigned long int s)
{
  ranlxs_set_lux (vstate, s, 397);
}


static const gsl_rng_type ranlxs0_type =
{"ranlxs0",                     /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranlxs_state_t),
 &ranlxs0_set,
 &ranlxs_get,
 &ranlxs_get_double};

static const gsl_rng_type ranlxs1_type =
{"ranlxs1",                     /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranlxs_state_t),
 &ranlxs1_set,
 &ranlxs_get,
 &ranlxs_get_double};

static const gsl_rng_type ranlxs2_type =
{"ranlxs2",                     /* name */
 0x00ffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (ranlxs_state_t),
 &ranlxs2_set,
 &ranlxs_get,
 &ranlxs_get_double};

const gsl_rng_type *gsl_rng_ranlxs0 = &ranlxs0_type;
const gsl_rng_type *gsl_rng_ranlxs1 = &ranlxs1_type;
const gsl_rng_type *gsl_rng_ranlxs2 = &ranlxs2_type;
