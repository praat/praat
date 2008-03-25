/* randist/tdist.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
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
#include "gsl_math.h"
#include "gsl_sf_gamma.h"
#include "gsl_rng.h"
#include "gsl_randist.h"

/* The t-distribution has the form

   p(x) dx = (Gamma((nu + 1)/2)/(sqrt(pi nu) Gamma(nu/2))
   * (1 + (x^2)/nu)^-((nu + 1)/2) dx

   The method used here is the one described in Knuth */

double
gsl_ran_tdist (const gsl_rng * r, const double nu)
{
  if (nu <= 2)
    {
      double Y1 = gsl_ran_ugaussian (r);
      double Y2 = gsl_ran_chisq (r, nu);

      double t = Y1 / sqrt (Y2 / nu);

      return t;
    }
  else
    {
      double Y1, Y2, Z, t;
      do
        {
          Y1 = gsl_ran_ugaussian (r);
          Y2 = gsl_ran_exponential (r, 1 / (nu/2 - 1));

          Z = Y1 * Y1 / (nu - 2);
        }
      while (1 - Z < 0 || exp (-Y2 - Z) > (1 - Z));

      /* Note that there is a typo in Knuth's formula, the line below
         is taken from the original paper of Marsaglia, Mathematics of
         Computation, 34 (1980), p 234-256 */

      t = Y1 / sqrt ((1 - 2 / nu) * (1 - Z));
      return t;
    }
}

double
gsl_ran_tdist_pdf (const double x, const double nu)
{
  double p;

  double lg1 = gsl_sf_lngamma (nu / 2);
  double lg2 = gsl_sf_lngamma ((nu + 1) / 2);

  p = ((exp (lg2 - lg1) / sqrt (M_PI * nu)) 
       * pow ((1 + x * x / nu), -(nu + 1) / 2));
  return p;
}


