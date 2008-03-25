/* roots/convergence.c
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

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_roots.h"

int
gsl_root_test_interval (double x_lower, double x_upper, double epsabs, double epsrel)
{
  const double abs_lower = fabs(x_lower) ;
  const double abs_upper = fabs(x_upper) ;

  double min_abs, tolerance;

  if (epsrel < 0.0)
    GSL_ERROR ("relative tolerance is negative", GSL_EBADTOL);
  
  if (epsabs < 0.0)
    GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);

  if (x_lower > x_upper)
    GSL_ERROR ("lower bound larger than upper bound", GSL_EINVAL);

  if ((x_lower > 0.0 && x_upper > 0.0) || (x_lower < 0.0 && x_upper < 0.0)) 
    {
      min_abs = GSL_MIN_DBL(abs_lower, abs_upper) ;
    }
  else
    {
      min_abs = 0;
    }

  tolerance = epsabs + epsrel * min_abs  ;
  
  if (fabs(x_upper - x_lower) < tolerance)
    return GSL_SUCCESS;
  
  return GSL_CONTINUE ;
}

int
gsl_root_test_delta (double x1, double x0, double epsabs, double epsrel)
{
  const double tolerance = epsabs + epsrel * fabs(x1)  ;

  if (epsrel < 0.0)
    GSL_ERROR ("relative tolerance is negative", GSL_EBADTOL);
  
  if (epsabs < 0.0)
    GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);
  
  if (fabs(x1 - x0) < tolerance || x1 == x0)
    return GSL_SUCCESS;
  
  return GSL_CONTINUE ;
}

int
gsl_root_test_residual (double f, double epsabs)
{
  if (epsabs < 0.0)
    GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);
 
  if (fabs(f) < epsabs)
    return GSL_SUCCESS;
  
  return GSL_CONTINUE ;
}

