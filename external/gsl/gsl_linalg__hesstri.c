/* linalg/hesstri.c
 * 
 * Copyright (C) 2006, 2007 Patrick Alken
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

#include <stdlib.h>
#include <math.h>

#include "gsl__config.h"
#include "gsl_linalg.h"
#include "gsl_matrix.h"
#include "gsl_vector.h"
#include "gsl_blas.h"

#include "gsl_linalg__givens.c"

/*
 * This module contains routines related to the Hessenberg-Triangular
 * reduction of two general real matrices
 *
 * See Golub & Van Loan, "Matrix Computations", 3rd ed, sec 7.7.4
 */

/*
gsl_linalg_hesstri_decomp()
  Perform a reduction to generalized upper Hessenberg form.
Given A and B, this function overwrites A with an upper Hessenberg
matrix H = U^T A V and B with an upper triangular matrix R = U^T B V
with U and V orthogonal.

See Golub & Van Loan, "Matrix Computations" (3rd ed) algorithm 7.7.1

Inputs: A    - real square matrix
        B    - real square matrix
        U    - (output) if non-null, U is stored here on output
        V    - (output) if non-null, V is stored here on output
        work - workspace (length n)

Return: success or error
*/

int
gsl_linalg_hesstri_decomp(gsl_matrix * A, gsl_matrix * B, gsl_matrix * U,
                          gsl_matrix * V, gsl_vector * work)
{
  const size_t N = A->size1;

  if ((N != A->size2) || (N != B->size1) || (N != B->size2))
    {
      GSL_ERROR ("Hessenberg-triangular reduction requires square matrices",
                 GSL_ENOTSQR);
    }
  else if (N != work->size)
    {
      GSL_ERROR ("length of workspace must match matrix dimension",
                 GSL_EBADLEN);
    }
  else
    {
      double cs, sn;          /* rotation parameters */
      size_t i, j;            /* looping */
      gsl_vector_view xv, yv; /* temporary views */

      /* B -> Q^T B = R (upper triangular) */
      gsl_linalg_QR_decomp(B, work);

      /* A -> Q^T A */
      gsl_linalg_QR_QTmat(B, work, A);

      /* initialize U and V if desired */

      if (U)
        {
          gsl_linalg_QR_unpack(B, work, U, B);
        }
      else
        {
          /* zero out lower triangle of B */
          for (j = 0; j < N - 1; ++j)
            {
              for (i = j + 1; i < N; ++i)
                gsl_matrix_set(B, i, j, 0.0);
            }
        }

      if (V)
        gsl_matrix_set_identity(V);

      if (N < 3)
        return GSL_SUCCESS; /* nothing more to do */

      /* reduce A and B */
      for (j = 0; j < N - 2; ++j)
        {
          for (i = N - 1; i >= (j + 2); --i)
            {
              /* step 1: rotate rows i - 1, i to kill A(i,j) */

              /*
               * compute G = [ CS SN ] so that G^t [ A(i-1,j) ] = [ * ]
               *             [-SN CS ]             [ A(i, j)  ]   [ 0 ]
               */
              create_givens(gsl_matrix_get(A, i - 1, j),
                            gsl_matrix_get(A, i, j),
                            &cs,
                            &sn);
              /* invert so drot() works correctly (G -> G^t) */
              sn = -sn;

              /* compute G^t A(i-1:i, j:n) */
              xv = gsl_matrix_subrow(A, i - 1, j, N - j);
              yv = gsl_matrix_subrow(A, i, j, N - j);
              gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

              /* compute G^t B(i-1:i, i-1:n) */
              xv = gsl_matrix_subrow(B, i - 1, i - 1, N - i + 1);
              yv = gsl_matrix_subrow(B, i, i - 1, N - i + 1);
              gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

              if (U)
                {
                  /* accumulate U: U -> U G */
                  xv = gsl_matrix_column(U, i - 1);
                  yv = gsl_matrix_column(U, i);
                  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
                }

              /* step 2: rotate columns i, i - 1 to kill B(i, i - 1) */

              create_givens(-gsl_matrix_get(B, i, i),
                            gsl_matrix_get(B, i, i - 1),
                            &cs,
                            &sn);
              /* invert so drot() works correctly (G -> G^t) */
              sn = -sn;

              /* compute B(1:i, i-1:i) G */
              xv = gsl_matrix_subcolumn(B, i - 1, 0, i + 1);
              yv = gsl_matrix_subcolumn(B, i, 0, i + 1);
              gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

              /* apply to A(1:n, i-1:i) */
              xv = gsl_matrix_column(A, i - 1);
              yv = gsl_matrix_column(A, i);
              gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

              if (V)
                {
                  /* accumulate V: V -> V G */
                  xv = gsl_matrix_column(V, i - 1);
                  yv = gsl_matrix_column(V, i);
                  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
                }
            }
        }

      return GSL_SUCCESS;
    }
} /* gsl_linalg_hesstri_decomp() */
