/* specfunc/gsl_sf_debye.h
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
/* augmented by D_5(x) and D_6(x) by Richard J. Mathar, 2005-11-08 */

#ifndef __GSL_SF_DEBYE_H__
#define __GSL_SF_DEBYE_H__

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


/* D_n(x) := n/x^n Integrate[t^n/(e^t - 1), {t,0,x}] */

/* D_1(x)
 *
 * exceptions: GSL_EDOM
 */
int     gsl_sf_debye_1_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_1(const double x);


/* D_2(x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_debye_2_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_2(const double x);


/* D_3(x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_debye_3_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_3(const double x);


/* D_4(x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_debye_4_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_4(const double x);

/* D_5(x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_debye_5_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_5(const double x);

/* D_6(x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_debye_6_e(const double x, gsl_sf_result * result);
double     gsl_sf_debye_6(const double x);


__END_DECLS

#endif /* __GSL_SF_DEBYE_H__ */
