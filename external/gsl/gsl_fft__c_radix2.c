/* fft/c_radix2.c
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
FUNCTION(gsl_fft_complex,radix2_forward) (TYPE(gsl_complex_packed_array) data,
                                          const size_t stride, const size_t n)
{
  gsl_fft_direction sign = gsl_fft_forward;
  int status = FUNCTION(gsl_fft_complex,radix2_transform) (data, stride, n, sign);
  return status;
}

int
FUNCTION(gsl_fft_complex,radix2_backward) (TYPE(gsl_complex_packed_array) data,
                                           const size_t stride, const size_t n)
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_fft_complex,radix2_transform) (data, stride, n, sign);
  return status;
}

int
FUNCTION(gsl_fft_complex,radix2_inverse) (TYPE(gsl_complex_packed_array) data,
                                          const size_t stride, const size_t n)
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_fft_complex,radix2_transform) (data, stride, n, sign);

  if (status)
    {
      return status;
    }

  /* normalize inverse fft with 1/n */

  {
    const ATOMIC norm = 1.0 / n;
    size_t i;
    for (i = 0; i < n; i++)
      {
        REAL(data,stride,i) *= norm;
        IMAG(data,stride,i) *= norm;
      }
  }

  return status;
}



int
FUNCTION(gsl_fft_complex,radix2_transform) (TYPE(gsl_complex_packed_array) data,
                                            const size_t stride, 
                                            const size_t n,
                                            const gsl_fft_direction sign)
{
  int result ;
  size_t dual;
  size_t bit; 
  size_t logn = 0;

  if (n == 1) /* identity operation */
    {
      return 0 ;
    }

  /* make sure that n is a power of 2 */

  result = fft_binary_logn(n) ;

  if (result == -1) 
    {
      GSL_ERROR ("n is not a power of 2", GSL_EINVAL);
    } 
  else 
    {
      logn = result ;
    }

  /* bit reverse the ordering of input data for decimation in time algorithm */
  
  (void) FUNCTION(fft_complex,bitreverse_order) (data, stride, n, logn) ;

  /* apply fft recursion */

  dual = 1;

  for (bit = 0; bit < logn; bit++)
    {
      ATOMIC w_real = 1.0;
      ATOMIC w_imag = 0.0;

      const double theta = 2.0 * ((int) sign) * M_PI / (2.0 * (double) dual);

      const ATOMIC s = sin (theta);
      const ATOMIC t = sin (theta / 2.0);
      const ATOMIC s2 = 2.0 * t * t;

      size_t a, b;

      /* a = 0 */

      for (b = 0; b < n; b += 2 * dual)
        {
          const size_t i = b ;
          const size_t j = b + dual;
          
          const ATOMIC z1_real = REAL(data,stride,j) ;
          const ATOMIC z1_imag = IMAG(data,stride,j) ;

          const ATOMIC wd_real = z1_real ;
          const ATOMIC wd_imag = z1_imag ;
          
          REAL(data,stride,j) = REAL(data,stride,i) - wd_real;
          IMAG(data,stride,j) = IMAG(data,stride,i) - wd_imag;
          REAL(data,stride,i) += wd_real;
          IMAG(data,stride,i) += wd_imag;
        }
      
      /* a = 1 .. (dual-1) */

      for (a = 1; a < dual; a++)
        {

          /* trignometric recurrence for w-> exp(i theta) w */

          {
            const ATOMIC tmp_real = w_real - s * w_imag - s2 * w_real;
            const ATOMIC tmp_imag = w_imag + s * w_real - s2 * w_imag;
            w_real = tmp_real;
            w_imag = tmp_imag;
          }

          for (b = 0; b < n; b += 2 * dual)
            {
              const size_t i = b + a;
              const size_t j = b + a + dual;

              const ATOMIC z1_real = REAL(data,stride,j) ;
              const ATOMIC z1_imag = IMAG(data,stride,j) ;
              
              const ATOMIC wd_real = w_real * z1_real - w_imag * z1_imag;
              const ATOMIC wd_imag = w_real * z1_imag + w_imag * z1_real;

              REAL(data,stride,j) = REAL(data,stride,i) - wd_real;
              IMAG(data,stride,j) = IMAG(data,stride,i) - wd_imag;
              REAL(data,stride,i) += wd_real;
              IMAG(data,stride,i) += wd_imag;
            }
        }
      dual *= 2;
    }

  return 0;

}


int
FUNCTION(gsl_fft_complex,radix2_dif_forward) (TYPE(gsl_complex_packed_array) data, 
                                              const size_t stride, 
                                              const size_t n)
{
  gsl_fft_direction sign = gsl_fft_forward;
  int status = FUNCTION(gsl_fft_complex,radix2_dif_transform) (data, stride, n, sign);
  return status;
}

int
FUNCTION(gsl_fft_complex,radix2_dif_backward) (TYPE(gsl_complex_packed_array) data,
                                               const size_t stride, 
                                               const size_t n)
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_fft_complex,radix2_dif_transform) (data, stride, n, sign);
  return status;
}

int
FUNCTION(gsl_fft_complex,radix2_dif_inverse) (TYPE(gsl_complex_packed_array) data, 
                                              const size_t stride, 
                                              const size_t n)
{
  gsl_fft_direction sign = gsl_fft_backward;
  int status = FUNCTION(gsl_fft_complex,radix2_dif_transform) (data, stride, n, sign);

  if (status)
    {
      return status;
    }

  /* normalize inverse fft with 1/n */

  {
    const ATOMIC norm = 1.0 / n;
    size_t i;
    for (i = 0; i < n; i++)
      {
        REAL(data,stride,i) *= norm;
        IMAG(data,stride,i) *= norm;
      }
  }

  return status;
}

int
FUNCTION(gsl_fft_complex,radix2_dif_transform) (TYPE(gsl_complex_packed_array) data, 
                                      const size_t stride, 
                                      const size_t n,
                                      const gsl_fft_direction sign)
{
  int result ;
  size_t dual;
  size_t bit; 
  size_t logn = 0;

  if (n == 1) /* identity operation */
    {
      return 0 ;
    }

  /* make sure that n is a power of 2 */

  result = fft_binary_logn(n) ;

  if (result == -1) 
    {
      GSL_ERROR ("n is not a power of 2", GSL_EINVAL);
    } 
  else 
    {
      logn = result ;
    }

  /* apply fft recursion */

  dual = n / 2;

  for (bit = 0; bit < logn; bit++)
    {
      ATOMIC w_real = 1.0;
      ATOMIC w_imag = 0.0;

      const double theta = 2.0 * ((int) sign) * M_PI / ((double) (2 * dual));

      const ATOMIC s = sin (theta);
      const ATOMIC t = sin (theta / 2.0);
      const ATOMIC s2 = 2.0 * t * t;

      size_t a, b;

      for (b = 0; b < dual; b++)
        {
          for (a = 0; a < n; a+= 2 * dual)
            {
              const size_t i = b + a;
              const size_t j = b + a + dual;
              
              const ATOMIC t1_real = REAL(data,stride,i) + REAL(data,stride,j);
              const ATOMIC t1_imag = IMAG(data,stride,i) + IMAG(data,stride,j);
              const ATOMIC t2_real = REAL(data,stride,i) - REAL(data,stride,j);
              const ATOMIC t2_imag = IMAG(data,stride,i) - IMAG(data,stride,j);

              REAL(data,stride,i) = t1_real;
              IMAG(data,stride,i) = t1_imag;
              REAL(data,stride,j) = w_real*t2_real - w_imag * t2_imag;
              IMAG(data,stride,j) = w_real*t2_imag + w_imag * t2_real;
            }

          /* trignometric recurrence for w-> exp(i theta) w */

          {
            const ATOMIC tmp_real = w_real - s * w_imag - s2 * w_real;
            const ATOMIC tmp_imag = w_imag + s * w_real - s2 * w_imag;
            w_real = tmp_real;
            w_imag = tmp_imag;
          }
        }
      dual /= 2;
    }

  /* bit reverse the ordering of output data for decimation in
     frequency algorithm */
  
  (void) FUNCTION(fft_complex,bitreverse_order)(data, stride, n, logn) ;

  return 0;

}








