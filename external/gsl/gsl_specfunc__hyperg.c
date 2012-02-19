/* specfunc/hyperg.c
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

/* Miscellaneous implementations of use
 * for evaluation of hypergeometric functions.
 */
#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__hyperg.h"

#define SUM_LARGE  (1.0e-5*GSL_DBL_MAX)


int
gsl_sf_hyperg_1F1_series_e(const double a, const double b, const double x,
                              gsl_sf_result * result
                              )
{
  double an  = a;
  double bn  = b;
  double n   = 1.0;
  double del = 1.0;
  double abs_del = 1.0;
  double max_abs_del = 1.0;
  double sum_val = 1.0;
  double sum_err = 0.0;

  while(abs_del/fabs(sum_val) > 0.25*GSL_DBL_EPSILON) {
    double u, abs_u;

    if(bn == 0.0) {
      DOMAIN_ERROR(result);
    }

    if(an == 0.0) {
      result->val  = sum_val;
      result->err  = sum_err;
      result->err += 2.0 * GSL_DBL_EPSILON * n * fabs(sum_val);
      return GSL_SUCCESS;
    }

    if (n > 10000.0) {
      result->val  = sum_val;
      result->err  = sum_err;
      GSL_ERROR ("hypergeometric series failed to converge", GSL_EFAILED);
    }      

    u = x * (an/(bn*n));
    abs_u = fabs(u);
    if(abs_u > 1.0 && max_abs_del > GSL_DBL_MAX/abs_u) {
      result->val = sum_val;
      result->err = fabs(sum_val);
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }
    del *= u;
    sum_val += del;
    if(fabs(sum_val) > SUM_LARGE) {
      result->val = sum_val;
      result->err = fabs(sum_val);
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }

    abs_del = fabs(del);
    max_abs_del = GSL_MAX_DBL(abs_del, max_abs_del);
    sum_err += 2.0*GSL_DBL_EPSILON*abs_del;

    an += 1.0;
    bn += 1.0;
    n  += 1.0;
  }

  result->val  = sum_val;
  result->err  = sum_err;
  result->err += abs_del;
  result->err += 2.0 * GSL_DBL_EPSILON * n * fabs(sum_val);

  return GSL_SUCCESS;
}


int
gsl_sf_hyperg_1F1_large_b_e(const double a, const double b, const double x, gsl_sf_result * result)
{
  if(fabs(x/b) < 1.0) {
    const double u = x/b;
    const double v = 1.0/(1.0-u);
    const double pre = pow(v,a);
    const double uv  = u*v;
    const double uv2 = uv*uv;
    const double t1  = a*(a+1.0)/(2.0*b)*uv2;
    const double t2a = a*(a+1.0)/(24.0*b*b)*uv2;
    const double t2b = 12.0 + 16.0*(a+2.0)*uv + 3.0*(a+2.0)*(a+3.0)*uv2;
    const double t2  = t2a*t2b;
    result->val  = pre * (1.0 - t1 + t2);
    result->err  = pre * GSL_DBL_EPSILON * (1.0 + fabs(t1) + fabs(t2));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    DOMAIN_ERROR(result);
  }
}


int
gsl_sf_hyperg_U_large_b_e(const double a, const double b, const double x,
                             gsl_sf_result * result,
                             double * ln_multiplier
                             )
{
  double N   = floor(b);  /* b = N + eps */
  double eps = b - N;
  
  if(fabs(eps) < GSL_SQRT_DBL_EPSILON) {
    double lnpre_val;
    double lnpre_err;
    gsl_sf_result M;
    if(b > 1.0) {
      double tmp = (1.0-b)*log(x);
      gsl_sf_result lg_bm1;
      gsl_sf_result lg_a;
      gsl_sf_lngamma_e(b-1.0, &lg_bm1);
      gsl_sf_lngamma_e(a, &lg_a);
      lnpre_val = tmp + x + lg_bm1.val - lg_a.val;
      lnpre_err = lg_bm1.err + lg_a.err + GSL_DBL_EPSILON * (fabs(x) + fabs(tmp));
      gsl_sf_hyperg_1F1_large_b_e(1.0-a, 2.0-b, -x, &M);
    }
    else {
      gsl_sf_result lg_1mb;
      gsl_sf_result lg_1pamb;
      gsl_sf_lngamma_e(1.0-b, &lg_1mb);
      gsl_sf_lngamma_e(1.0+a-b, &lg_1pamb);
      lnpre_val = lg_1mb.val - lg_1pamb.val;
      lnpre_err = lg_1mb.err + lg_1pamb.err;
      gsl_sf_hyperg_1F1_large_b_e(a, b, x, &M);
    }

    if(lnpre_val > GSL_LOG_DBL_MAX-10.0) {
      result->val  = M.val;
      result->err  = M.err;
      *ln_multiplier = lnpre_val;
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }
    else {
      gsl_sf_result epre;
      int stat_e = gsl_sf_exp_err_e(lnpre_val, lnpre_err, &epre);
      result->val  = epre.val * M.val;
      result->err  = epre.val * M.err + epre.err * fabs(M.val);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      *ln_multiplier = 0.0;
      return stat_e;
    }
  }
  else {
    double omb_lnx = (1.0-b)*log(x);
    gsl_sf_result lg_1mb;    double sgn_1mb;
    gsl_sf_result lg_1pamb;  double sgn_1pamb;
    gsl_sf_result lg_bm1;    double sgn_bm1;
    gsl_sf_result lg_a;      double sgn_a;
    gsl_sf_result M1, M2;
    double lnpre1_val, lnpre2_val;
    double lnpre1_err, lnpre2_err;
    double sgpre1, sgpre2;
    gsl_sf_hyperg_1F1_large_b_e(    a,     b, x, &M1);
    gsl_sf_hyperg_1F1_large_b_e(1.0-a, 2.0-b, x, &M2);

    gsl_sf_lngamma_sgn_e(1.0-b,   &lg_1mb,   &sgn_1mb);
    gsl_sf_lngamma_sgn_e(1.0+a-b, &lg_1pamb, &sgn_1pamb);

    gsl_sf_lngamma_sgn_e(b-1.0, &lg_bm1, &sgn_bm1);
    gsl_sf_lngamma_sgn_e(a,     &lg_a,   &sgn_a);

    lnpre1_val = lg_1mb.val - lg_1pamb.val;
    lnpre1_err = lg_1mb.err + lg_1pamb.err;
    lnpre2_val = lg_bm1.val - lg_a.val - omb_lnx - x;
    lnpre2_err = lg_bm1.err + lg_a.err + GSL_DBL_EPSILON * (fabs(omb_lnx)+fabs(x));
    sgpre1 = sgn_1mb * sgn_1pamb;
    sgpre2 = sgn_bm1 * sgn_a;

    if(lnpre1_val > GSL_LOG_DBL_MAX-10.0 || lnpre2_val > GSL_LOG_DBL_MAX-10.0) {
      double max_lnpre_val = GSL_MAX(lnpre1_val,lnpre2_val);
      double max_lnpre_err = GSL_MAX(lnpre1_err,lnpre2_err);
      double lp1 = lnpre1_val - max_lnpre_val;
      double lp2 = lnpre2_val - max_lnpre_val;
      double t1  = sgpre1*exp(lp1);
      double t2  = sgpre2*exp(lp2);
      result->val  = t1*M1.val + t2*M2.val;
      result->err  = fabs(t1)*M1.err + fabs(t2)*M2.err;
      result->err += GSL_DBL_EPSILON * exp(max_lnpre_err) * (fabs(t1*M1.val) + fabs(t2*M2.val));
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      *ln_multiplier = max_lnpre_val;
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }
    else {
      double t1 = sgpre1*exp(lnpre1_val);
      double t2 = sgpre2*exp(lnpre2_val);
      result->val  = t1*M1.val + t2*M2.val;
      result->err  = fabs(t1) * M1.err + fabs(t2)*M2.err;
      result->err += GSL_DBL_EPSILON * (exp(lnpre1_err)*fabs(t1*M1.val) + exp(lnpre2_err)*fabs(t2*M2.val));
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      *ln_multiplier = 0.0;
      return GSL_SUCCESS;
    }
  }
}



/* [Carlson, p.109] says the error in truncating this asymptotic series
 * is less than the absolute value of the first neglected term.
 *
 * A termination argument is provided, so that the series will
 * be summed at most up to n=n_trunc. If n_trunc is set negative,
 * then the series is summed until it appears to start diverging.
 */
int
gsl_sf_hyperg_2F0_series_e(const double a, const double b, const double x,
                              int n_trunc,
                              gsl_sf_result * result
                              )
{
  const int maxiter = 2000;
  double an = a;
  double bn = b;  
  double n   = 1.0;
  double sum = 1.0;
  double del = 1.0;
  double abs_del = 1.0;
  double max_abs_del = 1.0;
  double last_abs_del = 1.0;
  
  while(abs_del/fabs(sum) > GSL_DBL_EPSILON && n < maxiter) {

    double u = an * (bn/n * x);
    double abs_u = fabs(u);

    if(abs_u > 1.0 && (max_abs_del > GSL_DBL_MAX/abs_u)) {
      result->val = sum;
      result->err = fabs(sum);
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }

    del *= u;
    sum += del;

    abs_del = fabs(del);

    if(abs_del > last_abs_del) break; /* series is probably starting to grow */

    last_abs_del = abs_del;
    max_abs_del  = GSL_MAX(abs_del, max_abs_del);

    an += 1.0;
    bn += 1.0;
    n  += 1.0;
    
    if(an == 0.0 || bn == 0.0) break;        /* series terminated */
    
    if(n_trunc >= 0 && n >= n_trunc) break;  /* reached requested timeout */
  }

  result->val = sum;
  result->err = GSL_DBL_EPSILON * n + abs_del;
  if(n >= maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}
