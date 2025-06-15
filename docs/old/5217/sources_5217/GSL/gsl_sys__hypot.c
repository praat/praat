/* sys/hypot.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough, Patrick Alken
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

double gsl_hypot (const double x, const double y);
double gsl_hypot3 (const double x, const double y, const double z);

double gsl_hypot (const double x, const double y)
{
  double xabs = fabs(x) ;
  double yabs = fabs(y) ;
  double min, max;

  if (xabs < yabs) {
    min = xabs ;
    max = yabs ;
  } else {
    min = yabs ;
    max = xabs ;
  }

  if (min == 0) 
    {
      return max ;
    }

  {
    double u = min / max ;
    return max * sqrt (1 + u * u) ;
  }
}

double
gsl_hypot3(const double x, const double y, const double z)
{
  double xabs = fabs(x);
  double yabs = fabs(y);
  double zabs = fabs(z);
  double w = GSL_MAX(xabs, GSL_MAX(yabs, zabs));

  if (w == 0.0)
    {
      return (0.0);
    }
  else
    {
      double r = w * sqrt((xabs / w) * (xabs / w) +
                          (yabs / w) * (yabs / w) +
                          (zabs / w) * (zabs / w));
      return r;
    }
}
