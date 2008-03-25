/* linalg/svd.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Gerard Jungman, Brian Gough
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

#include "gsl_linalg__givens.c"
#include "gsl_linalg__svdstep.c"

/* Factorise a general M x N matrix A into,
 *
 *   A = U D V^T
 *
 * where U is a column-orthogonal M x N matrix (U^T U = I), 
 * D is a diagonal N x N matrix, 
 * and V is an N x N orthogonal matrix (V^T V = V V^T = I)
 *
 * U is stored in the original matrix A, which has the same size
 *
 * V is stored as a separate matrix (not V^T). You must take the
 * transpose to form the product above.
 *
 * The diagonal matrix D is stored in the vector S,  D_ii = S_i
 */

int
gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, 
                      gsl_vector * work)
{
  size_t a, b, i, j, iter;

  const size_t M = A->size1;
  const size_t N = A->size2;
  const size_t K = GSL_MIN (M, N);

  if (M < N)
    {
      GSL_ERROR ("svd of MxN matrix, M<N, is not implemented", GSL_EUNIMPL);
    }
  else if (V->size1 != N)
    {
      GSL_ERROR ("square matrix V must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (V->size1 != V->size2)
    {
      GSL_ERROR ("matrix V must be square", GSL_ENOTSQR);
    }
  else if (S->size != N)
    {
      GSL_ERROR ("length of vector S must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (work->size != N)
    {
      GSL_ERROR ("length of workspace must match second dimension of matrix A",
                 GSL_EBADLEN);
    }

  /* Handle the case of N = 1 (SVD of a column vector) */

  if (N == 1)
    {
      gsl_vector_view column = gsl_matrix_column (A, 0);
      double norm = gsl_blas_dnrm2 (&column.vector);

      gsl_vector_set (S, 0, norm); 
      gsl_matrix_set (V, 0, 0, 1.0);
      
      if (norm != 0.0)
        {
          gsl_blas_dscal (1.0/norm, &column.vector);
        }

      return GSL_SUCCESS;
    }
  
  {
    gsl_vector_view f = gsl_vector_subvector (work, 0, K - 1);
    
    /* bidiagonalize matrix A, unpack A into U S V */
    
    gsl_linalg_bidiag_decomp (A, S, &f.vector);
    gsl_linalg_bidiag_unpack2 (A, S, &f.vector, V);
    
    /* apply reduction steps to B=(S,Sd) */
    
    chop_small_elements (S, &f.vector);
    
    /* Progressively reduce the matrix until it is diagonal */
    
    b = N - 1;
    iter = 0;

    while (b > 0)
      {
        double fbm1 = gsl_vector_get (&f.vector, b - 1);

        if (fbm1 == 0.0 || gsl_isnan (fbm1))
          {
            b--;
            continue;
          }
        
        /* Find the largest unreduced block (a,b) starting from b
           and working backwards */
        
        a = b - 1;
        
        while (a > 0)
          {
            double fam1 = gsl_vector_get (&f.vector, a - 1);

            if (fam1 == 0.0 || gsl_isnan (fam1))
              {
                break;
              }
            
            a--;
          }

        iter++;
        
        if (iter > 100 * N) 
          {
            GSL_ERROR("SVD decomposition failed to converge", GSL_EMAXITER);
          }

        
        {
          const size_t n_block = b - a + 1;
          gsl_vector_view S_block = gsl_vector_subvector (S, a, n_block);
          gsl_vector_view f_block = gsl_vector_subvector (&f.vector, a, n_block - 1);
          
          gsl_matrix_view U_block =
            gsl_matrix_submatrix (A, 0, a, A->size1, n_block);
          gsl_matrix_view V_block =
            gsl_matrix_submatrix (V, 0, a, V->size1, n_block);
          
          qrstep (&S_block.vector, &f_block.vector, &U_block.matrix, &V_block.matrix);
          
          /* remove any small off-diagonal elements */
          
          chop_small_elements (&S_block.vector, &f_block.vector);
        }
      }
  }
  /* Make singular values positive by reflections if necessary */
  
  for (j = 0; j < K; j++)
    {
      double Sj = gsl_vector_get (S, j);
      
      if (Sj < 0.0)
        {
          for (i = 0; i < N; i++)
            {
              double Vij = gsl_matrix_get (V, i, j);
              gsl_matrix_set (V, i, j, -Vij);
            }
          
          gsl_vector_set (S, j, -Sj);
        }
    }
  
  /* Sort singular values into decreasing order */
  
  for (i = 0; i < K; i++)
    {
      double S_max = gsl_vector_get (S, i);
      size_t i_max = i;
      
      for (j = i + 1; j < K; j++)
        {
          double Sj = gsl_vector_get (S, j);
          
          if (Sj > S_max)
            {
              S_max = Sj;
              i_max = j;
            }
        }
      
      if (i_max != i)
        {
          /* swap eigenvalues */
          gsl_vector_swap_elements (S, i, i_max);
          
          /* swap eigenvectors */
          gsl_matrix_swap_columns (A, i, i_max);
          gsl_matrix_swap_columns (V, i, i_max);
        }
    }
  
  return GSL_SUCCESS;
}


/* Modified algorithm which is better for M>>N */

int
gsl_linalg_SV_decomp_mod (gsl_matrix * A,
                          gsl_matrix * X,
                          gsl_matrix * V, gsl_vector * S, gsl_vector * work)
{
  size_t i, j;

  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M < N)
    {
      GSL_ERROR ("svd of MxN matrix, M<N, is not implemented", GSL_EUNIMPL);
    }
  else if (V->size1 != N)
    {
      GSL_ERROR ("square matrix V must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (V->size1 != V->size2)
    {
      GSL_ERROR ("matrix V must be square", GSL_ENOTSQR);
    }
  else if (X->size1 != N)
    {
      GSL_ERROR ("square matrix X must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (X->size1 != X->size2)
    {
      GSL_ERROR ("matrix X must be square", GSL_ENOTSQR);
    }
  else if (S->size != N)
    {
      GSL_ERROR ("length of vector S must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (work->size != N)
    {
      GSL_ERROR ("length of workspace must match second dimension of matrix A",
                 GSL_EBADLEN);
    }

  if (N == 1)
    {
      gsl_vector_view column = gsl_matrix_column (A, 0);
      double norm = gsl_blas_dnrm2 (&column.vector);

      gsl_vector_set (S, 0, norm); 
      gsl_matrix_set (V, 0, 0, 1.0);
      
      if (norm != 0.0)
        {
          gsl_blas_dscal (1.0/norm, &column.vector);
        }

      return GSL_SUCCESS;
    }

  /* Convert A into an upper triangular matrix R */

  for (i = 0; i < N; i++)
    {
      gsl_vector_view c = gsl_matrix_column (A, i);
      gsl_vector_view v = gsl_vector_subvector (&c.vector, i, M - i);
      double tau_i = gsl_linalg_householder_transform (&v.vector);

      /* Apply the transformation to the remaining columns */

      if (i + 1 < N)
        {
          gsl_matrix_view m =
            gsl_matrix_submatrix (A, i, i + 1, M - i, N - (i + 1));
          gsl_linalg_householder_hm (tau_i, &v.vector, &m.matrix);
        }

      gsl_vector_set (S, i, tau_i);
    }

  /* Copy the upper triangular part of A into X */

  for (i = 0; i < N; i++)
    {
      for (j = 0; j < i; j++)
        {
          gsl_matrix_set (X, i, j, 0.0);
        }

      {
        double Aii = gsl_matrix_get (A, i, i);
        gsl_matrix_set (X, i, i, Aii);
      }

      for (j = i + 1; j < N; j++)
        {
          double Aij = gsl_matrix_get (A, i, j);
          gsl_matrix_set (X, i, j, Aij);
        }
    }

  /* Convert A into an orthogonal matrix L */

  for (j = N; j > 0 && j--;)
    {
      /* Householder column transformation to accumulate L */
      double tj = gsl_vector_get (S, j);
      gsl_matrix_view m = gsl_matrix_submatrix (A, j, j, M - j, N - j);
      gsl_linalg_householder_hm1 (tj, &m.matrix);
    }

  /* unpack R into X V S */

  gsl_linalg_SV_decomp (X, V, S, work);

  /* Multiply L by X, to obtain U = L X, stored in U */

  {
    gsl_vector_view sum = gsl_vector_subvector (work, 0, N);

    for (i = 0; i < M; i++)
      {
        gsl_vector_view L_i = gsl_matrix_row (A, i);
        gsl_vector_set_zero (&sum.vector);

        for (j = 0; j < N; j++)
          {
            double Lij = gsl_vector_get (&L_i.vector, j);
            gsl_vector_view X_j = gsl_matrix_row (X, j);
            gsl_blas_daxpy (Lij, &X_j.vector, &sum.vector);
          }

        gsl_vector_memcpy (&L_i.vector, &sum.vector);
      }
  }

  return GSL_SUCCESS;
}


/*  Solves the system A x = b using the SVD factorization
 *
 *  A = U S V^T
 *
 *  to obtain x. For M x N systems it finds the solution in the least
 *  squares sense.  
 */

int
gsl_linalg_SV_solve (const gsl_matrix * U,
                     const gsl_matrix * V,
                     const gsl_vector * S,
                     const gsl_vector * b, gsl_vector * x)
{
  if (U->size1 != b->size)
    {
      GSL_ERROR ("first dimension of matrix U must size of vector b",
                 GSL_EBADLEN);
    }
  else if (U->size2 != S->size)
    {
      GSL_ERROR ("length of vector S must match second dimension of matrix U",
                 GSL_EBADLEN);
    }
  else if (V->size1 != V->size2)
    {
      GSL_ERROR ("matrix V must be square", GSL_ENOTSQR);
    }
  else if (S->size != V->size1)
    {
      GSL_ERROR ("length of vector S must match size of matrix V",
                 GSL_EBADLEN);
    }
  else if (V->size2 != x->size)
    {
      GSL_ERROR ("size of matrix V must match size of vector x", GSL_EBADLEN);
    }
  else
    {
      const size_t N = U->size2;
      size_t i;

      gsl_vector *w = gsl_vector_calloc (N);

      gsl_blas_dgemv (CblasTrans, 1.0, U, b, 0.0, w);

      for (i = 0; i < N; i++)
        {
          double wi = gsl_vector_get (w, i);
          double alpha = gsl_vector_get (S, i);
          if (alpha != 0)
            alpha = 1.0 / alpha;
          gsl_vector_set (w, i, alpha * wi);
        }

      gsl_blas_dgemv (CblasNoTrans, 1.0, V, w, 0.0, x);

      gsl_vector_free (w);

      return GSL_SUCCESS;
    }
}

/* This is a the jacobi version */
/* Author:  G. Jungman */

/*
 * Algorithm due to J.C. Nash, Compact Numerical Methods for
 * Computers (New York: Wiley and Sons, 1979), chapter 3.
 * See also Algorithm 4.1 in
 * James Demmel, Kresimir Veselic, "Jacobi's Method is more
 * accurate than QR", Lapack Working Note 15 (LAWN15), October 1989.
 * Available from netlib.
 *
 * Based on code by Arthur Kosowsky, Rutgers University
 *                  kosowsky@physics.rutgers.edu  
 *
 * Another relevant paper is, P.P.M. De Rijk, "A One-Sided Jacobi
 * Algorithm for computing the singular value decomposition on a
 * vector computer", SIAM Journal of Scientific and Statistical
 * Computing, Vol 10, No 2, pp 359-371, March 1989.
 * 
 */

int
gsl_linalg_SV_decomp_jacobi (gsl_matrix * A, gsl_matrix * Q, gsl_vector * S)
{
  if (A->size1 < A->size2)
    {
      /* FIXME: only implemented  M>=N case so far */

      GSL_ERROR ("svd of MxN matrix, M<N, is not implemented", GSL_EUNIMPL);
    }
  else if (Q->size1 != A->size2)
    {
      GSL_ERROR ("square matrix Q must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else if (Q->size1 != Q->size2)
    {
      GSL_ERROR ("matrix Q must be square", GSL_ENOTSQR);
    }
  else if (S->size != A->size2)
    {
      GSL_ERROR ("length of vector S must match second dimension of matrix A",
                 GSL_EBADLEN);
    }
  else
    {
      const size_t M = A->size1;
      const size_t N = A->size2;
      size_t i, j, k;

      /* Initialize the rotation counter and the sweep counter. */
      int count = 1;
      int sweep = 0;
      int sweepmax = 5*N;

      double tolerance = 10 * M * GSL_DBL_EPSILON;

      /* Always do at least 12 sweeps. */
      sweepmax = GSL_MAX (sweepmax, 12);

      /* Set Q to the identity matrix. */
      gsl_matrix_set_identity (Q);

      /* Store the column error estimates in S, for use during the
         orthogonalization */

      for (j = 0; j < N; j++)
        {
          gsl_vector_view cj = gsl_matrix_column (A, j);
          double sj = gsl_blas_dnrm2 (&cj.vector);
          gsl_vector_set(S, j, GSL_DBL_EPSILON * sj);
        }
    
      /* Orthogonalize A by plane rotations. */

      while (count > 0 && sweep <= sweepmax)
        {
          /* Initialize rotation counter. */
          count = N * (N - 1) / 2;

          for (j = 0; j < N - 1; j++)
            {
              for (k = j + 1; k < N; k++)
                {
                  double a = 0.0;
                  double b = 0.0;
                  double p = 0.0;
                  double q = 0.0;
                  double cosine, sine;
                  double v;
                  double abserr_a, abserr_b;
                  int sorted, orthog, noisya, noisyb;

                  gsl_vector_view cj = gsl_matrix_column (A, j);
                  gsl_vector_view ck = gsl_matrix_column (A, k);

                  gsl_blas_ddot (&cj.vector, &ck.vector, &p);
                  p *= 2.0 ;  /* equation 9a:  p = 2 x.y */

                  a = gsl_blas_dnrm2 (&cj.vector);
                  b = gsl_blas_dnrm2 (&ck.vector);

                  q = a * a - b * b;
                  v = hypot(p, q);

                  /* test for columns j,k orthogonal, or dominant errors */

                  abserr_a = gsl_vector_get(S,j);
                  abserr_b = gsl_vector_get(S,k);

                  sorted = (gsl_coerce_double(a) >= gsl_coerce_double(b));
                  orthog = (fabs (p) <= tolerance * gsl_coerce_double(a * b));
                  noisya = (a < abserr_a);
                  noisyb = (b < abserr_b);

                  if (sorted && (orthog || noisya || noisyb))
                    {
                      count--;
                      continue;
                    }

                  /* calculate rotation angles */
                  if (v == 0 || !sorted)
                    {
                      cosine = 0.0;
                      sine = 1.0;
                    }
                  else
                    {
                      cosine = sqrt((v + q) / (2.0 * v));
                      sine = p / (2.0 * v * cosine);
                    }

                  /* apply rotation to A */
                  for (i = 0; i < M; i++)
                    {
                      const double Aik = gsl_matrix_get (A, i, k);
                      const double Aij = gsl_matrix_get (A, i, j);
                      gsl_matrix_set (A, i, j, Aij * cosine + Aik * sine);
                      gsl_matrix_set (A, i, k, -Aij * sine + Aik * cosine);
                    }

                  gsl_vector_set(S, j, fabs(cosine) * abserr_a + fabs(sine) * abserr_b);
                  gsl_vector_set(S, k, fabs(sine) * abserr_a + fabs(cosine) * abserr_b);

                  /* apply rotation to Q */
                  for (i = 0; i < N; i++)
                    {
                      const double Qij = gsl_matrix_get (Q, i, j);
                      const double Qik = gsl_matrix_get (Q, i, k);
                      gsl_matrix_set (Q, i, j, Qij * cosine + Qik * sine);
                      gsl_matrix_set (Q, i, k, -Qij * sine + Qik * cosine);
                    }
                }
            }

          /* Sweep completed. */
          sweep++;
        }

      /* 
       * Orthogonalization complete. Compute singular values.
       */

      {
        double prev_norm = -1.0;

        for (j = 0; j < N; j++)
          {
            gsl_vector_view column = gsl_matrix_column (A, j);
            double norm = gsl_blas_dnrm2 (&column.vector);

            /* Determine if singular value is zero, according to the
               criteria used in the main loop above (i.e. comparison
               with norm of previous column). */

            if (norm == 0.0 || prev_norm == 0.0 
                || (j > 0 && norm <= tolerance * prev_norm))
              {
                gsl_vector_set (S, j, 0.0);     /* singular */
                gsl_vector_set_zero (&column.vector);   /* annihilate column */

                prev_norm = 0.0;
              }
            else
              {
                gsl_vector_set (S, j, norm);    /* non-singular */
                gsl_vector_scale (&column.vector, 1.0 / norm);  /* normalize column */

                prev_norm = norm;
              }
          }
      }

      if (count > 0)
        {
          /* reached sweep limit */
          GSL_ERROR ("Jacobi iterations did not reach desired tolerance",
                     GSL_ETOL);
        }

      return GSL_SUCCESS;
    }
}
