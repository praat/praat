/* fft/hc_pass_3.c
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
FUNCTION(fft_halfcomplex,pass_3) (const BASE in[],
                                  const size_t istride,
                                  BASE out[],
                                  const size_t ostride,
                                  const size_t product,
                                  const size_t n,
                                  const TYPE(gsl_complex) twiddle1[],
                                  const TYPE(gsl_complex) twiddle2[])
{
  size_t i, j, k, k1, jump;
  size_t factor, q, m, product_1;

  ATOMIC tau = sqrt (3.0) / 2.0;

  i = 0;
  j = 0;

  factor = 3;
  m = n / factor;
  q = n / product;
  product_1 = product / factor;
  jump = (factor - 1) * q;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 3 * k1 * q;
      const size_t from1 = from0 + 2 * q - 1;

      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

      const ATOMIC t1_real = 2 * z1_real;
      const ATOMIC t2_real = z0_real - z1_real;
      const ATOMIC t3_imag = 2 * tau * z1_imag;

      const size_t to0 = q * k1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;

      VECTOR(out,ostride,to0) = z0_real + t1_real;
      VECTOR(out,ostride,to1) = t2_real - t3_imag;
      VECTOR(out,ostride,to2) = t2_real + t3_imag;

    }

  if (q == 1)
    return;

  for (k = 1; k < (q + 1) / 2; k++)
    {
      const ATOMIC w1_real = GSL_REAL(twiddle1[k - 1]);
      const ATOMIC w1_imag = GSL_IMAG(twiddle1[k - 1]);
      const ATOMIC w2_real = GSL_REAL(twiddle2[k - 1]);
      const ATOMIC w2_imag = GSL_IMAG(twiddle2[k - 1]);

      for (k1 = 0; k1 < product_1; k1++)
        {
          const size_t from0 = 3 * k1 * q + 2 * k - 1;
          const size_t from1 = from0 + 2 * q;
          const size_t from2 = 3 * k1 * q - 2 * k + 2 * q - 1;

          const ATOMIC z0_real = VECTOR(in,istride,from0);
          const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);

          const ATOMIC z1_real = VECTOR(in,istride,from1);
          const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

          const ATOMIC z2_real = VECTOR(in,istride,from2);
          const ATOMIC z2_imag = -VECTOR(in,istride,from2 + 1);

          /* compute x = W(3) z */

          /* t1 = z1 + z2 */
          const ATOMIC t1_real = z1_real + z2_real;
          const ATOMIC t1_imag = z1_imag + z2_imag;

          /* t2 = z0 - t1/2 */
          const ATOMIC t2_real = z0_real - t1_real / 2.0;
          const ATOMIC t2_imag = z0_imag - t1_imag / 2.0;

          /* t3 = sin(pi/3)*(z1 - z2) */
          const ATOMIC t3_real = tau * (z1_real - z2_real);
          const ATOMIC t3_imag = tau * (z1_imag - z2_imag);

          /* x0 = z0 + t1 */
          const ATOMIC x0_real = z0_real + t1_real;
          const ATOMIC x0_imag = z0_imag + t1_imag;

          /* x1 = t2 + i t3 */
          const ATOMIC x1_real = t2_real - t3_imag;
          const ATOMIC x1_imag = t2_imag + t3_real;

          /* x2 = t2 - i t3 */
          const ATOMIC x2_real = t2_real + t3_imag;
          const ATOMIC x2_imag = t2_imag - t3_real;

          const size_t to0 = k1 * q + 2 * k - 1;
          const size_t to1 = to0 + m;
          const size_t to2 = to1 + m;

          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;

          VECTOR(out,ostride,to1) = w1_real * x1_real - w1_imag * x1_imag;
          VECTOR(out,ostride,to1 + 1) = w1_imag * x1_real + w1_real * x1_imag;

          VECTOR(out,ostride,to2) = w2_real * x2_real - w2_imag * x2_imag;
          VECTOR(out,ostride,to2 + 1) = w2_imag * x2_real + w2_real * x2_imag;

        }
    }

  if (q % 2 == 1)
    return;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 3 * k1 * q + q - 1;
      const size_t from1 = from0 + 2 * q;

      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);
      const ATOMIC z1_real = VECTOR(in,istride,from1);

      const ATOMIC t1_real = z0_real - z1_real;
      const ATOMIC t2_real = 2 * tau * z0_imag;

      const ATOMIC x0_real = 2 * z0_real + z1_real;
      const ATOMIC x1_real = t1_real - t2_real;
      const ATOMIC x2_real = -t1_real - t2_real;

      const size_t to0 = k1 * q + q - 1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;

      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to2) = x2_real;
    }
  return;
}
