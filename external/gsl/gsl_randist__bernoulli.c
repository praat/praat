/* randist/bernoulli.c
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

/* The bernoulli distribution has the form,

   prob(0) = 1-p, prob(1) = p

   */

unsigned int
gsl_ran_bernoulli (const gsl_rng * r, double p)
{
  double u = gsl_rng_uniform (r) ;

  if (u < p)
    {
      return 1 ;
    }
  else
    {
      return 0 ;
    }
}

double
gsl_ran_bernoulli_pdf (const unsigned int k, double p)
{
  if (k == 0)
    {
      return 1 - p ;
    }
  else if (k == 1)
    {
      return p ;
    }
  else
    {
      return 0 ;
    }
}
