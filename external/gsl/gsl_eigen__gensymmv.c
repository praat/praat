/* eigen/gensymmv.c
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
 * This module computes the eigenvalues and eigenvectors of a real
 * generalized symmetric-definite eigensystem A x = \lambda B x, where
 * A and B are symmetric, and B is positive-definite.
 */

static void gensymmv_normalize_eigenvectors(gsl_matrix *evec);

/*
gsl_eigen_gensymmv_alloc()

Allocate a workspace for solving the generalized symmetric-definite
eigenvalue problem. The size of this workspace is O(4n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_gensymmv_workspace *
gsl_eigen_gensymmv_alloc(const size_t n)
{
  gsl_eigen_gensymmv_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_gensymmv_workspace *)calloc (1, sizeof (gsl_eigen_gensymmv_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;

  w->symmv_workspace_p = gsl_eigen_symmv_alloc(n);
  if (!w->symmv_workspace_p)
    {
      gsl_eigen_gensymmv_free(w);
      GSL_ERROR_NULL("failed to allocate space for symmv workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_gensymmv_alloc() */

/*
gsl_eigen_gensymmv_free()
  Free workspace w
*/

void
gsl_eigen_gensymmv_free (gsl_eigen_gensymmv_workspace * w)
{
  if (w->symmv_workspace_p)
    gsl_eigen_symmv_free(w->symmv_workspace_p);

  free(w);
} /* gsl_eigen_gensymmv_free() */

/*
gsl_eigen_gensymmv()

Solve the generalized symmetric-definite eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda and eigenvectors x.

Inputs: A    - real symmetric matrix
        B    - real symmetric and positive definite matrix
        eval - where to store eigenvalues
        evec - where to store eigenvectors
        w    - workspace

Return: success or error
*/

int
gsl_eigen_gensymmv (gsl_matrix * A, gsl_matrix * B, gsl_vector * eval,
                    gsl_matrix * evec, gsl_eigen_gensymmv_workspace * w)
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
      s = gsl_linalg_cholesky_decomp(B);
      if (s != GSL_SUCCESS)
        return s; /* B is not positive definite */

      /* transform to standard symmetric eigenvalue problem */
      gsl_eigen_gensymm_standardize(A, B);

      /* compute eigenvalues and eigenvectors */
      s = gsl_eigen_symmv(A, eval, evec, w->symmv_workspace_p);
      if (s != GSL_SUCCESS)
        return s;

      /* backtransform eigenvectors: evec -> L^{-T} evec */
      gsl_blas_dtrsm(CblasLeft,
                     CblasLower,
                     CblasTrans,
                     CblasNonUnit,
                     1.0,
                     B,
                     evec);

      /* the blas call destroyed the normalization - renormalize */
      gensymmv_normalize_eigenvectors(evec);

      return GSL_SUCCESS;
    }
} /* gsl_eigen_gensymmv() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
gensymmv_normalize_eigenvectors()
  Normalize eigenvectors so that their Euclidean norm is 1

Inputs: evec - eigenvectors
*/

static void
gensymmv_normalize_eigenvectors(gsl_matrix *evec)
{
  const size_t N = evec->size1;
  size_t i;     /* looping */

  for (i = 0; i < N; ++i)
    {
      gsl_vector_view vi = gsl_matrix_column(evec, i);
      double scale = 1.0 / gsl_blas_dnrm2(&vi.vector);

      gsl_blas_dscal(scale, &vi.vector);
    }
} /* gensymmv_normalize_eigenvectors() */
