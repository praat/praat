/* fft/hc_pass_5.c
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
FUNCTION(fft_halfcomplex,pass_5) (const BASE in[],
                                  const size_t istride,
                                  BASE out[],
                                  const size_t ostride,
                                  const size_t product,
                                  const size_t n,
                                  const TYPE(gsl_complex) twiddle1[],
                                  const TYPE(gsl_complex) twiddle2[],
                                  const TYPE(gsl_complex) twiddle3[],
                                  const TYPE(gsl_complex) twiddle4[])
{

  size_t k, k1;
  size_t factor, q, m, product_1;

  const ATOMIC sina = sin (2.0 * M_PI / 5.0);
  const ATOMIC sinb = sin (2.0 * M_PI / 10.0);

  factor = 5;
  m = n / factor;
  q = n / product;
  product_1 = product / factor;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 5 * k1 * q;
      const size_t from1 = from0 + 2 * q - 1;
      const size_t from2 = from1 + 2 * q;

      const ATOMIC z0_real = VECTOR(in,istride,from0);
      const ATOMIC z1_real = VECTOR(in,istride,from1);
      const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);
      const ATOMIC z2_real = VECTOR(in,istride,from2);
      const ATOMIC z2_imag = VECTOR(in,istride,from2 + 1);

      const ATOMIC t1_real = 2 * (z1_real + z2_real);
      const ATOMIC t2_real = 2 * (sqrt (5.0) / 4.0) * (z1_real - z2_real);
      const ATOMIC t3_real = z0_real - t1_real / 4.0;
      const ATOMIC t4_real = t2_real + t3_real;
      const ATOMIC t5_real = -t2_real + t3_real;
      const ATOMIC t6_imag = 2 * (sina * z1_imag + sinb * z2_imag);
      const ATOMIC t7_imag = 2 * (sinb * z1_imag - sina * z2_imag);

      const ATOMIC x0_real = z0_real + t1_real;
      const ATOMIC x1_real = t4_real - t6_imag;
      const ATOMIC x2_real = t5_real - t7_imag;
      const ATOMIC x3_real = t5_real + t7_imag;
      const ATOMIC x4_real = t4_real + t6_imag;

      const size_t to0 = q * k1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;
      const size_t to3 = to2 + m;
      const size_t to4 = to3 + m;

      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to2) = x2_real;
      VECTOR(out,ostride,to3) = x3_real;
      VECTOR(out,ostride,to4) = x4_real;
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
      const ATOMIC w4_real = GSL_REAL(twiddle4[k - 1]);
      const ATOMIC w4_imag = GSL_IMAG(twiddle4[k - 1]);

      for (k1 = 0; k1 < product_1; k1++)
        {
          const size_t from0 = 5 * k1 * q + 2 * k - 1;
          const size_t from1 = from0 + 2 * q;
          const size_t from2 = from1 + 2 * q;
          const size_t from3 = 5 * k1 * q - 2 * k + 2 * q - 1;
          const size_t from4 = from3 + 2 * q;

          const ATOMIC z0_real = VECTOR(in,istride,from0);
          const ATOMIC z0_imag = VECTOR(in,istride,from0 + 1);

          const ATOMIC z1_real = VECTOR(in,istride,from1);
          const ATOMIC z1_imag = VECTOR(in,istride,from1 + 1);

          const ATOMIC z2_real = VECTOR(in,istride,from2);
          const ATOMIC z2_imag = VECTOR(in,istride,from2 + 1);

          const ATOMIC z3_real = VECTOR(in,istride,from4);
          const ATOMIC z3_imag = -VECTOR(in,istride,from4 + 1);

          const ATOMIC z4_real = VECTOR(in,istride,from3);
          const ATOMIC z4_imag = -VECTOR(in,istride,from3 + 1);

          /* compute x = W(5) z */

          /* t1 = z1 + z4 */
          const ATOMIC t1_real = z1_real + z4_real;
          const ATOMIC t1_imag = z1_imag + z4_imag;

          /* t2 = z2 + z3 */
          const ATOMIC t2_real = z2_real + z3_real;
          const ATOMIC t2_imag = z2_imag + z3_imag;

          /* t3 = z1 - z4 */
          const ATOMIC t3_real = z1_real - z4_real;
          const ATOMIC t3_imag = z1_imag - z4_imag;

          /* t4 = z2 - z3 */
          const ATOMIC t4_real = z2_real - z3_real;
          const ATOMIC t4_imag = z2_imag - z3_imag;

          /* t5 = t1 + t2 */
          const ATOMIC t5_real = t1_real + t2_real;
          const ATOMIC t5_imag = t1_imag + t2_imag;

          /* t6 = (sqrt(5)/4)(t1 - t2) */
          const ATOMIC t6_real = (sqrt (5.0) / 4.0) * (t1_real - t2_real);
          const ATOMIC t6_imag = (sqrt (5.0) / 4.0) * (t1_imag - t2_imag);

          /* t7 = z0 - ((t5)/4) */
          const ATOMIC t7_real = z0_real - t5_real / 4.0;
          const ATOMIC t7_imag = z0_imag - t5_imag / 4.0;

          /* t8 = t7 + t6 */
          const ATOMIC t8_real = t7_real + t6_real;
          const ATOMIC t8_imag = t7_imag + t6_imag;

          /* t9 = t7 - t6 */
          const ATOMIC t9_real = t7_real - t6_real;
          const ATOMIC t9_imag = t7_imag - t6_imag;

          /* t10 = sin(2 pi/5) t3 + sin(2 pi/10) t4 */
          const ATOMIC t10_real = sina * t3_real + sinb * t4_real;
          const ATOMIC t10_imag = sina * t3_imag + sinb * t4_imag;

          /* t11 = sin(2 pi/10) t3 - sin(2 pi/5) t4 */
          const ATOMIC t11_real = sinb * t3_real - sina * t4_real;
          const ATOMIC t11_imag = sinb * t3_imag - sina * t4_imag;

          /* x0 = z0 + t5 */
          const ATOMIC x0_real = z0_real + t5_real;
          const ATOMIC x0_imag = z0_imag + t5_imag;

          /* x1 = t8 + i t10 */
          const ATOMIC x1_real = t8_real - t10_imag;
          const ATOMIC x1_imag = t8_imag + t10_real;

          /* x2 = t9 + i t11 */
          const ATOMIC x2_real = t9_real - t11_imag;
          const ATOMIC x2_imag = t9_imag + t11_real;

          /* x3 = t9 - i t11 */
          const ATOMIC x3_real = t9_real + t11_imag;
          const ATOMIC x3_imag = t9_imag - t11_real;

          /* x4 = t8 - i t10 */
          const ATOMIC x4_real = t8_real + t10_imag;
          const ATOMIC x4_imag = t8_imag - t10_real;

          const size_t to0 = k1 * q + 2 * k - 1;
          const size_t to1 = to0 + m;
          const size_t to2 = to1 + m;
          const size_t to3 = to2 + m;
          const size_t to4 = to3 + m;

          /* apply twiddle factors */

          /* to0 = 1 * x0 */
          VECTOR(out,ostride,to0) = x0_real;
          VECTOR(out,ostride,to0 + 1) = x0_imag;

          /* to1 = w1 * x1 */
          VECTOR(out,ostride,to1) = w1_real * x1_real - w1_imag * x1_imag;
          VECTOR(out,ostride,to1 + 1) = w1_real * x1_imag + w1_imag * x1_real;

          /* to2 = w2 * x2 */
          VECTOR(out,ostride,to2) = w2_real * x2_real - w2_imag * x2_imag;
          VECTOR(out,ostride,to2 + 1) = w2_real * x2_imag + w2_imag * x2_real;

          /* to3 = w3 * x3 */
          VECTOR(out,ostride,to3) = w3_real * x3_real - w3_imag * x3_imag;
          VECTOR(out,ostride,to3 + 1) = w3_real * x3_imag + w3_imag * x3_real;

          /* to4 = w4 * x4 */
          VECTOR(out,ostride,to4) = w4_real * x4_real - w4_imag * x4_imag;
          VECTOR(out,ostride,to4 + 1) = w4_real * x4_imag + w4_imag * x4_real;
        }
    }

  if (q % 2 == 1)
    return;

  for (k1 = 0; k1 < product_1; k1++)
    {
      const size_t from0 = 5 * k1 * q + q - 1;
      const size_t from1 = from0 + 2 * q;
      const size_t from2 = from1 + 2 * q;

      const ATOMIC z0_real = 2 * VECTOR(in,istride,from0);
      const ATOMIC z0_imag = 2 * VECTOR(in,istride,from0 + 1);

      const ATOMIC z1_real = 2 * VECTOR(in,istride,from1);
      const ATOMIC z1_imag = 2 * VECTOR(in,istride,from1 + 1);

      const ATOMIC z2_real = VECTOR(in,istride,from2);

      const ATOMIC t1_real = z0_real + z1_real;
      const ATOMIC t2_real = (t1_real / 4.0) - z2_real;
      const ATOMIC t3_real = (sqrt (5.0) / 4.0) * (z0_real - z1_real);
      const ATOMIC t4_real = sinb * z0_imag + sina * z1_imag;
      const ATOMIC t5_real = sina * z0_imag - sinb * z1_imag;
      const ATOMIC t6_real = t3_real + t2_real;
      const ATOMIC t7_real = t3_real - t2_real;

      const ATOMIC x0_real = t1_real + z2_real;
      const ATOMIC x1_real = t6_real - t4_real;
      const ATOMIC x2_real = t7_real - t5_real;
      const ATOMIC x3_real = -t7_real - t5_real;
      const ATOMIC x4_real = -t6_real - t4_real;

      const size_t to0 = k1 * q + q - 1;
      const size_t to1 = to0 + m;
      const size_t to2 = to1 + m;
      const size_t to3 = to2 + m;
      const size_t to4 = to3 + m;

      VECTOR(out,ostride,to0) = x0_real;
      VECTOR(out,ostride,to1) = x1_real;
      VECTOR(out,ostride,to2) = x2_real;
      VECTOR(out,ostride,to3) = x3_real;
      VECTOR(out,ostride,to4) = x4_real;
    }
  return;
}
