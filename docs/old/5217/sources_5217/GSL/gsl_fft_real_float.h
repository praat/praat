/* fft/gsl_fft_real_float.h
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

#ifndef __GSL_FFT_REAL_FLOAT_H__
#define __GSL_FFT_REAL_FLOAT_H__

#include <stddef.h>

#include "gsl_math.h"
#include "gsl_complex.h"
#include "gsl_fft.h"

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

int gsl_fft_real_float_radix2_transform (float data[], const size_t stride, const size_t n) ;

typedef struct
  {
    size_t n;
    size_t nf;
    size_t factor[64];
    gsl_complex_float *twiddle[64];
    gsl_complex_float *trig;
  }
gsl_fft_real_wavetable_float;

typedef struct
  {
    size_t n;
    float *scratch;
  }
gsl_fft_real_workspace_float;

gsl_fft_real_wavetable_float * gsl_fft_real_wavetable_float_alloc (size_t n);

void  gsl_fft_real_wavetable_float_free (gsl_fft_real_wavetable_float * wavetable);

gsl_fft_real_workspace_float * gsl_fft_real_workspace_float_alloc (size_t n);

void  gsl_fft_real_workspace_float_free (gsl_fft_real_workspace_float * workspace);

int gsl_fft_real_float_transform (float data[], const size_t stride, const size_t n,
                                  const gsl_fft_real_wavetable_float * wavetable,
                                  gsl_fft_real_workspace_float * work);


int gsl_fft_real_float_unpack (const float real_float_coefficient[],
                               float complex_coefficient[],
                               const size_t stride, const size_t n);

__END_DECLS

#endif /* __GSL_FFT_REAL_FLOAT_H__ */
