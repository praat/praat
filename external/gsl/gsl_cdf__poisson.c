/* cdf/poisson.c
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

/*
 * Computes the cumulative distribution function for a Poisson
 * random variable. For a Poisson random variable X with parameter
 * mu,
 *
 *          Pr( X <= k ) = Pr( Y >= p )
 *
 * where Y is a gamma random variable with parameters k+1 and 1.
 *
 * Reference: 
 * 
 * W. Feller, "An Introduction to Probability and Its
 * Applications," volume 1. Wiley, 1968. Exercise 46, page 173,
 * chapter 6.
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_cdf.h"

#include "gsl_cdf__error.h"

/*
 * Pr (X <= k) for a Poisson random variable X.
 */

double
gsl_cdf_poisson_P (const unsigned int k, const double mu)
{
  double P;
  double a;

  if (mu <= 0.0)
    {
      CDF_ERROR ("mu <= 0", GSL_EDOM);
    }

  a = (double) k + 1.0;
  P = gsl_cdf_gamma_Q (mu, a, 1.0);

  return P;
}

/*
 * Pr ( X > k ) for a Possion random variable X.
 */

double
gsl_cdf_poisson_Q (const unsigned int k, const double mu)
{
  double Q;
  double a;

  if (mu <= 0.0)
    {
      CDF_ERROR ("mu <= 0", GSL_EDOM);
    }

  a = (double) k + 1.0;
  Q = gsl_cdf_gamma_P (mu, a, 1.0);

  return Q;
}
