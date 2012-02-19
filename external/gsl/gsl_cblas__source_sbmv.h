/* blas/source_sbmv.h
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

  if (N == 0)
    return;

  if (alpha == 0.0 && beta == 1.0)
    return;

  /* form  y := beta*y */
  if (beta == 0.0) {
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      Y[iy] = 0.0;
      iy += incY;
    }
  } else if (beta != 1.0) {
    INDEX iy = OFFSET(N, incY);
    for (i = 0; i < N; i++) {
      Y[iy] *= beta;
      iy += incY;
    }
  }

  if (alpha == 0.0)
    return;

  /* form  y := alpha*A*x + y */

  if ((order == CblasRowMajor && Uplo == CblasUpper)
      || (order == CblasColMajor && Uplo == CblasLower)) {
    INDEX ix = OFFSET(N, incX);
    INDEX iy = OFFSET(N, incY);

    for (i = 0; i < N; i++) {
      BASE tmp1 = alpha * X[ix];
      BASE tmp2 = 0.0;
      const INDEX j_min = i + 1;
      const INDEX j_max = GSL_MIN(N, i + K + 1);
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      INDEX jy = OFFSET(N, incY) + j_min * incY;
      Y[iy] += tmp1 * A[0 + i * lda];
      for (j = j_min; j < j_max; j++) {
        BASE Aij = A[(j - i) + i * lda];
        Y[jy] += tmp1 * Aij;
        tmp2 += Aij * X[jx];
        jx += incX;
        jy += incY;
      }
      Y[iy] += alpha * tmp2;
      ix += incX;
      iy += incY;
    }
  } else if ((order == CblasRowMajor && Uplo == CblasLower)
             || (order == CblasColMajor && Uplo == CblasUpper)) {
    INDEX ix = OFFSET(N, incX);
    INDEX iy = OFFSET(N, incY);

    for (i = 0; i < N; i++) {
      BASE tmp1 = alpha * X[ix];
      BASE tmp2 = 0.0;
      const INDEX j_min = (i > K) ? i - K : 0;
      const INDEX j_max = i;
      INDEX jx = OFFSET(N, incX) + j_min * incX;
      INDEX jy = OFFSET(N, incY) + j_min * incY;
      for (j = j_min; j < j_max; j++) {
        BASE Aij = A[(K - i + j) + i * lda];
        Y[jy] += tmp1 * Aij;
        tmp2 += Aij * X[jx];
        jx += incX;
        jy += incY;
      }
      Y[iy] += tmp1 * A[K + i * lda] + alpha * tmp2;
      ix += incX;
      iy += incY;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }

}
