/* specfunc/exp.c
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
#include "gsl_sf_exp.h"

#include "gsl_specfunc__error.h"

/* Evaluate the continued fraction for exprel.
 * [Abramowitz+Stegun, 4.2.41]
 */
static
int
exprel_n_CF(const int N, const double x, gsl_sf_result * result)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 5000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = 1.0;
  double b1 = 1.0;
  double a2 = -x;
  double b2 = N+1;
  double an, bn;

  double fn;

  double An = b1*Anm1 + a1*Anm2;   /* A1 */
  double Bn = b1*Bnm1 + a1*Bnm2;   /* B1 */
  
  /* One explicit step, before we get to the main pattern. */
  n++;
  Anm2 = Anm1;
  Bnm2 = Bnm1;
  Anm1 = An;
  Bnm1 = Bn;
  An = b2*Anm1 + a2*Anm2;   /* A2 */
  Bn = b2*Bnm1 + a2*Bnm2;   /* B2 */

  fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = ( GSL_IS_ODD(n) ? ((n-1)/2)*x : -(N+(n/2)-1)*x );
    bn = N + n - 1;
    An = bn*Anm1 + an*Anm2;
    Bn = bn*Bnm1 + an*Bnm2;

    if(fabs(An) > RECUR_BIG || fabs(Bn) > RECUR_BIG) {
      An /= RECUR_BIG;
      Bn /= RECUR_BIG;
      Anm1 /= RECUR_BIG;
      Bnm1 /= RECUR_BIG;
      Anm2 /= RECUR_BIG;
      Bnm2 /= RECUR_BIG;
    }

    old_fn = fn;
    fn = An/Bn;
    del = old_fn/fn;
    
    if(fabs(del - 1.0) < 2.0*GSL_DBL_EPSILON) break;
  }

  result->val = fn;
  result->err = 2.0*(n+1.0)*GSL_DBL_EPSILON*fabs(fn);

  if(n == maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_exp_e(const double x, gsl_sf_result * result)
{
  if(x > GSL_LOG_DBL_MAX) {
    OVERFLOW_ERROR(result);
  }
  else if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else {
    result->val = exp(x);
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}

int gsl_sf_exp_e10_e(const double x, gsl_sf_result_e10 * result)
{
  if(x > INT_MAX-1) {
    OVERFLOW_ERROR_E10(result);
  }
  else if(x < INT_MIN+1) {
    UNDERFLOW_ERROR_E10(result);
  }
  else {
    const int N = (int) floor(x/M_LN10);
    result->val = exp(x-N*M_LN10);
    result->err = 2.0 * (fabs(x)+1.0) * GSL_DBL_EPSILON * fabs(result->val);
    result->e10 = N;
    return GSL_SUCCESS;
  }
}


int gsl_sf_exp_mult_e(const double x, const double y, gsl_sf_result * result)
{
  const double ay  = fabs(y);

  if(y == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(   ( x < 0.5*GSL_LOG_DBL_MAX   &&   x > 0.5*GSL_LOG_DBL_MIN)
          && (ay < 0.8*GSL_SQRT_DBL_MAX  &&  ay > 1.2*GSL_SQRT_DBL_MIN)
    ) {
    const double ex = exp(x);
    result->val = y * ex;
    result->err = (2.0 + fabs(x)) * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    const double ly  = log(ay);
    const double lnr = x + ly;

    if(lnr > GSL_LOG_DBL_MAX - 0.01) {
      OVERFLOW_ERROR(result);
    }
    else if(lnr < GSL_LOG_DBL_MIN + 0.01) {
      UNDERFLOW_ERROR(result);
    }
    else {
      const double sy   = GSL_SIGN(y);
      const double M    = floor(x);
      const double N    = floor(ly);
      const double a    = x  - M;
      const double b    = ly - N;
      const double berr = 2.0 * GSL_DBL_EPSILON * (fabs(ly) + fabs(N));
      result->val  = sy * exp(M+N) * exp(a+b);
      result->err  = berr * fabs(result->val);
      result->err += 2.0 * GSL_DBL_EPSILON * (M + N + 1.0) * fabs(result->val);
      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_exp_mult_e10_e(const double x, const double y, gsl_sf_result_e10 * result)
{
  const double ay  = fabs(y);

  if(y == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    result->e10 = 0;
    return GSL_SUCCESS;
  }
  else if(   ( x < 0.5*GSL_LOG_DBL_MAX   &&   x > 0.5*GSL_LOG_DBL_MIN)
          && (ay < 0.8*GSL_SQRT_DBL_MAX  &&  ay > 1.2*GSL_SQRT_DBL_MIN)
    ) {
    const double ex = exp(x);
    result->val = y * ex;
    result->err = (2.0 + fabs(x)) * GSL_DBL_EPSILON * fabs(result->val);
    result->e10 = 0;
    return GSL_SUCCESS;
  }
  else {
    const double ly  = log(ay);
    const double l10_val = (x + ly)/M_LN10;

    if(l10_val > INT_MAX-1) {
      OVERFLOW_ERROR_E10(result);
    }
    else if(l10_val < INT_MIN+1) {
      UNDERFLOW_ERROR_E10(result);
    }
    else {
      const double sy  = GSL_SIGN(y);
      const int    N   = (int) floor(l10_val);
      const double arg_val = (l10_val - N) * M_LN10;
      const double arg_err = 2.0 * GSL_DBL_EPSILON * fabs(ly);

      result->val  = sy * exp(arg_val);
      result->err  = arg_err * fabs(result->val);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      result->e10 = N;

      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_exp_mult_err_e(const double x, const double dx,
                             const double y, const double dy,
                             gsl_sf_result * result)
{
  const double ay  = fabs(y);

  if(y == 0.0) {
    result->val = 0.0;
    result->err = fabs(dy * exp(x));
    return GSL_SUCCESS;
  }
  else if(   ( x < 0.5*GSL_LOG_DBL_MAX   &&   x > 0.5*GSL_LOG_DBL_MIN)
          && (ay < 0.8*GSL_SQRT_DBL_MAX  &&  ay > 1.2*GSL_SQRT_DBL_MIN)
    ) {
    double ex = exp(x);
    result->val  = y * ex;
    result->err  = ex * (fabs(dy) + fabs(y*dx));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    const double ly  = log(ay);
    const double lnr = x + ly;

    if(lnr > GSL_LOG_DBL_MAX - 0.01) {
      OVERFLOW_ERROR(result);
    }
    else if(lnr < GSL_LOG_DBL_MIN + 0.01) {
      UNDERFLOW_ERROR(result);
    }
    else {
      const double sy  = GSL_SIGN(y);
      const double M   = floor(x);
      const double N   = floor(ly);
      const double a   = x  - M;
      const double b   = ly - N;
      const double eMN = exp(M+N);
      const double eab = exp(a+b);
      result->val  = sy * eMN * eab;
      result->err  = eMN * eab * 2.0*GSL_DBL_EPSILON;
      result->err += eMN * eab * fabs(dy/y);
      result->err += eMN * eab * fabs(dx);
      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_exp_mult_err_e10_e(const double x, const double dx,
                             const double y, const double dy,
                             gsl_sf_result_e10 * result)
{
  const double ay  = fabs(y);

  if(y == 0.0) {
    result->val = 0.0;
    result->err = fabs(dy * exp(x));
    result->e10 = 0;
    return GSL_SUCCESS;
  }
  else if(   ( x < 0.5*GSL_LOG_DBL_MAX   &&   x > 0.5*GSL_LOG_DBL_MIN)
          && (ay < 0.8*GSL_SQRT_DBL_MAX  &&  ay > 1.2*GSL_SQRT_DBL_MIN)
    ) {
    const double ex = exp(x);
    result->val  = y * ex;
    result->err  = ex * (fabs(dy) + fabs(y*dx));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    result->e10 = 0;
    return GSL_SUCCESS;
  }
  else {
    const double ly  = log(ay);
    const double l10_val = (x + ly)/M_LN10;

    if(l10_val > INT_MAX-1) {
      OVERFLOW_ERROR_E10(result);
    }
    else if(l10_val < INT_MIN+1) {
      UNDERFLOW_ERROR_E10(result);
    }
    else {
      const double sy  = GSL_SIGN(y);
      const int    N   = (int) floor(l10_val);
      const double arg_val = (l10_val - N) * M_LN10;
      const double arg_err = dy/fabs(y) + dx + 2.0*GSL_DBL_EPSILON*fabs(arg_val);

      result->val  = sy * exp(arg_val);
      result->err  = arg_err * fabs(result->val);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      result->e10 = N;

      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_expm1_e(const double x, gsl_sf_result * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    result->val = -1.0;
    result->err = GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    result->val = exp(x) - 1.0;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    result->val = x * (1.0 + 0.5*x*(1.0 + x/3.0*(1.0 + 0.25*x*(1.0 + 0.2*x))));
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    result->val = exp(x) - 1.0;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}


int gsl_sf_exprel_e(const double x, gsl_sf_result * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    result->val = -1.0/x;
    result->err = GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    result->val = (exp(x) - 1.0)/x;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    result->val = (1.0 + 0.5*x*(1.0 + x/3.0*(1.0 + 0.25*x*(1.0 + 0.2*x))));
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    result->val = (exp(x) - 1.0)/x;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}


int gsl_sf_exprel_2_e(double x, gsl_sf_result * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    result->val = -2.0/x*(1.0 + 1.0/x);
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    result->val = 2.0*(exp(x) - 1.0 - x)/(x*x);
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    result->val = (1.0 + 1.0/3.0*x*(1.0 + 0.25*x*(1.0 + 0.2*x*(1.0 + 1.0/6.0*x))));
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    result->val = 2.0*(exp(x) - 1.0 - x)/(x*x);
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}


int
gsl_sf_exprel_n_e(const int N, const double x, gsl_sf_result * result)
{
  if(N < 0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(fabs(x) < GSL_ROOT3_DBL_EPSILON * N) {
    result->val = 1.0 + x/(N+1) * (1.0 + x/(N+2));
    result->err = 2.0 * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(N == 0) {
    return gsl_sf_exp_e(x, result);
  }
  else if(N == 1) {
    return gsl_sf_exprel_e(x, result);
  }
  else if(N == 2) {
    return gsl_sf_exprel_2_e(x, result);
  }
  else {
    if(x > N && (-x + N*(1.0 + log(x/N)) < GSL_LOG_DBL_EPSILON)) {
      /* x is much larger than n.
       * Ignore polynomial part, so
       * exprel_N(x) ~= e^x N!/x^N
       */
      gsl_sf_result lnf_N;
      double lnr_val;
      double lnr_err;
      double lnterm;
      gsl_sf_lnfact_e(N, &lnf_N);
      lnterm = N*log(x);
      lnr_val  = x + lnf_N.val - lnterm;
      lnr_err  = GSL_DBL_EPSILON * (fabs(x) + fabs(lnf_N.val) + fabs(lnterm));
      lnr_err += lnf_N.err;
      return gsl_sf_exp_err_e(lnr_val, lnr_err, result);
    }
    else if(x > N) {
      /* Write the identity
       *   exprel_n(x) = e^x n! / x^n (1 - Gamma[n,x]/Gamma[n])
       * then use the asymptotic expansion
       * Gamma[n,x] ~ x^(n-1) e^(-x) (1 + (n-1)/x + (n-1)(n-2)/x^2 + ...)
       */
      double ln_x = log(x);
      gsl_sf_result lnf_N;
      double lg_N;
      double lnpre_val;
      double lnpre_err;
      gsl_sf_lnfact_e(N, &lnf_N);    /* log(N!)       */
      lg_N  = lnf_N.val - log(N);       /* log(Gamma(N)) */
      lnpre_val  = x + lnf_N.val - N*ln_x;
      lnpre_err  = GSL_DBL_EPSILON * (fabs(x) + fabs(lnf_N.val) + fabs(N*ln_x));
      lnpre_err += lnf_N.err;
      if(lnpre_val < GSL_LOG_DBL_MAX - 5.0) {
        int stat_eG;
        gsl_sf_result bigG_ratio;
        gsl_sf_result pre;
        int stat_ex = gsl_sf_exp_err_e(lnpre_val, lnpre_err, &pre);
        double ln_bigG_ratio_pre = -x + (N-1)*ln_x - lg_N;
        double bigGsum = 1.0;
        double term = 1.0;
        int k;
        for(k=1; k<N; k++) {
          term *= (N-k)/x;
          bigGsum += term;
        }
        stat_eG = gsl_sf_exp_mult_e(ln_bigG_ratio_pre, bigGsum, &bigG_ratio);
        if(stat_eG == GSL_SUCCESS) {
          result->val  = pre.val * (1.0 - bigG_ratio.val);
          result->err  = pre.val * (2.0*GSL_DBL_EPSILON + bigG_ratio.err);
          result->err += pre.err * fabs(1.0 - bigG_ratio.val);
          result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
          return stat_ex;
        }
        else {
          result->val = 0.0;
          result->err = 0.0;
          return stat_eG;
        }
      }
      else {
        OVERFLOW_ERROR(result);
      }
    }
    else if(x > -10.0*N) {
      return exprel_n_CF(N, x, result);
    }
    else {
      /* x -> -Inf asymptotic:
       * exprel_n(x) ~ e^x n!/x^n - n/x (1 + (n-1)/x + (n-1)(n-2)/x + ...)
       *             ~ - n/x (1 + (n-1)/x + (n-1)(n-2)/x + ...)
       */
      double sum  = 1.0;
      double term = 1.0;
      int k;
      for(k=1; k<N; k++) {
        term *= (N-k)/x;
        sum  += term;
      }
      result->val = -N/x * sum;
      result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
  }
}


int
gsl_sf_exp_err_e(const double x, const double dx, gsl_sf_result * result)
{
  const double adx = fabs(dx);

  /* CHECK_POINTER(result) */

  if(x + adx > GSL_LOG_DBL_MAX) {
    OVERFLOW_ERROR(result);
  }
  else if(x - adx < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else {
    const double ex  = exp(x);
    const double edx = exp(adx);
    result->val  = ex;
    result->err  = ex * GSL_MAX_DBL(GSL_DBL_EPSILON, edx - 1.0/edx);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}


int
gsl_sf_exp_err_e10_e(const double x, const double dx, gsl_sf_result_e10 * result)
{
  const double adx = fabs(dx);

  /* CHECK_POINTER(result) */

  if(x + adx > INT_MAX - 1) {
    OVERFLOW_ERROR_E10(result);
  }
  else if(x - adx < INT_MIN + 1) {
    UNDERFLOW_ERROR_E10(result);
  }
  else {
    const int    N  = (int)floor(x/M_LN10);
    const double ex = exp(x-N*M_LN10);
    result->val = ex;
    result->err = ex * (2.0 * GSL_DBL_EPSILON * (fabs(x) + 1.0) + adx);
    result->e10 = N;
    return GSL_SUCCESS;
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_exp(const double x)
{
  EVAL_RESULT(gsl_sf_exp_e(x, &result));
}

double gsl_sf_exp_mult(const double x, const double y)
{
  EVAL_RESULT(gsl_sf_exp_mult_e(x, y, &result));
}

double gsl_sf_expm1(const double x)
{
  EVAL_RESULT(gsl_sf_expm1_e(x, &result));
}

double gsl_sf_exprel(const double x)
{
  EVAL_RESULT(gsl_sf_exprel_e(x, &result));
}

double gsl_sf_exprel_2(const double x)
{
  EVAL_RESULT(gsl_sf_exprel_2_e(x, &result));
}

double gsl_sf_exprel_n(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_exprel_n_e(n, x, &result));
}
