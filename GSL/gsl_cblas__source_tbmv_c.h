/* blas/source_tbmv_c.h
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
  const int conj = (TransA == CblasConjTrans) ? -1 : 1;
  const int Trans = (TransA != CblasConjTrans) ? TransA : CblasTrans;
  const int nonunit = (Diag == CblasNonUnit);
  INDEX i, j;


  if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasUpper)
      || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasLower)) {
    /* form  x := A*x */

    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      BASE temp_r = 0.0;
      BASE temp_i = 0.0;
      const INDEX j_min = i + 1;
      const INDEX j_max = GSL_MIN(N, i + K + 1);
      INDEX jx = OFFSET(N, incX) + incX * j_min;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = REAL(X, jx);
        const BASE x_imag = IMAG(X, jx);
        const BASE A_real = CONST_REAL(A, lda * i + (j - i));
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + (j - i));

        temp_r += A_real * x_real - A_imag * x_imag;
        temp_i += A_real * x_imag + A_imag * x_real;

        jx += incX;
      }
      if (nonunit) {
        const BASE x_real = REAL(X, ix);
        const BASE x_imag = IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + 0);
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + 0);

        REAL(X, ix) = temp_r + (A_real * x_real - A_imag * x_imag);
        IMAG(X, ix) = temp_i + (A_real * x_imag + A_imag * x_real);
      } else {
        REAL(X, ix) += temp_r;
        IMAG(X, ix) += temp_i;
      }
      ix += incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasUpper)) {
    INDEX ix = OFFSET(N, incX) + (N - 1) * incX;

    for (i = N; i > 0 && i--;) {        /*  N-1 ... 0 */
      BASE temp_r = 0.0;
      BASE temp_i = 0.0;
      const INDEX j_min = (K > i ? 0 : i - K);
      const INDEX j_max = i;
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = REAL(X, jx);
        const BASE x_imag = IMAG(X, jx);
        const BASE A_real = CONST_REAL(A, lda * i + (K - i + j));
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + (K - i + j));

        temp_r += A_real * x_real - A_imag * x_imag;
        temp_i += A_real * x_imag + A_imag * x_real;

        jx += incX;
      }
      if (nonunit) {
        const BASE x_real = REAL(X, ix);
        const BASE x_imag = IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + K);
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + K);

        REAL(X, ix) = temp_r + (A_real * x_real - A_imag * x_imag);
        IMAG(X, ix) = temp_i + (A_real * x_imag + A_imag * x_real);
      } else {
        REAL(X, ix) += temp_r;
        IMAG(X, ix) += temp_i;
      }
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasUpper)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasLower)) {
    /* form  x := A'*x */

    INDEX ix = OFFSET(N, incX) + (N - 1) * incX;
    for (i = N; i > 0 && i--;) {        /*  N-1 ... 0 */
      BASE temp_r = 0.0;
      BASE temp_i = 0.0;
      const INDEX j_min = (K > i ? 0 : i - K);
      const INDEX j_max = i;
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = REAL(X, jx);
        const BASE x_imag = IMAG(X, jx);
        const BASE A_real = CONST_REAL(A, lda * j + (i - j));
        const BASE A_imag = conj * CONST_IMAG(A, lda * j + (i - j));

        temp_r += A_real * x_real - A_imag * x_imag;
        temp_i += A_real * x_imag + A_imag * x_real;

        jx += incX;
      }
      if (nonunit) {
        const BASE x_real = REAL(X, ix);
        const BASE x_imag = IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + 0);
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + 0);

        REAL(X, ix) = temp_r + (A_real * x_real - A_imag * x_imag);
        IMAG(X, ix) = temp_i + (A_real * x_imag + A_imag * x_real);
      } else {
        REAL(X, ix) += temp_r;
        IMAG(X, ix) += temp_i;
      }
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasUpper)) {
    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      BASE temp_r = 0.0;
      BASE temp_i = 0.0;
      const INDEX j_min = i + 1;
      const INDEX j_max = GSL_MIN(N, i + K + 1);
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = REAL(X, jx);
        const BASE x_imag = IMAG(X, jx);
        const BASE A_real = CONST_REAL(A, lda * j + (K - j + i));
        const BASE A_imag = conj * CONST_IMAG(A, lda * j + (K - j + i));

        temp_r += A_real * x_real - A_imag * x_imag;
        temp_i += A_real * x_imag + A_imag * x_real;

        jx += incX;
      }
      if (nonunit) {
        const BASE x_real = REAL(X, ix);
        const BASE x_imag = IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + K);
        const BASE A_imag = conj * CONST_IMAG(A, lda * i + K);

        REAL(X, ix) = temp_r + (A_real * x_real - A_imag * x_imag);
        IMAG(X, ix) = temp_i + (A_real * x_imag + A_imag * x_real);
      } else {
        REAL(X, ix) += temp_r;
        IMAG(X, ix) += temp_i;
      }
      ix += incX;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
