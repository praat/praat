/* randist/exppow.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 James Theiler, Brian Gough
 * Copyright (C) 2006 Giulio Bottazzi
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

/* The exponential power probability distribution is  

   p(x) dx = (1/(2 a Gamma(1+1/b))) * exp(-|x/a|^b) dx

   for -infty < x < infty. For b = 1 it reduces to the Laplace
   distribution. 

   The exponential power distribution is related to the gamma
   distribution by E = a * pow(G(1/b),1/b), where E is an exponential
   power variate and G is a gamma variate.

   We use this relation for b < 1. For b >=1 we use rejection methods
   based on the laplace and gaussian distributions which should be
   faster.  For b>4 we revert to the gamma method.

   See P. R. Tadikamalla, "Random Sampling from the Exponential Power
   Distribution", Journal of the American Statistical Association,
   September 1980, Volume 75, Number 371, pages 683-686.
   
*/

double
gsl_ran_exppow (const gsl_rng * r, const double a, const double b)
{
  if (b < 1 || b > 4)
    {
      double u = gsl_rng_uniform (r);
      double v = gsl_ran_gamma (r, 1 / b, 1.0);
      double z = a * pow (v, 1 / b);

      if (u > 0.5)
        {
          return z;
        }
      else
        {
          return -z;
        }
    }
  else if (b == 1)
    {
      /* Laplace distribution */
      return gsl_ran_laplace (r, a);
    }
  else if (b < 2)
    {
      /* Use laplace distribution for rejection method, from Tadikamalla */

      double x, h, u;

      double B = pow (1 / b, 1 / b);

      do
        {
          x = gsl_ran_laplace (r, B);
          u = gsl_rng_uniform_pos (r);
          h = -pow (fabs (x), b) + fabs (x) / B - 1 + (1 / b);
        }
      while (log (u) > h);

      return a * x;
    }
  else if (b == 2)
    {
      /* Gaussian distribution */
      return gsl_ran_gaussian (r, a / sqrt (2.0));
    }
  else
    {
      /* Use gaussian for rejection method, from Tadikamalla */

      double x, h, u;

      double B = pow (1 / b, 1 / b);

      do
        {
          x = gsl_ran_gaussian (r, B);
          u = gsl_rng_uniform_pos (r);
          h = -pow (fabs (x), b) + (x * x) / (2 * B * B) + (1 / b) - 0.5;
        }
      while (log (u) > h);

      return a * x;
    }
}

double
gsl_ran_exppow_pdf (const double x, const double a, const double b)
{
  double p;
  double lngamma = gsl_sf_lngamma (1 + 1 / b);
  p = (1 / (2 * a)) * exp (-pow (fabs (x / a), b) - lngamma);
  return p;
}
