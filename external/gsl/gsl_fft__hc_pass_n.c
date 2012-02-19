/* fft/hc_pass_n.c
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
FUNCTION(fft_halfcomplex,pass_n) (const BASE in[],
                                  const size_t istride,
                                  BASE out[],
                                  const size_t ostride,
                                  const size_t factor,
                                  const size_t product,
                                  const size_t n,
                                  const TYPE(gsl_complex) twiddle[])
{

  size_t k, k1;

  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t product_1 = product / factor;

  size_t e1, e2;

  const double d_theta = 2.0 * M_PI / ((double) factor);
  const ATOMIC cos_d_theta = cos (d_theta);
  const ATOMIC sin_d_theta = sin (d_theta);

  for (k1 = 0; k1 < product_1; k1++)
    {
      /* compute z = W(factor) x, for x halfcomplex */

      ATOMIC dw_real = 1.0, dw_imag = 0.0;

      for (e1 = 0; e1 < factor; e1++)
        {
          ATOMIC sum_real = 0.0;
          ATOMIC w_real = 1.0, w_imag = 0.0;

          if (e1 > 0)
            {
              ATOMIC tmp_real = dw_real * cos_d_theta - dw_imag * sin_d_theta;
              ATOMIC tmp_imag = dw_real * sin_d_theta + dw_imag * cos_d_theta;
              dw_real = tmp_real;
              dw_imag = tmp_imag;
            }

          for (e2 = 0; e2 <= factor - e2; e2++)
            {
              ATOMIC z_real, z_imag;

              if (e2 > 0)
                {
                  ATOMIC tmp_real = dw_real * w_real - dw_imag * w_imag;
                  ATOMIC tmp_imag = dw_real * w_imag + dw_imag * w_real;
                  w_real = tmp_real;
                  w_imag = tmp_imag;
                }

              if (e2 == 0)
                {
                  size_t from_idx = factor * k1 * q;
                  z_real = VECTOR(in,istride,from_idx);
                  z_imag = 0.0;
                  sum_real += w_real * z_real - w_imag * z_imag;
                }
              else if (e2 == factor - e2)
                {
                  size_t from_idx = factor * q * k1 + 2 * e2 * q - 1;
                  z_real = VECTOR(in,istride,from_idx);
                  z_imag = 0.0;
                  sum_real += w_real * z_real;
                }
              else
                {
                  size_t from_idx = factor * q * k1 + 2 * e2 * q - 1;
                  z_real = VECTOR(in,istride,from_idx);
                  z_imag = VECTOR(in,istride,from_idx + 1);
                  sum_real += 2 * (w_real * z_real - w_imag * z_imag);
                }

            }

          {
            const size_t to_idx = q * k1 + e1 * m;
            VECTOR(out,ostride,to_idx) = sum_real;
          }
        }
    }

  if (q == 1)
    return;

  for (k = 1; k < (q + 1) / 2; k++)
    {
      for (k1 = 0; k1 < product_1; k1++)
        {

          ATOMIC dw_real = 1.0, dw_imag = 0.0;

          for (e1 = 0; e1 < factor; e1++)
            {
              ATOMIC z_real, z_imag;
              ATOMIC sum_real = 0.0;
              ATOMIC sum_imag = 0.0;
              ATOMIC w_real = 1.0, w_imag = 0.0;

              if (e1 > 0)
                {
                  ATOMIC t_real = dw_real * cos_d_theta - dw_imag * sin_d_theta;
                  ATOMIC t_imag = dw_real * sin_d_theta + dw_imag * cos_d_theta;
                  dw_real = t_real;
                  dw_imag = t_imag;
                }

              for (e2 = 0; e2 < factor; e2++)
                {

                  if (e2 > 0)
                    {
                      ATOMIC tmp_real = dw_real * w_real - dw_imag * w_imag;
                      ATOMIC tmp_imag = dw_real * w_imag + dw_imag * w_real;
                      w_real = tmp_real;
                      w_imag = tmp_imag;
                    }

                  if (e2 < factor - e2)
                    {
                      const size_t from0 = factor * k1 * q + 2 * k + 2 * e2 * q - 1;
                      z_real = VECTOR(in,istride,from0);
                      z_imag = VECTOR(in,istride,from0 + 1);
                    }
                  else
                    {
                      const size_t from0 = factor * k1 * q - 2 * k + 2 * (factor - e2) * q - 1;
                      z_real = VECTOR(in,istride,from0);
                      z_imag = -VECTOR(in,istride,from0 + 1);
                    }

                  sum_real += w_real * z_real - w_imag * z_imag;
                  sum_imag += w_real * z_imag + w_imag * z_real;
                }

              if (k == 0 || e1 == 0)
                {
                  w_real = 1.0;
                  w_imag = 0.0;
                }
              else
                {
                  size_t tskip = (q + 1) / 2 - 1;
                  w_real = GSL_REAL(twiddle[k - 1 + tskip * (e1 - 1)]);
                  w_imag = GSL_IMAG(twiddle[k - 1 + tskip * (e1 - 1)]);
                }

              {
                const size_t to0 = k1 * q + 2 * k + e1 * m - 1;
                VECTOR(out,ostride,to0) = w_real * sum_real - w_imag * sum_imag;
                VECTOR(out,ostride,to0 + 1) = w_real * sum_imag + w_imag * sum_real;
              }

            }
        }
    }


  if (q % 2 == 1)
    return;

  {
    double tw_arg = M_PI / ((double) factor);
    ATOMIC cos_tw_arg = cos (tw_arg);
    ATOMIC sin_tw_arg = sin (tw_arg);

    for (k1 = 0; k1 < product_1; k1++)
      {

        ATOMIC dw_real = 1.0, dw_imag = 0.0;
        ATOMIC tw_real = 1.0, tw_imag = 0.0;

        for (e1 = 0; e1 < factor; e1++)
          {
            ATOMIC w_real, w_imag, z_real, z_imag;

            ATOMIC sum_real = 0.0;

            if (e1 > 0)
              {
                ATOMIC tmp_real = tw_real * cos_tw_arg - tw_imag * sin_tw_arg;
                ATOMIC tmp_imag = tw_real * sin_tw_arg + tw_imag * cos_tw_arg;
                tw_real = tmp_real;
                tw_imag = tmp_imag;
              }

            w_real = tw_real;
            w_imag = tw_imag;

            if (e1 > 0)
              {
                ATOMIC t_real = dw_real * cos_d_theta - dw_imag * sin_d_theta;
                ATOMIC t_imag = dw_real * sin_d_theta + dw_imag * cos_d_theta;
                dw_real = t_real;
                dw_imag = t_imag;
              }

            for (e2 = 0; e2 <= factor - e2 - 1; e2++)
              {

                if (e2 > 0)
                  {
                    ATOMIC tmp_real = dw_real * w_real - dw_imag * w_imag;
                    ATOMIC tmp_imag = dw_real * w_imag + dw_imag * w_real;
                    w_real = tmp_real;
                    w_imag = tmp_imag;
                  }


                if (e2 == factor - e2 - 1)
                  {
                    const size_t from0 = factor * k1 * q + q + 2 * e2 * q - 1;
                    z_real = VECTOR(in,istride,from0);
                    z_imag = 0.0;
                    sum_real += w_real * z_real - w_imag * z_imag;
                  }
                else
                  {
                    const size_t from0 = factor * k1 * q + q + 2 * e2 * q - 1;
                    z_real = VECTOR(in,istride,from0);
                    z_imag = VECTOR(in,istride,from0 + 1);
                    sum_real += 2 * (w_real * z_real - w_imag * z_imag);
                  }

              }

            {
              const size_t to0 = k1 * q + q + e1 * m - 1;
              VECTOR(out,ostride,to0) = sum_real;
            }
          }
      }
  }
  return;
}
