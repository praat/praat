/* linalg/balance.c
 * 
 * Copyright (C) 2001, 2004, 2007 Brian Gough
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

/* Balance a general matrix by scaling the columns
 *
 * B =  A D
 *
 * where D is a diagonal matrix
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_blas.h"

#include "gsl_linalg.h"

int
gsl_linalg_balance_columns (gsl_matrix * A, gsl_vector * D)
{
  const size_t N = A->size2;
  size_t j;

  if (D->size != A->size2)
    {
      GSL_ERROR("length of D must match second dimension of A", GSL_EINVAL);
    }
  
  gsl_vector_set_all (D, 1.0);

  for (j = 0; j < N; j++)
    {
      gsl_vector_view A_j = gsl_matrix_column (A, j);
      
      double s = gsl_blas_dasum(&A_j.vector);
      
      double f = 1.0;
      
      if (s == 0.0 || !gsl_finite(s))
        {
          gsl_vector_set (D, j, f);
          continue;
        }

      /* FIXME: we could use frexp() here */

      while (s > 1.0)
        {
          s /= 2.0;
          f *= 2.0;
        }
      
      while (s < 0.5)
        {
          s *= 2.0;
          f /= 2.0;
        }
      
      gsl_vector_set (D, j, f);

      if (f != 1.0)
        {
          gsl_blas_dscal(1.0/f, &A_j.vector);
        }
    }

  return GSL_SUCCESS;
}
