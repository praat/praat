/* specfunc/gsl_sf_zeta.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Author:  G. Jungman */

#ifndef __GSL_SF_ZETA_H__
#define __GSL_SF_ZETA_H__

#include "gsl_sf_result.h"




/* Riemann Zeta Function
 * zeta(n) = Sum[ k^(-n), {k,1,Infinity} ]
 *
 * n=integer, n != 1
 * exceptions: GSL_EDOM, GSL_EOVRFLW
 */
int gsl_sf_zeta_int_e(const int n, gsl_sf_result * result);
double gsl_sf_zeta_int(const int n);


/* Riemann Zeta Function
 * zeta(x) = Sum[ k^(-s), {k,1,Infinity} ], s != 1.0
 *
 * s != 1.0
 * exceptions: GSL_EDOM, GSL_EOVRFLW
 */
int gsl_sf_zeta_e(const double s, gsl_sf_result * result);
double gsl_sf_zeta(const double s);


/* Hurwitz Zeta Function
 * zeta(s,q) = Sum[ (k+q)^(-s), {k,0,Infinity} ]
 *
 * s > 1.0, q > 0.0
 * exceptions: GSL_EDOM, GSL_EUNDRFLW, GSL_EOVRFLW
 */
int gsl_sf_hzeta_e(const double s, const double q, gsl_sf_result * result);
double gsl_sf_hzeta(const double s, const double q);


/* Eta Function
 * eta(n) = (1-2^(1-n)) zeta(n)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int gsl_sf_eta_int_e(int n, gsl_sf_result * result);
double gsl_sf_eta_int(const int n);


/* Eta Function
 * eta(s) = (1-2^(1-s)) zeta(s)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int gsl_sf_eta_e(const double s, gsl_sf_result * result);
double gsl_sf_eta(const double s);



#endif /* __GSL_SF_ZETA_H__ */
