/* fft/hc_pass_4.c
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
FUNCTION(fft_halfcomplex,pass_4) (const BASE in[],
                                  const size_t istride,
                                  BASE out[],
                                  const size_t ostride,
                                  const size_t product,
                                  const size_t n,
                                  const TYPE(gsl_complex) twiddle1[],
                                  const TYPE(gsl_complex) twiddle2[],
                                  const TYPE(gsl_complex) twiddle3[])
{
  size_t i, j, k, k1, jump;
  size_t factor, q, m, product_1;

  i = 0;
  j = 0;

  factor = 4;
  m = n / factor;
  q = n / product;
  product_1 = product / factor;
  jump = (factor - 1) * q;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 4 * k1 * q;
      const size_t from1 = from0 + 2 * q - 1;
      const size_t from2 = from1 + 2 * q;

      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);
      const ATOMIC z2_real = VECTOR(in,istride,from2);

      const ATOMIC t1_real = z0_real + z2_real;
      const ATOMIC t2_real = 2 * z1_real;
      const ATOMIC t3_real = z0_real - z2_real;
      const ATOMIC t4_imag = 2 * z1_imag;

      const size_t to0 = q * k1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;
      const size_t to3 = to2 + m;

      VECTOR(out,ostride,to0) = t1_real + t2_real;
      VECTOR(out,ostride,to1) = t3_real - t4_imag;
      VECTOR(out,ostride,to2) = t1_real - t2_real;
      VECTOR(out,ostride,to3) = t3_real + t4_imag;
    }

  if (q == 1)
    return;

  for (k = 1; k < (q + 1) / 2; k++)
    {
      const ATOMIC w1_real = GSL_REAL(twiddle1[k - 1]);
      const ATOMIC w1_imag = GSL_IMAG(twiddle1[k - 1]);
      const ATOMIC w2_real = GSL_REAL(twiddle2[k - 1]);
      const ATOMIC w2_imag = GSL_IMAG(twiddle2[k - 1]);
      const ATOMIC w3_real = GSL_REAL(twiddle3[k - 1]);
      const ATOMIC w3_imag = GSL_IMAG(twiddle3[k - 1]);

      for (k1 = 0; k1 < product_1; k1++)
        {
          const size_t from0 = 4 * k1 * q + 2 * k - 1;
          const size_t from1 = from0 + 2 * q;
          const size_t from2 = 4 * k1 * q - 2 * k + 2 * q - 1;
          const size_t from3 = from2 + 2 * q;

          const ATOMIC z0_real = VECTOR(in,istride,from0);
          const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);

          const ATOMIC z1_real = VECTOR(in,istride,from1);
          const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

          const ATOMIC z2_real = VECTOR(in,istride,from3);
          const ATOMIC z2_imag = -VECTOR(in,istride,from3 + 1);

          const ATOMIC z3_real = VECTOR(in,istride,from2);
          const ATOMIC z3_imag = -VECTOR(in,istride,from2 + 1);

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

          /* t4 = (z1 - z3) */
          const ATOMIC t4_real = (z1_real - z3_real);
          const ATOMIC t4_imag = (z1_imag - z3_imag);

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

          const size_t to0 = k1 * q + 2 * k - 1;
          const size_t to1 = to0 + m;
          const size_t to2 = to1 + m;
          const size_t to3 = to2 + m;

          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;

          VECTOR(out,ostride,to1) = w1_real * x1_real - w1_imag * x1_imag;
          VECTOR(out,ostride,to1 + 1) = w1_imag * x1_real + w1_real * x1_imag;

          VECTOR(out,ostride,to2) = w2_real * x2_real - w2_imag * x2_imag;
          VECTOR(out,ostride,to2 + 1) = w2_imag * x2_real + w2_real * x2_imag;

          /* to3 = w3 * x3 */
          VECTOR(out,ostride,to3) = w3_real * x3_real - w3_imag * x3_imag;
          VECTOR(out,ostride,to3 + 1) = w3_real * x3_imag + w3_imag * x3_real;

        }
    }

  if (q % 2 == 1)
    return;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 4 * k1 * q + q - 1;
      const size_t from1 = from0 + 2 * q;

      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);

      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

      const ATOMIC t1_real = sqrt (2.0) * (z0_imag + z1_imag);
      const ATOMIC t2_real = sqrt (2.0) * (z0_real - z1_real);

      const ATOMIC x0_real = 2 * (z0_real + z1_real);
      const ATOMIC x1_real = t2_real - t1_real;
      const ATOMIC x2_real = 2 * (z1_imag - z0_imag);
      const ATOMIC x3_real = -(t2_real + t1_real);

      const size_t to0 = k1 * q + q - 1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;
      const size_t to3 = to2 + m;

      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to2) = x2_real;
      VECTOR(out,ostride,to3) = x3_real;
    }
  return;
}
