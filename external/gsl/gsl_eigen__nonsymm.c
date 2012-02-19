/* eigen/nonsymm.c
 * 
 * Copyright (C) 2006 Patrick Alken
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
#include <stdlib.h>
#include <math.h>
#include "gsl_eigen.h"
#include "gsl_linalg.h"
#include "gsl_math.h"
#include "gsl_blas.h"
#include "gsl_vector.h"
#include "gsl_vector_complex.h"
#include "gsl_matrix.h"

/*
 * This module computes the eigenvalues of a real nonsymmetric
 * matrix, using the double shift Francis method.
 *
 * See the references in francis.c.
 *
 * This module gets the matrix ready by balancing it and
 * reducing it to Hessenberg form before passing it to the
 * francis module.
 */

/*
gsl_eigen_nonsymm_alloc()

Allocate a workspace for solving the nonsymmetric eigenvalue problem.
The size of this workspace is O(2n)

Inputs: n - size of matrix

Return: pointer to workspace
*/

gsl_eigen_nonsymm_workspace *
gsl_eigen_nonsymm_alloc(const size_t n)
{
  gsl_eigen_nonsymm_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_nonsymm_workspace *)
      calloc (1, sizeof (gsl_eigen_nonsymm_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;
  w->Z = NULL;
  w->do_balance = 0;

  w->diag = gsl_vector_alloc(n);

  if (w->diag == 0)
    {
      gsl_eigen_nonsymm_free(w);
      GSL_ERROR_NULL ("failed to allocate space for balancing vector", GSL_ENOMEM);
    }

  w->tau = gsl_vector_alloc(n);

  if (w->tau == 0)
    {
      gsl_eigen_nonsymm_free(w);
      GSL_ERROR_NULL ("failed to allocate space for hessenberg coefficients", GSL_ENOMEM);
    }

  w->francis_workspace_p = gsl_eigen_francis_alloc();

  if (w->francis_workspace_p == 0)
    {
      gsl_eigen_nonsymm_free(w);
      GSL_ERROR_NULL ("failed to allocate space for francis workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_nonsymm_alloc() */

/*
gsl_eigen_nonsymm_free()
  Free workspace w
*/

void
gsl_eigen_nonsymm_free (gsl_eigen_nonsymm_workspace * w)
{
  if (w->tau)
    gsl_vector_free(w->tau);

  if (w->diag)
    gsl_vector_free(w->diag);

  if (w->francis_workspace_p)
    gsl_eigen_francis_free(w->francis_workspace_p);

  free(w);
} /* gsl_eigen_nonsymm_free() */

/*
gsl_eigen_nonsymm_params()
  Set some parameters which define how we solve the eigenvalue
problem.

Inputs: compute_t - 1 if we want to compute T, 0 if not
        balance   - 1 if we want to balance the matrix, 0 if not
        w         - nonsymm workspace
*/

void
gsl_eigen_nonsymm_params (const int compute_t, const int balance,
                          gsl_eigen_nonsymm_workspace *w)
{
  gsl_eigen_francis_T(compute_t, w->francis_workspace_p);
  w->do_balance = balance;
} /* gsl_eigen_nonsymm_params() */

/*
gsl_eigen_nonsymm()

Solve the nonsymmetric eigenvalue problem

A x = \lambda x

for the eigenvalues \lambda using the Francis method.

Here we compute the real Schur form

T = Z^t A Z

with the diagonal blocks of T giving us the eigenvalues.
Z is a matrix of Schur vectors which is not computed by
this algorithm. See gsl_eigen_nonsymm_Z().

Inputs: A    - general real matrix
        eval - where to store eigenvalues
        w    - workspace

Return: success or error

Notes: If T is computed, it is stored in A on output. Otherwise
       the diagonal of A contains the 1-by-1 and 2-by-2 eigenvalue
       blocks.
*/

int
gsl_eigen_nonsymm (gsl_matrix * A, gsl_vector_complex * eval,
                   gsl_eigen_nonsymm_workspace * w)
{
  const size_t N = A->size1;

  /* check matrix and vector sizes */

  if (N != A->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues", GSL_ENOTSQR);
    }
  else if (eval->size != N)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else
    {
      int s;

      if (w->do_balance)
        {
          /* balance the matrix */
          gsl_linalg_balance_matrix(A, w->diag);
        }

      /* compute the Hessenberg reduction of A */
      gsl_linalg_hessenberg_decomp(A, w->tau);

      if (w->Z)
        {
          /*
           * initialize the matrix Z to U, which is the matrix used
           * to construct the Hessenberg reduction.
           */

          /* compute U and store it in Z */
          gsl_linalg_hessenberg_unpack(A, w->tau, w->Z);

          /* find the eigenvalues and Schur vectors */
          s = gsl_eigen_francis_Z(A, eval, w->Z, w->francis_workspace_p);

          if (w->do_balance)
            {
              /*
               * The Schur vectors in Z are the vectors for the balanced
               * matrix. We now must undo the balancing to get the
               * vectors for the original matrix A.
               */
              gsl_linalg_balance_accum(w->Z, w->diag);
            }
        }
      else
        {
          /* find the eigenvalues only */
          s = gsl_eigen_francis(A, eval, w->francis_workspace_p);
        }

      w->n_evals = w->francis_workspace_p->n_evals;

      return s;
    }
} /* gsl_eigen_nonsymm() */

/*
gsl_eigen_nonsymm_Z()

Solve the nonsymmetric eigenvalue problem

A x = \lambda x

for the eigenvalues \lambda.

Here we compute the real Schur form

T = Z^t A Z

with the diagonal blocks of T giving us the eigenvalues.
Z is the matrix of Schur vectors.

Inputs: A    - general real matrix
        eval - where to store eigenvalues
        Z    - where to store Schur vectors
        w    - workspace

Return: success or error

Notes: If T is computed, it is stored in A on output. Otherwise
       the diagonal of A contains the 1-by-1 and 2-by-2 eigenvalue
       blocks.
*/

int
gsl_eigen_nonsymm_Z (gsl_matrix * A, gsl_vector_complex * eval,
                     gsl_matrix * Z, gsl_eigen_nonsymm_workspace * w)
{
  /* check matrix and vector sizes */

  if (A->size1 != A->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues", GSL_ENOTSQR);
    }
  else if (eval->size != A->size1)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else if ((Z->size1 != Z->size2) || (Z->size1 != A->size1))
    {
      GSL_ERROR ("Z matrix has wrong dimensions", GSL_EBADLEN);
    }
  else
    {
      int s;

      w->Z = Z;

      s = gsl_eigen_nonsymm(A, eval, w);

      w->Z = NULL;

      return s;
    }
} /* gsl_eigen_nonsymm_Z() */
