/* randist/poisson.c
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

/* The poisson distribution has the form

   p(n) = (mu^n / n!) exp(-mu) 

   for n = 0, 1, 2, ... . The method used here is the one from Knuth. */

unsigned int
gsl_ran_poisson (const gsl_rng * r, double mu)
{
  double emu;
  double prod = 1.0;
  unsigned int k = 0;

  while (mu > 10)
    {
      unsigned int m = mu * (7.0 / 8.0);

      double X = gsl_ran_gamma_int (r, m);

      if (X >= mu)
        {
          return k + gsl_ran_binomial (r, mu / X, m - 1);
        }
      else
        {
          k += m;
          mu -= X; 
        }
    }

  /* This following method works well when mu is small */

  emu = exp (-mu);

  do
    {
      prod *= gsl_rng_uniform (r);
      k++;
    }
  while (prod > emu);

  return k - 1;

}

void
gsl_ran_poisson_array (const gsl_rng * r, size_t n, unsigned int array[],
                       double mu)
{
  size_t i;

  for (i = 0; i < n; i++)
    {
      array[i] = gsl_ran_poisson (r, mu);
    }

  return;
}

double
gsl_ran_poisson_pdf (const unsigned int k, const double mu)
{
  double p;
  double lf = gsl_sf_lnfact (k); 

  p = exp (log (mu) * k - lf - mu);
  return p;
}
