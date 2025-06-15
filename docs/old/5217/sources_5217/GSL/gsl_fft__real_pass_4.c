/* fft/real_pass_4.c
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
FUNCTION(fft_real,pass_4) (const BASE in[],
                           const size_t istride,
                           BASE out[],
                           const size_t ostride,
                           const size_t product,
                           const size_t n,
                           const TYPE(gsl_complex) twiddle1[],
                           const TYPE(gsl_complex) twiddle2[],
                           const TYPE(gsl_complex) twiddle3[])
{
  size_t k, k1;

  const size_t factor = 4;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t product_1 = product / factor;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1;
      const size_t from1 = from0 + m;
      const size_t from2 = from1 + m;
      const size_t from3 = from2 + m;
      
      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z2_real = VECTOR(in,istride,from2);
      const ATOMIC z3_real = VECTOR(in,istride,from3);

      /* compute x = W(4) z */

      /* t1 = z0 + z2 */
      const ATOMIC t1_real = z0_real + z2_real;
      
      /* t2 = z1 + z3 */
      const ATOMIC t2_real = z1_real + z3_real;
      
        /* t3 = z0 - z2 */
      const ATOMIC t3_real = z0_real - z2_real;
      
      /* t4 = - (z1 - z3) */
      const ATOMIC t4_real = -(z1_real - z3_real);
      
      /* x0 = t1 + t2 */
      const ATOMIC x0_real = t1_real + t2_real;

      /* x1 = t3 + i t4 */
      const ATOMIC x1_real = t3_real;
      const ATOMIC x1_imag = t4_real;

      /* x2 = t1 - t2 */
      const ATOMIC x2_real = t1_real - t2_real;

      const size_t to0 = product * k1;
      const size_t to1 = to0 + 2 * product_1 - 1;
      const size_t to2 = to1 + 2 * product_1;
      
      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to1 + 1) = x1_imag;
      VECTOR(out,ostride,to2) = x2_real;
    }

  if (product_1 == 1)
    return;

  for (k = 1; k < (product_1 + 1) / 2; k++)
    {
      ATOMIC w1_real, w1_imag, w2_real, w2_imag, w3_real, w3_imag;
      w1_real = GSL_REAL(twiddle1[k - 1]);
      w1_imag = -GSL_IMAG(twiddle1[k - 1]);
      w2_real = GSL_REAL(twiddle2[k - 1]);
      w2_imag = -GSL_IMAG(twiddle2[k - 1]);
      w3_real = GSL_REAL(twiddle3[k - 1]);
      w3_imag = -GSL_IMAG(twiddle3[k - 1]);

      for (k1 = 0; k1 < q; k1++)
        {
          const size_t from0 = k1 * product_1 + 2 * k - 1;
          const size_t from1 = from0 + m;
          const size_t from2 = from1 + m;
          const size_t from3 = from2 + m;
          
          const ATOMIC f0_real = VECTOR(in,istride,from0);
          const ATOMIC f0_imag = VECTOR(in,istride,from0 + 1);
          const ATOMIC f1_real = VECTOR(in,istride,from1);
          const ATOMIC f1_imag = VECTOR(in,istride,from1 + 1);
          const ATOMIC f2_real = VECTOR(in,istride,from2);
          const ATOMIC f2_imag = VECTOR(in,istride,from2 + 1);
          const ATOMIC f3_real = VECTOR(in,istride,from3);
          const ATOMIC f3_imag = VECTOR(in,istride,from3 + 1);
          
          const ATOMIC z0_real = f0_real;
          const ATOMIC z0_imag = f0_imag;
          const ATOMIC z1_real = w1_real * f1_real - w1_imag * f1_imag;
          const ATOMIC z1_imag = w1_real * f1_imag + w1_imag * f1_real;
          const ATOMIC z2_real = w2_real * f2_real - w2_imag * f2_imag;
          const ATOMIC z2_imag = w2_real * f2_imag + w2_imag * f2_real;
          const ATOMIC z3_real = w3_real * f3_real - w3_imag * f3_imag;
          const ATOMIC z3_imag = w3_real * f3_imag + w3_imag * f3_real;

          /* compute x = W(4) z */

          /* t1 = z0 + z2 */
          const ATOMIC t1_real = z0_real + z2_real;
          const ATOMIC t1_imag = z0_imag + z2_imag;
          
          /* t2 = z1 + z3 */
          const ATOMIC t2_real = z1_real + z3_real;
          const ATOMIC t2_imag = z1_imag + z3_imag;
          
          /* t3 = z0 - z2 */
          const ATOMIC t3_real = z0_real - z2_real;
          const ATOMIC t3_imag = z0_imag - z2_imag;
          
          /* t4 = - (z1 - z3) */
          const ATOMIC t4_real = -(z1_real - z3_real);
          const ATOMIC t4_imag = -(z1_imag - z3_imag);
          
          /* x0 = t1 + t2 */
          const ATOMIC x0_real = t1_real + t2_real;
          const ATOMIC x0_imag = t1_imag + t2_imag;
          
          /* x1 = t3 + i t4 */
          const ATOMIC x1_real = t3_real - t4_imag;
          const ATOMIC x1_imag = t3_imag + t4_real;
          
          /* x2 = t1 - t2 */
          const ATOMIC x2_real = t1_real - t2_real;
          const ATOMIC x2_imag = t1_imag - t2_imag;
          
          /* x3 = t3 - i t4 */
          const ATOMIC x3_real = t3_real + t4_imag;
          const ATOMIC x3_imag = t3_imag - t4_real;

          const size_t to0 = k1 * product + 2 * k - 1;
          const size_t to1 = to0 + 2 * product_1;
          const size_t to2 = 2 * product_1 - 2 * k + k1 * product - 1;
          const size_t to3 = to2 + 2 * product_1;
          
          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;
          
          VECTOR(out,ostride,to1) = x1_real;
          VECTOR(out,ostride,to1 + 1) = x1_imag;
          
          VECTOR(out,ostride,to3) = x2_real;
          VECTOR(out,ostride,to3 + 1) = -x2_imag;
          
          VECTOR(out,ostride,to2) = x3_real;
          VECTOR(out,ostride,to2 + 1) = -x3_imag;
        }
    }

  if (product_1 % 2 == 1)
    return;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1 + product_1 - 1;
      const size_t from1 = from0 + m;
      const size_t from2 = from1 + m;
      const size_t from3 = from2 + m;
      
      const ATOMIC x0 = VECTOR(in,istride,from0);
      const ATOMIC x1 = VECTOR(in,istride,from1);
      const ATOMIC x2 = VECTOR(in,istride,from2);
      const ATOMIC x3 = VECTOR(in,istride,from3);
      
      const ATOMIC t1 = (1.0 / sqrt (2.0)) * (x1 - x3);
      const ATOMIC t2 = (1.0 / sqrt (2.0)) * (x1 + x3);
      
      const size_t to0 = k1 * product + 2 * k - 1;
      const size_t to1 = to0 + 2 * product_1;
      
      VECTOR(out,ostride,to0) = x0 + t1;
      VECTOR(out,ostride,to0 + 1) = -x2 - t2;
      
      VECTOR(out,ostride,to1) = x0 - t1;
      VECTOR(out,ostride,to1 + 1) = x2 - t2;
    }
  return;
}
