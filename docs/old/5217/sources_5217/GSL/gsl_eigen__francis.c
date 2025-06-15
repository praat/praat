/* eigen/francis.c
 * 
 * Copyright (C) 2006, 2007 Patrick Alken
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
#include "gsl_cblas.h"
#include "gsl_complex.h"
#include "gsl_complex_math.h"

/*
 * This module computes the eigenvalues of a real upper hessenberg
 * matrix, using the classical double shift Francis QR algorithm.
 * It will also optionally compute the full Schur form and matrix of
 * Schur vectors.
 *
 * See Golub & Van Loan, "Matrix Computations" (3rd ed),
 * algorithm 7.5.2
 */

/* exceptional shift coefficients - these values are from LAPACK DLAHQR */
#define GSL_FRANCIS_COEFF1        (0.75)
#define GSL_FRANCIS_COEFF2        (-0.4375)

static inline void francis_schur_decomp(gsl_matrix * H,
                                        gsl_vector_complex * eval,
                                        gsl_eigen_francis_workspace * w);
static inline size_t francis_search_subdiag_small_elements(gsl_matrix * A);
static inline int francis_qrstep(gsl_matrix * H,
                                 gsl_eigen_francis_workspace * w);
static inline void francis_schur_standardize(gsl_matrix *A,
                                             gsl_complex *eval1,
                                             gsl_complex *eval2,
                                             gsl_eigen_francis_workspace *w);
static inline size_t francis_get_submatrix(gsl_matrix *A, gsl_matrix *B);
static void francis_standard_form(gsl_matrix *A, double *cs, double *sn);

/*
gsl_eigen_francis_alloc()

Allocate a workspace for solving the nonsymmetric eigenvalue problem.
The size of this workspace is O(1)

Inputs: none

Return: pointer to workspace
*/

gsl_eigen_francis_workspace *
gsl_eigen_francis_alloc(void)
{
  gsl_eigen_francis_workspace *w;

  w = (gsl_eigen_francis_workspace *)
      calloc (1, sizeof (gsl_eigen_francis_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  /* these are filled in later */

  w->size = 0;
  w->max_iterations = 0;
  w->n_iter = 0;
  w->n_evals = 0;

  w->compute_t = 0;
  w->Z = NULL;
  w->H = NULL;

  return (w);
} /* gsl_eigen_francis_alloc() */

/*
gsl_eigen_francis_free()
  Free francis workspace w
*/

void
gsl_eigen_francis_free (gsl_eigen_francis_workspace *w)
{
  free(w);
} /* gsl_eigen_francis_free() */

/*
gsl_eigen_francis_T()
  Called when we want to compute the Schur form T, or no longer
compute the Schur form T

Inputs: compute_t - 1 to compute T, 0 to not compute T
        w         - francis workspace
*/

void
gsl_eigen_francis_T (const int compute_t, gsl_eigen_francis_workspace *w)
{
  w->compute_t = compute_t;
}

/*
gsl_eigen_francis()

Solve the nonsymmetric eigenvalue problem

H x = \lambda x

for the eigenvalues \lambda using algorithm 7.5.2 of
Golub & Van Loan, "Matrix Computations" (3rd ed)

Inputs: H    - upper hessenberg matrix
        eval - where to store eigenvalues
        w    - workspace

Return: success or error - if error code is returned,
        then the QR procedure did not converge in the
        allowed number of iterations. In the event of non-
        convergence, the number of eigenvalues found will
        still be stored in the beginning of eval,

Notes: On output, the diagonal of H contains 1-by-1 or 2-by-2
       blocks containing the eigenvalues. If T is desired,
       H will contain the full Schur form on output.
*/

int
gsl_eigen_francis (gsl_matrix * H, gsl_vector_complex * eval,
                   gsl_eigen_francis_workspace * w)
{
  /* check matrix and vector sizes */

  if (H->size1 != H->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues", GSL_ENOTSQR);
    }
  else if (eval->size != H->size1)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else
    {
      const size_t N = H->size1;
      int j;

      /*
       * Set internal parameters which depend on matrix size.
       * The Francis solver can be called with any size matrix
       * since the workspace does not depend on N.
       * Furthermore, multishift solvers which call the Francis
       * solver may need to call it with different sized matrices
       */
      w->size = N;
      w->max_iterations = 30 * N;

      /*
       * save a pointer to original matrix since francis_schur_decomp
       * is recursive
       */
      w->H = H;

      w->n_iter = 0;
      w->n_evals = 0;

      /*
       * zero out the first two subdiagonals (below the main subdiagonal)
       * needed as scratch space by the QR sweep routine
       */
      for (j = 0; j < (int) N - 3; ++j)
        {
          gsl_matrix_set(H, (size_t) j + 2, (size_t) j, 0.0);
          gsl_matrix_set(H, (size_t) j + 3, (size_t) j, 0.0);
        }

      if (N > 2)
        gsl_matrix_set(H, N - 1, N - 3, 0.0);

      /*
       * compute Schur decomposition of H and store eigenvalues
       * into eval
       */
      francis_schur_decomp(H, eval, w);

      if (w->n_evals != N)
        return GSL_EMAXITER;

      return GSL_SUCCESS;
    }
} /* gsl_eigen_francis() */

/*
gsl_eigen_francis_Z()

Solve the nonsymmetric eigenvalue problem for a Hessenberg
matrix

H x = \lambda x

for the eigenvalues \lambda using the Francis double-shift
method.

Here we compute the real Schur form

T = Q^t H Q

with the diagonal blocks of T giving us the eigenvalues.
Q is the matrix of Schur vectors.

Originally, H was obtained from a general nonsymmetric matrix
A via a transformation

H = U^t A U

so that

T = (UQ)^t A (UQ) = Z^t A Z

Z is the matrix of Schur vectors computed by this algorithm

Inputs: H    - upper hessenberg matrix
        eval - where to store eigenvalues
        Z    - where to store Schur vectors
        w    - workspace

Notes: 1) If T is computed, it is stored in H on output. Otherwise,
          the diagonal of H will contain 1-by-1 and 2-by-2 blocks
          containing the eigenvalues.

       2) The matrix Z must be initialized to the Hessenberg
          similarity matrix U. Or if you want the eigenvalues
          of H, initialize Z to the identity matrix.
*/

int
gsl_eigen_francis_Z (gsl_matrix * H, gsl_vector_complex * eval,
                     gsl_matrix * Z, gsl_eigen_francis_workspace * w)
{
  int s;

  /* set internal Z pointer so we know to accumulate transformations */
  w->Z = Z;

  s = gsl_eigen_francis(H, eval, w);

  w->Z = NULL;

  return s;
} /* gsl_eigen_francis_Z() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
francis_schur_decomp()
  Compute the Schur decomposition of the matrix H

Inputs: H     - hessenberg matrix
        eval  - where to store eigenvalues
        w     - workspace

Return: none
*/

static inline void
francis_schur_decomp(gsl_matrix * H, gsl_vector_complex * eval,
                     gsl_eigen_francis_workspace * w)
{
  gsl_matrix_view m;   /* active matrix we are working on */
  size_t N;            /* size of matrix */
  size_t q;            /* index of small subdiagonal element */
  gsl_complex lambda1, /* eigenvalues */
              lambda2;

  N = H->size1;
  m = gsl_matrix_submatrix(H, 0, 0, N, N);

  while ((N > 2) && ((w->n_iter)++ < w->max_iterations))
    {
      q = francis_search_subdiag_small_elements(&m.matrix);

      if (q == 0)
        {
          /*
           * no small subdiagonal element found - perform a QR
           * sweep on the active reduced hessenberg matrix
           */
          francis_qrstep(&m.matrix, w);
          continue;
        }

      /*
       * a small subdiagonal element was found - one or two eigenvalues
       * have converged or the matrix has split into two smaller matrices
       */

      if (q == (N - 1))
        {
          /*
           * the last subdiagonal element of the matrix is 0 -
           * m_{NN} is a real eigenvalue
           */
          GSL_SET_COMPLEX(&lambda1,
                          gsl_matrix_get(&m.matrix, q, q), 0.0);
          gsl_vector_complex_set(eval, w->n_evals, lambda1);
          w->n_evals += 1;
          w->n_iter = 0;

          --N;
          m = gsl_matrix_submatrix(&m.matrix, 0, 0, N, N);
        }
      else if (q == (N - 2))
        {
          gsl_matrix_view v;

          /*
           * The bottom right 2-by-2 block of m is an eigenvalue
           * system
           */

          v = gsl_matrix_submatrix(&m.matrix, q, q, 2, 2);
          francis_schur_standardize(&v.matrix, &lambda1, &lambda2, w);

          gsl_vector_complex_set(eval, w->n_evals, lambda1);
          gsl_vector_complex_set(eval, w->n_evals + 1, lambda2);
          w->n_evals += 2;
          w->n_iter = 0;

          N -= 2;
          m = gsl_matrix_submatrix(&m.matrix, 0, 0, N, N);
        }
      else if (q == 1)
        {
          /* the first matrix element is an eigenvalue */
          GSL_SET_COMPLEX(&lambda1,
                          gsl_matrix_get(&m.matrix, 0, 0), 0.0);
          gsl_vector_complex_set(eval, w->n_evals, lambda1);
          w->n_evals += 1;
          w->n_iter = 0;

          --N;
          m = gsl_matrix_submatrix(&m.matrix, 1, 1, N, N);
        }
      else if (q == 2)
        {
          gsl_matrix_view v;

          /* the upper left 2-by-2 block is an eigenvalue system */

          v = gsl_matrix_submatrix(&m.matrix, 0, 0, 2, 2);
          francis_schur_standardize(&v.matrix, &lambda1, &lambda2, w);

          gsl_vector_complex_set(eval, w->n_evals, lambda1);
          gsl_vector_complex_set(eval, w->n_evals + 1, lambda2);
          w->n_evals += 2;
          w->n_iter = 0;

          N -= 2;
          m = gsl_matrix_submatrix(&m.matrix, 2, 2, N, N);
        }
      else
        {
          gsl_matrix_view v;

          /*
           * There is a zero element on the subdiagonal somewhere
           * in the middle of the matrix - we can now operate
           * separately on the two submatrices split by this
           * element. q is the row index of the zero element.
           */

          /* operate on lower right (N - q)-by-(N - q) block first */
          v = gsl_matrix_submatrix(&m.matrix, q, q, N - q, N - q);
          francis_schur_decomp(&v.matrix, eval, w);

          /* operate on upper left q-by-q block */
          v = gsl_matrix_submatrix(&m.matrix, 0, 0, q, q);
          francis_schur_decomp(&v.matrix, eval, w);

          N = 0;
        }
    }

  /* handle special cases of N = 1 or 2 */

  if (N == 1)
    {
      GSL_SET_COMPLEX(&lambda1, gsl_matrix_get(&m.matrix, 0, 0), 0.0);
      gsl_vector_complex_set(eval, w->n_evals, lambda1);
      w->n_evals += 1;
      w->n_iter = 0;
    }
  else if (N == 2)
    {
      francis_schur_standardize(&m.matrix, &lambda1, &lambda2, w);
      gsl_vector_complex_set(eval, w->n_evals, lambda1);
      gsl_vector_complex_set(eval, w->n_evals + 1, lambda2);
      w->n_evals += 2;
      w->n_iter = 0;
    }
} /* francis_schur_decomp() */

/*
francis_qrstep()
  Perform a Francis QR step.

See Golub & Van Loan, "Matrix Computations" (3rd ed),
algorithm 7.5.1

Inputs: H - upper Hessenberg matrix
        w - workspace

Notes: The matrix H must be "reduced", ie: have no tiny subdiagonal
       elements. When computing the first householder reflection,
       we divide by H_{21} so it is necessary that this element
       is not zero. When a subdiagonal element becomes negligible,
       the calling function should call this routine with the
       submatrices split by that element, so that we don't divide
       by zeros.
*/

static inline int
francis_qrstep(gsl_matrix * H, gsl_eigen_francis_workspace * w)
{
  const size_t N = H->size1;
  size_t i;        /* looping */
  gsl_matrix_view m;
  double tau_i;    /* householder coefficient */
  double dat[3];   /* householder vector */
  double scale;    /* scale factor to avoid overflow */
  gsl_vector_view v2, v3;
  size_t q, r;
  size_t top = 0;  /* location of H in original matrix */
  double s,
         disc;
  double h_nn,     /* H(n,n) */
         h_nm1nm1, /* H(n-1,n-1) */
         h_cross,  /* H(n,n-1) * H(n-1,n) */
         h_tmp1,
         h_tmp2;

  v2 = gsl_vector_view_array(dat, 2);
  v3 = gsl_vector_view_array(dat, 3);

  if ((w->n_iter % 10) == 0)
    {
      /*
       * exceptional shifts: we have gone 10 iterations
       * without finding a new eigenvalue, try a new choice of shifts.
       * See LAPACK routine DLAHQR
       */
      s = fabs(gsl_matrix_get(H, N - 1, N - 2)) +
          fabs(gsl_matrix_get(H, N - 2, N - 3));
      h_nn = gsl_matrix_get(H, N - 1, N - 1) + GSL_FRANCIS_COEFF1 * s;
      h_nm1nm1 = h_nn;
      h_cross = GSL_FRANCIS_COEFF2 * s * s;
    }
  else
    {
      /*
       * normal shifts - compute Rayleigh quotient and use
       * Wilkinson shift if possible
       */

      h_nn = gsl_matrix_get(H, N - 1, N - 1);
      h_nm1nm1 = gsl_matrix_get(H, N - 2, N - 2);
      h_cross = gsl_matrix_get(H, N - 1, N - 2) *
                gsl_matrix_get(H, N - 2, N - 1);

      disc = 0.5 * (h_nm1nm1 - h_nn);
      disc = disc * disc + h_cross;
      if (disc > 0.0)
        {
          double ave;

          /* real roots - use Wilkinson's shift twice */
          disc = sqrt(disc);
          ave = 0.5 * (h_nm1nm1 + h_nn);
          if (fabs(h_nm1nm1) - fabs(h_nn) > 0.0)
            {
              h_nm1nm1 = h_nm1nm1 * h_nn - h_cross;
              h_nn = h_nm1nm1 / (disc * GSL_SIGN(ave) + ave);
            }
          else
            {
              h_nn = disc * GSL_SIGN(ave) + ave;
            }

          h_nm1nm1 = h_nn;
          h_cross = 0.0;
        }
    }

  h_tmp1 = h_nm1nm1 - gsl_matrix_get(H, 0, 0);
  h_tmp2 = h_nn - gsl_matrix_get(H, 0, 0);

  /*
   * These formulas are equivalent to those in Golub & Van Loan
   * for the normal shift case - the terms have been rearranged
   * to reduce possible roundoff error when subdiagonal elements
   * are small
   */

  dat[0] = (h_tmp1*h_tmp2 - h_cross) / gsl_matrix_get(H, 1, 0) +
           gsl_matrix_get(H, 0, 1);
  dat[1] = gsl_matrix_get(H, 1, 1) - gsl_matrix_get(H, 0, 0) - h_tmp1 -
           h_tmp2;
  dat[2] = gsl_matrix_get(H, 2, 1);

  scale = fabs(dat[0]) + fabs(dat[1]) + fabs(dat[2]);
  if (scale != 0.0)
    {
      /* scale to prevent overflow or underflow */
      dat[0] /= scale;
      dat[1] /= scale;
      dat[2] /= scale;
    }

  if (w->Z || w->compute_t)
    {
      /*
       * get absolute indices of this (sub)matrix relative to the
       * original Hessenberg matrix
       */
      top = francis_get_submatrix(w->H, H);
    }

  for (i = 0; i < N - 2; ++i)
    {
      tau_i = gsl_linalg_householder_transform(&v3.vector);

      if (tau_i != 0.0)
        {
          /* q = max(1, i - 1) */
          q = (1 > ((int)i - 1)) ? 0 : (i - 1);

          /* r = min(i + 3, N - 1) */
          r = ((i + 3) < (N - 1)) ? (i + 3) : (N - 1);

          if (w->compute_t)
            {
              /*
               * We are computing the Schur form T, so we
               * need to transform the whole matrix H
               *
               * H -> P_k^t H P_k
               *
               * where P_k is the current Householder matrix
               */

              /* apply left householder matrix (I - tau_i v v') to H */
              m = gsl_matrix_submatrix(w->H,
                                       top + i,
                                       top + q,
                                       3,
                                       w->size - top - q);
              gsl_linalg_householder_hm(tau_i, &v3.vector, &m.matrix);

              /* apply right householder matrix (I - tau_i v v') to H */
              m = gsl_matrix_submatrix(w->H,
                                       0,
                                       top + i,
                                       top + r + 1,
                                       3);
              gsl_linalg_householder_mh(tau_i, &v3.vector, &m.matrix);
            }
          else
            {
              /*
               * We are not computing the Schur form T, so we
               * only need to transform the active block
               */

              /* apply left householder matrix (I - tau_i v v') to H */
              m = gsl_matrix_submatrix(H, i, q, 3, N - q);
              gsl_linalg_householder_hm(tau_i, &v3.vector, &m.matrix);

              /* apply right householder matrix (I - tau_i v v') to H */
              m = gsl_matrix_submatrix(H, 0, i, r + 1, 3);
              gsl_linalg_householder_mh(tau_i, &v3.vector, &m.matrix);
            }

          if (w->Z)
            {
              /* accumulate the similarity transformation into Z */
              m = gsl_matrix_submatrix(w->Z, 0, top + i, w->size, 3);
              gsl_linalg_householder_mh(tau_i, &v3.vector, &m.matrix);
            }
        } /* if (tau_i != 0.0) */

      dat[0] = gsl_matrix_get(H, i + 1, i);
      dat[1] = gsl_matrix_get(H, i + 2, i);
      if (i < (N - 3))
        {
          dat[2] = gsl_matrix_get(H, i + 3, i);
        }

      scale = fabs(dat[0]) + fabs(dat[1]) + fabs(dat[2]);
      if (scale != 0.0)
        {
          /* scale to prevent overflow or underflow */
          dat[0] /= scale;
          dat[1] /= scale;
          dat[2] /= scale;
        }
    } /* for (i = 0; i < N - 2; ++i) */

  scale = fabs(dat[0]) + fabs(dat[1]);
  if (scale != 0.0)
    {
      /* scale to prevent overflow or underflow */
      dat[0] /= scale;
      dat[1] /= scale;
    }

  tau_i = gsl_linalg_householder_transform(&v2.vector);

  if (w->compute_t)
    {
      m = gsl_matrix_submatrix(w->H,
                               top + N - 2,
                               top + N - 3,
                               2,
                               w->size - top - N + 3);
      gsl_linalg_householder_hm(tau_i, &v2.vector, &m.matrix);

      m = gsl_matrix_submatrix(w->H,
                               0,
                               top + N - 2,
                               top + N,
                               2);
      gsl_linalg_householder_mh(tau_i, &v2.vector, &m.matrix);
    }
  else
    {
      m = gsl_matrix_submatrix(H, N - 2, N - 3, 2, 3);
      gsl_linalg_householder_hm(tau_i, &v2.vector, &m.matrix);

      m = gsl_matrix_submatrix(H, 0, N - 2, N, 2);
      gsl_linalg_householder_mh(tau_i, &v2.vector, &m.matrix);
    }

  if (w->Z)
    {
      /* accumulate transformation into Z */
      m = gsl_matrix_submatrix(w->Z, 0, top + N - 2, w->size, 2);
      gsl_linalg_householder_mh(tau_i, &v2.vector, &m.matrix);
    }

  return GSL_SUCCESS;
} /* francis_qrstep() */

/*
francis_search_subdiag_small_elements()
  Search for a small subdiagonal element starting from the bottom
of a matrix A. A small element is one that satisfies:

|A_{i,i-1}| <= eps * (|A_{i,i}| + |A_{i-1,i-1}|)

Inputs: A - matrix (must be at least 3-by-3)

Return: row index of small subdiagonal element or 0 if not found

Notes: the first small element that is found (starting from bottom)
       is set to zero
*/

static inline size_t
francis_search_subdiag_small_elements(gsl_matrix * A)
{
  const size_t N = A->size1;
  size_t i;
  double dpel = gsl_matrix_get(A, N - 2, N - 2);

  for (i = N - 1; i > 0; --i)
    {
      double sel = gsl_matrix_get(A, i, i - 1);
      double del = gsl_matrix_get(A, i, i);

      if ((sel == 0.0) ||
          (fabs(sel) < GSL_DBL_EPSILON * (fabs(del) + fabs(dpel))))
        {
          gsl_matrix_set(A, i, i - 1, 0.0);
          return (i);
        }

      dpel = del;
    }

  return (0);
} /* francis_search_subdiag_small_elements() */

/*
francis_schur_standardize()
  Convert a 2-by-2 diagonal block in the Schur form to standard form
and update the rest of T and Z matrices if required.

Inputs: A     - 2-by-2 matrix
        eval1 - where to store eigenvalue 1
        eval2 - where to store eigenvalue 2
        w     - francis workspace
*/

static inline void
francis_schur_standardize(gsl_matrix *A, gsl_complex *eval1,
                          gsl_complex *eval2,
                          gsl_eigen_francis_workspace *w)
{
  const size_t N = w->size;
  double cs, sn;
  size_t top;

  /*
   * figure out where the submatrix A resides in the
   * original matrix H
   */
  top = francis_get_submatrix(w->H, A);

  /* convert 2-by-2 block to standard form */
  francis_standard_form(A, &cs, &sn);

  /* set eigenvalues */

  GSL_SET_REAL(eval1, gsl_matrix_get(A, 0, 0));
  GSL_SET_REAL(eval2, gsl_matrix_get(A, 1, 1));
  if (gsl_matrix_get(A, 1, 0) == 0.0)
    {
      GSL_SET_IMAG(eval1, 0.0);
      GSL_SET_IMAG(eval2, 0.0);
    }
  else
    {
      double tmp = sqrt(fabs(gsl_matrix_get(A, 0, 1)) *
                        fabs(gsl_matrix_get(A, 1, 0)));
      GSL_SET_IMAG(eval1, tmp);
      GSL_SET_IMAG(eval2, -tmp);
    }

  if (w->compute_t)
    {
      gsl_vector_view xv, yv;

      /*
       * The above call to francis_standard_form transformed a 2-by-2 block
       * of T into upper triangular form via the transformation
       *
       * U = [ CS -SN ]
       *     [ SN  CS ]
       *
       * The original matrix T was
       *
       * T = [ T_{11} | T_{12} | T_{13} ]
       *     [   0*   |   A    | T_{23} ]
       *     [   0    |   0*   | T_{33} ]
       *
       * where 0* indicates all zeros except for possibly
       * one subdiagonal element next to A.
       *
       * After francis_standard_form, T looks like this:
       *
       * T = [ T_{11} | T_{12}  | T_{13} ]
       *     [   0*   | U^t A U | T_{23} ]
       *     [   0    |    0*   | T_{33} ]
       *
       * since only the 2-by-2 block of A was changed. However,
       * in order to be able to back transform T at the end,
       * we need to apply the U transformation to the rest
       * of the matrix T since there is no way to apply a
       * similarity transformation to T and change only the
       * middle 2-by-2 block. In other words, let
       *
       * M = [ I 0 0 ]
       *     [ 0 U 0 ]
       *     [ 0 0 I ]
       *
       * and compute
       *
       * M^t T M = [ T_{11} | T_{12} U |   T_{13}   ]
       *           [ U^t 0* | U^t A U  | U^t T_{23} ]
       *           [   0    |   0* U   |   T_{33}   ]
       *
       * So basically we need to apply the transformation U
       * to the i x 2 matrix T_{12} and the 2 x (n - i + 2)
       * matrix T_{23}, where i is the index of the top of A
       * in T.
       *
       * The BLAS routine drot() is suited for this.
       */

      if (top < (N - 2))
        {
          /* transform the 2 rows of T_{23} */

          xv = gsl_matrix_subrow(w->H, top, top + 2, N - top - 2);
          yv = gsl_matrix_subrow(w->H, top + 1, top + 2, N - top - 2);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }

      if (top > 0)
        {
          /* transform the 2 columns of T_{12} */

          xv = gsl_matrix_subcolumn(w->H, top, 0, top);
          yv = gsl_matrix_subcolumn(w->H, top + 1, 0, top);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }
    } /* if (w->compute_t) */

  if (w->Z)
    {
      gsl_vector_view xv, yv;

      /*
       * Accumulate the transformation in Z. Here, Z -> Z * M
       *
       * So:
       *
       * Z -> [ Z_{11} | Z_{12} U | Z_{13} ]
       *      [ Z_{21} | Z_{22} U | Z_{23} ]
       *      [ Z_{31} | Z_{32} U | Z_{33} ]
       *
       * So we just need to apply drot() to the 2 columns
       * starting at index 'top'
       */

      xv = gsl_matrix_column(w->Z, top);
      yv = gsl_matrix_column(w->Z, top + 1);

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
    } /* if (w->Z) */
} /* francis_schur_standardize() */

/*
francis_get_submatrix()
  B is a submatrix of A. The goal of this function is to
compute the indices in A of where the matrix B resides
*/

static inline size_t
francis_get_submatrix(gsl_matrix *A, gsl_matrix *B)
{
  size_t diff;
  double ratio;
  size_t top;

  diff = (size_t) (B->data - A->data);

  ratio = (double)diff / ((double) (A->tda + 1));

  top = (size_t) floor(ratio);

  return top;
} /* francis_get_submatrix() */

/*
francis_standard_form()
  Compute the Schur factorization of a real 2-by-2 matrix in
standard form:

[ A B ] = [ CS -SN ] [ T11 T12 ] [ CS SN ]
[ C D ]   [ SN  CS ] [ T21 T22 ] [-SN CS ]

where either:
1) T21 = 0 so that T11 and T22 are real eigenvalues of the matrix, or
2) T11 = T22 and T21*T12 < 0, so that T11 +/- sqrt(|T21*T12|) are
   complex conjugate eigenvalues

Inputs: A  - 2-by-2 matrix
        cs - where to store cosine parameter of rotation matrix
        sn - where to store sine parameter of rotation matrix

Notes: 1) based on LAPACK routine DLANV2
       2) On output, A is modified to contain the matrix in standard form
*/

static void
francis_standard_form(gsl_matrix *A, double *cs, double *sn)
{
  double a, b, c, d; /* input matrix values */
  double tmp;
  double p, z;
  double bcmax, bcmis, scale;
  double tau, sigma;
  double cs1, sn1;
  double aa, bb, cc, dd;
  double sab, sac;

  a = gsl_matrix_get(A, 0, 0);
  b = gsl_matrix_get(A, 0, 1);
  c = gsl_matrix_get(A, 1, 0);
  d = gsl_matrix_get(A, 1, 1);

  if (c == 0.0)
    {
      /*
       * matrix is already upper triangular - set rotation matrix
       * to the identity
       */
      *cs = 1.0;
      *sn = 0.0;
    }
  else if (b == 0.0)
    {
      /* swap rows and columns to make it upper triangular */

      *cs = 0.0;
      *sn = 1.0;

      tmp = d;
      d = a;
      a = tmp;
      b = -c;
      c = 0.0;
    }
  else if (((a - d) == 0.0) && (GSL_SIGN(b) != GSL_SIGN(c)))
    {
      /* the matrix has complex eigenvalues with a == d */
      *cs = 1.0;
      *sn = 0.0;
    }
  else
    {
      tmp = a - d;
      p = 0.5 * tmp;
      bcmax = GSL_MAX(fabs(b), fabs(c));
      bcmis = GSL_MIN(fabs(b), fabs(c)) * GSL_SIGN(b) * GSL_SIGN(c);
      scale = GSL_MAX(fabs(p), bcmax);
      z = (p / scale) * p + (bcmax / scale) * bcmis;

      if (z >= 4.0 * GSL_DBL_EPSILON)
        {
          /* real eigenvalues, compute a and d */

          z = p + GSL_SIGN(p) * fabs(sqrt(scale) * sqrt(z));
          a = d + z;
          d -= (bcmax / z) * bcmis;

          /* compute b and the rotation matrix */

          tau = gsl_hypot(c, z);
          *cs = z / tau;
          *sn = c / tau;
          b -= c;
          c = 0.0;
        }
      else
        {
          /*
           * complex eigenvalues, or real (almost) equal eigenvalues -
           * make diagonal elements equal
           */

          sigma = b + c;
          tau = gsl_hypot(sigma, tmp);
          *cs = sqrt(0.5 * (1.0 + fabs(sigma) / tau));
          *sn = -(p / (tau * (*cs))) * GSL_SIGN(sigma);

          /*
           * Compute [ AA BB ] = [ A B ] [ CS -SN ]
           *         [ CC DD ]   [ C D ] [ SN  CS ]
           */
          aa = a * (*cs) + b * (*sn);
          bb = -a * (*sn) + b * (*cs);
          cc = c * (*cs) + d * (*sn);
          dd = -c * (*sn) + d * (*cs);

          /*
           * Compute [ A B ] = [ CS SN ] [ AA BB ]
           *         [ C D ]   [-SN CS ] [ CC DD ]
           */
          a = aa * (*cs) + cc * (*sn);
          b = bb * (*cs) + dd * (*sn);
          c = -aa * (*sn) + cc * (*cs);
          d = -bb * (*sn) + dd * (*cs);

          tmp = 0.5 * (a + d);
          a = d = tmp;

          if (c != 0.0)
            {
              if (b != 0.0)
                {
                  if (GSL_SIGN(b) == GSL_SIGN(c))
                    {
                      /*
                       * real eigenvalues: reduce to upper triangular
                       * form
                       */
                      sab = sqrt(fabs(b));
                      sac = sqrt(fabs(c));
                      p = GSL_SIGN(c) * fabs(sab * sac);
                      tau = 1.0 / sqrt(fabs(b + c));
                      a = tmp + p;
                      d = tmp - p;
                      b -= c;
                      c = 0.0;

                      cs1 = sab * tau;
                      sn1 = sac * tau;
                      tmp = (*cs) * cs1 - (*sn) * sn1;
                      *sn = (*cs) * sn1 + (*sn) * cs1;
                      *cs = tmp;
                    }
                }
              else
                {
                  b = -c;
                  c = 0.0;
                  tmp = *cs;
                  *cs = -(*sn);
                  *sn = tmp;
                }
            }
        }
    }

  /* set new matrix elements */

  gsl_matrix_set(A, 0, 0, a);
  gsl_matrix_set(A, 0, 1, b);
  gsl_matrix_set(A, 1, 0, c);
  gsl_matrix_set(A, 1, 1, d);
} /* francis_standard_form() */
