/* sys/ldfrexp.c
 * 
 * Copyright (C) 2002, Gert Van den Eynde
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

double
gsl_ldexp (const double x, const int e)
{
  double p2 = pow (2.0, (double)e);
  return x * p2;
}

double
gsl_frexp (const double x, int *e)
{
  if (x == 0.0)
    {
      *e = 0;
      return 0.0;
    }
  else
    {
      double ex = ceil (log (fabs (x)) / M_LN2);
      int ei = (int) ex;
      double f = gsl_ldexp (x, -ei);

      while (fabs (f) >= 1.0)
        {
          ei++;
          f /= 2.0;
        }
      
      while (fabs (f) < 0.5)
        {
          ei--;
          f *= 2.0;
        }

      *e = ei;
      return f;
    }
}
