/* fft/dft_source.c
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
FUNCTION(gsl_dft_complex,forward) (const BASE data[], 
                                   const size_t stride, const size_t n,
                                   BASE result[])
{
  gsl_fft_direction sign = gsl_fft_forward;
  int status = FUNCTION(gsl_dft_complex,transform) (data, stride, n, result, sign);
  return status;
}

int
FUNCTION(gsl_dft_complex,backward) (const BASE data[], 
                                    const size_t stride, const size_t n,
                                    BASE result[])
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_dft_complex,transform) (data, stride, n, result, sign);
  return status;
}


int
FUNCTION(gsl_dft_complex,inverse) (const BASE data[], 
                                   const size_t stride, const size_t n,
                                   BASE result[])
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_dft_complex,transform) (data, stride, n, result, sign);

  /* normalize inverse fft with 1/n */

  {
    const ATOMIC norm = ONE / (ATOMIC)n;
    size_t i;
    for (i = 0; i < n; i++)
      {
        REAL(result,stride,i) *= norm;
        IMAG(result,stride,i) *= norm;
      }
  }
  return status;
}

int
FUNCTION(gsl_dft_complex,transform) (const BASE data[], 
                                     const size_t stride, const size_t n,
                                     BASE result[],
                                     const gsl_fft_direction sign)
{

  size_t i, j, exponent;

  const double d_theta = 2.0 * ((int) sign) * M_PI / (double) n;

  /* FIXME: check that input length == output length and give error */

  for (i = 0; i < n; i++)
    {
      ATOMIC sum_real = 0;
      ATOMIC sum_imag = 0;

      exponent = 0;

      for (j = 0; j < n; j++)
        {
          double theta = d_theta * (double) exponent;
          /* sum = exp(i theta) * data[j] */

          ATOMIC w_real = (ATOMIC) cos (theta);
          ATOMIC w_imag = (ATOMIC) sin (theta);

          ATOMIC data_real = REAL(data,stride,j);
          ATOMIC data_imag = IMAG(data,stride,j);

          sum_real += w_real * data_real - w_imag * data_imag;
          sum_imag += w_real * data_imag + w_imag * data_real;

          exponent = (exponent + i) % n;
        }
      REAL(result,stride,i) = sum_real;
      IMAG(result,stride,i) = sum_imag;
    }

  return 0;
}
