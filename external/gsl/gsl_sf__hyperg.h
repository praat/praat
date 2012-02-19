/* specfunc/hyperg.h
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

/* Author:  G. Jungman */

/* Miscellaneous implementations of use
 * for evaluation of hypergeometric functions.
 */
#ifndef _HYPERG_H_
#define _HYPERG_H_

#include "gsl_sf_result.h"


/* Direct implementation of 1F1 series.
 */
int
gsl_sf_hyperg_1F1_series_e(const double a, const double b, const double x, gsl_sf_result * result);


/* Implementation of the 1F1 related to the
 * incomplete gamma function: 1F1(1,b,x), b >= 1.
 */
int
gsl_sf_hyperg_1F1_1_e(double b, double x, gsl_sf_result * result);


/* 1F1(1,b,x) for integer b >= 1
 */
int
gsl_sf_hyperg_1F1_1_int_e(int b, double x, gsl_sf_result * result);


/* Implementation of large b asymptotic.
 * [Bateman v. I, 6.13.3 (18)]
 * [Luke, The Special Functions and Their Approximations v. I, p. 129, 4.8 (4)]
 *
 * a^2 << b, |x|/|b| < 1 - delta
 */
int
gsl_sf_hyperg_1F1_large_b_e(const double a, const double b, const double x, gsl_sf_result * result);


/* Implementation of large b asymptotic.
 *
 * Assumes a > 0 is small, x > 0, and |x|<|b|.
 */
int
gsl_sf_hyperg_U_large_b_e(const double a, const double b, const double x,
                             gsl_sf_result * result,
                             double * ln_multiplier
                             );


/* Implementation of 2F0 asymptotic series.
 */
int
gsl_sf_hyperg_2F0_series_e(const double a, const double b, const double x, int n_trunc,
                              gsl_sf_result * result);


#endif /* !_HYPERG_H_ */
