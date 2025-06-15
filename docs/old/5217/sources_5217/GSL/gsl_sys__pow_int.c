/* sys/pow_int.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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
#include "gsl_pow_int.h"

#ifndef HIDE_INLINE_STATIC
double gsl_pow_2(const double x) { return x*x;   }
double gsl_pow_3(const double x) { return x*x*x; }
double gsl_pow_4(const double x) { double x2 = x*x;   return x2*x2;    }
double gsl_pow_5(const double x) { double x2 = x*x;   return x2*x2*x;  }
double gsl_pow_6(const double x) { double x2 = x*x;   return x2*x2*x2; }
double gsl_pow_7(const double x) { double x3 = x*x*x; return x3*x3*x;  }
double gsl_pow_8(const double x) { double x2 = x*x;   double x4 = x2*x2; return x4*x4; }
double gsl_pow_9(const double x) { double x3 = x*x*x; return x3*x3*x3; }
#endif

double gsl_pow_int(double x, int n)
{
  double value = 1.0;

  if(n < 0) {
    x = 1.0/x;
    n = -n;
  }

  /* repeated squaring method 
   * returns 0.0^0 = 1.0, so continuous in x
   */
  do {
     if(n & 1) value *= x;  /* for n odd */
     n >>= 1;
     x *= x;
  } while (n);

  return value;
}

