/* linalg/qr.c
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
#include "gsl_blas.h"

#include "gsl_linalg.h"

#define REAL double

#include "gsl_linalg__givens.c"
#include "gsl_linalg__apply_givens.c"

/* Factorise a general M x N matrix A into
 *  
 *   A = Q R
 *
 * where Q is orthogonal (M x M) and R is upper triangular (M x N).
 *
 * Q is stored as a packed set of Householder transformations in the
 * strict lower triangular part of the input matrix.
 *
 * R is stored in the diagonal and upper triangle of the input matrix.
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
gsl_linalg_QR_decomp (gsl_matrix * A, gsl_vector * tau)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

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

          gsl_vector_view c_full = gsl_matrix_column (A, i);
          gsl_vector_view c = gsl_vector_subvector (&(c_full.vector), i, M-i);

          double tau_i = gsl_linalg_householder_transform (&(c.vector));

          gsl_vector_set (tau, i, tau_i);

          /* Apply the transformation to the remaining columns and
             update the norms */

          if (i + 1 < N)
            {
              gsl_matrix_view m = gsl_matrix_submatrix (A, i, i + 1, M - i, N - (i + 1));
              gsl_linalg_householder_hm (tau_i, &(c.vector), &(m.matrix));
            }
        }

      return GSL_SUCCESS;
    }
}

/* Solves the system A x = b using the QR factorisation,

 *  R x = Q^T b
 *
 * to obtain x. Based on SLATEC code. 
 */

int
gsl_linalg_QR_solve (const gsl_matrix * QR, const gsl_vector * tau, const gsl_vector * b, gsl_vector * x)
{
  if (QR->size1 != QR->size2)
    {
      GSL_ERROR ("QR matrix must be square", GSL_ENOTSQR);
    }
  else if (QR->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (QR->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve for x */

      gsl_linalg_QR_svx (QR, tau, x);

      return GSL_SUCCESS;
    }
}

/* Solves the system A x = b in place using the QR factorisation,

 *  R x = Q^T b
 *
 * to obtain x. Based on SLATEC code. 
 */

int
gsl_linalg_QR_svx (const gsl_matrix * QR, const gsl_vector * tau, gsl_vector * x)
{

  if (QR->size1 != QR->size2)
    {
      GSL_ERROR ("QR matrix must be square", GSL_ENOTSQR);
    }
  else if (QR->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match x/rhs size", GSL_EBADLEN);
    }
  else
    {
      /* compute rhs = Q^T b */

      gsl_linalg_QR_QTvec (QR, tau, x);

      /* Solve R x = rhs, storing x in-place */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, QR, x);

      return GSL_SUCCESS;
    }
}


/* Find the least squares solution to the overdetermined system 
 *
 *   A x = b 
 *  
 * for M >= N using the QR factorization A = Q R. 
 */

int
gsl_linalg_QR_lssolve (const gsl_matrix * QR, const gsl_vector * tau, const gsl_vector * b, gsl_vector * x, gsl_vector * residual)
{
  const size_t M = QR->size1;
  const size_t N = QR->size2;

  if (M < N)
    {
      GSL_ERROR ("QR matrix must have M>=N", GSL_EBADLEN);
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
      gsl_matrix_const_view R = gsl_matrix_const_submatrix (QR, 0, 0, N, N);
      gsl_vector_view c = gsl_vector_subvector(residual, 0, N);

      gsl_vector_memcpy(residual, b);

      /* compute rhs = Q^T b */

      gsl_linalg_QR_QTvec (QR, tau, residual);

      /* Solve R x = rhs */

      gsl_vector_memcpy(x, &(c.vector));

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, &(R.matrix), x);

      /* Compute residual = b - A x = Q (Q^T b - R x) */
      
      gsl_vector_set_zero(&(c.vector));

      gsl_linalg_QR_Qvec(QR, tau, residual);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_QR_Rsolve (const gsl_matrix * QR, const gsl_vector * b, gsl_vector * x)
{
  if (QR->size1 != QR->size2)
    {
      GSL_ERROR ("QR matrix must be square", GSL_ENOTSQR);
    }
  else if (QR->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (QR->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match x size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve R x = b, storing x in-place */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, QR, x);

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_QR_Rsvx (const gsl_matrix * QR, gsl_vector * x)
{
  if (QR->size1 != QR->size2)
    {
      GSL_ERROR ("QR matrix must be square", GSL_ENOTSQR);
    }
  else if (QR->size1 != x->size)
    {
      GSL_ERROR ("matrix size must match rhs size", GSL_EBADLEN);
    }
  else
    {
      /* Solve R x = b, storing x in-place */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, QR, x);

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_R_solve (const gsl_matrix * R, const gsl_vector * b, gsl_vector * x)
{
  if (R->size1 != R->size2)
    {
      GSL_ERROR ("R matrix must be square", GSL_ENOTSQR);
    }
  else if (R->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (R->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Copy x <- b */

      gsl_vector_memcpy (x, b);

      /* Solve R x = b, storing x inplace in b */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, R, x);

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_R_svx (const gsl_matrix * R, gsl_vector * x)
{
  if (R->size1 != R->size2)
    {
      GSL_ERROR ("R matrix must be square", GSL_ENOTSQR);
    }
  else if (R->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* Solve R x = b, storing x inplace in b */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, R, x);

      return GSL_SUCCESS;
    }
}



/* Form the product Q^T v  from a QR factorized matrix 
 */

int
gsl_linalg_QR_QTvec (const gsl_matrix * QR, const gsl_vector * tau, gsl_vector * v)
{
  const size_t M = QR->size1;
  const size_t N = QR->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else if (v->size != M)
    {
      GSL_ERROR ("vector size must be N", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      /* compute Q^T v */

      for (i = 0; i < GSL_MIN (M, N); i++)
        {
          gsl_vector_const_view c = gsl_matrix_const_column (QR, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&(c.vector), i, M - i);
          gsl_vector_view w = gsl_vector_subvector (v, i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hv (ti, &(h.vector), &(w.vector));
        }
      return GSL_SUCCESS;
    }
}


int
gsl_linalg_QR_Qvec (const gsl_matrix * QR, const gsl_vector * tau, gsl_vector * v)
{
  const size_t M = QR->size1;
  const size_t N = QR->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else if (v->size != M)
    {
      GSL_ERROR ("vector size must be N", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      /* compute Q^T v */

      for (i = GSL_MIN (M, N); i > 0 && i--;)
        {
          gsl_vector_const_view c = gsl_matrix_const_column (QR, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&(c.vector), 
                                                                i, M - i);
          gsl_vector_view w = gsl_vector_subvector (v, i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hv (ti, &h.vector, &w.vector);
        }
      return GSL_SUCCESS;
    }
}

/* Form the product Q^T A  from a QR factorized matrix */

int
gsl_linalg_QR_QTmat (const gsl_matrix * QR, const gsl_vector * tau, gsl_matrix * A)
{
  const size_t M = QR->size1;
  const size_t N = QR->size2;

  if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else if (A->size1 != M)
    {
      GSL_ERROR ("matrix must have M rows", GSL_EBADLEN);
    }
  else
    {
      size_t i;

      /* compute Q^T A */

      for (i = 0; i < GSL_MIN (M, N); i++)
        {
          gsl_vector_const_view c = gsl_matrix_const_column (QR, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&(c.vector), i, M - i);
          gsl_matrix_view m = gsl_matrix_submatrix(A, i, 0, M - i, A->size2);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hm (ti, &(h.vector), &(m.matrix));
        }
      return GSL_SUCCESS;
    }
}


/*  Form the orthogonal matrix Q from the packed QR matrix */

int
gsl_linalg_QR_unpack (const gsl_matrix * QR, const gsl_vector * tau, gsl_matrix * Q, gsl_matrix * R)
{
  const size_t M = QR->size1;
  const size_t N = QR->size2;

  if (Q->size1 != M || Q->size2 != M)
    {
      GSL_ERROR ("Q matrix must be M x M", GSL_ENOTSQR);
    }
  else if (R->size1 != M || R->size2 != N)
    {
      GSL_ERROR ("R matrix must be M x N", GSL_ENOTSQR);
    }
  else if (tau->size != GSL_MIN (M, N))
    {
      GSL_ERROR ("size of tau must be MIN(M,N)", GSL_EBADLEN);
    }
  else
    {
      size_t i, j;

      /* Initialize Q to the identity */

      gsl_matrix_set_identity (Q);

      for (i = GSL_MIN (M, N); i > 0 && i--;)
        {
          gsl_vector_const_view c = gsl_matrix_const_column (QR, i);
          gsl_vector_const_view h = gsl_vector_const_subvector (&c.vector,
                                                                i, M - i);
          gsl_matrix_view m = gsl_matrix_submatrix (Q, i, i, M - i, M - i);
          double ti = gsl_vector_get (tau, i);
          gsl_linalg_householder_hm (ti, &h.vector, &m.matrix);
        }

      /*  Form the right triangular matrix R from a packed QR matrix */

      for (i = 0; i < M; i++)
        {
          for (j = 0; j < i && j < N; j++)
            gsl_matrix_set (R, i, j, 0.0);

          for (j = i; j < N; j++)
            gsl_matrix_set (R, i, j, gsl_matrix_get (QR, i, j));
        }

      return GSL_SUCCESS;
    }
}


/* Update a QR factorisation for A= Q R ,  A' = A + u v^T,

 * Q' R' = QR + u v^T
 *       = Q (R + Q^T u v^T)
 *       = Q (R + w v^T)
 *
 * where w = Q^T u.
 *
 * Algorithm from Golub and Van Loan, "Matrix Computations", Section
 * 12.5 (Updating Matrix Factorizations, Rank-One Changes)  
 */

int
gsl_linalg_QR_update (gsl_matrix * Q, gsl_matrix * R,
                      gsl_vector * w, const gsl_vector * v)
{
  const size_t M = R->size1;
  const size_t N = R->size2;

  if (Q->size1 != M || Q->size2 != M)
    {
      GSL_ERROR ("Q matrix must be M x M if R is M x N", GSL_ENOTSQR);
    }
  else if (w->size != M)
    {
      GSL_ERROR ("w must be length M if R is M x N", GSL_EBADLEN);
    }
  else if (v->size != N)
    {
      GSL_ERROR ("v must be length N if R is M x N", GSL_EBADLEN);
    }
  else
    {
      size_t j, k;
      double w0;

      /* Apply Given's rotations to reduce w to (|w|, 0, 0, ... , 0)

         J_1^T .... J_(n-1)^T w = +/- |w| e_1

         simultaneously applied to R,  H = J_1^T ... J^T_(n-1) R
         so that H is upper Hessenberg.  (12.5.2) */

      for (k = M - 1; k > 0; k--)
        {
          double c, s;
          double wk = gsl_vector_get (w, k);
          double wkm1 = gsl_vector_get (w, k - 1);

          create_givens (wkm1, wk, &c, &s);
          apply_givens_vec (w, k - 1, k, c, s);
          apply_givens_qr (M, N, Q, R, k - 1, k, c, s);
        }

      w0 = gsl_vector_get (w, 0);

      /* Add in w v^T  (Equation 12.5.3) */

      for (j = 0; j < N; j++)
        {
          double r0j = gsl_matrix_get (R, 0, j);
          double vj = gsl_vector_get (v, j);
          gsl_matrix_set (R, 0, j, r0j + w0 * vj);
        }

      /* Apply Givens transformations R' = G_(n-1)^T ... G_1^T H
         Equation 12.5.4 */

      for (k = 1; k < GSL_MIN(M,N+1); k++)
        {
          double c, s;
          double diag = gsl_matrix_get (R, k - 1, k - 1);
          double offdiag = gsl_matrix_get (R, k, k - 1);

          create_givens (diag, offdiag, &c, &s);
          apply_givens_qr (M, N, Q, R, k - 1, k, c, s);

          gsl_matrix_set (R, k, k - 1, 0.0);    /* exact zero of G^T */
        }

      return GSL_SUCCESS;
    }
}

int
gsl_linalg_QR_QRsolve (gsl_matrix * Q, gsl_matrix * R, const gsl_vector * b, gsl_vector * x)
{
  const size_t M = R->size1;
  const size_t N = R->size2;

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
      /* compute sol = Q^T b */

      gsl_blas_dgemv (CblasTrans, 1.0, Q, b, 0.0, x);

      /* Solve R x = sol, storing x in-place */

      gsl_blas_dtrsv (CblasUpper, CblasNoTrans, CblasNonUnit, R, x);

      return GSL_SUCCESS;
    }
}
