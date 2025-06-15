/* fft/real_pass_3.c
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

static void
FUNCTION(fft_real,pass_3) (const BASE in[], 
                           const size_t istride,
                           BASE out[],
                           const size_t ostride,
                           const size_t product,
                           const size_t n,
                           const TYPE(gsl_complex) twiddle1[],
                           const TYPE(gsl_complex) twiddle2[])
{
  size_t k, k1;

  const size_t factor = 3;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t product_1 = product / factor;

  const ATOMIC tau = sqrt (3.0) / 2.0;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1;
      const size_t from1 = from0 + m;
      const size_t from2 = from1 + m;
      
      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z2_real = VECTOR(in,istride,from2);
      
      const ATOMIC t1 = z1_real + z2_real;

      const ATOMIC x0_real = z0_real + t1;
      const ATOMIC x1_real = z0_real - t1 / 2.0;
      const ATOMIC x1_imag = -tau * (z1_real - z2_real);

      const size_t to0 = product * k1;
      const size_t to1 = to0 + 2 * product_1 - 1;

      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to1 + 1) = x1_imag;
    }

  if (product_1 == 1)
    return;

  for (k = 1; k < (product_1 + 1) / 2; k++)
    {
      const ATOMIC w1_real = GSL_REAL(twiddle1[k - 1]);
      const ATOMIC w1_imag = -GSL_IMAG(twiddle1[k - 1]);
      const ATOMIC w2_real = GSL_REAL(twiddle2[k - 1]);
      const ATOMIC w2_imag = -GSL_IMAG(twiddle2[k - 1]);

      for (k1 = 0; k1 < q; k1++)
        {
          const size_t from0 = k1 * product_1 + 2 * k - 1;
          const size_t from1 = from0 + m;
          const size_t from2 = from1 + m;
          
          const ATOMIC f0_real = VECTOR(in,istride,from0);
          const ATOMIC f0_imag = VECTOR(in,istride,from0 + 1);
          const ATOMIC f1_real = VECTOR(in,istride,from1);
          const ATOMIC f1_imag = VECTOR(in,istride,from1 + 1);
          const ATOMIC f2_real = VECTOR(in,istride,from2);
          const ATOMIC f2_imag = VECTOR(in,istride,from2 + 1);
          
          const ATOMIC z0_real = f0_real;
          const ATOMIC z0_imag = f0_imag;
          const ATOMIC z1_real = w1_real * f1_real - w1_imag * f1_imag;
          const ATOMIC z1_imag = w1_real * f1_imag + w1_imag * f1_real;
          const ATOMIC z2_real = w2_real * f2_real - w2_imag * f2_imag;
          const ATOMIC z2_imag = w2_real * f2_imag + w2_imag * f2_real;

          /* compute x = W(3) z */

          /* t1 = z1 + z2 */
          const ATOMIC t1_real = z1_real + z2_real;
          const ATOMIC t1_imag = z1_imag + z2_imag;
          
          /* t2 = z0 - t1/2 */
          const ATOMIC t2_real = z0_real - t1_real / 2;
          const ATOMIC t2_imag = z0_imag - t1_imag / 2;
          
          /* t3 = (+/-) sin(pi/3)*(z1 - z2) */
          const ATOMIC t3_real = -tau * (z1_real - z2_real);
          const ATOMIC t3_imag = -tau * (z1_imag - z2_imag);
          
          /* x0 = z0 + t1 */
          const ATOMIC x0_real = z0_real + t1_real;
          const ATOMIC x0_imag = z0_imag + t1_imag;
          
          /* x1 = t2 + i t3 */
          const ATOMIC x1_real = t2_real - t3_imag;
          const ATOMIC x1_imag = t2_imag + t3_real;

            /* x2 = t2 - i t3 */
          const ATOMIC x2_real = t2_real + t3_imag;
          const ATOMIC x2_imag = t2_imag - t3_real;

          /* apply twiddle factors */
          
          const size_t to0 = k1 * product + 2 * k - 1;
          const size_t to1 = to0 + 2 * product_1;
          const size_t to2 = 2 * product_1 - 2 * k + k1 * product - 1;
          
          /* to0 = 1 * x0 */
          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;
          
          /* to1 = 1 * x1 */
          VECTOR(out,ostride,to1) = x1_real;
          VECTOR(out,ostride,to1 + 1) = x1_imag;
          
          /* to2 = 1 * x2 */
          VECTOR(out,ostride,to2) = x2_real;
          VECTOR(out,ostride,to2 + 1) = -x2_imag;
        }
    }
  
  if (product_1 % 2 == 1)
    return;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1 + product_1 - 1;
      const size_t from1 = from0 + m;
      const size_t from2 = from1 + m;
      
      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z2_real = VECTOR(in,istride,from2);

      const ATOMIC t1 = z1_real - z2_real;
      const ATOMIC x0_real = z0_real + t1 / 2.0;
      const ATOMIC x0_imag = -tau * (z1_real + z2_real);
      const ATOMIC x1_real = z0_real - t1;

      const size_t to0 = k1 * product + product_1 - 1;
      const size_t to1 = to0 + 2 * product_1;
      
      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to0 + 1) = x0_imag;
      VECTOR(out,ostride,to1) = x1_real;
    }

  return;
}
