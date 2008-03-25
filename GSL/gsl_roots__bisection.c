/* roots/bisection.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
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


/* bisection.c -- bisection root finding algorithm */

#include "gsl__config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_roots.h"

#include "gsl_roots__roots.h"

typedef struct
  {
    double f_lower, f_upper;
  }
bisection_state_t;

static int bisection_init (void * vstate, gsl_function * f, double * root, double x_lower, double x_upper);
static int bisection_iterate (void * vstate, gsl_function * f, double * root, double * x_lower, double * x_upper);

static int
bisection_init (void * vstate, gsl_function * f, double * root, double x_lower, double x_upper)
{
  bisection_state_t * state = (bisection_state_t *) vstate;

  double f_lower, f_upper ;

  *root = 0.5 * (x_lower + x_upper) ;

  SAFE_FUNC_CALL (f, x_lower, &f_lower);
  SAFE_FUNC_CALL (f, x_upper, &f_upper);
  
  state->f_lower = f_lower;
  state->f_upper = f_upper;

  if ((f_lower < 0.0 && f_upper < 0.0) || (f_lower > 0.0 && f_upper > 0.0))
    {
      GSL_ERROR ("endpoints do not straddle y=0", GSL_EINVAL);
    }

  return GSL_SUCCESS;

}

static int
bisection_iterate (void * vstate, gsl_function * f, double * root, double * x_lower, double * x_upper)
{
  bisection_state_t * state = (bisection_state_t *) vstate;

  double x_bisect, f_bisect;

  const double x_left = *x_lower ;
  const double x_right = *x_upper ;

  const double f_lower = state->f_lower; 
  const double f_upper = state->f_upper;

  if (f_lower == 0.0)
    {
      *root = x_left ;
      *x_upper = x_left;
      return GSL_SUCCESS;
    }
  
  if (f_upper == 0.0)
    {
      *root = x_right ;
      *x_lower = x_right;
      return GSL_SUCCESS;
    }
  
  x_bisect = (x_left + x_right) / 2.0;
  
  SAFE_FUNC_CALL (f, x_bisect, &f_bisect);
      
  if (f_bisect == 0.0)
    {
      *root = x_bisect;
      *x_lower = x_bisect;
      *x_upper = x_bisect;
      return GSL_SUCCESS;
    }
      
  /* Discard the half of the interval which doesn't contain the root. */
  
  if ((f_lower > 0.0 && f_bisect < 0.0) || (f_lower < 0.0 && f_bisect > 0.0))
    {
      *root = 0.5 * (x_left + x_bisect) ;
      *x_upper = x_bisect;
      state->f_upper = f_bisect;
    }
  else
    {
      *root = 0.5 * (x_bisect + x_right) ;
      *x_lower = x_bisect;
      state->f_lower = f_bisect;
    }

  return GSL_SUCCESS;
}


static const gsl_root_fsolver_type bisection_type =
{"bisection",                           /* name */
 sizeof (bisection_state_t),
 &bisection_init,
 &bisection_iterate};

const gsl_root_fsolver_type  * gsl_root_fsolver_bisection = &bisection_type;
