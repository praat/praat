/* cdf/binomial.c
 * 
 * Copyright (C) 2004 Jason H. Stover.
 * Copyright (C) 2004 Giulio Bottazzi
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_math.h"
#include "gsl_cdf.h"
#include "gsl_sf_gamma.h"

#include "gsl_cdf__error.h"

/* Computes the cumulative distribution function for a binomial
   random variable. For a binomial random variable X with n trials
   and success probability p,
   
           Pr( X <= k ) = Pr( Y >= p )
 
   where Y is a beta random variable with parameters k+1 and n-k.
 
   The binomial distribution has the form,

   prob(k) =  n!/(k!(n-k)!) *  p^k (1-p)^(n-k) for k = 0, 1, ..., n

   The cumulated distributions can be expressed in terms of normalized
   incomplete beta functions (see Abramowitz & Stegun eq. 26.5.26 and
   eq. 6.6.3).

   Reference: 
  
   W. Feller, "An Introduction to Probability and Its
   Applications," volume 1. Wiley, 1968. Exercise 45, page 173,
   chapter 6.
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_cdf.h"

double
gsl_cdf_binomial_P (const unsigned int k, const double p, const unsigned int n)
{
  double P;
  double a;
  double b;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (k >= n)
    {
      P = 1.0;
    }
  else
    {
      a = (double) k + 1.0;
      b = (double) n - k;
      P = gsl_cdf_beta_Q (p, a, b);
    }

  return P;
}

double
gsl_cdf_binomial_Q (const unsigned int k, const double p, const unsigned int n)
{
  double Q;
  double a;
  double b;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (k >= n)
    {
      Q = 0.0;
    }
  else
    {
      a = (double) k + 1.0;
      b = (double) n - k;
      Q = gsl_cdf_beta_P (p, a, b);
    }

  return Q;
}
