/* specfunc/gsl_sf_fermi_dirac.h
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

#ifndef __GSL_SF_FERMI_DIRAC_H__
#define __GSL_SF_FERMI_DIRAC_H__

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


/* Complete Fermi-Dirac Integrals:
 *
 *  F_j(x)   := 1/Gamma[j+1] Integral[ t^j /(Exp[t-x] + 1), {t,0,Infinity}]
 *
 *
 * Incomplete Fermi-Dirac Integrals:
 *
 *  F_j(x,b) := 1/Gamma[j+1] Integral[ t^j /(Exp[t-x] + 1), {t,b,Infinity}]
 */


/* Complete integral F_{-1}(x) = e^x / (1 + e^x)
 *
 * exceptions: GSL_EUNDRFLW
 */
int     gsl_sf_fermi_dirac_m1_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_m1(const double x);


/* Complete integral F_0(x) = ln(1 + e^x)
 *
 * exceptions: GSL_EUNDRFLW
 */
int     gsl_sf_fermi_dirac_0_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_0(const double x);


/* Complete integral F_1(x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_1_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_1(const double x);


/* Complete integral F_2(x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_2_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_2(const double x);


/* Complete integral F_j(x)
 * for integer j
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_int_e(const int j, const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_int(const int j, const double x);


/* Complete integral F_{-1/2}(x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_mhalf_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_mhalf(const double x);


/* Complete integral F_{1/2}(x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_half_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_half(const double x);


/* Complete integral F_{3/2}(x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EOVRFLW
 */
int     gsl_sf_fermi_dirac_3half_e(const double x, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_3half(const double x);


/* Incomplete integral F_0(x,b) = ln(1 + e^(b-x)) - (b-x)
 *
 * exceptions: GSL_EUNDRFLW, GSL_EDOM
 */
int     gsl_sf_fermi_dirac_inc_0_e(const double x, const double b, gsl_sf_result * result);
double     gsl_sf_fermi_dirac_inc_0(const double x, const double b);


__END_DECLS

#endif /* __GSL_SF_FERMI_DIRAC_H__ */
