/* specfunc/recurse.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author: G. Jungman */

#ifndef _RECURSE_H_
#define _RECURSE_H_

#define CONCAT(a,b) a ## _ ## b


/* n_max >= n_min + 2
 * f[n+1] + a[n] f[n] + b[n] f[n-1] = 0
 *
 * Trivial forward recurrence.
 */
#define GEN_RECURSE_FORWARD_SIMPLE(func)                                      \
int CONCAT(recurse_forward_simple, func) (                                    \
                               const int n_max, const int n_min,              \
                               const double parameters[],                     \
                               const double f_n_min,                          \
                               const double f_n_min_p1,                       \
                               double * f,                                    \
                               double * f_n_max                               \
                               )                                              \
{                                                                             \
  int n;                                                                      \
                                                                              \
  if(f == 0) {                                                                \
    double f2 = f_n_min;                                                      \
    double f1 = f_n_min_p1;                                                   \
    double f0;                                                                \
    for(n=n_min+2; n<=n_max; n++) {                                           \
      f0 = -REC_COEFF_A(n-1,parameters) * f1 - REC_COEFF_B(n-1, parameters) * f2; \
      f2 = f1;                                                                \
      f1 = f0;                                                                \
    }                                                                         \
    *f_n_max = f0;                                                            \
  }                                                                           \
  else {                                                                      \
    f[n_min]     = f_n_min;                                                   \
    f[n_min + 1] = f_n_min_p1;                                                \
    for(n=n_min+2; n<=n_max; n++) {                                           \
      f[n] = -REC_COEFF_A(n-1,parameters) * f[n-1] - REC_COEFF_B(n-1, parameters) * f[n-2]; \
    }                                                                         \
    *f_n_max = f[n_max];                                                      \
  }                                                                           \
                                                                              \
  return GSL_SUCCESS;                                                         \
}                                                                             \


/* n_start >= n_max >= n_min 
 * f[n+1] + a[n] f[n] + b[n] f[n-1] = 0
 *
 * Generate the minimal solution of the above recursion relation,
 * with the simplest form of the normalization condition, f[n_min] given.
 * [Gautschi, SIAM Rev. 9, 24 (1967); (3.9) with s[n]=0]
 */
#define GEN_RECURSE_BACKWARD_MINIMAL_SIMPLE(func)                             \
int CONCAT(recurse_backward_minimal_simple, func) (                           \
                               const int n_start,                             \
                               const int n_max, const int n_min,              \
                               const double parameters[],                     \
                               const double f_n_min,                          \
                               double * f,                                    \
                               double * f_n_max                               \
                               )                                              \
{                                                                             \
  int n;                                                                      \
  double r_n = 0.;                                                            \
  double r_nm1;                                                               \
  double ratio;                                                               \
                                                                              \
  for(n=n_start; n > n_max; n--) {                                            \
    r_nm1 = -REC_COEFF_B(n, parameters) / (REC_COEFF_A(n, parameters) + r_n); \
    r_n = r_nm1;                                                              \
  }                                                                           \
                                                                              \
  if(f != 0) {                                                                \
    f[n_max] = 10.*DBL_MIN;                                                      \
    for(n=n_max; n > n_min; n--) {                                               \
      r_nm1  = -REC_COEFF_B(n, parameters) / (REC_COEFF_A(n, parameters) + r_n); \
      f[n-1] = f[n] / r_nm1;                                                     \
      r_n = r_nm1;                                                               \
    }                                                                         \
    ratio = f_n_min / f[n_min];                                               \
    for(n=n_min; n<=n_max; n++) {                                             \
      f[n] *= ratio;                                                          \
    }                                                                         \
  }                                                                           \
  else {                                                                      \
    double f_nm1;                                                             \
    double f_n = 10.*DBL_MIN;                                                 \
    *f_n_max = f_n;                                                           \
    for(n=n_max; n > n_min; n--) {                                               \
      r_nm1 = -REC_COEFF_B(n, parameters) / (REC_COEFF_A(n, parameters) + r_n);  \
      f_nm1 = f_n / r_nm1;                                                       \
      r_n = r_nm1;                                                               \
    }                                                                         \
    ratio = f_n_min / f_nm1;                                                  \
    *f_n_max *= ratio;                                                        \
  }                                                                           \
                                                                              \
  return GSL_SUCCESS;                                                         \
}                                                                             \


#endif /* !_RECURSE_H_ */
