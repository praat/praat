/* blas/source_nrm2_c.h
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
  BASE scale = 0.0;
  BASE ssq = 1.0;
  INDEX i;
  INDEX ix = 0;

  if (N == 0 || incX < 1) {
    return 0;
  }

  for (i = 0; i < N; i++) {
    const BASE x = CONST_REAL(X, ix);
    const BASE y = CONST_IMAG(X, ix);

    if (x != 0.0) {
      const BASE ax = fabs(x);

      if (scale < ax) {
        ssq = 1.0 + ssq * (scale / ax) * (scale / ax);
        scale = ax;
      } else {
        ssq += (ax / scale) * (ax / scale);
      }
    }

    if (y != 0.0) {
      const BASE ay = fabs(y);

      if (scale < ay) {
        ssq = 1.0 + ssq * (scale / ay) * (scale / ay);
        scale = ay;
      } else {
        ssq += (ay / scale) * (ay / scale);
      }
    }

    ix += incX;
  }

  return scale * sqrt(ssq);
}
