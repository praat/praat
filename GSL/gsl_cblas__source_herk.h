/* blas/source_herk.h
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
  INDEX i, j, k;
  int uplo, trans;

  if (beta == 1.0 && (alpha == 0.0 || K == 0))
    return;

  if (Order == CblasRowMajor) {
    uplo = Uplo;
    trans = Trans;
  } else {
    uplo = (Uplo == CblasUpper) ? CblasLower : CblasUpper;
    trans = (Trans == CblasNoTrans) ? CblasConjTrans : CblasNoTrans;
  }

  /* form  y := beta*y */
  if (beta == 0.0) {
    if (uplo == CblasUpper) {
      for (i = 0; i < N; i++) {
        for (j = i; j < N; j++) {
          REAL(C, ldc * i + j) = 0.0;
          IMAG(C, ldc * i + j) = 0.0;
        }
      }
    } else {
      for (i = 0; i < N; i++) {
        for (j = 0; j <= i; j++) {
          REAL(C, ldc * i + j) = 0.0;
          IMAG(C, ldc * i + j) = 0.0;
        }
      }
    }
  } else if (beta != 1.0) {
    if (uplo == CblasUpper) {
      for (i = 0; i < N; i++) {
        REAL(C, ldc * i + i) *= beta;
        IMAG(C, ldc * i + i) = 0;
        for (j = i + 1; j < N; j++) {
          REAL(C, ldc * i + j) *= beta;
          IMAG(C, ldc * i + j) *= beta;
        }
      }
    } else {
      for (i = 0; i < N; i++) {
        for (j = 0; j < i; j++) {
          REAL(C, ldc * i + j) *= beta;
          IMAG(C, ldc * i + j) *= beta;
        }
        REAL(C, ldc * i + i) *= beta;
        IMAG(C, ldc * i + i) = 0;
      }
    }
  } else {
    /* set imaginary part of Aii to zero */
    for (i = 0; i < N; i++) {
      IMAG(C, ldc * i + i) = 0.0;
    }
  }

  if (alpha == 0.0)
    return;

  if (uplo == CblasUpper && trans == CblasNoTrans) {

    for (i = 0; i < N; i++) {
      for (j = i; j < N; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = -CONST_IMAG(A, j * lda + k);
          temp_real += Aik_real * Ajk_real - Aik_imag * Ajk_imag;
          temp_imag += Aik_real * Ajk_imag + Aik_imag * Ajk_real;
        }
        REAL(C, i * ldc + j) += alpha * temp_real;
        IMAG(C, i * ldc + j) += alpha * temp_imag;
      }
    }

  } else if (uplo == CblasUpper && trans == CblasConjTrans) {

    for (i = 0; i < N; i++) {
      for (j = i; j < N; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aki_real = CONST_REAL(A, k * lda + i);
          const BASE Aki_imag = -CONST_IMAG(A, k * lda + i);
          const BASE Akj_real = CONST_REAL(A, k * lda + j);
          const BASE Akj_imag = CONST_IMAG(A, k * lda + j);
          temp_real += Aki_real * Akj_real - Aki_imag * Akj_imag;
          temp_imag += Aki_real * Akj_imag + Aki_imag * Akj_real;
        }
        REAL(C, i * ldc + j) += alpha * temp_real;
        IMAG(C, i * ldc + j) += alpha * temp_imag;
      }
    }

  } else if (uplo == CblasLower && trans == CblasNoTrans) {

    for (i = 0; i < N; i++) {
      for (j = 0; j <= i; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = -CONST_IMAG(A, j * lda + k);
          temp_real += Aik_real * Ajk_real - Aik_imag * Ajk_imag;
          temp_imag += Aik_real * Ajk_imag + Aik_imag * Ajk_real;
        }
        REAL(C, i * ldc + j) += alpha * temp_real;
        IMAG(C, i * ldc + j) += alpha * temp_imag;
      }
    }

  } else if (uplo == CblasLower && trans == CblasConjTrans) {

    for (i = 0; i < N; i++) {
      for (j = 0; j <= i; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aki_real = CONST_REAL(A, k * lda + i);
          const BASE Aki_imag = -CONST_IMAG(A, k * lda + i);
          const BASE Akj_real = CONST_REAL(A, k * lda + j);
          const BASE Akj_imag = CONST_IMAG(A, k * lda + j);
          temp_real += Aki_real * Akj_real - Aki_imag * Akj_imag;
          temp_imag += Aki_real * Akj_imag + Aki_imag * Akj_real;
        }
        REAL(C, i * ldc + j) += alpha * temp_real;
        IMAG(C, i * ldc + j) += alpha * temp_imag;
      }
    }

  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
