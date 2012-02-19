/* blas/source_trsv_r.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

{
  const int nonunit = (Diag == CblasNonUnit);
  INDEX ix, jx;
  INDEX i, j;
  const int Trans = (TransA != CblasConjTrans) ? TransA : CblasTrans;

  if (N == 0)
    return;

  /* form  x := inv( A )*x */

  if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasUpper)
      || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasLower)) {
    /* backsubstitution */
    ix = OFFSET(N, incX) + incX * (N - 1);
    if (nonunit) {
      X[ix] = X[ix] / A[lda * (N - 1) + (N - 1)];
    }
    ix -= incX;
    for (i = N - 1; i > 0 && i--;) {
      BASE tmp = X[ix];
      jx = ix + incX;
      for (j = i + 1; j < N; j++) {
        const BASE Aij = A[lda * i + j];
        tmp -= Aij * X[jx];
        jx += incX;
      }
      if (nonunit) {
        X[ix] = tmp / A[lda * i + i];
      } else {
        X[ix] = tmp;
      }
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasUpper)) {

    /* forward substitution */
    ix = OFFSET(N, incX);
    if (nonunit) {
      X[ix] = X[ix] / A[lda * 0 + 0];
    }
    ix += incX;
    for (i = 1; i < N; i++) {
      BASE tmp = X[ix];
      jx = OFFSET(N, incX);
      for (j = 0; j < i; j++) {
        const BASE Aij = A[lda * i + j];
        tmp -= Aij * X[jx];
        jx += incX;
      }
      if (nonunit) {
        X[ix] = tmp / A[lda * i + i];
      } else {
        X[ix] = tmp;
      }
      ix += incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasUpper)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasLower)) {

    /* form  x := inv( A' )*x */

    /* forward substitution */
    ix = OFFSET(N, incX);
    if (nonunit) {
      X[ix] = X[ix] / A[lda * 0 + 0];
    }
    ix += incX;
    for (i = 1; i < N; i++) {
      BASE tmp = X[ix];
      jx = OFFSET(N, incX);
      for (j = 0; j < i; j++) {
        const BASE Aji = A[lda * j + i];
        tmp -= Aji * X[jx];
        jx += incX;
      }
      if (nonunit) {
        X[ix] = tmp / A[lda * i + i];
      } else {
        X[ix] = tmp;
      }
      ix += incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasUpper)) {

    /* backsubstitution */
    ix = OFFSET(N, incX) + (N - 1) * incX;
    if (nonunit) {
      X[ix] = X[ix] / A[lda * (N - 1) + (N - 1)];
    }
    ix -= incX;
    for (i = N - 1; i > 0 && i--;) {
      BASE tmp = X[ix];
      jx = ix + incX;
      for (j = i + 1; j < N; j++) {
        const BASE Aji = A[lda * j + i];
        tmp -= Aji * X[jx];
        jx += incX;
      }
      if (nonunit) {
        X[ix] = tmp / A[lda * i + i];
      } else {
        X[ix] = tmp;
      }
      ix -= incX;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }

}
