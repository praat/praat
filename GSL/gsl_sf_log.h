/* specfunc/gsl_sf_log.h
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

#ifndef __GSL_SF_LOG_H__
#define __GSL_SF_LOG_H__

#include "gsl_sf_result.h"




/* Provide a logarithm function with GSL semantics.
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_log_e(const double x, gsl_sf_result * result);
double gsl_sf_log(const double x);


/* Log(|x|)
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_log_abs_e(const double x, gsl_sf_result * result);
double gsl_sf_log_abs(const double x);


/* Complex Logarithm
 *   exp(lnr + I theta) = zr + I zi
 * Returns argument in [-pi,pi].
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_complex_log_e(const double zr, const double zi, gsl_sf_result * lnr, gsl_sf_result * theta);


/* Log(1 + x)
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_log_1plusx_e(const double x, gsl_sf_result * result);
double gsl_sf_log_1plusx(const double x);


/* Log(1 + x) - x
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_log_1plusx_mx_e(const double x, gsl_sf_result * result);
double gsl_sf_log_1plusx_mx(const double x);


#ifdef HAVE_INLINE
extern inline
int
gsl_sf_log_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    result->val = GSL_NAN;
    result->err = GSL_NAN;
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else {
    result->val = log(x);
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}
extern inline
int
gsl_sf_log_abs_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x == 0.0) {
    result->val = GSL_NAN;
    result->err = GSL_NAN;
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else {
    result->val = log(fabs(x));
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}
#endif /* HAVE_INLINE */



#endif /* __GSL_SF_LOG_H__ */
