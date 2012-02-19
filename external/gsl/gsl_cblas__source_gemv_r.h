/* blas/source_gemv_r.h
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
  INDEX lenX, lenY;

  const int Trans = (TransA != CblasConjTrans) ? TransA : CblasTrans;

  if (M == 0 || N == 0)
    return;

  if (alpha == 0.0 && beta == 1.0)
    return;

  if (Trans == CblasNoTrans) {
    lenX = N;
    lenY = M;
  } else {
    lenX = M;
    lenY = N;
  }

  /* form  y := beta*y */
  if (beta == 0.0) {
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      Y[iy] = 0.0;
      iy += incY;
    }
  } else if (beta != 1.0) {
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      Y[iy] *= beta;
      iy += incY;
    }
  }

  if (alpha == 0.0)
    return;

  if ((order == CblasRowMajor && Trans == CblasNoTrans)
      || (order == CblasColMajor && Trans == CblasTrans)) {
    /* form  y := alpha*A*x + y */
    INDEX iy = OFFSET(lenY, incY);
    for (i = 0; i < lenY; i++) {
      BASE temp = 0.0;
      INDEX ix = OFFSET(lenX, incX);
      for (j = 0; j < lenX; j++) {
        temp += X[ix] * A[lda * i + j];
        ix += incX;
      }
      Y[iy] += alpha * temp;
      iy += incY;
    }
  } else if ((order == CblasRowMajor && Trans == CblasTrans)
             || (order == CblasColMajor && Trans == CblasNoTrans)) {
    /* form  y := alpha*A'*x + y */
    INDEX ix = OFFSET(lenX, incX);
    for (j = 0; j < lenX; j++) {
      const BASE temp = alpha * X[ix];
      if (temp != 0.0) {
        INDEX iy = OFFSET(lenY, incY);
        for (i = 0; i < lenY; i++) {
          Y[iy] += temp * A[lda * j + i];
          iy += incY;
        }
      }
      ix += incX;
    }
  } else {
    BLAS_ERROR("unrecognized operation");
  }
}
