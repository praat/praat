/* fft/c_pass_2.c
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
FUNCTION(fft_complex,pass_2) (const BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle[])
{
  size_t i = 0, j = 0;
  size_t k, k1;

  const size_t factor = 2;
  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t product_1 = product / factor;
  const size_t jump = (factor - 1) * product_1;

  for (k = 0; k < q; k++)
    {
      ATOMIC w_real, w_imag;

      if (k == 0)
        {
          w_real = 1.0;
          w_imag = 0.0;
        }
      else
        {
          if (sign == gsl_fft_forward)
            {
              /* forward tranform */
              w_real = GSL_REAL(twiddle[k - 1]);
              w_imag = GSL_IMAG(twiddle[k - 1]);
            }
          else
            {
              /* backward tranform: w -> conjugate(w) */
              w_real = GSL_REAL(twiddle[k - 1]);
              w_imag = -GSL_IMAG(twiddle[k - 1]);
            }
        }

      for (k1 = 0; k1 < product_1; k1++)
        {
          const ATOMIC z0_real = REAL(in,istride,i);
          const ATOMIC z0_imag = IMAG(in,istride,i);

          const ATOMIC z1_real = REAL(in,istride,i+m);
          const ATOMIC z1_imag = IMAG(in,istride,i+m);

          /* compute x = W(2) z */

          /* x0 = z0 + z1 */
          const ATOMIC x0_real = z0_real + z1_real;
          const ATOMIC x0_imag = z0_imag + z1_imag;

          /* x1 = z0 - z1 */
          const ATOMIC x1_real = z0_real - z1_real;
          const ATOMIC x1_imag = z0_imag - z1_imag;

          /* apply twiddle factors */
          
          /* out0 = 1 * x0 */
          REAL(out,ostride,j) = x0_real;
          IMAG(out,ostride,j) = x0_imag;
          
          /* out1 = w * x1 */
          REAL(out,ostride,j+product_1) = w_real * x1_real - w_imag * x1_imag;
          IMAG(out,ostride,j+product_1) = w_real * x1_imag + w_imag * x1_real;
          
          i++;
          j++;
        }
      j += jump;
    }
  return 0;
}
