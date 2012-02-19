/* eigen/nonsymmv.c
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

#include "gsl_complex.h"
#include "gsl_complex_math.h"
#include "gsl_eigen.h"
#include "gsl_linalg.h"
#include "gsl_math.h"
#include "gsl_blas.h"
#include "gsl_cblas.h"
#include "gsl_vector.h"
#include "gsl_vector_complex.h"
#include "gsl_matrix.h"

/*
 * This module computes the eigenvalues and eigenvectors of a real
 * nonsymmetric matrix.
 * 
 * This file contains routines based on original code from LAPACK
 * which is distributed under the modified BSD license. The LAPACK
 * routines used are DTREVC and DLALN2.
 */

#define GSL_NONSYMMV_SMLNUM (2.0 * GSL_DBL_MIN)
#define GSL_NONSYMMV_BIGNUM ((1.0 - GSL_DBL_EPSILON) / GSL_NONSYMMV_SMLNUM)

static void nonsymmv_get_right_eigenvectors(gsl_matrix *T, gsl_matrix *Z,
                                            gsl_vector_complex *eval,
                                            gsl_matrix_complex *evec,
                                            gsl_eigen_nonsymmv_workspace *w);
static void nonsymmv_normalize_eigenvectors(gsl_vector_complex *eval,
                                            gsl_matrix_complex *evec);

/*
gsl_eigen_nonsymmv_alloc()

Allocate a workspace for solving the nonsymmetric eigenvalue problem.
The size of this workspace is O(5n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_nonsymmv_workspace *
gsl_eigen_nonsymmv_alloc(const size_t n)
{
  gsl_eigen_nonsymmv_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_nonsymmv_workspace *)
      calloc (1, sizeof (gsl_eigen_nonsymmv_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;
  w->Z = NULL;
  w->nonsymm_workspace_p = gsl_eigen_nonsymm_alloc(n);

  if (w->nonsymm_workspace_p == 0)
    {
      gsl_eigen_nonsymmv_free(w);
      GSL_ERROR_NULL ("failed to allocate space for nonsymm workspace", GSL_ENOMEM);
    }

  /*
   * set parameters to compute the full Schur form T and balance
   * the matrices
   */
  gsl_eigen_nonsymm_params(1, 1, w->nonsymm_workspace_p);

  w->work = gsl_vector_alloc(n);
  w->work2 = gsl_vector_alloc(n);
  w->work3 = gsl_vector_alloc(n);
  if (w->work == 0 || w->work2 == 0 || w->work3 == 0)
    {
      gsl_eigen_nonsymmv_free(w);
      GSL_ERROR_NULL ("failed to allocate space for nonsymmv additional workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_nonsymmv_alloc() */

/*
gsl_eigen_nonsymmv_free()
  Free workspace w
*/

void
gsl_eigen_nonsymmv_free (gsl_eigen_nonsymmv_workspace * w)
{
  if (w->nonsymm_workspace_p)
    gsl_eigen_nonsymm_free(w->nonsymm_workspace_p);

  if (w->work)
    gsl_vector_free(w->work);

  if (w->work2)
    gsl_vector_free(w->work2);

  if (w->work3)
    gsl_vector_free(w->work3);

  free(w);
} /* gsl_eigen_nonsymmv_free() */

/*
gsl_eigen_nonsymmv()

Solve the nonsymmetric eigensystem problem

A x = \lambda x

for the eigenvalues \lambda and right eigenvectors x

Inputs: A    - general real matrix
        eval - where to store eigenvalues
        evec - where to store eigenvectors
        w    - workspace

Return: success or error
*/

int
gsl_eigen_nonsymmv (gsl_matrix * A, gsl_vector_complex * eval,
                    gsl_matrix_complex * evec,
                    gsl_eigen_nonsymmv_workspace * w)
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
  else if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (evec->size1 != N)
    {
      GSL_ERROR ("eigenvector matrix has wrong size", GSL_EBADLEN);
    }
  else
    {
      int s;
      gsl_matrix Z;

      /*
       * We need a place to store the Schur vectors, so we will
       * treat evec as a real matrix and store them in the left
       * half - the factor of 2 in the tda corresponds to the
       * complex multiplicity
       */
      Z.size1 = N;
      Z.size2 = N;
      Z.tda = 2 * N;
      Z.data = evec->data;
      Z.block = 0;
      Z.owner = 0;

      /* compute eigenvalues, Schur form, and Schur vectors */
      s = gsl_eigen_nonsymm_Z(A, eval, &Z, w->nonsymm_workspace_p);

      if (w->Z)
        {
          /*
           * save the Schur vectors in user supplied matrix, since
           * they will be destroyed when computing eigenvectors
           */
          gsl_matrix_memcpy(w->Z, &Z);
        }

      /* only compute eigenvectors if we found all eigenvalues */
      if (s == GSL_SUCCESS)
        {
          /* compute eigenvectors */
          nonsymmv_get_right_eigenvectors(A, &Z, eval, evec, w);

          /* normalize so that Euclidean norm is 1 */
          nonsymmv_normalize_eigenvectors(eval, evec);
        }

      return s;
    }
} /* gsl_eigen_nonsymmv() */

/*
gsl_eigen_nonsymmv_Z()
  Compute eigenvalues and eigenvectors of a real nonsymmetric matrix
and also save the Schur vectors. See comments in gsl_eigen_nonsymm_Z
for more information.

Inputs: A    - real nonsymmetric matrix
        eval - where to store eigenvalues
        evec - where to store eigenvectors
        Z    - where to store Schur vectors
        w    - nonsymmv workspace

Return: success or error
*/

int
gsl_eigen_nonsymmv_Z (gsl_matrix * A, gsl_vector_complex * eval,
                      gsl_matrix_complex * evec, gsl_matrix * Z,
                      gsl_eigen_nonsymmv_workspace * w)
{
  /* check matrix and vector sizes */

  if (A->size1 != A->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues/eigenvectors", GSL_ENOTSQR);
    }
  else if (eval->size != A->size1)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (evec->size1 != A->size1)
    {
      GSL_ERROR ("eigenvector matrix has wrong size", GSL_EBADLEN);
    }
  else if ((Z->size1 != Z->size2) || (Z->size1 != A->size1))
    {
      GSL_ERROR ("Z matrix has wrong dimensions", GSL_EBADLEN);
    }
  else
    {
      int s;

      w->Z = Z;

      s = gsl_eigen_nonsymmv(A, eval, evec, w);

      w->Z = NULL;

      return s;
    }
} /* gsl_eigen_nonsymmv_Z() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
nonsymmv_get_right_eigenvectors()
  Compute the right eigenvectors of the Schur form T and then
backtransform them using the Schur vectors to get right eigenvectors of
the original matrix.

Inputs: T    - Schur form
        Z    - Schur vectors
        eval - where to store eigenvalues (to ensure that the
               correct eigenvalue is stored in the same position
               as the eigenvectors)
        evec - where to store eigenvectors
        w    - nonsymmv workspace

Return: none

Notes: 1) based on LAPACK routine DTREVC - the algorithm used is
          backsubstitution on the upper quasi triangular system T
          followed by backtransformation by Z to get vectors of the
          original matrix.

       2) The Schur vectors in Z are destroyed and replaced with
          eigenvectors stored with the same storage scheme as DTREVC.
          The eigenvectors are also stored in 'evec'

       3) The matrix T is unchanged on output

       4) Each eigenvector is normalized so that the element of
          largest magnitude has magnitude 1; here the magnitude of
          a complex number (x,y) is taken to be |x| + |y|
*/

static void
nonsymmv_get_right_eigenvectors(gsl_matrix *T, gsl_matrix *Z,
                                gsl_vector_complex *eval,
                                gsl_matrix_complex *evec,
                                gsl_eigen_nonsymmv_workspace *w)
{
  const size_t N = T->size1;
  const double smlnum = GSL_DBL_MIN * N / GSL_DBL_EPSILON;
  const double bignum = (1.0 - GSL_DBL_EPSILON) / smlnum;
  int i;              /* looping */
  size_t iu,          /* looping */
         ju,
         ii;
  gsl_complex lambda; /* current eigenvalue */
  double lambda_re,   /* Re(lambda) */
         lambda_im;   /* Im(lambda) */
  gsl_matrix_view Tv, /* temporary views */
                  Zv;
  gsl_vector_view y,  /* temporary views */
                  y2,
                  ev,
                  ev2;
  double dat[4],      /* scratch arrays */
         dat_X[4];
  double scale;       /* scale factor */
  double xnorm;       /* |X| */
  gsl_vector_complex_view ecol, /* column of evec */
                          ecol2;
  int complex_pair;   /* complex eigenvalue pair? */
  double smin;

  /*
   * Compute 1-norm of each column of upper triangular part of T
   * to control overflow in triangular solver
   */

  gsl_vector_set(w->work3, 0, 0.0);
  for (ju = 1; ju < N; ++ju)
    {
      gsl_vector_set(w->work3, ju, 0.0);
      for (iu = 0; iu < ju; ++iu)
        {
          gsl_vector_set(w->work3, ju,
                         gsl_vector_get(w->work3, ju) +
                         fabs(gsl_matrix_get(T, iu, ju)));
        }
    }

  for (i = (int) N - 1; i >= 0; --i)
    {
      iu = (size_t) i;

      /* get current eigenvalue and store it in lambda */
      lambda_re = gsl_matrix_get(T, iu, iu);

      if (iu != 0 && gsl_matrix_get(T, iu, iu - 1) != 0.0)
        {
          lambda_im = sqrt(fabs(gsl_matrix_get(T, iu, iu - 1))) *
                      sqrt(fabs(gsl_matrix_get(T, iu - 1, iu)));
        }
      else
        {
          lambda_im = 0.0;
        }

      GSL_SET_COMPLEX(&lambda, lambda_re, lambda_im);

      smin = GSL_MAX(GSL_DBL_EPSILON * (fabs(lambda_re) + fabs(lambda_im)),
                     smlnum);
      smin = GSL_MAX(smin, GSL_NONSYMMV_SMLNUM);

      if (lambda_im == 0.0)
        {
          int k, l;
          gsl_vector_view bv, xv;

          /* real eigenvector */

          /*
           * The ordering of eigenvalues in 'eval' is arbitrary and
           * does not necessarily follow the Schur form T, so store
           * lambda in the right slot in eval to ensure it corresponds
           * to the eigenvector we are about to compute
           */
          gsl_vector_complex_set(eval, iu, lambda);

          /*
           * We need to solve the system:
           *
           * (T(1:iu-1, 1:iu-1) - lambda*I)*X = -T(1:iu-1,iu)
           */

          /* construct right hand side */
          for (k = 0; k < i; ++k)
            {
              gsl_vector_set(w->work,
                             (size_t) k,
                             -gsl_matrix_get(T, (size_t) k, iu));
            }

          gsl_vector_set(w->work, iu, 1.0);

          for (l = i - 1; l >= 0; --l)
            {
              size_t lu = (size_t) l;

              if (lu == 0)
                complex_pair = 0;
              else
                complex_pair = gsl_matrix_get(T, lu, lu - 1) != 0.0;

              if (!complex_pair)
                {
                  double x;

                  /*
                   * 1-by-1 diagonal block - solve the system:
                   *
                   * (T_{ll} - lambda)*x = -T_{l(iu)}
                   */

                  Tv = gsl_matrix_submatrix(T, lu, lu, 1, 1);
                  bv = gsl_vector_view_array(dat, 1);
                  gsl_vector_set(&bv.vector, 0,
                                 gsl_vector_get(w->work, lu));
                  xv = gsl_vector_view_array(dat_X, 1);

                  gsl_schur_solve_equation(1.0,
                                           &Tv.matrix,
                                           lambda_re,
                                           1.0,
                                           1.0,
                                           &bv.vector,
                                           &xv.vector,
                                           &scale,
                                           &xnorm,
                                           smin);

                  /* scale x to avoid overflow */
                  x = gsl_vector_get(&xv.vector, 0);
                  if (xnorm > 1.0)
                    {
                      if (gsl_vector_get(w->work3, lu) > bignum / xnorm)
                        {
                          x /= xnorm;
                          scale /= xnorm;
                        }
                    }

                  if (scale != 1.0)
                    {
                      gsl_vector_view wv;

                      wv = gsl_vector_subvector(w->work, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                    }

                  gsl_vector_set(w->work, lu, x);

                  if (lu > 0)
                    {
                      gsl_vector_view v1, v2;

                      /* update right hand side */

                      v1 = gsl_matrix_subcolumn(T, lu, 0, lu);
                      v2 = gsl_vector_subvector(w->work, 0, lu);
                      gsl_blas_daxpy(-x, &v1.vector, &v2.vector);
                    } /* if (l > 0) */
                } /* if (!complex_pair) */
              else
                {
                  double x11, x21;

                  /*
                   * 2-by-2 diagonal block
                   */

                  Tv = gsl_matrix_submatrix(T, lu - 1, lu - 1, 2, 2);
                  bv = gsl_vector_view_array(dat, 2);
                  gsl_vector_set(&bv.vector, 0,
                                 gsl_vector_get(w->work, lu - 1));
                  gsl_vector_set(&bv.vector, 1,
                                 gsl_vector_get(w->work, lu));
                  xv = gsl_vector_view_array(dat_X, 2);

                  gsl_schur_solve_equation(1.0,
                                           &Tv.matrix,
                                           lambda_re,
                                           1.0,
                                           1.0,
                                           &bv.vector,
                                           &xv.vector,
                                           &scale,
                                           &xnorm,
                                           smin);

                  /* scale X(1,1) and X(2,1) to avoid overflow */
                  x11 = gsl_vector_get(&xv.vector, 0);
                  x21 = gsl_vector_get(&xv.vector, 1);

                  if (xnorm > 1.0)
                    {
                      double beta;

                      beta = GSL_MAX(gsl_vector_get(w->work3, lu - 1),
                                     gsl_vector_get(w->work3, lu));
                      if (beta > bignum / xnorm)
                        {
                          x11 /= xnorm;
                          x21 /= xnorm;
                          scale /= xnorm;
                        }
                    }

                  /* scale if necessary */
                  if (scale != 1.0)
                    {
                      gsl_vector_view wv;

                      wv = gsl_vector_subvector(w->work, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                    }

                  gsl_vector_set(w->work, lu - 1, x11);
                  gsl_vector_set(w->work, lu, x21);

                  /* update right hand side */
                  if (lu > 1)
                    {
                      gsl_vector_view v1, v2;

                      v1 = gsl_matrix_subcolumn(T, lu - 1, 0, lu - 1);
                      v2 = gsl_vector_subvector(w->work, 0, lu - 1);
                      gsl_blas_daxpy(-x11, &v1.vector, &v2.vector);

                      v1 = gsl_matrix_subcolumn(T, lu, 0, lu - 1);
                      gsl_blas_daxpy(-x21, &v1.vector, &v2.vector);
                    }

                  --l;
                } /* if (complex_pair) */
            } /* for (l = i - 1; l >= 0; --l) */

          /*
           * At this point, w->work is an eigenvector of the
           * Schur form T. To get an eigenvector of the original
           * matrix, we multiply on the left by Z, the matrix of
           * Schur vectors
           */

          ecol = gsl_matrix_complex_column(evec, iu);
          y = gsl_matrix_column(Z, iu);

          if (iu > 0)
            {
              gsl_vector_view x;

              Zv = gsl_matrix_submatrix(Z, 0, 0, N, iu);

              x = gsl_vector_subvector(w->work, 0, iu);

              /* compute Z * w->work and store it in Z(:,iu) */
              gsl_blas_dgemv(CblasNoTrans,
                             1.0,
                             &Zv.matrix,
                             &x.vector,
                             gsl_vector_get(w->work, iu),
                             &y.vector);
            } /* if (iu > 0) */

          /* store eigenvector into evec */

          ev = gsl_vector_complex_real(&ecol.vector);
          ev2 = gsl_vector_complex_imag(&ecol.vector);

          scale = 0.0;
          for (ii = 0; ii < N; ++ii)
            {
              double a = gsl_vector_get(&y.vector, ii);

              /* store real part of eigenvector */
              gsl_vector_set(&ev.vector, ii, a);

              /* set imaginary part to 0 */
              gsl_vector_set(&ev2.vector, ii, 0.0);

              if (fabs(a) > scale)
                scale = fabs(a);
            }

          if (scale != 0.0)
            scale = 1.0 / scale;

          /* scale by magnitude of largest element */
          gsl_blas_dscal(scale, &ev.vector);
        } /* if (GSL_IMAG(lambda) == 0.0) */
      else
        {
          gsl_vector_complex_view bv, xv;
          size_t k;
          int l;
          gsl_complex lambda2;

          /* complex eigenvector */

          /*
           * Store the complex conjugate eigenvalues in the right
           * slots in eval
           */
          GSL_SET_REAL(&lambda2, GSL_REAL(lambda));
          GSL_SET_IMAG(&lambda2, -GSL_IMAG(lambda));
          gsl_vector_complex_set(eval, iu - 1, lambda);
          gsl_vector_complex_set(eval, iu, lambda2);

          /*
           * First solve:
           *
           * [ T(i:i+1,i:i+1) - lambda*I ] * X = 0
           */

          if (fabs(gsl_matrix_get(T, iu - 1, iu)) >=
              fabs(gsl_matrix_get(T, iu, iu - 1)))
            {
              gsl_vector_set(w->work, iu - 1, 1.0);
              gsl_vector_set(w->work2, iu,
                             lambda_im / gsl_matrix_get(T, iu - 1, iu));
            }
          else
            {
              gsl_vector_set(w->work, iu - 1,
                             -lambda_im / gsl_matrix_get(T, iu, iu - 1));
              gsl_vector_set(w->work2, iu, 1.0);
            }
          gsl_vector_set(w->work, iu, 0.0);
          gsl_vector_set(w->work2, iu - 1, 0.0);

          /* construct right hand side */
          for (k = 0; k < iu - 1; ++k)
            {
              gsl_vector_set(w->work, k,
                             -gsl_vector_get(w->work, iu - 1) *
                             gsl_matrix_get(T, k, iu - 1));
              gsl_vector_set(w->work2, k,
                             -gsl_vector_get(w->work2, iu) *
                             gsl_matrix_get(T, k, iu));
            }

          /*
           * We must solve the upper quasi-triangular system:
           *
           * [ T(1:i-2,1:i-2) - lambda*I ] * X = s*(work + i*work2)
           */

          for (l = i - 2; l >= 0; --l)
            {
              size_t lu = (size_t) l;

              if (lu == 0)
                complex_pair = 0;
              else
                complex_pair = gsl_matrix_get(T, lu, lu - 1) != 0.0;

              if (!complex_pair)
                {
                  gsl_complex bval;
                  gsl_complex x;

                  /*
                   * 1-by-1 diagonal block - solve the system:
                   *
                   * (T_{ll} - lambda)*x = work + i*work2
                   */

                  Tv = gsl_matrix_submatrix(T, lu, lu, 1, 1);
                  bv = gsl_vector_complex_view_array(dat, 1);
                  xv = gsl_vector_complex_view_array(dat_X, 1);

                  GSL_SET_COMPLEX(&bval,
                                  gsl_vector_get(w->work, lu),
                                  gsl_vector_get(w->work2, lu));
                  gsl_vector_complex_set(&bv.vector, 0, bval);

                  gsl_schur_solve_equation_z(1.0,
                                             &Tv.matrix,
                                             &lambda,
                                             1.0,
                                             1.0,
                                             &bv.vector,
                                             &xv.vector,
                                             &scale,
                                             &xnorm,
                                             smin);

                  if (xnorm > 1.0)
                    {
                      if (gsl_vector_get(w->work3, lu) > bignum / xnorm)
                        {
                          gsl_blas_zdscal(1.0/xnorm, &xv.vector);
                          scale /= xnorm;
                        }
                    }

                  /* scale if necessary */
                  if (scale != 1.0)
                    {
                      gsl_vector_view wv;

                      wv = gsl_vector_subvector(w->work, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                      wv = gsl_vector_subvector(w->work2, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                    }

                  x = gsl_vector_complex_get(&xv.vector, 0);
                  gsl_vector_set(w->work, lu, GSL_REAL(x));
                  gsl_vector_set(w->work2, lu, GSL_IMAG(x));

                  /* update the right hand side */
                  if (lu > 0)
                    {
                      gsl_vector_view v1, v2;

                      v1 = gsl_matrix_subcolumn(T, lu, 0, lu);
                      v2 = gsl_vector_subvector(w->work, 0, lu);
                      gsl_blas_daxpy(-GSL_REAL(x), &v1.vector, &v2.vector);

                      v2 = gsl_vector_subvector(w->work2, 0, lu);
                      gsl_blas_daxpy(-GSL_IMAG(x), &v1.vector, &v2.vector);
                    } /* if (lu > 0) */
                } /* if (!complex_pair) */
              else
                {
                  gsl_complex b1, b2, x1, x2;

                  /*
                   * 2-by-2 diagonal block - solve the system
                   */

                  Tv = gsl_matrix_submatrix(T, lu - 1, lu - 1, 2, 2);
                  bv = gsl_vector_complex_view_array(dat, 2);
                  xv = gsl_vector_complex_view_array(dat_X, 2);

                  GSL_SET_COMPLEX(&b1,
                                  gsl_vector_get(w->work, lu - 1),
                                  gsl_vector_get(w->work2, lu - 1));
                  GSL_SET_COMPLEX(&b2,
                                  gsl_vector_get(w->work, lu),
                                  gsl_vector_get(w->work2, lu));
                  gsl_vector_complex_set(&bv.vector, 0, b1);
                  gsl_vector_complex_set(&bv.vector, 1, b2);

                  gsl_schur_solve_equation_z(1.0,
                                             &Tv.matrix,
                                             &lambda,
                                             1.0,
                                             1.0,
                                             &bv.vector,
                                             &xv.vector,
                                             &scale,
                                             &xnorm,
                                             smin);

                  x1 = gsl_vector_complex_get(&xv.vector, 0);
                  x2 = gsl_vector_complex_get(&xv.vector, 1);

                  if (xnorm > 1.0)
                    {
                      double beta;

                      beta = GSL_MAX(gsl_vector_get(w->work3, lu - 1),
                                     gsl_vector_get(w->work3, lu));
                      if (beta > bignum / xnorm)
                        {
                          gsl_blas_zdscal(1.0/xnorm, &xv.vector);
                          scale /= xnorm;
                        }
                    }

                  /* scale if necessary */
                  if (scale != 1.0)
                    {
                      gsl_vector_view wv;

                      wv = gsl_vector_subvector(w->work, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                      wv = gsl_vector_subvector(w->work2, 0, iu + 1);
                      gsl_blas_dscal(scale, &wv.vector);
                    }
                  gsl_vector_set(w->work, lu - 1, GSL_REAL(x1));
                  gsl_vector_set(w->work, lu, GSL_REAL(x2));
                  gsl_vector_set(w->work2, lu - 1, GSL_IMAG(x1));
                  gsl_vector_set(w->work2, lu, GSL_IMAG(x2));

                  /* update right hand side */
                  if (lu > 1)
                    {
                      gsl_vector_view v1, v2, v3, v4;

                      v1 = gsl_matrix_subcolumn(T, lu - 1, 0, lu - 1);
                      v4 = gsl_matrix_subcolumn(T, lu, 0, lu - 1);
                      v2 = gsl_vector_subvector(w->work, 0, lu - 1);
                      v3 = gsl_vector_subvector(w->work2, 0, lu - 1);

                      gsl_blas_daxpy(-GSL_REAL(x1), &v1.vector, &v2.vector);
                      gsl_blas_daxpy(-GSL_REAL(x2), &v4.vector, &v2.vector);
                      gsl_blas_daxpy(-GSL_IMAG(x1), &v1.vector, &v3.vector);
                      gsl_blas_daxpy(-GSL_IMAG(x2), &v4.vector, &v3.vector);
                    } /* if (lu > 1) */

                  --l;
                } /* if (complex_pair) */
            } /* for (l = i - 2; l >= 0; --l) */

          /*
           * At this point, work + i*work2 is an eigenvector
           * of T - backtransform to get an eigenvector of the
           * original matrix
           */

          y = gsl_matrix_column(Z, iu - 1);
          y2 = gsl_matrix_column(Z, iu);

          if (iu > 1)
            {
              gsl_vector_view x;

              /* compute real part of eigenvectors */

              Zv = gsl_matrix_submatrix(Z, 0, 0, N, iu - 1);
              x = gsl_vector_subvector(w->work, 0, iu - 1);

              gsl_blas_dgemv(CblasNoTrans,
                             1.0,
                             &Zv.matrix,
                             &x.vector,
                             gsl_vector_get(w->work, iu - 1),
                             &y.vector);


              /* now compute the imaginary part */
              x = gsl_vector_subvector(w->work2, 0, iu - 1);

              gsl_blas_dgemv(CblasNoTrans,
                             1.0,
                             &Zv.matrix,
                             &x.vector,
                             gsl_vector_get(w->work2, iu),
                             &y2.vector);
            }
          else
            {
              gsl_blas_dscal(gsl_vector_get(w->work, iu - 1), &y.vector);
              gsl_blas_dscal(gsl_vector_get(w->work2, iu), &y2.vector);
            }

          /*
           * Now store the eigenvectors into evec - the real parts
           * are Z(:,iu - 1) and the imaginary parts are
           * +/- Z(:,iu)
           */

          /* get views of the two eigenvector slots */
          ecol = gsl_matrix_complex_column(evec, iu - 1);
          ecol2 = gsl_matrix_complex_column(evec, iu);

          /*
           * save imaginary part first as it may get overwritten
           * when copying the real part due to our storage scheme
           * in Z/evec
           */
          ev = gsl_vector_complex_imag(&ecol.vector);
          ev2 = gsl_vector_complex_imag(&ecol2.vector);
          scale = 0.0;
          for (ii = 0; ii < N; ++ii)
            {
              double a = gsl_vector_get(&y2.vector, ii);

              scale = GSL_MAX(scale,
                              fabs(a) + fabs(gsl_vector_get(&y.vector, ii)));

              gsl_vector_set(&ev.vector, ii, a);
              gsl_vector_set(&ev2.vector, ii, -a);
            }

          /* now save the real part */
          ev = gsl_vector_complex_real(&ecol.vector);
          ev2 = gsl_vector_complex_real(&ecol2.vector);
          for (ii = 0; ii < N; ++ii)
            {
              double a = gsl_vector_get(&y.vector, ii);

              gsl_vector_set(&ev.vector, ii, a);
              gsl_vector_set(&ev2.vector, ii, a);
            }

          if (scale != 0.0)
            scale = 1.0 / scale;

          /* scale by largest element magnitude */

          gsl_blas_zdscal(scale, &ecol.vector);
          gsl_blas_zdscal(scale, &ecol2.vector);

          /*
           * decrement i since we took care of two eigenvalues at
           * the same time
           */
          --i;
        } /* if (GSL_IMAG(lambda) != 0.0) */
    } /* for (i = (int) N - 1; i >= 0; --i) */
} /* nonsymmv_get_right_eigenvectors() */

/*
nonsymmv_normalize_eigenvectors()
  Normalize eigenvectors so that their Euclidean norm is 1

Inputs: eval - eigenvalues
        evec - eigenvectors
*/

static void
nonsymmv_normalize_eigenvectors(gsl_vector_complex *eval,
                                gsl_matrix_complex *evec)
{
  const size_t N = evec->size1;
  size_t i;     /* looping */
  gsl_complex ei;
  gsl_vector_complex_view vi;
  gsl_vector_view re, im;
  double scale; /* scaling factor */

  for (i = 0; i < N; ++i)
    {
      ei = gsl_vector_complex_get(eval, i);
      vi = gsl_matrix_complex_column(evec, i);

      re = gsl_vector_complex_real(&vi.vector);

      if (GSL_IMAG(ei) == 0.0)
        {
          scale = 1.0 / gsl_blas_dnrm2(&re.vector);
          gsl_blas_dscal(scale, &re.vector);
        }
      else if (GSL_IMAG(ei) > 0.0)
        {
          im = gsl_vector_complex_imag(&vi.vector);

          scale = 1.0 / gsl_hypot(gsl_blas_dnrm2(&re.vector),
                                  gsl_blas_dnrm2(&im.vector));
          gsl_blas_zdscal(scale, &vi.vector);

          vi = gsl_matrix_complex_column(evec, i + 1);
          gsl_blas_zdscal(scale, &vi.vector);
        }
    }
} /* nonsymmv_normalize_eigenvectors() */
