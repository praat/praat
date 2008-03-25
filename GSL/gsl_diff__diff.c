/* diff/diff.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 David Morrison
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
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_diff.h"

int
gsl_diff_backward (const gsl_function * f,
                   double x, double *result, double *abserr)
{
  /* Construct a divided difference table with a fairly large step
     size to get a very rough estimate of f''.  Use this to estimate
     the step size which will minimize the error in calculating f'. */

  int i, k;
  double h = GSL_SQRT_DBL_EPSILON;
  double a[3], d[3], a2;

  /* Algorithm based on description on pg. 204 of Conte and de Boor
     (CdB) - coefficients of Newton form of polynomial of degree 2. */

  for (i = 0; i < 3; i++)
    {
      a[i] = x + (i - 2.0) * h;
      d[i] = GSL_FN_EVAL (f, a[i]);
    }

  for (k = 1; k < 4; k++)
    {
      for (i = 0; i < 3 - k; i++)
        {
          d[i] = (d[i + 1] - d[i]) / (a[i + k] - a[i]);
        }
    }

  /* Adapt procedure described on pg. 282 of CdB to find best value of
     step size. */

  a2 = fabs (d[0] + d[1] + d[2]);

  if (a2 < 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      a2 = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  h = sqrt (GSL_SQRT_DBL_EPSILON / (2.0 * a2));

  if (h > 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      h = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  *result = (GSL_FN_EVAL (f, x) - GSL_FN_EVAL (f, x - h)) / h;
  *abserr = fabs (10.0 * a2 * h);

  return GSL_SUCCESS;
}

int
gsl_diff_forward (const gsl_function * f,
                  double x, double *result, double *abserr)
{
  /* Construct a divided difference table with a fairly large step
     size to get a very rough estimate of f''.  Use this to estimate
     the step size which will minimize the error in calculating f'. */

  int i, k;
  double h = GSL_SQRT_DBL_EPSILON;
  double a[3], d[3], a2;

  /* Algorithm based on description on pg. 204 of Conte and de Boor
     (CdB) - coefficients of Newton form of polynomial of degree 2. */

  for (i = 0; i < 3; i++)
    {
      a[i] = x + i * h;
      d[i] = GSL_FN_EVAL (f, a[i]);
    }

  for (k = 1; k < 4; k++)
    {
      for (i = 0; i < 3 - k; i++)
        {
          d[i] = (d[i + 1] - d[i]) / (a[i + k] - a[i]);
        }
    }

  /* Adapt procedure described on pg. 282 of CdB to find best value of
     step size. */

  a2 = fabs (d[0] + d[1] + d[2]);

  if (a2 < 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      a2 = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  h = sqrt (GSL_SQRT_DBL_EPSILON / (2.0 * a2));

  if (h > 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      h = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  *result = (GSL_FN_EVAL (f, x + h) - GSL_FN_EVAL (f, x)) / h;
  *abserr = fabs (10.0 * a2 * h);

  return GSL_SUCCESS;
}

int
gsl_diff_central (const gsl_function * f,
                  double x, double *result, double *abserr)
{
  /* Construct a divided difference table with a fairly large step
     size to get a very rough estimate of f'''.  Use this to estimate
     the step size which will minimize the error in calculating f'. */

  int i, k;
  double h = GSL_SQRT_DBL_EPSILON;
  double a[4], d[4], a3;

  /* Algorithm based on description on pg. 204 of Conte and de Boor
     (CdB) - coefficients of Newton form of polynomial of degree 3. */

  for (i = 0; i < 4; i++)
    {
      a[i] = x + (i - 2.0) * h;
      d[i] = GSL_FN_EVAL (f, a[i]);
    }

  for (k = 1; k < 5; k++)
    {
      for (i = 0; i < 4 - k; i++)
        {
          d[i] = (d[i + 1] - d[i]) / (a[i + k] - a[i]);
        }
    }

  /* Adapt procedure described on pg. 282 of CdB to find best
     value of step size. */

  a3 = fabs (d[0] + d[1] + d[2] + d[3]);

  if (a3 < 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      a3 = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  h = pow (GSL_SQRT_DBL_EPSILON / (2.0 * a3), 1.0 / 3.0);

  if (h > 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      h = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  *result = (GSL_FN_EVAL (f, x + h) - GSL_FN_EVAL (f, x - h)) / (2.0 * h);
  *abserr = fabs (100.0 * a3 * h * h);

  return GSL_SUCCESS;
}
