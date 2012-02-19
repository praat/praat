/* fft/c_pass_n.c
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
FUNCTION(fft_complex,pass_n) (BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t factor,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle[])
{
  size_t i = 0, j = 0;
  size_t k, k1;

  const size_t m = n / factor;
  const size_t q = n / product;
  const size_t p_1 = product / factor;
  const size_t jump = (factor - 1) * p_1;

  size_t e, e1;

  for (i = 0; i < m; i++)
    {
      REAL(out,ostride,i) = REAL(in,istride,i);
      IMAG(out,ostride,i) = IMAG(in,istride,i);
    }

  for (e = 1; e < (factor - 1) / 2 + 1; e++)
    {
      for (i = 0; i < m; i++)
        {
          const size_t idx = i + e * m;
          const size_t idxc = i + (factor - e) * m;
          REAL(out,ostride,idx) = REAL(in,istride,idx) + REAL(in,istride,idxc);
          IMAG(out,ostride,idx) = IMAG(in,istride,idx) + IMAG(in,istride,idxc);
          REAL(out,ostride,idxc) = REAL(in,istride,idx) - REAL(in,istride,idxc);
          IMAG(out,ostride,idxc) = IMAG(in,istride,idx) - IMAG(in,istride,idxc);
        }
    }

  /* e = 0 */

  for (i=0 ; i<m; i++) 
    {
      REAL(in,istride,i) = REAL(out,ostride,i);
      IMAG(in,istride,i) = IMAG(out,ostride,i);
    }

  for (e1 = 1; e1 < (factor - 1) / 2 + 1; e1++)
    {
      for (i = 0; i < m; i++)
        {
          REAL(in,istride,i) += REAL(out,ostride,i + e1*m) ;
          IMAG(in,istride,i) += IMAG(out,ostride,i + e1*m) ;
        }
    }

  for (e = 1; e < (factor-1)/2 + 1; e++)
    {
      size_t idx = e*q ;
      const size_t idx_step = e * q ;
      ATOMIC w_real, w_imag ;

      const size_t em = e * m ;
      const size_t ecm = (factor - e) * m ;

      for (i = 0; i < m; i++) 
        {
          REAL(in,istride,i+em) = REAL(out,ostride,i) ;
          IMAG(in,istride,i+em) = IMAG(out,ostride,i) ;
          REAL(in,istride,i+ecm) = REAL(out,ostride,i) ;
          IMAG(in,istride,i+ecm) = IMAG(out,ostride,i) ;
        }

      for (e1 = 1; e1 < (factor - 1) / 2 + 1; e1++)
        {
          if (idx == 0) {
            w_real = 1 ;
            w_imag = 0 ;
          } else {
            if (sign == gsl_fft_forward) {
              w_real = GSL_REAL(twiddle[idx - 1]) ;
              w_imag = GSL_IMAG(twiddle[idx - 1]) ;
            } else {
              w_real = GSL_REAL(twiddle[idx - 1]) ;
              w_imag = -GSL_IMAG(twiddle[idx - 1]) ;
            }
          }

          for (i = 0; i < m; i++) 
            {
              const ATOMIC xp_real = REAL(out,ostride,i + e1 * m);
              const ATOMIC xp_imag = IMAG(out,ostride,i + e1 * m);
              const ATOMIC xm_real = REAL(out,ostride,i + (factor - e1) *m);
              const ATOMIC xm_imag = IMAG(out,ostride,i + (factor - e1) *m);
        
              const ATOMIC ap = w_real * xp_real ;
              const ATOMIC am = w_imag * xm_imag ; 

              ATOMIC sum_real = ap - am;
              ATOMIC sumc_real = ap + am;

              const ATOMIC bp = w_real * xp_imag ;
              const ATOMIC bm = w_imag * xm_real ;

              ATOMIC sum_imag = bp + bm;
              ATOMIC sumc_imag = bp - bm;

              REAL(in,istride,i + em) += sum_real;
              IMAG(in,istride,i + em) += sum_imag;
              REAL(in,istride,i + ecm) += sumc_real;
              IMAG(in,istride,i + ecm) += sumc_imag;
            }
          idx += idx_step ;
          idx %= factor * q ;
        }
    }

  i = 0;
  j = 0;

  /* k = 0 */
  for (k1 = 0; k1 < p_1; k1++)
    {
      REAL(out,ostride,k1) = REAL(in,istride,k1);
      IMAG(out,ostride,k1) = IMAG(in,istride,k1);
    }

  for (e1 = 1; e1 < factor; e1++)
    {
      for (k1 = 0; k1 < p_1; k1++)
        {
          REAL(out,ostride,k1 + e1 * p_1) = REAL(in,istride,k1 + e1 * m) ;
          IMAG(out,ostride,k1 + e1 * p_1) = IMAG(in,istride,k1 + e1 * m) ;
        }
    }

  i = p_1 ;
  j = product ;

  for (k = 1; k < q; k++)
    {
      for (k1 = 0; k1 < p_1; k1++)
        {
          REAL(out,ostride,j) = REAL(in,istride,i);
          IMAG(out,ostride,j) = IMAG(in,istride,i);
          i++;
          j++;
        }
      j += jump;
    }

  i = p_1 ;
  j = product ;

  for (k = 1; k < q; k++)
    {
      for (k1 = 0; k1 < p_1; k1++)
        {
          for (e1 = 1; e1 < factor; e1++)
            {
              ATOMIC x_real = REAL(in, istride,i + e1 * m);
              ATOMIC x_imag = IMAG(in, istride,i + e1 * m);

              ATOMIC w_real, w_imag ;
              if (sign == gsl_fft_forward) {
                w_real = GSL_REAL(twiddle[(e1-1)*q + k-1]) ;
                w_imag = GSL_IMAG(twiddle[(e1-1)*q + k-1]) ;
              } else {
                w_real = GSL_REAL(twiddle[(e1-1)*q + k-1]) ;
                w_imag = -GSL_IMAG(twiddle[(e1-1)*q + k-1]) ; 
              }

              REAL(out,ostride,j + e1 * p_1) = w_real * x_real - w_imag * x_imag;
              IMAG(out,ostride,j + e1 * p_1) = w_real * x_imag + w_imag * x_real;
            }
          i++;
          j++;
        }
      j += jump;
    }

  return 0;
}

