/* specfunc/bessel_Yn.c
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
#include "gsl_sf_gamma.h"
#include "gsl_sf_psi.h"
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel.h"
#include "gsl_specfunc__bessel_amp_phase.h"
#include "gsl_specfunc__bessel_olver.h"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* assumes n >= 1 */
static int bessel_Yn_small_x(const int n, const double x, gsl_sf_result * result)
{
  int k;
  double y = 0.25 * x * x;
  double ln_x_2 = log(0.5*x);
  gsl_sf_result ln_nm1_fact;
  double k_term;
  double term1, sum1, ln_pre1;
  double term2, sum2, pre2;

  gsl_sf_lnfact_e((unsigned int)(n-1), &ln_nm1_fact);

  ln_pre1 = -n*ln_x_2 + ln_nm1_fact.val;
  if(ln_pre1 > GSL_LOG_DBL_MAX - 3.0) GSL_ERROR ("error", GSL_EOVRFLW);

  sum1 = 1.0;
  k_term = 1.0;
  for(k=1; k<=n-1; k++) {
    k_term *= y/(k * (n-k));
    sum1 += k_term;
  }
  term1 = -exp(ln_pre1) * sum1 / M_PI;
  
  pre2 = -exp(n*ln_x_2) / M_PI;
  if(fabs(pre2) > 0.0) {
    const int KMAX = 20;
    gsl_sf_result psi_n;
    gsl_sf_result npk_fact;
    double yk = 1.0;
    double k_fact  = 1.0;
    double psi_kp1 = -M_EULER;
    double psi_npkp1;
    gsl_sf_psi_int_e(n, &psi_n);
    gsl_sf_fact_e((unsigned int)n, &npk_fact);
    psi_npkp1 = psi_n.val + 1.0/n;
    sum2 = (psi_kp1 + psi_npkp1 - 2.0*ln_x_2)/npk_fact.val;
    for(k=1; k<KMAX; k++) {
      psi_kp1   += 1./k;
      psi_npkp1 += 1./(n+k);
      k_fact   *= k;
      npk_fact.val *= n+k;
      yk *= -y;
      k_term = yk*(psi_kp1 + psi_npkp1 - 2.0*ln_x_2)/(k_fact*npk_fact.val);
      sum2 += k_term;
    }
    term2 = pre2 * sum2;
  }
  else {
    term2 = 0.0;
  }

  result->val  = term1 + term2;
  result->err  = GSL_DBL_EPSILON * (fabs(ln_pre1)*fabs(term1) + fabs(term2));
  result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

  return GSL_SUCCESS;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/


int
gsl_sf_bessel_Yn_e(int n, const double x, gsl_sf_result * result)
{
  int sign = 1;

  if(n < 0) {
    /* reduce to case n >= 0 */
    n = -n;
    if(GSL_IS_ODD(n)) sign = -1;
  }

  /* CHECK_POINTER(result) */

  if(n == 0) {
    int status = gsl_sf_bessel_Y0_e(x, result);
    result->val *= sign;
    return status;
  }
  else if(n == 1) {
    int status = gsl_sf_bessel_Y1_e(x, result);
    result->val *= sign;
    return status;
  }
  else {
    if(x <= 0.0) {
      DOMAIN_ERROR(result);
    }
    if(x < 5.0) {
      int status = bessel_Yn_small_x(n, x, result);
      result->val *= sign;
      return status;
    }
    else if(GSL_ROOT3_DBL_EPSILON * x > (n*n + 1.0)) {
      int status = gsl_sf_bessel_Ynu_asympx_e((double)n, x, result);
      result->val *= sign;
      return status;
    }
    else if(n > 50) {
      int status = gsl_sf_bessel_Ynu_asymp_Olver_e((double)n, x, result);
      result->val *= sign;
      return status;
    }
    else {
      double two_over_x = 2.0/x;
      gsl_sf_result r_by;
      gsl_sf_result r_bym;
      int stat_1 = gsl_sf_bessel_Y1_e(x, &r_by);
      int stat_0 = gsl_sf_bessel_Y0_e(x, &r_bym);
      double bym = r_bym.val;
      double by  = r_by.val;
      double byp;
      int j;

      for(j=1; j<n; j++) { 
        byp = j*two_over_x*by - bym;
        bym = by;
        by  = byp;
      }
      result->val  = sign * by;
      result->err  = fabs(result->val) * (fabs(r_by.err/r_by.val) + fabs(r_bym.err/r_bym.val));
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

      return GSL_ERROR_SELECT_2(stat_1, stat_0);
    }
  }
}


int
gsl_sf_bessel_Yn_array(const int nmin, const int nmax, const double x, double * result_array)
{
  /* CHECK_POINTER(result_array) */

  if(nmin < 0 || nmax < nmin || x <= 0.0) {
    int j;
    for(j=0; j<=nmax-nmin; j++) result_array[j] = 0.0;
    GSL_ERROR ("error", GSL_EDOM);
  }
  else {
    gsl_sf_result r_Ynm1;
    gsl_sf_result r_Yn;
    int stat_nm1 = gsl_sf_bessel_Yn_e(nmin,   x, &r_Ynm1);
    int stat_n   = gsl_sf_bessel_Yn_e(nmin+1, x, &r_Yn);
    double Ynp1;
    double Yn   = r_Yn.val;
    double Ynm1 = r_Ynm1.val;
    int n;

    int stat = GSL_ERROR_SELECT_2(stat_nm1, stat_n);

    if(stat == GSL_SUCCESS) {
      for(n=nmin+1; n<=nmax+1; n++) {
        result_array[n-nmin-1] = Ynm1;
        Ynp1 = -Ynm1 + 2.0*n/x * Yn;
        Ynm1 = Yn;
        Yn   = Ynp1;
      }
    }
    else {
      for(n=nmin; n<=nmax; n++) {
        result_array[n-nmin] = 0.0;
      }
    }

    return stat;
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Yn(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Yn_e(n, x, &result));
}

