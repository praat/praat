/* specfunc/gsl_sf_airy.h
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

#ifndef __GSL_SF_AIRY_H__
#define __GSL_SF_AIRY_H__

#include "gsl_mode.h"
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


/* Airy function Ai(x)
 *
 * exceptions: GSL_EUNDRFLW
 */
int gsl_sf_airy_Ai_e(const double x, const gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Ai(const double x, gsl_mode_t mode);


/* Airy function Bi(x)
 *
 * exceptions: GSL_EOVRFLW
 */
int gsl_sf_airy_Bi_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Bi(const double x, gsl_mode_t mode);


/* scaled Ai(x):
 *                     Ai(x)   x < 0
 *   exp(+2/3 x^{3/2}) Ai(x)   x > 0
 *
 * exceptions: none
 */
int gsl_sf_airy_Ai_scaled_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Ai_scaled(const double x, gsl_mode_t mode);


/* scaled Bi(x):
 *                     Bi(x)   x < 0
 *   exp(-2/3 x^{3/2}) Bi(x)   x > 0
 *
 * exceptions: none
 */
int gsl_sf_airy_Bi_scaled_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Bi_scaled(const double x, gsl_mode_t mode);


/* derivative Ai'(x)
 *
 * exceptions: GSL_EUNDRFLW
 */
int gsl_sf_airy_Ai_deriv_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Ai_deriv(const double x, gsl_mode_t mode);


/* derivative Bi'(x)
 *
 * exceptions: GSL_EOVRFLW
 */
int gsl_sf_airy_Bi_deriv_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Bi_deriv(const double x, gsl_mode_t mode);


/* scaled derivative Ai'(x):
 *                     Ai'(x)   x < 0
 *   exp(+2/3 x^{3/2}) Ai'(x)   x > 0
 *
 * exceptions: none
 */
int gsl_sf_airy_Ai_deriv_scaled_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Ai_deriv_scaled(const double x, gsl_mode_t mode);


/* scaled derivative:
 *                     Bi'(x)   x < 0
 *   exp(-2/3 x^{3/2}) Bi'(x)   x > 0
 *
 * exceptions: none
 */
int gsl_sf_airy_Bi_deriv_scaled_e(const double x, gsl_mode_t mode, gsl_sf_result * result);
double gsl_sf_airy_Bi_deriv_scaled(const double x, gsl_mode_t mode);


/* Zeros of Ai(x)
 */
int gsl_sf_airy_zero_Ai_e(unsigned int s, gsl_sf_result * result);
double gsl_sf_airy_zero_Ai(unsigned int s);


/* Zeros of Bi(x)
 */
int gsl_sf_airy_zero_Bi_e(unsigned int s, gsl_sf_result * result);
double gsl_sf_airy_zero_Bi(unsigned int s);


/* Zeros of Ai'(x)
 */
int gsl_sf_airy_zero_Ai_deriv_e(unsigned int s, gsl_sf_result * result);
double gsl_sf_airy_zero_Ai_deriv(unsigned int s);


/* Zeros of Bi'(x)
 */
int gsl_sf_airy_zero_Bi_deriv_e(unsigned int s, gsl_sf_result * result);
double gsl_sf_airy_zero_Bi_deriv(unsigned int s);


__END_DECLS

#endif /* __GSL_SF_AIRY_H__ */
