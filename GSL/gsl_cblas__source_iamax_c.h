/* blas/source_iamax_c.h
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
  BASE max = 0.0;
  INDEX ix = 0;
  INDEX i;
  CBLAS_INDEX result = 0;

  if (incX <= 0) {
    return 0;
  }

  for (i = 0; i < N; i++) {
    const BASE a = fabs(CONST_REAL(X, ix)) + fabs(CONST_IMAG(X, ix));

    if (a > max) {
      max = a;
      result = i;
    }
    ix += incX;
  }

  return result;
}
