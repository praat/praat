/* blas/source_hemm.h
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
  INDEX n1, n2;
  int uplo, side;

  const BASE alpha_real = CONST_REAL0(alpha);
  const BASE alpha_imag = CONST_IMAG0(alpha);

  const BASE beta_real = CONST_REAL0(beta);
  const BASE beta_imag = CONST_IMAG0(beta);

  if ((alpha_real == 0.0 && alpha_imag == 0.0)
      && (beta_real == 1.0 && beta_imag == 0.0))
    return;

  if (Order == CblasRowMajor) {
    n1 = M;
    n2 = N;
    uplo = Uplo;
    side = Side;
  } else {
    n1 = N;
    n2 = M;
    uplo = (Uplo == CblasUpper) ? CblasLower : CblasUpper;
    side = (Side == CblasLeft) ? CblasRight : CblasLeft;
  }

  /* form  y := beta*y */
  if (beta_real == 0.0 && beta_imag == 0.0) {
    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        REAL(C, ldc * i + j) = 0.0;
        IMAG(C, ldc * i + j) = 0.0;
      }
    }
  } else if (!(beta_real == 1.0 && beta_imag == 0.0)) {
    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE Cij_real = REAL(C, ldc * i + j);
        const BASE Cij_imag = IMAG(C, ldc * i + j);
        REAL(C, ldc * i + j) = beta_real * Cij_real - beta_imag * Cij_imag;
        IMAG(C, ldc * i + j) = beta_real * Cij_imag + beta_imag * Cij_real;
      }
    }
  }

  if (alpha_real == 0.0 && alpha_imag == 0.0)
    return;

  if (side == CblasLeft && uplo == CblasUpper) {

    /* form  C := alpha*A*B + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE Bij_real = CONST_REAL(B, ldb * i + j);
        const BASE Bij_imag = CONST_IMAG(B, ldb * i + j);
        const BASE temp1_real = alpha_real * Bij_real - alpha_imag * Bij_imag;
        const BASE temp1_imag = alpha_real * Bij_imag + alpha_imag * Bij_real;
        BASE temp2_real = 0.0;
        BASE temp2_imag = 0.0;
        {
          const BASE Aii_real = CONST_REAL(A, i * lda + i);
          /* const BASE Aii_imag = 0.0; */
          REAL(C, i * ldc + j) += temp1_real * Aii_real;
          IMAG(C, i * ldc + j) += temp1_imag * Aii_real;
        }
        for (k = i + 1; k < n1; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          const BASE Bkj_real = CONST_REAL(B, ldb * k + j);
          const BASE Bkj_imag = CONST_IMAG(B, ldb * k + j);
          REAL(C, k * ldc + j) += Aik_real * temp1_real - (-Aik_imag) * temp1_imag;
          IMAG(C, k * ldc + j) += Aik_real * temp1_imag + (-Aik_imag) * temp1_real;
          temp2_real += Aik_real * Bkj_real - Aik_imag * Bkj_imag;
          temp2_imag += Aik_real * Bkj_imag + Aik_imag * Bkj_real;
        }
        REAL(C, i * ldc + j) += alpha_real * temp2_real - alpha_imag * temp2_imag;
        IMAG(C, i * ldc + j) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      }
    }

  } else if (side == CblasLeft && uplo == CblasLower) {

    /* form  C := alpha*A*B + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE Bij_real = CONST_REAL(B, ldb * i + j);
        const BASE Bij_imag = CONST_IMAG(B, ldb * i + j);
        const BASE temp1_real = alpha_real * Bij_real - alpha_imag * Bij_imag;
        const BASE temp1_imag = alpha_real * Bij_imag + alpha_imag * Bij_real;
        BASE temp2_real = 0.0;
        BASE temp2_imag = 0.0;
        for (k = 0; k < i; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          const BASE Bkj_real = CONST_REAL(B, ldb * k + j);
          const BASE Bkj_imag = CONST_IMAG(B, ldb * k + j);
          REAL(C, k * ldc + j) += Aik_real * temp1_real - (-Aik_imag) * temp1_imag;
          IMAG(C, k * ldc + j) += Aik_real * temp1_imag + (-Aik_imag) * temp1_real;
          temp2_real += Aik_real * Bkj_real - Aik_imag * Bkj_imag;
          temp2_imag += Aik_real * Bkj_imag + Aik_imag * Bkj_real;
        }
        {
          const BASE Aii_real = CONST_REAL(A, i * lda + i);
          /* const BASE Aii_imag = 0.0; */
          REAL(C, i * ldc + j) += temp1_real * Aii_real;
          IMAG(C, i * ldc + j) += temp1_imag * Aii_real;
        }
        REAL(C, i * ldc + j) += alpha_real * temp2_real - alpha_imag * temp2_imag;
        IMAG(C, i * ldc + j) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper) {

    /* form  C := alpha*B*A + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE Bij_real = CONST_REAL(B, ldb * i + j);
        const BASE Bij_imag = CONST_IMAG(B, ldb * i + j);
        const BASE temp1_real = alpha_real * Bij_real - alpha_imag * Bij_imag;
        const BASE temp1_imag = alpha_real * Bij_imag + alpha_imag * Bij_real;
        BASE temp2_real = 0.0;
        BASE temp2_imag = 0.0;
        {
          const BASE Ajj_real = CONST_REAL(A, j * lda + j);
          /* const BASE Ajj_imag = 0.0; */
          REAL(C, i * ldc + j) += temp1_real * Ajj_real;
          IMAG(C, i * ldc + j) += temp1_imag * Ajj_real;
        }
        for (k = j + 1; k < n2; k++) {
          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = CONST_IMAG(A, j * lda + k);
          const BASE Bik_real = CONST_REAL(B, ldb * i + k);
          const BASE Bik_imag = CONST_IMAG(B, ldb * i + k);
          REAL(C, i * ldc + k) += temp1_real * Ajk_real - temp1_imag * Ajk_imag;
          IMAG(C, i * ldc + k) += temp1_real * Ajk_imag + temp1_imag * Ajk_real;
          temp2_real += Bik_real * Ajk_real - Bik_imag * (-Ajk_imag);
          temp2_imag += Bik_real * (-Ajk_imag) + Bik_imag * Ajk_real;
        }
        REAL(C, i * ldc + j) += alpha_real * temp2_real - alpha_imag * temp2_imag;
        IMAG(C, i * ldc + j) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      }
    }

  } else if (side == CblasRight && uplo == CblasLower) {

    /* form  C := alpha*B*A + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE Bij_real = CONST_REAL(B, ldb * i + j);
        const BASE Bij_imag = CONST_IMAG(B, ldb * i + j);
        const BASE temp1_real = alpha_real * Bij_real - alpha_imag * Bij_imag;
        const BASE temp1_imag = alpha_real * Bij_imag + alpha_imag * Bij_real;
        BASE temp2_real = 0.0;
        BASE temp2_imag = 0.0;
        for (k = 0; k < j; k++) {
          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = CONST_IMAG(A, j * lda + k);
          const BASE Bik_real = CONST_REAL(B, ldb * i + k);
          const BASE Bik_imag = CONST_IMAG(B, ldb * i + k);
          REAL(C, i * ldc + k) += temp1_real * Ajk_real - temp1_imag * Ajk_imag;
          IMAG(C, i * ldc + k) += temp1_real * Ajk_imag + temp1_imag * Ajk_real;
          temp2_real += Bik_real * Ajk_real - Bik_imag * (-Ajk_imag);
          temp2_imag += Bik_real * (-Ajk_imag) + Bik_imag * Ajk_real;
        }
        {
          const BASE Ajj_real = CONST_REAL(A, j * lda + j);
          /* const BASE Ajj_imag = 0.0; */
          REAL(C, i * ldc + j) += temp1_real * Ajj_real;
          IMAG(C, i * ldc + j) += temp1_imag * Ajj_real;
        }
        REAL(C, i * ldc + j) += alpha_real * temp2_real - alpha_imag * temp2_imag;
        IMAG(C, i * ldc + j) += alpha_real * temp2_imag + alpha_imag * temp2_real;
      }
    }

  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
