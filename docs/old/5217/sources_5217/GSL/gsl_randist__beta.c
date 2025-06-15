/* randist/beta.c
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
#include "gsl_sf_gamma.h"

/* The beta distribution has the form

   p(x) dx = (Gamma(a + b)/(Gamma(a) Gamma(b))) x^(a-1) (1-x)^(b-1) dx

   The method used here is the one described in Knuth */

double
gsl_ran_beta (const gsl_rng * r, const double a, const double b)
{
  double x1 = gsl_ran_gamma (r, a, 1.0);
  double x2 = gsl_ran_gamma (r, b, 1.0);

  return x1 / (x1 + x2);
}

double
gsl_ran_beta_pdf (const double x, const double a, const double b)
{
  if (x < 0 || x > 1)
    {
      return 0 ;
    }
  else 
    {
      double p;

      double gab = gsl_sf_lngamma (a + b);
      double ga = gsl_sf_lngamma (a);
      double gb = gsl_sf_lngamma (b);
      
      if (x == 0.0 || x == 1.0) 
        {
          p = exp (gab - ga - gb) * pow (x, a - 1) * pow (1 - x, b - 1);
        }
      else
        {
          p = exp (gab - ga - gb + log(x) * (a - 1)  + log1p(-x) * (b - 1));
        }

      return p;
    }
}
