/* eigen/genherm.c
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
#include "gsl_complex.h"
#include "gsl_complex_math.h"

/*
 * This module computes the eigenvalues of a complex generalized
 * hermitian-definite eigensystem A x = \lambda B x, where A and
 * B are hermitian, and B is positive-definite.
 */

/*
gsl_eigen_genherm_alloc()

Allocate a workspace for solving the generalized hermitian-definite
eigenvalue problem. The size of this workspace is O(3n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_genherm_workspace *
gsl_eigen_genherm_alloc(const size_t n)
{
  gsl_eigen_genherm_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_genherm_workspace *) calloc (1, sizeof (gsl_eigen_genherm_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;

  w->herm_workspace_p = gsl_eigen_herm_alloc(n);
  if (!w->herm_workspace_p)
    {
      gsl_eigen_genherm_free(w);
      GSL_ERROR_NULL("failed to allocate space for herm workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_genherm_alloc() */

/*
gsl_eigen_genherm_free()
  Free workspace w
*/

void
gsl_eigen_genherm_free (gsl_eigen_genherm_workspace * w)
{
  if (w->herm_workspace_p)
    gsl_eigen_herm_free(w->herm_workspace_p);

  free(w);
} /* gsl_eigen_genherm_free() */

/*
gsl_eigen_genherm()

Solve the generalized hermitian-definite eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda.

Inputs: A    - complex hermitian matrix
        B    - complex hermitian and positive definite matrix
        eval - where to store eigenvalues
        w    - workspace

Return: success or error
*/

int
gsl_eigen_genherm (gsl_matrix_complex * A, gsl_matrix_complex * B,
                   gsl_vector * eval, gsl_eigen_genherm_workspace * w)
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
      s = gsl_linalg_complex_cholesky_decomp(B);
      if (s != GSL_SUCCESS)
        return s; /* B is not positive definite */

      /* transform to standard hermitian eigenvalue problem */
      gsl_eigen_genherm_standardize(A, B);

      s = gsl_eigen_herm(A, eval, w->herm_workspace_p);

      return s;
    }
} /* gsl_eigen_genherm() */

/*
gsl_eigen_genherm_standardize()
  Reduce the generalized hermitian-definite eigenproblem to
the standard hermitian eigenproblem by computing

C = L^{-1} A L^{-H}

where L L^H is the Cholesky decomposition of B

Inputs: A - (input/output) complex hermitian matrix
        B - complex hermitian, positive definite matrix in Cholesky form

Return: success

Notes: A is overwritten by L^{-1} A L^{-H}
*/

int
gsl_eigen_genherm_standardize(gsl_matrix_complex *A,
                              const gsl_matrix_complex *B)
{
  const size_t N = A->size1;
  size_t i;
  double a, b;
  gsl_complex y, z;

  GSL_SET_IMAG(&z, 0.0);

  for (i = 0; i < N; ++i)
    {
      /* update lower triangle of A(i:n, i:n) */

      y = gsl_matrix_complex_get(A, i, i);
      a = GSL_REAL(y);
      y = gsl_matrix_complex_get(B, i, i);
      b = GSL_REAL(y);
      a /= b * b;
      GSL_SET_REAL(&z, a);
      gsl_matrix_complex_set(A, i, i, z);

      if (i < N - 1)
        {
          gsl_vector_complex_view ai =
            gsl_matrix_complex_subcolumn(A, i, i + 1, N - i - 1);
          gsl_matrix_complex_view ma =
            gsl_matrix_complex_submatrix(A, i + 1, i + 1, N - i - 1, N - i - 1);
          gsl_vector_complex_const_view bi =
            gsl_matrix_complex_const_subcolumn(B, i, i + 1, N - i - 1);
          gsl_matrix_complex_const_view mb =
            gsl_matrix_complex_const_submatrix(B, i + 1, i + 1, N - i - 1, N - i - 1);

          gsl_blas_zdscal(1.0 / b, &ai.vector);

          GSL_SET_REAL(&z, -0.5 * a);
          gsl_blas_zaxpy(z, &bi.vector, &ai.vector);

          gsl_blas_zher2(CblasLower,
                         GSL_COMPLEX_NEGONE,
                         &ai.vector,
                         &bi.vector,
                         &ma.matrix);

          gsl_blas_zaxpy(z, &bi.vector, &ai.vector);

          gsl_blas_ztrsv(CblasLower,
                         CblasNoTrans,
                         CblasNonUnit,
                         &mb.matrix,
                         &ai.vector);
        }
    }

  return GSL_SUCCESS;
} /* gsl_eigen_genherm_standardize() */
