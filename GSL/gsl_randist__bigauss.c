/* randist/bigauss.c
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

/* The Bivariate Gaussian probability distribution is 

   p(x,y) dxdy = (1/(2 pi sigma_x sigma_y sqrt(c))) 
    exp(-((x/sigma_x)^2 + (y/sigma_y)^2 - 2 r (x/sigma_x)(y/sigma_y))/2c) dxdy 

   where c = 1-r^2
*/

void
gsl_ran_bivariate_gaussian (const gsl_rng * r, 
                            double sigma_x, double sigma_y, double rho,
                            double *x, double *y)
{
  double u, v, r2, scale;

  do
    {
      /* choose x,y in uniform square (-1,-1) to (+1,+1) */

      u = -1 + 2 * gsl_rng_uniform (r);
      v = -1 + 2 * gsl_rng_uniform (r);

      /* see if it is in the unit circle */
      r2 = u * u + v * v;
    }
  while (r2 > 1.0 || r2 == 0);

  scale = sqrt (-2.0 * log (r2) / r2);

  *x = sigma_x * u * scale;
  *y = sigma_y * (rho * u + sqrt(1 - rho*rho) * v) * scale;
}

double
gsl_ran_bivariate_gaussian_pdf (const double x, const double y, 
                                const double sigma_x, const double sigma_y,
                                const double rho)
{
  double u = x / sigma_x ;
  double v = y / sigma_y ;
  double c = 1 - rho*rho ;
  double p = (1 / (2 * M_PI * sigma_x * sigma_y * sqrt(c))) 
    * exp (-(u * u - 2 * rho * u * v + v * v) / (2 * c));
  return p;
}
