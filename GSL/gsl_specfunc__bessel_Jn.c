/* specfunc/bessel_Jn.c
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
#include "gsl_sf_pow_int.h"
#include "gsl_specfunc__bessel.h"
#include "gsl_specfunc__bessel_amp_phase.h"
#include "gsl_specfunc__bessel_olver.h"
#include "gsl_sf_bessel.h"



/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/


int gsl_sf_bessel_Jn_e(int n, double x, gsl_sf_result * result)
{
  int sign = 1;

  if(n < 0) {
    /* reduce to case n >= 0 */
    n = -n;
    if(GSL_IS_ODD(n)) sign = -sign;
  }  

  if(x < 0.0) {
    /* reduce to case x >= 0. */
    x = -x;
    if(GSL_IS_ODD(n)) sign = -sign;
  }

  /* CHECK_POINTER(result) */

  if(n == 0) {
    gsl_sf_result b0;
    int stat_J0 = gsl_sf_bessel_J0_e(x, &b0);
    result->val = sign * b0.val;
    result->err = b0.err;
    return stat_J0;
  }
  else if(n == 1) {
    gsl_sf_result b1;
    int stat_J1 = gsl_sf_bessel_J1_e(x, &b1);
    result->val = sign * b1.val;
    result->err = b1.err;
    return stat_J1;
  }
  else {
    if(x == 0.0) {
      result->val = 0.0;
      result->err = 0.0;
      return GSL_SUCCESS;
    }
    else if(x*x < 10.0*(n+1.0)*GSL_ROOT5_DBL_EPSILON) {
      gsl_sf_result b;
      int status = gsl_sf_bessel_IJ_taylor_e((double)n, x, -1, 50, GSL_DBL_EPSILON, &b);
      result->val  = sign * b.val;
      result->err  = b.err;
      result->err += GSL_DBL_EPSILON * fabs(result->val);
      return status;
    }
    else if(GSL_ROOT4_DBL_EPSILON * x > (n*n+1.0)) {
      int status = gsl_sf_bessel_Jnu_asympx_e((double)n, x, result);
      result->val *= sign;
      return status;
    }
    else if(n > 50) {
      int status = gsl_sf_bessel_Jnu_asymp_Olver_e((double)n, x, result);
      result->val *= sign;
      return status;
    }
    else if(x > 1000.0)
    {
      /* We need this to avoid feeding large x to CF1; note that
       * due to the above check, we know that n <= 50.
       */
      int status = gsl_sf_bessel_Jnu_asympx_e((double)n, x, result);
      result->val *= sign;
      return status;      
    }
    else {
      double ans;
      double err;
      double ratio;
      double sgn;
      int stat_b;
      int stat_CF1 = gsl_sf_bessel_J_CF1((double)n, x, &ratio, &sgn);

      /* backward recurrence */
      double Jkp1 = GSL_SQRT_DBL_MIN * ratio;
      double Jk   = GSL_SQRT_DBL_MIN;
      double Jkm1;
      int k;

      for(k=n; k>0; k--) {
        Jkm1 = 2.0*k/x * Jk - Jkp1;
        Jkp1 = Jk;
        Jk   = Jkm1;
      }

      if(fabs(Jkp1) > fabs(Jk)) {
        gsl_sf_result b1;
        stat_b = gsl_sf_bessel_J1_e(x, &b1);
        ans = b1.val/Jkp1 * GSL_SQRT_DBL_MIN;
        err = b1.err/Jkp1 * GSL_SQRT_DBL_MIN;
      }
      else {
        gsl_sf_result b0;
        stat_b = gsl_sf_bessel_J0_e(x, &b0);
        ans = b0.val/Jk * GSL_SQRT_DBL_MIN;
        err = b0.err/Jk * GSL_SQRT_DBL_MIN;
      }

      result->val = sign * ans;
      result->err = fabs(err);
      return GSL_ERROR_SELECT_2(stat_CF1, stat_b);
    }
  }
}


int
gsl_sf_bessel_Jn_array(int nmin, int nmax, double x, double * result_array)
{
  /* CHECK_POINTER(result_array) */

  if(nmin < 0 || nmax < nmin) {
    int n;
    for(n=nmax; n>=nmin; n--) {
      result_array[n-nmin] = 0.0;
    }
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else if(x == 0.0) {
    int n;
    for(n=nmax; n>=nmin; n--) {
      result_array[n-nmin] = 0.0;
    }
    if(nmin == 0) result_array[0] = 1.0;
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result r_Jnp1;
    gsl_sf_result r_Jn;
    int stat_np1 = gsl_sf_bessel_Jn_e(nmax+1, x, &r_Jnp1);
    int stat_n   = gsl_sf_bessel_Jn_e(nmax,   x, &r_Jn);
    int stat = GSL_ERROR_SELECT_2(stat_np1, stat_n);

    double Jnp1 = r_Jnp1.val;
    double Jn   = r_Jn.val;
    double Jnm1;
    int n;

    if(stat == GSL_SUCCESS) {
      for(n=nmax; n>=nmin; n--) {
        result_array[n-nmin] = Jn;
        Jnm1 = -Jnp1 + 2.0*n/x * Jn;
        Jnp1 = Jn;
        Jn   = Jnm1;
      }
    }
    else {
      for(n=nmax; n>=nmin; n--) {
        result_array[n-nmin] = 0.0;
      }
    }

    return stat;
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Jn(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Jn_e(n, x, &result));
}
