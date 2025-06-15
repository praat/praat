/* blas/source_trmm_r.h
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
  const int nonunit = (Diag == CblasNonUnit);
  int side, uplo, trans;

  if (Order == CblasRowMajor) {
    n1 = M;
    n2 = N;
    side = Side;
    uplo = Uplo;
    trans = (TransA == CblasConjTrans) ? CblasTrans : TransA;
  } else {
    n1 = N;
    n2 = M;
    side = (Side == CblasLeft) ? CblasRight : CblasLeft;
    uplo = (Uplo == CblasUpper) ? CblasLower : CblasUpper;
    trans = (TransA == CblasConjTrans) ? CblasTrans : TransA;
  }

  if (side == CblasLeft && uplo == CblasUpper && trans == CblasNoTrans) {

    /* form  B := alpha * TriU(A)*B */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        if (nonunit) {
          temp = A[i * lda + i] * B[i * ldb + j];
        } else {
          temp = B[i * ldb + j];
        }

        for (k = i + 1; k < n1; k++) {
          temp += A[lda * i + k] * B[k * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasLeft && uplo == CblasUpper && trans == CblasTrans) {

    /* form  B := alpha * (TriU(A))' *B */

    for (i = n1; i > 0 && i--;) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        for (k = 0; k < i; k++) {
          temp += A[lda * k + i] * B[k * ldb + j];
        }

        if (nonunit) {
          temp += A[i * lda + i] * B[i * ldb + j];
        } else {
          temp += B[i * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasLeft && uplo == CblasLower && trans == CblasNoTrans) {

    /* form  B := alpha * TriL(A)*B */


    for (i = n1; i > 0 && i--;) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        for (k = 0; k < i; k++) {
          temp += A[lda * i + k] * B[k * ldb + j];
        }

        if (nonunit) {
          temp += A[i * lda + i] * B[i * ldb + j];
        } else {
          temp += B[i * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }



  } else if (side == CblasLeft && uplo == CblasLower && trans == CblasTrans) {

    /* form  B := alpha * TriL(A)' *B */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        if (nonunit) {
          temp = A[i * lda + i] * B[i * ldb + j];
        } else {
          temp = B[i * ldb + j];
        }

        for (k = i + 1; k < n1; k++) {
          temp += A[lda * k + i] * B[k * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper && trans == CblasNoTrans) {

    /* form  B := alpha * B * TriU(A) */

    for (i = 0; i < n1; i++) {
      for (j = n2; j > 0 && j--;) {
        BASE temp = 0.0;

        for (k = 0; k < j; k++) {
          temp += A[lda * k + j] * B[i * ldb + k];
        }

        if (nonunit) {
          temp += A[j * lda + j] * B[i * ldb + j];
        } else {
          temp += B[i * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper && trans == CblasTrans) {

    /* form  B := alpha * B * (TriU(A))' */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        if (nonunit) {
          temp = A[j * lda + j] * B[i * ldb + j];
        } else {
          temp = B[i * ldb + j];
        }

        for (k = j + 1; k < n2; k++) {
          temp += A[lda * j + k] * B[i * ldb + k];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasRight && uplo == CblasLower && trans == CblasNoTrans) {

    /* form  B := alpha *B * TriL(A) */

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        BASE temp = 0.0;

        if (nonunit) {
          temp = A[j * lda + j] * B[i * ldb + j];
        } else {
          temp = B[i * ldb + j];
        }

        for (k = j + 1; k < n2; k++) {
          temp += A[lda * k + j] * B[i * ldb + k];
        }


        B[ldb * i + j] = alpha * temp;
      }
    }

  } else if (side == CblasRight && uplo == CblasLower && trans == CblasTrans) {

    /* form  B := alpha * B * TriL(A)' */

    for (i = 0; i < n1; i++) {
      for (j = n2; j > 0 && j--;) {
        BASE temp = 0.0;

        for (k = 0; k < j; k++) {
          temp += A[lda * j + k] * B[i * ldb + k];
        }

        if (nonunit) {
          temp += A[j * lda + j] * B[i * ldb + j];
        } else {
          temp += B[i * ldb + j];
        }

        B[ldb * i + j] = alpha * temp;
      }
    }

  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
