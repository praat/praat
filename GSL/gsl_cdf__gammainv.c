/* cdf/gammainv.c
 * 
 * Copyright (C) 2003, 2007 Brian Gough
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_cdf.h"
#include "gsl_math.h"
#include "gsl_randist.h"
#include "gsl_sf_gamma.h"

#include <stdio.h>

double
gsl_cdf_gamma_Pinv (const double P, const double a, const double b)
{
  double x;

  if (P == 1.0)
    {
      return GSL_POSINF;
    }
  else if (P == 0.0)
    {
      return 0.0;
    }

  /* Consider, small, large and intermediate cases separately.  The
     boundaries at 0.05 and 0.95 have not been optimised, but seem ok
     for an initial approximation. */

  if (P < 0.05)
    {
      double x0 = exp ((gsl_sf_lngamma (a) + log (P)) / a);
      x = x0;
    }
  else if (P > 0.95)
    {
      double x0 = -log1p (-P) + gsl_sf_lngamma (a);
      x = x0;
    }
  else
    {
      double xg = gsl_cdf_ugaussian_Pinv (P);
      double x0 = (xg < -sqrt (a)) ? a : sqrt (a) * xg + a;
      x = x0;
    }

  /* Use Lagrange's interpolation for E(x)/phi(x0) to work backwards
     to an improved value of x (Abramowitz & Stegun, 3.6.6) 

     where E(x)=P-integ(phi(u),u,x0,x) and phi(u) is the pdf.
   */

  {
    double lambda, dP, phi;
    unsigned int n = 0;

  start:
    dP = P - gsl_cdf_gamma_P (x, a, 1.0);
    phi = gsl_ran_gamma_pdf (x, a, 1.0);

    if (dP == 0.0 || n++ > 32)
      goto end;

    lambda = dP / GSL_MAX (2 * fabs (dP / x), phi);

    {
      double step0 = lambda;
      double step1 = -((a - 1) / x - 1) * lambda * lambda / 4.0;

      double step = step0;
      if (fabs (step1) < fabs (step0))
        step += step1;

      if (x + step > 0)
        x += step;
      else
        {
          x /= 2.0;
        }

      if (fabs (step0) > 1e-10 * x)
        goto start;
    }

  end:
    if (fabs(dP) > GSL_SQRT_DBL_EPSILON * P)
      {
        GSL_ERROR_VAL("inverse failed to converge", GSL_EFAILED, GSL_NAN);
      }
    
    return b * x;
  }
}

double
gsl_cdf_gamma_Qinv (const double Q, const double a, const double b)
{
  double x;

  if (Q == 1.0)
    {
      return 0.0;
    }
  else if (Q == 0.0)
    {
      return GSL_POSINF;
    }

  /* Consider, small, large and intermediate cases separately.  The
     boundaries at 0.05 and 0.95 have not been optimised, but seem ok
     for an initial approximation. */

  if (Q < 0.05)
    {
      double x0 = -log (Q) + gsl_sf_lngamma (a);
      x = x0;
    }
  else if (Q > 0.95)
    {
      double x0 = exp ((gsl_sf_lngamma (a) + log1p (-Q)) / a);
      x = x0;
    }
  else
    {
      double xg = gsl_cdf_ugaussian_Qinv (Q);
      double x0 = (xg < -sqrt (a)) ? a : sqrt (a) * xg + a;
      x = x0;
    }

  /* Use Lagrange's interpolation for E(x)/phi(x0) to work backwards
     to an improved value of x (Abramowitz & Stegun, 3.6.6) 

     where E(x)=P-integ(phi(u),u,x0,x) and phi(u) is the pdf.
   */

  {
    double lambda, dQ, phi;
    unsigned int n = 0;

  start:
    dQ = Q - gsl_cdf_gamma_Q (x, a, 1.0);
    phi = gsl_ran_gamma_pdf (x, a, 1.0);

    if (dQ == 0.0 || n++ > 32)
      goto end;

    lambda = -dQ / GSL_MAX (2 * fabs (dQ / x), phi);

    {
      double step0 = lambda;
      double step1 = -((a - 1) / x - 1) * lambda * lambda / 4.0;

      double step = step0;
      if (fabs (step1) < fabs (step0))
        step += step1;

      if (x + step > 0)
        x += step;
      else
        {
          x /= 2.0;
        }

      if (fabs (step0) > 1e-10 * x)
        goto start;
    }

  }

end:
  return b * x;
}
