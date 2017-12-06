/* poly/zsolve.c
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

/* zsolve.c - finds the complex roots of  = 0 */

#include "gsl__config.h"
#include <math.h>
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_complex.h"
#include "gsl_poly.h"

/* C-style matrix elements */
#define MAT(m,i,j,n) ((m)[(i)*(n) + (j)])

/* Fortran-style matrix elements */
#define FMAT(m,i,j,n) ((m)[((i)-1)*(n) + ((j)-1)])

#include "gsl_poly__companion.c"
#include "gsl_poly__balance.c"
#include "gsl_poly__qr.c"

int
gsl_poly_complex_solve (const double *a, size_t n,
                        gsl_poly_complex_workspace * w,
                        gsl_complex_packed_ptr z)
{
  int status;
  double *m;

  if (n == 0)
    {
      GSL_ERROR ("number of terms must be a positive integer", GSL_EINVAL);
    }

  if (n == 1)
    {
      GSL_ERROR ("cannot solve for only one term", GSL_EINVAL);
    }

  if (a[n - 1] == 0)
    {
      GSL_ERROR ("leading term of polynomial must be non-zero", GSL_EINVAL) ;
    }

  if (w->nc != n - 1)
    {
      GSL_ERROR ("size of workspace does not match polynomial", GSL_EINVAL);
    }
  
  m = w->matrix;

  set_companion_matrix (a, n - 1, m);

  balance_companion_matrix (m, n - 1);

  status = qr_companion (m, n - 1, z);

  if (status)
    {
      GSL_ERROR("root solving qr method failed to converge", GSL_EFAILED);
    }

  return GSL_SUCCESS;
}



