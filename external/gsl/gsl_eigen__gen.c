/* eigen/gen.c
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

#include <stdlib.h>
#include <math.h>

#include "gsl__config.h"
#include "gsl_eigen.h"
#include "gsl_linalg.h"
#include "gsl_math.h"
#include "gsl_blas.h"
#include "gsl_vector.h"
#include "gsl_vector_complex.h"
#include "gsl_matrix.h"

/*
 * This module computes the eigenvalues of a real generalized
 * eigensystem A x = \lambda B x. Left and right Schur vectors
 * are optionally computed as well.
 * 
 * Based on the algorithm from Moler and Stewart
 * [1] C. Moler, G. Stewart, "An Algorithm for Generalized Matrix
 *     Eigenvalue Problems", SIAM J. Numer. Anal., Vol 10, No 2, 1973.
 *
 * This algorithm is also described in the book
 * [2] Golub & Van Loan, "Matrix Computations" (3rd ed), algorithm 7.7.3
 *
 * This file contains routines based on original code from LAPACK
 * which is distributed under the modified BSD license.
 */

#define GEN_ESHIFT_COEFF     (1.736)

static void gen_schur_decomp(gsl_matrix *H, gsl_matrix *R,
                             gsl_vector_complex *alpha, gsl_vector *beta,
                             gsl_eigen_gen_workspace *w);
static inline int gen_qzstep(gsl_matrix *H, gsl_matrix *R,
                             gsl_eigen_gen_workspace *w);
static inline void gen_qzstep_d(gsl_matrix *H, gsl_matrix *R,
                                gsl_eigen_gen_workspace *w);
static void gen_tri_split_top(gsl_matrix *H, gsl_matrix *R,
                              gsl_eigen_gen_workspace *w);
static inline void gen_tri_chase_zero(gsl_matrix *H, gsl_matrix *R,
                                      size_t q,
                                      gsl_eigen_gen_workspace *w);
static inline void gen_tri_zero_H(gsl_matrix *H, gsl_matrix *R,
                                  gsl_eigen_gen_workspace *w);
static inline size_t gen_search_small_elements(gsl_matrix *H,
                                               gsl_matrix *R,
                                               int *flag,
                                               gsl_eigen_gen_workspace *w);
static int gen_schur_standardize1(gsl_matrix *A, gsl_matrix *B,
                                  double *alphar, double *beta,
                                  gsl_eigen_gen_workspace *w);
static int gen_schur_standardize2(gsl_matrix *A, gsl_matrix *B,
                                  gsl_complex *alpha1,
                                  gsl_complex *alpha2,
                                  double *beta1, double *beta2,
                                  gsl_eigen_gen_workspace *w);
static int gen_compute_eigenvals(gsl_matrix *A, gsl_matrix *B,
                                 gsl_complex *alpha1,
                                 gsl_complex *alpha2, double *beta1,
                                 double *beta2);
static void gen_store_eigval1(const gsl_matrix *H, const double a,
                              const double b, gsl_vector_complex *alpha,
                              gsl_vector *beta,
                              gsl_eigen_gen_workspace *w);
static void gen_store_eigval2(const gsl_matrix *H,
                              const gsl_complex *alpha1,
                              const double beta1,
                              const gsl_complex *alpha2,
                              const double beta2,
                              gsl_vector_complex *alpha,
                              gsl_vector *beta,
                              gsl_eigen_gen_workspace *w);
static inline size_t gen_get_submatrix(const gsl_matrix *A,
                                       const gsl_matrix *B);

/*FIX**/
inline static double normF (gsl_matrix * A);
inline static void create_givens (const double a, const double b, double *c, double *s);

/*
gsl_eigen_gen_alloc()

Allocate a workspace for solving the generalized eigenvalue problem.
The size of this workspace is O(n)

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_gen_workspace *
gsl_eigen_gen_alloc(const size_t n)
{
  gsl_eigen_gen_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_gen_workspace *) calloc (1, sizeof (gsl_eigen_gen_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;
  w->max_iterations = 30 * n;
  w->n_evals = 0;
  w->n_iter = 0;
  w->needtop = 0;
  w->atol = 0.0;
  w->btol = 0.0;
  w->ascale = 0.0;
  w->bscale = 0.0;
  w->eshift = 0.0;
  w->H = NULL;
  w->R = NULL;
  w->compute_s = 0;
  w->compute_t = 0;
  w->Q = NULL;
  w->Z = NULL;

  w->work = gsl_vector_alloc(n);

  if (w->work == 0)
    {
      gsl_eigen_gen_free(w);
      GSL_ERROR_NULL ("failed to allocate space for additional workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_gen_alloc() */

/*
gsl_eigen_gen_free()
  Free workspace w
*/

void
gsl_eigen_gen_free (gsl_eigen_gen_workspace * w)
{
  if (w->work)
    gsl_vector_free(w->work);

  free(w);
} /* gsl_eigen_gen_free() */

/*
gsl_eigen_gen_params()
  Set parameters which define how we solve the eigenvalue problem

Inputs: compute_s - 1 if we want to compute S, 0 if not
        compute_t - 1 if we want to compute T, 0 if not
        balance   - 1 if we want to balance matrices, 0 if not
        w         - gen workspace

Return: none
*/

void
gsl_eigen_gen_params (const int compute_s, const int compute_t,
                      const int balance, gsl_eigen_gen_workspace *w)
{
  w->compute_s = compute_s;
  w->compute_t = compute_t;
} /* gsl_eigen_gen_params() */

/*
gsl_eigen_gen()

Solve the generalized eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda.

Inputs: A     - general real matrix
        B     - general real matrix
        alpha - where to store eigenvalue numerators
        beta  - where to store eigenvalue denominators
        w     - workspace

Return: success or error
*/

int
gsl_eigen_gen (gsl_matrix * A, gsl_matrix * B, gsl_vector_complex * alpha,
               gsl_vector * beta, gsl_eigen_gen_workspace * w)
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
  else if (alpha->size != N || beta->size != N)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else if (w->size != N)
    {
      GSL_ERROR ("matrix size does not match workspace", GSL_EBADLEN);
    }
  else
    {
      double anorm, bnorm;

      /* compute the Hessenberg-Triangular reduction of (A, B) */
      gsl_linalg_hesstri_decomp(A, B, w->Q, w->Z, w->work);

      /* save pointers to original matrices */
      w->H = A;
      w->R = B;

      w->n_evals = 0;
      w->n_iter = 0;
      w->eshift = 0.0;

      /* determine if we need to compute top indices in QZ step */
      w->needtop = w->Q != 0 || w->Z != 0 || w->compute_t || w->compute_s;

      /* compute matrix norms */
      anorm = normF(A);
      bnorm = normF(B);

      /* compute tolerances and scaling factors */
      w->atol = GSL_MAX(GSL_DBL_MIN, GSL_DBL_EPSILON * anorm);
      w->btol = GSL_MAX(GSL_DBL_MIN, GSL_DBL_EPSILON * bnorm);
      w->ascale = 1.0 / GSL_MAX(GSL_DBL_MIN, anorm);
      w->bscale = 1.0 / GSL_MAX(GSL_DBL_MIN, bnorm);

      /* compute the generalized Schur decomposition and eigenvalues */
      gen_schur_decomp(A, B, alpha, beta, w);

      if (w->n_evals != N)
        return GSL_EMAXITER;

      return GSL_SUCCESS;
    }
} /* gsl_eigen_gen() */

/*
gsl_eigen_gen_QZ()

Solve the generalized eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda. Optionally compute left and/or right
Schur vectors Q and Z which satisfy:

A = Q S Z^t
B = Q T Z^t

where (S, T) is the generalized Schur form of (A, B)

Inputs: A     - general real matrix
        B     - general real matrix
        alpha - where to store eigenvalue numerators
        beta  - where to store eigenvalue denominators
        Q     - if non-null, where to store left Schur vectors
        Z     - if non-null, where to store right Schur vectors
        w     - workspace

Return: success or error
*/

int
gsl_eigen_gen_QZ (gsl_matrix * A, gsl_matrix * B,
                  gsl_vector_complex * alpha, gsl_vector * beta,
                  gsl_matrix * Q, gsl_matrix * Z,
                  gsl_eigen_gen_workspace * w)
{
  if (Q && (A->size1 != Q->size1 || A->size1 != Q->size2))
    {
      GSL_ERROR("Q matrix has wrong dimensions", GSL_EBADLEN);
    }
  else if (Z && (A->size1 != Z->size1 || A->size1 != Z->size2))
    {
      GSL_ERROR("Z matrix has wrong dimensions", GSL_EBADLEN);
    }
  else
    {
      int s;

      w->Q = Q;
      w->Z = Z;

      s = gsl_eigen_gen(A, B, alpha, beta, w);

      w->Q = NULL;
      w->Z = NULL;

      return s;
    }
} /* gsl_eigen_gen_QZ() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
gen_schur_decomp()
  Compute the generalized Schur decomposition of the matrix pencil
(H, R) which is in Hessenberg-Triangular form

Inputs: H     - upper hessenberg matrix
        R     - upper triangular matrix
        alpha - (output) where to store eigenvalue numerators
        beta  - (output) where to store eigenvalue denominators
        w     - workspace

Return: none

Notes: 1) w->n_evals is updated to keep track of how many eigenvalues
          are found
*/

static void
gen_schur_decomp(gsl_matrix *H, gsl_matrix *R, gsl_vector_complex *alpha,
                 gsl_vector *beta, gsl_eigen_gen_workspace *w)
{
  size_t N;
  gsl_matrix_view h, r;
  gsl_matrix_view vh, vr;
  size_t q;             /* index of small subdiagonal element */
  gsl_complex z1, z2;   /* complex values */
  double a, b;
  int s;
  int flag;

  N = H->size1;

  h = gsl_matrix_submatrix(H, 0, 0, N, N);
  r = gsl_matrix_submatrix(R, 0, 0, N, N);

  while ((N > 1) && (w->n_iter)++ < w->max_iterations)
    {
      q = gen_search_small_elements(&h.matrix, &r.matrix, &flag, w);

      if (flag == 0)
        {
          /* no small elements found - do a QZ sweep */
          s = gen_qzstep(&h.matrix, &r.matrix, w);

          if (s == GSL_CONTINUE)
            {
              /*
               * (h, r) is a 2-by-2 block with complex eigenvalues -
               * standardize and read off eigenvalues
               */
              s = gen_schur_standardize2(&h.matrix,
                                         &r.matrix,
                                         &z1,
                                         &z2,
                                         &a,
                                         &b,
                                         w);
              if (s != GSL_SUCCESS)
                {
                  /*
                   * if we get here, then the standardization process
                   * perturbed the eigenvalues onto the real line -
                   * continue QZ iteration to break them into 1-by-1
                   * blocks
                   */
                  continue;
                }

              gen_store_eigval2(&h.matrix, &z1, a, &z2, b, alpha, beta, w);
              N = 0;
            } /* if (s) */

          continue;
        } /* if (flag == 0) */
      else if (flag == 2)
        {
          if (q == 0)
            {
              /*
               * the leading element of R is zero, split off a block
               * at the top
               */
              gen_tri_split_top(&h.matrix, &r.matrix, w);
            }
          else
            {
              /*
               * we found a small element on the diagonal of R - chase the
               * zero to the bottom of the active block and then zero
               * H(n, n - 1) to split off a 1-by-1 block
               */

              if (q != N - 1)
                gen_tri_chase_zero(&h.matrix, &r.matrix, q, w);

              /* now zero H(n, n - 1) */
              gen_tri_zero_H(&h.matrix, &r.matrix, w);
            }

          /* continue so the next iteration detects the zero in H */
          continue;
        }

      /*
       * a small subdiagonal element of H was found - one or two
       * eigenvalues have converged or the matrix has split into
       * two smaller matrices
       */

      if (q == (N - 1))
        {
          /*
           * the last subdiagonal element of the hessenberg matrix is 0 -
           * H_{NN} / R_{NN} is a real eigenvalue - standardize so
           * R_{NN} > 0
           */

          vh = gsl_matrix_submatrix(&h.matrix, q, q, 1, 1);
          vr = gsl_matrix_submatrix(&r.matrix, q, q, 1, 1);
          gen_schur_standardize1(&vh.matrix, &vr.matrix, &a, &b, w);

          gen_store_eigval1(&vh.matrix, a, b, alpha, beta, w);

          --N;
          h = gsl_matrix_submatrix(&h.matrix, 0, 0, N, N);
          r = gsl_matrix_submatrix(&r.matrix, 0, 0, N, N);
        }
      else if (q == (N - 2))
        {
          /* bottom right 2-by-2 block may have converged */

          vh = gsl_matrix_submatrix(&h.matrix, q, q, 2, 2);
          vr = gsl_matrix_submatrix(&r.matrix, q, q, 2, 2);
          s = gen_schur_standardize2(&vh.matrix,
                                     &vr.matrix,
                                     &z1,
                                     &z2,
                                     &a,
                                     &b,
                                     w);
          if (s != GSL_SUCCESS)
            {
              /*
               * this 2-by-2 block contains real eigenvalues that
               * have not yet separated into 1-by-1 blocks -
               * recursively call gen_schur_decomp() to finish off
               * this block
               */
              gen_schur_decomp(&vh.matrix, &vr.matrix, alpha, beta, w);
            }
          else
            {
              /* we got 2 complex eigenvalues */

              gen_store_eigval2(&vh.matrix, &z1, a, &z2, b, alpha, beta, w);
            }

          N -= 2;
          h = gsl_matrix_submatrix(&h.matrix, 0, 0, N, N);
          r = gsl_matrix_submatrix(&r.matrix, 0, 0, N, N);
        }
      else if (q == 1)
        {
          /* H_{11} / R_{11} is an eigenvalue */

          vh = gsl_matrix_submatrix(&h.matrix, 0, 0, 1, 1);
          vr = gsl_matrix_submatrix(&r.matrix, 0, 0, 1, 1);
          gen_schur_standardize1(&vh.matrix, &vr.matrix, &a, &b, w);

          gen_store_eigval1(&vh.matrix, a, b, alpha, beta, w);

          --N;
          h = gsl_matrix_submatrix(&h.matrix, 1, 1, N, N);
          r = gsl_matrix_submatrix(&r.matrix, 1, 1, N, N);
        }
      else if (q == 2)
        {
          /* upper left 2-by-2 block may have converged */

          vh = gsl_matrix_submatrix(&h.matrix, 0, 0, 2, 2);
          vr = gsl_matrix_submatrix(&r.matrix, 0, 0, 2, 2);
          s = gen_schur_standardize2(&vh.matrix,
                                     &vr.matrix,
                                     &z1,
                                     &z2,
                                     &a,
                                     &b,
                                     w);
          if (s != GSL_SUCCESS)
            {
              /*
               * this 2-by-2 block contains real eigenvalues that
               * have not yet separated into 1-by-1 blocks -
               * recursively call gen_schur_decomp() to finish off
               * this block
               */
              gen_schur_decomp(&vh.matrix, &vr.matrix, alpha, beta, w);
            }
          else
            {
              /* we got 2 complex eigenvalues */
              gen_store_eigval2(&vh.matrix, &z1, a, &z2, b, alpha, beta, w);
            }

          N -= 2;
          h = gsl_matrix_submatrix(&h.matrix, 2, 2, N, N);
          r = gsl_matrix_submatrix(&r.matrix, 2, 2, N, N);
        }
      else
        {
          /*
           * There is a zero element on the subdiagonal somewhere
           * in the middle of the matrix - we can now operate
           * separately on the two submatrices split by this
           * element. q is the row index of the zero element.
           */

          /* operate on lower right (N - q)-by-(N - q) block first */
          vh = gsl_matrix_submatrix(&h.matrix, q, q, N - q, N - q);
          vr = gsl_matrix_submatrix(&r.matrix, q, q, N - q, N - q);
          gen_schur_decomp(&vh.matrix, &vr.matrix, alpha, beta, w);

          /* operate on upper left q-by-q block */
          vh = gsl_matrix_submatrix(&h.matrix, 0, 0, q, q);
          vr = gsl_matrix_submatrix(&r.matrix, 0, 0, q, q);
          gen_schur_decomp(&vh.matrix, &vr.matrix, alpha, beta, w);

          N = 0;
        }
    } /* while ((N > 1) && (w->n_iter)++ < w->max_iterations) */

  /* handle special case of N = 1 */

  if (N == 1)
    {
      gen_schur_standardize1(&h.matrix, &r.matrix, &a, &b, w);
      gen_store_eigval1(&h.matrix, a, b, alpha, beta, w);
    }
} /* gen_schur_decomp() */

/*
gen_qzstep()
  This routine determines what type of QZ step to perform on
the generalized matrix pair (H, R). If the pair is 3-by-3 or bigger,
we look at the bottom right 2-by-2 block. If this block has complex
eigenvalues, we perform a Francis double shift QZ sweep. If it
has real eigenvalues, we perform an implicit single shift QZ sweep.

If the pair is 2-by-2 with real eigenvalues, we perform a single
shift sweep. If it has complex eigenvalues, we return GSL_CONTINUE
to notify the calling function that a 2-by-2 block with complex
eigenvalues has converged, so that it may then call
gen_schur_standardize2(). In the real eigenvalue case, we want to
continue doing QZ sweeps to break it up into two 1-by-1 blocks.

See LAPACK routine DHGEQZ and [1] for more information.

Inputs: H - upper Hessenberg matrix (at least 2-by-2)
        R - upper triangular matrix (at least 2-by-2)
        w - workspace

Return: GSL_SUCCESS on normal completion
        GSL_CONTINUE if we detect a 2-by-2 block with complex eigenvalues
*/

static inline int
gen_qzstep(gsl_matrix *H, gsl_matrix *R, gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  gsl_matrix_view vh, vr; /* views of bottom right 2-by-2 block */
  double wr1, wr2, wi;
  double scale1, scale2, scale;
  double cs, sn;          /* givens rotation */
  double temp,            /* temporary variables */
         temp2;
  size_t j;               /* looping */
  gsl_vector_view xv, yv; /* temporary views */
  size_t top;
  size_t rows;

  if (w->n_iter % 10 == 0)
    {
      /*
       * Exceptional shift - we have gone 10 iterations without finding
       * a new eigenvalue, do a single shift sweep with an
       * exceptional shift
       */

      if ((GSL_DBL_MIN * w->max_iterations) *
          fabs(gsl_matrix_get(H, N - 2, N - 1)) <
          fabs(gsl_matrix_get(R, N - 2, N - 2)))
        {
          w->eshift += gsl_matrix_get(H, N - 2, N - 1) /
                       gsl_matrix_get(R, N - 2, N - 2);
        }
      else
        w->eshift += 1.0 / (GSL_DBL_MIN * w->max_iterations);

      if ((w->eshift < GSL_DBL_EPSILON) &&
          (GSL_DBL_MIN * w->max_iterations) *
          fabs(gsl_matrix_get(H, N - 1, N - 2)) <
          fabs(gsl_matrix_get(R, N - 2, N - 2)))
        {
          w->eshift = GEN_ESHIFT_COEFF *
                      (w->ascale * gsl_matrix_get(H, N - 1, N - 2)) /
                      (w->bscale * gsl_matrix_get(R, N - 2, N - 2));
        }

      scale1 = 1.0;
      wr1 = w->eshift;
    }
  else
    {
      /*
       * Compute generalized eigenvalues of bottom right 2-by-2 block
       * to be used as shifts - wr1 is the Wilkinson shift
       */

      vh = gsl_matrix_submatrix(H, N - 2, N - 2, 2, 2);
      vr = gsl_matrix_submatrix(R, N - 2, N - 2, 2, 2);
      gsl_schur_gen_eigvals(&vh.matrix,
                            &vr.matrix,
                            &wr1,
                            &wr2,
                            &wi,
                            &scale1,
                            &scale2);

      if (wi != 0.0)
        {
          /* complex eigenvalues */

          if (N == 2)
            {
              /*
               * its a 2-by-2 block with complex eigenvalues - notify
               * the calling function to deflate
               */
              return (GSL_CONTINUE);
            }
          else
            {
              /* do a francis double shift sweep */
              gen_qzstep_d(H, R, w);
            }

          return GSL_SUCCESS;
        }
    }

  /* real eigenvalues - perform single shift QZ step */

  temp = GSL_MIN(w->ascale, 1.0) * (0.5 / GSL_DBL_MIN);
  if (scale1 > temp)
    scale = temp / scale1;
  else
    scale = 1.0;

  temp = GSL_MIN(w->bscale, 1.0) * (0.5 / GSL_DBL_MIN);
  if (fabs(wr1) > temp)
    scale = GSL_MIN(scale, temp / fabs(wr1));

  scale1 *= scale;
  wr1 *= scale;

  if (w->needtop)
    {
      /* get absolute index of this matrix relative to original matrix */
      top = gen_get_submatrix(w->H, H);
    }

  temp = scale1*gsl_matrix_get(H, 0, 0) - wr1*gsl_matrix_get(R, 0, 0);
  temp2 = scale1*gsl_matrix_get(H, 1, 0);

  create_givens(temp, temp2, &cs, &sn);
  sn = -sn;

  for (j = 0; j < N - 1; ++j)
    {
      if (j > 0)
        {
          temp = gsl_matrix_get(H, j, j - 1);
          temp2 = gsl_matrix_get(H, j + 1, j - 1);
          create_givens(temp, temp2, &cs, &sn);
          sn = -sn;

          /* apply to column (j - 1) */
          temp = cs * gsl_matrix_get(H, j, j - 1) +
                 sn * gsl_matrix_get(H, j + 1, j - 1);
          gsl_matrix_set(H, j, j - 1, temp);
          gsl_matrix_set(H, j + 1, j - 1, 0.0);
        }

      /* apply G to H(j:j+1,:) and T(j:j+1,:) */

      if (w->compute_s)
        {
          xv = gsl_matrix_subrow(w->H, top + j, top + j, w->size - top - j);
          yv = gsl_matrix_subrow(w->H, top + j + 1, top + j, w->size - top - j);
        }
      else
        {
          xv = gsl_matrix_subrow(H, j, j, N - j);
          yv = gsl_matrix_subrow(H, j + 1, j, N - j);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      if (w->compute_t)
        {
          xv = gsl_matrix_subrow(w->R, top + j, top + j, w->size - top - j);
          yv = gsl_matrix_subrow(w->R, top + j + 1, top + j, w->size - top - j);
        }
      else
        {
          xv = gsl_matrix_subrow(R, j, j, N - j);
          yv = gsl_matrix_subrow(R, j + 1, j, N - j);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      if (w->Q)
        {
          /* accumulate Q: Q -> QG */
          xv = gsl_matrix_column(w->Q, top + j);
          yv = gsl_matrix_column(w->Q, top + j + 1);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }

      temp = gsl_matrix_get(R, j + 1, j + 1);
      temp2 = gsl_matrix_get(R, j + 1, j);
      create_givens(temp, temp2, &cs, &sn);

      rows = GSL_MIN(j + 3, N);

      if (w->compute_s)
        {
          xv = gsl_matrix_subcolumn(w->H, top + j, 0, top + rows);
          yv = gsl_matrix_subcolumn(w->H, top + j + 1, 0, top + rows);
        }
      else
        {
          xv = gsl_matrix_subcolumn(H, j, 0, rows);
          yv = gsl_matrix_subcolumn(H, j + 1, 0, rows);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      rows = GSL_MIN(j + 2, N);

      if (w->compute_t)
        {
          xv = gsl_matrix_subcolumn(w->R, top + j, 0, top + rows);
          yv = gsl_matrix_subcolumn(w->R, top + j + 1, 0, top + rows);
        }
      else
        {
          xv = gsl_matrix_subcolumn(R, j, 0, rows);
          yv = gsl_matrix_subcolumn(R, j + 1, 0, rows);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      if (w->Z)
        {
          /* accumulate Z: Z -> ZG */
          xv = gsl_matrix_column(w->Z, top + j);
          yv = gsl_matrix_column(w->Z, top + j + 1);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }
    } /* for (j = 0; j < N - 1; ++j) */

  return GSL_SUCCESS;
} /* gen_qzstep() */

/*
gen_qzstep_d()
  Perform an implicit double shift QZ step.

See Golub & Van Loan, "Matrix Computations" (3rd ed), algorithm 7.7.2

Inputs: H - upper Hessenberg matrix (at least 3-by-3)
        R - upper triangular matrix (at least 3-by-3)
        w - workspace
*/

static inline void
gen_qzstep_d(gsl_matrix *H, gsl_matrix *R, gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  size_t j;               /* looping */
  double dat[3];          /* householder vector */
  double tau;             /* householder coefficient */
  gsl_vector_view v2, v3; /* views into 'dat' */
  gsl_matrix_view m;      /* temporary view */
  double tmp;
  size_t q, r;
  size_t top;             /* location of H in original matrix */
  double scale;
  double AB11,            /* various matrix element ratios */
         AB22,
         ABNN,
         ABMM,
         AMNBNN,
         ANMBMM,
         A21B11,
         A12B22,
         A32B22,
         B12B22,
         BMNBNN;

  v2 = gsl_vector_view_array(dat, 2);
  v3 = gsl_vector_view_array(dat, 3);

  if (w->needtop)
    {
      /* get absolute index of this matrix relative to original matrix */
      top = gen_get_submatrix(w->H, H);
    }

  /*
   * Similar to the QR method, we take the shifts to be the two
   * zeros of the problem
   *
   * det[H(n-1:n,n-1:n) - s*R(n-1:n,n-1:n)] = 0
   *
   * The initial householder vector elements are then given by
   * Eq. 4.1 of [1], which are designed to reduce errors when
   * off diagonal elements are small.
   */

  ABMM = (w->ascale * gsl_matrix_get(H, N - 2, N - 2)) /
         (w->bscale * gsl_matrix_get(R, N - 2, N - 2));
  ABNN = (w->ascale * gsl_matrix_get(H, N - 1, N - 1)) /
         (w->bscale * gsl_matrix_get(R, N - 1, N - 1));
  AB11 = (w->ascale * gsl_matrix_get(H, 0, 0)) /
         (w->bscale * gsl_matrix_get(R, 0, 0));
  AB22 = (w->ascale * gsl_matrix_get(H, 1, 1)) /
         (w->bscale * gsl_matrix_get(R, 1, 1));
  AMNBNN = (w->ascale * gsl_matrix_get(H, N - 2, N - 1)) /
           (w->bscale * gsl_matrix_get(R, N - 1, N - 1));
  ANMBMM = (w->ascale * gsl_matrix_get(H, N - 1, N - 2)) /
           (w->bscale * gsl_matrix_get(R, N - 2, N - 2));
  BMNBNN = gsl_matrix_get(R, N - 2, N - 1) /
           gsl_matrix_get(R, N - 1, N - 1);
  A21B11 = (w->ascale * gsl_matrix_get(H, 1, 0)) /
           (w->bscale * gsl_matrix_get(R, 0, 0));
  A12B22 = (w->ascale * gsl_matrix_get(H, 0, 1)) /
           (w->bscale * gsl_matrix_get(R, 1, 1));
  A32B22 = (w->ascale * gsl_matrix_get(H, 2, 1)) /
           (w->bscale * gsl_matrix_get(R, 1, 1));
  B12B22 = gsl_matrix_get(R, 0, 1) / gsl_matrix_get(R, 1, 1);

  /*
   * These are the Eqs (4.1) of [1], just multiplied by the factor
   * (A_{21} / B_{11})
   */
  dat[0] = (ABMM - AB11) * (ABNN - AB11) - (AMNBNN * ANMBMM) +
           (ANMBMM * BMNBNN * AB11) + (A12B22 - (AB11 * B12B22)) * A21B11;
  dat[1] = ((AB22 - AB11) - (A21B11 * B12B22) - (ABMM - AB11) -
            (ABNN - AB11) + (ANMBMM * BMNBNN)) * A21B11;
  dat[2] = A32B22 * A21B11;

  scale = fabs(dat[0]) + fabs(dat[1]) + fabs(dat[2]);
  if (scale != 0.0)
    {
      dat[0] /= scale;
      dat[1] /= scale;
      dat[2] /= scale;
    }

  for (j = 0; j < N - 2; ++j)
    {
      r = GSL_MIN(j + 4, N);

      /*
       * Find householder Q so that
       *
       * Q [x y z]^t = [ * 0 0 ]^t
       */

      tau = gsl_linalg_householder_transform(&v3.vector);

      if (tau != 0.0)
        {
          /*
           * q is the initial column to start applying the householder
           * transformation. The GSL_MAX() simply ensures we don't
           * try to apply it to column (-1), since we are zeroing out
           * column (j - 1) except for the first iteration which
           * introduces the bulge.
           */
          q = (size_t) GSL_MAX(0, (int)j - 1);

          /* H -> QH, R -> QR */

          if (w->compute_s)
            {
              /*
               * We are computing the Schur form S, so we need to
               * transform the whole matrix H
               */
              m = gsl_matrix_submatrix(w->H,
                                       top + j,
                                       top + q,
                                       3,
                                       w->size - top - q);
              gsl_linalg_householder_hm(tau, &v3.vector, &m.matrix);
            }
          else
            {
              /* just transform the active block */
              m = gsl_matrix_submatrix(H, j, q, 3, N - q);
              gsl_linalg_householder_hm(tau, &v3.vector, &m.matrix);
            }

          if (w->compute_t)
            {
              /*
               * We are computing the Schur form T, so we need to
               * transform the whole matrix R
               */
              m = gsl_matrix_submatrix(w->R,
                                       top + j,
                                       top + j,
                                       3,
                                       w->size - top - j);
              gsl_linalg_householder_hm(tau, &v3.vector, &m.matrix);
            }
          else
            {
              /* just transform the active block */
              m = gsl_matrix_submatrix(R, j, j, 3, N - j);
              gsl_linalg_householder_hm(tau, &v3.vector, &m.matrix);
            }

          if (w->Q)
            {
              /* accumulate the transformation into Q */
              m = gsl_matrix_submatrix(w->Q, 0, top + j, w->size, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }
        } /* if (tau != 0.0) */

      /*
       * Find householder Z so that
       * 
       * [ r_{j+2,j} r_{j+2, j+1}, r_{j+2, j+2} ] Z = [ 0 0 * ]
       *
       * This isn't exactly what gsl_linalg_householder_transform
       * does, so we need to rotate the input vector so it preserves
       * the last element, and then rotate it back afterwards.
       *
       * So instead of transforming [x y z], we transform [z x y],
       * and the resulting HH vector [1 v2 v3] -> [v2 v3 1] but
       * then needs to be scaled to have the first element = 1, so
       * it becomes [1 v3/v2 1/v2] (tau must also be scaled accordingly).
       */

      dat[0] = gsl_matrix_get(R, j + 2, j + 2);
      dat[1] = gsl_matrix_get(R, j + 2, j);
      dat[2] = gsl_matrix_get(R, j + 2, j + 1);
      scale = fabs(dat[0]) + fabs(dat[1]) + fabs(dat[2]);
      if (scale != 0.0)
        {
          dat[0] /= scale;
          dat[1] /= scale;
          dat[2] /= scale;
        }

      tau = gsl_linalg_householder_transform(&v3.vector);

      if (tau != 0.0)
        {
          /* rotate back */
          tmp = gsl_vector_get(&v3.vector, 1);
          gsl_vector_set(&v3.vector, 1, gsl_vector_get(&v3.vector, 2)/tmp);
          gsl_vector_set(&v3.vector, 2, 1.0 / tmp);
          tau *= tmp * tmp;

          /* H -> HZ, R -> RZ */

          if (w->compute_s)
            {
              m = gsl_matrix_submatrix(w->H, 0, top + j, top + r, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }
          else
            {
              m = gsl_matrix_submatrix(H, 0, j, r, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }

          if (w->compute_t)
            {
              m = gsl_matrix_submatrix(w->R, 0, top + j, top + j + 3, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }
          else
            {
              m = gsl_matrix_submatrix(R, 0, j, j + 3, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }

          if (w->Z)
            {
              /* accumulate transformation into Z */
              m = gsl_matrix_submatrix(w->Z, 0, top + j, w->size, 3);
              gsl_linalg_householder_mh(tau, &v3.vector, &m.matrix);
            }
        } /* if (tau != 0.0) */

      /*
       * Find householder Z so that
       * 
       * [ r_{j+1,j} r_{j+1, j+1} ] Z = [ 0 * ]
       */

      dat[0] = gsl_matrix_get(R, j + 1, j + 1);
      dat[1] = gsl_matrix_get(R, j + 1, j);
      scale = fabs(dat[0]) + fabs(dat[1]);
      if (scale != 0.0)
        {
          dat[0] /= scale;
          dat[1] /= scale;
        }

      tau = gsl_linalg_householder_transform(&v2.vector);

      if (tau != 0.0)
        {
          /* rotate back */
          tmp = gsl_vector_get(&v2.vector, 1);
          gsl_vector_set(&v2.vector, 1, 1.0 / tmp);
          tau *= tmp * tmp;

          /* H -> HZ, R -> RZ */

          if (w->compute_s)
            {
              m = gsl_matrix_submatrix(w->H, 0, top + j, top + r, 2);
              gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
            }
          else
            {
              m = gsl_matrix_submatrix(H, 0, j, r, 2);
              gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
            }

          if (w->compute_t)
            {
              m = gsl_matrix_submatrix(w->R, 0, top + j, top + j + 3, 2);
              gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
            }
          else
            {
              m = gsl_matrix_submatrix(R, 0, j, j + 3, 2);
              gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
            }

          if (w->Z)
            {
              /* accumulate transformation into Z */
              m = gsl_matrix_submatrix(w->Z, 0, top + j, w->size, 2);
              gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
            }
        } /* if (tau != 0.0) */

      dat[0] = gsl_matrix_get(H, j + 1, j);
      dat[1] = gsl_matrix_get(H, j + 2, j);
      if (j < N - 3)
        dat[2] = gsl_matrix_get(H, j + 3, j);

      scale = fabs(dat[0]) + fabs(dat[1]) + fabs(dat[2]);
      if (scale != 0.0)
        {
          dat[0] /= scale;
          dat[1] /= scale;
          dat[2] /= scale;
        }
    } /* for (j = 0; j < N - 2; ++j) */

  /*
   * Find Householder Q so that
   *
   * Q [ x y ]^t = [ * 0 ]^t
   */

  scale = fabs(dat[0]) + fabs(dat[1]);
  if (scale != 0.0)
    {
      dat[0] /= scale;
      dat[1] /= scale;
    }

  tau = gsl_linalg_householder_transform(&v2.vector);
  
  if (w->compute_s)
    {
      m = gsl_matrix_submatrix(w->H,
                               top + N - 2,
                               top + N - 3,
                               2,
                               w->size - top - N + 3);
      gsl_linalg_householder_hm(tau, &v2.vector, &m.matrix);
    }
  else
    {
      m = gsl_matrix_submatrix(H, N - 2, N - 3, 2, 3);
      gsl_linalg_householder_hm(tau, &v2.vector, &m.matrix);
    }

  if (w->compute_t)
    {
      m = gsl_matrix_submatrix(w->R,
                               top + N - 2,
                               top + N - 2,
                               2,
                               w->size - top - N + 2);
      gsl_linalg_householder_hm(tau, &v2.vector, &m.matrix);
    }
  else
    {
      m = gsl_matrix_submatrix(R, N - 2, N - 2, 2, 2);
      gsl_linalg_householder_hm(tau, &v2.vector, &m.matrix);
    }

  if (w->Q)
    {
      /* accumulate the transformation into Q */
      m = gsl_matrix_submatrix(w->Q, 0, top + N - 2, w->size, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }

  /*
   * Find Householder Z so that
   *
   * [ b_{n,n-1} b_{nn} ] Z = [ 0 * ]
   */

  dat[0] = gsl_matrix_get(R, N - 1, N - 1);
  dat[1] = gsl_matrix_get(R, N - 1, N - 2);
  scale = fabs(dat[0]) + fabs(dat[1]);
  if (scale != 0.0)
    {
      dat[0] /= scale;
      dat[1] /= scale;
    }

  tau = gsl_linalg_householder_transform(&v2.vector);

  /* rotate back */
  tmp = gsl_vector_get(&v2.vector, 1);
  gsl_vector_set(&v2.vector, 1, 1.0 / tmp);
  tau *= tmp * tmp;

  if (w->compute_s)
    {
      m = gsl_matrix_submatrix(w->H, 0, top + N - 2, top + N, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }
  else
    {
      m = gsl_matrix_submatrix(H, 0, N - 2, N, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }

  if (w->compute_t)
    {
      m = gsl_matrix_submatrix(w->R, 0, top + N - 2, top + N, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }
  else
    {
      m = gsl_matrix_submatrix(R, 0, N - 2, N, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }

  if (w->Z)
    {
      /* accumulate the transformation into Z */
      m = gsl_matrix_submatrix(w->Z, 0, top + N - 2, w->size, 2);
      gsl_linalg_householder_mh(tau, &v2.vector, &m.matrix);
    }
} /* gen_qzstep_d() */

/*
gen_tri_split_top()
  This routine is called when the leading element on the diagonal of R
has become negligible. Split off a 1-by-1 block at the top.

Inputs: H - upper hessenberg matrix
        R - upper triangular matrix
        w - workspace
*/

static void
gen_tri_split_top(gsl_matrix *H, gsl_matrix *R, gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  size_t j, top;
  double cs, sn;
  gsl_vector_view xv, yv;

  if (w->needtop)
    top = gen_get_submatrix(w->H, H);

  j = 0;

  create_givens(gsl_matrix_get(H, j, j),
                gsl_matrix_get(H, j + 1, j),
                &cs,
                &sn);
  sn = -sn;

  if (w->compute_s)
    {
      xv = gsl_matrix_subrow(w->H, top + j, top, w->size - top);
      yv = gsl_matrix_subrow(w->H, top + j + 1, top, w->size - top);
    }
  else
    {
      xv = gsl_matrix_row(H, j);
      yv = gsl_matrix_row(H, j + 1);
    }

  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
  gsl_matrix_set(H, j + 1, j, 0.0);

  if (w->compute_t)
    {
      xv = gsl_matrix_subrow(w->R, top + j, top + 1, w->size - top - 1);
      yv = gsl_matrix_subrow(w->R, top + j + 1, top + 1, w->size - top - 1);
    }
  else
    {
      xv = gsl_matrix_subrow(R, j, 1, N - 1);
      yv = gsl_matrix_subrow(R, j + 1, 1, N - 1);
    }

  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

  if (w->Q)
    {
      xv = gsl_matrix_column(w->Q, top + j);
      yv = gsl_matrix_column(w->Q, top + j + 1);
      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
    }
} /* gen_tri_split_top() */

/*
gen_tri_chase_zero()
  This routine is called when an element on the diagonal of R
has become negligible. Chase the zero to the bottom of the active
block so we can split off a 1-by-1 block.

Inputs: H - upper hessenberg matrix
        R - upper triangular matrix
        q - index such that R(q,q) = 0 (q must be > 0)
        w - workspace
*/

static inline void
gen_tri_chase_zero(gsl_matrix *H, gsl_matrix *R, size_t q,
                   gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  size_t j, top;
  double cs, sn;
  gsl_vector_view xv, yv;

  if (w->needtop)
    top = gen_get_submatrix(w->H, H);

  for (j = q; j < N - 1; ++j)
    {
      create_givens(gsl_matrix_get(R, j, j + 1),
                    gsl_matrix_get(R, j + 1, j + 1),
                    &cs,
                    &sn);
      sn = -sn;

      if (w->compute_t)
        {
          xv = gsl_matrix_subrow(w->R, top + j, top + j + 1, w->size - top - j - 1);
          yv = gsl_matrix_subrow(w->R, top + j + 1, top + j + 1, w->size - top - j - 1);
        }
      else
        {
          xv = gsl_matrix_subrow(R, j, j + 1, N - j - 1);
          yv = gsl_matrix_subrow(R, j + 1, j + 1, N - j - 1);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
      gsl_matrix_set(R, j + 1, j + 1, 0.0);

      if (w->compute_s)
        {
          xv = gsl_matrix_subrow(w->H, top + j, top + j - 1, w->size - top - j + 1);
          yv = gsl_matrix_subrow(w->H, top + j + 1, top + j - 1, w->size - top - j + 1);
        }
      else
        {
          xv = gsl_matrix_subrow(H, j, j - 1, N - j + 1);
          yv = gsl_matrix_subrow(H, j + 1, j - 1, N - j + 1);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      if (w->Q)
        {
          /* accumulate Q */
          xv = gsl_matrix_column(w->Q, top + j);
          yv = gsl_matrix_column(w->Q, top + j + 1);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }

      create_givens(gsl_matrix_get(H, j + 1, j),
                    gsl_matrix_get(H, j + 1, j - 1),
                    &cs,
                    &sn);
      sn = -sn;

      if (w->compute_s)
        {
          xv = gsl_matrix_subcolumn(w->H, top + j, 0, top + j + 2);
          yv = gsl_matrix_subcolumn(w->H, top + j - 1, 0, top + j + 2);
        }
      else
        {
          xv = gsl_matrix_subcolumn(H, j, 0, j + 2);
          yv = gsl_matrix_subcolumn(H, j - 1, 0, j + 2);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
      gsl_matrix_set(H, j + 1, j - 1, 0.0);

      if (w->compute_t)
        {
          xv = gsl_matrix_subcolumn(w->R, top + j, 0, top + j + 1);
          yv = gsl_matrix_subcolumn(w->R, top + j - 1, 0, top + j + 1);
        }
      else
        {
          xv = gsl_matrix_subcolumn(R, j, 0, j + 1);
          yv = gsl_matrix_subcolumn(R, j - 1, 0, j + 1);
        }

      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

      if (w->Z)
        {
          /* accumulate Z */
          xv = gsl_matrix_column(w->Z, top + j);
          yv = gsl_matrix_column(w->Z, top + j - 1);
          gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
        }
    }
} /* gen_tri_chase_zero() */

/*
gen_tri_zero_H()
  Companion function to get_tri_chase_zero(). After the zero on
the diagonal of R has been chased to the bottom, we zero the element
H(n, n - 1) in order to split off a 1-by-1 block.
*/

static inline void
gen_tri_zero_H(gsl_matrix *H, gsl_matrix *R, gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  size_t top;
  double cs, sn;
  gsl_vector_view xv, yv;

  if (w->needtop)
    top = gen_get_submatrix(w->H, H);

  create_givens(gsl_matrix_get(H, N - 1, N - 1),
                gsl_matrix_get(H, N - 1, N - 2),
                &cs,
                &sn);
  sn = -sn;

  if (w->compute_s)
    {
      xv = gsl_matrix_subcolumn(w->H, top + N - 1, 0, top + N);
      yv = gsl_matrix_subcolumn(w->H, top + N - 2, 0, top + N);
    }
  else
    {
      xv = gsl_matrix_column(H, N - 1);
      yv = gsl_matrix_column(H, N - 2);
    }

  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

  gsl_matrix_set(H, N - 1, N - 2, 0.0);

  if (w->compute_t)
    {
      xv = gsl_matrix_subcolumn(w->R, top + N - 1, 0, top + N - 1);
      yv = gsl_matrix_subcolumn(w->R, top + N - 2, 0, top + N - 1);
    }
  else
    {
      xv = gsl_matrix_subcolumn(R, N - 1, 0, N - 1);
      yv = gsl_matrix_subcolumn(R, N - 2, 0, N - 1);
    }

  gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);

  if (w->Z)
    {
      /* accumulate Z */
      xv = gsl_matrix_column(w->Z, top + N - 1);
      yv = gsl_matrix_column(w->Z, top + N - 2);
      gsl_blas_drot(&xv.vector, &yv.vector, cs, sn);
    }
} /* gen_tri_zero_H() */

/*
gen_search_small_elements()
  This routine searches for small elements in the matrix pencil
(H, R) to determine if any eigenvalues have converged.

Tests:

1. Test if the Hessenberg matrix has a small subdiagonal element:
   H(i, i - 1) < tolerance

2. Test if the Triangular matrix has a small diagonal element:
   R(i, i) < tolerance

Possible outcomes:

(A) Neither test passed: in this case 'flag' is set to 0 and the
    function returns 0

(B) Test 1 passes and 2 does not: in this case 'flag' is set to 1
    and we return the row index i such that H(i, i - 1) < tol

(C) Test 2 passes and 1 does not: in this case 'flag' is set to 2
    and we return the index i such that R(i, i) < tol

(D) Tests 1 and 2 both pass: in this case 'flag' is set to 3 and
    we return the index i such that H(i, i - 1) < tol and R(i, i) < tol

Inputs: H    - upper Hessenberg matrix
        R    - upper Triangular matrix
        flag - (output) flag set on output (see above)
        w    - workspace

Return: see above
*/

static inline size_t
gen_search_small_elements(gsl_matrix *H, gsl_matrix *R,
                          int *flag, gsl_eigen_gen_workspace *w)
{
  const size_t N = H->size1;
  int k;
  size_t i;
  int pass1 = 0;
  int pass2 = 0;

  for (k = (int) N - 1; k >= 0; --k)
    {
      i = (size_t) k;

      if (i != 0 && fabs(gsl_matrix_get(H, i, i - 1)) <= w->atol)
        {
          gsl_matrix_set(H, i, i - 1, 0.0);
          pass1 = 1;
        }

      if (fabs(gsl_matrix_get(R, i, i)) < w->btol)
        {
          gsl_matrix_set(R, i, i, 0.0);
          pass2 = 1;
        }

      if (pass1 && !pass2)      /* case B */
        {
          *flag = 1;
          return (i);
        }
      else if (!pass1 && pass2) /* case C */
        {
          *flag = 2;
          return (i);
        }
      else if (pass1 && pass2)  /* case D */
        {
          *flag = 3;
          return (i);
        }
    }

  /* neither test passed: case A */

  *flag = 0;
  return (0);
} /* gen_search_subdiag_small_elements() */

/*
gen_schur_standardize1()
  This function is called when a 1-by-1 block has converged -
convert the block to standard form and update the Schur forms and
vectors if required. Standard form here means that the diagonal
element of B is positive.

Inputs: A      - 1-by-1 matrix in Schur form S
        B      - 1-by-1 matrix in Schur form T
        alphar - where to store real part of eigenvalue numerator
        beta   - where to store eigenvalue denominator
        w      - workspace

Return: success
*/

static int
gen_schur_standardize1(gsl_matrix *A, gsl_matrix *B, double *alphar,
                       double *beta, gsl_eigen_gen_workspace *w)
{
  size_t i;
  size_t top;

  /*
   * it is a 1-by-1 block - the only requirement is that
   * B_{00} is > 0, so if it isn't apply a -I transformation
   */
  if (gsl_matrix_get(B, 0, 0) < 0.0)
    {
      if (w->needtop)
        top = gen_get_submatrix(w->H, A);

      if (w->compute_t)
        {
          for (i = 0; i <= top; ++i)
            gsl_matrix_set(w->R, i, top, -gsl_matrix_get(w->R, i, top));
        }
      else
        gsl_matrix_set(B, 0, 0, -gsl_matrix_get(B, 0, 0));

      if (w->compute_s)
        {
          for (i = 0; i <= top; ++i)
            gsl_matrix_set(w->H, i, top, -gsl_matrix_get(w->H, i, top));
        }
      else
        gsl_matrix_set(A, 0, 0, -gsl_matrix_get(A, 0, 0));

      if (w->Z)
        {
          for (i = 0; i < w->size; ++i)
            gsl_matrix_set(w->Z, i, top, -gsl_matrix_get(w->Z, i, top));
        }
    }

  *alphar = gsl_matrix_get(A, 0, 0);
  *beta = gsl_matrix_get(B, 0, 0);

  return GSL_SUCCESS;
} /* gen_schur_standardize1() */

/*
gen_schur_standardize2()
  This function is called when a 2-by-2 generalized block has
converged. Convert the block to standard form, which means B
is rotated so that

B = [ B11  0  ] with B11, B22 non-negative
    [  0  B22 ]

If the resulting block (A, B) has complex eigenvalues, they are
computed. Otherwise, the function will return GSL_CONTINUE to
notify caller that we need to do more single shift sweeps to
convert the 2-by-2 block into two 1-by-1 blocks.

Inputs: A      - 2-by-2 submatrix of schur form S
        B      - 2-by-2 submatrix of schur form T
        alpha1 - (output) where to store eigenvalue 1 numerator
        alpha2 - (output) where to store eigenvalue 2 numerator
        beta1  - (output) where to store eigenvalue 1 denominator
        beta2  - (output) where to store eigenvalue 2 denominator
        w      - workspace

Return: GSL_SUCCESS if block has complex eigenvalues (they are computed)
        GSL_CONTINUE if block has real eigenvalues (they are not computed)
*/

static int
gen_schur_standardize2(gsl_matrix *A, gsl_matrix *B, gsl_complex *alpha1,
                       gsl_complex *alpha2, double *beta1, double *beta2,
                       gsl_eigen_gen_workspace *w)
{
  double datB[4],
         datV[4],
         datS[2],
         work[2];
  gsl_matrix_view uv = gsl_matrix_view_array(datB, 2, 2);
  gsl_matrix_view vv = gsl_matrix_view_array(datV, 2, 2);
  gsl_vector_view sv = gsl_vector_view_array(datS, 2);
  gsl_vector_view wv = gsl_vector_view_array(work, 2);
  double B11, B22;
  size_t top;
  double det;
  double cr, sr, cl, sl;
  gsl_vector_view xv, yv;
  int s;

  if (w->needtop)
    top = gen_get_submatrix(w->H, A);

  /*
   * Rotate B so that
   *
   * B = [ B11  0  ]
   *     [  0  B22 ]
   *
   * with B11 non-negative
   */

  gsl_matrix_memcpy(&uv.matrix, B);
  gsl_linalg_SV_decomp(&uv.matrix, &vv.matrix, &sv.vector, &wv.vector);

  /*
   * Right now, B = U S V^t, where S = diag(s)
   *
   * The SVD routine may have computed reflection matrices U and V,
   * but it would be much nicer to have rotations since we won't have
   * to use BLAS mat-mat multiplications to update our matrices,
   * and can instead use drot. So convert them to rotations if
   * necessary
   */

  det = gsl_matrix_get(&vv.matrix, 0, 0) * gsl_matrix_get(&vv.matrix, 1, 1) -
        gsl_matrix_get(&vv.matrix, 0, 1) * gsl_matrix_get(&vv.matrix, 1, 0);
  if (det < 0.0)
    {
      /* V is a reflection, convert it to a rotation by inserting
       * F = [1 0; 0 -1] so that:
       *
       * B = U S [1  0] [1  0] V^t
       *         [0 -1] [0 -1]
       *
       * so S -> S F and V -> V F where F is the reflection matrix
       * We just need to invert S22 since the first column of V
       * will remain unchanged and we can just read off the CS and SN
       * parameters.
       */
      datS[1] = -datS[1];
    }

  cr = gsl_matrix_get(&vv.matrix, 0, 0);
  sr = gsl_matrix_get(&vv.matrix, 1, 0);

  /* same for U */
  det = gsl_matrix_get(&uv.matrix, 0, 0) * gsl_matrix_get(&uv.matrix, 1, 1) -
        gsl_matrix_get(&uv.matrix, 0, 1) * gsl_matrix_get(&uv.matrix, 1, 0);
  if (det < 0.0)
    datS[1] = -datS[1];

  cl = gsl_matrix_get(&uv.matrix, 0, 0);
  sl = gsl_matrix_get(&uv.matrix, 1, 0);

  B11 = gsl_vector_get(&sv.vector, 0);
  B22 = gsl_vector_get(&sv.vector, 1);

  /* make sure B11 is positive */
  if (B11 < 0.0)
    {
      B11 = -B11;
      B22 = -B22;
      cr = -cr;
      sr = -sr;
    }

  /*
   * At this point,
   *
   * [ S11  0  ] = [ CSL  SNL ] B [ CSR -SNR ]
   * [  0  S22 ]   [-SNL  CSL ]   [ SNR  CSR ]
   *
   * apply rotations to H and rest of R
   */

  if (w->compute_s)
    {
      xv = gsl_matrix_subrow(w->H, top, top, w->size - top);
      yv = gsl_matrix_subrow(w->H, top + 1, top, w->size - top);
      gsl_blas_drot(&xv.vector, &yv.vector, cl, sl);

      xv = gsl_matrix_subcolumn(w->H, top, 0, top + 2);
      yv = gsl_matrix_subcolumn(w->H, top + 1, 0, top + 2);
      gsl_blas_drot(&xv.vector, &yv.vector, cr, sr);
    }
  else
    {
      xv = gsl_matrix_row(A, 0);
      yv = gsl_matrix_row(A, 1);
      gsl_blas_drot(&xv.vector, &yv.vector, cl, sl);

      xv = gsl_matrix_column(A, 0);
      yv = gsl_matrix_column(A, 1);
      gsl_blas_drot(&xv.vector, &yv.vector, cr, sr);
    }

  if (w->compute_t)
    {
      if (top != (w->size - 2))
        {
          xv = gsl_matrix_subrow(w->R, top, top + 2, w->size - top - 2);
          yv = gsl_matrix_subrow(w->R, top + 1, top + 2, w->size - top - 2);
          gsl_blas_drot(&xv.vector, &yv.vector, cl, sl);
        }

      if (top != 0)
        {
          xv = gsl_matrix_subcolumn(w->R, top, 0, top);
          yv = gsl_matrix_subcolumn(w->R, top + 1, 0, top);
          gsl_blas_drot(&xv.vector, &yv.vector, cr, sr);
        }
    }

  if (w->Q)
    {
      xv = gsl_matrix_column(w->Q, top);
      yv = gsl_matrix_column(w->Q, top + 1);
      gsl_blas_drot(&xv.vector, &yv.vector, cl, sl);
    }

  if (w->Z)
    {
      xv = gsl_matrix_column(w->Z, top);
      yv = gsl_matrix_column(w->Z, top + 1);
      gsl_blas_drot(&xv.vector, &yv.vector, cr, sr);
    }

  gsl_matrix_set(B, 0, 0, B11);
  gsl_matrix_set(B, 0, 1, 0.0);
  gsl_matrix_set(B, 1, 0, 0.0);
  gsl_matrix_set(B, 1, 1, B22);

  /* if B22 is < 0, make it positive by negating its column */
  if (B22 < 0.0)
    {
      size_t i;

      if (w->compute_s)
        {
          for (i = 0; i < top + 2; ++i)
            gsl_matrix_set(w->H, i, top + 1, -gsl_matrix_get(w->H, i, top + 1));
        }
      else
        {
          gsl_matrix_set(A, 0, 1, -gsl_matrix_get(A, 0, 1));
          gsl_matrix_set(A, 1, 1, -gsl_matrix_get(A, 1, 1));
        }

      if (w->compute_t)
        {
          for (i = 0; i < top + 2; ++i)
            gsl_matrix_set(w->R, i, top + 1, -gsl_matrix_get(w->R, i, top + 1));
        }
      else
        {
          gsl_matrix_set(B, 0, 1, -gsl_matrix_get(B, 0, 1));
          gsl_matrix_set(B, 1, 1, -gsl_matrix_get(B, 1, 1));
        }

      if (w->Z)
        {
          xv = gsl_matrix_column(w->Z, top + 1);
          gsl_vector_scale(&xv.vector, -1.0);
        }
    }

  /* our block is now in standard form - compute eigenvalues */

  s = gen_compute_eigenvals(A, B, alpha1, alpha2, beta1, beta2);

  return s;
} /* gen_schur_standardize2() */

/*
gen_compute_eigenvals()
  Compute the complex eigenvalues of a 2-by-2 block

Return: GSL_CONTINUE if block contains real eigenvalues (they are not
        computed)
        GSL_SUCCESS on normal completion
*/

static int
gen_compute_eigenvals(gsl_matrix *A, gsl_matrix *B, gsl_complex *alpha1,
                      gsl_complex *alpha2, double *beta1, double *beta2)
{
  double wr1, wr2, wi, scale1, scale2;
  double s1inv;
  double A11, A12, A21, A22;
  double B11, B22;
  double c11r, c11i, c12, c21, c22r, c22i;
  double cz, cq;
  double szr, szi, sqr, sqi;
  double a1r, a1i, a2r, a2i, b1r, b1i, b1a, b2r, b2i, b2a;
  double alphar, alphai;
  double t1, an, bn, tempr, tempi, wabs;

  /*
   * This function is called from gen_schur_standardize2() and
   * its possible the standardization has perturbed the eigenvalues
   * onto the real line - so check for this before computing them
   */

  gsl_schur_gen_eigvals(A, B, &wr1, &wr2, &wi, &scale1, &scale2);
  if (wi == 0.0)
    return GSL_CONTINUE; /* real eigenvalues - continue QZ iteration */

  /* complex eigenvalues - compute alpha and beta */

  s1inv = 1.0 / scale1;

  A11 = gsl_matrix_get(A, 0, 0);
  A12 = gsl_matrix_get(A, 0, 1);
  A21 = gsl_matrix_get(A, 1, 0);
  A22 = gsl_matrix_get(A, 1, 1);

  B11 = gsl_matrix_get(B, 0, 0);
  B22 = gsl_matrix_get(B, 1, 1);

  c11r = scale1 * A11 - wr1 * B11;
  c11i = -wi * B11;
  c12 = scale1 * A12;
  c21 = scale1 * A21;
  c22r = scale1 * A22 - wr1 * B22;
  c22i = -wi * B22;

  if (fabs(c11r) + fabs(c11i) + fabs(c12) >
      fabs(c21) + fabs(c22r) + fabs(c22i))
    {
      t1 = gsl_hypot3(c12, c11r, c11i);
      if (t1 != 0.0)
        {
          cz = c12 / t1;
          szr = -c11r / t1;
          szi = -c11i / t1;
        }
      else
        {
          cz = 0.0;
          szr = 1.0;
          szi = 0.0;
        }
    }
  else
    {
      cz = hypot(c22r, c22i);
      if (cz <= GSL_DBL_MIN)
        {
          cz = 0.0;
          szr = 1.0;
          szi = 0.0;
        }
      else
        {
          tempr = c22r / cz;
          tempi = c22i / cz;
          t1 = hypot(cz, c21);
          cz /= t1;
          szr = -c21*tempr / t1;
          szi = c21*tempi / t1;
        }
    }

  an = fabs(A11) + fabs(A12) + fabs(A21) + fabs(A22);
  bn = fabs(B11) + fabs(B22);
  wabs = fabs(wr1) + fabs(wi);
  if (scale1*an > wabs*bn)
    {
      cq = cz * B11;
      if (cq <= GSL_DBL_MIN)
        {
          cq = 0.0;
          sqr = 1.0;
          sqi = 0.0;
        }
      else
        {
          sqr = szr * B22;
          sqi = -szi * B22;
        }
    }
  else
    {
      a1r = cz * A11 + szr * A12;
      a1i = szi * A12;
      a2r = cz * A21 + szr * A22;
      a2i = szi * A22;
      cq = hypot(a1r, a1i);
      if (cq <= GSL_DBL_MIN)
        {
          cq = 0.0;
          sqr = 1.0;
          sqi = 0.0;
        }
      else
        {
          tempr = a1r / cq;
          tempi = a1i / cq;
          sqr = tempr * a2r + tempi * a2i;
          sqi = tempi * a2r - tempr * a2i;
        }
    }

  t1 = gsl_hypot3(cq, sqr, sqi);
  cq /= t1;
  sqr /= t1;
  sqi /= t1;

  tempr = sqr*szr - sqi*szi;
  tempi = sqr*szi + sqi*szr;
  b1r = cq*cz*B11 + tempr*B22;
  b1i = tempi*B22;
  b1a = hypot(b1r, b1i);
  b2r = cq*cz*B22 + tempr*B11;
  b2i = -tempi*B11;
  b2a = hypot(b2r, b2i);

  *beta1 = b1a;
  *beta2 = b2a;
  
  alphar = (wr1 * b1a) * s1inv;
  alphai = (wi * b1a) * s1inv;
  GSL_SET_COMPLEX(alpha1, alphar, alphai);

  alphar = (wr1 * b2a) * s1inv;
  alphai = -(wi * b2a) * s1inv;
  GSL_SET_COMPLEX(alpha2, alphar, alphai);

  return GSL_SUCCESS;
} /* gen_compute_eigenvals() */

/*
gen_store_eigval1()
  Store eigenvalue of a 1-by-1 block into the alpha and beta
output vectors. This routine ensures that eigenvalues are stored
in the same order as they appear in the Schur form and updates
various internal workspace quantities.
*/

static void
gen_store_eigval1(const gsl_matrix *H, const double a, const double b,
                  gsl_vector_complex *alpha,
                  gsl_vector *beta, gsl_eigen_gen_workspace *w)
{
  size_t top = gen_get_submatrix(w->H, H);
  gsl_complex z;

  GSL_SET_COMPLEX(&z, a, 0.0);

  gsl_vector_complex_set(alpha, top, z);
  gsl_vector_set(beta, top, b);

  w->n_evals += 1;
  w->n_iter = 0;
  w->eshift = 0.0;
} /* gen_store_eigval1() */

/*
gen_store_eigval2()
  Store eigenvalues of a 2-by-2 block into the alpha and beta
output vectors. This routine ensures that eigenvalues are stored
in the same order as they appear in the Schur form and updates
various internal workspace quantities.
*/

static void
gen_store_eigval2(const gsl_matrix *H, const gsl_complex *alpha1,
                  const double beta1, const gsl_complex *alpha2,
                  const double beta2, gsl_vector_complex *alpha,
                  gsl_vector *beta, gsl_eigen_gen_workspace *w)
{
  size_t top = gen_get_submatrix(w->H, H);

  gsl_vector_complex_set(alpha, top, *alpha1);
  gsl_vector_set(beta, top, beta1);

  gsl_vector_complex_set(alpha, top + 1, *alpha2);
  gsl_vector_set(beta, top + 1, beta2);

  w->n_evals += 2;
  w->n_iter = 0;
  w->eshift = 0.0;
} /* gen_store_eigval2() */

/*
gen_get_submatrix()
  B is a submatrix of A. The goal of this function is to
compute the indices in A of where the matrix B resides
*/

static inline size_t
gen_get_submatrix(const gsl_matrix *A, const gsl_matrix *B)
{
  size_t diff;
  double ratio;
  size_t top;

  diff = (size_t) (B->data - A->data);

  /* B is on the diagonal of A, so measure distance in units of
     tda+1 */

  ratio = (double)diff / ((double) (A->tda + 1));

  top = (size_t) floor(ratio);

  return top;
} /* gen_get_submatrix() */

/* Frobenius norm */
inline static double
normF (gsl_matrix * A)
{
  size_t i, j, M = A->size1, N = A->size2;
  double sum = 0.0, scale = 0.0, ssq = 1.0;

  for (i = 0; i < M; i++)
    {
      for (j = 0; j < N; j++)
        {
          double Aij = gsl_matrix_get (A, i, j);

          if (Aij != 0.0)
            {
              double ax = fabs (Aij);

              if (scale < ax)
                {
                  ssq = 1.0 + ssq * (scale / ax) * (scale / ax);
                  scale = ax;
                }
              else
                {
                  ssq += (ax / scale) * (ax / scale);
                }
            }

        }
    }

  sum = scale * sqrt (ssq);

  return sum;
}

/* Generate a Givens rotation (cos,sin) which takes v=(x,y) to (|v|,0) 

   From Golub and Van Loan, "Matrix Computations", Section 5.1.8 */

inline static void
create_givens (const double a, const double b, double *c, double *s)
{
  if (b == 0)
    {
      *c = 1;
      *s = 0;
    }
  else if (fabs (b) > fabs (a))
    {
      double t = -a / b;
      double s1 = 1.0 / sqrt (1 + t * t);
      *s = s1;
      *c = s1 * t;
    }
  else
    {
      double t = -b / a;
      double c1 = 1.0 / sqrt (1 + t * t);
      *c = c1;
      *s = c1 * t;
    }
}
