/* cdf/cdf_gamma.c
 * 
 * Copyright (C) 2003 Jason Stover.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 */

/* 
 * Author: J. Stover
 */

#include "gsl__config.h"
#include <math.h>
#include "gsl_cdf.h"
#include "gsl_math.h"
#include "gsl_sf_gamma.h"

double
gsl_cdf_gamma_P (const double x, const double a, const double b)
{
  double P;
  double y = x / b;

  if (x <= 0.0)
    {
      return 0.0;
    }

  if (y > a)
    {
      P = 1 - gsl_sf_gamma_inc_Q (a, y);
    }
  else
    {
      P = gsl_sf_gamma_inc_P (a, y);
    }

  return P;
}

double
gsl_cdf_gamma_Q (const double x, const double a, const double b)
{
  double Q;
  double y = x / b;

  if (x <= 0.0)
    {
      return 1.0;
    }

  if (y < a)
    {
      Q = 1 - gsl_sf_gamma_inc_P (a, y);
    }
  else
    {
      Q = gsl_sf_gamma_inc_Q (a, y);
    }

  return Q;
}
