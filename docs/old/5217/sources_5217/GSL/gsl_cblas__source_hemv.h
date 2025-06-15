/* blas/source_hemv.h
 * 
 * Copyright (C) 2001, 2007 Brian Gough
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
  const int conj = (order == CblasColMajor) ? -1 : 1;
  INDEX i, j;

  const BASE alpha_real = CONST_REAL0(alpha);
  const BASE alpha_imag = CONST_IMAG0(alpha);

  const BASE beta_real = CONST_REAL0(beta);
  const BASE beta_imag = CONST_IMAG0(beta);

  if ((alpha_real == 0.0 && alpha_imag == 0.0)
      && (beta_real == 1.0 && beta_imag == 0.0))
    return;

  /* form  y := beta*y */
  if (beta_real == 0.0 && beta_imag == 0.0) {
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      REAL(Y, iy) = 0.0;
      IMAG(Y, iy) = 0.0;
      iy += incY;
    }
  } else if (!(beta_real == 1.0 && beta_imag == 0.0)) {
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      const BASE y_real = REAL(Y, iy);
      const BASE y_imag = IMAG(Y, iy);
      const BASE tmpR = y_real * beta_real - y_imag * beta_imag;
      const BASE tmpI = y_real * beta_imag + y_imag * beta_real;
      REAL(Y, iy) = tmpR;
      IMAG(Y, iy) = tmpI;
      iy += incY;
    }
  }

  if (alpha_real == 0.0 && alpha_imag == 0.0)
    return;

  /* form  y := alpha*A*x + y */

  if ((order == CblasRowMajor && Uplo == CblasUpper)
      || (order == CblasColMajor && Uplo == CblasLower)) {
    INDEX ix = OFFSET(N, incX);
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      BASE x_real = CONST_REAL(X, ix);
      BASE x_imag = CONST_IMAG(X, ix);
      BASE temp1_real = alpha_real * x_real - alpha_imag * x_imag;
      BASE temp1_imag = alpha_real * x_imag + alpha_imag * x_real;
      BASE temp2_real = 0.0;
      BASE temp2_imag = 0.0;
      const INDEX j_min = i + 1;
      const INDEX j_max = N;
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      INDEX jy = OFFSET(N, incY) + j_min * incY;
      BASE Aii_real = CONST_REAL(A, lda * i + i);
      /* Aii_imag is zero */
      REAL(Y, iy) += temp1_real * Aii_real;
      IMAG(Y, iy) += temp1_imag * Aii_real;
      for (j = j_min; j < j_max; j++) {
        BASE Aij_real = CONST_REAL(A, lda * i + j);
        BASE Aij_imag = conj * CONST_IMAG(A, lda * i + j);
        REAL(Y, jy) += temp1_real * Aij_real - temp1_imag * (-Aij_imag);
        IMAG(Y, jy) += temp1_real * (-Aij_imag) + temp1_imag * Aij_real;
        x_real = CONST_REAL(X, jx);
        x_imag = CONST_IMAG(X, jx);
        temp2_real += x_real * Aij_real - x_imag * Aij_imag;
        temp2_imag += x_real * Aij_imag + x_imag * Aij_real;
        jx += incX;
        jy += incY;
      }
      REAL(Y, iy) += alpha_real * temp2_real - alpha_imag * temp2_imag;
      IMAG(Y, iy) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      ix += incX;
      iy += incY;
    }
  } else if ((order == CblasRowMajor && Uplo == CblasLower)
             || (order == CblasColMajor && Uplo == CblasUpper)) {
    INDEX ix = OFFSET(N, incX) + (N - 1) * incX;
    INDEX iy = OFFSET(N, incY) + (N - 1) * incY;
    for (i = N; i > 0 && i--;) {
      BASE x_real = CONST_REAL(X, ix);
      BASE x_imag = CONST_IMAG(X, ix);
      BASE temp1_real = alpha_real * x_real - alpha_imag * x_imag;
      BASE temp1_imag = alpha_real * x_imag + alpha_imag * x_real;
      BASE temp2_real = 0.0;
      BASE temp2_imag = 0.0;
      const INDEX j_min = 0;
      const INDEX j_max = i;
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      INDEX jy = OFFSET(N, incY) + j_min * incY;
      BASE Aii_real = CONST_REAL(A, lda * i + i);
      /* Aii_imag is zero */
      REAL(Y, iy) += temp1_real * Aii_real;
      IMAG(Y, iy) += temp1_imag * Aii_real;

      for (j = j_min; j < j_max; j++) {
        BASE Aij_real = CONST_REAL(A, lda * i + j);
        BASE Aij_imag = conj * CONST_IMAG(A, lda * i + j);
        REAL(Y, jy) += temp1_real * Aij_real - temp1_imag * (-Aij_imag);
        IMAG(Y, jy) += temp1_real * (-Aij_imag) + temp1_imag * Aij_real;
        x_real = CONST_REAL(X, jx);
        x_imag = CONST_IMAG(X, jx);
        temp2_real += x_real * Aij_real - x_imag * Aij_imag;
        temp2_imag += x_real * Aij_imag + x_imag * Aij_real;
        jx += incX;
        jy += incY;
      }
      REAL(Y, iy) += alpha_real * temp2_real - alpha_imag * temp2_imag;
      IMAG(Y, iy) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      ix -= incX;
      iy -= incY;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
