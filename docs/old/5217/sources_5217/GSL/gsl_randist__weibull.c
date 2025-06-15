/* randist/weibull.c
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

/* The Weibull distribution has the form,

   p(x) dx = (b/a) (x/a)^(b-1) exp(-(x/a)^b) dx

 */

double
gsl_ran_weibull (const gsl_rng * r, const double a, const double b)
{
  double x = gsl_rng_uniform_pos (r);

  double z = pow (-log (x), 1 / b);

  return a * z;
}

double
gsl_ran_weibull_pdf (const double x, const double a, const double b)
{
  if (x < 0)
    {
      return 0 ;
    }
  else if (x == 0)
    {
      if (b == 1)
        return 1/a ;
      else
        return 0 ;
    }
  else if (b == 1)
    {
      return exp(-x/a)/a ;
    }
  else
    {
      double p = (b/a) * exp (-pow (x/a, b) + (b - 1) * log (x/a));
      return p;
    }
}
