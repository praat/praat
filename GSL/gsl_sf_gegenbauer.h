/* specfunc/gsl_sf_gegenbauer.h
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

#ifndef __GSL_SF_GEGENBAUER_H__
#define __GSL_SF_GEGENBAUER_H__

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


/* Evaluate Gegenbauer polynomials
 * using explicit representations.
 *
 * exceptions: none
 */
int gsl_sf_gegenpoly_1_e(double lambda, double x, gsl_sf_result * result);
int gsl_sf_gegenpoly_2_e(double lambda, double x, gsl_sf_result * result);
int gsl_sf_gegenpoly_3_e(double lambda, double x, gsl_sf_result * result);
double gsl_sf_gegenpoly_1(double lambda, double x);
double gsl_sf_gegenpoly_2(double lambda, double x);
double gsl_sf_gegenpoly_3(double lambda, double x);


/* Evaluate Gegenbauer polynomials.
 *
 * lambda > -1/2, n >= 0
 * exceptions: GSL_EDOM
 */
int gsl_sf_gegenpoly_n_e(int n, double lambda, double x, gsl_sf_result * result);
double gsl_sf_gegenpoly_n(int n, double lambda, double x);


/* Calculate array of Gegenbauer polynomials
 * for n = (0, 1, 2, ... nmax)
 *
 * lambda > -1/2, nmax >= 0
 * exceptions: GSL_EDOM
 */
int gsl_sf_gegenpoly_array(int nmax, double lambda, double x, double * result_array);


__END_DECLS

#endif /* __GSL_SF_GEGENBAUER_H__ */
