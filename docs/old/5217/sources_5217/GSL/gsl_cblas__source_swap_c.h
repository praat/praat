/* blas/source_swap_c.h
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
  INDEX i;
  INDEX ix = OFFSET(N, incX);
  INDEX iy = OFFSET(N, incY);

  for (i = 0; i < N; i++) {
    const BASE tmp_real = REAL(X, ix);
    const BASE tmp_imag = IMAG(X, ix);
    REAL(X, ix) = REAL(Y, iy);
    IMAG(X, ix) = IMAG(Y, iy);
    REAL(Y, iy) = tmp_real;
    IMAG(Y, iy) = tmp_imag;
    ix += incX;
    iy += incY;
  }
}
