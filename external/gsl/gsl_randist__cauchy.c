/* randist/cauchy.c
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

/* The Cauchy probability distribution is 

   p(x) dx = (1/(pi a)) (1 + (x/a)^2)^(-1) dx

   It is also known as the Lorentzian probability distribution */

double
gsl_ran_cauchy (const gsl_rng * r, const double a)
{
  double u;
  do
    {
      u = gsl_rng_uniform (r);
    }
  while (u == 0.5);

  return a * tan (M_PI * u);
}

double
gsl_ran_cauchy_pdf (const double x, const double a)
{
  double u = x / a;
  double p = (1 / (M_PI * a)) / (1 + u * u);
  return p;
}
