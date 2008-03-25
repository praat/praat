/* randist/gauss.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 James Theiler, Brian Gough
 * Copyright (C) 2006 Charles Karney
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

/* Of the two methods provided below, I think the Polar method is more
 * efficient, but only when you are actually producing two random
 * deviates.  We don't produce two, because then we'd have to save one
 * in a static variable for the next call, and that would screws up
 * re-entrant or threaded code, so we only produce one.  This makes
 * the Ratio method suddenly more appealing.
 *
 * [Added by Charles Karney] We use Leva's implementation of the Ratio
 * method which avoids calling log() nearly all the time and makes the
 * Ratio method faster than the Polar method (when it produces just one
 * result per call).  Timing per call (gcc -O2 on 866MHz Pentium,
 * average over 10^8 calls)
 *
 *   Polar method: 660 ns
 *   Ratio method: 368 ns
 *
 */

/* Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122 */

double
gsl_ran_gaussian (const gsl_rng * r, const double sigma)
{
  double x, y, r2;

  do
    {
      /* choose x,y in uniform square (-1,-1) to (+1,+1) */
      x = -1 + 2 * gsl_rng_uniform_pos (r);
      y = -1 + 2 * gsl_rng_uniform_pos (r);

      /* see if it is in the unit circle */
      r2 = x * x + y * y;
    }
  while (r2 > 1.0 || r2 == 0);

  /* Box-Muller transform */
  return sigma * y * sqrt (-2.0 * log (r2) / r2);
}

/* Ratio method (Kinderman-Monahan); see Knuth v2, 3rd ed, p130.
 * K+M, ACM Trans Math Software 3 (1977) 257-260.
 *
 * [Added by Charles Karney] This is an implementation of Leva's
 * modifications to the original K+M method; see:
 * J. L. Leva, ACM Trans Math Software 18 (1992) 449-453 and 454-455. */

double
gsl_ran_gaussian_ratio_method (const gsl_rng * r, const double sigma)
{
  double u, v, x, y, Q;
  const double s = 0.449871;    /* Constants from Leva */
  const double t = -0.386595;
  const double a = 0.19600;
  const double b = 0.25472;
  const double r1 = 0.27597;
  const double r2 = 0.27846;

  do                            /* This loop is executed 1.369 times on average  */
    {
      /* Generate a point P = (u, v) uniform in a rectangle enclosing
         the K+M region v^2 <= - 4 u^2 log(u). */

      /* u in (0, 1] to avoid singularity at u = 0 */
      u = 1 - gsl_rng_uniform (r);

      /* v is in the asymmetric interval [-0.5, 0.5).  However v = -0.5
         is rejected in the last part of the while clause.  The
         resulting normal deviate is strictly symmetric about 0
         (provided that v is symmetric once v = -0.5 is excluded). */
      v = gsl_rng_uniform (r) - 0.5;

      /* Constant 1.7156 > sqrt(8/e) (for accuracy); but not by too
         much (for efficiency). */
      v *= 1.7156;

      /* Compute Leva's quadratic form Q */
      x = u - s;
      y = fabs (v) - t;
      Q = x * x + y * (a * y - b * x);

      /* Accept P if Q < r1 (Leva) */
      /* Reject P if Q > r2 (Leva) */
      /* Accept if v^2 <= -4 u^2 log(u) (K+M) */
      /* This final test is executed 0.012 times on average. */
    }
  while (Q >= r1 && (Q > r2 || v * v > -4 * u * u * log (u)));

  return sigma * (v / u);       /* Return slope */
}

double
gsl_ran_gaussian_pdf (const double x, const double sigma)
{
  double u = x / fabs (sigma);
  double p = (1 / (sqrt (2 * M_PI) * fabs (sigma))) * exp (-u * u / 2);
  return p;
}

double
gsl_ran_ugaussian (const gsl_rng * r)
{
  return gsl_ran_gaussian (r, 1.0);
}

double
gsl_ran_ugaussian_ratio_method (const gsl_rng * r)
{
  return gsl_ran_gaussian_ratio_method (r, 1.0);
}

double
gsl_ran_ugaussian_pdf (const double x)
{
  return gsl_ran_gaussian_pdf (x, 1.0);
}

