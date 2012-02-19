/* randist/gamma.c
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
#include "gsl_sf_gamma.h"
#include "gsl_rng.h"
#include "gsl_randist.h"

static double gamma_large (const gsl_rng * r, const double a);
static double gamma_frac (const gsl_rng * r, const double a);

/* The Gamma distribution of order a>0 is defined by:

   p(x) dx = {1 / \Gamma(a) b^a } x^{a-1} e^{-x/b} dx

   for x>0.  If X and Y are independent gamma-distributed random
   variables of order a1 and a2 with the same scale parameter b, then
   X+Y has gamma distribution of order a1+a2.

   The algorithms below are from Knuth, vol 2, 2nd ed, p. 129. */

double
gsl_ran_gamma_knuth (const gsl_rng * r, const double a, const double b)
{
  /* assume a > 0 */
  unsigned int na = floor (a);

  if (a == na)
    {
      return b * gsl_ran_gamma_int (r, na);
    }
  else if (na == 0)
    {
      return b * gamma_frac (r, a);
    }
  else
    {
      return b * (gsl_ran_gamma_int (r, na) + gamma_frac (r, a - na)) ;
    }
}

double
gsl_ran_gamma_int (const gsl_rng * r, const unsigned int a)
{
  if (a < 12)
    {
      unsigned int i;
      double prod = 1;

      for (i = 0; i < a; i++)
        {
          prod *= gsl_rng_uniform_pos (r);
        }

      /* Note: for 12 iterations we are safe against underflow, since
         the smallest positive random number is O(2^-32). This means
         the smallest possible product is 2^(-12*32) = 10^-116 which
         is within the range of double precision. */

      return -log (prod);
    }
  else
    {
      return gamma_large (r, (double) a);
    }
}

static double
gamma_large (const gsl_rng * r, const double a)
{
  /* Works only if a > 1, and is most efficient if a is large

     This algorithm, reported in Knuth, is attributed to Ahrens.  A
     faster one, we are told, can be found in: J. H. Ahrens and
     U. Dieter, Computing 12 (1974) 223-246.  */

  double sqa, x, y, v;
  sqa = sqrt (2 * a - 1);
  do
    {
      do
        {
          y = tan (M_PI * gsl_rng_uniform (r));
          x = sqa * y + a - 1;
        }
      while (x <= 0);
      v = gsl_rng_uniform (r);
    }
  while (v > (1 + y * y) * exp ((a - 1) * log (x / (a - 1)) - sqa * y));

  return x;
}

static double
gamma_frac (const gsl_rng * r, const double a)
{
  /* This is exercise 16 from Knuth; see page 135, and the solution is
     on page 551.  */

  double p, q, x, u, v;
  p = M_E / (a + M_E);
  do
    {
      u = gsl_rng_uniform (r);
      v = gsl_rng_uniform_pos (r);

      if (u < p)
        {
          x = exp ((1 / a) * log (v));
          q = exp (-x);
        }
      else
        {
          x = 1 - log (v);
          q = exp ((a - 1) * log (x));
        }
    }
  while (gsl_rng_uniform (r) >= q);

  return x;
}

double
gsl_ran_gamma_pdf (const double x, const double a, const double b)
{
  if (x < 0)
    {
      return 0 ;
    }
  else if (x == 0)
    {
      if (a == 1)
        return 1/b ;
      else
        return 0 ;
    }
  else if (a == 1)
    {
      return exp(-x/b)/b ;
    }
  else 
    {
      double p;
      double lngamma = gsl_sf_lngamma (a);
      p = exp ((a - 1) * log (x/b) - x/b - lngamma)/b;
      return p;
    }
}


/* New version based on Marsaglia and Tsang, "A Simple Method for
 * generating gamma variables", ACM Transactions on Mathematical
 * Software, Vol 26, No 3 (2000), p363-372.
 *
 * Implemented by J.D.Lamb@btinternet.com, minor modifications for GSL
 * by Brian Gough
 */

double
gsl_ran_gamma_mt (const gsl_rng * r, const double a, const double b)
{
  return gsl_ran_gamma (r, a, b);
}

double
gsl_ran_gamma (const gsl_rng * r, const double a, const double b)
{
  /* assume a > 0 */

  if (a < 1)
    {
      double u = gsl_rng_uniform_pos (r);
      return gsl_ran_gamma (r, 1.0 + a, b) * pow (u, 1.0 / a);
    }

  {
    double x, v, u;
    double d = a - 1.0 / 3.0;
    double c = (1.0 / 3.0) / sqrt (d);

    while (1)
      {
        do
          {
            x = gsl_ran_gaussian_ziggurat (r, 1.0);
            v = 1.0 + c * x;
          }
        while (v <= 0);

        v = v * v * v;
        u = gsl_rng_uniform_pos (r);

        if (u < 1 - 0.0331 * x * x * x * x) 
          break;

        if (log (u) < 0.5 * x * x + d * (1 - v + log (v)))
          break;
      }
    
    return b * d * v;
  }
}
