/* specfunc/gsl_sf_psi.h
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

#ifndef __GSL_SF_PSI_H__
#define __GSL_SF_PSI_H__

#include "gsl_sf_result.h"




/* Poly-Gamma Functions
 *
 * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
 */


/* Di-Gamma Function  psi(n)
 *
 * n > 0
 * exceptions: GSL_EDOM
 */
int     gsl_sf_psi_int_e(const int n, gsl_sf_result * result);
double     gsl_sf_psi_int(const int n);


/* Di-Gamma Function psi(x)
 *
 * x != 0.0
 * exceptions: GSL_EDOM, GSL_ELOSS
 */
int     gsl_sf_psi_e(const double x, gsl_sf_result * result);
double     gsl_sf_psi(const double x);


/* Di-Gamma Function Re[psi(1 + I y)]
 *
 * exceptions: none
 */
int     gsl_sf_psi_1piy_e(const double y, gsl_sf_result * result);
double     gsl_sf_psi_1piy(const double y);


/* Tri-Gamma Function psi^(1)(n)
 *
 * n > 0
 * exceptions: GSL_EDOM
 */
int     gsl_sf_psi_1_int_e(const int n, gsl_sf_result * result);
double     gsl_sf_psi_1_int(const int n);


/* Poly-Gamma Function psi^(n)(x)
 *
 * n >= 0, x > 0.0
 * exceptions: GSL_EDOM
 */
int     gsl_sf_psi_n_e(const int n, const double x, gsl_sf_result * result);
double     gsl_sf_psi_n(const int n, const double x);



#endif /* __GSL_SF_PSI_H__ */
