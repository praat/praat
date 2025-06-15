/* cdf/tdistinv.c
 *
 * Copyright (C) 2007 Brian Gough
 * Copyright (C) 2002 Jason H. Stover.
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

static double
inv_cornish_fisher (double z, double nu)
{
  double a = 1 / (nu - 0.5);
  double b = 48.0 / (a * a);

  double cf1 = z * (3 + z * z);
  double cf2 = z * (945 + z * z * (360 + z * z * (63 + z * z * 4)));

  double y = z - cf1 / b + cf2 / (10 * b * b);

  double t = GSL_SIGN (z) * sqrt (nu * expm1 (a * y * y));

  return t;
}


double
gsl_cdf_tdist_Pinv (const double P, const double nu)
{
  double x, ptail;

  if (P == 1.0)
    {
      return GSL_POSINF;
    }
  else if (P == 0.0)
    {
      return GSL_NEGINF;
    }

  if (nu == 1.0)
    {
      x = tan (M_PI * (P - 0.5));
    }
  else if (nu == 2.0)
    {
      double a = 2 * P - 1;
      x = a / sqrt (2 * (1 - a * a));
    }

  ptail = (P < 0.5) ? P : 1 - P;

  if (sqrt (M_PI * nu / 2) * ptail > pow (0.05, nu / 2))
    {
      double xg = gsl_cdf_ugaussian_Pinv (P);
      x = inv_cornish_fisher (xg, nu);
    }
  else
    {
      /* Use an asymptotic expansion of the tail of integral */

      double beta = gsl_sf_beta (0.5, nu / 2);

      if (P < 0.5)
        {
          x = -sqrt (nu) * pow (beta * nu * P, -1.0 / nu);
        }
      else
        {
          x = sqrt (nu) * pow (beta * nu * (1 - P), -1.0 / nu);
        }

      /* Correct nu -> nu/(1+nu/x^2) in the leading term to account
         for higher order terms. This avoids overestimating x, which
         makes the iteration unstable due to the rapidly decreasing
         tails of the distribution. */

      x /= sqrt (1 + nu / (x * x));
    }

  {
    double dP, phi;
    unsigned int n = 0;

  start:
    dP = P - gsl_cdf_tdist_P (x, nu);
    phi = gsl_ran_tdist_pdf (x, nu);

    if (dP == 0.0 || n++ > 32)
      goto end;

    {
      double lambda = dP / phi;
      double step0 = lambda;
      double step1 = ((nu + 1) * x / (x * x + nu)) * (lambda * lambda / 4.0);

      double step = step0;

      if (fabs (step1) < fabs (step0))
        {
          step += step1;
        }

      if (P > 0.5 && x + step < 0)
        x /= 2;
      else if (P < 0.5 && x + step > 0)
        x /= 2;
      else
        x += step;

      if (fabs (step) > 1e-10 * fabs (x))
        goto start;
    }
    
  end:
    if (fabs(dP) > GSL_SQRT_DBL_EPSILON * P)
      {
        GSL_ERROR_VAL("inverse failed to converge", GSL_EFAILED, GSL_NAN);
      }
    
    return x;
  }
}

double
gsl_cdf_tdist_Qinv (const double Q, const double nu)
{
  double x, qtail;

  if (Q == 0.0)
    {
      return GSL_POSINF;
    }
  else if (Q == 1.0)
    {
      return GSL_NEGINF;
    }

  if (nu == 1.0)
    {
      x = tan (M_PI * (0.5 - Q));
    }
  else if (nu == 2.0)
    {
      double a = 2 * (1 - Q) - 1;
      x = a / sqrt (2 * (1 - a * a));
    }

  qtail = (Q < 0.5) ? Q : 1 - Q;

  if (sqrt (M_PI * nu / 2) * qtail > pow (0.05, nu / 2))
    {
      double xg = gsl_cdf_ugaussian_Qinv (Q);
      x = inv_cornish_fisher (xg, nu);
    }
  else
    {
      /* Use an asymptotic expansion of the tail of integral */

      double beta = gsl_sf_beta (0.5, nu / 2);

      if (Q < 0.5)
        {
          x = sqrt (nu) * pow (beta * nu * Q, -1.0 / nu);
        }
      else
        {
          x = -sqrt (nu) * pow (beta * nu * (1 - Q), -1.0 / nu);
        }

      /* Correct nu -> nu/(1+nu/x^2) in the leading term to account
         for higher order terms. This avoids overestimating x, which
         makes the iteration unstable due to the rapidly decreasing
         tails of the distribution. */

      x /= sqrt (1 + nu / (x * x));
    }

  {
    double dQ, phi;
    unsigned int n = 0;

  start:
    dQ = Q - gsl_cdf_tdist_Q (x, nu);
    phi = gsl_ran_tdist_pdf (x, nu);

    if (dQ == 0.0 || n++ > 32)
      goto end;

    {
      double lambda = - dQ / phi;
      double step0 = lambda;
      double step1 = ((nu + 1) * x / (x * x + nu)) * (lambda * lambda / 4.0);

      double step = step0;

      if (fabs (step1) < fabs (step0))
        {
          step += step1;
        }

      if (Q < 0.5 && x + step < 0)
        x /= 2;
      else if (Q > 0.5 && x + step > 0)
        x /= 2;
      else
        x += step;

      if (fabs (step) > 1e-10 * fabs (x))
        goto start;
    }
  }

end:

  return x;
}
