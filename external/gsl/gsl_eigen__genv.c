/* eigen/genv.c
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
#include <math.h>

#include "gsl__config.h"
#include "gsl_eigen.h"
#include "gsl_linalg.h"
#include "gsl_math.h"
#include "gsl_blas.h"
#include "gsl_vector.h"
#include "gsl_vector_complex.h"
#include "gsl_matrix.h"
#include "gsl_errno.h"

/*
 * This module computes the eigenvalues and eigenvectors of a
 * real generalized eigensystem A x = \lambda B x. Left and right
 * Schur vectors are optionally computed as well.
 *
 * This file contains routines based on original code from LAPACK
 * which is distributed under the modified BSD license.
 */

static int genv_get_right_eigenvectors(const gsl_matrix *S,
                                       const gsl_matrix *T,
                                       gsl_matrix *Z,
                                       gsl_matrix_complex *evec,
                                       gsl_eigen_genv_workspace *w);
static void genv_normalize_eigenvectors(gsl_vector_complex *alpha,
                                        gsl_matrix_complex *evec);

/*
gsl_eigen_genv_alloc()
  Allocate a workspace for solving the generalized eigenvalue problem.
The size of this workspace is O(7n).

Inputs: n - size of matrices

Return: pointer to workspace
*/

gsl_eigen_genv_workspace *
gsl_eigen_genv_alloc(const size_t n)
{
  gsl_eigen_genv_workspace *w;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer",
                      GSL_EINVAL);
    }

  w = (gsl_eigen_genv_workspace *) calloc (1, sizeof (gsl_eigen_genv_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->size = n;
  w->Q = NULL;
  w->Z = NULL;

  w->gen_workspace_p = gsl_eigen_gen_alloc(n);

  if (w->gen_workspace_p == 0)
    {
      gsl_eigen_genv_free(w);
      GSL_ERROR_NULL ("failed to allocate space for gen workspace", GSL_ENOMEM);
    }

  /* compute the full Schur forms */
  gsl_eigen_gen_params(1, 1, 1, w->gen_workspace_p);

  w->work1 = gsl_vector_alloc(n);
  w->work2 = gsl_vector_alloc(n);
  w->work3 = gsl_vector_alloc(n);
  w->work4 = gsl_vector_alloc(n);
  w->work5 = gsl_vector_alloc(n);
  w->work6 = gsl_vector_alloc(n);

  if (w->work1 == 0 || w->work2 == 0 || w->work3 == 0 ||
      w->work4 == 0 || w->work5 == 0 || w->work6 == 0)
    {
      gsl_eigen_genv_free(w);
      GSL_ERROR_NULL ("failed to allocate space for additional workspace", GSL_ENOMEM);
    }

  return (w);
} /* gsl_eigen_genv_alloc() */

/*
gsl_eigen_genv_free()
  Free workspace w
*/

void
gsl_eigen_genv_free(gsl_eigen_genv_workspace *w)
{
  if (w->gen_workspace_p)
    gsl_eigen_gen_free(w->gen_workspace_p);

  if (w->work1)
    gsl_vector_free(w->work1);

  if (w->work2)
    gsl_vector_free(w->work2);

  if (w->work3)
    gsl_vector_free(w->work3);

  if (w->work4)
    gsl_vector_free(w->work4);

  if (w->work5)
    gsl_vector_free(w->work5);

  if (w->work6)
    gsl_vector_free(w->work6);

  free(w);
} /* gsl_eigen_genv_free() */

/*
gsl_eigen_genv()

Solve the generalized eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda and right eigenvectors x.

Inputs: A     - general real matrix
        B     - general real matrix
        alpha - (output) where to store eigenvalue numerators
        beta  - (output) where to store eigenvalue denominators
        evec  - (output) where to store eigenvectors
        w     - workspace

Return: success or error
*/

int
gsl_eigen_genv (gsl_matrix * A, gsl_matrix * B, gsl_vector_complex * alpha,
                gsl_vector * beta, gsl_matrix_complex *evec,
                gsl_eigen_genv_workspace * w)
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
  else if (evec->size1 != N)
    {
      GSL_ERROR ("eigenvector matrix has wrong size", GSL_EBADLEN);
    }
  else
    {
      int s;
      gsl_matrix Z;

      /*
       * We need a place to store the right Schur vectors, so we will
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

      s = gsl_eigen_gen_QZ(A, B, alpha, beta, w->Q, &Z, w->gen_workspace_p);

      if (w->Z)
        {
          /* save right Schur vectors */
          gsl_matrix_memcpy(w->Z, &Z);
        }

      /* only compute eigenvectors if we found all eigenvalues */
      if (s == GSL_SUCCESS)
        {
          /* compute eigenvectors */
          s = genv_get_right_eigenvectors(A, B, &Z, evec, w);

          if (s == GSL_SUCCESS)
            genv_normalize_eigenvectors(alpha, evec);
        }

      return s;
    }
} /* gsl_eigen_genv() */

/*
gsl_eigen_genv_QZ()

Solve the generalized eigenvalue problem

A x = \lambda B x

for the eigenvalues \lambda and right eigenvectors x. Optionally
compute left and/or right Schur vectors Q and Z which satisfy:

A = Q S Z^t
B = Q T Z^t

where (S, T) is the generalized Schur form of (A, B)

Inputs: A     - general real matrix
        B     - general real matrix
        alpha - (output) where to store eigenvalue numerators
        beta  - (output) where to store eigenvalue denominators
        evec  - (output) where to store eigenvectors
        Q     - (output) if non-null, where to store left Schur vectors
        Z     - (output) if non-null, where to store right Schur vectors
        w     - workspace

Return: success or error
*/

int
gsl_eigen_genv_QZ (gsl_matrix * A, gsl_matrix * B,
                   gsl_vector_complex * alpha, gsl_vector * beta,
                   gsl_matrix_complex * evec,
                   gsl_matrix * Q, gsl_matrix * Z,
                   gsl_eigen_genv_workspace * w)
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

      s = gsl_eigen_genv(A, B, alpha, beta, evec, w);

      w->Q = NULL;
      w->Z = NULL;

      return s;
    }
} /* gsl_eigen_genv_QZ() */

/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

/*
genv_get_right_eigenvectors()
  Compute right eigenvectors of the Schur form (S, T) and then
backtransform them using the right Schur vectors to get right
eigenvectors of the original system.

Inputs: S     - upper quasi-triangular Schur form of A
        T     - upper triangular Schur form of B
        Z     - right Schur vectors
        evec  - (output) where to store eigenvectors
        w     - workspace

Return: success or error

Notes: 1) based on LAPACK routine DTGEVC
       2) eigenvectors are stored in the order that their
          eigenvalues appear in the Schur form
*/

static int
genv_get_right_eigenvectors(const gsl_matrix *S, const gsl_matrix *T,
                            gsl_matrix *Z,
                            gsl_matrix_complex *evec,
                            gsl_eigen_genv_workspace *w)
{
  const size_t N = w->size;
  const double small = GSL_DBL_MIN * N / GSL_DBL_EPSILON;
  const double big = 1.0 / small;
  const double bignum = 1.0 / (GSL_DBL_MIN * N);
  size_t i, j, k, end;
  int is;
  double anorm, bnorm;
  double temp, temp2, temp2r, temp2i;
  double ascale, bscale;
  double salfar, sbeta;
  double acoef, bcoefr, bcoefi, acoefa, bcoefa;
  double creala, cimaga, crealb, cimagb, cre2a, cim2a, cre2b, cim2b;
  double dmin, xmax;
  double scale;
  size_t nw, na;
  int lsa, lsb;
  int complex_pair;
  gsl_complex z_zero, z_one;
  double bdiag[2] = { 0.0, 0.0 };
  double sum[4];
  int il2by2;
  size_t jr, jc, ja;
  double xscale;
  gsl_vector_complex_view ecol;
  gsl_vector_view re, im, re2, im2;

  GSL_SET_COMPLEX(&z_zero, 0.0, 0.0);
  GSL_SET_COMPLEX(&z_one, 1.0, 0.0);

  /*
   * Compute the 1-norm of each column of (S, T) excluding elements
   * belonging to the diagonal blocks to check for possible overflow
   * in the triangular solver
   */

  anorm = fabs(gsl_matrix_get(S, 0, 0));
  if (N > 1)
    anorm += fabs(gsl_matrix_get(S, 1, 0));
  bnorm = fabs(gsl_matrix_get(T, 0, 0));

  gsl_vector_set(w->work1, 0, 0.0);
  gsl_vector_set(w->work2, 0, 0.0);

  for (j = 1; j < N; ++j)
    {
      temp = temp2 = 0.0;
      if (gsl_matrix_get(S, j, j - 1) == 0.0)
        end = j;
      else
        end = j - 1;

      for (i = 0; i < end; ++i)
        {
          temp += fabs(gsl_matrix_get(S, i, j));
          temp2 += fabs(gsl_matrix_get(T, i, j));
        }

      gsl_vector_set(w->work1, j, temp);
      gsl_vector_set(w->work2, j, temp2);

      for (i = end; i < GSL_MIN(j + 2, N); ++i)
        {
          temp += fabs(gsl_matrix_get(S, i, j));
          temp2 += fabs(gsl_matrix_get(T, i, j));
        }

      anorm = GSL_MAX(anorm, temp);
      bnorm = GSL_MAX(bnorm, temp2);
    }

  ascale = 1.0 / GSL_MAX(anorm, GSL_DBL_MIN);
  bscale = 1.0 / GSL_MAX(bnorm, GSL_DBL_MIN);

  complex_pair = 0;
  for (k = 0; k < N; ++k)
    {
      size_t je = N - 1 - k;

      if (complex_pair)
        {
          complex_pair = 0;
          continue;
        }

      nw = 1;
      if (je > 0)
        {
          if (gsl_matrix_get(S, je, je - 1) != 0.0)
            {
              complex_pair = 1;
              nw = 2;
            }
        }

      if (!complex_pair)
        {
          if (fabs(gsl_matrix_get(S, je, je)) <= GSL_DBL_MIN &&
              fabs(gsl_matrix_get(T, je, je)) <= GSL_DBL_MIN)
            {
              /* singular matrix pencil - unit eigenvector */
              for (i = 0; i < N; ++i)
                gsl_matrix_complex_set(evec, i, je, z_zero);

              gsl_matrix_complex_set(evec, je, je, z_one);

              continue;
            }

          /* clear vector */
          for (i = 0; i < N; ++i)
            gsl_vector_set(w->work3, i, 0.0);
        }
      else
        {
          /* clear vectors */
          for (i = 0; i < N; ++i)
            {
              gsl_vector_set(w->work3, i, 0.0);
              gsl_vector_set(w->work4, i, 0.0);
            }
        }

      if (!complex_pair)
        {
          /* real eigenvalue */

          temp = 1.0 / GSL_MAX(GSL_DBL_MIN,
                               GSL_MAX(fabs(gsl_matrix_get(S, je, je)) * ascale,
                                       fabs(gsl_matrix_get(T, je, je)) * bscale));
          salfar = (temp * gsl_matrix_get(S, je, je)) * ascale;
          sbeta = (temp * gsl_matrix_get(T, je, je)) * bscale;
          acoef = sbeta * ascale;
          bcoefr = salfar * bscale;
          bcoefi = 0.0;

          /* scale to avoid underflow */
          scale = 1.0;
          lsa = fabs(sbeta) >= GSL_DBL_MIN && fabs(acoef) < small;
          lsb = fabs(salfar) >= GSL_DBL_MIN && fabs(bcoefr) < small;
          if (lsa)
            scale = (small / fabs(sbeta)) * GSL_MIN(anorm, big);
          if (lsb)
            scale = GSL_MAX(scale, (small / fabs(salfar)) * GSL_MIN(bnorm, big));

          if (lsa || lsb)
            {
              scale = GSL_MIN(scale,
                        1.0 / (GSL_DBL_MIN *
                               GSL_MAX(1.0,
                                 GSL_MAX(fabs(acoef), fabs(bcoefr)))));
              if (lsa)
                acoef = ascale * (scale * sbeta);
              else
                acoef *= scale;

              if (lsb)
                bcoefr = bscale * (scale * salfar);
              else
                bcoefr *= scale;
            }

          acoefa = fabs(acoef);
          bcoefa = fabs(bcoefr);

          /* first component is 1 */
          gsl_vector_set(w->work3, je, 1.0);
          xmax = 1.0;

          /* compute contribution from column je of A and B to sum */

          for (i = 0; i < je; ++i)
            {
              gsl_vector_set(w->work3, i,
                bcoefr*gsl_matrix_get(T, i, je) -
                acoef * gsl_matrix_get(S, i, je));
            }
        }
      else
        {
          gsl_matrix_const_view vs =
            gsl_matrix_const_submatrix(S, je - 1, je - 1, 2, 2);
          gsl_matrix_const_view vt =
            gsl_matrix_const_submatrix(T, je - 1, je - 1, 2, 2);

          /* complex eigenvalue */

          gsl_schur_gen_eigvals(&vs.matrix,
                                &vt.matrix,
                                &bcoefr,
                                &temp2,
                                &bcoefi,
                                &acoef,
                                &temp);
          if (bcoefi == 0.0)
            {
              GSL_ERROR("gsl_schur_gen_eigvals failed on complex block", GSL_FAILURE);
            }

          /* scale to avoid over/underflow */
          acoefa = fabs(acoef);
          bcoefa = fabs(bcoefr) + fabs(bcoefi);
          scale = 1.0;

          if (acoefa*GSL_DBL_EPSILON < GSL_DBL_MIN && acoefa >= GSL_DBL_MIN)
            scale = (GSL_DBL_MIN / GSL_DBL_EPSILON) / acoefa;
          if (bcoefa*GSL_DBL_EPSILON < GSL_DBL_MIN && bcoefa >= GSL_DBL_MIN)
            scale = GSL_MAX(scale, (GSL_DBL_MIN/GSL_DBL_EPSILON) / bcoefa);
          if (GSL_DBL_MIN*acoefa > ascale)
            scale = ascale / (GSL_DBL_MIN * acoefa);
          if (GSL_DBL_MIN*bcoefa > bscale)
            scale = GSL_MIN(scale, bscale / (GSL_DBL_MIN*bcoefa));
          if (scale != 1.0)
            {
              acoef *= scale;
              acoefa = fabs(acoef);
              bcoefr *= scale;
              bcoefi *= scale;
              bcoefa = fabs(bcoefr) + fabs(bcoefi);
            }

          /* compute first two components of eigenvector */

          temp = acoef * gsl_matrix_get(S, je, je - 1);
          temp2r = acoef * gsl_matrix_get(S, je, je) -
                   bcoefr * gsl_matrix_get(T, je, je);
          temp2i = -bcoefi * gsl_matrix_get(T, je, je);

          if (fabs(temp) >= fabs(temp2r) + fabs(temp2i))
            {
              gsl_vector_set(w->work3, je, 1.0);
              gsl_vector_set(w->work4, je, 0.0);
              gsl_vector_set(w->work3, je - 1, -temp2r / temp);
              gsl_vector_set(w->work4, je - 1, -temp2i / temp);
            }
          else
            {
              gsl_vector_set(w->work3, je - 1, 1.0);
              gsl_vector_set(w->work4, je - 1, 0.0);
              temp = acoef * gsl_matrix_get(S, je - 1, je);
              gsl_vector_set(w->work3, je,
                (bcoefr*gsl_matrix_get(T, je - 1, je - 1) -
                 acoef*gsl_matrix_get(S, je - 1, je - 1)) / temp);
              gsl_vector_set(w->work4, je,
                bcoefi*gsl_matrix_get(T, je - 1, je - 1) / temp);
            }

          xmax = GSL_MAX(fabs(gsl_vector_get(w->work3, je)) +
                         fabs(gsl_vector_get(w->work4, je)),
                         fabs(gsl_vector_get(w->work3, je - 1)) +
                         fabs(gsl_vector_get(w->work4, je - 1)));

          /* compute contribution from column je and je - 1 */

          creala = acoef * gsl_vector_get(w->work3, je - 1);
          cimaga = acoef * gsl_vector_get(w->work4, je - 1);
          crealb = bcoefr * gsl_vector_get(w->work3, je - 1) -
                   bcoefi * gsl_vector_get(w->work4, je - 1);
          cimagb = bcoefi * gsl_vector_get(w->work3, je - 1) +
                   bcoefr * gsl_vector_get(w->work4, je - 1);
          cre2a = acoef * gsl_vector_get(w->work3, je);
          cim2a = acoef * gsl_vector_get(w->work4, je);
          cre2b = bcoefr * gsl_vector_get(w->work3, je) -
                  bcoefi * gsl_vector_get(w->work4, je);
          cim2b = bcoefi * gsl_vector_get(w->work3, je) +
                  bcoefr * gsl_vector_get(w->work4, je);

          for (i = 0; i < je - 1; ++i)
            {
              gsl_vector_set(w->work3, i,
                -creala * gsl_matrix_get(S, i, je - 1) +
                crealb * gsl_matrix_get(T, i, je - 1) -
                cre2a * gsl_matrix_get(S, i, je) +
                cre2b * gsl_matrix_get(T, i, je));
              gsl_vector_set(w->work4, i,
                -cimaga * gsl_matrix_get(S, i, je - 1) +
                cimagb * gsl_matrix_get(T, i, je - 1) -
                cim2a * gsl_matrix_get(S, i, je) +
                cim2b * gsl_matrix_get(T, i, je));
            }
        }

      dmin = GSL_MAX(GSL_DBL_MIN,
               GSL_MAX(GSL_DBL_EPSILON*acoefa*anorm,
                       GSL_DBL_EPSILON*bcoefa*bnorm));

      /* triangular solve of (a A - b B) x = 0 */

      il2by2 = 0;
      for (is = (int) je - (int) nw; is >= 0; --is)
        {
          j = (size_t) is;

          if (!il2by2 && j > 0)
            {
              if (gsl_matrix_get(S, j, j - 1) != 0.0)
                {
                  il2by2 = 1;
                  continue;
                }
            }

          bdiag[0] = gsl_matrix_get(T, j, j);
          if (il2by2)
            {
              na = 2;
              bdiag[1] = gsl_matrix_get(T, j + 1, j + 1);
            }
          else
            na = 1;


          if (nw == 1)
            {
              gsl_matrix_const_view sv =
                gsl_matrix_const_submatrix(S, j, j, na, na);
              gsl_vector_view xv, bv;

              bv = gsl_vector_subvector(w->work3, j, na);

              /*
               * the loop below expects the solution in the first column
               * of sum, so set stride to 2
               */
              xv = gsl_vector_view_array_with_stride(sum, 2, na);

              gsl_schur_solve_equation(acoef,
                                       &sv.matrix,
                                       bcoefr,
                                       bdiag[0],
                                       bdiag[1],
                                       &bv.vector,
                                       &xv.vector,
                                       &scale,
                                       &temp,
                                       dmin);
            }
          else
            {
              double bdat[4];
              gsl_matrix_const_view sv =
                gsl_matrix_const_submatrix(S, j, j, na, na);
              gsl_vector_complex_view xv =
                gsl_vector_complex_view_array(sum, na);
              gsl_vector_complex_view bv =
                gsl_vector_complex_view_array(bdat, na);
              gsl_complex z;

              bdat[0] = gsl_vector_get(w->work3, j);
              bdat[1] = gsl_vector_get(w->work4, j);
              if (na == 2)
                {
                  bdat[2] = gsl_vector_get(w->work3, j + 1);
                  bdat[3] = gsl_vector_get(w->work4, j + 1);
                }

              GSL_SET_COMPLEX(&z, bcoefr, bcoefi);

              gsl_schur_solve_equation_z(acoef,
                                         &sv.matrix,
                                         &z,
                                         bdiag[0],
                                         bdiag[1],
                                         &bv.vector,
                                         &xv.vector,
                                         &scale,
                                         &temp,
                                         dmin);
            }

          if (scale < 1.0)
            {
              for (jr = 0; jr <= je; ++jr)
                {
                  gsl_vector_set(w->work3, jr,
                    scale * gsl_vector_get(w->work3, jr));
                  if (nw == 2)
                    {
                      gsl_vector_set(w->work4, jr,
                        scale * gsl_vector_get(w->work4, jr));
                    }
                }
            }

          xmax = GSL_MAX(scale * xmax, temp);

          for (jr = 0; jr < na; ++jr)
            {
              gsl_vector_set(w->work3, j + jr, sum[jr*na]);
              if (nw == 2)
                gsl_vector_set(w->work4, j + jr, sum[jr*na + 1]);
            }

          if (j > 0)
            {
              xscale = 1.0 / GSL_MAX(1.0, xmax);
              temp = acoefa * gsl_vector_get(w->work1, j) +
                     bcoefa * gsl_vector_get(w->work2, j);
              if (il2by2)
                {
                  temp = GSL_MAX(temp,
                           acoefa * gsl_vector_get(w->work1, j + 1) +
                           bcoefa * gsl_vector_get(w->work2, j + 1));
                }

              temp = GSL_MAX(temp, GSL_MAX(acoefa, bcoefa));
              if (temp > bignum * xscale)
                {
                  for (jr = 0; jr <= je; ++jr)
                    {
                      gsl_vector_set(w->work3, jr,
                        xscale * gsl_vector_get(w->work3, jr));
                      if (nw == 2)
                        {
                          gsl_vector_set(w->work4, jr,
                            xscale * gsl_vector_get(w->work4, jr));
                        }
                    }
                  xmax *= xscale;
                }

              for (ja = 0; ja < na; ++ja)
                {
                  if (complex_pair)
                    {
                      creala = acoef * gsl_vector_get(w->work3, j + ja);
                      cimaga = acoef * gsl_vector_get(w->work4, j + ja);
                      crealb = bcoefr * gsl_vector_get(w->work3, j + ja) -
                               bcoefi * gsl_vector_get(w->work4, j + ja);
                      cimagb = bcoefi * gsl_vector_get(w->work3, j + ja) +
                               bcoefr * gsl_vector_get(w->work4, j + ja);
                      for (jr = 0; jr <= j - 1; ++jr)
                        {
                          gsl_vector_set(w->work3, jr,
                            gsl_vector_get(w->work3, jr) -
                            creala * gsl_matrix_get(S, jr, j + ja) +
                            crealb * gsl_matrix_get(T, jr, j + ja));
                          gsl_vector_set(w->work4, jr,
                            gsl_vector_get(w->work4, jr) -
                            cimaga * gsl_matrix_get(S, jr, j + ja) +
                            cimagb * gsl_matrix_get(T, jr, j + ja));
                        }
                    }
                  else
                    {
                      creala = acoef * gsl_vector_get(w->work3, j + ja);
                      crealb = bcoefr * gsl_vector_get(w->work3, j + ja);
                      for (jr = 0; jr <= j - 1; ++jr)
                        {
                          gsl_vector_set(w->work3, jr,
                            gsl_vector_get(w->work3, jr) -
                            creala * gsl_matrix_get(S, jr, j + ja) +
                            crealb * gsl_matrix_get(T, jr, j + ja));
                        }
                    } /* if (!complex_pair) */
                } /* for (ja = 0; ja < na; ++ja) */
            } /* if (j > 0) */

          il2by2 = 0;
        } /* for (i = 0; i < je - nw; ++i) */

      for (jr = 0; jr < N; ++jr)
        {
          gsl_vector_set(w->work5, jr,
            gsl_vector_get(w->work3, 0) * gsl_matrix_get(Z, jr, 0));
          if (nw == 2)
            {
              gsl_vector_set(w->work6, jr,
                gsl_vector_get(w->work4, 0) * gsl_matrix_get(Z, jr, 0));
            }
        }

      for (jc = 1; jc <= je; ++jc)
        {
          for (jr = 0; jr < N; ++jr)
            {
              gsl_vector_set(w->work5, jr,
                gsl_vector_get(w->work5, jr) +
                gsl_vector_get(w->work3, jc) * gsl_matrix_get(Z, jr, jc));
              if (nw == 2)
                {
                  gsl_vector_set(w->work6, jr,
                    gsl_vector_get(w->work6, jr) +
                    gsl_vector_get(w->work4, jc) * gsl_matrix_get(Z, jr, jc));
                }
            }
        }

      /* store the eigenvector */

      if (complex_pair)
        {
          ecol = gsl_matrix_complex_column(evec, je - 1);
          re = gsl_vector_complex_real(&ecol.vector);
          im = gsl_vector_complex_imag(&ecol.vector);

          ecol = gsl_matrix_complex_column(evec, je);
          re2 = gsl_vector_complex_real(&ecol.vector);
          im2 = gsl_vector_complex_imag(&ecol.vector);
        }
      else
        {
          ecol = gsl_matrix_complex_column(evec, je);
          re = gsl_vector_complex_real(&ecol.vector);
          im = gsl_vector_complex_imag(&ecol.vector);
        }

      for (jr = 0; jr < N; ++jr)
        {
          gsl_vector_set(&re.vector, jr, gsl_vector_get(w->work5, jr));
          if (complex_pair)
            {
              gsl_vector_set(&im.vector, jr, gsl_vector_get(w->work6, jr));
              gsl_vector_set(&re2.vector, jr, gsl_vector_get(w->work5, jr));
              gsl_vector_set(&im2.vector, jr, -gsl_vector_get(w->work6, jr));
            }
          else
            {
              gsl_vector_set(&im.vector, jr, 0.0);
            }
        }

      /* scale eigenvector */
      xmax = 0.0;
      if (complex_pair)
        {
          for (j = 0; j < N; ++j)
            {
              xmax = GSL_MAX(xmax,
                             fabs(gsl_vector_get(&re.vector, j)) +
                             fabs(gsl_vector_get(&im.vector, j)));
            }
        }
      else
        {
          for (j = 0; j < N; ++j)
            {
              xmax = GSL_MAX(xmax, fabs(gsl_vector_get(&re.vector, j)));
            }
        }

      if (xmax > GSL_DBL_MIN)
        {
          xscale = 1.0 / xmax;
          for (j = 0; j < N; ++j)
            {
              gsl_vector_set(&re.vector, j,
                             gsl_vector_get(&re.vector, j) * xscale);
              if (complex_pair)
                {
                  gsl_vector_set(&im.vector, j,
                                 gsl_vector_get(&im.vector, j) * xscale);
                  gsl_vector_set(&re2.vector, j,
                                 gsl_vector_get(&re2.vector, j) * xscale);
                  gsl_vector_set(&im2.vector, j,
                                 gsl_vector_get(&im2.vector, j) * xscale);
                }
            }
        }
    } /* for (k = 0; k < N; ++k) */

  return GSL_SUCCESS;
} /* genv_get_right_eigenvectors() */

/*
genv_normalize_eigenvectors()
  Normalize eigenvectors so that their Euclidean norm is 1

Inputs: alpha - eigenvalue numerators
        evec  - eigenvectors
*/

static void
genv_normalize_eigenvectors(gsl_vector_complex *alpha,
                            gsl_matrix_complex *evec)
{
  const size_t N = evec->size1;
  size_t i;     /* looping */
  gsl_complex ai;
  gsl_vector_complex_view vi;
  gsl_vector_view re, im;
  double scale; /* scaling factor */

  for (i = 0; i < N; ++i)
    {
      ai = gsl_vector_complex_get(alpha, i);
      vi = gsl_matrix_complex_column(evec, i);

      re = gsl_vector_complex_real(&vi.vector);

      if (GSL_IMAG(ai) == 0.0)
        {
          scale = 1.0 / gsl_blas_dnrm2(&re.vector);
          gsl_blas_dscal(scale, &re.vector);
        }
      else if (GSL_IMAG(ai) > 0.0)
        {
          im = gsl_vector_complex_imag(&vi.vector);

          scale = 1.0 / gsl_hypot(gsl_blas_dnrm2(&re.vector),
                                  gsl_blas_dnrm2(&im.vector));
          gsl_blas_zdscal(scale, &vi.vector);

          vi = gsl_matrix_complex_column(evec, i + 1);
          gsl_blas_zdscal(scale, &vi.vector);
        }
    }
} /* genv_normalize_eigenvectors() */
