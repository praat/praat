/* fft/c_pass.h
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
                              const TYPE(gsl_complex) twiddle[]);

static int
FUNCTION(fft_complex,pass_3) (const BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle1[],
                              const TYPE(gsl_complex) twiddle2[]);

static int
FUNCTION(fft_complex,pass_4) (const BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle1[],
                              const TYPE(gsl_complex) twiddle2[],
                              const TYPE(gsl_complex) twiddle3[]);

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
                              const TYPE(gsl_complex) twiddle4[]);

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
                              const TYPE(gsl_complex) twiddle5[]);

static int
FUNCTION(fft_complex,pass_7) (const BASE in[],
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
                              const TYPE(gsl_complex) twiddle5[],
                              const TYPE(gsl_complex) twiddle6[]);


static int
FUNCTION(fft_complex,pass_n) (BASE in[],
                              const size_t istride,
                              BASE out[],
                              const size_t ostride,
                              const gsl_fft_direction sign,
                              const size_t factor,
                              const size_t product,
                              const size_t n,
                              const TYPE(gsl_complex) twiddle[]);

