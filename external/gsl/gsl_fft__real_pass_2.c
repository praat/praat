/* fft/real_pass_2.c
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
FUNCTION(fft_real,pass_2) (const BASE in[],
                           const size_t istride,
                           BASE out[],
                           const size_t ostride,
                           const size_t product,
                           const size_t n,
                           const TYPE(gsl_complex) twiddle[])
{
  size_t k, k1;

  const size_t factor = 2;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t product_1 = product / factor;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1;
      const size_t from1 = from0 + m;

      const ATOMIC r0 = VECTOR(in,istride,from0);
      const ATOMIC r1 = VECTOR(in,istride,from1);
      
      const ATOMIC s0 = r0 + r1;
      const ATOMIC s1 = r0 - r1;
      
      const size_t to0 = product * k1;
      const size_t to1 = to0 + product - 1;
      
      VECTOR(out,ostride,to0) = s0;
      VECTOR(out,ostride,to1) = s1;
    }

  if (product_1 == 1)
    return;

  for (k = 1; k < (product_1 + 1) / 2; k++)
    {

      /* forward real transform: w -> conjugate(w) */
      const ATOMIC w_real = GSL_REAL(twiddle[k - 1]);
      const ATOMIC w_imag = -GSL_IMAG(twiddle[k - 1]);

      for (k1 = 0; k1 < q; k1++)
        {
          const size_t from0 = k1 * product_1 + 2 * k - 1;
          const size_t from1 = from0 + m;

          const ATOMIC f0_real = VECTOR(in,istride,from0);
          const ATOMIC f0_imag = VECTOR(in,istride,from0 + 1);

          const ATOMIC f1_real = VECTOR(in,istride,from1);
          const ATOMIC f1_imag = VECTOR(in,istride,from1 + 1);

          const ATOMIC z0_real = f0_real;
          const ATOMIC z0_imag = f0_imag;

          const ATOMIC z1_real = w_real * f1_real - w_imag * f1_imag;
          const ATOMIC z1_imag = w_real * f1_imag + w_imag * f1_real;

          /* compute x = W(2) z */

          /* x0 = z0 + z1 */
          const ATOMIC x0_real = z0_real + z1_real;
          const ATOMIC x0_imag = z0_imag + z1_imag;

          /* x1 = z0 - z1 */
          const ATOMIC x1_real = z0_real - z1_real;
          const ATOMIC x1_imag = z0_imag - z1_imag;

          const size_t to0 = k1 * product + 2 * k - 1;
          const size_t to1 = k1 * product + product - 2 * k - 1;
          
          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;
          
          /* stored in conjugate location */
          VECTOR(out,ostride,to1) = x1_real;
          VECTOR(out,ostride,to1 + 1) = -x1_imag;
        }
    }
  
  if (product_1 % 2 == 1)
    return;

  for (k1 = 0; k1 < q; k1++)
    {
      const size_t from0 = k1 * product_1 + product_1 - 1;
      const size_t from1 = from0 + m;
      const size_t to0 = k1 * product + product_1 - 1;

      VECTOR(out,ostride,to0) = VECTOR(in,istride,from0);
      VECTOR(out,ostride,to0 + 1) = -VECTOR(in,istride,from1);
    }
  return;
}
