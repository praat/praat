/* blas/source_rotg.h
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
  const BASE roe = (fabs(*a) > fabs(*b) ? *a : *b);
  const BASE scale = fabs(*a) + fabs(*b);
  BASE r, z;

  if (scale != 0.0) {
    const BASE aos = *a / scale;
    const BASE bos = *b / scale;
    r = scale * sqrt(aos * aos + bos * bos);
    r = GSL_SIGN(roe) * r;
    *c = *a / r;
    *s = *b / r;
    z = 1.0;
    if (fabs(*a) > fabs(*b))
      z = *s;
    if (fabs(*b) >= fabs(*a) && *c != 0.0)
      z = 1.0 / (*c);
  } else {
    *c = 1.0;
    *s = 0.0;
    r = 0.0;
    z = 0.0;
  }

  *a = r;
  *b = z;
}
