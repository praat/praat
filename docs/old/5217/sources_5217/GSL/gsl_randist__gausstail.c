/* randist/gausstail.c
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
#include "gsl_rng.h"
#include "gsl_randist.h"
#include "gsl_sf_erf.h"

double
gsl_ran_gaussian_tail (const gsl_rng * r, const double a, const double sigma)
{
  /* Returns a gaussian random variable larger than a
   * This implementation does one-sided upper-tailed deviates.
   */

  double s = a / sigma;

  if (s < 1)
    {
      /* For small s, use a direct rejection method. The limit s < 1
         can be adjusted to optimise the overall efficiency */

      double x;

      do
        {
          x = gsl_ran_gaussian (r, 1.0);
        }
      while (x < s);
      return x * sigma;
    }
  else
    {
      /* Use the "supertail" deviates from the last two steps
       * of Marsaglia's rectangle-wedge-tail method, as described
       * in Knuth, v2, 3rd ed, pp 123-128.  (See also exercise 11, p139,
       * and the solution, p586.)
       */

      double u, v, x;

      do
        {
          u = gsl_rng_uniform (r);
          do
            {
              v = gsl_rng_uniform (r);
            }
          while (v == 0.0);
          x = sqrt (s * s - 2 * log (v));
        }
      while (x * u > s);
      return x * sigma;
    }
}

double
gsl_ran_gaussian_tail_pdf (const double x, const double a, const double sigma)
{
  if (x < a)
    {
      return 0;
    }
  else
    {
      double N, p;
      double u = x / sigma ;

      double f = gsl_sf_erfc (a / (sqrt (2.0) * sigma));

      N = 0.5 * f;

      p = (1 / (N * sqrt (2 * M_PI) * sigma)) * exp (-u * u / 2);

      return p;
    }
}

double
gsl_ran_ugaussian_tail (const gsl_rng * r, const double a)
{
  return gsl_ran_gaussian_tail (r, a, 1.0) ;
}

double
gsl_ran_ugaussian_tail_pdf (const double x, const double a)
{
  return gsl_ran_gaussian_tail_pdf (x, a, 1.0) ;
}
