/* specfunc/bessel.h
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

#ifndef _BESSEL_H_
#define _BESSEL_H_

#include "gsl_sf_result.h"


/* Taylor expansion for J_nu(x) or I_nu(x)
 *   sign = -1  ==> Jnu
 *   sign = +1  ==> Inu
 */
int gsl_sf_bessel_IJ_taylor_e(const double nu, const double x,
                                 const int sign,
                                 const int kmax,
                                 const double threshold,
                                 gsl_sf_result * result
                                 );

int gsl_sf_bessel_Jnu_asympx_e(const double nu, const double x, gsl_sf_result * result);
int gsl_sf_bessel_Ynu_asympx_e(const double nu, const double x, gsl_sf_result * result);

int gsl_sf_bessel_Inu_scaled_asympx_e(const double nu, const double x, gsl_sf_result * result);
int gsl_sf_bessel_Knu_scaled_asympx_e(const double nu, const double x, gsl_sf_result * result);

int gsl_sf_bessel_Inu_scaled_asymp_unif_e(const double nu, const double x, gsl_sf_result * result);
int gsl_sf_bessel_Knu_scaled_asymp_unif_e(const double nu, const double x, gsl_sf_result * result);


/* ratio = J_{nu+1}(x) / J_nu(x)
 * sgn   = sgn(J_nu(x))
 */
int
gsl_sf_bessel_J_CF1(const double nu, const double x, double * ratio, double * sgn);


/* ratio = I_{nu+1}(x) / I_nu(x)
 */
int
gsl_sf_bessel_I_CF1_ser(const double nu, const double x, double * ratio);


/* Evaluate the Steed method continued fraction CF2 for
 *
 * (J' + i Y')/(J + i Y) := P + i Q
 */
int
gsl_sf_bessel_JY_steed_CF2(const double nu, const double x,
                           double * P, double * Q);


int
gsl_sf_bessel_JY_mu_restricted(const double mu, const double x,
                               gsl_sf_result * Jmu, gsl_sf_result * Jmup1,
                               gsl_sf_result * Ymu, gsl_sf_result * Ymup1);


int
gsl_sf_bessel_K_scaled_steed_temme_CF2(const double nu, const double x,
                                       double * K_nu, double * K_nup1,
                                       double * Kp_nu);


/* These are of use in calculating the oscillating
 * Bessel functions.
 *   cos(y - pi/4 + eps)
 *   sin(y - pi/4 + eps)
 */
int gsl_sf_bessel_cos_pi4_e(double y, double eps, gsl_sf_result * result);
int gsl_sf_bessel_sin_pi4_e(double y, double eps, gsl_sf_result * result);


#endif /* !_BESSEL_H_ */
