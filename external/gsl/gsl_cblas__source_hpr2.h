/* blas/source_hpr2.h
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
  const int conj = (order == CblasColMajor) ? -1 : 1;
  INDEX i, j;

  const BASE alpha_real = CONST_REAL0(alpha);
  const BASE alpha_imag = CONST_IMAG0(alpha);

  if (alpha_real == 0.0 && alpha_imag == 0.0)
    return;

  if ((order == CblasRowMajor && Uplo == CblasUpper)
      || (order == CblasColMajor && Uplo == CblasLower)) {
    INDEX ix = OFFSET(N, incX);
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      const BASE Xi_real = CONST_REAL(X, ix);
      const BASE Xi_imag = CONST_IMAG(X, ix);
      /* tmp1 = alpha Xi */
      const BASE tmp1_real = alpha_real * Xi_real - alpha_imag * Xi_imag;
      const BASE tmp1_imag = alpha_imag * Xi_real + alpha_real * Xi_imag;

      const BASE Yi_real = CONST_REAL(Y, iy);
      const BASE Yi_imag = CONST_IMAG(Y, iy);
      /* tmp2 = conj(alpha) Yi */
      const BASE tmp2_real = alpha_real * Yi_real + alpha_imag * Yi_imag;
      const BASE tmp2_imag = -alpha_imag * Yi_real + alpha_real * Yi_imag;

      INDEX jx = ix + incX;
      INDEX jy = iy + incY;

      /* Aij = alpha*Xi*conj(Yj) + conj(alpha)*Yi*conj(Xj) */

      REAL(Ap, TPUP(N, i, i)) += 2 * (tmp1_real * Yi_real + tmp1_imag * Yi_imag);
      IMAG(Ap, TPUP(N, i, i)) = 0;

      for (j = i + 1; j < N; j++) {
        const BASE Xj_real = CONST_REAL(X, jx);
        const BASE Xj_imag = CONST_IMAG(X, jx);
        const BASE Yj_real = CONST_REAL(Y, jy);
        const BASE Yj_imag = CONST_IMAG(Y, jy);
        REAL(Ap, TPUP(N, i, j)) += ((tmp1_real * Yj_real + tmp1_imag * Yj_imag)
                                    + (tmp2_real * Xj_real + tmp2_imag * Xj_imag));
        IMAG(Ap, TPUP(N, i, j)) +=
            conj * ((tmp1_imag * Yj_real - tmp1_real * Yj_imag) +
                    (tmp2_imag * Xj_real - tmp2_real * Xj_imag));
        jx += incX;
        jy += incY;
      }
      ix += incX;
      iy += incY;
    }
  } else if ((order == CblasRowMajor && Uplo == CblasLower)
             || (order == CblasColMajor && Uplo == CblasUpper)) {

    INDEX ix = OFFSET(N, incX);
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      const BASE Xi_real = CONST_REAL(X, ix);
      const BASE Xi_imag = CONST_IMAG(X, ix);
      const BASE tmp1_real = alpha_real * Xi_real - alpha_imag * Xi_imag;
      const BASE tmp1_imag = alpha_imag * Xi_real + alpha_real * Xi_imag;

      const BASE Yi_real = CONST_REAL(Y, iy);
      const BASE Yi_imag = CONST_IMAG(Y, iy);
      const BASE tmp2_real = alpha_real * Yi_real + alpha_imag * Yi_imag;
      const BASE tmp2_imag = -alpha_imag * Yi_real + alpha_real * Yi_imag;

      INDEX jx = OFFSET(N, incX);
      INDEX jy = OFFSET(N, incY);

      /* Aij = alpha*Xi*conj(Yj) + conj(alpha)*Yi*conj(Xj) */

      for (j = 0; j < i; j++) {
        const BASE Xj_real = CONST_REAL(X, jx);
        const BASE Xj_imag = CONST_IMAG(X, jx);
        const BASE Yj_real = CONST_REAL(Y, jy);
        const BASE Yj_imag = CONST_IMAG(Y, jy);
        REAL(Ap, TPLO(N, i, j)) += ((tmp1_real * Yj_real + tmp1_imag * Yj_imag)
                                    + (tmp2_real * Xj_real + tmp2_imag * Xj_imag));
        IMAG(Ap, TPLO(N, i, j)) +=
            conj * ((tmp1_imag * Yj_real - tmp1_real * Yj_imag) +
                    (tmp2_imag * Xj_real - tmp2_real * Xj_imag));
        jx += incX;
        jy += incY;
      }

      REAL(Ap, TPLO(N, i, i)) += 2 * (tmp1_real * Yi_real + tmp1_imag * Yi_imag);
      IMAG(Ap, TPLO(N, i, i)) = 0;

      ix += incX;
      iy += incY;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
