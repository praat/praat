/* fft/bitreverse.c
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
#include "gsl_fft.h"

#include "gsl_fft__complex_internal.h"
#include "gsl_fft__bitreverse.h"

static int 
FUNCTION(fft_complex,bitreverse_order) (BASE data[], 
                                        const size_t stride,
                                        const size_t n,
                                        size_t logn)
{
  /* This is the Goldrader bit-reversal algorithm */

  size_t i;
  size_t j = 0;

  logn = 0 ; /* not needed for this algorithm */

  for (i = 0; i < n - 1; i++)
    {
      size_t k = n / 2 ;

      if (i < j)
        {
          const BASE tmp_real = REAL(data,stride,i);
          const BASE tmp_imag = IMAG(data,stride,i);
          REAL(data,stride,i) = REAL(data,stride,j);
          IMAG(data,stride,i) = IMAG(data,stride,j);
          REAL(data,stride,j) = tmp_real;
          IMAG(data,stride,j) = tmp_imag;
        }

      while (k <= j) 
        {
          j = j - k ;
          k = k / 2 ;
        }

      j += k ;
    }

  return 0;
}


static int 
FUNCTION(fft_real,bitreverse_order) (BASE data[], 
                                const size_t stride, 
                                const size_t n,
                                size_t logn)
{
  /* This is the Goldrader bit-reversal algorithm */

  size_t i;
  size_t j = 0;

  logn = 0 ; /* not needed for this algorithm */

  for (i = 0; i < n - 1; i++)
    {
      size_t k = n / 2 ;

      if (i < j)
        {
          const BASE tmp = VECTOR(data,stride,i);
          VECTOR(data,stride,i) = VECTOR(data,stride,j);
          VECTOR(data,stride,j) = tmp;
        }

      while (k <= j) 
        {
          j = j - k ;
          k = k / 2 ;
        }

      j += k ;
    }

  return 0;
}

