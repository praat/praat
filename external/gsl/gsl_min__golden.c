/* min/golden.c
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


/* goldensection.c -- goldensection minimum finding algorithm */

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
    double dummy;
  }
goldensection_state_t;

static int goldensection_init (void * vstate, gsl_function * f, double x_minimum, double f_minimum, double x_lower, double f_lower, double x_upper, double f_upper);
static int goldensection_iterate (void * vstate, gsl_function * f, double * x_minimum, double * f_minimum, double * x_lower, double * f_lower, double * x_upper, double * f_upper);

static int
goldensection_init (void * vstate, gsl_function * f, double x_minimum, double f_minimum, double x_lower, double f_lower, double x_upper, double f_upper)
{

  /* no initialization required, prevent warnings about unused variables */

  f = 0;
  x_minimum = 0;
  f_minimum = 0;
  x_lower = 0;
  f_lower = 0;
  x_upper = 0;
  f_upper = 0;

  return GSL_SUCCESS;
}

static int
goldensection_iterate (void * vstate, gsl_function * f, double * x_minimum, double * f_minimum, double * x_lower, double * f_lower, double * x_upper, double * f_upper)
{
  const double x_center = *x_minimum ;
  const double x_left = *x_lower ;
  const double x_right = *x_upper ;

  const double f_min = *f_minimum;

  const double golden = 0.3819660; /* golden = (3 - sqrt(5))/2 */
  
  const double w_lower = (x_center - x_left);
  const double w_upper = (x_right - x_center);

  double x_new, f_new;
  
  x_new = x_center + golden * ((w_upper > w_lower) ? w_upper : -w_lower) ;

  SAFE_FUNC_CALL (f, x_new, &f_new);

  if (f_new < f_min)
    {
      *x_minimum = x_new ;
      *f_minimum = f_new ;
      return GSL_SUCCESS;
    }
  else if (x_new < x_center && f_new > f_min)
    {
      *x_lower = x_new ;
      *f_lower = f_new ;
      return GSL_SUCCESS;
    }
  else if (x_new > x_center && f_new > f_min)
    {
      *x_upper = x_new ;
      *f_upper = f_new ;
      return GSL_SUCCESS;
    }
  else
    {
      return GSL_FAILURE;
    }
}


static const gsl_min_fminimizer_type goldensection_type =
{"goldensection",                               /* name */
 sizeof (goldensection_state_t),
 &goldensection_init,
 &goldensection_iterate};

const gsl_min_fminimizer_type  * gsl_min_fminimizer_goldensection = &goldensection_type;
