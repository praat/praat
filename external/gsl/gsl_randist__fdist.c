/* randist/fdist.c
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
#include "gsl_sf_gamma.h"
#include "gsl_rng.h"
#include "gsl_randist.h"

/* The F distribution has the form

   p(x) dx = (nu1^(nu1/2) nu2^(nu2/2) Gamma((nu1 + nu2)/2) /
   Gamma(nu1/2) Gamma(nu2/2)) *
   x^(nu1/2 - 1) (nu2 + nu1 * x)^(-nu1/2 -nu2/2) dx

   The method used here is the one described in Knuth */

double
gsl_ran_fdist (const gsl_rng * r, const double nu1, const double nu2)
{

  double Y1 =  gsl_ran_gamma (r, nu1 / 2, 2.0);
  double Y2 =  gsl_ran_gamma (r, nu2 / 2, 2.0);

  double f = (Y1 * nu2) / (Y2 * nu1);

  return f;
}

double
gsl_ran_fdist_pdf (const double x, const double nu1, const double nu2)
{
  if (x < 0)
    {
      return 0 ;
    }
  else
    {
      double p;
      double lglg = (nu1 / 2) * log (nu1) + (nu2 / 2) * log (nu2) ;

      double lg12 = gsl_sf_lngamma ((nu1 + nu2) / 2);
      double lg1 = gsl_sf_lngamma (nu1 / 2);
      double lg2 = gsl_sf_lngamma (nu2 / 2);
      
      p = exp (lglg + lg12 - lg1 - lg2)
        * pow (x, nu1 / 2 - 1) * pow (nu2 + nu1 * x, -nu1 / 2 - nu2 / 2);
      
      return p;
    }
}
