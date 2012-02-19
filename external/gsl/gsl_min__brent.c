/* min/brent.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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

/* brent.c -- brent minimum finding algorithm */

#include "gsl__config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_min.h"

#include "gsl_min__min.h"

typedef struct
  {
    double d, e, v, w;
    double f_v, f_w;
  }
brent_state_t;

static int brent_init (void *vstate, gsl_function * f, double x_minimum, double f_minimum, double x_lower, double f_lower, double x_upper, double f_upper);
static int brent_iterate (void *vstate, gsl_function * f, double *x_minimum, double * f_minimum, double * x_lower, double * f_lower, double * x_upper, double * f_upper);

static int
brent_init (void *vstate, gsl_function * f, double x_minimum, double f_minimum, double x_lower, double f_lower, double x_upper, double f_upper)
{
  brent_state_t *state = (brent_state_t *) vstate;

  const double golden = 0.3819660;      /* golden = (3 - sqrt(5))/2 */

  double v = x_lower + golden * (x_upper - x_lower);
  double w = v;

  double f_vw;

  x_minimum = 0 ;  /* avoid warnings about unused varibles */
  f_minimum = 0 ;
  f_lower = 0 ;
  f_upper = 0 ;

  state->v = v;
  state->w = w;

  state->d = 0;
  state->e = 0;

  SAFE_FUNC_CALL (f, v, &f_vw);

  state->f_v = f_vw;
  state->f_w = f_vw;

  return GSL_SUCCESS;
}

static int
brent_iterate (void *vstate, gsl_function * f, double *x_minimum, double * f_minimum, double * x_lower, double * f_lower, double * x_upper, double * f_upper)
{
  brent_state_t *state = (brent_state_t *) vstate;

  const double x_left = *x_lower;
  const double x_right = *x_upper;

  const double z = *x_minimum;
  double d = state->e;
  double e = state->d;
  double u, f_u;
  const double v = state->v;
  const double w = state->w;
  const double f_v = state->f_v;
  const double f_w = state->f_w;
  const double f_z = *f_minimum;

  const double golden = 0.3819660;      /* golden = (3 - sqrt(5))/2 */

  const double w_lower = (z - x_left);
  const double w_upper = (x_right - z);

  const double tolerance =  GSL_SQRT_DBL_EPSILON * fabs (z);

  double p = 0, q = 0, r = 0;

  const double midpoint = 0.5 * (x_left + x_right);

  if (fabs (e) > tolerance)
    {
      /* fit parabola */

      r = (z - w) * (f_z - f_v);
      q = (z - v) * (f_z - f_w);
      p = (z - v) * q - (z - w) * r;
      q = 2 * (q - r);

      if (q > 0)
        {
          p = -p;
        }
      else
        {
          q = -q;
        }

      r = e;
      e = d;
    }

  if (fabs (p) < fabs (0.5 * q * r) && p < q * w_lower && p < q * w_upper)
    {
      double t2 = 2 * tolerance ;

      d = p / q;
      u = z + d;

      if ((u - x_left) < t2 || (x_right - u) < t2)
        {
          d = (z < midpoint) ? tolerance : -tolerance ;
        }
    }
  else
    {
      e = (z < midpoint) ? x_right - z : -(z - x_left) ;
      d = golden * e;
    }


  if (fabs (d) >= tolerance)
    {
      u = z + d;
    }
  else
    {
      u = z + ((d > 0) ? tolerance : -tolerance) ;
    }

  state->e = e;
  state->d = d;

  SAFE_FUNC_CALL(f, u, &f_u);

  if (f_u <= f_z)
    {
      if (u < z)
        {
          *x_upper = z;
          *f_upper = f_z;
        }
      else
        {
          *x_lower = z;
          *f_lower = f_z;
        }

      state->v = w;
      state->f_v = f_w;
      state->w = z;
      state->f_w = f_z;
      *x_minimum = u;
      *f_minimum = f_u;
      return GSL_SUCCESS;
    }
  else
    {
      if (u < z)
        {
          *x_lower = u;
          *f_lower = f_u;
          return GSL_SUCCESS;
        }
      else
        {
          *x_upper = u;
          *f_upper = f_u;
          return GSL_SUCCESS;
        }

      if (f_u <= f_w || w == z)
        {
          state->v = w;
          state->f_v = f_w;
          state->w = u;
          state->f_w = f_u;
          return GSL_SUCCESS;
        }
      else if (f_u <= f_v || v == z || v == w)
        {
          state->v = u;
          state->f_v = f_u;
          return GSL_SUCCESS;
        }
    }

  return GSL_FAILURE;
}


static const gsl_min_fminimizer_type brent_type =
{"brent",                       /* name */
 sizeof (brent_state_t),
 &brent_init,
 &brent_iterate};

const gsl_min_fminimizer_type *gsl_min_fminimizer_brent = &brent_type;
