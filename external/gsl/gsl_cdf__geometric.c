/* cdf/geometric.c
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

/* Pr (X <= k), i.e., the probability of n or fewer failures until the
   first success. */

double
gsl_cdf_geometric_P (const unsigned int k, const double p)
{
  double P, a, q;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (k < 1)
    {
      return 0.0;
    }

  q = 1.0 - p;
  a = (double) k;

  if (p < 0.5)
    {
      P = -expm1 (a * log1p (-p));
    }
  else
    {
      P = 1.0 - pow (q, a);
    }

  return P;
}

double
gsl_cdf_geometric_Q (const unsigned int k, const double p)
{
  double Q, a, q;

  if (p > 1.0 || p < 0.0)
    {
      CDF_ERROR ("p < 0 or p > 1", GSL_EDOM);
    }

  if (k < 1)
    {
      Q = 1.0;
    }

  q = 1.0 - p;
  a = (double) k;

  if (p < 0.5)
    {
      Q = exp (a * log1p (-p));
    }
  else
    {
      Q = pow (q, a);
    }

  return Q;
}
