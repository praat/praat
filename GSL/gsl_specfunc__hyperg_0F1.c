/* specfunc/hyperg_0F1.c
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

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_bessel.h"
#include "gsl_sf_hyperg.h"

#include "gsl_specfunc__error.h"

#define locEPS  (1000.0*GSL_DBL_EPSILON)


/* Evaluate bessel_I(nu, x), allowing nu < 0.
 * This is fine here because we do not not allow
 * nu to be a negative integer.
 * x > 0.
 */
static
int
hyperg_0F1_bessel_I(const double nu, const double x, gsl_sf_result * result)
{
  if(x > GSL_LOG_DBL_MAX) {
    OVERFLOW_ERROR(result);
  }

  if(nu < 0.0) { 
    const double anu = -nu;
    const double s   = 2.0/M_PI * sin(anu*M_PI);
    const double ex  = exp(x);
    gsl_sf_result I;
    gsl_sf_result K;
    int stat_I = gsl_sf_bessel_Inu_scaled_e(anu, x, &I);
    int stat_K = gsl_sf_bessel_Knu_scaled_e(anu, x, &K);
    result->val  = ex * I.val + s * (K.val / ex);
    result->err  = ex * I.err + fabs(s * K.err/ex);
    result->err += fabs(s * (K.val/ex)) * GSL_DBL_EPSILON * anu * M_PI;
    return GSL_ERROR_SELECT_2(stat_K, stat_I);
  }
  else {
    const double ex  = exp(x);
    gsl_sf_result I;
    int stat_I = gsl_sf_bessel_Inu_scaled_e(nu, x, &I);
    result->val = ex * I.val;
    result->err = ex * I.err + GSL_DBL_EPSILON * fabs(result->val);
    return stat_I;
  }
}


/* Evaluate bessel_J(nu, x), allowing nu < 0.
 * This is fine here because we do not not allow
 * nu to be a negative integer.
 * x > 0.
 */
static
int
hyperg_0F1_bessel_J(const double nu, const double x, gsl_sf_result * result)
{
  if(nu < 0.0) { 
    const double anu = -nu;
    const double s   = sin(anu*M_PI);
    const double c   = cos(anu*M_PI);
    gsl_sf_result J;
    gsl_sf_result Y;
    int stat_J = gsl_sf_bessel_Jnu_e(anu, x, &J);
    int stat_Y = gsl_sf_bessel_Ynu_e(anu, x, &Y);
    result->val  = c * J.val - s * Y.val;
    result->err  = fabs(c * J.err) + fabs(s * Y.err);
    result->err += fabs(anu * M_PI) * GSL_DBL_EPSILON * fabs(J.val + Y.val);
    return GSL_ERROR_SELECT_2(stat_Y, stat_J);
  }
  else {
    return gsl_sf_bessel_Jnu_e(nu, x, result);
  }
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_hyperg_0F1_e(double c, double x, gsl_sf_result * result)
{
  const double rintc = floor(c + 0.5);
  const int c_neg_integer = (c < 0.0 && fabs(c - rintc) < locEPS);

  /* CHECK_POINTER(result) */

  if(c == 0.0 || c_neg_integer) {
    DOMAIN_ERROR(result);
  }
  else if(x < 0.0) {
    gsl_sf_result Jcm1;
    gsl_sf_result lg_c;
    double sgn;
    int stat_g = gsl_sf_lngamma_sgn_e(c, &lg_c, &sgn);
    int stat_J = hyperg_0F1_bessel_J(c-1.0, 2.0*sqrt(-x), &Jcm1);
    if(stat_g != GSL_SUCCESS) {
      result->val = 0.0;
      result->err = 0.0;
      return stat_g;
    }
    else if(Jcm1.val == 0.0) {
      result->val = 0.0;
      result->err = 0.0;
      return stat_J;
    }
    else {
      const double tl = log(-x)*0.5*(1.0-c);
      double ln_pre_val = lg_c.val + tl;
      double ln_pre_err = lg_c.err + 2.0 * GSL_DBL_EPSILON * fabs(tl);
      return gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                      sgn*Jcm1.val, Jcm1.err,
                                      result);
    }
  }
  else if(x == 0.0) {
    result->val = 1.0;
    result->err = 1.0;
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result Icm1;
    gsl_sf_result lg_c;
    double sgn;
    int stat_g = gsl_sf_lngamma_sgn_e(c, &lg_c, &sgn);
    int stat_I = hyperg_0F1_bessel_I(c-1.0, 2.0*sqrt(x), &Icm1);
    if(stat_g != GSL_SUCCESS) {
      result->val = 0.0;
      result->err = 0.0;
      return stat_g;
    }
    else if(Icm1.val == 0.0) {
      result->val = 0.0;
      result->err = 0.0;
      return stat_I;
    }
    else {
      const double tl = log(x)*0.5*(1.0-c);
      const double ln_pre_val = lg_c.val + tl;
      const double ln_pre_err = lg_c.err + 2.0 * GSL_DBL_EPSILON * fabs(tl);
      return gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                      sgn*Icm1.val, Icm1.err,
                                      result);
    }
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_hyperg_0F1(const double c, const double x)
{
  EVAL_RESULT(gsl_sf_hyperg_0F1_e(c, x, &result));
}
