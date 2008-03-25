/* specfunc/bessel_Kn.c
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

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* [Abramowitz+Stegun, 9.6.11]
 * assumes n >= 1
 */
static
int
bessel_Kn_scaled_small_x(const int n, const double x, gsl_sf_result * result)
{
  int k;
  double y = 0.25 * x * x;
  double ln_x_2 = log(0.5*x);
  double ex = exp(x);
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
    k_term *= -y/(k * (n-k));
    sum1 += k_term;
  }
  term1 = 0.5 * exp(ln_pre1) * sum1;

  pre2 = 0.5 * exp(n*ln_x_2);
  if(pre2 > 0.0) {
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
      psi_kp1   += 1.0/k;
      psi_npkp1 += 1.0/(n+k);
      k_fact    *= k;
      npk_fact.val *= n+k;
      yk *= y;
      k_term = yk*(psi_kp1 + psi_npkp1 - 2.0*ln_x_2)/(k_fact*npk_fact.val);
      sum2 += k_term;
    }
    term2 = ( GSL_IS_ODD(n) ? -1.0 : 1.0 ) * pre2 * sum2;
  }
  else {
    term2 = 0.0;
  }

  result->val  = ex * (term1 + term2);
  result->err  = ex * GSL_DBL_EPSILON * (fabs(ln_pre1)*fabs(term1) + fabs(term2));
  result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

  return GSL_SUCCESS;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_Kn_scaled_e(int n, const double x, gsl_sf_result * result)
{
  n = abs(n); /* K(-n, z) = K(n, z) */

  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(n == 0) {
    return gsl_sf_bessel_K0_scaled_e(x, result);
  }
  else if(n == 1) {
    return gsl_sf_bessel_K1_scaled_e(x, result);
  }
  else if(x <= 5.0) {
    return bessel_Kn_scaled_small_x(n, x, result);
  }
  else if(GSL_ROOT3_DBL_EPSILON * x > 0.25 * (n*n + 1)) {
    return gsl_sf_bessel_Knu_scaled_asympx_e((double)n, x, result);
  }
  else if(GSL_MIN(0.29/(n*n), 0.5/(n*n + x*x)) < GSL_ROOT3_DBL_EPSILON) {
    return gsl_sf_bessel_Knu_scaled_asymp_unif_e((double)n, x, result);
  }
  else {
    /* Upward recurrence. [Gradshteyn + Ryzhik, 8.471.1] */
    double two_over_x = 2.0/x;
    gsl_sf_result r_b_jm1;
    gsl_sf_result r_b_j;
    int stat_0 = gsl_sf_bessel_K0_scaled_e(x, &r_b_jm1);
    int stat_1 = gsl_sf_bessel_K1_scaled_e(x, &r_b_j);
    double b_jm1 = r_b_jm1.val;
    double b_j   = r_b_j.val;
    double b_jp1;
    int j;

    for(j=1; j<n; j++) {
      b_jp1 = b_jm1 + j * two_over_x * b_j;
      b_jm1 = b_j;
      b_j   = b_jp1; 
    } 
    
    result->val  = b_j;
    result->err  = n * (fabs(b_j) * (fabs(r_b_jm1.err/r_b_jm1.val) + fabs(r_b_j.err/r_b_j.val)));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

    return GSL_ERROR_SELECT_2(stat_0, stat_1);
  }
}


int gsl_sf_bessel_Kn_e(const int n, const double x, gsl_sf_result * result)
{
  const int status = gsl_sf_bessel_Kn_scaled_e(n, x, result);
  const double ex = exp(-x);
  result->val *= ex;
  result->err *= ex;
  result->err += x * GSL_DBL_EPSILON * fabs(result->val);
  return status;
}


int gsl_sf_bessel_Kn_scaled_array(const int nmin, const int nmax, const double x, double * result_array)
{
  /* CHECK_POINTER(result_array) */

  if(nmin < 0 || nmax < nmin || x <= 0.0) {
    int j;
    for(j=0; j<=nmax-nmin; j++) result_array[j] = 0.0;
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else if(nmax == 0) {
    gsl_sf_result b;
    int stat = gsl_sf_bessel_K0_scaled_e(x, &b);
    result_array[0] = b.val;
    return stat;
  }
  else {
    double two_over_x = 2.0/x;
    gsl_sf_result r_Knm1;
    gsl_sf_result r_Kn;
    int stat_0 = gsl_sf_bessel_Kn_scaled_e(nmin,   x, &r_Knm1);
    int stat_1 = gsl_sf_bessel_Kn_scaled_e(nmin+1, x, &r_Kn);
    int stat = GSL_ERROR_SELECT_2(stat_0, stat_1);
    double Knp1;
    double Kn   = r_Kn.val;
    double Knm1 = r_Knm1.val;
    int n;

    for(n=nmin+1; n<=nmax+1; n++) {
      if(Knm1 < GSL_DBL_MAX) {
        result_array[n-1-nmin] = Knm1;
        Knp1 = Knm1 + n * two_over_x * Kn;
        Knm1 = Kn;
        Kn   = Knp1;
      }
      else {
        /* Overflow. Set the rest of the elements to
         * zero and bug out.
         * FIXME: Note: this relies on the convention
         * that the test x < DBL_MIN fails for x not
         * a number. This may be only an IEEE convention,
         * so the portability is unclear.
         */
        int j;
        for(j=n; j<=nmax+1; j++) result_array[j-1-nmin] = 0.0;
        GSL_ERROR ("overflow", GSL_EOVRFLW);
      }
    }

    return stat;
  }
}


int
gsl_sf_bessel_Kn_array(const int nmin, const int nmax, const double x, double * result_array)
{
  int status = gsl_sf_bessel_Kn_scaled_array(nmin, nmax, x, result_array);
  double ex = exp(-x);
  int i;
  for(i=0; i<=nmax-nmin; i++) result_array[i] *= ex;
  return status;
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Kn_scaled(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Kn_scaled_e(n, x, &result));
}

double gsl_sf_bessel_Kn(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Kn_e(n, x, &result));
}
