/* fft/signals_source.c
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

#include "gsl_fft__signals.h"

int
FUNCTION(fft_signal,complex_pulse) (const size_t k,
                                    const size_t n,
                                    const size_t stride,
                                    const BASE z_real,
                                    const BASE z_imag,
                                    BASE data[],
                                    BASE fft[])
{
  size_t j;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  /* gsl_complex pulse at position k,  data[j] = z * delta_{jk} */

  for (j = 0; j < n; j++)
    {
      REAL(data,stride,j) = 0.0;
      IMAG(data,stride,j) = 0.0;
    }

  REAL(data,stride,k % n) = z_real;
  IMAG(data,stride,k % n) = z_imag;

  /* fourier transform, fft[j] = z * exp(-2 pi i j k / n) */

  for (j = 0; j < n; j++)
    {
      const double arg = -2 * M_PI * ((double) ((j * k) % n)) / ((double) n);
      const BASE w_real = (BASE)cos (arg);
      const BASE w_imag = (BASE)sin (arg);
      REAL(fft,stride,j) = w_real * z_real - w_imag * z_imag;
      IMAG(fft,stride,j) = w_real * z_imag + w_imag * z_real;
    }

  return 0;

}


int
FUNCTION(fft_signal,complex_constant) (const size_t n,
                                       const size_t stride,
                                       const BASE z_real,
                                       const BASE z_imag,
                                       BASE data[],
                                       BASE fft[])
{
  size_t j;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  /* constant, data[j] = z */

  for (j = 0; j < n; j++)
    {
      REAL(data,stride,j) = z_real;
      IMAG(data,stride,j) = z_imag;
    }

  /* fourier transform, fft[j] = n z delta_{j0} */

  for (j = 0; j < n; j++)
    {
      REAL(fft,stride,j) = 0.0;
      IMAG(fft,stride,j) = 0.0;
    }

  REAL(fft,stride,0) = ((BASE) n) * z_real;
  IMAG(fft,stride,0) = ((BASE) n) * z_imag;

  return 0;

}


int
FUNCTION(fft_signal,complex_exp) (const int k,
                                  const size_t n,
                                  const size_t stride,
                                  const BASE z_real,
                                  const BASE z_imag,
                                  BASE data[],
                                  BASE fft[])
{
  size_t j;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  /* exponential,  data[j] = z * exp(2 pi i j k) */

  for (j = 0; j < n; j++)
    {
      const double arg = 2 * M_PI * ((double) ((j * k) % n)) / ((double) n);
      const BASE w_real = (BASE)cos (arg);
      const BASE w_imag = (BASE)sin (arg);
      REAL(data,stride,j) = w_real * z_real - w_imag * z_imag;
      IMAG(data,stride,j) = w_real * z_imag + w_imag * z_real;
    }

  /* fourier transform, fft[j] = z * delta{(j - k),0} */

  for (j = 0; j < n; j++)
    {
      REAL(fft,stride,j) = 0.0;
      IMAG(fft,stride,j) = 0.0;
    }

  {
    int freq;

    if (k <= 0)
      {
        freq = (n-k) % n ;
      }
    else
      {
        freq = (k % n);
      };

    REAL(fft,stride,freq) = ((BASE) n) * z_real;
    IMAG(fft,stride,freq) = ((BASE) n) * z_imag;
  }

  return 0;

}


int
FUNCTION(fft_signal,complex_exppair) (const int k1,
                                      const int k2,
                                      const size_t n,
                                      const size_t stride,
                                      const BASE z1_real,
                                      const BASE z1_imag,
                                      const BASE z2_real,
                                      const BASE z2_imag,
                                      BASE data[],
                                      BASE fft[])
{
  size_t j;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  /* exponential,  data[j] = z1 * exp(2 pi i j k1) + z2 * exp(2 pi i j k2) */

  for (j = 0; j < n; j++)
    {
      const double arg1 = 2 * M_PI * ((double) ((j * k1) % n)) / ((double) n);
      const BASE w1_real = (BASE)cos (arg1);
      const BASE w1_imag = (BASE)sin (arg1);
      const double arg2 = 2 * M_PI * ((double) ((j * k2) % n)) / ((double) n);
      const BASE w2_real = (BASE)cos (arg2);
      const BASE w2_imag = (BASE)sin (arg2);
      REAL(data,stride,j) = w1_real * z1_real - w1_imag * z1_imag;
      IMAG(data,stride,j) = w1_real * z1_imag + w1_imag * z1_real;
      REAL(data,stride,j) += w2_real * z2_real - w2_imag * z2_imag;
      IMAG(data,stride,j) += w2_real * z2_imag + w2_imag * z2_real;
    }

  /* fourier transform, fft[j] = z1 * delta{(j - k1),0} + z2 *
     delta{(j - k2,0)} */

  for (j = 0; j < n; j++)
    {
      REAL(fft,stride,j) = 0.0;
      IMAG(fft,stride,j) = 0.0;
    }

  {
    int freq1, freq2;

    if (k1 <= 0)
      {
        freq1 = (n - k1) % n;
      }
    else
      {
        freq1 = (k1 % n);
      };

    if (k2 <= 0)
      {
        freq2 = (n - k2) % n;
      }
    else
      {
        freq2 = (k2 % n);
      };

    REAL(fft,stride,freq1) += ((BASE) n) * z1_real;
    IMAG(fft,stride,freq1) += ((BASE) n) * z1_imag;
    REAL(fft,stride,freq2) += ((BASE) n) * z2_real;
    IMAG(fft,stride,freq2) += ((BASE) n) * z2_imag;
  }

  return 0;

}


int
FUNCTION(fft_signal,complex_noise) (const size_t n,
                                    const size_t stride,
                                    BASE data[],
                                    BASE fft[])
{
  size_t i;
  int status;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  for (i = 0; i < n; i++)
    {
      REAL(data,stride,i) = (BASE)urand();
      IMAG(data,stride,i) = (BASE)urand();
    }

  /* compute the dft */
  status = FUNCTION(gsl_dft_complex,forward) (data, stride, n, fft);

  return status;
}


int
FUNCTION(fft_signal,real_noise) (const size_t n,
                                 const size_t stride,
                                 BASE data[],
                                 BASE fft[])
{
  size_t i;
  int status;

  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  for (i = 0; i < n; i++)
    {
      REAL(data,stride,i) = (BASE)urand();
      IMAG(data,stride,i) = 0.0;
    }

  /* compute the dft */
  status = FUNCTION(gsl_dft_complex,forward) (data, stride, n, fft);

  return status;
}

