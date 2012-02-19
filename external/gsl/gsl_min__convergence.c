/* min/convergence.c
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

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_min.h"

int
gsl_min_test_interval (double x_lower, double x_upper, double epsabs, double epsrel)
{
  const double lower = x_lower;
  const double upper = x_upper;

  const double abs_lower = fabs(lower) ;
  const double abs_upper = fabs(upper) ;

  double min_abs, tolerance;

  if (epsrel < 0.0)
    GSL_ERROR ("relative tolerance is negative", GSL_EBADTOL);
  
  if (epsabs < 0.0)
    GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);

  if (lower > upper)
    GSL_ERROR ("lower bound larger than upper_bound", GSL_EINVAL);

  if ((lower > 0 && upper > 0) || (lower < 0 && upper < 0)) 
    {
      min_abs = GSL_MIN_DBL(abs_lower, abs_upper) ;
    }
  else
    {
      min_abs = 0;
    }

  tolerance = epsabs + epsrel * min_abs  ;
  
  if (fabs(upper - lower) < tolerance)
    return GSL_SUCCESS;
  
  return GSL_CONTINUE ;
}

