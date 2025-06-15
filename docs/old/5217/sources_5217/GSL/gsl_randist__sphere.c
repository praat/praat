/* randist/sphere.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 James Theiler, Brian Gough
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

void
gsl_ran_dir_2d (const gsl_rng * r, double *x, double *y)
{
  /* This method avoids trig, but it does take an average of 8/pi =
   * 2.55 calls to the RNG, instead of one for the direct
   * trigonometric method.  */

  double u, v, s;
  do
    {
      u = -1 + 2 * gsl_rng_uniform (r);
      v = -1 + 2 * gsl_rng_uniform (r);
      s = u * u + v * v;
    }
  while (s > 1.0 || s == 0.0);

  /* This is the Von Neumann trick. See Knuth, v2, 3rd ed, p140
   * (exercise 23).  Note, no sin, cos, or sqrt !  */

  *x = (u * u - v * v) / s;
  *y = 2 * u * v / s;

  /* Here is the more straightforward approach, 
   *     s = sqrt (s);  *x = u / s;  *y = v / s;
   * It has fewer total operations, but one of them is a sqrt */
}

void
gsl_ran_dir_2d_trig_method (const gsl_rng * r, double *x, double *y)
{
  /* This is the obvious solution... */
  /* It ain't clever, but since sin/cos are often hardware accelerated,
   * it can be faster -- it is on my home Pentium -- than von Neumann's
   * solution, or slower -- as it is on my Sun Sparc 20 at work
   */
  double t = 6.2831853071795864 * gsl_rng_uniform (r);          /* 2*PI */
  *x = cos (t);
  *y = sin (t);
}

void
gsl_ran_dir_3d (const gsl_rng * r, double *x, double *y, double *z)
{
  double s, a;

  /* This is a variant of the algorithm for computing a random point
   * on the unit sphere; the algorithm is suggested in Knuth, v2,
   * 3rd ed, p136; and attributed to Robert E Knop, CACM, 13 (1970),
   * 326.
   */

  /* Begin with the polar method for getting x,y inside a unit circle
   */
  do
    {
      *x = -1 + 2 * gsl_rng_uniform (r);
      *y = -1 + 2 * gsl_rng_uniform (r);
      s = (*x) * (*x) + (*y) * (*y);
    }
  while (s > 1.0);

  *z = -1 + 2 * s;              /* z uniformly distributed from -1 to 1 */
  a = 2 * sqrt (1 - s);         /* factor to adjust x,y so that x^2+y^2
                                 * is equal to 1-z^2 */
  *x *= a;
  *y *= a;
}

void
gsl_ran_dir_nd (const gsl_rng * r, size_t n, double *x)
{
  double d;
  size_t i;
  /* See Knuth, v2, 3rd ed, p135-136.  The method is attributed to
   * G. W. Brown, in Modern Mathematics for the Engineer (1956).
   * The idea is that gaussians G(x) have the property that
   * G(x)G(y)G(z)G(...) is radially symmetric, a function only
   * r = sqrt(x^2+y^2+...)
   */
  d = 0;
  do
    {
      for (i = 0; i < n; ++i)
        {
          x[i] = gsl_ran_gaussian (r, 1.0);
          d += x[i] * x[i];
        }
    }
  while (d == 0);
  d = sqrt (d);
  for (i = 0; i < n; ++i)
    {
      x[i] /= d;
    }
}
