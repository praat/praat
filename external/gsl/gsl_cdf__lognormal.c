/* cdf/lognormal.c
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
gsl_cdf_lognormal_P (const double x, const double zeta, const double sigma)
{
  double u = (log (x) - zeta) / sigma;
  double P = gsl_cdf_ugaussian_P (u);
  return P;
}

double
gsl_cdf_lognormal_Q (const double x, const double zeta, const double sigma)
{
  double u = (log (x) - zeta) / sigma;
  double Q = gsl_cdf_ugaussian_Q (u);
  return Q;
}
