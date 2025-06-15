/* blas/source_tpmv_r.h
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
  INDEX i, j;
  const int nonunit = (Diag == CblasNonUnit);
  const int Trans = (TransA != CblasConjTrans) ? TransA : CblasTrans;

  if (N == 0)
    return;

  if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasUpper)
      || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasLower)) {
    /* form  x:= A*x */

    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      BASE atmp = Ap[TPUP(N, i, i)];
      BASE temp = (nonunit ? X[ix] * atmp : X[ix]);
      INDEX jx = OFFSET(N, incX) + (i + 1) * incX;
      for (j = i + 1; j < N; j++) {
        atmp = Ap[TPUP(N, i, j)];
        temp += atmp * X[jx];
        jx += incX;
      }
      X[ix] = temp;
      ix += incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasUpper)) {

    INDEX ix = OFFSET(N, incX) + (N - 1) * incX;
    for (i = N; i > 0 && i--;) {
      BASE atmp = Ap[TPLO(N, i, i)];
      BASE temp = (nonunit ? X[ix] * atmp : X[ix]);
      INDEX jx = OFFSET(N, incX);
      for (j = 0; j < i; j++) {
        atmp = Ap[TPLO(N, i, j)];
        temp += atmp * X[jx];
        jx += incX;
      }
      X[ix] = temp;
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasUpper)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasLower)) {
    /* form  x := A'*x */

    INDEX ix = OFFSET(N, incX) + (N - 1) * incX;
    for (i = N; i > 0 && i--;) {
      BASE atmp = Ap[TPUP(N, i, i)];
      BASE temp = (nonunit ? X[ix] * atmp : X[ix]);
      INDEX jx = OFFSET(N, incX);
      for (j = 0; j < i; j++) {
        atmp = Ap[TPUP(N, j, i)];
        temp += atmp * X[jx];
        jx += incX;
      }
      X[ix] = temp;
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasUpper)) {

    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      BASE atmp = Ap[TPLO(N, i, i)];
      BASE temp = (nonunit ? X[ix] * atmp : X[ix]);
      INDEX jx = OFFSET(N, incX) + (i + 1) * incX;
      for (j = i + 1; j < N; j++) {
        atmp = Ap[TPLO(N, j, i)];
        temp += atmp * X[jx];
        jx += incX;
      }
      X[ix] = temp;
      ix += incX;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
