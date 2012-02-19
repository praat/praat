/* fft/c_pass_6.c
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
FUNCTION(fft_complex,pass_6) (const BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle1[],
                              const TYPE(gsl_complex) twiddle2[],
                              const TYPE(gsl_complex) twiddle3[],
                              const TYPE(gsl_complex) twiddle4[],
                              const TYPE(gsl_complex) twiddle5[])
{

  size_t i = 0, j = 0;
  size_t k, k1;

  const size_t factor = 6;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t p_1 = product / factor;
  const size_t jump = (factor - 1) * p_1;

  const ATOMIC tau = sqrt (3.0) / 2.0;

  for (k = 0; k < q; k++)
    {
      ATOMIC w1_real, w1_imag, w2_real, w2_imag, w3_real, w3_imag, w4_real,
        w4_imag, w5_real, w5_imag;

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
          w5_real = 1.0;
          w5_imag = 0.0;
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
              w5_real = GSL_REAL(twiddle5[k - 1]);
              w5_imag = GSL_IMAG(twiddle5[k - 1]);
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
              w5_real = GSL_REAL(twiddle5[k - 1]);
              w5_imag = -GSL_IMAG(twiddle5[k - 1]);
            }
        }

      for (k1 = 0; k1 < p_1; k1++)
        {
          const ATOMIC z0_real = REAL(in,istride,i);
          const ATOMIC z0_imag = IMAG(in,istride,i);
          const ATOMIC z1_real = REAL(in,istride,i+m);
          const ATOMIC z1_imag = IMAG(in,istride,i+m);
          const ATOMIC z2_real = REAL(in,istride,i+2*m);
          const ATOMIC z2_imag = IMAG(in,istride,i+2*m);
          const ATOMIC z3_real = REAL(in,istride,i+3*m);
          const ATOMIC z3_imag = IMAG(in,istride,i+3*m);
          const ATOMIC z4_real = REAL(in,istride,i+4*m);
          const ATOMIC z4_imag = IMAG(in,istride,i+4*m);
          const ATOMIC z5_real = REAL(in,istride,i+5*m);
          const ATOMIC z5_imag = IMAG(in,istride,i+5*m);

          /* compute x = W(6) z */

          /* W(6) is a combination of sums and differences of W(3) acting
             on the even and odd elements of z */
          
          /* ta1 = z2 + z4 */
          const ATOMIC ta1_real = z2_real + z4_real;
          const ATOMIC ta1_imag = z2_imag + z4_imag;
          
          /* ta2 = z0 - ta1/2 */
          const ATOMIC ta2_real = z0_real - ta1_real / 2;
          const ATOMIC ta2_imag = z0_imag - ta1_imag / 2;
          
          /* ta3 = (+/-) sin(pi/3)*(z2 - z4) */
          const ATOMIC ta3_real = ((int) sign) * tau * (z2_real - z4_real);
          const ATOMIC ta3_imag = ((int) sign) * tau * (z2_imag - z4_imag);
          
          /* a0 = z0 + ta1 */
          const ATOMIC a0_real = z0_real + ta1_real;
          const ATOMIC a0_imag = z0_imag + ta1_imag;
          
          /* a1 = ta2 + i ta3 */
          const ATOMIC a1_real = ta2_real - ta3_imag;
          const ATOMIC a1_imag = ta2_imag + ta3_real;
          
          /* a2 = ta2 - i ta3 */
          const ATOMIC a2_real = ta2_real + ta3_imag;
          const ATOMIC a2_imag = ta2_imag - ta3_real;
          
          /* tb1 = z5 + z1 */
          const ATOMIC tb1_real = z5_real + z1_real;
          const ATOMIC tb1_imag = z5_imag + z1_imag;
          
          /* tb2 = z3 - tb1/2 */
          const ATOMIC tb2_real = z3_real - tb1_real / 2;
          const ATOMIC tb2_imag = z3_imag - tb1_imag / 2;
          
          /* tb3 = (+/-) sin(pi/3)*(z5 - z1) */
          const ATOMIC tb3_real = ((int) sign) * tau * (z5_real - z1_real);
          const ATOMIC tb3_imag = ((int) sign) * tau * (z5_imag - z1_imag);
          
          /* b0 = z3 + tb1 */
          const ATOMIC b0_real = z3_real + tb1_real;
          const ATOMIC b0_imag = z3_imag + tb1_imag;
          
          /* b1 = tb2 + i tb3 */
          const ATOMIC b1_real = tb2_real - tb3_imag;
          const ATOMIC b1_imag = tb2_imag + tb3_real;
          
          /* b2 = tb2 - i tb3 */
          const ATOMIC b2_real = tb2_real + tb3_imag;
          const ATOMIC b2_imag = tb2_imag - tb3_real;
          
          /* x0 = a0 + b0 */
          const ATOMIC x0_real = a0_real + b0_real;
          const ATOMIC x0_imag = a0_imag + b0_imag;
          
          /* x4 = a1 + b1 */
          const ATOMIC x4_real = a1_real + b1_real;
          const ATOMIC x4_imag = a1_imag + b1_imag;
          
          /* x2 = a2 + b2 */
          const ATOMIC x2_real = a2_real + b2_real;
          const ATOMIC x2_imag = a2_imag + b2_imag;
          
          /* x3 = a0 - b0 */
          const ATOMIC x3_real = a0_real - b0_real;
          const ATOMIC x3_imag = a0_imag - b0_imag;
          
          /* x1 = a1 - b1 */
          const ATOMIC x1_real = a1_real - b1_real;
          const ATOMIC x1_imag = a1_imag - b1_imag;
          
          /* x5 = a2 - b2 */
          const ATOMIC x5_real = a2_real - b2_real;
          const ATOMIC x5_imag = a2_imag - b2_imag;

          /* apply twiddle factors */
          
          /* to0 = 1 * x0 */
          REAL(out,ostride,j) = x0_real;
          IMAG(out,ostride,j) = x0_imag;
          
          /* to1 = w1 * x1 */
          REAL(out,ostride,j+p_1) = w1_real * x1_real - w1_imag * x1_imag;
          IMAG(out,ostride,j+p_1) = w1_real * x1_imag + w1_imag * x1_real;
          
          /* to2 = w2 * x2 */
          REAL(out,ostride,j+2*p_1) = w2_real * x2_real - w2_imag * x2_imag;
          IMAG(out,ostride,j+2*p_1) = w2_real * x2_imag + w2_imag * x2_real;
          
          /* to3 = w3 * x3 */
          REAL(out,ostride,j+3*p_1) = w3_real * x3_real - w3_imag * x3_imag;
          IMAG(out,ostride,j+3*p_1) = w3_real * x3_imag + w3_imag * x3_real;
          
          /* to4 = w4 * x4 */
          REAL(out,ostride,j+4*p_1) = w4_real * x4_real - w4_imag * x4_imag;
          IMAG(out,ostride,j+4*p_1) = w4_real * x4_imag + w4_imag * x4_real;
          
          /* to5 = w5 * x5 */
          REAL(out,ostride,j+5*p_1) = w5_real * x5_real - w5_imag * x5_imag;
          IMAG(out,ostride,j+5*p_1) = w5_real * x5_imag + w5_imag * x5_real;

          i++;
          j++;
        }
      j += jump;
    }
  return 0;
}
