/* specfunc/gsl_sf_transport.h
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

#ifndef __GSL_SF_TRANSPORT_H__
#define __GSL_SF_TRANSPORT_H__

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


/* Transport function:
 *   J(n,x) := Integral[ t^n e^t /(e^t - 1)^2, {t,0,x}]
 */

/* J(2,x)
 *
 * exceptions: GSL_EDOM
 */
int     gsl_sf_transport_2_e(const double x, gsl_sf_result * result);
double     gsl_sf_transport_2(const double x);


/* J(3,x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_transport_3_e(const double x, gsl_sf_result * result);
double     gsl_sf_transport_3(const double x);


/* J(4,x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_transport_4_e(const double x, gsl_sf_result * result);
double     gsl_sf_transport_4(const double x);


/* J(5,x)
 *
 * exceptions: GSL_EDOM, GSL_EUNDRFLW
 */
int     gsl_sf_transport_5_e(const double x, gsl_sf_result * result);
double     gsl_sf_transport_5(const double x);


__END_DECLS

#endif /* __GSL_SF_TRANSPORT_H__ */
