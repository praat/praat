/* cdf/inverse_normal.c
 *
 * Copyright (C) 2002 Przemyslaw Sliwa and Jason H. Stover.
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

/*
 * Computes the inverse normal cumulative distribution function 
 * according to the algorithm shown in 
 *
 *      Wichura, M.J. (1988).
 *      Algorithm AS 241: The Percentage Points of the Normal Distribution.
 *      Applied Statistics, 37, 477-484.
 */

#include "gsl__config.h"
#include "gsl_errno.h"
#include "gsl_math.h"
#include "gsl_cdf.h"

#include "gsl_cdf__rat_eval.h"

static double
small (double q)
{
  const double a[8] = { 3.387132872796366608, 133.14166789178437745,
    1971.5909503065514427, 13731.693765509461125,
    45921.953931549871457, 67265.770927008700853,
    33430.575583588128105, 2509.0809287301226727
  };

  const double b[8] = { 1.0, 42.313330701600911252,
    687.1870074920579083, 5394.1960214247511077,
    21213.794301586595867, 39307.89580009271061,
    28729.085735721942674, 5226.495278852854561
  };

  double r = 0.180625 - q * q;

  double x = q * rat_eval (a, 8, b, 8, r);

  return x;
}

static double
intermediate (double r)
{
  const double a[] = { 1.42343711074968357734, 4.6303378461565452959,
    5.7694972214606914055, 3.64784832476320460504,
    1.27045825245236838258, 0.24178072517745061177,
    0.0227238449892691845833, 7.7454501427834140764e-4
  };

  const double b[] = { 1.0, 2.05319162663775882187,
    1.6763848301838038494, 0.68976733498510000455,
    0.14810397642748007459, 0.0151986665636164571966,
    5.475938084995344946e-4, 1.05075007164441684324e-9
  };

  double x = rat_eval (a, 8, b, 8, (r - 1.6));

  return x;
}

static double
tail (double r)
{
  const double a[] = { 6.6579046435011037772, 5.4637849111641143699,
    1.7848265399172913358, 0.29656057182850489123,
    0.026532189526576123093, 0.0012426609473880784386,
    2.71155556874348757815e-5, 2.01033439929228813265e-7
  };

  const double b[] = { 1.0, 0.59983220655588793769,
    0.13692988092273580531, 0.0148753612908506148525,
    7.868691311456132591e-4, 1.8463183175100546818e-5,
    1.4215117583164458887e-7, 2.04426310338993978564e-15
  };

  double x = rat_eval (a, 8, b, 8, (r - 5.0));

  return x;
}

double
gsl_cdf_ugaussian_Pinv (const double P)
{
  double r, x, pp;

  double dP = P - 0.5;

  if (P == 1.0)
    {
      return GSL_POSINF;
    }
  else if (P == 0.0)
    {
      return GSL_NEGINF;
    }

  if (fabs (dP) <= 0.425)
    {
      x = small (dP);

      return x;
    }

  pp = (P < 0.5) ? P : 1.0 - P;

  r = sqrt (-log (pp));

  if (r <= 5.0)
    {
      x = intermediate (r);
    }
  else
    {
      x = tail (r);
    }

  if (P < 0.5)
    {
      return -x;
    }
  else
    {
      return x;
    }

}

double
gsl_cdf_ugaussian_Qinv (const double Q)
{
  double r, x, pp;

  double dQ = Q - 0.5;

  if (Q == 1.0)
    {
      return GSL_NEGINF;
    }
  else if (Q == 0.0)
    {
      return GSL_POSINF;
    }

  if (fabs (dQ) <= 0.425)
    {
      x = small (dQ);

      return -x;
    }

  pp = (Q < 0.5) ? Q : 1.0 - Q;

  r = sqrt (-log (pp));

  if (r <= 5.0)
    {
      x = intermediate (r);
    }
  else
    {
      x = tail (r);
    }

  if (Q < 0.5)
    {
      return x;
    }
  else
    {
      return -x;
    }
}


double
gsl_cdf_gaussian_Pinv (const double P, const double sigma)
{
  return sigma * gsl_cdf_ugaussian_Pinv (P);
}

double
gsl_cdf_gaussian_Qinv (const double Q, const double sigma)
{
  return sigma * gsl_cdf_ugaussian_Qinv (Q);
}
