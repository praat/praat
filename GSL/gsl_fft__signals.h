/* fft/signals.h
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

int FUNCTION(fft_signal,complex_pulse) (const size_t k, 
                                        const size_t n,
                                        const size_t stride,
                                        const BASE z_real, 
                                        const BASE z_imag,
                                        BASE data[],
                                        BASE fft[]);

int FUNCTION(fft_signal,complex_constant) (const size_t n,
                                           const size_t stride,
                                           const BASE z_real,
                                           const BASE z_imag,
                                           BASE data[],
                                           BASE fft[]);

int FUNCTION(fft_signal,complex_exp) (const int k,
                                      const size_t n,
                                      const size_t stride,
                                      const BASE z_real,
                                      const BASE z_imag,
                                      BASE data[],
                                      BASE fft[]);


int FUNCTION(fft_signal,complex_exppair) (const int k1,
                                          const int k2,
                                          const size_t n,
                                          const size_t stride,
                                          const BASE z1_real,
                                          const BASE z1_imag,
                                          const BASE z2_real,
                                          const BASE z2_imag,
                                          BASE data[],
                                          BASE fft[]);

int FUNCTION(fft_signal,complex_noise) (const size_t n,
                                        const size_t stride,
                                        BASE data[],
                                        BASE fft[]);

int FUNCTION(fft_signal,real_noise) (const size_t n,
                                     const size_t stride,
                                     BASE data[],
                                     BASE fft[]);

