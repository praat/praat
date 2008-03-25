/* specfunc/beta_inc.c
 * 
 * Copyright (C) 2007 Brian Gough
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
#include "gsl_sf_log.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_hyperg.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__check.h"

static double
isnegint (const double x) 
{
  return (x < 0) && (x == floor(x));
}

static
int
beta_cont_frac(
  const double a,
  const double b,
  const double x,
  gsl_sf_result * result
  )
{
  const unsigned int max_iter = 512;        /* control iterations      */
  const double cutoff = 2.0 * GSL_DBL_MIN;  /* control the zero cutoff */
  unsigned int iter_count = 0;
  double cf;

  /* standard initialization for continued fraction */
  double num_term = 1.0;
  double den_term = 1.0 - (a+b)*x/(a+1.0);
  if (fabs(den_term) < cutoff) den_term = cutoff;
  den_term = 1.0/den_term;
  cf = den_term;

  while(iter_count < max_iter) {
    const int k  = iter_count + 1;
    double coeff = k*(b-k)*x/(((a-1.0)+2*k)*(a+2*k));
    double delta_frac;

    /* first step */
    den_term = 1.0 + coeff*den_term;
    num_term = 1.0 + coeff/num_term;
    if(fabs(den_term) < cutoff) den_term = cutoff;
    if(fabs(num_term) < cutoff) num_term = cutoff;
    den_term  = 1.0/den_term;

    delta_frac = den_term * num_term;
    cf *= delta_frac;

    coeff = -(a+k)*(a+b+k)*x/((a+2*k)*(a+2*k+1.0));

    /* second step */
    den_term = 1.0 + coeff*den_term;
    num_term = 1.0 + coeff/num_term;
    if(fabs(den_term) < cutoff) den_term = cutoff;
    if(fabs(num_term) < cutoff) num_term = cutoff;
    den_term = 1.0/den_term;

    delta_frac = den_term*num_term;
    cf *= delta_frac;

    if(fabs(delta_frac-1.0) < 2.0*GSL_DBL_EPSILON) break;

    ++iter_count;
  }

  result->val = cf;
  result->err = iter_count * 4.0 * GSL_DBL_EPSILON * fabs(cf);

  if(iter_count >= max_iter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}



/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_beta_inc_e(
  const double a,
  const double b,
  const double x,
  gsl_sf_result * result
  )
{
  if(x < 0.0 || x > 1.0) {
    DOMAIN_ERROR(result);
  } else if (isnegint(a) || isnegint(b)) {
    DOMAIN_ERROR(result);
  } else if (isnegint(a+b)) { 
    DOMAIN_ERROR(result);
  } else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x == 1.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  } else if (a <= 0 || b <= 0) {
    gsl_sf_result f, beta;
    int stat;
    const int stat_f = gsl_sf_hyperg_2F1_e(a, 1-b, a+1, x, &f);
    const int stat_beta = gsl_sf_beta_e(a, b, &beta);
    double prefactor = (pow(x, a) / a);
    result->val = prefactor * f.val / beta.val;
    result->err = fabs(prefactor) * f.err/ fabs(beta.val) + fabs(result->val/beta.val) * beta.err;

    stat = GSL_ERROR_SELECT_2(stat_f, stat_beta);
    if(stat == GSL_SUCCESS) {
      CHECK_UNDERFLOW(result);
    }
    return stat;
  } else {
    gsl_sf_result ln_beta;
    gsl_sf_result ln_x;
    gsl_sf_result ln_1mx;
    gsl_sf_result prefactor;
    const int stat_ln_beta = gsl_sf_lnbeta_e(a, b, &ln_beta);
    const int stat_ln_1mx = gsl_sf_log_1plusx_e(-x, &ln_1mx);
    const int stat_ln_x = gsl_sf_log_e(x, &ln_x);
    const int stat_ln = GSL_ERROR_SELECT_3(stat_ln_beta, stat_ln_1mx, stat_ln_x);

    const double ln_pre_val = -ln_beta.val + a * ln_x.val + b * ln_1mx.val;
    const double ln_pre_err =  ln_beta.err + fabs(a*ln_x.err) + fabs(b*ln_1mx.err);
    const int stat_exp = gsl_sf_exp_err_e(ln_pre_val, ln_pre_err, &prefactor);

    if(stat_ln != GSL_SUCCESS) {
      result->val = 0.0;
      result->err = 0.0;
      GSL_ERROR ("error", GSL_ESANITY);
    }

    if(x < (a + 1.0)/(a+b+2.0)) {
      /* Apply continued fraction directly. */
      gsl_sf_result cf;
      const int stat_cf = beta_cont_frac(a, b, x, &cf);
      int stat;
      result->val = prefactor.val * cf.val / a;
      result->err = (fabs(prefactor.err * cf.val) + fabs(prefactor.val * cf.err))/a;

      stat = GSL_ERROR_SELECT_2(stat_exp, stat_cf);
      if(stat == GSL_SUCCESS) {
        CHECK_UNDERFLOW(result);
      }
      return stat;
    }
    else {
      /* Apply continued fraction after hypergeometric transformation. */
      gsl_sf_result cf;
      const int stat_cf = beta_cont_frac(b, a, 1.0-x, &cf);
      int stat;
      const double term = prefactor.val * cf.val / b;
      result->val  = 1.0 - term;
      result->err  = fabs(prefactor.err * cf.val)/b;
      result->err += fabs(prefactor.val * cf.err)/b;
      result->err += 2.0 * GSL_DBL_EPSILON * (1.0 + fabs(term));
      stat = GSL_ERROR_SELECT_2(stat_exp, stat_cf);
      if(stat == GSL_SUCCESS) {
        CHECK_UNDERFLOW(result);
      }
      return stat;
    }
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_beta_inc(const double a, const double b, const double x)
{
  EVAL_RESULT(gsl_sf_beta_inc_e(a, b, x, &result));
}
