/* randist/chisq.c
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

/* The chisq distribution has the form

   p(x) dx = (1/(2*Gamma(nu/2))) (x/2)^(nu/2 - 1) exp(-x/2) dx

   for x = 0 ... +infty */

double
gsl_ran_chisq (const gsl_rng * r, const double nu)
{
  double chisq = 2 * gsl_ran_gamma (r, nu / 2, 1.0);
  return chisq;
}

double
gsl_ran_chisq_pdf (const double x, const double nu)
{
  if (x <= 0)
    {
      return 0 ;
    }
  else
    {
      double p;
      double lngamma = gsl_sf_lngamma (nu / 2);
      
      p = exp ((nu / 2 - 1) * log (x/2) - x/2 - lngamma) / 2;
      return p;
    }
}
