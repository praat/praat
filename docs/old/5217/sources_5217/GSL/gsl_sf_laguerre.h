/* specfunc/gsl_sf_laguerre.h
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

#ifndef __GSL_SF_LAGUERRE_H__
#define __GSL_SF_LAGUERRE_H__

#include "gsl_sf_result.h"

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


/* L^a_n(x) = (a+1)_n / n! 1F1(-n,a+1,x) */


/* Evaluate generalized Laguerre polynomials
 * using explicit representations.
 *
 * exceptions: none
 */
int gsl_sf_laguerre_1_e(const double a, const double x, gsl_sf_result * result);
int gsl_sf_laguerre_2_e(const double a, const double x, gsl_sf_result * result);
int gsl_sf_laguerre_3_e(const double a, const double x, gsl_sf_result * result);
double gsl_sf_laguerre_1(double a, double x);
double gsl_sf_laguerre_2(double a, double x);
double gsl_sf_laguerre_3(double a, double x);


/* Evaluate generalized Laguerre polynomials.
 *
 * a > -1.0
 * n >= 0
 * exceptions: GSL_EDOM
 */
int     gsl_sf_laguerre_n_e(const int n, const double a, const double x, gsl_sf_result * result);
double     gsl_sf_laguerre_n(int n, double a, double x);


__END_DECLS

#endif /* __GSL_SF_LAGUERRE_H__ */
