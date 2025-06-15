/* blas/source_trsm_r.h
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

    /* form  B := alpha * inv(TriU(A)) *B */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = n1; i > 0 && i--;) {
      if (nonunit) {
        BASE Aii = A[lda * i + i];
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] /= Aii;
        }
      }

      for (k = 0; k < i; k++) {
        const BASE Aki = A[k * lda + i];
        for (j = 0; j < n2; j++) {
          B[ldb * k + j] -= Aki * B[ldb * i + j];
        }
      }
    }

  } else if (side == CblasLeft && uplo == CblasUpper && trans == CblasTrans) {

    /* form  B := alpha * inv(TriU(A))' *B */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      if (nonunit) {
        BASE Aii = A[lda * i + i];
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] /= Aii;
        }
      }

      for (k = i + 1; k < n1; k++) {
        const BASE Aik = A[i * lda + k];
        for (j = 0; j < n2; j++) {
          B[ldb * k + j] -= Aik * B[ldb * i + j];
        }
      }
    }

  } else if (side == CblasLeft && uplo == CblasLower && trans == CblasNoTrans) {

    /* form  B := alpha * inv(TriL(A))*B */


    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      if (nonunit) {
        BASE Aii = A[lda * i + i];
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] /= Aii;
        }
      }

      for (k = i + 1; k < n1; k++) {
        const BASE Aki = A[k * lda + i];
        for (j = 0; j < n2; j++) {
          B[ldb * k + j] -= Aki * B[ldb * i + j];
        }
      }
    }


  } else if (side == CblasLeft && uplo == CblasLower && trans == CblasTrans) {

    /* form  B := alpha * TriL(A)' *B */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = n1; i > 0 && i--;) {
      if (nonunit) {
        BASE Aii = A[lda * i + i];
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] /= Aii;
        }
      }

      for (k = 0; k < i; k++) {
        const BASE Aik = A[i * lda + k];
        for (j = 0; j < n2; j++) {
          B[ldb * k + j] -= Aik * B[ldb * i + j];
        }
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper && trans == CblasNoTrans) {

    /* form  B := alpha * B * inv(TriU(A)) */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        if (nonunit) {
          BASE Ajj = A[lda * j + j];
          B[ldb * i + j] /= Ajj;
        }

        {
          BASE Bij = B[ldb * i + j];
          for (k = j + 1; k < n2; k++) {
            B[ldb * i + k] -= A[j * lda + k] * Bij;
          }
        }
      }
    }

  } else if (side == CblasRight && uplo == CblasUpper && trans == CblasTrans) {

    /* form  B := alpha * B * inv(TriU(A))' */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      for (j = n2; j > 0 && j--;) {

        if (nonunit) {
          BASE Ajj = A[lda * j + j];
          B[ldb * i + j] /= Ajj;
        }

        {
          BASE Bij = B[ldb * i + j];
          for (k = 0; k < j; k++) {
            B[ldb * i + k] -= A[k * lda + j] * Bij;
          }
        }
      }
    }


  } else if (side == CblasRight && uplo == CblasLower && trans == CblasNoTrans) {

    /* form  B := alpha * B * inv(TriL(A)) */

    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      for (j = n2; j > 0 && j--;) {

        if (nonunit) {
          BASE Ajj = A[lda * j + j];
          B[ldb * i + j] /= Ajj;
        }

        {
          BASE Bij = B[ldb * i + j];
          for (k = 0; k < j; k++) {
            B[ldb * i + k] -= A[j * lda + k] * Bij;
          }
        }
      }
    }

  } else if (side == CblasRight && uplo == CblasLower && trans == CblasTrans) {

    /* form  B := alpha * B * inv(TriL(A))' */


    if (alpha != 1.0) {
      for (i = 0; i < n1; i++) {
        for (j = 0; j < n2; j++) {
          B[ldb * i + j] *= alpha;
        }
      }
    }

    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        if (nonunit) {
          BASE Ajj = A[lda * j + j];
          B[ldb * i + j] /= Ajj;
        }

        {
          BASE Bij = B[ldb * i + j];
          for (k = j + 1; k < n2; k++) {
            B[ldb * i + k] -= A[k * lda + j] * Bij;
          }
        }
      }
    }



  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
