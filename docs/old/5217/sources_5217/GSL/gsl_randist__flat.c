/* randist/flat.c
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

/* This is the uniform distribution in the range [a, b)

   p(x) dx = 1/(b-a) dx   if  a <= x < b
   .....   = 0            otherwise 

 */

double
gsl_ran_flat (const gsl_rng * r, const double a, const double b)
{
  double u = gsl_rng_uniform (r);

  /* A uniform distribution over [a,b] */

  return a * (1 - u) + b * u;
}

double
gsl_ran_flat_pdf (double x, const double a, const double b)
{
  if (x < b && x >= a)
    {
      return 1 / (b - a);
    }
  else
    {
      return 0;
    }
}
