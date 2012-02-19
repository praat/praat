/* specfunc/bessel.c
 * 
 * Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003 Gerard Jungman
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
/* Miscellaneous support functions for Bessel function evaluations.
 */
#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_airy.h"
#include "gsl_sf_elementary.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_trig.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel_amp_phase.h"
#include "gsl_specfunc__bessel_temme.h"
#include "gsl_specfunc__bessel.h"

#define CubeRoot2_  1.25992104989487316476721060728



/* Debye functions [Abramowitz+Stegun, 9.3.9-10] */

inline static double 
debye_u1(const double * tpow)
{
  return (3.0*tpow[1] - 5.0*tpow[3])/24.0;
}

inline static double 
debye_u2(const double * tpow)
{
  return (81.0*tpow[2] - 462.0*tpow[4] + 385.0*tpow[6])/1152.0;
}

inline
static double debye_u3(const double * tpow)
{
  return (30375.0*tpow[3] - 369603.0*tpow[5] + 765765.0*tpow[7] - 425425.0*tpow[9])/414720.0;
}

inline
static double debye_u4(const double * tpow)
{
  return (4465125.0*tpow[4] - 94121676.0*tpow[6] + 349922430.0*tpow[8] - 
          446185740.0*tpow[10] + 185910725.0*tpow[12])/39813120.0;
}

inline
static double debye_u5(const double * tpow)
{
  return (1519035525.0*tpow[5]     - 49286948607.0*tpow[7] + 
          284499769554.0*tpow[9]   - 614135872350.0*tpow[11] + 
          566098157625.0*tpow[13]  - 188699385875.0*tpow[15])/6688604160.0;
}

#if 0
inline
static double debye_u6(const double * tpow)
{
  return (2757049477875.0*tpow[6] - 127577298354750.0*tpow[8] + 
          1050760774457901.0*tpow[10] - 3369032068261860.0*tpow[12] + 
          5104696716244125.0*tpow[14] - 3685299006138750.0*tpow[16] + 
          1023694168371875.0*tpow[18])/4815794995200.0;
}
#endif


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_bessel_IJ_taylor_e(const double nu, const double x,
                             const int sign,
                             const int kmax,
                             const double threshold,
                             gsl_sf_result * result
                             )
{
  /* CHECK_POINTER(result) */

  if(nu < 0.0 || x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    if(nu == 0.0) {
      result->val = 1.0;
      result->err = 0.0;
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
    }
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result prefactor;   /* (x/2)^nu / Gamma(nu+1) */
    gsl_sf_result sum;

    int stat_pre;
    int stat_sum;
    int stat_mul;

    if(nu == 0.0) {
      prefactor.val = 1.0;
      prefactor.err = 0.0;
      stat_pre = GSL_SUCCESS;
    }
    else if(nu < INT_MAX-1) {
      /* Separate the integer part and use
       * y^nu / Gamma(nu+1) = y^N /N! y^f / (N+1)_f,
       * to control the error.
       */
      const int    N = (int)floor(nu + 0.5);
      const double f = nu - N;
      gsl_sf_result poch_factor;
      gsl_sf_result tc_factor;
      const int stat_poch = gsl_sf_poch_e(N+1.0, f, &poch_factor);
      const int stat_tc   = gsl_sf_taylorcoeff_e(N, 0.5*x, &tc_factor);
      const double p = pow(0.5*x,f);
      prefactor.val  = tc_factor.val * p / poch_factor.val;
      prefactor.err  = tc_factor.err * p / poch_factor.val;
      prefactor.err += fabs(prefactor.val) / poch_factor.val * poch_factor.err;
      prefactor.err += 2.0 * GSL_DBL_EPSILON * fabs(prefactor.val);
      stat_pre = GSL_ERROR_SELECT_2(stat_tc, stat_poch);
    }
    else {
      gsl_sf_result lg;
      const int stat_lg = gsl_sf_lngamma_e(nu+1.0, &lg);
      const double term1  = nu*log(0.5*x);
      const double term2  = lg.val;
      const double ln_pre = term1 - term2;
      const double ln_pre_err = GSL_DBL_EPSILON * (fabs(term1)+fabs(term2)) + lg.err;
      const int stat_ex = gsl_sf_exp_err_e(ln_pre, ln_pre_err, &prefactor);
      stat_pre = GSL_ERROR_SELECT_2(stat_ex, stat_lg);
    }

    /* Evaluate the sum.
     * [Abramowitz+Stegun, 9.1.10]
     * [Abramowitz+Stegun, 9.6.7]
     */
    {
      const double y = sign * 0.25 * x*x;
      double sumk = 1.0;
      double term = 1.0;
      int k;

      for(k=1; k<=kmax; k++) {
        term *= y/((nu+k)*k);
        sumk += term;
        if(fabs(term/sumk) < threshold) break;
      }

      sum.val = sumk;
      sum.err = threshold * fabs(sumk);

      stat_sum = ( k >= kmax ? GSL_EMAXITER : GSL_SUCCESS );
    }

    stat_mul = gsl_sf_multiply_err_e(prefactor.val, prefactor.err,
                                        sum.val, sum.err,
                                        result);

    return GSL_ERROR_SELECT_3(stat_mul, stat_pre, stat_sum);
  }
}


/* x >> nu*nu+1
 * error ~ O( ((nu*nu+1)/x)^4 )
 *
 * empirical error analysis:
 *   choose  GSL_ROOT4_MACH_EPS * x > (nu*nu + 1)
 *
 * This is not especially useful. When the argument gets
 * large enough for this to apply, the cos() and sin()
 * start loosing digits. However, this seems inevitable
 * for this particular method.
 *
 * Wed Jun 25 14:39:38 MDT 2003 [GJ]
 * This function was inconsistent since the Q term did not
 * go to relative order eps^2. That's why the error estimate
 * originally given was screwy (it didn't make sense that the
 * "empirical" error was coming out O(eps^3)).
 * With Q to proper order, the error is O(eps^4).
 */
int
gsl_sf_bessel_Jnu_asympx_e(const double nu, const double x, gsl_sf_result * result)
{
  double mu   = 4.0*nu*nu;
  double mum1 = mu-1.0;
  double mum9 = mu-9.0;
  double mum25 = mu-25.0;
  double chi = x - (0.5*nu + 0.25)*M_PI;
  double P   = 1.0 - mum1*mum9/(128.0*x*x);
  double Q   = mum1/(8.0*x) * (1.0 - mum9*mum25/(384.0*x*x));
  double pre = sqrt(2.0/(M_PI*x));
  double c   = cos(chi);
  double s   = sin(chi);
  double r   = mu/x;
  result->val  = pre * (c*P - s*Q);
  result->err  = pre * GSL_DBL_EPSILON * (1.0 + fabs(x)) * (fabs(c*P) + fabs(s*Q));
  result->err += pre * fabs(0.1*r*r*r*r);
  return GSL_SUCCESS;
}


/* x >> nu*nu+1
 */
int
gsl_sf_bessel_Ynu_asympx_e(const double nu, const double x, gsl_sf_result * result)
{
  double ampl;
  double theta;
  double alpha = x;
  double beta  = -0.5*nu*M_PI;
  int stat_a = gsl_sf_bessel_asymp_Mnu_e(nu, x, &ampl);
  int stat_t = gsl_sf_bessel_asymp_thetanu_corr_e(nu, x, &theta);
  double sin_alpha = sin(alpha);
  double cos_alpha = cos(alpha);
  double sin_chi   = sin(beta + theta);
  double cos_chi   = cos(beta + theta);
  double sin_term     = sin_alpha * cos_chi + sin_chi * cos_alpha;
  double sin_term_mag = fabs(sin_alpha * cos_chi) + fabs(sin_chi * cos_alpha);
  result->val  = ampl * sin_term;
  result->err  = fabs(ampl) * GSL_DBL_EPSILON * sin_term_mag;
  result->err += fabs(result->val) * 2.0 * GSL_DBL_EPSILON;

  if(fabs(alpha) > 1.0/GSL_DBL_EPSILON) {
    result->err *= 0.5 * fabs(alpha);
  }
  else if(fabs(alpha) > 1.0/GSL_SQRT_DBL_EPSILON) {
    result->err *= 256.0 * fabs(alpha) * GSL_SQRT_DBL_EPSILON;
  }

  return GSL_ERROR_SELECT_2(stat_t, stat_a);
}


/* x >> nu*nu+1
 */
int
gsl_sf_bessel_Inu_scaled_asympx_e(const double nu, const double x, gsl_sf_result * result)
{
  double mu   = 4.0*nu*nu;
  double mum1 = mu-1.0;
  double mum9 = mu-9.0;
  double pre  = 1.0/sqrt(2.0*M_PI*x);
  double r    = mu/x;
  result->val = pre * (1.0 - mum1/(8.0*x) + mum1*mum9/(128.0*x*x));
  result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val) + pre * fabs(0.1*r*r*r);
  return GSL_SUCCESS;
}

/* x >> nu*nu+1
 */
int
gsl_sf_bessel_Knu_scaled_asympx_e(const double nu, const double x, gsl_sf_result * result)
{
  double mu   = 4.0*nu*nu;
  double mum1 = mu-1.0;
  double mum9 = mu-9.0;
  double pre  = sqrt(M_PI/(2.0*x));
  double r    = nu/x;
  result->val = pre * (1.0 + mum1/(8.0*x) + mum1*mum9/(128.0*x*x));
  result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val) + pre * fabs(0.1*r*r*r);
  return GSL_SUCCESS;
}


/* nu -> Inf; uniform in x > 0  [Abramowitz+Stegun, 9.7.7]
 *
 * error:
 *   The error has the form u_N(t)/nu^N  where  0 <= t <= 1.
 *   It is not hard to show that |u_N(t)| is small for such t.
 *   We have N=6 here, and |u_6(t)| < 0.025, so the error is clearly
 *   bounded by 0.025/nu^6. This gives the asymptotic bound on nu
 *   seen below as nu ~ 100. For general MACH_EPS it will be 
 *                     nu > 0.5 / MACH_EPS^(1/6)
 *   When t is small, the bound is even better because |u_N(t)| vanishes
 *   as t->0. In fact u_N(t) ~ C t^N as t->0, with C ~= 0.1.
 *   We write
 *                     err_N <= min(0.025, C(1/(1+(x/nu)^2))^3) / nu^6
 *   therefore
 *                     min(0.29/nu^2, 0.5/(nu^2+x^2)) < MACH_EPS^{1/3}
 *   and this is the general form.
 *
 * empirical error analysis, assuming 14 digit requirement:
 *   choose   x > 50.000 nu   ==>  nu >   3
 *   choose   x > 10.000 nu   ==>  nu >  15
 *   choose   x >  2.000 nu   ==>  nu >  50
 *   choose   x >  1.000 nu   ==>  nu >  75
 *   choose   x >  0.500 nu   ==>  nu >  80
 *   choose   x >  0.100 nu   ==>  nu >  83
 *
 * This makes sense. For x << nu, the error will be of the form u_N(1)/nu^N,
 * since the polynomial term will be evaluated near t=1, so the bound
 * on nu will become constant for small x. Furthermore, increasing x with
 * nu fixed will decrease the error.
 */
int
gsl_sf_bessel_Inu_scaled_asymp_unif_e(const double nu, const double x, gsl_sf_result * result)
{
  int i;
  double z = x/nu;
  double root_term = hypot(1.0,z);
  double pre = 1.0/sqrt(2.0*M_PI*nu * root_term);
  double eta = root_term + log(z/(1.0+root_term));
  double ex_arg = ( z < 1.0/GSL_ROOT3_DBL_EPSILON ? nu*(-z + eta) : -0.5*nu/z*(1.0 - 1.0/(12.0*z*z)) );
  gsl_sf_result ex_result;
  int stat_ex = gsl_sf_exp_e(ex_arg, &ex_result);
  if(stat_ex == GSL_SUCCESS) {
    double t = 1.0/root_term;
    double sum;
    double tpow[16];
    tpow[0] = 1.0;
    for(i=1; i<16; i++) tpow[i] = t * tpow[i-1];
    sum = 1.0 + debye_u1(tpow)/nu + debye_u2(tpow)/(nu*nu) + debye_u3(tpow)/(nu*nu*nu)
          + debye_u4(tpow)/(nu*nu*nu*nu) + debye_u5(tpow)/(nu*nu*nu*nu*nu);
    result->val  = pre * ex_result.val * sum;
    result->err  = pre * ex_result.val / (nu*nu*nu*nu*nu*nu);
    result->err += pre * ex_result.err * fabs(sum);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    result->val = 0.0;
    result->err = 0.0;
    return stat_ex;
  }
}


/* nu -> Inf; uniform in x > 0  [Abramowitz+Stegun, 9.7.8]
 *
 * error:
 *   identical to that above for Inu_scaled
 */
int
gsl_sf_bessel_Knu_scaled_asymp_unif_e(const double nu, const double x, gsl_sf_result * result)
{
  int i;
  double z = x/nu;
  double root_term = hypot(1.0,z);
  double pre = sqrt(M_PI/(2.0*nu*root_term));
  double eta = root_term + log(z/(1.0+root_term));
  double ex_arg = ( z < 1.0/GSL_ROOT3_DBL_EPSILON ? nu*(z - eta) : 0.5*nu/z*(1.0 + 1.0/(12.0*z*z)) );
  gsl_sf_result ex_result;
  int stat_ex = gsl_sf_exp_e(ex_arg, &ex_result);
  if(stat_ex == GSL_SUCCESS) {
    double t = 1.0/root_term;
    double sum;
    double tpow[16];
    tpow[0] = 1.0;
    for(i=1; i<16; i++) tpow[i] = t * tpow[i-1];
    sum = 1.0 - debye_u1(tpow)/nu + debye_u2(tpow)/(nu*nu) - debye_u3(tpow)/(nu*nu*nu)
          + debye_u4(tpow)/(nu*nu*nu*nu) - debye_u5(tpow)/(nu*nu*nu*nu*nu);
    result->val  = pre * ex_result.val * sum;
    result->err  = pre * ex_result.err * fabs(sum);
    result->err += pre * ex_result.val / (nu*nu*nu*nu*nu*nu);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    result->val = 0.0;
    result->err = 0.0;
    return stat_ex;
  }
}


/* Evaluate J_mu(x),J_{mu+1}(x) and Y_mu(x),Y_{mu+1}(x)  for |mu| < 1/2
 */
int
gsl_sf_bessel_JY_mu_restricted(const double mu, const double x,
                               gsl_sf_result * Jmu, gsl_sf_result * Jmup1,
                               gsl_sf_result * Ymu, gsl_sf_result * Ymup1)
{
  /* CHECK_POINTER(Jmu) */
  /* CHECK_POINTER(Jmup1) */
  /* CHECK_POINTER(Ymu) */
  /* CHECK_POINTER(Ymup1) */

  if(x < 0.0 || fabs(mu) > 0.5) {
    Jmu->val   = 0.0;
    Jmu->err   = 0.0;
    Jmup1->val = 0.0;
    Jmup1->err = 0.0;
    Ymu->val   = 0.0;
    Ymu->err   = 0.0;
    Ymup1->val = 0.0;
    Ymup1->err = 0.0;
    GSL_ERROR ("error", GSL_EDOM);
  }
  else if(x == 0.0) {
    if(mu == 0.0) {
      Jmu->val   = 1.0;
      Jmu->err   = 0.0;
    }
    else {
      Jmu->val   = 0.0;
      Jmu->err   = 0.0;
    }
    Jmup1->val = 0.0;
    Jmup1->err = 0.0;
    Ymu->val   = 0.0;
    Ymu->err   = 0.0;
    Ymup1->val = 0.0;
    Ymup1->err = 0.0;
    GSL_ERROR ("error", GSL_EDOM);
  }
  else {
    int stat_Y;
    int stat_J;

    if(x < 2.0) {
      /* Use Taylor series for J and the Temme series for Y.
       * The Taylor series for J requires nu > 0, so we shift
       * up one and use the recursion relation to get Jmu, in
       * case mu < 0.
       */
      gsl_sf_result Jmup2;
      int stat_J1 = gsl_sf_bessel_IJ_taylor_e(mu+1.0, x, -1, 100, GSL_DBL_EPSILON,  Jmup1);
      int stat_J2 = gsl_sf_bessel_IJ_taylor_e(mu+2.0, x, -1, 100, GSL_DBL_EPSILON, &Jmup2);
      double c = 2.0*(mu+1.0)/x;
      Jmu->val  = c * Jmup1->val - Jmup2.val;
      Jmu->err  = c * Jmup1->err + Jmup2.err;
      Jmu->err += 2.0 * GSL_DBL_EPSILON * fabs(Jmu->val);
      stat_J = GSL_ERROR_SELECT_2(stat_J1, stat_J2);
      stat_Y = gsl_sf_bessel_Y_temme(mu, x, Ymu, Ymup1);
      return GSL_ERROR_SELECT_2(stat_J, stat_Y);
    }
    else if(x < 1000.0) {
      double P, Q;
      double J_ratio;
      double J_sgn;
      const int stat_CF1 = gsl_sf_bessel_J_CF1(mu, x, &J_ratio, &J_sgn);
      const int stat_CF2 = gsl_sf_bessel_JY_steed_CF2(mu, x, &P, &Q);
      double Jprime_J_ratio = mu/x - J_ratio;
      double gamma = (P - Jprime_J_ratio)/Q;
      Jmu->val = J_sgn * sqrt(2.0/(M_PI*x) / (Q + gamma*(P-Jprime_J_ratio)));
      Jmu->err = 4.0 * GSL_DBL_EPSILON * fabs(Jmu->val);
      Jmup1->val = J_ratio * Jmu->val;
      Jmup1->err = fabs(J_ratio) * Jmu->err;
      Ymu->val = gamma * Jmu->val;
      Ymu->err = fabs(gamma) * Jmu->err;
      Ymup1->val = Ymu->val * (mu/x - P - Q/gamma);
      Ymup1->err = Ymu->err * fabs(mu/x - P - Q/gamma) + 4.0*GSL_DBL_EPSILON*fabs(Ymup1->val);
      return GSL_ERROR_SELECT_2(stat_CF1, stat_CF2);
    }
    else {
      /* Use asymptotics for large argument.
       */
      const int stat_J0 = gsl_sf_bessel_Jnu_asympx_e(mu,     x, Jmu);
      const int stat_J1 = gsl_sf_bessel_Jnu_asympx_e(mu+1.0, x, Jmup1);
      const int stat_Y0 = gsl_sf_bessel_Ynu_asympx_e(mu,     x, Ymu);
      const int stat_Y1 = gsl_sf_bessel_Ynu_asympx_e(mu+1.0, x, Ymup1);
      stat_J = GSL_ERROR_SELECT_2(stat_J0, stat_J1);
      stat_Y = GSL_ERROR_SELECT_2(stat_Y0, stat_Y1);
      return GSL_ERROR_SELECT_2(stat_J, stat_Y);
    }
  }
}


int
gsl_sf_bessel_J_CF1(const double nu, const double x,
                    double * ratio, double * sgn)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 10000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = x/(2.0*(nu+1.0));
  double An = Anm1 + a1*Anm2;
  double Bn = Bnm1 + a1*Bnm2;
  double an;
  double fn = An/Bn;
  double dn = a1;
  double s  = 1.0;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = -x*x/(4.0*(nu+n-1.0)*(nu+n));
    An = Anm1 + an*Anm2;
    Bn = Bnm1 + an*Bnm2;

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

    dn = 1.0 / (2.0*(nu+n)/x - dn);
    if(dn < 0.0) s = -s;

    if(fabs(del - 1.0) < 2.0*GSL_DBL_EPSILON) break;
  }

  *ratio = fn;
  *sgn   = s;

  if(n >= maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}



/* Evaluate the continued fraction CF1 for J_{nu+1}/J_nu
 * using Gautschi (Euler) equivalent series.
 * This exhibits an annoying problem because the
 * a_k are not positive definite (in fact they are all negative).
 * There are cases when rho_k blows up. Example: nu=1,x=4.
 */
#if 0
int
gsl_sf_bessel_J_CF1_ser(const double nu, const double x,
                        double * ratio, double * sgn)
{
  const int maxk = 20000;
  double tk   = 1.0;
  double sum  = 1.0;
  double rhok = 0.0;
  double dk = 0.0;
  double s  = 1.0;
  int k;

  for(k=1; k<maxk; k++) {
    double ak = -0.25 * (x/(nu+k)) * x/(nu+k+1.0);
    rhok = -ak*(1.0 + rhok)/(1.0 + ak*(1.0 + rhok));
    tk  *= rhok;
    sum += tk;

    dk = 1.0 / (2.0/x - (nu+k-1.0)/(nu+k) * dk);
    if(dk < 0.0) s = -s;

    if(fabs(tk/sum) < GSL_DBL_EPSILON) break;
  }

  *ratio = x/(2.0*(nu+1.0)) * sum;
  *sgn   = s;

  if(k == maxk)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}
#endif


/* Evaluate the continued fraction CF1 for I_{nu+1}/I_nu
 * using Gautschi (Euler) equivalent series.
 */
int
gsl_sf_bessel_I_CF1_ser(const double nu, const double x, double * ratio)
{
  const int maxk = 20000;
  double tk   = 1.0;
  double sum  = 1.0;
  double rhok = 0.0;
  int k;

  for(k=1; k<maxk; k++) {
    double ak = 0.25 * (x/(nu+k)) * x/(nu+k+1.0);
    rhok = -ak*(1.0 + rhok)/(1.0 + ak*(1.0 + rhok));
    tk  *= rhok;
    sum += tk;
    if(fabs(tk/sum) < GSL_DBL_EPSILON) break;
  }

  *ratio = x/(2.0*(nu+1.0)) * sum;

  if(k == maxk)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}


int
gsl_sf_bessel_JY_steed_CF2(const double nu, const double x,
                           double * P, double * Q)
{
  const int max_iter = 10000;
  const double SMALL = 1.0e-100;

  int i = 1;

  double x_inv = 1.0/x;
  double a = 0.25 - nu*nu;
  double p = -0.5*x_inv;
  double q = 1.0;
  double br = 2.0*x;
  double bi = 2.0;
  double fact = a*x_inv/(p*p + q*q);
  double cr = br + q*fact;
  double ci = bi + p*fact;
  double den = br*br + bi*bi;
  double dr = br/den;
  double di = -bi/den;
  double dlr = cr*dr - ci*di;
  double dli = cr*di + ci*dr;
  double temp = p*dlr - q*dli;
  q = p*dli + q*dlr;
  p = temp;
  for (i=2; i<=max_iter; i++) {
    a  += 2*(i-1);
    bi += 2.0;
    dr = a*dr + br;
    di = a*di + bi;
    if(fabs(dr)+fabs(di) < SMALL) dr = SMALL;
    fact = a/(cr*cr+ci*ci);
    cr = br + cr*fact;
    ci = bi - ci*fact;
    if(fabs(cr)+fabs(ci) < SMALL) cr = SMALL;
    den = dr*dr + di*di;
    dr /= den;
    di /= -den;
    dlr = cr*dr - ci*di;
    dli = cr*di + ci*dr;
    temp = p*dlr - q*dli;
    q = p*dli + q*dlr;
    p = temp;
    if(fabs(dlr-1.0)+fabs(dli) < GSL_DBL_EPSILON) break;
  }

  *P = p;
  *Q = q;

  if(i == max_iter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}


/* Evaluate continued fraction CF2, using Thompson-Barnett-Temme method,
 * to obtain values of exp(x)*K_nu and exp(x)*K_{nu+1}.
 *
 * This is unstable for small x; x > 2 is a good cutoff.
 * Also requires |nu| < 1/2.
 */
int
gsl_sf_bessel_K_scaled_steed_temme_CF2(const double nu, const double x,
                                       double * K_nu, double * K_nup1,
                                       double * Kp_nu)
{
  const int maxiter = 10000;

  int i = 1;
  double bi = 2.0*(1.0 + x);
  double di = 1.0/bi;
  double delhi = di;
  double hi    = di;

  double qi   = 0.0;
  double qip1 = 1.0;

  double ai = -(0.25 - nu*nu);
  double a1 = ai;
  double ci = -ai;
  double Qi = -ai;

  double s = 1.0 + Qi*delhi;

  for(i=2; i<=maxiter; i++) {
    double dels;
    double tmp;
    ai -= 2.0*(i-1);
    ci  = -ai*ci/i;
    tmp  = (qi - bi*qip1)/ai;
    qi   = qip1;
    qip1 = tmp;
    Qi += ci*qip1;
    bi += 2.0;
    di  = 1.0/(bi + ai*di);
    delhi = (bi*di - 1.0) * delhi;
    hi += delhi;
    dels = Qi*delhi;
    s += dels;
    if(fabs(dels/s) < GSL_DBL_EPSILON) break;
  }
  
  hi *= -a1;
  
  *K_nu   = sqrt(M_PI/(2.0*x)) / s;
  *K_nup1 = *K_nu * (nu + x + 0.5 - hi)/x;
  *Kp_nu  = - *K_nup1 + nu/x * *K_nu;
  if(i == maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}


int gsl_sf_bessel_cos_pi4_e(double y, double eps, gsl_sf_result * result)
{
  const double sy = sin(y);
  const double cy = cos(y);
  const double s = sy + cy;
  const double d = sy - cy;
  const double abs_sum = fabs(cy) + fabs(sy);
  double seps;
  double ceps;
  if(fabs(eps) < GSL_ROOT5_DBL_EPSILON) {
    const double e2 = eps*eps;
    seps = eps * (1.0 - e2/6.0 * (1.0 - e2/20.0));
    ceps = 1.0 - e2/2.0 * (1.0 - e2/12.0);
  }
  else {
    seps = sin(eps);
    ceps = cos(eps);
  }
  result->val = (ceps * s - seps * d)/ M_SQRT2;
  result->err = 2.0 * GSL_DBL_EPSILON * (fabs(ceps) + fabs(seps)) * abs_sum / M_SQRT2;

  /* Try to account for error in evaluation of sin(y), cos(y).
   * This is a little sticky because we don't really know
   * how the library routines are doing their argument reduction.
   * However, we will make a reasonable guess.
   * FIXME ?
   */
  if(y > 1.0/GSL_DBL_EPSILON) {
    result->err *= 0.5 * y;
  }
  else if(y > 1.0/GSL_SQRT_DBL_EPSILON) {
    result->err *= 256.0 * y * GSL_SQRT_DBL_EPSILON;
  }

  return GSL_SUCCESS;
}


int gsl_sf_bessel_sin_pi4_e(double y, double eps, gsl_sf_result * result)
{
  const double sy = sin(y);
  const double cy = cos(y);
  const double s = sy + cy;
  const double d = sy - cy;
  const double abs_sum = fabs(cy) + fabs(sy);
  double seps;
  double ceps;
  if(fabs(eps) < GSL_ROOT5_DBL_EPSILON) {
    const double e2 = eps*eps;
    seps = eps * (1.0 - e2/6.0 * (1.0 - e2/20.0));
    ceps = 1.0 - e2/2.0 * (1.0 - e2/12.0);
  }
  else {
    seps = sin(eps);
    ceps = cos(eps);
  }
  result->val = (ceps * d + seps * s)/ M_SQRT2;
  result->err = 2.0 * GSL_DBL_EPSILON * (fabs(ceps) + fabs(seps)) * abs_sum / M_SQRT2;

  /* Try to account for error in evaluation of sin(y), cos(y).
   * See above.
   * FIXME ?
   */
  if(y > 1.0/GSL_DBL_EPSILON) {
    result->err *= 0.5 * y;
  }
  else if(y > 1.0/GSL_SQRT_DBL_EPSILON) {
    result->err *= 256.0 * y * GSL_SQRT_DBL_EPSILON;
  }

  return GSL_SUCCESS;
}


/************************************************************************
 *                                                                      *
  Asymptotic approximations 8.11.5, 8.12.5, and 8.42.7 from
  G.N.Watson, A Treatise on the Theory of Bessel Functions,
  2nd Edition (Cambridge University Press, 1944).
  Higher terms in expansion for x near l given by
  Airey in Phil. Mag. 31, 520 (1916).

  This approximation is accurate to near 0.1% at the boundaries
  between the asymptotic regions; well away from the boundaries
  the accuracy is better than 10^{-5}.
 *                                                                      *
 ************************************************************************/
#if 0
double besselJ_meissel(double nu, double x)
{
  double beta = pow(nu, 0.325);
  double result;

  /* Fitted matching points.   */
  double llimit = 1.1 * beta;
  double ulimit = 1.3 * beta;

  double nu2 = nu * nu;

  if (nu < 5. && x < 1.)
    {
      /* Small argument and order. Use a Taylor expansion. */
      int k;
      double xo2 = 0.5 * x;
      double gamfactor = pow(nu,nu) * exp(-nu) * sqrt(nu * 2. * M_PI)
        * (1. + 1./(12.*nu) + 1./(288.*nu*nu));
      double prefactor = pow(xo2, nu) / gamfactor;
      double C[5];

      C[0] = 1.;
      C[1] = -C[0] / (nu+1.);
      C[2] = -C[1] / (2.*(nu+2.));
      C[3] = -C[2] / (3.*(nu+3.));
      C[4] = -C[3] / (4.*(nu+4.));
      
      result = 0.;
      for(k=0; k<5; k++)
        result += C[k] * pow(xo2, 2.*k);

      result *= prefactor;
    }
  else if(x < nu - llimit)
    {
      /* Small x region: x << l.    */
      double z = x / nu;
      double z2 = z*z;
      double rtomz2 = sqrt(1.-z2);
      double omz2_2 = (1.-z2)*(1.-z2);

      /* Calculate Meissel exponent. */
      double term1 = 1./(24.*nu) * ((2.+3.*z2)/((1.-z2)*rtomz2) -2.);
      double term2 = - z2*(4. + z2)/(16.*nu2*(1.-z2)*omz2_2);
      double V_nu = term1 + term2;
      
      /* Calculate the harmless prefactor. */
      double sterlingsum = 1. + 1./(12.*nu) + 1./(288*nu2);
      double harmless = 1. / (sqrt(rtomz2*2.*M_PI*nu) * sterlingsum);

      /* Calculate the logarithm of the nu dependent prefactor. */
      double ln_nupre = rtomz2 + log(z) - log(1. + rtomz2);

      result = harmless * exp(nu*ln_nupre - V_nu);
    } 
  else if(x < nu + ulimit)
    {         
      /* Intermediate region 1: x near nu. */
      double eps = 1.-nu/x;
      double eps_x = eps * x;
      double eps_x_2 = eps_x * eps_x;
      double xo6 = x/6.;
      double B[6];
      static double gam[6] = {2.67894, 1.35412, 1., 0.89298, 0.902745, 1.};
      static double sf[6] = {0.866025, 0.866025, 0., -0.866025, -0.866025, 0.};
      
      /* Some terms are identically zero, because sf[] can be zero.
       * Some terms do not appear in the result.
       */
      B[0] = 1.;
      B[1] = eps_x;
      /* B[2] = 0.5 * eps_x_2 - 1./20.; */
      B[3] = eps_x * (eps_x_2/6. - 1./15.);
      B[4] = eps_x_2 * (eps_x_2 - 1.)/24. + 1./280.;
      /* B[5] = eps_x * (eps_x_2*(0.5*eps_x_2 - 1.)/60. + 43./8400.); */

      result  = B[0] * gam[0] * sf[0] / pow(xo6, 1./3.);
      result += B[1] * gam[1] * sf[1] / pow(xo6, 2./3.);
      result += B[3] * gam[3] * sf[3] / pow(xo6, 4./3.);
      result += B[4] * gam[4] * sf[4] / pow(xo6, 5./3.);

      result /= (3.*M_PI);
    }
  else 
    {
      /* Region of very large argument. Use expansion
       * for x>>l, and we need not be very exacting.
       */
      double secb = x/nu;
      double sec2b= secb*secb;
      
      double cotb = 1./sqrt(sec2b-1.);      /* cotb=cot(beta) */

      double beta = acos(nu/x);
      double trigarg = nu/cotb - nu*beta - 0.25 * M_PI;
      
      double cot3b = cotb * cotb * cotb;
      double cot6b = cot3b * cot3b;

      double sum1, sum2, expterm, prefactor, trigcos;

      sum1  = 2.0 + 3.0 * sec2b;
      trigarg -= sum1 * cot3b / (24.0 * nu);

      trigcos = cos(trigarg);

      sum2 = 4.0 + sec2b;
      expterm = sum2 * sec2b * cot6b / (16.0 * nu2);

      expterm = exp(-expterm);
      prefactor = sqrt(2. * cotb / (nu * M_PI));
      
      result = prefactor * expterm * trigcos;
    }

  return  result;
}
#endif
