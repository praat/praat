/* randist/levy.c
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

/* The stable Levy probability distributions have the form

   p(x) dx = (1/(2 pi)) \int dt exp(- it x - |c t|^alpha)

   with 0 < alpha <= 2. 

   For alpha = 1, we get the Cauchy distribution
   For alpha = 2, we get the Gaussian distribution with sigma = sqrt(2) c.

   Fromn Chapter 5 of Bratley, Fox and Schrage "A Guide to
   Simulation". The original reference given there is,

   J.M. Chambers, C.L. Mallows and B. W. Stuck. "A method for
   simulating stable random variates". Journal of the American
   Statistical Association, JASA 71 340-344 (1976).

   */

double
gsl_ran_levy (const gsl_rng * r, const double c, const double alpha)
{
  double u, v, t, s;

  u = M_PI * (gsl_rng_uniform_pos (r) - 0.5);

  if (alpha == 1)               /* cauchy case */
    {
      t = tan (u);
      return c * t;
    }

  do
    {
      v = gsl_ran_exponential (r, 1.0);
    }
  while (v == 0);

  if (alpha == 2)             /* gaussian case */
    {
      t = 2 * sin (u) * sqrt(v);
      return c * t;
    }

  /* general case */

  t = sin (alpha * u) / pow (cos (u), 1 / alpha);
  s = pow (cos ((1 - alpha) * u) / v, (1 - alpha) / alpha);

  return c * t * s;
}


/* The following routine for the skew-symmetric case was provided by
   Keith Briggs.

   The stable Levy probability distributions have the form

   2*pi* p(x) dx

     = \int dt exp(mu*i*t-|sigma*t|^alpha*(1-i*beta*sign(t)*tan(pi*alpha/2))) for alpha!=1
     = \int dt exp(mu*i*t-|sigma*t|^alpha*(1+i*beta*sign(t)*2/pi*log(|t|)))   for alpha==1

   with 0<alpha<=2, -1<=beta<=1, sigma>0.

   For beta=0, sigma=c, mu=0, we get gsl_ran_levy above.

   For alpha = 1, beta=0, we get the Lorentz distribution
   For alpha = 2, beta=0, we get the Gaussian distribution

   See A. Weron and R. Weron: Computer simulation of Lévy alpha-stable 
   variables and processes, preprint Technical University of Wroclaw.
   http://www.im.pwr.wroc.pl/~hugo/Publications.html

*/

double
gsl_ran_levy_skew (const gsl_rng * r, const double c, 
                   const double alpha, const double beta)
{
  double V, W, X;

  if (beta == 0)  /* symmetric case */
    {
      return gsl_ran_levy (r, c, alpha);
    }

  V = M_PI * (gsl_rng_uniform_pos (r) - 0.5);

  do
    {
      W = gsl_ran_exponential (r, 1.0);
    }
  while (W == 0);

  if (alpha == 1)
    {
      X = ((M_PI_2 + beta * V) * tan (V) -
           beta * log (M_PI_2 * W * cos (V) / (M_PI_2 + beta * V))) / M_PI_2;
      return c * (X + beta * log (c) / M_PI_2);
    }
  else
    {
      double t = beta * tan (M_PI_2 * alpha);
      double B = atan (t) / alpha;
      double S = pow (1 + t * t, 1/(2 * alpha));

      X = S * sin (alpha * (V + B)) / pow (cos (V), 1 / alpha)
        * pow (cos (V - alpha * (V + B)) / W, (1 - alpha) / alpha);
      return c * X;
    }
}
