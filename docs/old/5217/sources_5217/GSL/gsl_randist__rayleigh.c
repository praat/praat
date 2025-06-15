/* randist/rayleigh.c
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
#include "gsl_rng.h"
#include "gsl_randist.h"

/* The Rayleigh distribution has the form

   p(x) dx = (x / sigma^2) exp(-x^2/(2 sigma^2)) dx

   for x = 0 ... +infty */

double
gsl_ran_rayleigh (const gsl_rng * r, const double sigma)
{
  double u = gsl_rng_uniform_pos (r);

  return sigma * sqrt(-2.0 * log (u));
}

double
gsl_ran_rayleigh_pdf (const double x, const double sigma)
{
  if (x < 0)
    {
      return 0 ;
    }
  else
    {
      double u = x / sigma ;
      double p = (u / sigma) * exp(-u * u / 2.0) ;
      
      return p;
    }
}

/* The Rayleigh tail distribution has the form

   p(x) dx = (x / sigma^2) exp((a^2 - x^2)/(2 sigma^2)) dx

   for x = a ... +infty */

double
gsl_ran_rayleigh_tail (const gsl_rng * r, const double a, const double sigma)
{
  double u = gsl_rng_uniform_pos (r);

  return sqrt(a * a - 2.0 * sigma * sigma * log (u));
}

double
gsl_ran_rayleigh_tail_pdf (const double x, const double a, const double sigma)
{
  if (x < a)
    {
      return 0 ;
    }
  else
    {
      double u = x / sigma ;
      double v = a / sigma ;

      double p = (u / sigma) * exp((v + u) * (v - u) / 2.0) ;
      
      return p;
    }
}
