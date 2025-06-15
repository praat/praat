/* fft/hc_unpack.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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

int
FUNCTION(gsl_fft_halfcomplex,unpack) (const BASE halfcomplex_coefficient[],
                                      BASE complex_coefficient[],
                                      const size_t stride, const size_t n)
{
  size_t i;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  REAL(complex_coefficient,stride,0) = halfcomplex_coefficient[0];
  IMAG(complex_coefficient,stride,0) = 0.0;

  for (i = 1; i < n - i; i++)
    {
      const ATOMIC hc_real = halfcomplex_coefficient[(2 * i - 1) * stride];
      const ATOMIC hc_imag = halfcomplex_coefficient[2 * i * stride];

      REAL(complex_coefficient,stride,i) = hc_real;
      IMAG(complex_coefficient,stride,i) = hc_imag;
      REAL(complex_coefficient,stride,n - i) = hc_real;
      IMAG(complex_coefficient,stride,n - i) = -hc_imag;
    }

  if (i == n - i)
    {
      REAL(complex_coefficient,stride,i) = halfcomplex_coefficient[(n - 1) * stride];
      IMAG(complex_coefficient,stride,i) = 0.0;
    }

  return 0;
}


int
FUNCTION(gsl_fft_halfcomplex,radix2_unpack) (const BASE halfcomplex_coefficient[],
                                             BASE complex_coefficient[],
                                             const size_t stride, const size_t n)
{
  size_t i;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  REAL(complex_coefficient,stride,0) = halfcomplex_coefficient[0];
  IMAG(complex_coefficient,stride,0) = 0.0;

  for (i = 1; i < n - i; i++)
    {
      const ATOMIC hc_real = halfcomplex_coefficient[i * stride];
      const ATOMIC hc_imag = halfcomplex_coefficient[(n - i) * stride];

      REAL(complex_coefficient,stride,i) = hc_real;
      IMAG(complex_coefficient,stride,i) = hc_imag;
      REAL(complex_coefficient,stride,n - i) = hc_real;
      IMAG(complex_coefficient,stride,n - i) = -hc_imag;
    }

  if (i == n - i)
    {
      REAL(complex_coefficient,stride,i) = halfcomplex_coefficient[i * stride];
      IMAG(complex_coefficient,stride,i) = 0.0;
    }

  return 0;
}

