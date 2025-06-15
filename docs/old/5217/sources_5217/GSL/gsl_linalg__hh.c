/* linalg/hh.c
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

/* Author:  G. Jungman */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_linalg.h"

#define REAL double

/* [Engeln-Mullges + Uhlig, Alg. 4.42]
 */

int
gsl_linalg_HH_solve (gsl_matrix * A, const gsl_vector * b, gsl_vector * x)
{
  if (A->size1 > A->size2)
    {
      /* System is underdetermined. */

      GSL_ERROR ("System is underdetermined", GSL_EINVAL);
    }
  else if (A->size2 != x->size)
    {
      GSL_ERROR ("matrix and vector sizes must be equal", GSL_EBADLEN);
    }
  else
    {
      int status ;

      gsl_vector_memcpy (x, b);

      status = gsl_linalg_HH_svx (A, x);
      
      return status ;
    }
}

int
gsl_linalg_HH_svx (gsl_matrix * A, gsl_vector * x)
{
  if (A->size1 > A->size2)
    {
      /* System is underdetermined. */

      GSL_ERROR ("System is underdetermined", GSL_EINVAL);
    }
  else if (A->size2 != x->size)
    {
      GSL_ERROR ("matrix and vector sizes must be equal", GSL_EBADLEN);
    }
  else
    {
      const size_t N = A->size1;
      const size_t M = A->size2;
      size_t i, j, k;
      REAL *d = (REAL *) malloc (N * sizeof (REAL));

      if (d == 0)
        {
          GSL_ERROR ("could not allocate memory for workspace", GSL_ENOMEM);
        }

      /* Perform Householder transformation. */

      for (i = 0; i < N; i++)
        {
          const REAL aii = gsl_matrix_get (A, i, i);
          REAL alpha;
          REAL f;
          REAL ak;
          REAL max_norm = 0.0;
          REAL r = 0.0;

          for (k = i; k < M; k++)
            {
              REAL aki = gsl_matrix_get (A, k, i);
              r += aki * aki;
            }

          if (r == 0.0)
            {
              /* Rank of matrix is less than size1. */
              free (d);
              GSL_ERROR ("matrix is rank deficient", GSL_ESING);
            }

          alpha = sqrt (r) * GSL_SIGN (aii);

          ak = 1.0 / (r + alpha * aii);
          gsl_matrix_set (A, i, i, aii + alpha);

          d[i] = -alpha;

          for (k = i + 1; k < N; k++)
            {
              REAL norm = 0.0;
              f = 0.0;
              for (j = i; j < M; j++)
                {
                  REAL ajk = gsl_matrix_get (A, j, k);
                  REAL aji = gsl_matrix_get (A, j, i);
                  norm += ajk * ajk;
                  f += ajk * aji;
                }
              max_norm = GSL_MAX (max_norm, norm);

              f *= ak;

              for (j = i; j < M; j++)
                {
                  REAL ajk = gsl_matrix_get (A, j, k);
                  REAL aji = gsl_matrix_get (A, j, i);
                  gsl_matrix_set (A, j, k, ajk - f * aji);
                }
            }

          if (fabs (alpha) < 2.0 * GSL_DBL_EPSILON * sqrt (max_norm))
            {
              /* Apparent singularity. */
              free (d);
              GSL_ERROR("apparent singularity detected", GSL_ESING);
            }

          /* Perform update of RHS. */

          f = 0.0;
          for (j = i; j < M; j++)
            {
              f += gsl_vector_get (x, j) * gsl_matrix_get (A, j, i);
            }
          f *= ak;
          for (j = i; j < M; j++)
            {
              REAL xj = gsl_vector_get (x, j);
              REAL aji = gsl_matrix_get (A, j, i);
              gsl_vector_set (x, j, xj - f * aji);
            }
        }

      /* Perform back-substitution. */

      for (i = N; i > 0 && i--;)
        {
          REAL xi = gsl_vector_get (x, i);
          REAL sum = 0.0;
          for (k = i + 1; k < N; k++)
            {
              sum += gsl_matrix_get (A, i, k) * gsl_vector_get (x, k);
            }

          gsl_vector_set (x, i, (xi - sum) / d[i]);
        }

      free (d);
      return GSL_SUCCESS;
    }
}

