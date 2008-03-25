/* specfunc/bessel_In.c
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
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel.h"

/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/


int
gsl_sf_bessel_In_scaled_e(int n, const double x, gsl_sf_result * result)
{
  const double ax = fabs(x);

  n = abs(n);  /* I(-n, z) = I(n, z) */

  /* CHECK_POINTER(result) */

  if(n == 0) {
    return gsl_sf_bessel_I0_scaled_e(x, result);
  }
  else if(n == 1) {
    return gsl_sf_bessel_I1_scaled_e(x, result);
  }
  else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x*x < 10.0*(n+1.0)/M_E) {
    gsl_sf_result t;
    double ex   = exp(-ax);
    int stat_In = gsl_sf_bessel_IJ_taylor_e((double)n, ax, 1, 50, GSL_DBL_EPSILON, &t);
    result->val  = t.val * ex;
    result->err  = t.err * ex;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    if(x < 0.0 && GSL_IS_ODD(n)) result->val = -result->val;
    return stat_In;
  }
  else if(n < 150 && ax < 1e7) {
    gsl_sf_result I0_scaled;
    int stat_I0 = gsl_sf_bessel_I0_scaled_e(ax, &I0_scaled);
    double rat;
    int stat_CF1 = gsl_sf_bessel_I_CF1_ser((double)n, ax, &rat);
    double Ikp1 = rat * GSL_SQRT_DBL_MIN;
    double Ik   = GSL_SQRT_DBL_MIN;
    double Ikm1;
    int k;
    for(k=n; k >= 1; k--) {
      Ikm1 = Ikp1 + 2.0*k/ax * Ik;
      Ikp1 = Ik;
      Ik   = Ikm1;
    }
    result->val  = I0_scaled.val * (GSL_SQRT_DBL_MIN / Ik);
    result->err  = I0_scaled.err * (GSL_SQRT_DBL_MIN / Ik);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    if(x < 0.0 && GSL_IS_ODD(n)) result->val = -result->val;
    return GSL_ERROR_SELECT_2(stat_I0, stat_CF1);
  }
  else if( GSL_MIN( 0.29/(n*n), 0.5/(n*n + x*x) ) < 0.5*GSL_ROOT3_DBL_EPSILON) {
    int stat_as = gsl_sf_bessel_Inu_scaled_asymp_unif_e((double)n, ax, result);
    if(x < 0.0 && GSL_IS_ODD(n)) result->val = -result->val;
    return stat_as;
  }
  else {
    const int nhi = 2 + (int) (1.2 / GSL_ROOT6_DBL_EPSILON);
    gsl_sf_result r_Ikp1;
    gsl_sf_result r_Ik;
    int stat_a1 = gsl_sf_bessel_Inu_scaled_asymp_unif_e(nhi+1.0,     ax, &r_Ikp1);
    int stat_a2 = gsl_sf_bessel_Inu_scaled_asymp_unif_e((double)nhi, ax, &r_Ik);
    double Ikp1 = r_Ikp1.val;
    double Ik   = r_Ik.val;
    double Ikm1;
    int k;
    for(k=nhi; k > n; k--) {
      Ikm1 = Ikp1 + 2.0*k/ax * Ik;
      Ikp1 = Ik;
      Ik   = Ikm1;
    }
    result->val = Ik;
    result->err = Ik * (r_Ikp1.err/r_Ikp1.val + r_Ik.err/r_Ik.val);
    if(x < 0.0 && GSL_IS_ODD(n)) result->val = -result->val;
    return GSL_ERROR_SELECT_2(stat_a1, stat_a2);
  }
}


int
gsl_sf_bessel_In_scaled_array(const int nmin, const int nmax, const double x, double * result_array)
{
  /* CHECK_POINTER(result_array) */

  if(nmax < nmin || nmin < 0) {
    int j;
    for(j=0; j<=nmax-nmin; j++) result_array[j] = 0.0;
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else if(x == 0.0) {
    int j;
    for(j=0; j<=nmax-nmin; j++) result_array[j] = 0.0;
    if(nmin == 0) result_array[0] = 1.0;
    return GSL_SUCCESS;
  }
  else if(nmax == 0) {
    gsl_sf_result I0_scaled;
    int stat = gsl_sf_bessel_I0_scaled_e(x, &I0_scaled);
    result_array[0] = I0_scaled.val;
    return stat;
  }
  else {
    const double ax = fabs(x);
    const double two_over_x = 2.0/ax;

    /* starting values */
    gsl_sf_result r_Inp1;
    gsl_sf_result r_In;
    int stat_0 = gsl_sf_bessel_In_scaled_e(nmax+1, ax, &r_Inp1);
    int stat_1 = gsl_sf_bessel_In_scaled_e(nmax,   ax, &r_In);
    double Inp1 = r_Inp1.val;
    double In   = r_In.val;
    double Inm1;
    int n;

    for(n=nmax; n>=nmin; n--) {
      result_array[n-nmin] = In;
      Inm1 = Inp1 + n * two_over_x * In;
      Inp1 = In;
      In   = Inm1;
    }

    /* deal with signs */
    if(x < 0.0) {
      for(n=nmin; n<=nmax; n++) {
        if(GSL_IS_ODD(n)) result_array[n-nmin] = -result_array[n-nmin];
      }
    }

    return GSL_ERROR_SELECT_2(stat_0, stat_1);
  }
}


int
gsl_sf_bessel_In_e(const int n_in, const double x, gsl_sf_result * result)
{
  const double ax = fabs(x);
  const int n = abs(n_in);  /* I(-n, z) = I(n, z) */
  gsl_sf_result In_scaled;
  const int stat_In_scaled = gsl_sf_bessel_In_scaled_e(n, ax, &In_scaled);

  /* In_scaled is always less than 1,
   * so this overflow check is conservative.
   */
  if(ax > GSL_LOG_DBL_MAX - 1.0) {
    OVERFLOW_ERROR(result);
  }
  else {
    const double ex = exp(ax);
    result->val  = ex * In_scaled.val;
    result->err  = ex * In_scaled.err;
    result->err += ax * GSL_DBL_EPSILON * fabs(result->val);
    if(x < 0.0 && GSL_IS_ODD(n)) result->val = -result->val;
    return stat_In_scaled;
  }
}


int
gsl_sf_bessel_In_array(const int nmin, const int nmax, const double x, double * result_array)
{
  double ax = fabs(x);

  /* CHECK_POINTER(result_array) */

  if(ax > GSL_LOG_DBL_MAX - 1.0) {
    int j;
    for(j=0; j<=nmax-nmin; j++) result_array[j] = 0.0; /* FIXME: should be Inf */
    GSL_ERROR ("overflow", GSL_EOVRFLW);
  }
  else {
    int j;
    double eax = exp(ax);
    int status = gsl_sf_bessel_In_scaled_array(nmin, nmax, x, result_array);
    for(j=0; j<=nmax-nmin; j++) result_array[j] *= eax;
    return status;
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_In_scaled(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_In_scaled_e(n, x, &result));
}

double gsl_sf_bessel_In(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_In_e(n, x, &result));
}
