/* sys/infnan.c
 * 
 * Copyright (C) 2001 Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gsl__config.h"
#include <math.h>

double gsl_nan (void);
double gsl_posinf (void);
double gsl_neginf (void);
double gsl_fdiv (const double x, const double y);

double gsl_nan (void)
{
  return gsl_fdiv (0.0, 0.0);
}

double gsl_posinf (void)
{
  return gsl_fdiv (+1.0, 0.0);
}

double gsl_neginf (void)
{
  return gsl_fdiv (-1.0, 0.0);
}


int gsl_isnan (const double x);
int gsl_isinf (const double x);
int gsl_finite (const double x);

#if defined(_MSC_VER) /* Microsoft Visual C++ */
#include <float.h>
int
gsl_isnan (const double x)
{
  return _isnan(x);
}

int
gsl_isinf (const double x)
{
  int fpc = _fpclass(x);

  if (fpc == _FPCLASS_PINF)
    return +1;
  else if (fpc == _FPCLASS_NINF)
    return -1;
  else 
    return 0;
}

int
gsl_finite (const double x)
{
  return _finite(x);
}
#elif defined(HAVE_IEEE_COMPARISONS)
int
gsl_isnan (const double x)
{
  int status = (x != x);
  return status;
}

int
gsl_isinf (const double x)
{
  double y = x - x;
  int s = (y != y);

  if (s && x > 0)
    return +1;
  else if (s && x < 0)
    return -1;
  else
    return 0;
}

int
gsl_finite (const double x)
{
  const double y = x - x;
  int status = (y == y);
  return status;
}
#endif

