/* fft/gsl_fft_real.h
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

#ifndef __GSL_FFT_REAL_H__
#define __GSL_FFT_REAL_H__

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

int gsl_fft_real_radix2_transform (double data[], const size_t stride, const size_t n) ;

typedef struct
  {
    size_t n;
    size_t nf;
    size_t factor[64];
    gsl_complex *twiddle[64];
    gsl_complex *trig;
  }
gsl_fft_real_wavetable;

typedef struct
  {
    size_t n;
    double *scratch;
  }
gsl_fft_real_workspace;

gsl_fft_real_wavetable * gsl_fft_real_wavetable_alloc (size_t n);

void  gsl_fft_real_wavetable_free (gsl_fft_real_wavetable * wavetable);

gsl_fft_real_workspace * gsl_fft_real_workspace_alloc (size_t n);

void  gsl_fft_real_workspace_free (gsl_fft_real_workspace * workspace);


int gsl_fft_real_transform (double data[], const size_t stride, const size_t n,
                            const gsl_fft_real_wavetable * wavetable,
                            gsl_fft_real_workspace * work);


int gsl_fft_real_unpack (const double real_coefficient[],
                         double complex_coefficient[],
                         const size_t stride, const size_t n);

__END_DECLS

#endif /* __GSL_FFT_REAL_H__ */
