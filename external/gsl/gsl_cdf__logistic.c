/* cdf/logistic.c
 * 
 * Copyright (C) 2003, 2007 Brian Gough
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
#include "gsl_cdf.h"

double
gsl_cdf_logistic_P (const double x, const double a)
{
  double P;
  double u = x / a;

  if (u >= 0)
    {
      P = 1 / (1 + exp (-u));
    }
  else
    {
      P = exp (u) / (1 + exp (u));
    }

  return P;
}

double
gsl_cdf_logistic_Q (const double x, const double a)
{
  double Q;
  double u = x / a;

  if (u >= 0)
    {
      Q = exp (-u) / (1 + exp (-u));
    }
  else
    {
      Q = 1 / (1 + exp (u));
    }

  return Q;
}
