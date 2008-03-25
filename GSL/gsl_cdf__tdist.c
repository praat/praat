/* cdf/tdist.c
 *
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

/*
 * Computes the Student's t cumulative distribution function using
 * the method detailed in 
 * 
 * W.J. Kennedy and J.E. Gentle, "Statistical Computing." 1980. 
 * Marcel Dekker. ISBN 0-8247-6898-1.
 *
 * G.W. Hill and A.W. Davis. "Generalized asymptotic expansions
 * of Cornish-Fisher type." Annals of Mathematical Statistics, 
 * vol. 39, 1264-1273. 1968.
 *
 * G.W. Hill. "Algorithm 395: Student's t-distribution," Communications
 * of the ACM, volume 13, number 10, page 617. October 1970.
 *
 * G.W. Hill, "Remark on algorithm 395: Student's t-distribution,"
 * Transactions on Mathematical Software, volume 7, number 2, page
 * 247. June 1982.
 */

#include "gsl__config.h"
#include "gsl_cdf.h"
#include "gsl_sf_gamma.h"
#include "gsl_math.h"
#include "gsl_errno.h"

#include "gsl_cdf__beta_inc.c"

static double
poly_eval (const double c[], unsigned int n, double x)
{
  unsigned int i;
  double y = c[0] * x;

  for (i = 1; i < n; i++)
    {
      y = x * (y + c[i]);
    }

  y += c[n];

  return y;
}

/* 
 * Use the Cornish-Fisher asymptotic expansion to find a point u such
 * that gsl_cdf_gauss(y) = tcdf(t).
 * 
 */

static double
cornish_fisher (double t, double n)
{
  const double coeffs6[10] = {
    0.265974025974025974026,
    5.449696969696969696970,
    122.20294372294372294372,
    2354.7298701298701298701,
    37625.00902597402597403,
    486996.1392857142857143,
    4960870.65,
    37978595.55,
    201505390.875,
    622437908.625
  };
  const double coeffs5[8] = {
    0.2742857142857142857142,
    4.499047619047619047619,
    78.45142857142857142857,
    1118.710714285714285714,
    12387.6,
    101024.55,
    559494.0,
    1764959.625
  };
  const double coeffs4[6] = {
    0.3047619047619047619048,
    3.752380952380952380952,
    46.67142857142857142857,
    427.5,
    2587.5,
    8518.5
  };
  const double coeffs3[4] = {
    0.4,
    3.3,
    24.0,
    85.5
  };

  double a = n - 0.5;
  double b = 48.0 * a * a;

  double z2 = a * log1p (t * t / n);
  double z = sqrt (z2);

  double p5 = z * poly_eval (coeffs6, 9, z2);
  double p4 = -z * poly_eval (coeffs5, 7, z2);
  double p3 = z * poly_eval (coeffs4, 5, z2);
  double p2 = -z * poly_eval (coeffs3, 3, z2);
  double p1 = z * (z2 + 3.0);
  double p0 = z;

  double y = p5;
  y = (y / b) + p4;
  y = (y / b) + p3;
  y = (y / b) + p2;
  y = (y / b) + p1;
  y = (y / b) + p0;

  if (t < 0)
    y *= -1;

  return y;
}

#if 0
/*
 * Series approximation for t > 4.0. This needs to be fixed;
 * it shouldn't subtract the result from 1.0. A better way is
 * to use two different series expansions. Figuring this out
 * means rummaging through Fisher's paper in Metron, v5, 1926, 
 * "Expansion of Student's integral in powers of n^{-1}."
 */

#define MAXI 40

static double
normal_approx (const double x, const double nu)
{
  double y;
  double num;
  double diff;
  double q;
  int i;
  double lg1, lg2;

  y = 1 / sqrt (1 + x * x / nu);
  num = 1.0;
  q = 0.0;
  diff = 2 * GSL_DBL_EPSILON;
  for (i = 2; (i < MAXI) && (diff > GSL_DBL_EPSILON); i += 2)
    {
      diff = q;
      num *= y * y * (i - 1) / i;
      q += num / (nu + i);
      diff = q - diff;
    }
  q += 1 / nu;
  lg1 = gsl_sf_lngamma (nu / 2.0);
  lg2 = gsl_sf_lngamma ((nu + 1.0) / 2.0);

  diff = lg2 - lg1;
  q *= pow (y, nu) * exp (diff) / sqrt (M_PI);

  return q;
}
#endif

double
gsl_cdf_tdist_P (const double x, const double nu)
{
  double P;

  double x2 = x * x;

  if (nu > 30 && x2 < 10 * nu)
    {
      double u = cornish_fisher (x, nu);
      P = gsl_cdf_ugaussian_P (u);

      return P;
    }

  if (x2 < nu)
    {
      double u = x2 / nu;
      double eps = u / (1 + u);

      if (x >= 0)
        {
          P = beta_inc_AXPY (0.5, 0.5, 0.5, nu / 2.0, eps);
        }
      else
        {
          P = beta_inc_AXPY (-0.5, 0.5, 0.5, nu / 2.0, eps);
        }
    }
  else
    {
      double v = nu / (x * x);
      double eps = v / (1 + v);

      if (x >= 0)
        {
          P = beta_inc_AXPY (-0.5, 1.0, nu / 2.0, 0.5, eps);
        }
      else
        {
          P = beta_inc_AXPY (0.5, 0.0, nu / 2.0, 0.5, eps);
        }
    }

  return P;
}


double
gsl_cdf_tdist_Q (const double x, const double nu)
{
  double Q;

  double x2 = x * x;

  if (nu > 30 && x2 < 10 * nu)
    {
      double u = cornish_fisher (x, nu);
      Q = gsl_cdf_ugaussian_Q (u);

      return Q;
    }

  if (x2 < nu)
    {
      double u = x2 / nu;
      double eps = u / (1 + u);

      if (x >= 0)
        {
          Q = beta_inc_AXPY (-0.5, 0.5, 0.5, nu / 2.0, eps);
        }
      else
        {
          Q = beta_inc_AXPY (0.5, 0.5, 0.5, nu / 2.0, eps);
        }
    }
  else
    {
      double v = nu / (x * x);
      double eps = v / (1 + v);

      if (x >= 0)
        {
          Q = beta_inc_AXPY (0.5, 0.0, nu / 2.0, 0.5, eps);
        }
      else
        {
          Q = beta_inc_AXPY (-0.5, 1.0, nu / 2.0, 0.5, eps);
        }
    }

  return Q;
}
