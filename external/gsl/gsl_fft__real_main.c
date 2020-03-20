/* fft/real_main.c
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

#include "gsl__config.h"
#include <stdlib.h>
#include <math.h>

#include "gsl_errno.h"
#include "gsl_complex.h"
#include "gsl_fft_real.h"

#include "gsl_fft__real_pass.h"

int
FUNCTION(gsl_fft_real,transform) (BASE data[], const size_t stride, const size_t n,
                                  const TYPE(gsl_fft_real_wavetable) * wavetable,
                                  TYPE(gsl_fft_real_workspace) * work)
{
  const size_t nf = wavetable->nf;

  size_t i;

  size_t product = 1;
  size_t tskip;
  size_t product_1;

  BASE *const scratch = work->scratch;
  TYPE(gsl_complex) *twiddle1, *twiddle2, *twiddle3, *twiddle4;

  size_t state = 0;
  BASE *in = data;
  size_t istride = stride ;
  BASE *out = scratch;
  size_t ostride = 1 ;
  
  if (n == 0)
    {
      GSL_ERROR ("length n must be positive integer", GSL_EDOM);
    }

  if (n == 1)
    {                           /* FFT of one data point is the identity */
      return 0;
    }

  if (n != wavetable->n)
    {
      GSL_ERROR ("wavetable does not match length of data", GSL_EINVAL);
    }

  if (n != work->n)
    {
      GSL_ERROR ("workspace does not match length of data", GSL_EINVAL);
    }

  for (i = 0; i < nf; i++)
    {
      const size_t factor = wavetable->factor[i];
      product_1 = product;
      product *= factor;

      tskip = (product_1 + 1) / 2 - 1;

      if (state == 0)
        {
          in = data;
          istride = stride;
          out = scratch;
          ostride = 1;
          state = 1;
        }
      else
        {
          in = scratch;
          istride = 1;
          out = data;
          ostride = stride;
          state = 0;
        }

      if (factor == 2)
        {
          twiddle1 = wavetable->twiddle[i];
          FUNCTION(fft_real,pass_2) (in, istride, out, ostride, product, n, twiddle1);
        }
      else if (factor == 3)
        {
          twiddle1 = wavetable->twiddle[i];
          twiddle2 = twiddle1 + tskip;
          FUNCTION(fft_real,pass_3) (in, istride, out, ostride, product, n, twiddle1,
                               twiddle2);
        }
      else if (factor == 4)
        {
          twiddle1 = wavetable->twiddle[i];
          twiddle2 = twiddle1 + tskip;
          twiddle3 = twiddle2 + tskip;
          FUNCTION(fft_real,pass_4) (in, istride, out, ostride, product, n, twiddle1,
                                     twiddle2, twiddle3);
        }
      else if (factor == 5)
        {
          twiddle1 = wavetable->twiddle[i];
          twiddle2 = twiddle1 + tskip;
          twiddle3 = twiddle2 + tskip;
          twiddle4 = twiddle3 + tskip;
          FUNCTION(fft_real,pass_5) (in, istride, out, ostride, product, n, twiddle1,
                                     twiddle2, twiddle3, twiddle4);
        }
      else
        {
          twiddle1 = wavetable->twiddle[i];
          FUNCTION(fft_real,pass_n) (in, istride, out, ostride, factor, product, n,
                                     twiddle1);
        }
    }

  if (state == 1)               /* copy results back from scratch to data */
    {
      for (i = 0; i < n; i++)
        {
          data[stride*i] = scratch[i] ;
        }
    }

  return 0;

}
