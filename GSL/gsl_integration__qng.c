/* integration/qng.c
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

#include "gsl_integration__err.c"
#include "gsl_integration__qng.h"

int
gsl_integration_qng (const gsl_function *f,
                     double a, double b,
                     double epsabs, double epsrel,
                     double * result, double * abserr, size_t * neval)
{
  double fv1[5], fv2[5], fv3[5], fv4[5];
  double savfun[21];  /* array of function values which have been computed */
  double res10, res21, res43, res87;    /* 10, 21, 43 and 87 point results */
  double result_kronrod, err ; 
  double resabs; /* approximation to the integral of abs(f) */
  double resasc; /* approximation to the integral of abs(f-i/(b-a)) */

  const double half_length =  0.5 * (b - a);
  const double abs_half_length = fabs (half_length);
  const double center = 0.5 * (b + a);
  const double f_center = GSL_FN_EVAL(f, center);

  int k ;

  if (epsabs <= 0 && (epsrel < 50 * GSL_DBL_EPSILON || epsrel < 0.5e-28))
    {
      * result = 0;
      * abserr = 0;
      * neval = 0;
      GSL_ERROR ("tolerance cannot be acheived with given epsabs and epsrel",
                 GSL_EBADTOL);
    };

  /* Compute the integral using the 10- and 21-point formula. */

  res10 = 0;
  res21 = w21b[5] * f_center;
  resabs = w21b[5] * fabs (f_center);

  for (k = 0; k < 5; k++)
    {
      const double abscissa = half_length * x1[k];
      const double fval1 = GSL_FN_EVAL(f, center + abscissa);
      const double fval2 = GSL_FN_EVAL(f, center - abscissa);
      const double fval = fval1 + fval2;
      res10 += w10[k] * fval;
      res21 += w21a[k] * fval;
      resabs += w21a[k] * (fabs (fval1) + fabs (fval2));
      savfun[k] = fval;
      fv1[k] = fval1;
      fv2[k] = fval2;
    }

  for (k = 0; k < 5; k++)
    {
      const double abscissa = half_length * x2[k];
      const double fval1 = GSL_FN_EVAL(f, center + abscissa);
      const double fval2 = GSL_FN_EVAL(f, center - abscissa);
      const double fval = fval1 + fval2;
      res21 += w21b[k] * fval;
      resabs += w21b[k] * (fabs (fval1) + fabs (fval2));
      savfun[k + 5] = fval;
      fv3[k] = fval1;
      fv4[k] = fval2;
    }

  resabs *= abs_half_length ;

  { 
    const double mean = 0.5 * res21;
  
    resasc = w21b[5] * fabs (f_center - mean);
    
    for (k = 0; k < 5; k++)
      {
        resasc +=
          (w21a[k] * (fabs (fv1[k] - mean) + fabs (fv2[k] - mean))
          + w21b[k] * (fabs (fv3[k] - mean) + fabs (fv4[k] - mean)));
      }
    resasc *= abs_half_length ;
  }

  result_kronrod = res21 * half_length;
  
  err = rescale_error ((res21 - res10) * half_length, resabs, resasc) ;

  /*   test for convergence. */

  if (err < epsabs || err < epsrel * fabs (result_kronrod))
    {
      * result = result_kronrod ;
      * abserr = err ;
      * neval = 21;
      return GSL_SUCCESS;
    }

  /* compute the integral using the 43-point formula. */

  res43 = w43b[11] * f_center;

  for (k = 0; k < 10; k++)
    {
      res43 += savfun[k] * w43a[k];
    }

  for (k = 0; k < 11; k++)
    {
      const double abscissa = half_length * x3[k];
      const double fval = (GSL_FN_EVAL(f, center + abscissa) 
                           + GSL_FN_EVAL(f, center - abscissa));
      res43 += fval * w43b[k];
      savfun[k + 10] = fval;
    }

  /*  test for convergence */

  result_kronrod = res43 * half_length;
  err = rescale_error ((res43 - res21) * half_length, resabs, resasc);

  if (err < epsabs || err < epsrel * fabs (result_kronrod))
    {
      * result = result_kronrod ;
      * abserr = err ;
      * neval = 43;
      return GSL_SUCCESS;
    }

  /* compute the integral using the 87-point formula. */

  res87 = w87b[22] * f_center;

  for (k = 0; k < 21; k++)
    {
      res87 += savfun[k] * w87a[k];
    }

  for (k = 0; k < 22; k++)
    {
      const double abscissa = half_length * x4[k];
      res87 += w87b[k] * (GSL_FN_EVAL(f, center + abscissa) 
                          + GSL_FN_EVAL(f, center - abscissa));
    }

  /*  test for convergence */

  result_kronrod = res87 * half_length ;
  
  err = rescale_error ((res87 - res43) * half_length, resabs, resasc);
  
  if (err < epsabs || err < epsrel * fabs (result_kronrod))
    {
      * result = result_kronrod ;
      * abserr = err ;
      * neval = 87;
      return GSL_SUCCESS;
    }

  /* failed to converge */

  * result = result_kronrod ;
  * abserr = err ;
  * neval = 87;

  GSL_ERROR("failed to reach tolerance with highest-order rule", GSL_ETOL) ;
}
