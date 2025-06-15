/* blas/source_ger.h
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

  if (order == CblasRowMajor) {
    INDEX ix = OFFSET(M, incX);
    for (i = 0; i < M; i++) {
      const BASE tmp = alpha * X[ix];
      INDEX jy = OFFSET(N, incY);
      for (j = 0; j < N; j++) {
        A[lda * i + j] += Y[jy] * tmp;
        jy += incY;
      }
      ix += incX;
    }
  } else if (order == CblasColMajor) {
    INDEX jy = OFFSET(N, incY);
    for (j = 0; j < N; j++) {
      const BASE tmp = alpha * Y[jy];
      INDEX ix = OFFSET(M, incX);
      for (i = 0; i < M; i++) {
        A[i + lda * j] += X[ix] * tmp;
        ix += incX;
      }
      jy += incY;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
