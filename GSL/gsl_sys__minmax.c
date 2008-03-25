/* sys/minmax.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

#define GSL_MAX(a,b) ((a) > (b) ? (a) : (b))
#define GSL_MIN(a,b) ((a) < (b) ? (a) : (b))

#ifndef HIDE_INLINE_STATIC
int GSL_MAX_INT (int a, int b);
int GSL_MIN_INT (int a, int b);
double GSL_MAX_DBL (double a, double b);
double GSL_MIN_DBL (double a, double b);
long double GSL_MAX_LDBL (long double a, long double b);
long double GSL_MIN_LDBL (long double a, long double b);

int
GSL_MAX_INT (int a, int b)
{
  return GSL_MAX (a, b);
}

int
GSL_MIN_INT (int a, int b)
{
  return GSL_MIN (a, b);
}

double
GSL_MAX_DBL (double a, double b)
{
  return GSL_MAX (a, b);
}

double
GSL_MIN_DBL (double a, double b)
{
  return GSL_MIN (a, b);
}

long double
GSL_MAX_LDBL (long double a, long double b)
{
  return GSL_MAX (a, b);
}

long double
GSL_MIN_LDBL (long double a, long double b)
{
  return GSL_MIN (a, b);
}
#endif

/* Define some static functions which are always available */

double gsl_max (double a, double b);
double gsl_min (double a, double b);

double gsl_max (double a, double b)
{
  return GSL_MAX (a, b);
}

double gsl_min (double a, double b)
{
  return GSL_MIN (a, b);
}

