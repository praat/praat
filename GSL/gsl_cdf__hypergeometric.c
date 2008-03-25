/* cdf/hypergeometric.c
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
 * Computes the cumulative distribution function for a hypergeometric
 * random variable. A hypergeometric random variable X is the number
 * of elements of type 1 in a sample of size t, drawn from a population
 * of size n1 + n2, in which n1 are of type 1 and n2 are of type 2.
 *
 * This algorithm computes Pr( X <= k ) by summing the terms from
 * the mass function, Pr( X = k ).
 *
 * References:
 *
 * T. Wu. An accurate computation of the hypergeometric distribution 
 * function. ACM Transactions on Mathematical Software. Volume 19, number 1,
 * March 1993.
 *  This algorithm is not used, since it requires factoring the
 *  numerator and denominator, then cancelling. It is more accurate
 *  than the algorithm used here, but the cancellation requires more
 *  time than the algorithm used here.
 *
 * W. Feller. An Introduction to Probability Theory and Its Applications,
 * third edition. 1968. Chapter 2, section 6. 
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_cdf.h"
#include "gsl_randist.h"

#include "gsl_cdf__error.h"

static double
lower_tail (const unsigned int k, const unsigned int n1,
            const unsigned int n2, const unsigned int t)
{
  double relerr;
  int i = k;
  double s, P;

  s = gsl_ran_hypergeometric_pdf (i, n1, n2, t);
  P = s;
  
  while (i > 0)
    {
      double factor =
        (i / (n1 - i + 1.0)) * ((n2 + i - t) / (t - i + 1.0));
      s *= factor;
      P += s;
      relerr = s / P;
      if (relerr < GSL_DBL_EPSILON)
        break;
      i--;
    }

  return P;
}
  
static double 
upper_tail (const unsigned int k, const unsigned int n1,
            const unsigned int n2, const unsigned int t)
{
  double relerr;
  unsigned int i = k + 1;
  double s, Q;
  
  s = gsl_ran_hypergeometric_pdf (i, n1, n2, t);
  Q = s;
  
  while (i < t)
    {
      double factor =
        ((n1 - i) / (i + 1.0)) * ((t - i) / (n2 + i + 1.0 - t));
      s *= factor;
      Q += s;
      relerr = s / Q;
      if (relerr < GSL_DBL_EPSILON)
        break;
      i++;
    }

  return Q;
}




/*
 * Pr (X <= k)
 */
double
gsl_cdf_hypergeometric_P (const unsigned int k,
                          const unsigned int n1,
                          const unsigned int n2, const unsigned int t)
{
  double P;

  if (t > (n1 + n2))
    {
      CDF_ERROR ("t larger than population size", GSL_EDOM);
    }
  else if (k >= n1 || k >= t)
    {
      P = 1.0;
    }
  else if (k < 0.0)
    {
      P = 0.0;
    }
  else
    {
      double midpoint = (int) (t * n1 / (n1 + n2));

      if (k >= midpoint)
        {
          P = 1 - upper_tail (k, n1, n2, t);
        }
      else
        {
          P = lower_tail (k, n1, n2, t);
        }
    }

  return P;
}

/*
 * Pr (X > k)
 */
double
gsl_cdf_hypergeometric_Q (const unsigned int k,
                          const unsigned int n1,
                          const unsigned int n2, const unsigned int t)
{
  double Q;

  if (t > (n1 + n2))
    {
      CDF_ERROR ("t larger than population size", GSL_EDOM);
    }
  else if (k >= n1 || k >= t)
    {
      Q = 0.0;
    }
  else if (k < 0.0)
    {
      Q = 1.0;
    }
  else
    {
      double midpoint = (int) (t * n1 / (n1 + n2));

      if (k < midpoint)
        {
          Q = 1 - lower_tail (k, n1, n2, t);
        }
      else
        {
          Q = upper_tail (k, n1, n2, t);
        }
    }

  return Q;
}
