/* cdf/cdf_chisq.c
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 */

#include "gsl__config.h"
#include "gsl_cdf.h"
#include "gsl_sf_gamma.h"

double
gsl_cdf_chisq_P (const double x, const double nu)
{
  return gsl_cdf_gamma_P (x, nu / 2, 2.0);
}

double
gsl_cdf_chisq_Q (const double x, const double nu)
{
  return gsl_cdf_gamma_Q (x, nu / 2, 2.0);
}
