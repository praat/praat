/* specfunc/gsl_sf_erf.h
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

#ifndef __GSL_SF_ERF_H__
#define __GSL_SF_ERF_H__

#include "gsl_sf_result.h"




/* Complementary Error Function
 * erfc(x) := 2/Sqrt[Pi] Integrate[Exp[-t^2], {t,x,Infinity}]
 *
 * exceptions: none
 */
int gsl_sf_erfc_e(double x, gsl_sf_result * result);
double gsl_sf_erfc(double x);


/* Log Complementary Error Function
 *
 * exceptions: none
 */
int gsl_sf_log_erfc_e(double x, gsl_sf_result * result);
double gsl_sf_log_erfc(double x);


/* Error Function
 * erf(x) := 2/Sqrt[Pi] Integrate[Exp[-t^2], {t,0,x}]
 *
 * exceptions: none
 */
int gsl_sf_erf_e(double x, gsl_sf_result * result);
double gsl_sf_erf(double x);


/* Probability functions:
 * Z(x) :  Abramowitz+Stegun 26.2.1
 * Q(x) :  Abramowitz+Stegun 26.2.3
 *
 * exceptions: none
 */
int gsl_sf_erf_Z_e(double x, gsl_sf_result * result);
int gsl_sf_erf_Q_e(double x, gsl_sf_result * result);
double gsl_sf_erf_Z(double x);
double gsl_sf_erf_Q(double x);



#endif /* __GSL_SF_ERF_H__ */
