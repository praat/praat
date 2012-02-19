/* fft/hc_pass_2.c
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
FUNCTION(fft_halfcomplex,pass_2) (const BASE in[],
                                  const size_t istride,
                                  BASE out[],
                                  const size_t ostride,
                                  const size_t product,
                                  const size_t n,
                                  const TYPE(gsl_complex) twiddle[])
{
  size_t i, j, k, k1, jump;
  size_t factor, q, m, product_1;
  i = 0;
  j = 0;

  factor = 2;
  m = n / factor;
  q = n / product;
  product_1 = product / factor;
  jump = (factor - 1) * q;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const ATOMIC r0 = VECTOR(in,istride,2 * k1 * q);
      const ATOMIC r1 = VECTOR(in,istride,2 * k1 * q + 2 * q - 1);

      const ATOMIC s0 = r0 + r1;
      const ATOMIC s1 = r0 - r1;

      VECTOR(out,ostride,q * k1) = s0;
      VECTOR(out,ostride,q * k1 + m) = s1;
    }

  if (q == 1)
    return;

  for (k = 1; k < (q + 1) / 2; k++)
    {
      const ATOMIC w_real = GSL_REAL(twiddle[k - 1]);
      const ATOMIC w_imag = GSL_IMAG(twiddle[k - 1]);

      for (k1 = 0; k1 < product_1; k1++)
        {
          const size_t from0 = 2 * k1 * q + 2 * k - 1;
          const size_t from1 = 2 * k1 * q - 2 * k + 2 * q - 1;

          const ATOMIC z0_real = VECTOR(in,istride,from0);
          const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);

          const ATOMIC z1_real = VECTOR(in,istride,from1);
          const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

          /* compute x = W(2) z */

          /* x0 = z0 + z1 */
          const ATOMIC x0_real = z0_real + z1_real;
          const ATOMIC x0_imag = z0_imag - z1_imag;

          /* x1 = z0 - z1 */
          const ATOMIC x1_real = z0_real - z1_real;
          const ATOMIC x1_imag = z0_imag + z1_imag;

          const size_t to0 = k1 * q + 2 * k - 1;
          const size_t to1 = to0 + m;

          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;

          VECTOR(out,ostride,to1) = w_real * x1_real - w_imag * x1_imag;
          VECTOR(out,ostride,to1 + 1) = w_imag * x1_real + w_real * x1_imag;

        }
    }

  if (q % 2 == 1)
    return;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 2 * k1 * q + q - 1;
      const size_t to0 = k1 * q + q - 1;
      const size_t to1 = to0 + m;

      VECTOR(out,ostride,to0) = 2 * VECTOR(in,istride,from0);
      VECTOR(out,ostride,to1) = -2 * VECTOR(in,istride,from0 + 1);
    }
  return;
}
