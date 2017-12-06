/* eigen/genhermv.c
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
 * This module computes the eigenvalues and eigenvectors of a complex
 * generalized hermitian-definite eigensystem A x = \lambda B x, where
 * A and B are hermitian, and B is positive-definite.
 */

static void genhermv_normalize_eigenvectors(gsl_matrix_complex *evec);

/*
gsl_eigen_genhermv_alloc()

Allocate a workspace for solving the generalized hermitian-definite
eigenvalue problem. The size of this workspace is O(5n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_genhermv_workspace *
gsl_eigen_genhermv_alloc(const size_t n)
{
  gsl_eigen_genhermv_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_genhermv_workspace *) calloc (1, sizeof (gsl_eigen_genhermv_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;

  w->hermv_workspace_p = gsl_eigen_hermv_alloc(n);
  if (!w->hermv_workspace_p)
    {
      gsl_eigen_genhermv_free(w);
      GSL_ERROR_NULL("failed to allocate space for hermv workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_genhermv_alloc() */

/*
gsl_eigen_genhermv_free()
  Free workspace w
*/

void
gsl_eigen_genhermv_free (gsl_eigen_genhermv_workspace * w)
{
  if (w->hermv_workspace_p)
    gsl_eigen_hermv_free(w->hermv_workspace_p);

  free(w);
} /* gsl_eigen_genhermv_free() */

/*
gsl_eigen_genhermv()

Solve the generalized hermitian-definite eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda and eigenvectors x.

Inputs: A    - complex hermitian matrix
        B    - complex hermitian and positive definite matrix
        eval - where to store eigenvalues
        evec - where to store eigenvectors
        w    - workspace

Return: success or error
*/

int
gsl_eigen_genhermv (gsl_matrix_complex * A, gsl_matrix_complex * B,
                    gsl_vector * eval, gsl_matrix_complex * evec,
                    gsl_eigen_genhermv_workspace * w)
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
  else if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (evec->size1 != N)
    {
      GSL_ERROR ("eigenvector matrix has wrong size", GSL_EBADLEN);
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

      /* compute eigenvalues and eigenvectors */
      s = gsl_eigen_hermv(A, eval, evec, w->hermv_workspace_p);
      if (s != GSL_SUCCESS)
        return s;

      /* backtransform eigenvectors: evec -> L^{-H} evec */
      gsl_blas_ztrsm(CblasLeft,
                     CblasLower,
                     CblasConjTrans,
                     CblasNonUnit,
                     GSL_COMPLEX_ONE,
                     B,
                     evec);

      /* the blas call destroyed the normalization - renormalize */
      genhermv_normalize_eigenvectors(evec);

      return GSL_SUCCESS;
    }
} /* gsl_eigen_genhermv() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
genhermv_normalize_eigenvectors()
  Normalize eigenvectors so that their Euclidean norm is 1

Inputs: evec - eigenvectors
*/

static void
genhermv_normalize_eigenvectors(gsl_matrix_complex *evec)
{
  const size_t N = evec->size1;
  size_t i;     /* looping */

  for (i = 0; i < N; ++i)
    {
      gsl_vector_complex_view vi = gsl_matrix_complex_column(evec, i);
      double scale = 1.0 / gsl_blas_dznrm2(&vi.vector);

      gsl_blas_zdscal(scale, &vi.vector);
    }
} /* genhermv_normalize_eigenvectors() */
