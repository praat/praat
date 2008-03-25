/* blas/source_gbmv_c.h
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
  INDEX lenX, lenY, L, U;

  const BASE alpha_real = CONST_REAL0(alpha);
  const BASE alpha_imag = CONST_IMAG0(alpha);

  const BASE beta_real = CONST_REAL0(beta);
  const BASE beta_imag = CONST_IMAG0(beta);

  if (M == 0 || N == 0)
    return;

  if ((alpha_real == 0.0 && alpha_imag == 0.0)
      && (beta_real == 1.0 && beta_imag == 0.0))
    return;

  if (TransA == CblasNoTrans) {
    lenX = N;
    lenY = M;
    L = KL;
    U = KU;
  } else {
    lenX = M;
    lenY = N;
    L = KU;
    U = KL;
  }

  /* form  y := beta*y */
  if (beta_real == 0.0 && beta_imag == 0.0) {
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      REAL(Y, iy) = 0.0;
      IMAG(Y, iy) = 0.0;
      iy += incY;
    }
  } else if (!(beta_real == 1.0 && beta_imag == 0.0)) {
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
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

  if ((order == CblasRowMajor && TransA == CblasNoTrans)
      || (order == CblasColMajor && TransA == CblasTrans)) {
    /* form  y := alpha*A*x + y */
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      BASE dotR = 0.0;
      BASE dotI = 0.0;
      const INDEX j_min = (i > L ? i - L : 0);
      const INDEX j_max = GSL_MIN(lenX, i + U + 1);
      INDEX ix = OFFSET(lenX, incX) + j_min * incX;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = CONST_REAL(X, ix);
        const BASE x_imag = CONST_IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + (L + j - i));
        const BASE A_imag = CONST_IMAG(A, lda * i + (L + j - i));

        dotR += A_real * x_real - A_imag * x_imag;
        dotI += A_real * x_imag + A_imag * x_real;
        ix += incX;
      }

      REAL(Y, iy) += alpha_real * dotR - alpha_imag * dotI;
      IMAG(Y, iy) += alpha_real * dotI + alpha_imag * dotR;
      iy += incY;
    }
  } else if ((order == CblasRowMajor && TransA == CblasTrans)
             || (order == CblasColMajor && TransA == CblasNoTrans)) {
    /* form  y := alpha*A'*x + y */
    INDEX ix = OFFSET(lenX, incX);
    for (j = 0; j < lenX; j++) {
      const BASE x_real = CONST_REAL(X, ix);
      const BASE x_imag = CONST_IMAG(X, ix);
      BASE tmpR = alpha_real * x_real - alpha_imag * x_imag;
      BASE tmpI = alpha_real * x_imag + alpha_imag * x_real;
      if (!(tmpR == 0.0 && tmpI == 0.0)) {
        const INDEX i_min = (j > U ? j - U : 0);
        const INDEX i_max = GSL_MIN(lenY, j + L + 1);
        INDEX iy = OFFSET(lenY, incY) + i_min * incY;
        for (i = i_min; i < i_max; i++) {
          const BASE A_real = CONST_REAL(A, lda * j + (U + i - j));
          const BASE A_imag = CONST_IMAG(A, lda * j + (U + i - j));
          REAL(Y, iy) += A_real * tmpR - A_imag * tmpI;
          IMAG(Y, iy) += A_real * tmpI + A_imag * tmpR;
          iy += incY;
        }
      }
      ix += incX;
    }
  } else if (order == CblasRowMajor && TransA == CblasConjTrans) {
    /* form  y := alpha*A^H*x + y */
    INDEX ix = OFFSET(lenX, incX);
    for (j = 0; j < lenX; j++) {
      const BASE x_real = CONST_REAL(X, ix);
      const BASE x_imag = CONST_IMAG(X, ix);
      BASE tmpR = alpha_real * x_real - alpha_imag * x_imag;
      BASE tmpI = alpha_real * x_imag + alpha_imag * x_real;
      if (!(tmpR == 0.0 && tmpI == 0.0)) {
        const INDEX i_min = (j > U ? j - U : 0);
        const INDEX i_max = GSL_MIN(lenY, j + L + 1);
        INDEX iy = OFFSET(lenY, incY) + i_min * incY;
        for (i = i_min; i < i_max; i++) {
          const BASE A_real = CONST_REAL(A, lda * j + (U + i - j));
          const BASE A_imag = CONST_IMAG(A, lda * j + (U + i - j));
          REAL(Y, iy) += A_real * tmpR - (-A_imag) * tmpI;
          IMAG(Y, iy) += A_real * tmpI + (-A_imag) * tmpR;
          iy += incY;
        }
      }
      ix += incX;
    }
  } else if (order == CblasColMajor && TransA == CblasConjTrans) {
    /* form  y := alpha*A^H*x + y */
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      BASE dotR = 0.0;
      BASE dotI = 0.0;
      const INDEX j_min = (i > L ? i - L : 0);
      const INDEX j_max = GSL_MIN(lenX, i + U + 1);
      INDEX ix = OFFSET(lenX, incX) + j_min * incX;
      for (j = j_min; j < j_max; j++) {
        const BASE x_real = CONST_REAL(X, ix);
        const BASE x_imag = CONST_IMAG(X, ix);
        const BASE A_real = CONST_REAL(A, lda * i + (L + j - i));
        const BASE A_imag = CONST_IMAG(A, lda * i + (L + j - i));

        dotR += A_real * x_real - (-A_imag) * x_imag;
        dotI += A_real * x_imag + (-A_imag) * x_real;
        ix += incX;
      }

      REAL(Y, iy) += alpha_real * dotR - alpha_imag * dotI;
      IMAG(Y, iy) += alpha_real * dotI + alpha_imag * dotR;
      iy += incY;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
