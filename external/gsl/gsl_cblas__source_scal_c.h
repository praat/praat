/* blas/source_scal_c.h
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
  INDEX ix;
  const BASE alpha_real = CONST_REAL0(alpha);
  const BASE alpha_imag = CONST_IMAG0(alpha);

  if (incX <= 0) {
    return;
  }

  ix = OFFSET(N, incX);

  for (i = 0; i < N; i++) {
    const BASE x_real = REAL(X, ix);
    const BASE x_imag = IMAG(X, ix);
    REAL(X, ix) = x_real * alpha_real - x_imag * alpha_imag;
    IMAG(X, ix) = x_real * alpha_imag + x_imag * alpha_real;
    ix += incX;
  }
}
