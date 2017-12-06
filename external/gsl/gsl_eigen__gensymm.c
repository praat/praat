/* eigen/gensymm.c
 * 
 * Copyright (C) 2007 Patrick Alken
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

#include "gsl__config.h"
#include "gsl_eigen.h"
#include "gsl_linalg.h"
#include "gsl_math.h"
#include "gsl_blas.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"

/*
 * This module computes the eigenvalues of a real generalized
 * symmetric-definite eigensystem A x = \lambda B x, where A and
 * B are symmetric, and B is positive-definite.
 */

/*
gsl_eigen_gensymm_alloc()

Allocate a workspace for solving the generalized symmetric-definite
eigenvalue problem. The size of this workspace is O(2n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_gensymm_workspace *
gsl_eigen_gensymm_alloc(const size_t n)
{
  gsl_eigen_gensymm_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_gensymm_workspace *) calloc (1, sizeof (gsl_eigen_gensymm_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;

  w->symm_workspace_p = gsl_eigen_symm_alloc(n);
  if (!w->symm_workspace_p)
    {
      gsl_eigen_gensymm_free(w);
      GSL_ERROR_NULL("failed to allocate space for symm workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_gensymm_alloc() */

/*
gsl_eigen_gensymm_free()
  Free workspace w
*/

void
gsl_eigen_gensymm_free (gsl_eigen_gensymm_workspace * w)
{
  if (w->symm_workspace_p)
    gsl_eigen_symm_free(w->symm_workspace_p);

  free(w);
} /* gsl_eigen_gensymm_free() */

/*
gsl_eigen_gensymm()

Solve the generalized symmetric-definite eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda.

Inputs: A    - real symmetric matrix
        B    - real symmetric and positive definite matrix
        eval - where to store eigenvalues
        w    - workspace

Return: success or error
*/

int
gsl_eigen_gensymm (gsl_matrix * A, gsl_matrix * B, gsl_vector * eval,
                   gsl_eigen_gensymm_workspace * w)
{
  const size_t N = A->size1;

  /* check matrix and vector sizes */

  if (N != A->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues", GSL_ENOTSQR);
    }
  else if ((N != B->size1) || (N != B->size2))
    {
      GSL_ERROR ("B matrix dimensions must match A", GSL_EBADLEN);
    }
  else if (eval->size != N)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else if (w->size != N)
    {
      GSL_ERROR ("matrix size does not match workspace", GSL_EBADLEN);
    }
  else
    {
      int s;

      /* compute Cholesky factorization of B */
      s = gsl_linalg_cholesky_decomp(B);
      if (s != GSL_SUCCESS)
        return s; /* B is not positive definite */

      /* transform to standard symmetric eigenvalue problem */
      gsl_eigen_gensymm_standardize(A, B);

      s = gsl_eigen_symm(A, eval, w->symm_workspace_p);

      return s;
    }
} /* gsl_eigen_gensymm() */

/*
gsl_eigen_gensymm_standardize()
  Reduce the generalized symmetric-definite eigenproblem to
the standard symmetric eigenproblem by computing

C = L^{-1} A L^{-t}

where L L^t is the Cholesky decomposition of B

Inputs: A - (input/output) real symmetric matrix
        B - real symmetric, positive definite matrix in Cholesky form

Return: success

Notes: A is overwritten by L^{-1} A L^{-t}
*/

int
gsl_eigen_gensymm_standardize(gsl_matrix *A, const gsl_matrix *B)
{
  const size_t N = A->size1;
  size_t i;
  double a, b, c;

  for (i = 0; i < N; ++i)
    {
      /* update lower triangle of A(i:n, i:n) */

      a = gsl_matrix_get(A, i, i);
      b = gsl_matrix_get(B, i, i);
      a /= b * b;
      gsl_matrix_set(A, i, i, a);

      if (i < N - 1)
        {
          gsl_vector_view ai = gsl_matrix_subcolumn(A, i, i + 1, N - i - 1);
          gsl_matrix_view ma =
            gsl_matrix_submatrix(A, i + 1, i + 1, N - i - 1, N - i - 1);
          gsl_vector_const_view bi =
            gsl_matrix_const_subcolumn(B, i, i + 1, N - i - 1);
          gsl_matrix_const_view mb =
            gsl_matrix_const_submatrix(B, i + 1, i + 1, N - i - 1, N - i - 1);

          gsl_blas_dscal(1.0 / b, &ai.vector);

          c = -0.5 * a;
          gsl_blas_daxpy(c, &bi.vector, &ai.vector);

          gsl_blas_dsyr2(CblasLower, -1.0, &ai.vector, &bi.vector, &ma.matrix);

          gsl_blas_daxpy(c, &bi.vector, &ai.vector);

          gsl_blas_dtrsv(CblasLower,
                         CblasNoTrans,
                         CblasNonUnit,
                         &mb.matrix,
                         &ai.vector);
        }
    }

  return GSL_SUCCESS;
} /* gsl_eigen_gensymm_standardize() */
