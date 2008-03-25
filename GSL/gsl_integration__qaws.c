/* integration/qaws.c
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
#include <math.h>
#include <float.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_integration.h"

#include "gsl_integration__initialise.c"
#include "gsl_integration__append.c"
#include "gsl_integration__qpsrt.c"
#include "gsl_integration__util.c"
#include "gsl_integration__qc25s.c"

int
gsl_integration_qaws (gsl_function * f,
                      const double a, const double b,
                      gsl_integration_qaws_table * t,
                      const double epsabs, const double epsrel,
                      const size_t limit,
                      gsl_integration_workspace * workspace,
                      double *result, double *abserr)
{
  double area, errsum;
  double result0, abserr0;
  double tolerance;
  size_t iteration = 0;
  int roundoff_type1 = 0, roundoff_type2 = 0, error_type = 0;

  /* Initialize results */

  initialise (workspace, a, b);

  *result = 0;
  *abserr = 0;

  if (limit > workspace->limit)
    {
      GSL_ERROR ("iteration limit exceeds available workspace", GSL_EINVAL) ;
    }

  if (b <= a) 
    {
      GSL_ERROR ("limits must form an ascending sequence, a < b", GSL_EINVAL) ;
    }

  if (epsabs <= 0 && (epsrel < 50 * GSL_DBL_EPSILON || epsrel < 0.5e-28))
    {
      GSL_ERROR ("tolerance cannot be acheived with given epsabs and epsrel",
                 GSL_EBADTOL);
    }

  /* perform the first integration */

  {
    double area1, area2;
    double error1, error2;
    int err_reliable1, err_reliable2;
    double a1 = a;
    double b1 = 0.5 * (a + b);
    double a2 = b1;
    double b2 = b;

    qc25s (f, a, b, a1, b1, t, &area1, &error1, &err_reliable1);
    qc25s (f, a, b, a2, b2, t, &area2, &error2, &err_reliable2);
    
    if (error1 > error2)
      {
        append_interval (workspace, a1, b1, area1, error1);
        append_interval (workspace, a2, b2, area2, error2);
      }
    else
      {
        append_interval (workspace, a2, b2, area2, error2);
        append_interval (workspace, a1, b1, area1, error1);
      }
    
    result0 = area1 + area2;
    abserr0 = error1 + error2;
  }

  /* Test on accuracy */

  tolerance = GSL_MAX_DBL (epsabs, epsrel * fabs (result0));

  /* Test on accuracy, use 0.01 relative error as an extra safety
     margin on the first iteration (ignored for subsequent iterations) */

  if (abserr0 < tolerance && abserr0 < 0.01 * fabs(result0))
    {
      *result = result0;
      *abserr = abserr0;

      return GSL_SUCCESS;
    }
  else if (limit == 1)
    {
      *result = result0;
      *abserr = abserr0;

      GSL_ERROR ("a maximum of one iteration was insufficient", GSL_EMAXITER);
    }

  area = result0;
  errsum = abserr0;

  iteration = 2;

  do
    {
      double a1, b1, a2, b2;
      double a_i, b_i, r_i, e_i;
      double area1 = 0, area2 = 0, area12 = 0;
      double error1 = 0, error2 = 0, error12 = 0;
      int err_reliable1, err_reliable2;

      /* Bisect the subinterval with the largest error estimate */

      retrieve (workspace, &a_i, &b_i, &r_i, &e_i);

      a1 = a_i; 
      b1 = 0.5 * (a_i + b_i);
      a2 = b1;
      b2 = b_i;

      qc25s (f, a, b, a1, b1, t, &area1, &error1, &err_reliable1);
      qc25s (f, a, b, a2, b2, t, &area2, &error2, &err_reliable2);

      area12 = area1 + area2;
      error12 = error1 + error2;

      errsum += (error12 - e_i);
      area += area12 - r_i;

      if (err_reliable1 && err_reliable2)
        {
          double delta = r_i - area12;

          if (fabs (delta) <= 1.0e-5 * fabs (area12) && error12 >= 0.99 * e_i)
            {
              roundoff_type1++;
            }
          if (iteration >= 10 && error12 > e_i)
            {
              roundoff_type2++;
            }
        }

      tolerance = GSL_MAX_DBL (epsabs, epsrel * fabs (area));

      if (errsum > tolerance)
        {
          if (roundoff_type1 >= 6 || roundoff_type2 >= 20)
            {
              error_type = 2;   /* round off error */
            }

          /* set error flag in the case of bad integrand behaviour at
             a point of the integration range */

          if (subinterval_too_small (a1, a2, b2))
            {
              error_type = 3;
            }
        }

      update (workspace, a1, b1, area1, error1, a2, b2, area2, error2);

      retrieve (workspace, &a_i, &b_i, &r_i, &e_i);

      iteration++;

    }
  while (iteration < limit && !error_type && errsum > tolerance);

  *result = sum_results (workspace);
  *abserr = errsum;

  if (errsum <= tolerance)
    {
      return GSL_SUCCESS;
    }
  else if (error_type == 2)
    {
      GSL_ERROR ("roundoff error prevents tolerance from being achieved",
                 GSL_EROUND);
    }
  else if (error_type == 3)
    {
      GSL_ERROR ("bad integrand behavior found in the integration interval",
                 GSL_ESING);
    }
  else if (iteration == limit)
    {
      GSL_ERROR ("maximum number of subdivisions reached", GSL_EMAXITER);
    }
  else
    {
      GSL_ERROR ("could not integrate function", GSL_EFAILED);
    }

}
