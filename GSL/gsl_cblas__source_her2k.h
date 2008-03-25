/* blas/source_her2k_c.h
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

  const BASE alpha_real = CONST_REAL0(alpha);
  BASE alpha_imag = CONST_IMAG0(alpha);

  if (beta == 1.0 && ((alpha_real == 0.0 && alpha_imag == 0.0) || K == 0))
    return;

  if (Order == CblasRowMajor) {
    uplo = Uplo;
    trans = Trans;
  } else {
    uplo = (Uplo == CblasUpper) ? CblasLower : CblasUpper;
    trans = (Trans == CblasNoTrans) ? CblasConjTrans : CblasNoTrans;
    alpha_imag *= -1;           /* conjugate alpha */
  }

  /* form  C := beta*C */

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
        IMAG(C, ldc * i + i) = 0.0;
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
        IMAG(C, ldc * i + i) = 0.0;
      }
    }
  } else {
    for (i = 0; i < N; i++) {
      IMAG(C, ldc * i + i) = 0.0;
    }
  }

  if (alpha_real == 0.0 && alpha_imag == 0.0)
    return;

  if (uplo == CblasUpper && trans == CblasNoTrans) {

    for (i = 0; i < N; i++) {

      /* Cii += alpha Aik conj(Bik) + conj(alpha) Bik conj(Aik) */
      {
        BASE temp_real = 0.0;
        /* BASE temp_imag = 0.0; */
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          /* temp1 = alpha * Aik */
          const BASE temp1_real = alpha_real * Aik_real - alpha_imag * Aik_imag;
          const BASE temp1_imag = alpha_real * Aik_imag + alpha_imag * Aik_real;
          const BASE Bik_real = CONST_REAL(B, i * ldb + k);
          const BASE Bik_imag = CONST_IMAG(B, i * ldb + k);
          temp_real += temp1_real * Bik_real + temp1_imag * Bik_imag;
        }

        REAL(C, i * ldc + i) += 2 * temp_real;
        IMAG(C, i * ldc + i) = 0.0;
      }

      /* Cij += alpha Aik conj(Bjk) + conj(alpha) Bik conj(Ajk) */
      for (j = i + 1; j < N; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          /* temp1 = alpha * Aik */
          const BASE temp1_real = alpha_real * Aik_real - alpha_imag * Aik_imag;
          const BASE temp1_imag = alpha_real * Aik_imag + alpha_imag * Aik_real;
          const BASE Bik_real = CONST_REAL(B, i * ldb + k);
          const BASE Bik_imag = CONST_IMAG(B, i * ldb + k);

          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = CONST_IMAG(A, j * lda + k);
          /* temp2 = alpha * Ajk */
          const BASE temp2_real = alpha_real * Ajk_real - alpha_imag * Ajk_imag;
          const BASE temp2_imag = alpha_real * Ajk_imag + alpha_imag * Ajk_real;
          const BASE Bjk_real = CONST_REAL(B, j * ldb + k);
          const BASE Bjk_imag = CONST_IMAG(B, j * ldb + k);

          /* Cij += alpha * Aik * conj(Bjk) + conj(alpha) * Bik * conj(Ajk) */
          temp_real += ((temp1_real * Bjk_real + temp1_imag * Bjk_imag)
                        + (Bik_real * temp2_real + Bik_imag * temp2_imag));
          temp_imag += ((temp1_real * (-Bjk_imag) + temp1_imag * Bjk_real)
                        + (Bik_real * (-temp2_imag) + Bik_imag * temp2_real));
        }
        REAL(C, i * ldc + j) += temp_real;
        IMAG(C, i * ldc + j) += temp_imag;
      }
    }

  } else if (uplo == CblasUpper && trans == CblasConjTrans) {

    for (k = 0; k < K; k++) {
      for (i = 0; i < N; i++) {
        BASE Aki_real = CONST_REAL(A, k * lda + i);
        BASE Aki_imag = CONST_IMAG(A, k * lda + i);
        BASE Bki_real = CONST_REAL(B, k * ldb + i);
        BASE Bki_imag = CONST_IMAG(B, k * ldb + i);
        /* temp1 = alpha * conj(Aki) */
        BASE temp1_real = alpha_real * Aki_real - alpha_imag * (-Aki_imag);
        BASE temp1_imag = alpha_real * (-Aki_imag) + alpha_imag * Aki_real;
        /* temp2 = conj(alpha) * conj(Bki) */
        BASE temp2_real = alpha_real * Bki_real - alpha_imag * Bki_imag;
        BASE temp2_imag = -(alpha_real * Bki_imag + alpha_imag * Bki_real);

        /* Cii += alpha * conj(Aki) * Bki + conj(alpha) * conj(Bki) * Aki */
        {
          REAL(C, i * lda + i) += 2 * (temp1_real * Bki_real - temp1_imag * Bki_imag);
          IMAG(C, i * lda + i) = 0.0;
        }

        for (j = i + 1; j < N; j++) {
          BASE Akj_real = CONST_REAL(A, k * lda + j);
          BASE Akj_imag = CONST_IMAG(A, k * lda + j);
          BASE Bkj_real = CONST_REAL(B, k * ldb + j);
          BASE Bkj_imag = CONST_IMAG(B, k * ldb + j);
          /* Cij += alpha * conj(Aki) * Bkj + conj(alpha) * conj(Bki) * Akj */
          REAL(C, i * lda + j) += (temp1_real * Bkj_real - temp1_imag * Bkj_imag)
              + (temp2_real * Akj_real - temp2_imag * Akj_imag);
          IMAG(C, i * lda + j) += (temp1_real * Bkj_imag + temp1_imag * Bkj_real)
              + (temp2_real * Akj_imag + temp2_imag * Akj_real);
        }
      }
    }

  } else if (uplo == CblasLower && trans == CblasNoTrans) {

    for (i = 0; i < N; i++) {

      /* Cij += alpha Aik conj(Bjk) + conj(alpha) Bik conj(Ajk) */

      for (j = 0; j < i; j++) {
        BASE temp_real = 0.0;
        BASE temp_imag = 0.0;
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          /* temp1 = alpha * Aik */
          const BASE temp1_real = alpha_real * Aik_real - alpha_imag * Aik_imag;
          const BASE temp1_imag = alpha_real * Aik_imag + alpha_imag * Aik_real;
          const BASE Bik_real = CONST_REAL(B, i * ldb + k);
          const BASE Bik_imag = CONST_IMAG(B, i * ldb + k);

          const BASE Ajk_real = CONST_REAL(A, j * lda + k);
          const BASE Ajk_imag = CONST_IMAG(A, j * lda + k);
          /* temp2 = alpha * Ajk */
          const BASE temp2_real = alpha_real * Ajk_real - alpha_imag * Ajk_imag;
          const BASE temp2_imag = alpha_real * Ajk_imag + alpha_imag * Ajk_real;
          const BASE Bjk_real = CONST_REAL(B, j * ldb + k);
          const BASE Bjk_imag = CONST_IMAG(B, j * ldb + k);

          /* Cij += alpha * Aik * conj(Bjk) + conj(alpha) * Bik * conj(Ajk) */
          temp_real += ((temp1_real * Bjk_real + temp1_imag * Bjk_imag)
                        + (Bik_real * temp2_real + Bik_imag * temp2_imag));
          temp_imag += ((temp1_real * (-Bjk_imag) + temp1_imag * Bjk_real)
                        + (Bik_real * (-temp2_imag) + Bik_imag * temp2_real));
        }
        REAL(C, i * ldc + j) += temp_real;
        IMAG(C, i * ldc + j) += temp_imag;
      }

      /* Cii += alpha Aik conj(Bik) + conj(alpha) Bik conj(Aik) */
      {
        BASE temp_real = 0.0;
        /* BASE temp_imag = 0.0; */
        for (k = 0; k < K; k++) {
          const BASE Aik_real = CONST_REAL(A, i * lda + k);
          const BASE Aik_imag = CONST_IMAG(A, i * lda + k);
          /* temp1 = alpha * Aik */
          const BASE temp1_real = alpha_real * Aik_real - alpha_imag * Aik_imag;
          const BASE temp1_imag = alpha_real * Aik_imag + alpha_imag * Aik_real;
          const BASE Bik_real = CONST_REAL(B, i * ldb + k);
          const BASE Bik_imag = CONST_IMAG(B, i * ldb + k);
          temp_real += temp1_real * Bik_real + temp1_imag * Bik_imag;
        }

        REAL(C, i * ldc + i) += 2 * temp_real;
        IMAG(C, i * ldc + i) = 0.0;
      }
    }

  } else if (uplo == CblasLower && trans == CblasConjTrans) {

    for (k = 0; k < K; k++) {
      for (i = 0; i < N; i++) {
        BASE Aki_real = CONST_REAL(A, k * lda + i);
        BASE Aki_imag = CONST_IMAG(A, k * lda + i);
        BASE Bki_real = CONST_REAL(B, k * ldb + i);
        BASE Bki_imag = CONST_IMAG(B, k * ldb + i);
        /* temp1 = alpha * conj(Aki) */
        BASE temp1_real = alpha_real * Aki_real - alpha_imag * (-Aki_imag);
        BASE temp1_imag = alpha_real * (-Aki_imag) + alpha_imag * Aki_real;
        /* temp2 = conj(alpha) * conj(Bki) */
        BASE temp2_real = alpha_real * Bki_real - alpha_imag * Bki_imag;
        BASE temp2_imag = -(alpha_real * Bki_imag + alpha_imag * Bki_real);

        for (j = 0; j < i; j++) {
          BASE Akj_real = CONST_REAL(A, k * lda + j);
          BASE Akj_imag = CONST_IMAG(A, k * lda + j);
          BASE Bkj_real = CONST_REAL(B, k * ldb + j);
          BASE Bkj_imag = CONST_IMAG(B, k * ldb + j);
          /* Cij += alpha * conj(Aki) * Bkj + conj(alpha) * conj(Bki) * Akj */
          REAL(C, i * lda + j) += (temp1_real * Bkj_real - temp1_imag * Bkj_imag)
              + (temp2_real * Akj_real - temp2_imag * Akj_imag);
          IMAG(C, i * lda + j) += (temp1_real * Bkj_imag + temp1_imag * Bkj_real)
              + (temp2_real * Akj_imag + temp2_imag * Akj_real);
        }

        /* Cii += alpha * conj(Aki) * Bki + conj(alpha) * conj(Bki) * Aki */
        {
          REAL(C, i * lda + i) += 2 * (temp1_real * Bki_real - temp1_imag * Bki_imag);
          IMAG(C, i * lda + i) = 0.0;
        }
      }
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
