/* sys/coerce.c
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

double gsl_coerce_double (const double x);

double 
gsl_coerce_double (const double x)
{
  volatile double y;
  y = x;
  return y;
}

float gsl_coerce_float (const float x);

float 
gsl_coerce_float (const float x)
{
  volatile float y;
  y = x;
  return y;
}

/* The following function is not needed, but is included for completeness */

long double gsl_coerce_long_double (const long double x);

long double 
gsl_coerce_long_double (const long double x)
{
  volatile long double y;
  y = x;
  return y;
}

