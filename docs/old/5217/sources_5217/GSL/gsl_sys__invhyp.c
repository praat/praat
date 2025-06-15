/* sys/invhyp.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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
#include "gsl_machine.h"

double
gsl_acosh (const double x)
{
  if (x > 1.0 / GSL_SQRT_DBL_EPSILON)
    {
      return log (x) + M_LN2;
    }
  else if (x > 2)
    {
      return log (2 * x - 1 / (sqrt (x * x - 1) + x));
    }
  else if (x > 1)
    {
      double t = x - 1;
      return log1p (t + sqrt (2 * t + t * t));
    }
  else if (x == 1)
    {
      return 0;
    }
  else
    {
      return GSL_NAN;
    }
}

double
gsl_asinh (const double x)
{
  double a = fabs (x);
  double s = (x < 0) ? -1 : 1;

  if (a > 1 / GSL_SQRT_DBL_EPSILON)
    {
      return s * (log (a) + M_LN2);
    }
  else if (a > 2)
    {
      return s * log (2 * a + 1 / (a + sqrt (a * a + 1)));
    }
  else if (a > GSL_SQRT_DBL_EPSILON)
    {
      double a2 = a * a;
      return s * log1p (a + a2 / (1 + sqrt (1 + a2)));
    }
  else
    {
      return x;
    }
}

double
gsl_atanh (const double x)
{
  double a = fabs (x);
  double s = (x < 0) ? -1 : 1;

  if (a > 1)
    {
      return GSL_NAN;
    }
  else if (a == 1)
    {
      return (x < 0) ? GSL_NEGINF : GSL_POSINF;
    }
  else if (a >= 0.5)
    {
      return s * 0.5 * log1p (2 * a / (1 - a));
    }
  else if (a > GSL_DBL_EPSILON)
    {
      return s * 0.5 * log1p (2 * a + 2 * a * a / (1 - a));
    }
  else
    {
      return x;
    }
}
