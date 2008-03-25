/* linalg/lq.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
 * Copyright (C) 2004 Joerg Wensch, modifications for LQ.
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
#include <string.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_blas.h"

#include "gsl_linalg.h"

#define REAL double

#include "gsl_linalg__givens.c"
#include "gsl_linalg__apply_givens.c"

/* Note: The standard in numerical linear algebra is to solve A x = b
 * resp. ||A x - b||_2 -> min by QR-decompositions where x, b are
 * column vectors.
 *
 * When the matrix A has a large number of rows it is much more
 * efficient to work with the transposed matrix A^T and to solve the
 * system x^T A = b^T resp. ||x^T A - b^T||_2 -> min.  This is caused
 * by the row-oriented format in which GSL stores matrices.  Therefore
 * the QR-decomposition of A has to be replaced by a LQ decomposition
 * of A^T
 *
 * The purpose of this package is to provide the algorithms to compute
 * the LQ-decomposition and to solve the linear equations resp. least
 * squares problems.  The dimensions N, M of the matrix are switched
 * because here A will probably be a transposed matrix.  We write x^T,
 * b^T,... for vectors the comments to emphasize that they are row
 * vectors.
 *
 * It may even be useful to transpose your matrix explicitly (assumed
 * that there are no memory restrictions) because this takes O(M x N)
 * computing time where the decompostion takes O(M x N^2) computing
 * time.  */

/* Factorise a general N x M matrix A into
 *  
 *   A = L Q
 *
 * where Q is orthogonal (M x M) and L is lower triangular (N x M).
 *
 * Q is stored as a packed set of Householder transformations in the
 * strict upper triangular part of the input matrix.
 *
 * R is stored in the diagonal and lower triangle of the input matrix.
 *
 * The full matrix for Q can be obtained as the product
 *
 *       Q = Q_k .. Q_2 Q_1
 *
 * where k = MIN(M,N) and
 *
 *       Q_i = (I - tau_i * v_i * v_i')
 *
 * and where v_i is a Householder vector
 *
 *       v_i = [1, m(i+1,i), m(i+2,i), ... , m(M,i)]
 *
 * This storage scheme is the same as in LAPACK.  */

int
gsl_linalg_LQ_decomp (gsl_matrix * A, gsl_vector * tau)
{
  const size_t N = A->size1;
  const size_t M = A->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      for (i = 0; i < GSL_MIN (M, N); i++)
        {
          /* Compute the Householder transformation to reduce the j-th
             column of the matrix to a multiple of the j-th unit vector */

          gsl_vector_view c_full = gsl_matrix_row (A, i);
          gsl_vector_view c = gsl_vector_subvector (&(c_full.vector), i, M-i);

          double tau_i = gsl_linalg_householder_transform (&(c.vector));

          gsl_vector_set (tau, i, tau_i);

          /* Apply the transformation to the remaining columns and
             update the norms */

          if (i + 1 < N)
            {
              gsl_matrix_view m = gsl_matrix_submatrix (A, i + 1, i, N - (i + 1), M - i );
              gsl_linalg_householder_mh (tau_i, &(c.vector), &(m.matrix));
            }
        }

      return GSL_SUCCESS;
    }
}

/* Solves the system x^T A = b^T using the LQ factorisation,

 *  x^T L = b^T Q^T
 *
 * to obtain x. Based on SLATEC code. 
 */


int
gsl_linalg_LQ_solve_T (const gsl_matrix * LQ, const gsl_vector * tau, const gsl_vector * b, gsl_vector * x)
{
  if (LQ->size1 != LQ->size2)
    {
      GSL_ERROR ("LQ matrix must be square", GSL_ENOTSQR);
    }
  else if (LQ->size2 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (LQ->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve for x */

      gsl_linalg_LQ_svx_T (LQ, tau, x);

      return GSL_SUCCESS;
    }
}

/* Solves the system x^T A = b^T in place using the LQ factorisation,
 *
 *  x^T L = b^T Q^T
 *
 * to obtain x. Based on SLATEC code.
 */

int
gsl_linalg_LQ_svx_T (const gsl_matrix * LQ, const gsl_vector * tau, gsl_vector * x)
{

  if (LQ->size1 != LQ->size2)
    {
      GSL_ERROR ("LQ matrix must be square", GSL_ENOTSQR);
    }
  else if (LQ->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match x/rhs size", GSL_EBADLEN);
    }
  else
    {
      /* compute rhs = Q^T b */

      gsl_linalg_LQ_vecQT (LQ, tau, x);

      /* Solve R x = rhs, storing x in-place */

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, LQ, x);

      return GSL_SUCCESS;
    }
}


/* Find the least squares solution to the overdetermined system
 *
 *   x^T A = b^T
 *
 * for M >= N using the LQ factorization A = L Q.
 */

int
gsl_linalg_LQ_lssolve_T (const gsl_matrix * LQ, const gsl_vector * tau, const gsl_vector * b, gsl_vector * x, gsl_vector * residual)
{
  const size_t N = LQ->size1;
  const size_t M = LQ->size2;

  if (M < N)
    {
      GSL_ERROR ("LQ matrix must have M>=N", GSL_EBADLEN);
    }
  else if (M != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (N != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else if (M != residual->size)
    {
      GSL_ERROR ("matrix size must match residual size", GSL_EBADLEN);
    }
  else
    {
      gsl_matrix_const_view L = gsl_matrix_const_submatrix (LQ, 0, 0, N, N);
      gsl_vector_view c = gsl_vector_subvector(residual, 0, N);

      gsl_vector_memcpy(residual, b);

      /* compute rhs = b^T Q^T */

      gsl_linalg_LQ_vecQT (LQ, tau, residual);

      /* Solve x^T L = rhs */

      gsl_vector_memcpy(x, &(c.vector));

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, &(L.matrix), x);

      /* Compute residual = b^T - x^T A = (b^T Q^T - x^T L) Q */
      
      gsl_vector_set_zero(&(c.vector));

      gsl_linalg_LQ_vecQ(LQ, tau, residual);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_LQ_Lsolve_T (const gsl_matrix * LQ, const gsl_vector * b, gsl_vector * x)
{
  if (LQ->size1 != LQ->size2)
    {
      GSL_ERROR ("LQ matrix must be square", GSL_ENOTSQR);
    }
  else if (LQ->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (LQ->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match x size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve R x = b, storing x in-place */

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, LQ, x);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_LQ_Lsvx_T (const gsl_matrix * LQ, gsl_vector * x)
{
  if (LQ->size1 != LQ->size2)
    {
      GSL_ERROR ("LQ matrix must be square", GSL_ENOTSQR);
    }
  else if (LQ->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match rhs size", GSL_EBADLEN);
    }
  else
    {
      /* Solve x^T L = b^T, storing x in-place */

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, LQ, x);

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_L_solve_T (const gsl_matrix * L, const gsl_vector * b, gsl_vector * x)
{
  if (L->size1 != L->size2)
    {
      GSL_ERROR ("R matrix must be square", GSL_ENOTSQR);
    }
  else if (L->size2 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (L->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve R x = b, storing x inplace in b */

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, L, x);

      return GSL_SUCCESS;
    }
}




int
gsl_linalg_LQ_vecQT (const gsl_matrix * LQ, const gsl_vector * tau, gsl_vector * v)
{
  const size_t N = LQ->size1;
  const size_t M = LQ->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else if (v->size != M)
    {
      GSL_ERROR ("vector size must be M", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      /* compute v Q^T  */

      for (i = 0; i < GSL_MIN (M, N); i++)
        {
          gsl_vector_const_view c = gsl_matrix_const_row (LQ, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&(c.vector),
                                                                i, M - i);
          gsl_vector_view w = gsl_vector_subvector (v, i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hv (ti, &(h.vector), &(w.vector));
        }
      return GSL_SUCCESS;
    }
}

int
gsl_linalg_LQ_vecQ (const gsl_matrix * LQ, const gsl_vector * tau, gsl_vector * v)
{
  const size_t N = LQ->size1;
  const size_t M = LQ->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else if (v->size != M)
    {
      GSL_ERROR ("vector size must be M", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      /* compute v Q^T  */
      
      for (i =  GSL_MIN (M, N); i > 0 && i--;) 
        {
          gsl_vector_const_view c = gsl_matrix_const_row (LQ, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&(c.vector),
                                                                i, M - i);
          gsl_vector_view w = gsl_vector_subvector (v, i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hv (ti, &(h.vector), &(w.vector));
        }
      return GSL_SUCCESS;
    }
}


/*  Form the orthogonal matrix Q from the packed LQ matrix */

int
gsl_linalg_LQ_unpack (const gsl_matrix * LQ, const gsl_vector * tau, gsl_matrix * Q, gsl_matrix * L)
{
  const size_t N = LQ->size1;
  const size_t M = LQ->size2;

  if (Q->size1 != M || Q->size2 != M)
    {
      GSL_ERROR ("Q matrix must be M x M", GSL_ENOTSQR);
    }
  else if (L->size1 != N || L->size2 != M)
    {
      GSL_ERROR ("R matrix must be N x M", GSL_ENOTSQR);
    }
  else if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else
    {
      size_t i, j, l_border;

      /* Initialize Q to the identity */

      gsl_matrix_set_identity (Q);

      for (i = GSL_MIN (M, N); i > 0 && i--;)
        {
          gsl_vector_const_view c = gsl_matrix_const_row (LQ, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&c.vector,
                                                                i, M - i);
          gsl_matrix_view m = gsl_matrix_submatrix (Q, i, i, M - i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_mh (ti, &h.vector, &m.matrix);
        }

      /*  Form the lower triangular matrix L from a packed LQ matrix */

      for (i = 0; i < N; i++)
        {
	    l_border=GSL_MIN(i,M-1);
		for (j = 0; j <= l_border ; j++)
		    gsl_matrix_set (L, i, j, gsl_matrix_get (LQ, i, j));

	    for (j = l_border+1; j < M; j++)
		gsl_matrix_set (L, i, j, 0.0);
        }

      return GSL_SUCCESS;
    }
}


/* Update a LQ factorisation for A= L Q ,  A' = A + v u^T,

 * L' Q' = LQ + v u^T
 *       = (L + v u^T Q^T) Q
 *       = (L + v w^T) Q
 *
 * where w = Q u.
 *
 * Algorithm from Golub and Van Loan, "Matrix Computations", Section
 * 12.5 (Updating Matrix Factorizations, Rank-One Changes)
 */

int
gsl_linalg_LQ_update (gsl_matrix * Q, gsl_matrix * L,
                      const gsl_vector * v, gsl_vector * w)
{
  const size_t N = L->size1;
  const size_t M = L->size2;

  if (Q->size1 != M || Q->size2 != M)
    {
      GSL_ERROR ("Q matrix must be N x N if L is M x N", GSL_ENOTSQR);
    }
  else if (w->size != M)
    {
      GSL_ERROR ("w must be length N if L is M x N", GSL_EBADLEN);
    }
  else if (v->size != N)
    {
      GSL_ERROR ("v must be length M if L is M x N", GSL_EBADLEN);
    }
  else
    {
      size_t j, k;
      double w0;

      /* Apply Given's rotations to reduce w to (|w|, 0, 0, ... , 0)

         J_1^T .... J_(n-1)^T w = +/- |w| e_1

         simultaneously applied to L,  H = J_1^T ... J^T_(n-1) L
         so that H is upper Hessenberg.  (12.5.2) */
      
      for (k = M - 1; k > 0; k--)
        {
          double c, s;
          double wk = gsl_vector_get (w, k);
          double wkm1 = gsl_vector_get (w, k - 1);

          create_givens (wkm1, wk, &c, &s);
          apply_givens_vec (w, k - 1, k, c, s);
          apply_givens_lq (M, N, Q, L, k - 1, k, c, s);
       }

      w0 = gsl_vector_get (w, 0);

      /* Add in v w^T  (Equation 12.5.3) */

      for (j = 0; j < N; j++)
        {
          double lj0 = gsl_matrix_get (L, j, 0);
          double vj = gsl_vector_get (v, j);
          gsl_matrix_set (L, j, 0, lj0 + w0 * vj);
        }

      /* Apply Givens transformations L' = G_(n-1)^T ... G_1^T H
         Equation 12.5.4 */

      for (k = 1; k < GSL_MIN(M,N+1); k++)
        {
          double c, s;
          double diag = gsl_matrix_get (L, k - 1, k - 1);
          double offdiag = gsl_matrix_get (L, k - 1 , k);

          create_givens (diag, offdiag, &c, &s);
          apply_givens_lq (M, N, Q, L, k - 1, k, c, s);

          gsl_matrix_set (L, k - 1, k, 0.0);    /* exact zero of G^T */
        }

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_LQ_LQsolve (gsl_matrix * Q, gsl_matrix * L, const gsl_vector * b, gsl_vector * x)
{
  const size_t N = L->size1;
  const size_t M = L->size2;

  if (M != N)
    {
      return GSL_ENOTSQR;
    }
  else if (Q->size1 != M || b->size != M || x->size != M)
    {
      return GSL_EBADLEN;
    }
  else
    {
      /* compute sol = b^T Q^T */

      gsl_blas_dgemv (CblasNoTrans, 1.0, Q, b, 0.0, x);

      /* Solve x^T L = sol, storing x in-place */

      gsl_blas_dtrsv (CblasLower, CblasTrans, CblasNonUnit, L, x);

      return GSL_SUCCESS;
    }
}
