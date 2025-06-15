/* linalg/lu.c
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
#include <string.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_permute_vector.h"
#include "gsl_blas.h"

#include "gsl_linalg.h"

#define REAL double

/* Factorise a general N x N matrix A into,
 *
 *   P A = L U
 *
 * where P is a permutation matrix, L is unit lower triangular and U
 * is upper triangular.
 *
 * L is stored in the strict lower triangular part of the input
 * matrix. The diagonal elements of L are unity and are not stored.
 *
 * U is stored in the diagonal and upper triangular part of the
 * input matrix.  
 * 
 * P is stored in the permutation p. Column j of P is column k of the
 * identity matrix, where k = permutation->data[j]
 *
 * signum gives the sign of the permutation, (-1)^n, where n is the
 * number of interchanges in the permutation. 
 *
 * See Golub & Van Loan, Matrix Computations, Algorithm 3.4.1 (Gauss
 * Elimination with Partial Pivoting).
 */

int
gsl_linalg_LU_decomp (gsl_matrix * A, gsl_permutation * p, int *signum)
{
  if (A->size1 != A->size2)
    {
      GSL_ERROR ("LU decomposition requires square matrix", GSL_ENOTSQR);
    }
  else if (p->size != A->size1)
    {
      GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
    }
  else
    {
      const size_t N = A->size1;
      size_t i, j, k;

      *signum = 1;
      gsl_permutation_init (p);

      for (j = 0; j < N - 1; j++)
        {
          /* Find maximum in the j-th column */

          REAL ajj, max = fabs (gsl_matrix_get (A, j, j));
          size_t i_pivot = j;

          for (i = j + 1; i < N; i++)
            {
              REAL aij = fabs (gsl_matrix_get (A, i, j));

              if (aij > max)
                {
                  max = aij;
                  i_pivot = i;
                }
            }

          if (i_pivot != j)
            {
              gsl_matrix_swap_rows (A, j, i_pivot);
              gsl_permutation_swap (p, j, i_pivot);
              *signum = -(*signum);
            }

          ajj = gsl_matrix_get (A, j, j);

          if (ajj != 0.0)
            {
              for (i = j + 1; i < N; i++)
                {
                  REAL aij = gsl_matrix_get (A, i, j) / ajj;
                  gsl_matrix_set (A, i, j, aij);

                  for (k = j + 1; k < N; k++)
                    {
                      REAL aik = gsl_matrix_get (A, i, k);
                      REAL ajk = gsl_matrix_get (A, j, k);
                      gsl_matrix_set (A, i, k, aik - aij * ajk);
                    }
                }
            }
        }
      
      return GSL_SUCCESS;
    }
}

int
gsl_linalg_LU_solve (const gsl_matrix * LU, const gsl_permutation * p, const gsl_vector * b, gsl_vector * x)
{
  if (LU->size1 != LU->size2)
    {
      GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
    }
  else if (LU->size1 != p->size)
    {
      GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
    }
  else if (LU->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (LU->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve for x */

      gsl_linalg_LU_svx (LU, p, x);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_LU_svx (const gsl_matrix * LU, const gsl_permutation * p, gsl_vector * x)
{
  if (LU->size1 != LU->size2)
    {
      GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
    }
  else if (LU->size1 != p->size)
    {
      GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
    }
  else if (LU->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match solution/rhs size", GSL_EBADLEN);
    }
  else
    {
      /* Apply permutation to RHS */

      gsl_permute_vector (p, x);

      /* Solve for c using forward-substitution, L c = P b */

      gsl_blas_dtrsv (CblasLower, CblasNoTrans, CblasUnit, LU, x);

      /* Perform back-substitution, U x = c */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, LU, x);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_LU_refine (const gsl_matrix * A, const gsl_matrix * LU, const gsl_permutation * p, const gsl_vector * b, gsl_vector * x, gsl_vector * residual)
{
  if (A->size1 != A->size2)
    {
      GSL_ERROR ("matrix a must be square", GSL_ENOTSQR);
    }
  if (LU->size1 != LU->size2)
    {
      GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
    }
  else if (A->size1 != LU->size2)
    {
      GSL_ERROR ("LU matrix must be decomposition of a", GSL_ENOTSQR);
    }
  else if (LU->size1 != p->size)
    {
      GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
    }
  else if (LU->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (LU->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Compute residual, residual = (A * x  - b) */

      gsl_vector_memcpy (residual, b);
      gsl_blas_dgemv (CblasNoTrans, 1.0, A, x, -1.0, residual);

      /* Find correction, delta = - (A^-1) * residual, and apply it */

      gsl_linalg_LU_svx (LU, p, residual);
      gsl_blas_daxpy (-1.0, residual, x);

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_LU_invert (const gsl_matrix * LU, const gsl_permutation * p, gsl_matrix * inverse)
{
  size_t i, n = LU->size1;

  int status = GSL_SUCCESS;

  gsl_matrix_set_identity (inverse);

  for (i = 0; i < n; i++)
    {
      gsl_vector_view c = gsl_matrix_column (inverse, i);
      int status_i = gsl_linalg_LU_svx (LU, p, &(c.vector));

      if (status_i)
        status = status_i;
    }

  return status;
}

double
gsl_linalg_LU_det (gsl_matrix * LU, int signum)
{
  size_t i, n = LU->size1;

  double det = (double) signum;

  for (i = 0; i < n; i++)
    {
      det *= gsl_matrix_get (LU, i, i);
    }

  return det;
}


double
gsl_linalg_LU_lndet (gsl_matrix * LU)
{
  size_t i, n = LU->size1;

  double lndet = 0.0;

  for (i = 0; i < n; i++)
    {
      lndet += log (fabs (gsl_matrix_get (LU, i, i)));
    }

  return lndet;
}


int
gsl_linalg_LU_sgndet (gsl_matrix * LU, int signum)
{
  size_t i, n = LU->size1;

  int s = signum;

  for (i = 0; i < n; i++)
    {
      double u = gsl_matrix_get (LU, i, i);

      if (u < 0)
        {
          s *= -1;
        }
      else if (u == 0)
        {
          s = 0;
          break;
        }
    }

  return s;
}
