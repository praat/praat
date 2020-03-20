/* fft/hc_radix2.c
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
FUNCTION(gsl_fft_halfcomplex,radix2_backward) (BASE data[],
                                               const size_t stride,
                                               const size_t n)
{
  int status = FUNCTION(gsl_fft_halfcomplex,radix2_transform) (data, stride, n) ;
  return status ;
}

int
FUNCTION(gsl_fft_halfcomplex,radix2_inverse) (BASE data[],
                                              const size_t stride,
                                              const size_t n)
{
  int status = FUNCTION(gsl_fft_halfcomplex,radix2_transform) (data, stride, n);

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
        data[stride*i] *= norm;
      }
  }
  return status;
}

int
FUNCTION(gsl_fft_halfcomplex,radix2_transform) (BASE data[],
                                                const size_t stride,
                                                const size_t n)
{
  int result ;
  size_t p, p_1, q;
  size_t i; 
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

  p = n; q = 1 ; p_1 = n/2 ;

  for (i = 1; i <= logn; i++)
    {
      size_t a, b;

      /* a = 0 */

      for (b = 0; b < q; b++)
        {
          const ATOMIC z0 = VECTOR(data,stride,b*p);
          const ATOMIC z1 = VECTOR(data,stride,b*p + p_1);
          
          const ATOMIC t0_real = z0 + z1 ;
          const ATOMIC t1_real = z0 - z1 ;
          
          VECTOR(data,stride,b*p) = t0_real;
          VECTOR(data,stride,b*p + p_1) = t1_real ;
        }

      /* a = 1 ... p_{i-1}/2 - 1 */

      {
        ATOMIC w_real = 1.0;
        ATOMIC w_imag = 0.0;

        const ATOMIC theta = 2.0 * M_PI / p;
        
        const ATOMIC s = sin (theta);
        const ATOMIC t = sin (theta / 2.0);
        const ATOMIC s2 = 2.0 * t * t;
        
        for (a = 1; a < (p_1)/2; a++)
          {
            /* trignometric recurrence for w-> exp(i theta) w */
            
            {
              const ATOMIC tmp_real = w_real - s * w_imag - s2 * w_real;
              const ATOMIC tmp_imag = w_imag + s * w_real - s2 * w_imag;
              w_real = tmp_real;
              w_imag = tmp_imag;
            }
            
            for (b = 0; b < q; b++)
              {
                ATOMIC z0_real = VECTOR(data,stride,b*p + a) ;
                ATOMIC z0_imag = VECTOR(data,stride,b*p + p - a) ;
                ATOMIC z1_real = VECTOR(data,stride,b*p + p_1 - a) ;
                ATOMIC z1_imag = -VECTOR(data,stride,b*p + p_1 + a) ;
                
                /* t0 = z0 + z1 */
                
                ATOMIC t0_real = z0_real + z1_real;
                ATOMIC t0_imag = z0_imag + z1_imag;
                
                /* t1 = (z0 - z1) */
                
                ATOMIC t1_real = z0_real -  z1_real;
                ATOMIC t1_imag = z0_imag -  z1_imag;
                
                VECTOR(data,stride,b*p + a) = t0_real ;
                VECTOR(data,stride,b*p + p_1 - a) = t0_imag ;
                
                VECTOR(data,stride,b*p + p_1 + a) = (w_real * t1_real - w_imag * t1_imag) ;
                VECTOR(data,stride,b*p + p - a) = (w_real * t1_imag + w_imag * t1_real) ;
              }
          }
      }

      if (p_1 >  1) {
        for (b = 0; b < q; b++) {
          VECTOR(data,stride,b*p + p_1/2) *= 2 ;
          VECTOR(data,stride,b*p + p_1 + p_1/2) *= -2 ;
        }
      }

      p_1 = p_1 / 2 ;
      p = p / 2 ;
      q = q * 2 ;
    }

  /* bit reverse the ordering of output data for decimation in
     frequency algorithm */
  
 (void) FUNCTION(fft_real,bitreverse_order)(data, stride, n, logn) ;

  return 0;

}
