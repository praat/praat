/* fft/c_pass_5.c
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

static int
FUNCTION(fft_complex,pass_5) (const BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle1[],
                              const TYPE(gsl_complex) twiddle2[],
                              const TYPE(gsl_complex) twiddle3[],
                              const TYPE(gsl_complex) twiddle4[])
{
  size_t i = 0, j = 0;
  size_t k, k1;

  const size_t factor = 5;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t p_1 = product / factor;
  const size_t jump = (factor - 1) * p_1;

  const ATOMIC sin_2pi_by_5 = sin (2.0 * M_PI / 5.0);
  const ATOMIC sin_2pi_by_10 = sin (2.0 * M_PI / 10.0);

  for (k = 0; k < q; k++)
    {

      ATOMIC w1_real, w1_imag, w2_real, w2_imag, w3_real, w3_imag, w4_real,
        w4_imag;

      if (k == 0)
        {
          w1_real = 1.0;
          w1_imag = 0.0;
          w2_real = 1.0;
          w2_imag = 0.0;
          w3_real = 1.0;
          w3_imag = 0.0;
          w4_real = 1.0;
          w4_imag = 0.0;
        }
      else
        {
          if (sign == gsl_fft_forward)
            {
              /* forward tranform */
              w1_real = GSL_REAL(twiddle1[k - 1]);
              w1_imag = GSL_IMAG(twiddle1[k - 1]);
              w2_real = GSL_REAL(twiddle2[k - 1]);
              w2_imag = GSL_IMAG(twiddle2[k - 1]);
              w3_real = GSL_REAL(twiddle3[k - 1]);
              w3_imag = GSL_IMAG(twiddle3[k - 1]);
              w4_real = GSL_REAL(twiddle4[k - 1]);
              w4_imag = GSL_IMAG(twiddle4[k - 1]);
            }
          else
            {
              /* backward tranform: w -> conjugate(w) */
              w1_real = GSL_REAL(twiddle1[k - 1]);
              w1_imag = -GSL_IMAG(twiddle1[k - 1]);
              w2_real = GSL_REAL(twiddle2[k - 1]);
              w2_imag = -GSL_IMAG(twiddle2[k - 1]);
              w3_real = GSL_REAL(twiddle3[k - 1]);
              w3_imag = -GSL_IMAG(twiddle3[k - 1]);
              w4_real = GSL_REAL(twiddle4[k - 1]);
              w4_imag = -GSL_IMAG(twiddle4[k - 1]);
            }
        }

      for (k1 = 0; k1 < p_1; k1++)
        {

          ATOMIC x0_real, x0_imag, x1_real, x1_imag, x2_real, x2_imag,
            x3_real, x3_imag, x4_real, x4_imag;

          const ATOMIC z0_real = REAL(in,istride,i);
          const ATOMIC z0_imag = IMAG(in,istride,i);
          const ATOMIC z1_real = REAL(in,istride,i + m);
          const ATOMIC z1_imag = IMAG(in,istride,i + m);
          const ATOMIC z2_real = REAL(in,istride,i + 2*m);
          const ATOMIC z2_imag = IMAG(in,istride,i + 2*m);
          const ATOMIC z3_real = REAL(in,istride,i + 3*m);
          const ATOMIC z3_imag = IMAG(in,istride,i + 3*m);
          const ATOMIC z4_real = REAL(in,istride,i + 4*m);
          const ATOMIC z4_imag = IMAG(in,istride,i + 4*m);

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
          const ATOMIC t10_real = ((int) sign) * (sin_2pi_by_5 * t3_real +
                                                  sin_2pi_by_10 * t4_real);
          const ATOMIC t10_imag = ((int) sign) * (sin_2pi_by_5 * t3_imag +
                                                  sin_2pi_by_10 * t4_imag);
          
          /* t11 = sin(2 pi/10) t3 - sin(2 pi/5) t4 */
          const ATOMIC t11_real = ((int) sign) * (sin_2pi_by_10 * t3_real -
                                                  sin_2pi_by_5 * t4_real);
          const ATOMIC t11_imag = ((int) sign) * (sin_2pi_by_10 * t3_imag -
                                                  sin_2pi_by_5 * t4_imag);
          
          /* x0 = z0 + t5 */
          x0_real = z0_real + t5_real;
          x0_imag = z0_imag + t5_imag;
          
          /* x1 = t8 + i t10 */
          x1_real = t8_real - t10_imag;
          x1_imag = t8_imag + t10_real;
          
          /* x2 = t9 + i t11 */
          x2_real = t9_real - t11_imag;
          x2_imag = t9_imag + t11_real;
          
          /* x3 = t9 - i t11 */
          x3_real = t9_real + t11_imag;
          x3_imag = t9_imag - t11_real;
          
          /* x4 = t8 - i t10 */
          x4_real = t8_real + t10_imag;
          x4_imag = t8_imag - t10_real;
      
          /* apply twiddle factors */
          
          /* to0 = 1 * x0 */
          REAL(out,ostride,j) = x0_real;
          IMAG(out,ostride,j) = x0_imag;
          
          /* to1 = w1 * x1 */
          REAL(out,ostride,j + p_1) = w1_real * x1_real - w1_imag * x1_imag;
          IMAG(out,ostride,j + p_1) = w1_real * x1_imag + w1_imag * x1_real;
          
          /* to2 = w2 * x2 */
          REAL(out,ostride,j + 2*p_1) = w2_real * x2_real - w2_imag * x2_imag;
          IMAG(out,ostride,j+2*p_1) = w2_real * x2_imag + w2_imag * x2_real;
          
          /* to3 = w3 * x3 */
          REAL(out,ostride,j+3*p_1) = w3_real * x3_real - w3_imag * x3_imag;
          IMAG(out,ostride,j+3*p_1) = w3_real * x3_imag + w3_imag * x3_real;
          
          /* to4 = w4 * x4 */
          REAL(out,ostride,j+4*p_1) = w4_real * x4_real - w4_imag * x4_imag;
          IMAG(out,ostride,j+4*p_1) = w4_real * x4_imag + w4_imag * x4_real;
          
          i++;
          j++;
        }
      j += jump;
    }
  return 0;
}
