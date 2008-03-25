/* blas/source_copy_c.h
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
    REAL(Y, iy) = CONST_REAL(X, ix);
    IMAG(Y, iy) = CONST_IMAG(X, ix);
    ix += incX;
    iy += incY;
  }
}
