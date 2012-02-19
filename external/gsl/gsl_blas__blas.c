/* blas/blas.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Gerard Jungman & Brian 
 * Gough
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

/* GSL implementation of BLAS operations for vectors and dense
 * matrices.  Note that GSL native storage is row-major.  */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_cblas.h"
#include "gsl_cblas.h"
#include "gsl_blas_types.h"
#include "gsl_blas.h"

/* ========================================================================
 * Level 1
 * ========================================================================
 */

/* CBLAS defines vector sizes in terms of int. GSL defines sizes in
   terms of size_t, so we need to convert these into integers.  There
   is the possibility of overflow here. FIXME: Maybe this could be
   caught */

#define INT(X) ((int)(X))

int
gsl_blas_sdsdot (float alpha, const gsl_vector_float * X,
                 const gsl_vector_float * Y, float *result)
{
  if (X->size == Y->size)
    {
      *result =
        cblas_sdsdot (INT (X->size), alpha, X->data, INT (X->stride), Y->data,
                      INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_dsdot (const gsl_vector_float * X, const gsl_vector_float * Y,
                double *result)
{
  if (X->size == Y->size)
    {
      *result =
        cblas_dsdot (INT (X->size), X->data, INT (X->stride), Y->data,
                     INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_sdot (const gsl_vector_float * X, const gsl_vector_float * Y,
               float *result)
{
  if (X->size == Y->size)
    {
      *result =
        cblas_sdot (INT (X->size), X->data, INT (X->stride), Y->data,
                    INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_ddot (const gsl_vector * X, const gsl_vector * Y, double *result)
{
  if (X->size == Y->size)
    {
      *result =
        cblas_ddot (INT (X->size), X->data, INT (X->stride), Y->data,
                    INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_cdotu (const gsl_vector_complex_float * X,
                const gsl_vector_complex_float * Y, gsl_complex_float * dotu)
{
  if (X->size == Y->size)
    {
      cblas_cdotu_sub (INT (X->size), X->data, INT (X->stride), Y->data,
                       INT (Y->stride), GSL_COMPLEX_P (dotu));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_cdotc (const gsl_vector_complex_float * X,
                const gsl_vector_complex_float * Y, gsl_complex_float * dotc)
{
  if (X->size == Y->size)
    {
      cblas_cdotc_sub (INT (X->size), X->data, INT (X->stride), Y->data,
                       INT (Y->stride), GSL_COMPLEX_P (dotc));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_zdotu (const gsl_vector_complex * X, const gsl_vector_complex * Y,
                gsl_complex * dotu)
{
  if (X->size == Y->size)
    {
      cblas_zdotu_sub (INT (X->size), X->data, INT (X->stride), Y->data,
                       INT (Y->stride), GSL_COMPLEX_P (dotu));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_zdotc (const gsl_vector_complex * X, const gsl_vector_complex * Y,
                gsl_complex * dotc)
{
  if (X->size == Y->size)
    {
      cblas_zdotc_sub (INT (X->size), X->data, INT (X->stride), Y->data,
                       INT (Y->stride), GSL_COMPLEX_P (dotc));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

/* Norms of vectors */

float
gsl_blas_snrm2 (const gsl_vector_float * X)
{
  return cblas_snrm2 (INT (X->size), X->data, INT (X->stride));
}

double
gsl_blas_dnrm2 (const gsl_vector * X)
{
  return cblas_dnrm2 (INT (X->size), X->data, INT (X->stride));
}

float
gsl_blas_scnrm2 (const gsl_vector_complex_float * X)
{
  return cblas_scnrm2 (INT (X->size), X->data, INT (X->stride));
}

double
gsl_blas_dznrm2 (const gsl_vector_complex * X)
{
  return cblas_dznrm2 (INT (X->size), X->data, INT (X->stride));
}

/* Absolute sums of vectors */

float
gsl_blas_sasum (const gsl_vector_float * X)
{
  return cblas_sasum (INT (X->size), X->data, INT (X->stride));
}

double
gsl_blas_dasum (const gsl_vector * X)
{
  return cblas_dasum (INT (X->size), X->data, INT (X->stride));
}

float
gsl_blas_scasum (const gsl_vector_complex_float * X)
{
  return cblas_scasum (INT (X->size), X->data, INT (X->stride));
}

double
gsl_blas_dzasum (const gsl_vector_complex * X)
{
  return cblas_dzasum (INT (X->size), X->data, INT (X->stride));
}

/* Maximum elements of vectors */

CBLAS_INDEX_t
gsl_blas_isamax (const gsl_vector_float * X)
{
  return cblas_isamax (INT (X->size), X->data, INT (X->stride));
}

CBLAS_INDEX_t
gsl_blas_idamax (const gsl_vector * X)
{
  return cblas_idamax (INT (X->size), X->data, INT (X->stride));
}

CBLAS_INDEX_t
gsl_blas_icamax (const gsl_vector_complex_float * X)
{
  return cblas_icamax (INT (X->size), X->data, INT (X->stride));
}

CBLAS_INDEX_t
gsl_blas_izamax (const gsl_vector_complex * X)
{
  return cblas_izamax (INT (X->size), X->data, INT (X->stride));
}


/* Swap vectors */

int
gsl_blas_sswap (gsl_vector_float * X, gsl_vector_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_sswap (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_dswap (gsl_vector * X, gsl_vector * Y)
{
  if (X->size == Y->size)
    {
      cblas_dswap (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    };
}

int
gsl_blas_cswap (gsl_vector_complex_float * X, gsl_vector_complex_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_cswap (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_zswap (gsl_vector_complex * X, gsl_vector_complex * Y)
{
  if (X->size == Y->size)
    {
      cblas_zswap (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* Copy vectors */

int
gsl_blas_scopy (const gsl_vector_float * X, gsl_vector_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_scopy (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_dcopy (const gsl_vector * X, gsl_vector * Y)
{
  if (X->size == Y->size)
    {
      cblas_dcopy (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_ccopy (const gsl_vector_complex_float * X,
                gsl_vector_complex_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_ccopy (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_zcopy (const gsl_vector_complex * X, gsl_vector_complex * Y)
{
  if (X->size == Y->size)
    {
      cblas_zcopy (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* Compute Y = alpha X + Y */

int
gsl_blas_saxpy (float alpha, const gsl_vector_float * X, gsl_vector_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_saxpy (INT (X->size), alpha, X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_daxpy (double alpha, const gsl_vector * X, gsl_vector * Y)
{
  if (X->size == Y->size)
    {
      cblas_daxpy (INT (X->size), alpha, X->data, INT (X->stride), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_caxpy (const gsl_complex_float alpha,
                const gsl_vector_complex_float * X,
                gsl_vector_complex_float * Y)
{
  if (X->size == Y->size)
    {
      cblas_caxpy (INT (X->size), GSL_COMPLEX_P (&alpha), X->data,
                   INT (X->stride), Y->data, INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_zaxpy (const gsl_complex alpha, const gsl_vector_complex * X,
                gsl_vector_complex * Y)
{
  if (X->size == Y->size)
    {
      cblas_zaxpy (INT (X->size), GSL_COMPLEX_P (&alpha), X->data,
                   INT (X->stride), Y->data, INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

/* Generate rotation */

int
gsl_blas_srotg (float a[], float b[], float c[], float s[])
{
  cblas_srotg (a, b, c, s);
  return GSL_SUCCESS;
}

int
gsl_blas_drotg (double a[], double b[], double c[], double s[])
{
  cblas_drotg (a, b, c, s);
  return GSL_SUCCESS;
}

/* Apply rotation to vectors */

int
gsl_blas_srot (gsl_vector_float * X, gsl_vector_float * Y, float c, float s)
{
  if (X->size == Y->size)
    {
      cblas_srot (INT (X->size), X->data, INT (X->stride), Y->data,
                  INT (Y->stride), c, s);
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_drot (gsl_vector * X, gsl_vector * Y, const double c, const double s)
{
  if (X->size == Y->size)
    {
      cblas_drot (INT (X->size), X->data, INT (X->stride), Y->data,
                  INT (Y->stride), c, s);
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* Generate modified rotation */

int
gsl_blas_srotmg (float d1[], float d2[], float b1[], float b2, float P[])
{
  cblas_srotmg (d1, d2, b1, b2, P);
  return GSL_SUCCESS;
}

int
gsl_blas_drotmg (double d1[], double d2[], double b1[], double b2, double P[])
{
  cblas_drotmg (d1, d2, b1, b2, P);
  return GSL_SUCCESS;
}


/* Apply modified rotation */

int
gsl_blas_srotm (gsl_vector_float * X, gsl_vector_float * Y, const float P[])
{
  if (X->size == Y->size)
    {
      cblas_srotm (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride), P);
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_drotm (gsl_vector * X, gsl_vector * Y, const double P[])
{
  if (X->size != Y->size)
    {
      cblas_drotm (INT (X->size), X->data, INT (X->stride), Y->data,
                   INT (Y->stride), P);
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* Scale vector */

void
gsl_blas_sscal (float alpha, gsl_vector_float * X)
{
  cblas_sscal (INT (X->size), alpha, X->data, INT (X->stride));
}

void
gsl_blas_dscal (double alpha, gsl_vector * X)
{
  cblas_dscal (INT (X->size), alpha, X->data, INT (X->stride));
}

void
gsl_blas_cscal (const gsl_complex_float alpha, gsl_vector_complex_float * X)
{
  cblas_cscal (INT (X->size), GSL_COMPLEX_P (&alpha), X->data,
               INT (X->stride));
}

void
gsl_blas_zscal (const gsl_complex alpha, gsl_vector_complex * X)
{
  cblas_zscal (INT (X->size), GSL_COMPLEX_P (&alpha), X->data,
               INT (X->stride));
}

void
gsl_blas_csscal (float alpha, gsl_vector_complex_float * X)
{
  cblas_csscal (INT (X->size), alpha, X->data, INT (X->stride));
}

void
gsl_blas_zdscal (double alpha, gsl_vector_complex * X)
{
  cblas_zdscal (INT (X->size), alpha, X->data, INT (X->stride));
}

/* ===========================================================================
 * Level 2
 * ===========================================================================
 */

/* GEMV */

int
gsl_blas_sgemv (CBLAS_TRANSPOSE_t TransA, float alpha,
                const gsl_matrix_float * A, const gsl_vector_float * X,
                float beta, gsl_vector_float * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if ((TransA == CblasNoTrans && N == X->size && M == Y->size)
      || (TransA == CblasTrans && M == X->size && N == Y->size))
    {
      cblas_sgemv (CblasRowMajor, TransA, INT (M), INT (N), alpha, A->data,
                   INT (A->tda), X->data, INT (X->stride), beta, Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_dgemv (CBLAS_TRANSPOSE_t TransA, double alpha, const gsl_matrix * A,
                const gsl_vector * X, double beta, gsl_vector * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if ((TransA == CblasNoTrans && N == X->size && M == Y->size)
      || (TransA == CblasTrans && M == X->size && N == Y->size))
    {
      cblas_dgemv (CblasRowMajor, TransA, INT (M), INT (N), alpha, A->data,
                   INT (A->tda), X->data, INT (X->stride), beta, Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_cgemv (CBLAS_TRANSPOSE_t TransA, const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_vector_complex_float * X,
                const gsl_complex_float beta, gsl_vector_complex_float * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if ((TransA == CblasNoTrans && N == X->size && M == Y->size)
      || (TransA == CblasTrans && M == X->size && N == Y->size)
      || (TransA == CblasConjTrans && M == X->size && N == Y->size))
    {
      cblas_cgemv (CblasRowMajor, TransA, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), X->data,
                   INT (X->stride), GSL_COMPLEX_P (&beta), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_zgemv (CBLAS_TRANSPOSE_t TransA, const gsl_complex alpha,
                const gsl_matrix_complex * A, const gsl_vector_complex * X,
                const gsl_complex beta, gsl_vector_complex * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if ((TransA == CblasNoTrans && N == X->size && M == Y->size)
      || (TransA == CblasTrans && M == X->size && N == Y->size)
      || (TransA == CblasConjTrans && M == X->size && N == Y->size))
    {
      cblas_zgemv (CblasRowMajor, TransA, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), X->data,
                   INT (X->stride), GSL_COMPLEX_P (&beta), Y->data,
                   INT (Y->stride));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}



/* HEMV */

int
gsl_blas_chemv (CBLAS_UPLO_t Uplo, const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_vector_complex_float * X,
                const gsl_complex_float beta, gsl_vector_complex_float * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size || N != Y->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_chemv (CblasRowMajor, Uplo, INT (N), GSL_COMPLEX_P (&alpha), A->data,
               INT (A->tda), X->data, INT (X->stride), GSL_COMPLEX_P (&beta),
               Y->data, INT (Y->stride));
  return GSL_SUCCESS;
}

int
gsl_blas_zhemv (CBLAS_UPLO_t Uplo, const gsl_complex alpha,
                const gsl_matrix_complex * A, const gsl_vector_complex * X,
                const gsl_complex beta, gsl_vector_complex * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size || N != Y->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zhemv (CblasRowMajor, Uplo, INT (N), GSL_COMPLEX_P (&alpha), A->data,
               INT (A->tda), X->data, INT (X->stride), GSL_COMPLEX_P (&beta),
               Y->data, INT (Y->stride));
  return GSL_SUCCESS;
}


/* SYMV */

int
gsl_blas_ssymv (CBLAS_UPLO_t Uplo, float alpha, const gsl_matrix_float * A,
                const gsl_vector_float * X, float beta, gsl_vector_float * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size || N != Y->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ssymv (CblasRowMajor, Uplo, INT (N), alpha, A->data, INT (A->tda),
               X->data, INT (X->stride), beta, Y->data, INT (Y->stride));
  return GSL_SUCCESS;
}

int
gsl_blas_dsymv (CBLAS_UPLO_t Uplo, double alpha, const gsl_matrix * A,
                const gsl_vector * X, double beta, gsl_vector * Y)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size || N != Y->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dsymv (CblasRowMajor, Uplo, INT (N), alpha, A->data, INT (A->tda),
               X->data, INT (X->stride), beta, Y->data, INT (Y->stride));
  return GSL_SUCCESS;
}


/* TRMV */

int
gsl_blas_strmv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_float * A,
                gsl_vector_float * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_strmv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_dtrmv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix * A, gsl_vector * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dtrmv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_ctrmv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_complex_float * A,
                gsl_vector_complex_float * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ctrmv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_ztrmv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_complex * A,
                gsl_vector_complex * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ztrmv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


/* TRSV */

int
gsl_blas_strsv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_float * A,
                gsl_vector_float * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_strsv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_dtrsv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix * A, gsl_vector * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dtrsv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_ctrsv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_complex_float * A,
                gsl_vector_complex_float * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ctrsv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


int
gsl_blas_ztrsv (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
                CBLAS_DIAG_t Diag, const gsl_matrix_complex * A,
                gsl_vector_complex * X)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (N != X->size)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ztrsv (CblasRowMajor, Uplo, TransA, Diag, INT (N), A->data,
               INT (A->tda), X->data, INT (X->stride));
  return GSL_SUCCESS;
}


/* GER */

int
gsl_blas_sger (float alpha, const gsl_vector_float * X,
               const gsl_vector_float * Y, gsl_matrix_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_sger (CblasRowMajor, INT (M), INT (N), alpha, X->data,
                  INT (X->stride), Y->data, INT (Y->stride), A->data,
                  INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_dger (double alpha, const gsl_vector * X, const gsl_vector * Y,
               gsl_matrix * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_dger (CblasRowMajor, INT (M), INT (N), alpha, X->data,
                  INT (X->stride), Y->data, INT (Y->stride), A->data,
                  INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* GERU */

int
gsl_blas_cgeru (const gsl_complex_float alpha,
                const gsl_vector_complex_float * X,
                const gsl_vector_complex_float * Y,
                gsl_matrix_complex_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_cgeru (CblasRowMajor, INT (M), INT (N), GSL_COMPLEX_P (&alpha),
                   X->data, INT (X->stride), Y->data, INT (Y->stride),
                   A->data, INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_zgeru (const gsl_complex alpha, const gsl_vector_complex * X,
                const gsl_vector_complex * Y, gsl_matrix_complex * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_zgeru (CblasRowMajor, INT (M), INT (N), GSL_COMPLEX_P (&alpha),
                   X->data, INT (X->stride), Y->data, INT (Y->stride),
                   A->data, INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* GERC */

int
gsl_blas_cgerc (const gsl_complex_float alpha,
                const gsl_vector_complex_float * X,
                const gsl_vector_complex_float * Y,
                gsl_matrix_complex_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_cgerc (CblasRowMajor, INT (M), INT (N), GSL_COMPLEX_P (&alpha),
                   X->data, INT (X->stride), Y->data, INT (Y->stride),
                   A->data, INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_zgerc (const gsl_complex alpha, const gsl_vector_complex * X,
                const gsl_vector_complex * Y, gsl_matrix_complex * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (X->size == M && Y->size == N)
    {
      cblas_zgerc (CblasRowMajor, INT (M), INT (N), GSL_COMPLEX_P (&alpha),
                   X->data, INT (X->stride), Y->data, INT (Y->stride),
                   A->data, INT (A->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

/* HER */

int
gsl_blas_cher (CBLAS_UPLO_t Uplo, float alpha,
               const gsl_vector_complex_float * X,
               gsl_matrix_complex_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_cher (CblasRowMajor, Uplo, INT (M), alpha, X->data, INT (X->stride),
              A->data, INT (A->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_zher (CBLAS_UPLO_t Uplo, double alpha, const gsl_vector_complex * X,
               gsl_matrix_complex * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zher (CblasRowMajor, Uplo, INT (N), alpha, X->data, INT (X->stride),
              A->data, INT (A->tda));
  return GSL_SUCCESS;
}


/* HER2 */

int
gsl_blas_cher2 (CBLAS_UPLO_t Uplo, const gsl_complex_float alpha,
                const gsl_vector_complex_float * X,
                const gsl_vector_complex_float * Y,
                gsl_matrix_complex_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N || Y->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_cher2 (CblasRowMajor, Uplo, INT (N), GSL_COMPLEX_P (&alpha), X->data,
               INT (X->stride), Y->data, INT (Y->stride), A->data,
               INT (A->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_zher2 (CBLAS_UPLO_t Uplo, const gsl_complex alpha,
                const gsl_vector_complex * X, const gsl_vector_complex * Y,
                gsl_matrix_complex * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N || Y->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zher2 (CblasRowMajor, Uplo, INT (N), GSL_COMPLEX_P (&alpha), X->data,
               INT (X->stride), Y->data, INT (Y->stride), A->data,
               INT (A->tda));
  return GSL_SUCCESS;
}


/* SYR */

int
gsl_blas_ssyr (CBLAS_UPLO_t Uplo, float alpha, const gsl_vector_float * X,
               gsl_matrix_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ssyr (CblasRowMajor, Uplo, INT (N), alpha, X->data, INT (X->stride),
              A->data, INT (A->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_dsyr (CBLAS_UPLO_t Uplo, double alpha, const gsl_vector * X,
               gsl_matrix * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dsyr (CblasRowMajor, Uplo, INT (N), alpha, X->data, INT (X->stride),
              A->data, INT (A->tda));
  return GSL_SUCCESS;
}


/* SYR2 */

int
gsl_blas_ssyr2 (CBLAS_UPLO_t Uplo, float alpha, const gsl_vector_float * X,
                const gsl_vector_float * Y, gsl_matrix_float * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N || Y->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ssyr2 (CblasRowMajor, Uplo, INT (N), alpha, X->data, INT (X->stride),
               Y->data, INT (Y->stride), A->data, INT (A->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_dsyr2 (CBLAS_UPLO_t Uplo, double alpha, const gsl_vector * X,
                const gsl_vector * Y, gsl_matrix * A)
{
  const size_t M = A->size1;
  const size_t N = A->size2;

  if (M != N)
    {
      GSL_ERROR ("matrix must be square", GSL_ENOTSQR);
    }
  else if (X->size != N || Y->size != N)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dsyr2 (CblasRowMajor, Uplo, INT (N), alpha, X->data, INT (X->stride),
               Y->data, INT (Y->stride), A->data, INT (A->tda));
  return GSL_SUCCESS;
}


/*
 * ===========================================================================
 * Prototypes for level 3 BLAS
 * ===========================================================================
 */


/* GEMM */

int
gsl_blas_sgemm (CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB,
                float alpha, const gsl_matrix_float * A,
                const gsl_matrix_float * B, float beta, gsl_matrix_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (TransA == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (TransA == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (TransB == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (TransB == CblasNoTrans) ? B->size2 : B->size1;

  if (M == MA && N == NB && NA == MB)   /* [MxN] = [MAxNA][MBxNB] */
    {
      cblas_sgemm (CblasRowMajor, TransA, TransB, INT (M), INT (N), INT (NA),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda), beta,
                   C->data, INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_dgemm (CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB,
                double alpha, const gsl_matrix * A, const gsl_matrix * B,
                double beta, gsl_matrix * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (TransA == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (TransA == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (TransB == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (TransB == CblasNoTrans) ? B->size2 : B->size1;

  if (M == MA && N == NB && NA == MB)   /* [MxN] = [MAxNA][MBxNB] */
    {
      cblas_dgemm (CblasRowMajor, TransA, TransB, INT (M), INT (N), INT (NA),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda), beta,
                   C->data, INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_cgemm (CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_matrix_complex_float * B,
                const gsl_complex_float beta, gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (TransA == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (TransA == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (TransB == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (TransB == CblasNoTrans) ? B->size2 : B->size1;

  if (M == MA && N == NB && NA == MB)   /* [MxN] = [MAxNA][MBxNB] */
    {
      cblas_cgemm (CblasRowMajor, TransA, TransB, INT (M), INT (N), INT (NA),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_zgemm (CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                const gsl_matrix_complex * B, const gsl_complex beta,
                gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (TransA == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (TransA == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (TransB == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (TransB == CblasNoTrans) ? B->size2 : B->size1;

  if (M == MA && N == NB && NA == MB)   /* [MxN] = [MAxNA][MBxNB] */
    {
      cblas_zgemm (CblasRowMajor, TransA, TransB, INT (M), INT (N), INT (NA),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* SYMM */

int
gsl_blas_ssymm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, float alpha,
                const gsl_matrix_float * A, const gsl_matrix_float * B,
                float beta, gsl_matrix_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_ssymm (CblasRowMajor, Side, Uplo, INT (M), INT (N), alpha,
                   A->data, INT (A->tda), B->data, INT (B->tda), beta,
                   C->data, INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

}


int
gsl_blas_dsymm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, double alpha,
                const gsl_matrix * A, const gsl_matrix * B, double beta,
                gsl_matrix * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_dsymm (CblasRowMajor, Side, Uplo, INT (M), INT (N), alpha,
                   A->data, INT (A->tda), B->data, INT (B->tda), beta,
                   C->data, INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_csymm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_matrix_complex_float * B,
                const gsl_complex_float beta, gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_csymm (CblasRowMajor, Side, Uplo, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

int
gsl_blas_zsymm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                const gsl_matrix_complex * B, const gsl_complex beta,
                gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_zsymm (CblasRowMajor, Side, Uplo, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* HEMM */

int
gsl_blas_chemm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_matrix_complex_float * B,
                const gsl_complex_float beta, gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_chemm (CblasRowMajor, Side, Uplo, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

}


int
gsl_blas_zhemm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                const gsl_matrix_complex * B, const gsl_complex beta,
                gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;
  const size_t MB = B->size1;
  const size_t NB = B->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && (M == MA && N == NB && NA == MB))
      || (Side == CblasRight && (M == MB && N == NA && NB == MA)))
    {
      cblas_zhemm (CblasRowMajor, Side, Uplo, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda), GSL_COMPLEX_P (&beta), C->data,
                   INT (C->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}

/* SYRK */

int
gsl_blas_ssyrk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
                const gsl_matrix_float * A, float beta, gsl_matrix_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ssyrk (CblasRowMajor, Uplo, Trans, INT (N), INT (K), alpha, A->data,
               INT (A->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_dsyrk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
                const gsl_matrix * A, double beta, gsl_matrix * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dsyrk (CblasRowMajor, Uplo, Trans, INT (N), INT (K), alpha, A->data,
               INT (A->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;

}


int
gsl_blas_csyrk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                const gsl_complex_float beta, gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_csyrk (CblasRowMajor, Uplo, Trans, INT (N), INT (K),
               GSL_COMPLEX_P (&alpha), A->data, INT (A->tda),
               GSL_COMPLEX_P (&beta), C->data, INT (C->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_zsyrk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                const gsl_complex beta, gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zsyrk (CblasRowMajor, Uplo, Trans, INT (N), INT (K),
               GSL_COMPLEX_P (&alpha), A->data, INT (A->tda),
               GSL_COMPLEX_P (&beta), C->data, INT (C->tda));
  return GSL_SUCCESS;
}

/* HERK */

int
gsl_blas_cherk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
                const gsl_matrix_complex_float * A, float beta,
                gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_cherk (CblasRowMajor, Uplo, Trans, INT (N), INT (K), alpha, A->data,
               INT (A->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_zherk (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
                const gsl_matrix_complex * A, double beta,
                gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t J = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t K = (Trans == CblasNoTrans) ? A->size2 : A->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != J)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zherk (CblasRowMajor, Uplo, Trans, INT (N), INT (K), alpha, A->data,
               INT (A->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;
}

/* SYR2K */

int
gsl_blas_ssyr2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
                 const gsl_matrix_float * A, const gsl_matrix_float * B,
                 float beta, gsl_matrix_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_ssyr2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA), alpha, A->data,
                INT (A->tda), B->data, INT (B->tda), beta, C->data,
                INT (C->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_dsyr2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
                 const gsl_matrix * A, const gsl_matrix * B, double beta,
                 gsl_matrix * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_dsyr2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA), alpha, A->data,
                INT (A->tda), B->data, INT (B->tda), beta, C->data,
                INT (C->tda));
  return GSL_SUCCESS;
}


int
gsl_blas_csyr2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                 const gsl_complex_float alpha,
                 const gsl_matrix_complex_float * A,
                 const gsl_matrix_complex_float * B,
                 const gsl_complex_float beta, gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_csyr2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA),
                GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                INT (B->tda), GSL_COMPLEX_P (&beta), C->data, INT (C->tda));
  return GSL_SUCCESS;
}



int
gsl_blas_zsyr2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                 const gsl_complex alpha, const gsl_matrix_complex * A,
                 const gsl_matrix_complex * B, const gsl_complex beta,
                 gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zsyr2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA),
                GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                INT (B->tda), GSL_COMPLEX_P (&beta), C->data, INT (C->tda));
  return GSL_SUCCESS;
}

/* HER2K */

int
gsl_blas_cher2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                 const gsl_complex_float alpha,
                 const gsl_matrix_complex_float * A,
                 const gsl_matrix_complex_float * B, float beta,
                 gsl_matrix_complex_float * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_cher2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA),
                GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                INT (B->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;

}


int
gsl_blas_zher2k (CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans,
                 const gsl_complex alpha, const gsl_matrix_complex * A,
                 const gsl_matrix_complex * B, double beta,
                 gsl_matrix_complex * C)
{
  const size_t M = C->size1;
  const size_t N = C->size2;
  const size_t MA = (Trans == CblasNoTrans) ? A->size1 : A->size2;
  const size_t NA = (Trans == CblasNoTrans) ? A->size2 : A->size1;
  const size_t MB = (Trans == CblasNoTrans) ? B->size1 : B->size2;
  const size_t NB = (Trans == CblasNoTrans) ? B->size2 : B->size1;

  if (M != N)
    {
      GSL_ERROR ("matrix C must be square", GSL_ENOTSQR);
    }
  else if (N != MA || N != MB || NA != NB)
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }

  cblas_zher2k (CblasRowMajor, Uplo, Trans, INT (N), INT (NA),
                GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                INT (B->tda), beta, C->data, INT (C->tda));
  return GSL_SUCCESS;

}

/* TRMM */

int
gsl_blas_strmm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag, float alpha,
                const gsl_matrix_float * A, gsl_matrix_float * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_strmm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_dtrmm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag, double alpha,
                const gsl_matrix * A, gsl_matrix * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_dtrmm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_ctrmm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                gsl_matrix_complex_float * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_ctrmm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_ztrmm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                gsl_matrix_complex * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_ztrmm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


/* TRSM */

int
gsl_blas_strsm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag, float alpha,
                const gsl_matrix_float * A, gsl_matrix_float * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_strsm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_dtrsm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag, double alpha,
                const gsl_matrix * A, gsl_matrix * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_dtrsm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   alpha, A->data, INT (A->tda), B->data, INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_ctrsm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
                const gsl_complex_float alpha,
                const gsl_matrix_complex_float * A,
                gsl_matrix_complex_float * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_ctrsm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}


int
gsl_blas_ztrsm (CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo,
                CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
                const gsl_complex alpha, const gsl_matrix_complex * A,
                gsl_matrix_complex * B)
{
  const size_t M = B->size1;
  const size_t N = B->size2;
  const size_t MA = A->size1;
  const size_t NA = A->size2;

  if (MA != NA)
    {
      GSL_ERROR ("matrix A must be square", GSL_ENOTSQR);
    }

  if ((Side == CblasLeft && M == MA) || (Side == CblasRight && N == MA))
    {
      cblas_ztrsm (CblasRowMajor, Side, Uplo, TransA, Diag, INT (M), INT (N),
                   GSL_COMPLEX_P (&alpha), A->data, INT (A->tda), B->data,
                   INT (B->tda));
      return GSL_SUCCESS;
    }
  else
    {
      GSL_ERROR ("invalid length", GSL_EBADLEN);
    }
}
