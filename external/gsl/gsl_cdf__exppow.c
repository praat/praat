/* cdf/exppow.c
 * 
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_cdf.h"
#include "gsl_sf_gamma.h"

/* The exponential power density is parametrized according to

   p(x) dx = (1/(2 a Gamma(1 + 1/b))) * exp(-|x/a|^b) dx

   so that the distribution reads

           / x<0   0.5 - Gamma_inc_P(1/b,|x/a|^b)
   P(x) = |  x=0   0.5
           \ x>0   0.5 + Gamma_inc_P(1/b,|x/a|^b)


   for x in (-infty,+infty) */

double
gsl_cdf_exppow_P (const double x, const double a, const double b)
{
  const double u = x / a;

  if (u < 0)
    {
      double P = 0.5 * gsl_sf_gamma_inc_Q (1.0 / b, pow (-u, b));
      return P;
    }
  else
    {
      double P = 0.5 * (1.0 + gsl_sf_gamma_inc_P (1.0 / b, pow (u, b)));
      return P;
    }
}

double
gsl_cdf_exppow_Q (const double x, const double a, const double b)
{
  const double u = x / a;

  if (u < 0)
    {
      double Q = 0.5 * (1.0 + gsl_sf_gamma_inc_P (1.0 / b, pow (-u, b)));
      return Q;
    }
  else
    {
      double Q = 0.5 * gsl_sf_gamma_inc_Q (1.0 / b, pow (u, b));
      return Q;
    }
}
