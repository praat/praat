/* cdf/negbinom.c
 *
 * Copyright (C) 2004 Jason H. Stover.
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
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_cdf.h"

#include "gsl_cdf__error.h"

/*
 * Pr(X <= k) for a negative binomial random variable X, i.e.,
 * the probability of k or fewer failuers before success n.
 */

double
gsl_cdf_negative_binomial_P (const unsigned int k, const double p, const double n)
{
  double P;
  double a;
  double b;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (n < 0)
    {
      CDF_ERROR ("n < 0", GSL_EDOM);
    }

  a = (double) n;
  b = (double) k + 1.0;
  P = gsl_cdf_beta_P (p, a, b);

  return P;
}

/*
 * Pr ( X > k ).
 */

double
gsl_cdf_negative_binomial_Q (const unsigned int k, const double p, const double n)
{
  double Q;
  double a;
  double b;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (n < 0)
    {
      CDF_ERROR ("n < 0", GSL_EDOM);
    }

  a = (double) n;
  b = (double) k + 1.0;
  Q = gsl_cdf_beta_Q (p, a, b);

  return Q;
}
