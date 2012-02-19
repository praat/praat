/* multimin/convergence.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Fabrice Rossi
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
#include "gsl_multimin.h"
#include "gsl_blas.h"

int
gsl_multimin_test_gradient (const gsl_vector *g, double epsabs)
{
  double norm;

  if (epsabs < 0.0)
    {
      GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);
    }

  norm = gsl_blas_dnrm2(g);
  
  if (norm < epsabs)
    {
      return GSL_SUCCESS;
    }

  return GSL_CONTINUE;
}

int
gsl_multimin_test_size (const double size, double epsabs)
{
  if (epsabs < 0.0)
    {
      GSL_ERROR ("absolute tolerance is negative", GSL_EBADTOL);
    }
  
  if (size < epsabs)
    {
      return GSL_SUCCESS;
    }

  return GSL_CONTINUE;
}
