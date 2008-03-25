/* blas/source_tpmv_c.h
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

  const int conj = (TransA == CblasConjTrans) ? -1 : 1;
  const int Trans = (TransA != CblasConjTrans) ? TransA : CblasTrans;
  const int nonunit = (Diag == CblasNonUnit);

  if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasUpper)
      || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasLower)) {
    /* form  x:= A*x */

    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      const BASE Aii_real = CONST_REAL(Ap, TPUP(N, i, i));
      const BASE Aii_imag = conj * CONST_IMAG(Ap, TPUP(N, i, i));
      BASE temp_r;
      BASE temp_i;
      if (nonunit) {
        BASE x_real = REAL(X, ix);
        BASE x_imag = IMAG(X, ix);
        temp_r = Aii_real * x_real - Aii_imag * x_imag;
        temp_i = Aii_real * x_imag + Aii_imag * x_real;
      } else {
        temp_r = REAL(X, ix);
        temp_i = IMAG(X, ix);
      }

      {
        INDEX jx = OFFSET(N, incX) + (i + 1) * incX;
        for (j = i + 1; j < N; j++) {
          const BASE Aij_real = CONST_REAL(Ap, TPUP(N, i, j));
          const BASE Aij_imag = conj * CONST_IMAG(Ap, TPUP(N, i, j));
          BASE x_real = REAL(X, jx);
          BASE x_imag = IMAG(X, jx);
          temp_r += Aij_real * x_real - Aij_imag * x_imag;
          temp_i += Aij_real * x_imag + Aij_imag * x_real;
          jx += incX;
        }
      }

      REAL(X, ix) = temp_r;
      IMAG(X, ix) = temp_i;
      ix += incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasNoTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasTrans && Uplo == CblasUpper)) {

    INDEX ix = OFFSET(N, incX) + incX * (N - 1);
    for (i = N; i > 0 && i--;) {
      const BASE Aii_real = CONST_REAL(Ap, TPLO(N, i, i));
      const BASE Aii_imag = conj * CONST_IMAG(Ap, TPLO(N, i, i));
      BASE temp_r;
      BASE temp_i;
      if (nonunit) {
        BASE x_real = REAL(X, ix);
        BASE x_imag = IMAG(X, ix);
        temp_r = Aii_real * x_real - Aii_imag * x_imag;
        temp_i = Aii_real * x_imag + Aii_imag * x_real;
      } else {
        temp_r = REAL(X, ix);
        temp_i = IMAG(X, ix);
      }

      {
        INDEX jx = OFFSET(N, incX);
        for (j = 0; j < i; j++) {
          const BASE Aij_real = CONST_REAL(Ap, TPLO(N, i, j));
          const BASE Aij_imag = conj * CONST_IMAG(Ap, TPLO(N, i, j));
          BASE x_real = REAL(X, jx);
          BASE x_imag = IMAG(X, jx);
          temp_r += Aij_real * x_real - Aij_imag * x_imag;
          temp_i += Aij_real * x_imag + Aij_imag * x_real;
          jx += incX;
        }
      }

      REAL(X, ix) = temp_r;
      IMAG(X, ix) = temp_i;
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasUpper)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasLower)) {
    /* form  x := A'*x */

    INDEX ix = OFFSET(N, incX) + incX * (N - 1);
    for (i = N; i > 0 && i--;) {
      const BASE Aii_real = CONST_REAL(Ap, TPUP(N, i, i));
      const BASE Aii_imag = conj * CONST_IMAG(Ap, TPUP(N, i, i));
      BASE temp_r;
      BASE temp_i;
      if (nonunit) {
        BASE x_real = REAL(X, ix);
        BASE x_imag = IMAG(X, ix);
        temp_r = Aii_real * x_real - Aii_imag * x_imag;
        temp_i = Aii_real * x_imag + Aii_imag * x_real;
      } else {
        temp_r = REAL(X, ix);
        temp_i = IMAG(X, ix);
      }
      {
        INDEX jx = OFFSET(N, incX);
        for (j = 0; j < i; j++) {
          BASE x_real = REAL(X, jx);
          BASE x_imag = IMAG(X, jx);
          const BASE Aji_real = CONST_REAL(Ap, TPUP(N, j, i));
          const BASE Aji_imag = conj * CONST_IMAG(Ap, TPUP(N, j, i));
          temp_r += Aji_real * x_real - Aji_imag * x_imag;
          temp_i += Aji_real * x_imag + Aji_imag * x_real;
          jx += incX;
        }
      }

      REAL(X, ix) = temp_r;
      IMAG(X, ix) = temp_i;
      ix -= incX;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans && Uplo == CblasLower)
             || (order == CblasColMajor && Trans == CblasNoTrans && Uplo == CblasUpper)) {

    INDEX ix = OFFSET(N, incX);
    for (i = 0; i < N; i++) {
      const BASE Aii_real = CONST_REAL(Ap, TPLO(N, i, i));
      const BASE Aii_imag = conj * CONST_IMAG(Ap, TPLO(N, i, i));
      BASE temp_r;
      BASE temp_i;
      if (nonunit) {
        BASE x_real = REAL(X, ix);
        BASE x_imag = IMAG(X, ix);
        temp_r = Aii_real * x_real - Aii_imag * x_imag;
        temp_i = Aii_real * x_imag + Aii_imag * x_real;
      } else {
        temp_r = REAL(X, ix);
        temp_i = IMAG(X, ix);
      }
      {
        INDEX jx = OFFSET(N, incX) + (i + 1) * incX;
        for (j = i + 1; j < N; j++) {
          BASE x_real = REAL(X, jx);
          BASE x_imag = IMAG(X, jx);
          const BASE Aji_real = CONST_REAL(Ap, TPLO(N, j, i));
          const BASE Aji_imag = conj * CONST_IMAG(Ap, TPLO(N, j, i));
          temp_r += Aji_real * x_real - Aji_imag * x_imag;
          temp_i += Aji_real * x_imag + Aji_imag * x_real;
          jx += incX;
        }
      }
      REAL(X, ix) = temp_r;
      IMAG(X, ix) = temp_i;
      ix += incX;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
