/* specfunc/gegenbauer.c
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
#include "gsl_sf_gegenbauer.h"

#include "gsl_specfunc__error.h"

/* See: [Thompson, Atlas for Computing Mathematical Functions] */


int
gsl_sf_gegenpoly_1_e(double lambda, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(lambda == 0.0) {
    result->val = 2.0*x;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    result->val = 2.0*lambda*x;
    result->err = 4.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}

int
gsl_sf_gegenpoly_2_e(double lambda, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(lambda == 0.0) {
    const double txx = 2.0*x*x;
    result->val  = -1.0 + txx;
    result->err  = 2.0 * GSL_DBL_EPSILON * fabs(txx);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    result->val = lambda*(-1.0 + 2.0*(1.0+lambda)*x*x);
    result->err = GSL_DBL_EPSILON * (2.0 * fabs(result->val) + fabs(lambda));
    return GSL_SUCCESS;
  }
}

int
gsl_sf_gegenpoly_3_e(double lambda, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(lambda == 0.0) {
    result->val = x*(-2.0 + 4.0/3.0*x*x);
    result->err = GSL_DBL_EPSILON * (2.0 * fabs(result->val) + fabs(x));
    return GSL_SUCCESS;
  }
  else {
    double c = 4.0 + lambda*(6.0 + 2.0*lambda);
    result->val = 2.0*lambda * x * ( -1.0 - lambda + c*x*x/3.0 );
    result->err = GSL_DBL_EPSILON * (2.0 * fabs(result->val) + fabs(lambda * x));
    return GSL_SUCCESS;
  }
}


int
gsl_sf_gegenpoly_n_e(int n, double lambda, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(lambda <= -0.5 || n < 0) {
    DOMAIN_ERROR(result);
  }
  else if(n == 0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(n == 1) {
    return gsl_sf_gegenpoly_1_e(lambda, x, result);
  }
  else if(n == 2) {
    return gsl_sf_gegenpoly_2_e(lambda, x, result);
  }
  else if(n == 3) {
    return gsl_sf_gegenpoly_3_e(lambda, x, result);
  }
  else {
    if(lambda == 0.0 && (x >= -1.0 || x <= 1.0)) {
      /* 2 T_n(x)/n */
      const double z = n * acos(x);
      result->val = 2.0 * cos(z) / n;
      result->err = 2.0 * GSL_DBL_EPSILON * fabs(z * result->val);
      return GSL_SUCCESS;
    }
    else {
      int k;
      gsl_sf_result g2;
      gsl_sf_result g3;
      int stat_g2 = gsl_sf_gegenpoly_2_e(lambda, x, &g2);
      int stat_g3 = gsl_sf_gegenpoly_3_e(lambda, x, &g3);
      int stat_g  = GSL_ERROR_SELECT_2(stat_g2, stat_g3);
      double gkm2 = g2.val;
      double gkm1 = g3.val;
      double gk = 0.0;
      for(k=4; k<=n; k++) {
        gk = (2.0*(k+lambda-1.0)*x*gkm1 - (k+2.0*lambda-2.0)*gkm2) / k;
        gkm2 = gkm1;
        gkm1 = gk;
      }
      result->val = gk;
      result->err = 2.0 * GSL_DBL_EPSILON * 0.5 * n * fabs(gk);
      return stat_g;
    }
  }
}


int
gsl_sf_gegenpoly_array(int nmax, double lambda, double x, double * result_array)
{
  int k;

  /* CHECK_POINTER(result_array) */

  if(lambda <= -0.5 || nmax < 0) {
    GSL_ERROR("domain error", GSL_EDOM);
  }

  /* n == 0 */
  result_array[0] = 1.0;
  if(nmax == 0) return GSL_SUCCESS;

  /* n == 1 */
  if(lambda == 0.0)
    result_array[1] = 2.0*x;
  else
    result_array[1] = 2.0*lambda*x;

  /* n <= nmax */
  for(k=2; k<=nmax; k++) {
    double term1 = 2.0*(k+lambda-1.0) * x * result_array[k-1];
    double term2 = (k+2.0*lambda-2.0)     * result_array[k-2];
    result_array[k] = (term1 - term2) / k;
  }
  
  return GSL_SUCCESS;
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_gegenpoly_1(double lambda, double x)
{
  EVAL_RESULT(gsl_sf_gegenpoly_1_e(lambda, x, &result));
}

double gsl_sf_gegenpoly_2(double lambda, double x)
{
  EVAL_RESULT(gsl_sf_gegenpoly_2_e(lambda, x, &result));
}

double gsl_sf_gegenpoly_3(double lambda, double x)
{
  EVAL_RESULT(gsl_sf_gegenpoly_3_e(lambda, x, &result));
}

double gsl_sf_gegenpoly_n(int n, double lambda, double x)
{
  EVAL_RESULT(gsl_sf_gegenpoly_n_e(n, lambda, x, &result));
}
