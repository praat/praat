/* blas/source_symm_r.h
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

  if (alpha == 0.0 && beta == 1.0)
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
  if (beta == 0.0) {
    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        C[ldc * i + j] = 0.0;
      }
    }
  } else if (beta != 1.0) {
    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        C[ldc * i + j] *= beta;
      }
    }
  }

  if (alpha == 0.0)
    return;

  if (side == CblasLeft && uplo == CblasUpper) {

    /* form  C := alpha*A*B + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE temp1 = alpha * B[ldb * i + j];
        BASE temp2 = 0.0;
        C[i * ldc + j] += temp1 * A[i * lda + i];
        for (k = i + 1; k < n1; k++) {
          const BASE Aik = A[i * lda + k];
          C[k * ldc + j] += Aik * temp1;
          temp2 += Aik * B[ldb * k + j];
        }
        C[i * ldc + j] += alpha * temp2;
      }
    }

  } else if (side == CblasLeft && uplo == CblasLower) {

    /* form  C := alpha*A*B + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE temp1 = alpha * B[ldb * i + j];
        BASE temp2 = 0.0;
        for (k = 0; k < i; k++) {
          const BASE Aik = A[i * lda + k];
          C[k * ldc + j] += Aik * temp1;
          temp2 += Aik * B[ldb * k + j];
        }
        C[i * ldc + j] += temp1 * A[i * lda + i] + alpha * temp2;
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper) {

    /* form  C := alpha*B*A + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE temp1 = alpha * B[ldb * i + j];
        BASE temp2 = 0.0;
        C[i * ldc + j] += temp1 * A[j * lda + j];
        for (k = j + 1; k < n2; k++) {
          const BASE Ajk = A[j * lda + k];
          C[i * ldc + k] += temp1 * Ajk;
          temp2 += B[ldb * i + k] * Ajk;
        }
        C[i * ldc + j] += alpha * temp2;
      }
    }

  } else if (side == CblasRight && uplo == CblasLower) {

    /* form  C := alpha*B*A + C */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        const BASE temp1 = alpha * B[ldb * i + j];
        BASE temp2 = 0.0;
        for (k = 0; k < j; k++) {
          const BASE Ajk = A[j * lda + k];
          C[i * ldc + k] += temp1 * Ajk;
          temp2 += B[ldb * i + k] * Ajk;
        }
        C[i * ldc + j] += temp1 * A[j * lda + j] + alpha * temp2;
      }
    }

  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
