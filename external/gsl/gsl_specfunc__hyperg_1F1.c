/* specfunc/hyperg_1F1.c
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
#include "gsl_sf_elementary.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_bessel.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_laguerre.h"
#include "gsl_sf_hyperg.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__hyperg.h"

#define _1F1_INT_THRESHOLD (100.0*GSL_DBL_EPSILON)


/* Asymptotic result for 1F1(a, b, x)  x -> -Infinity.
 * Assumes b-a != neg integer and b != neg integer.
 */
static
int
hyperg_1F1_asymp_negx(const double a, const double b, const double x,
                     gsl_sf_result * result)
{
  gsl_sf_result lg_b;
  gsl_sf_result lg_bma;
  double sgn_b;
  double sgn_bma;

  int stat_b   = gsl_sf_lngamma_sgn_e(b,   &lg_b,   &sgn_b);
  int stat_bma = gsl_sf_lngamma_sgn_e(b-a, &lg_bma, &sgn_bma);

  if(stat_b == GSL_SUCCESS && stat_bma == GSL_SUCCESS) {
    gsl_sf_result F;
    int stat_F = gsl_sf_hyperg_2F0_series_e(a, 1.0+a-b, -1.0/x, -1, &F);
    if(F.val != 0) {
      double ln_term_val = a*log(-x);
      double ln_term_err = 2.0 * GSL_DBL_EPSILON * (fabs(a) + fabs(ln_term_val));
      double ln_pre_val = lg_b.val - lg_bma.val - ln_term_val;
      double ln_pre_err = lg_b.err + lg_bma.err + ln_term_err;
      int stat_e = gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                            sgn_bma*sgn_b*F.val, F.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_F);
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
      return stat_F;
    }
  }
  else {
    DOMAIN_ERROR(result);
  }
}


/* Asymptotic result for 1F1(a, b, x)  x -> +Infinity
 * Assumes b != neg integer and a != neg integer
 */
static
int
hyperg_1F1_asymp_posx(const double a, const double b, const double x,
                      gsl_sf_result * result)
{
  gsl_sf_result lg_b;
  gsl_sf_result lg_a;
  double sgn_b;
  double sgn_a;

  int stat_b = gsl_sf_lngamma_sgn_e(b, &lg_b, &sgn_b);
  int stat_a = gsl_sf_lngamma_sgn_e(a, &lg_a, &sgn_a);

  if(stat_a == GSL_SUCCESS && stat_b == GSL_SUCCESS) {
    gsl_sf_result F;
    int stat_F = gsl_sf_hyperg_2F0_series_e(b-a, 1.0-a, 1.0/x, -1, &F);
    if(stat_F == GSL_SUCCESS && F.val != 0) {
      double lnx = log(x);
      double ln_term_val = (a-b)*lnx;
      double ln_term_err = 2.0 * GSL_DBL_EPSILON * (fabs(a) + fabs(b)) * fabs(lnx)
                         + 2.0 * GSL_DBL_EPSILON * fabs(a-b);
      double ln_pre_val = lg_b.val - lg_a.val + ln_term_val + x;
      double ln_pre_err = lg_b.err + lg_a.err + ln_term_err + 2.0 * GSL_DBL_EPSILON * fabs(x);
      int stat_e = gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                            sgn_a*sgn_b*F.val, F.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_F);
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
      return stat_F;
    }
  }
  else {
    DOMAIN_ERROR(result);
  }
}

/* Asymptotic result from Slater 4.3.7 
 * 
 * To get the general series, write M(a,b,x) as
 *
 *  M(a,b,x)=sum ((a)_n/(b)_n) (x^n / n!)
 *
 * and expand (b)_n in inverse powers of b as follows
 *
 * -log(1/(b)_n) = sum_(k=0)^(n-1) log(b+k)
 *             = n log(b) + sum_(k=0)^(n-1) log(1+k/b)
 *
 * Do a taylor expansion of the log in 1/b and sum the resulting terms
 * using the standard algebraic formulas for finite sums of powers of
 * k.  This should then give
 *
 * M(a,b,x) = sum_(n=0)^(inf) (a_n/n!) (x/b)^n * (1 - n(n-1)/(2b) 
 *                          + (n-1)n(n+1)(3n-2)/(24b^2) + ...
 *
 * which can be summed explicitly. The trick for summing it is to take
 * derivatives of sum_(i=0)^(inf) a_n*y^n/n! = (1-y)^(-a);
 *
 * [BJG 16/01/2007]
 */

static 
int
hyperg_1F1_largebx(const double a, const double b, const double x, gsl_sf_result * result)
{
  double y = x/b;
  double f = exp(-a*log1p(-y));
  double t1 = -((a*(a+1.0))/(2*b))*pow((y/(1.0-y)),2.0);
  double t2 = (1/(24*b*b))*((a*(a+1)*y*y)/pow(1-y,4))*(12+8*(2*a+1)*y+(3*a*a-a-2)*y*y);
  double t3 = (-1/(48*b*b*b*pow(1-y,6)))*a*((a + 1)*((y*((a + 1)*(a*(y*(y*((y*(a - 2) + 16)*(a - 1)) + 72)) + 96)) + 24)*pow(y, 2)));
  result->val = f * (1 + t1 + t2 + t3);
  result->err = 2*fabs(f*t3) + 2*GSL_DBL_EPSILON*fabs(result->val);
  return GSL_SUCCESS;
}
 
/* Asymptotic result for x < 2b-4a, 2b-4a large.
 * [Abramowitz+Stegun, 13.5.21]
 *
 * assumes 0 <= x/(2b-4a) <= 1
 */
static
int
hyperg_1F1_large2bm4a(const double a, const double b, const double x, gsl_sf_result * result)
{
  double eta    = 2.0*b - 4.0*a;
  double cos2th = x/eta;
  double sin2th = 1.0 - cos2th;
  double th = acos(sqrt(cos2th));
  double pre_h  = 0.25*M_PI*M_PI*eta*eta*cos2th*sin2th;
  gsl_sf_result lg_b;
  int stat_lg = gsl_sf_lngamma_e(b, &lg_b);
  double t1 = 0.5*(1.0-b)*log(0.25*x*eta);
  double t2 = 0.25*log(pre_h);
  double lnpre_val = lg_b.val + 0.5*x + t1 - t2;
  double lnpre_err = lg_b.err + 2.0 * GSL_DBL_EPSILON * (fabs(0.5*x) + fabs(t1) + fabs(t2));
#if SMALL_ANGLE
  const double eps = asin(sqrt(cos2th));  /* theta = pi/2 - eps */
  double s1 = (fmod(a, 1.0) == 0.0) ? 0.0 : sin(a*M_PI);
  double eta_reduc = (fmod(eta + 1, 4.0) == 0.0) ? 0.0 : fmod(eta + 1, 8.0);
  double phi1 = 0.25*eta_reduc*M_PI;
  double phi2 = 0.25*eta*(2*eps + sin(2.0*eps));
  double s2 = sin(phi1 - phi2);
#else
  double s1 = sin(a*M_PI);
  double s2 = sin(0.25*eta*(2.0*th - sin(2.0*th)) + 0.25*M_PI);
#endif
  double ser_val = s1 + s2;
  double ser_err = 2.0 * GSL_DBL_EPSILON * (fabs(s1) + fabs(s2));
  int stat_e = gsl_sf_exp_mult_err_e(lnpre_val, lnpre_err,
                                        ser_val, ser_err,
                                        result);
  return GSL_ERROR_SELECT_2(stat_e, stat_lg);
}


/* Luke's rational approximation.
 * See [Luke, Algorithms for the Computation of Mathematical Functions, p.182]
 *
 * Like the case of the 2F1 rational approximations, these are
 * probably guaranteed to converge for x < 0, barring gross
 * numerical instability in the pre-asymptotic regime.
 */
static
int
hyperg_1F1_luke(const double a, const double c, const double xin,
                gsl_sf_result * result)
{
  const double RECUR_BIG = 1.0e+50;
  const int nmax = 5000;
  int n = 3;
  const double x  = -xin;
  const double x3 = x*x*x;
  const double t0 = a/c;
  const double t1 = (a+1.0)/(2.0*c);
  const double t2 = (a+2.0)/(2.0*(c+1.0));
  double F = 1.0;
  double prec;

  double Bnm3 = 1.0;                                  /* B0 */
  double Bnm2 = 1.0 + t1 * x;                         /* B1 */
  double Bnm1 = 1.0 + t2 * x * (1.0 + t1/3.0 * x);    /* B2 */
 
  double Anm3 = 1.0;                                                      /* A0 */
  double Anm2 = Bnm2 - t0 * x;                                            /* A1 */
  double Anm1 = Bnm1 - t0*(1.0 + t2*x)*x + t0 * t1 * (c/(c+1.0)) * x*x;   /* A2 */

  while(1) {
    double npam1 = n + a - 1;
    double npcm1 = n + c - 1;
    double npam2 = n + a - 2;
    double npcm2 = n + c - 2;
    double tnm1  = 2*n - 1;
    double tnm3  = 2*n - 3;
    double tnm5  = 2*n - 5;
    double F1 =  (n-a-2) / (2*tnm3*npcm1);
    double F2 =  (n+a)*npam1 / (4*tnm1*tnm3*npcm2*npcm1);
    double F3 = -npam2*npam1*(n-a-2) / (8*tnm3*tnm3*tnm5*(n+c-3)*npcm2*npcm1);
    double E  = -npam1*(n-c-1) / (2*tnm3*npcm2*npcm1);

    double An = (1.0+F1*x)*Anm1 + (E + F2*x)*x*Anm2 + F3*x3*Anm3;
    double Bn = (1.0+F1*x)*Bnm1 + (E + F2*x)*x*Bnm2 + F3*x3*Bnm3;
    double r = An/Bn;

    prec = fabs((F - r)/F);
    F = r;

    if(prec < GSL_DBL_EPSILON || n > nmax) break;

    if(fabs(An) > RECUR_BIG || fabs(Bn) > RECUR_BIG) {
      An   /= RECUR_BIG;
      Bn   /= RECUR_BIG;
      Anm1 /= RECUR_BIG;
      Bnm1 /= RECUR_BIG;
      Anm2 /= RECUR_BIG;
      Bnm2 /= RECUR_BIG;
      Anm3 /= RECUR_BIG;
      Bnm3 /= RECUR_BIG;
    }
    else if(fabs(An) < 1.0/RECUR_BIG || fabs(Bn) < 1.0/RECUR_BIG) {
      An   *= RECUR_BIG;
      Bn   *= RECUR_BIG;
      Anm1 *= RECUR_BIG;
      Bnm1 *= RECUR_BIG;
      Anm2 *= RECUR_BIG;
      Bnm2 *= RECUR_BIG;
      Anm3 *= RECUR_BIG;
      Bnm3 *= RECUR_BIG;
    }

    n++;
    Bnm3 = Bnm2;
    Bnm2 = Bnm1;
    Bnm1 = Bn;
    Anm3 = Anm2;
    Anm2 = Anm1;
    Anm1 = An;
  }

  result->val  = F;
  result->err  = 2.0 * fabs(F * prec);
  result->err += 2.0 * GSL_DBL_EPSILON * (n-1.0) * fabs(F);

  return GSL_SUCCESS;
}

/* Series for 1F1(1,b,x)
 * b > 0
 */
static
int
hyperg_1F1_1_series(const double b, const double x, gsl_sf_result * result)
{
  double sum_val = 1.0;
  double sum_err = 0.0;
  double term = 1.0;
  double n    = 1.0;
  while(fabs(term/sum_val) > 0.25*GSL_DBL_EPSILON) {
    term *= x/(b+n-1);
    sum_val += term;
    sum_err += 8.0*GSL_DBL_EPSILON*fabs(term) + GSL_DBL_EPSILON*fabs(sum_val);
    n += 1.0;
  }
  result->val  = sum_val;
  result->err  = sum_err;
  result->err += 2.0 *  fabs(term);
  return GSL_SUCCESS;
}


/* 1F1(1,b,x)
 * b >= 1, b integer
 */
static
int
hyperg_1F1_1_int(const int b, const double x, gsl_sf_result * result)
{
  if(b < 1) {
    DOMAIN_ERROR(result);
  }
  else if(b == 1) {
    return gsl_sf_exp_e(x, result);
  }
  else if(b == 2) {
    return gsl_sf_exprel_e(x, result);
  }
  else if(b == 3) {
    return gsl_sf_exprel_2_e(x, result);
  }
  else {
    return gsl_sf_exprel_n_e(b-1, x, result);
  }
}


/* 1F1(1,b,x)
 * b >=1, b real
 *
 * checked OK: [GJ] Thu Oct  1 16:46:35 MDT 1998
 */
static
int
hyperg_1F1_1(const double b, const double x, gsl_sf_result * result)
{
  double ax = fabs(x);
  double ib = floor(b + 0.1);

  if(b < 1.0) {
    DOMAIN_ERROR(result);
  }
  else if(b == 1.0) {
    return gsl_sf_exp_e(x, result);
  }
  else if(b >= 1.4*ax) {
    return hyperg_1F1_1_series(b, x, result);
  }
  else if(fabs(b - ib) < _1F1_INT_THRESHOLD && ib < INT_MAX) {
    return hyperg_1F1_1_int((int)ib, x, result);
  }
  else if(x > 0.0) {
    if(x > 100.0 && b < 0.75*x) {
      return hyperg_1F1_asymp_posx(1.0, b, x, result);
    }
    else if(b < 1.0e+05) {
      /* Recurse backward on b, from a
       * chosen offset point. For x > 0,
       * which holds here, this should
       * be a stable direction.
       */
      const double off = ceil(1.4*x-b) + 1.0;
      double bp = b + off;
      gsl_sf_result M;
      int stat_s = hyperg_1F1_1_series(bp, x, &M);
      const double err_rat = M.err / fabs(M.val);
      while(bp > b+0.1) {
        /* M(1,b-1) = x/(b-1) M(1,b) + 1 */
        bp -= 1.0;
        M.val  = 1.0 + x/bp * M.val;
      }
      result->val  = M.val;
      result->err  = err_rat * fabs(M.val);
      result->err += 2.0 * GSL_DBL_EPSILON * (fabs(off)+1.0) * fabs(M.val);
      return stat_s;
    } else if (fabs(x) < fabs(b) && fabs(x) < sqrt(fabs(b)) * fabs(b-x)) {
      return hyperg_1F1_largebx(1.0, b, x, result);
    } else if (fabs(x) > fabs(b)) {
      return hyperg_1F1_1_series(b, x, result);
    } else {
      return hyperg_1F1_large2bm4a(1.0, b, x, result);
    }
  }
  else {
    /* x <= 0 and b not large compared to |x|
     */
    if(ax < 10.0 && b < 10.0) {
      return hyperg_1F1_1_series(b, x, result);
    }
    else if(ax >= 100.0 && GSL_MAX_DBL(fabs(2.0-b),1.0) < 0.99*ax) {
      return hyperg_1F1_asymp_negx(1.0, b, x, result);
    }
    else {
      return hyperg_1F1_luke(1.0, b, x, result);
    }
  }
}


/* 1F1(a,b,x)/Gamma(b) for b->0
 * [limit of Abramowitz+Stegun 13.3.7]
 */
static
int
hyperg_1F1_renorm_b0(const double a, const double x, gsl_sf_result * result)
{
  double eta = a*x;
  if(eta > 0.0) {
    double root_eta = sqrt(eta);
    gsl_sf_result I1_scaled;
    int stat_I = gsl_sf_bessel_I1_scaled_e(2.0*root_eta, &I1_scaled);
    if(I1_scaled.val <= 0.0) {
      result->val = 0.0;
      result->err = 0.0;
      return GSL_ERROR_SELECT_2(stat_I, GSL_EDOM);
    }
    else {
      /* Note that 13.3.7 contains higher terms which are zeroth order
         in b.  These make a non-negligible contribution to the sum.
         With the first correction term, the I1 above is replaced by
         I1 + (2/3)*a*(x/(4a))**(3/2)*I2(2*root_eta).  We will add
         this as part of the result and error estimate. */

      const double corr1 =(2.0/3.0)*a*pow(x/(4.0*a),1.5)*gsl_sf_bessel_In_scaled(2, 2.0*root_eta)
 ;
      const double lnr_val = 0.5*x + 0.5*log(eta) + fabs(2.0*root_eta) + log(I1_scaled.val+corr1);
      const double lnr_err = GSL_DBL_EPSILON * (1.5*fabs(x) + 1.0) + fabs((I1_scaled.err+corr1)/I1_scaled.val);
      return gsl_sf_exp_err_e(lnr_val, lnr_err, result);
    }
  }
  else if(eta == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    /* eta < 0 */
    double root_eta = sqrt(-eta);
    gsl_sf_result J1;
    int stat_J = gsl_sf_bessel_J1_e(2.0*root_eta, &J1);
    if(J1.val <= 0.0) {
      result->val = 0.0;
      result->err = 0.0;
      return GSL_ERROR_SELECT_2(stat_J, GSL_EDOM);
    }
    else {
      const double t1 = 0.5*x;
      const double t2 = 0.5*log(-eta);
      const double t3 = fabs(x);
      const double t4 = log(J1.val);
      const double lnr_val = t1 + t2 + t3 + t4;
      const double lnr_err = GSL_DBL_EPSILON * (1.5*fabs(x) + 1.0) + fabs(J1.err/J1.val);
      gsl_sf_result ex;
      int stat_e = gsl_sf_exp_err_e(lnr_val, lnr_err, &ex);
      result->val = -ex.val;
      result->err =  ex.err;
      return stat_e;
    }
  }
  
}


/* 1F1'(a,b,x)/1F1(a,b,x)
 * Uses Gautschi's version of the CF.
 * [Gautschi, Math. Comp. 31, 994 (1977)]
 *
 * Supposedly this suffers from the "anomalous convergence"
 * problem when b < x. I have seen anomalous convergence
 * in several of the continued fractions associated with
 * 1F1(a,b,x). This particular CF formulation seems stable
 * for b > x. However, it does display a painful artifact
 * of the anomalous convergence; the convergence plateaus
 * unless b >>> x. For example, even for b=1000, x=1, this
 * method locks onto a ratio which is only good to about
 * 4 digits. Apparently the rest of the digits are hiding
 * way out on the plateau, but finite-precision lossage
 * means you will never get them.
 */
#if 0
static
int
hyperg_1F1_CF1_p(const double a, const double b, const double x, double * result)
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
  double An = b1*Anm1 + a1*Anm2;
  double Bn = b1*Bnm1 + a1*Bnm2;
  double an, bn;
  double fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = (a+n)*x/((b-x+n-1)*(b-x+n));
    bn = 1.0;
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
    
    if(fabs(del - 1.0) < 10.0*GSL_DBL_EPSILON) break;
  }

  *result = a/(b-x) * fn;

  if(n == maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}
#endif /* 0 */


/* 1F1'(a,b,x)/1F1(a,b,x)
 * Uses Gautschi's series transformation of the
 * continued fraction. This is apparently the best
 * method for getting this ratio in the stable region.
 * The convergence is monotone and supergeometric
 * when b > x.
 * Assumes a >= -1.
 */
static
int
hyperg_1F1_CF1_p_ser(const double a, const double b, const double x, double * result)
{
  if(a == 0.0) {
    *result = 0.0;
    return GSL_SUCCESS;
  }
  else {
    const int maxiter = 5000;
    double sum  = 1.0;
    double pk   = 1.0;
    double rhok = 0.0;
    int k;
    for(k=1; k<maxiter; k++) {
      double ak = (a + k)*x/((b-x+k-1.0)*(b-x+k));
      rhok = -ak*(1.0 + rhok)/(1.0 + ak*(1.0+rhok));
      pk  *= rhok;
      sum += pk;
      if(fabs(pk/sum) < 2.0*GSL_DBL_EPSILON) break;
    }
    *result = a/(b-x) * sum;
    if(k == maxiter)
      GSL_ERROR ("error", GSL_EMAXITER);
    else
      return GSL_SUCCESS;
  }
}


/* 1F1(a+1,b,x)/1F1(a,b,x)
 *
 * I think this suffers from typical "anomalous convergence".
 * I could not find a region where it was truly useful.
 */
#if 0
static
int
hyperg_1F1_CF1(const double a, const double b, const double x, double * result)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 5000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = b - a - 1.0;
  double b1 = b - x - 2.0*(a+1.0);
  double An = b1*Anm1 + a1*Anm2;
  double Bn = b1*Bnm1 + a1*Bnm2;
  double an, bn;
  double fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = (a + n - 1.0) * (b - a - n);
    bn = b - x - 2.0*(a+n);
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
    
    if(fabs(del - 1.0) < 10.0*GSL_DBL_EPSILON) break;
  }

  *result = fn;
  if(n == maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}
#endif /* 0 */


/* 1F1(a,b+1,x)/1F1(a,b,x)
 *
 * This seemed to suffer from "anomalous convergence".
 * However, I have no theory for this recurrence.
 */
#if 0
static
int
hyperg_1F1_CF1_b(const double a, const double b, const double x, double * result)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 5000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = b + 1.0;
  double b1 = (b + 1.0) * (b - x);
  double An = b1*Anm1 + a1*Anm2;
  double Bn = b1*Bnm1 + a1*Bnm2;
  double an, bn;
  double fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = (b + n) * (b + n - 1.0 - a) * x;
    bn = (b + n) * (b + n - 1.0 - x);
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
    
    if(fabs(del - 1.0) < 10.0*GSL_DBL_EPSILON) break;
  }

  *result = fn;
  if(n == maxiter)
    GSL_ERROR ("error", GSL_EMAXITER);
  else
    return GSL_SUCCESS;
}
#endif /* 0 */


/* 1F1(a,b,x)
 * |a| <= 1, b > 0
 */
static
int
hyperg_1F1_small_a_bgt0(const double a, const double b, const double x, gsl_sf_result * result)
{
  const double bma = b-a;
  const double oma = 1.0-a;
  const double ap1mb = 1.0+a-b;
  const double abs_bma = fabs(bma);
  const double abs_oma = fabs(oma);
  const double abs_ap1mb = fabs(ap1mb);

  const double ax = fabs(x);

  if(a == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(a == 1.0 && b >= 1.0) {
    return hyperg_1F1_1(b, x, result);
  }
  else if(a == -1.0) {
    result->val  = 1.0 + a/b * x;
    result->err  = GSL_DBL_EPSILON * (1.0 + fabs(a/b * x));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(b >= 1.4*ax) {
    return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
  }
  else if(x > 0.0) {
    if(x > 100.0 && abs_bma*abs_oma < 0.5*x) {
      return hyperg_1F1_asymp_posx(a, b, x, result);
    }
    else if(b < 5.0e+06) {
      /* Recurse backward on b from
       * a suitably high point.
       */
      const double b_del = ceil(1.4*x-b) + 1.0;
      double bp = b + b_del;
      gsl_sf_result r_Mbp1;
      gsl_sf_result r_Mb;
      double Mbp1;
      double Mb;
      double Mbm1;
      int stat_0 = gsl_sf_hyperg_1F1_series_e(a, bp+1.0, x, &r_Mbp1);
      int stat_1 = gsl_sf_hyperg_1F1_series_e(a, bp,     x, &r_Mb);
      const double err_rat = fabs(r_Mbp1.err/r_Mbp1.val) + fabs(r_Mb.err/r_Mb.val);
      Mbp1 = r_Mbp1.val;
      Mb   = r_Mb.val;
      while(bp > b+0.1) {
        /* Do backward recursion. */
        Mbm1 = ((x+bp-1.0)*Mb - x*(bp-a)/bp*Mbp1)/(bp-1.0);
        bp -= 1.0;
        Mbp1 = Mb;
        Mb   = Mbm1;
      }
      result->val  = Mb;
      result->err  = err_rat * (fabs(b_del)+1.0) * fabs(Mb);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(Mb);
      return GSL_ERROR_SELECT_2(stat_0, stat_1);
    }
    else if (fabs(x) < fabs(b) && fabs(a*x) < sqrt(fabs(b)) * fabs(b-x)) {
      return hyperg_1F1_largebx(a, b, x, result);
    } else {
      return hyperg_1F1_large2bm4a(a, b, x, result);
    }
  }
  else {
    /* x < 0 and b not large compared to |x|
     */
    if(ax < 10.0 && b < 10.0) {
      return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
    }
    else if(ax >= 100.0 && GSL_MAX(abs_ap1mb,1.0) < 0.99*ax) {
      return hyperg_1F1_asymp_negx(a, b, x, result);
    }
    else {
      return hyperg_1F1_luke(a, b, x, result);
    }
  }
}


/* 1F1(b+eps,b,x)
 * |eps|<=1, b > 0
 */
static
int
hyperg_1F1_beps_bgt0(const double eps, const double b, const double x, gsl_sf_result * result)
{
  if(b > fabs(x) && fabs(eps) < GSL_SQRT_DBL_EPSILON) {
    /* If b-a is very small and x/b is not too large we can
     * use this explicit approximation.
     *
     * 1F1(b+eps,b,x) = exp(ax/b) (1 - eps x^2 (v2 + v3 x + ...) + ...)
     *
     *   v2 = a/(2b^2(b+1))
     *   v3 = a(b-2a)/(3b^3(b+1)(b+2))
     *   ...
     *
     * See [Luke, Mathematical Functions and Their Approximations, p.292]
     *
     * This cannot be used for b near a negative integer or zero.
     * Also, if x/b is large the deviation from exp(x) behaviour grows.
     */
    double a = b + eps;
    gsl_sf_result exab;
    int stat_e = gsl_sf_exp_e(a*x/b, &exab);
    double v2 = a/(2.0*b*b*(b+1.0));
    double v3 = a*(b-2.0*a)/(3.0*b*b*b*(b+1.0)*(b+2.0));
    double v  = v2 + v3 * x;
    double f  = (1.0 - eps*x*x*v);
    result->val  = exab.val * f;
    result->err  = exab.err * fabs(f);
    result->err += fabs(exab.val) * GSL_DBL_EPSILON * (1.0 + fabs(eps*x*x*v));
    result->err += 4.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_e;
  }
  else {
    /* Otherwise use a Kummer transformation to reduce
     * it to the small a case.
     */
    gsl_sf_result Kummer_1F1;
    int stat_K = hyperg_1F1_small_a_bgt0(-eps, b, -x, &Kummer_1F1);
    if(Kummer_1F1.val != 0.0) {
      int stat_e = gsl_sf_exp_mult_err_e(x, 2.0*GSL_DBL_EPSILON*fabs(x),
                                            Kummer_1F1.val, Kummer_1F1.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_K);
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
      return stat_K;
    }
  }
}


/* 1F1(a,2a,x) = Gamma(a + 1/2) E(x) (|x|/4)^(-a+1/2) scaled_I(a-1/2,|x|/2)
 *
 * E(x) = exp(x) x > 0
 *      = 1      x < 0
 *
 * a >= 1/2
 */
static
int
hyperg_1F1_beq2a_pos(const double a, const double x, gsl_sf_result * result)
{
  if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result I;
    int stat_I = gsl_sf_bessel_Inu_scaled_e(a-0.5, 0.5*fabs(x), &I);
    gsl_sf_result lg;
    int stat_g = gsl_sf_lngamma_e(a + 0.5, &lg);
    double ln_term   = (0.5-a)*log(0.25*fabs(x));
    double lnpre_val = lg.val + GSL_MAX_DBL(x,0.0) + ln_term;
    double lnpre_err = lg.err + GSL_DBL_EPSILON * (fabs(ln_term) + fabs(x));
    int stat_e = gsl_sf_exp_mult_err_e(lnpre_val, lnpre_err,
                                          I.val, I.err,
                                          result);
    return GSL_ERROR_SELECT_3(stat_e, stat_g, stat_I);
  }
}


/* Determine middle parts of diagonal recursion along b=2a
 * from two endpoints, i.e.
 *
 * given:  M(a,b)      and  M(a+1,b+2)
 * get:    M(a+1,b+1)  and  M(a,b+1)
 */
#if 0
inline
static
int
hyperg_1F1_diag_step(const double a, const double b, const double x,
                     const double Mab, const double Map1bp2,
                     double * Map1bp1, double * Mabp1)
{
  if(a == b) {
    *Map1bp1 = Mab;
    *Mabp1   = Mab - x/(b+1.0) * Map1bp2;
  }
  else {
    *Map1bp1 = Mab - x * (a-b)/(b*(b+1.0)) * Map1bp2;
    *Mabp1   = (a * *Map1bp1 - b * Mab)/(a-b);
  }
  return GSL_SUCCESS;
}
#endif /* 0 */


/* Determine endpoint of diagonal recursion.
 *
 * given:  M(a,b)    and  M(a+1,b+2)
 * get:    M(a+1,b)  and  M(a+1,b+1)
 */
#if 0
inline
static
int
hyperg_1F1_diag_end_step(const double a, const double b, const double x,
                         const double Mab, const double Map1bp2,
                         double * Map1b, double * Map1bp1)
{
  *Map1bp1 = Mab - x * (a-b)/(b*(b+1.0)) * Map1bp2;
  *Map1b   = Mab + x/b * *Map1bp1;
  return GSL_SUCCESS;
}
#endif /* 0 */


/* Handle the case of a and b both positive integers.
 * Assumes a > 0 and b > 0.
 */
static
int
hyperg_1F1_ab_posint(const int a, const int b, const double x, gsl_sf_result * result)
{
  double ax = fabs(x);

  if(a == b) {
    return gsl_sf_exp_e(x, result);             /* 1F1(a,a,x) */
  }
  else if(a == 1) {
    return gsl_sf_exprel_n_e(b-1, x, result);   /* 1F1(1,b,x) */
  }
  else if(b == a + 1) {
    gsl_sf_result K;
    int stat_K = gsl_sf_exprel_n_e(a, -x, &K);  /* 1F1(1,1+a,-x) */
    int stat_e = gsl_sf_exp_mult_err_e(x, 2.0 * GSL_DBL_EPSILON * fabs(x),
                                          K.val, K.err,
                                          result);
    return GSL_ERROR_SELECT_2(stat_e, stat_K);
  }
  else if(a == b + 1) {
    gsl_sf_result ex;
    int stat_e = gsl_sf_exp_e(x, &ex);
    result->val  = ex.val * (1.0 + x/b);
    result->err  = ex.err * (1.0 + x/b);
    result->err += ex.val * GSL_DBL_EPSILON * (1.0 + fabs(x/b));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_e;
  }
  else if(a == b + 2) {
    gsl_sf_result ex;
    int stat_e = gsl_sf_exp_e(x, &ex);
    double poly  = (1.0 + x/b*(2.0 + x/(b+1.0)));
    result->val  = ex.val * poly;
    result->err  = ex.err * fabs(poly);
    result->err += ex.val * GSL_DBL_EPSILON * (1.0 + fabs(x/b) * (2.0 + fabs(x/(b+1.0))));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_e;
  }
  else if(b == 2*a) {
    return hyperg_1F1_beq2a_pos(a, x, result);  /* 1F1(a,2a,x) */
  }
  else if(   ( b < 10 && a < 10 && ax < 5.0 )
          || ( b > a*ax )
          || ( b > a && ax < 5.0 )
    ) {
    return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
  }
  else if(b > a && b >= 2*a + x) {
    /* Use the Gautschi CF series, then
     * recurse backward to a=0 for normalization.
     * This will work for either sign of x.
     */
    double rap;
    int stat_CF1 = hyperg_1F1_CF1_p_ser(a, b, x, &rap);
    double ra = 1.0 + x/a * rap;
    double Ma   = GSL_SQRT_DBL_MIN;
    double Map1 = ra * Ma;
    double Mnp1 = Map1;
    double Mn   = Ma;
    double Mnm1;
    int n;
    for(n=a; n>0; n--) {
      Mnm1 = (n * Mnp1 - (2*n-b+x) * Mn) / (b-n);
      Mnp1 = Mn;
      Mn   = Mnm1;
    }
    result->val = Ma/Mn;
    result->err = 2.0 * GSL_DBL_EPSILON * (fabs(a) + 1.0) * fabs(Ma/Mn);
    return stat_CF1;
  }
  else if(b > a && b < 2*a + x && b > x) {
    /* Use the Gautschi series representation of
     * the continued fraction. Then recurse forward
     * to the a=b line for normalization. This will
     * work for either sign of x, although we do need
     * to check for b > x, for when x is positive.
     */
    double rap;
    int stat_CF1 = hyperg_1F1_CF1_p_ser(a, b, x, &rap);
    double ra = 1.0 + x/a * rap;
    gsl_sf_result ex;
    int stat_ex;

    double Ma   = GSL_SQRT_DBL_MIN;
    double Map1 = ra * Ma;
    double Mnm1 = Ma;
    double Mn   = Map1;
    double Mnp1;
    int n;
    for(n=a+1; n<b; n++) {
      Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
      Mnm1 = Mn;
      Mn   = Mnp1;
    }

    stat_ex = gsl_sf_exp_e(x, &ex);  /* 1F1(b,b,x) */
    result->val  = ex.val * Ma/Mn;
    result->err  = ex.err * fabs(Ma/Mn);
    result->err += 4.0 * GSL_DBL_EPSILON * (fabs(b-a)+1.0) * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_ex, stat_CF1);
  }
  else if(x >= 0.0) {

    if(b < a) {
      /* The point b,b is below the b=2a+x line.
       * Forward recursion on a from b,b+1 is possible.
       * Note that a > b + 1 as well, since we already tried a = b + 1.
       */
      if(x + log(fabs(x/b)) < GSL_LOG_DBL_MAX-2.0) {
        double ex = exp(x);
        int n;
        double Mnm1 = ex;                 /* 1F1(b,b,x)   */
        double Mn   = ex * (1.0 + x/b);   /* 1F1(b+1,b,x) */
        double Mnp1;
        for(n=b+1; n<a; n++) {
          Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
          Mnm1 = Mn;
          Mn   = Mnp1;
        }
        result->val  = Mn;
        result->err  = (x + 1.0) * GSL_DBL_EPSILON * fabs(Mn);
        result->err *= fabs(a-b)+1.0;
        return GSL_SUCCESS;
      }
      else {
        OVERFLOW_ERROR(result);
      }
    }
    else {
      /* b > a
       * b < 2a + x 
       * b <= x (otherwise we would have finished above)
       *
       * Gautschi anomalous convergence region. However, we can
       * recurse forward all the way from a=0,1 because we are
       * always underneath the b=2a+x line.
       */
      gsl_sf_result r_Mn;
      double Mnm1 = 1.0;    /* 1F1(0,b,x) */
      double Mn;            /* 1F1(1,b,x)  */
      double Mnp1;
      int n;
      gsl_sf_exprel_n_e(b-1, x, &r_Mn);
      Mn = r_Mn.val;
      for(n=1; n<a; n++) {
        Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
        Mnm1 = Mn;
        Mn   = Mnp1;
      }
      result->val  = Mn;
      result->err  = fabs(Mn) * (1.0 + fabs(a)) * fabs(r_Mn.err / r_Mn.val);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(Mn);
      return GSL_SUCCESS;
    }
  }
  else {
    /* x < 0
     * b < a (otherwise we would have tripped one of the above)
     */

    if(a <= 0.5*(b-x) || a >= -x) {
      /* Gautschi continued fraction is in the anomalous region,
       * so we must find another way. We recurse down in b,
       * from the a=b line.
       */
      double ex = exp(x);
      double Manp1 = ex;
      double Man   = ex * (1.0 + x/(a-1.0));
      double Manm1;
      int n;
      for(n=a-1; n>b; n--) {
        Manm1 = (-n*(1-n-x)*Man - x*(n-a)*Manp1)/(n*(n-1.0));
        Manp1 = Man;
        Man = Manm1;
      }
      result->val  = Man;
      result->err  = (fabs(x) + 1.0) * GSL_DBL_EPSILON * fabs(Man);
      result->err *= fabs(b-a)+1.0;
      return GSL_SUCCESS;
    }
    else {
      /* Pick a0 such that b ~= 2a0 + x, then
       * recurse down in b from a0,a0 to determine
       * the values near the line b=2a+x. Then recurse
       * forward on a from a0.
       */
      int a0 = ceil(0.5*(b-x));
      double Ma0b;    /* M(a0,b)   */
      double Ma0bp1;  /* M(a0,b+1) */
      double Ma0p1b;  /* M(a0+1,b) */
      double Mnm1;
      double Mn;
      double Mnp1;
      int n;
      {
        double ex = exp(x);
        double Ma0np1 = ex;
        double Ma0n   = ex * (1.0 + x/(a0-1.0));
        double Ma0nm1;
        for(n=a0-1; n>b; n--) {
          Ma0nm1 = (-n*(1-n-x)*Ma0n - x*(n-a0)*Ma0np1)/(n*(n-1.0));
          Ma0np1 = Ma0n;
          Ma0n = Ma0nm1;
        }
        Ma0bp1 = Ma0np1;
        Ma0b   = Ma0n;
        Ma0p1b = (b*(a0+x)*Ma0b + x*(a0-b)*Ma0bp1)/(a0*b);
      }

      /* Initialise the recurrence correctly BJG */

      if (a0 >= a)
        { 
          Mn = Ma0b;
        }
      else if (a0 + 1>= a)
        {
          Mn = Ma0p1b;
        }
      else
        {
          Mnm1 = Ma0b;
          Mn   = Ma0p1b;

          for(n=a0+1; n<a; n++) {
            Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
            Mnm1 = Mn;
            Mn   = Mnp1;
          }
        }

      result->val  = Mn;
      result->err  = (fabs(x) + 1.0) * GSL_DBL_EPSILON *  fabs(Mn);
      result->err *= fabs(b-a)+1.0;
      return GSL_SUCCESS;
    }
  }
}


/* Evaluate a <= 0, a integer, cases directly. (Polynomial; Horner)
 * When the terms are all positive, this
 * must work. We will assume this here.
 */
static
int
hyperg_1F1_a_negint_poly(const int a, const double b, const double x, gsl_sf_result * result)
{
  if(a == 0) {
    result->val = 1.0;
    result->err = 1.0;
    return GSL_SUCCESS;
  }
  else {
    int N = -a;
    double poly = 1.0;
    int k;
    for(k=N-1; k>=0; k--) {
      double t = (a+k)/(b+k) * (x/(k+1));
      double r = t + 1.0/poly;
      if(r > 0.9*GSL_DBL_MAX/poly) {
        OVERFLOW_ERROR(result);
      }
      else {
        poly *= r;  /* P_n = 1 + t_n P_{n-1} */
      }
    }
    result->val = poly;
    result->err = 2.0 * (sqrt(N) + 1.0) * GSL_DBL_EPSILON * fabs(poly);
    return GSL_SUCCESS;
  }
}


/* Evaluate negative integer a case by relation
 * to Laguerre polynomials. This is more general than
 * the direct polynomial evaluation, but is safe
 * for all values of x.
 *
 * 1F1(-n,b,x) = n!/(b)_n Laguerre[n,b-1,x]
 *             = n B(b,n) Laguerre[n,b-1,x]
 *
 * assumes b is not a negative integer
 */
static
int
hyperg_1F1_a_negint_lag(const int a, const double b, const double x, gsl_sf_result * result)
{
  const int n = -a;

  gsl_sf_result lag;
  const int stat_l = gsl_sf_laguerre_n_e(n, b-1.0, x, &lag);
  if(b < 0.0) {
    gsl_sf_result lnfact;
    gsl_sf_result lng1;
    gsl_sf_result lng2;
    double s1, s2;
    const int stat_f  = gsl_sf_lnfact_e(n, &lnfact);
    const int stat_g1 = gsl_sf_lngamma_sgn_e(b + n, &lng1, &s1);
    const int stat_g2 = gsl_sf_lngamma_sgn_e(b, &lng2, &s2);
    const double lnpre_val = lnfact.val - (lng1.val - lng2.val);
    const double lnpre_err = lnfact.err + lng1.err + lng2.err
      + 2.0 * GSL_DBL_EPSILON * fabs(lnpre_val);
    const int stat_e = gsl_sf_exp_mult_err_e(lnpre_val, lnpre_err,
                                                s1*s2*lag.val, lag.err,
                                                result);
    return GSL_ERROR_SELECT_5(stat_e, stat_l, stat_g1, stat_g2, stat_f);
  }
  else {
    gsl_sf_result lnbeta;
    gsl_sf_lnbeta_e(b, n, &lnbeta);
    if(fabs(lnbeta.val) < 0.1) {
      /* As we have noted, when B(x,y) is near 1,
       * evaluating log(B(x,y)) is not accurate.
       * Instead we evaluate B(x,y) directly.
       */
      const double ln_term_val = log(1.25*n);
      const double ln_term_err = 2.0 * GSL_DBL_EPSILON * ln_term_val;
      gsl_sf_result beta;
      int stat_b = gsl_sf_beta_e(b, n, &beta);
      int stat_e = gsl_sf_exp_mult_err_e(ln_term_val, ln_term_err,
                                            lag.val, lag.err,
                                            result);
      result->val *= beta.val/1.25;
      result->err *= beta.val/1.25;
      return GSL_ERROR_SELECT_3(stat_e, stat_l, stat_b);
    }
    else {
      /* B(x,y) was not near 1, so it is safe to use
       * the logarithmic values.
       */
      const double ln_n = log(n);
      const double ln_term_val = lnbeta.val + ln_n;
      const double ln_term_err = lnbeta.err + 2.0 * GSL_DBL_EPSILON * fabs(ln_n);
      int stat_e = gsl_sf_exp_mult_err_e(ln_term_val, ln_term_err,
                                            lag.val, lag.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_l);
    }
  }
}


/* Handle negative integer a case for x > 0 and
 * generic b.
 *
 * Combine [Abramowitz+Stegun, 13.6.9 + 13.6.27]
 * M(-n,b,x) = (-1)^n / (b)_n U(-n,b,x) = n! / (b)_n Laguerre^(b-1)_n(x)
 */
#if 0
static
int
hyperg_1F1_a_negint_U(const int a, const double b, const double x, gsl_sf_result * result)
{
  const int n = -a;
  const double sgn = ( GSL_IS_ODD(n) ? -1.0 : 1.0 );
  double sgpoch;
  gsl_sf_result lnpoch;
  gsl_sf_result U;
  const int stat_p = gsl_sf_lnpoch_sgn_e(b, n, &lnpoch, &sgpoch);
  const int stat_U = gsl_sf_hyperg_U_e(-n, b, x, &U);
  const int stat_e = gsl_sf_exp_mult_err_e(-lnpoch.val, lnpoch.err,
                                              sgn * sgpoch * U.val, U.err,
                                              result);
  return GSL_ERROR_SELECT_3(stat_e, stat_U, stat_p);
}
#endif


/* Assumes a <= -1,  b <= -1, and b <= a.
 */
static
int
hyperg_1F1_ab_negint(const int a, const int b, const double x, gsl_sf_result * result)
{
  if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x > 0.0) {
    return hyperg_1F1_a_negint_poly(a, b, x, result);
  }
  else {
    /* Apply a Kummer transformation to make x > 0 so
     * we can evaluate the polynomial safely. Of course,
     * this assumes b <= a, which must be true for
     * a<0 and b<0, since otherwise the thing is undefined.
     */
    gsl_sf_result K;
    int stat_K = hyperg_1F1_a_negint_poly(b-a, b, -x, &K);
    int stat_e = gsl_sf_exp_mult_err_e(x, 2.0 * GSL_DBL_EPSILON * fabs(x),
                                          K.val, K.err,
                                          result);
    return GSL_ERROR_SELECT_2(stat_e, stat_K);
  }
}


/* [Abramowitz+Stegun, 13.1.3]
 *
 * M(a,b,x) = Gamma(1+a-b)/Gamma(2-b) x^(1-b) *
 *            { Gamma(b)/Gamma(a) M(1+a-b,2-b,x) - (b-1) U(1+a-b,2-b,x) }
 *
 * b not an integer >= 2
 * a-b not a negative integer
 */
static
int
hyperg_1F1_U(const double a, const double b, const double x, gsl_sf_result * result)
{
  const double bp = 2.0 - b;
  const double ap = a - b + 1.0;

  gsl_sf_result lg_ap, lg_bp;
  double sg_ap;
  int stat_lg0 = gsl_sf_lngamma_sgn_e(ap, &lg_ap, &sg_ap);
  int stat_lg1 = gsl_sf_lngamma_e(bp, &lg_bp);
  int stat_lg2 = GSL_ERROR_SELECT_2(stat_lg0, stat_lg1);
  double t1 = (bp-1.0) * log(x);
  double lnpre_val = lg_ap.val - lg_bp.val + t1;
  double lnpre_err = lg_ap.err + lg_bp.err + 2.0 * GSL_DBL_EPSILON * fabs(t1);

  gsl_sf_result lg_2mbp, lg_1papmbp;
  double sg_2mbp, sg_1papmbp;
  int stat_lg3 = gsl_sf_lngamma_sgn_e(2.0-bp,    &lg_2mbp,    &sg_2mbp);
  int stat_lg4 = gsl_sf_lngamma_sgn_e(1.0+ap-bp, &lg_1papmbp, &sg_1papmbp);
  int stat_lg5 = GSL_ERROR_SELECT_2(stat_lg3, stat_lg4);
  double lnc1_val = lg_2mbp.val - lg_1papmbp.val;
  double lnc1_err = lg_2mbp.err + lg_1papmbp.err
                    + GSL_DBL_EPSILON * (fabs(lg_2mbp.val) + fabs(lg_1papmbp.val));

  gsl_sf_result M;
  gsl_sf_result_e10 U;
  int stat_F = gsl_sf_hyperg_1F1_e(ap, bp, x, &M);
  int stat_U = gsl_sf_hyperg_U_e10_e(ap, bp, x, &U);
  int stat_FU = GSL_ERROR_SELECT_2(stat_F, stat_U);

  gsl_sf_result_e10 term_M;
  int stat_e0 = gsl_sf_exp_mult_err_e10_e(lnc1_val, lnc1_err,
                                             sg_2mbp*sg_1papmbp*M.val, M.err,
                                             &term_M);

  const double ombp = 1.0 - bp;
  const double Uee_val = U.e10*M_LN10;
  const double Uee_err = 2.0 * GSL_DBL_EPSILON * fabs(Uee_val);
  const double Mee_val = term_M.e10*M_LN10;
  const double Mee_err = 2.0 * GSL_DBL_EPSILON * fabs(Mee_val);
  int stat_e1;

  /* Do a little dance with the exponential prefactors
   * to avoid overflows in intermediate results.
   */
  if(Uee_val > Mee_val) {
    const double factorM_val = exp(Mee_val-Uee_val);
    const double factorM_err = 2.0 * GSL_DBL_EPSILON * (fabs(Mee_val-Uee_val)+1.0) * factorM_val;
    const double inner_val = term_M.val*factorM_val - ombp*U.val;
    const double inner_err =
        term_M.err*factorM_val + fabs(ombp) * U.err
      + fabs(term_M.val) * factorM_err
      + GSL_DBL_EPSILON * (fabs(term_M.val*factorM_val) + fabs(ombp*U.val));
    stat_e1 = gsl_sf_exp_mult_err_e(lnpre_val+Uee_val, lnpre_err+Uee_err,
                                       sg_ap*inner_val, inner_err,
                                       result);
  }
  else {
    const double factorU_val = exp(Uee_val - Mee_val);
    const double factorU_err = 2.0 * GSL_DBL_EPSILON * (fabs(Mee_val-Uee_val)+1.0) * factorU_val;
    const double inner_val = term_M.val - ombp*factorU_val*U.val;
    const double inner_err =
        term_M.err + fabs(ombp*factorU_val*U.err)
      + fabs(ombp*factorU_err*U.val)
      + GSL_DBL_EPSILON * (fabs(term_M.val) + fabs(ombp*factorU_val*U.val));
    stat_e1 = gsl_sf_exp_mult_err_e(lnpre_val+Mee_val, lnpre_err+Mee_err,
                                       sg_ap*inner_val, inner_err,
                                       result);
  }

  return GSL_ERROR_SELECT_5(stat_e1, stat_e0, stat_FU, stat_lg5, stat_lg2);
}


/* Handle case of generic positive a, b.
 * Assumes b-a is not a negative integer.
 */
static
int
hyperg_1F1_ab_pos(const double a, const double b,
                  const double x,
                  gsl_sf_result * result)
{
  const double ax = fabs(x);

  if(   ( b < 10.0 && a < 10.0 && ax < 5.0 )
     || ( b > a*ax )
     || ( b > a && ax < 5.0 )
    ) {
    return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
  }
  else if(   x < -100.0
          && GSL_MAX_DBL(fabs(a),1.0)*GSL_MAX_DBL(fabs(1.0+a-b),1.0) < 0.7*fabs(x)
    ) {
    /* Large negative x asymptotic.
     */
    return hyperg_1F1_asymp_negx(a, b, x, result);
  }
  else if(   x > 100.0
          && GSL_MAX_DBL(fabs(b-a),1.0)*GSL_MAX_DBL(fabs(1.0-a),1.0) < 0.7*fabs(x)
    ) {
    /* Large positive x asymptotic.
     */
    return hyperg_1F1_asymp_posx(a, b, x, result);
  }
  else if(fabs(b-a) <= 1.0) {
    /* Directly handle b near a.
     */
    return hyperg_1F1_beps_bgt0(a-b, b, x, result);  /* a = b + eps */
  }

  else if(b > a && b >= 2*a + x) {
    /* Use the Gautschi CF series, then
     * recurse backward to a near 0 for normalization.
     * This will work for either sign of x.
     */ 
    double rap;
    int stat_CF1 = hyperg_1F1_CF1_p_ser(a, b, x, &rap);
    double ra = 1.0 + x/a * rap;

    double Ma   = GSL_SQRT_DBL_MIN;
    double Map1 = ra * Ma;
    double Mnp1 = Map1;
    double Mn   = Ma;
    double Mnm1;
    gsl_sf_result Mn_true;
    int stat_Mt;
    double n;
    for(n=a; n>0.5; n -= 1.0) {
      Mnm1 = (n * Mnp1 - (2.0*n-b+x) * Mn) / (b-n);
      Mnp1 = Mn;
      Mn   = Mnm1;
    }

    stat_Mt = hyperg_1F1_small_a_bgt0(n, b, x, &Mn_true);

    result->val  = (Ma/Mn) * Mn_true.val;
    result->err  = fabs(Ma/Mn) * Mn_true.err;
    result->err += 2.0 * GSL_DBL_EPSILON * (fabs(a)+1.0) * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_Mt, stat_CF1);
  }
  else if(b > a && b < 2*a + x && b > x) {
    /* Use the Gautschi series representation of
     * the continued fraction. Then recurse forward
     * to near the a=b line for normalization. This will
     * work for either sign of x, although we do need
     * to check for b > x, which is relevant when x is positive.
     */
    gsl_sf_result Mn_true;
    int stat_Mt;
    double rap;
    int stat_CF1 = hyperg_1F1_CF1_p_ser(a, b, x, &rap);
    double ra = 1.0 + x/a * rap;
    double Ma   = GSL_SQRT_DBL_MIN;
    double Mnm1 = Ma;
    double Mn   = ra * Mnm1;
    double Mnp1;
    double n;
    for(n=a+1.0; n<b-0.5; n += 1.0) {
      Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
      Mnm1 = Mn;
      Mn   = Mnp1;
    }
    stat_Mt = hyperg_1F1_beps_bgt0(n-b, b, x, &Mn_true);
    result->val  = Ma/Mn * Mn_true.val;
    result->err  = fabs(Ma/Mn) * Mn_true.err;
    result->err += 2.0 * GSL_DBL_EPSILON * (fabs(b-a)+1.0) * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_Mt, stat_CF1);
  }
  else if(x >= 0.0) {

    if(b < a) {
      /* Forward recursion on a from a=b+eps-1,b+eps.
       */
      double N   = floor(a-b);
      double eps = a - b - N;
      gsl_sf_result r_M0;
      gsl_sf_result r_M1;
      int stat_0 = hyperg_1F1_beps_bgt0(eps-1.0, b, x, &r_M0);
      int stat_1 = hyperg_1F1_beps_bgt0(eps,     b, x, &r_M1);
      double M0 = r_M0.val;
      double M1 = r_M1.val;

      double Mam1 = M0;
      double Ma   = M1;
      double Map1;
      double ap;
      double start_pair = fabs(M0) + fabs(M1);
      double minim_pair = GSL_DBL_MAX;
      double pair_ratio;
      double rat_0 = fabs(r_M0.err/r_M0.val);
      double rat_1 = fabs(r_M1.err/r_M1.val);
      for(ap=b+eps; ap<a-0.1; ap += 1.0) {
        Map1 = ((b-ap)*Mam1 + (2.0*ap-b+x)*Ma)/ap;
        Mam1 = Ma;
        Ma   = Map1;
        minim_pair = GSL_MIN_DBL(fabs(Mam1) + fabs(Ma), minim_pair);
      }
      pair_ratio = start_pair/minim_pair;
      result->val  = Ma;
      result->err  = 2.0 * (rat_0 + rat_1 + GSL_DBL_EPSILON) * (fabs(b-a)+1.0) * fabs(Ma);
      result->err += 2.0 * (rat_0 + rat_1) * pair_ratio*pair_ratio * fabs(Ma);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(Ma);
      return GSL_ERROR_SELECT_2(stat_0, stat_1);
    }
    else {
      /* b > a
       * b < 2a + x 
       * b <= x
       *
       * Recurse forward on a from a=eps,eps+1.
       */
      double eps = a - floor(a);
      gsl_sf_result r_Mnm1;
      gsl_sf_result r_Mn;
      int stat_0 = hyperg_1F1_small_a_bgt0(eps,     b, x, &r_Mnm1);
      int stat_1 = hyperg_1F1_small_a_bgt0(eps+1.0, b, x, &r_Mn);
      double Mnm1 = r_Mnm1.val;
      double Mn   = r_Mn.val;
      double Mnp1;

      double n;
      double start_pair = fabs(Mn) + fabs(Mnm1);
      double minim_pair = GSL_DBL_MAX;
      double pair_ratio;
      double rat_0 = fabs(r_Mnm1.err/r_Mnm1.val);
      double rat_1 = fabs(r_Mn.err/r_Mn.val);
      for(n=eps+1.0; n<a-0.1; n++) {
        Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
        Mnm1 = Mn;
        Mn   = Mnp1;
        minim_pair = GSL_MIN_DBL(fabs(Mn) + fabs(Mnm1), minim_pair);
      }
      pair_ratio = start_pair/minim_pair;
      result->val  = Mn;
      result->err  = 2.0 * (rat_0 + rat_1 + GSL_DBL_EPSILON) * (fabs(a)+1.0) * fabs(Mn);
      result->err += 2.0 * (rat_0 + rat_1) * pair_ratio*pair_ratio * fabs(Mn);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(Mn);
      return GSL_ERROR_SELECT_2(stat_0, stat_1);
    }
  }
  else {
    /* x < 0
     * b < a
     */

    if(a <= 0.5*(b-x) || a >= -x) {
      /* Recurse down in b, from near the a=b line, b=a+eps,a+eps-1.
       */
      double N   = floor(a - b);
      double eps = 1.0 + N - a + b;
      gsl_sf_result r_Manp1;
      gsl_sf_result r_Man;
      int stat_0 = hyperg_1F1_beps_bgt0(-eps,    a+eps,     x, &r_Manp1);
      int stat_1 = hyperg_1F1_beps_bgt0(1.0-eps, a+eps-1.0, x, &r_Man);
      double Manp1 = r_Manp1.val;
      double Man   = r_Man.val;
      double Manm1;

      double n;
      double start_pair = fabs(Manp1) + fabs(Man);
      double minim_pair = GSL_DBL_MAX;
      double pair_ratio;
      double rat_0 = fabs(r_Manp1.err/r_Manp1.val);
      double rat_1 = fabs(r_Man.err/r_Man.val);
      for(n=a+eps-1.0; n>b+0.1; n -= 1.0) {
        Manm1 = (-n*(1-n-x)*Man - x*(n-a)*Manp1)/(n*(n-1.0));
        Manp1 = Man;
        Man = Manm1;
        minim_pair = GSL_MIN_DBL(fabs(Manp1) + fabs(Man), minim_pair);
      }

      /* FIXME: this is a nasty little hack; there is some
         (transient?) instability in this recurrence for some
         values. I can tell when it happens, which is when
         this pair_ratio is large. But I do not know how to
         measure the error in terms of it. I guessed quadratic
         below, but it is probably worse than that.
         */
      pair_ratio = start_pair/minim_pair;
      result->val  = Man;
      result->err  = 2.0 * (rat_0 + rat_1 + GSL_DBL_EPSILON) * (fabs(b-a)+1.0) * fabs(Man);
      result->err *= pair_ratio*pair_ratio + 1.0;
      return GSL_ERROR_SELECT_2(stat_0, stat_1);
    }
    else {
      /* Pick a0 such that b ~= 2a0 + x, then
       * recurse down in b from a0,a0 to determine
       * the values near the line b=2a+x. Then recurse
       * forward on a from a0.
       */
      double epsa = a - floor(a);
      double a0   = floor(0.5*(b-x)) + epsa;
      double N    = floor(a0 - b);
      double epsb = 1.0 + N - a0 + b;
      double Ma0b;
      double Ma0bp1;
      double Ma0p1b;
      int stat_a0;
      double Mnm1;
      double Mn;
      double Mnp1;
      double n;
      double err_rat;
      {
        gsl_sf_result r_Ma0np1;
        gsl_sf_result r_Ma0n;
        int stat_0 = hyperg_1F1_beps_bgt0(-epsb,    a0+epsb,     x, &r_Ma0np1);
        int stat_1 = hyperg_1F1_beps_bgt0(1.0-epsb, a0+epsb-1.0, x, &r_Ma0n);
        double Ma0np1 = r_Ma0np1.val;
        double Ma0n   = r_Ma0n.val;
        double Ma0nm1;

        err_rat = fabs(r_Ma0np1.err/r_Ma0np1.val) + fabs(r_Ma0n.err/r_Ma0n.val);

        for(n=a0+epsb-1.0; n>b+0.1; n -= 1.0) {
          Ma0nm1 = (-n*(1-n-x)*Ma0n - x*(n-a0)*Ma0np1)/(n*(n-1.0));
          Ma0np1 = Ma0n;
          Ma0n = Ma0nm1;
        }
        Ma0bp1 = Ma0np1;
        Ma0b   = Ma0n;
        Ma0p1b = (b*(a0+x)*Ma0b+x*(a0-b)*Ma0bp1)/(a0*b); /* right-down hook */
        stat_a0 = GSL_ERROR_SELECT_2(stat_0, stat_1);
      }

          
      /* Initialise the recurrence correctly BJG */

      if (a0 >= a - 0.1)
        { 
          Mn = Ma0b;
        }
      else if (a0 + 1>= a - 0.1)
        {
          Mn = Ma0p1b;
        }
      else
        {
          Mnm1 = Ma0b;
          Mn   = Ma0p1b;

          for(n=a0+1.0; n<a-0.1; n += 1.0) {
            Mnp1 = ((b-n)*Mnm1 + (2*n-b+x)*Mn)/n;
            Mnm1 = Mn;
            Mn   = Mnp1;
          }
        }

      result->val = Mn;
      result->err = (err_rat + GSL_DBL_EPSILON) * (fabs(b-a)+1.0) * fabs(Mn);
      return stat_a0;
    }
  }
}


/* Assumes b != integer
 * Assumes a != integer when x > 0
 * Assumes b-a != neg integer when x < 0
 */
static
int
hyperg_1F1_ab_neg(const double a, const double b, const double x,
                  gsl_sf_result * result)
{
  const double bma = b - a;
  const double abs_x = fabs(x);
  const double abs_a = fabs(a);
  const double abs_b = fabs(b);
  const double size_a = GSL_MAX(abs_a, 1.0);
  const double size_b = GSL_MAX(abs_b, 1.0);
  const int bma_integer = ( bma - floor(bma+0.5) < _1F1_INT_THRESHOLD );

  if(   (abs_a < 10.0 && abs_b < 10.0 && abs_x < 5.0)
     || (b > 0.8*GSL_MAX(fabs(a),1.0)*fabs(x))
    ) {
    return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
  }
  else if(   x > 0.0
          && size_b > size_a
          && size_a*log(M_E*x/size_b) < GSL_LOG_DBL_EPSILON+7.0
    ) {
    /* Series terms are positive definite up until
     * there is a sign change. But by then the
     * terms are small due to the last condition.
     */
    return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
  }
  else if(   (abs_x < 5.0 && fabs(bma) < 10.0 && abs_b < 10.0)
          || (b > 0.8*GSL_MAX_DBL(fabs(bma),1.0)*abs_x)
    ) {
    /* Use Kummer transformation to render series safe.
     */
    gsl_sf_result Kummer_1F1;
    int stat_K = gsl_sf_hyperg_1F1_series_e(bma, b, -x, &Kummer_1F1);
    int stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                      Kummer_1F1.val, Kummer_1F1.err,
                                      result);
    return GSL_ERROR_SELECT_2(stat_e, stat_K);
  }
  else if(   x < -30.0
          && GSL_MAX_DBL(fabs(a),1.0)*GSL_MAX_DBL(fabs(1.0+a-b),1.0) < 0.99*fabs(x)
    ) {
    /* Large negative x asymptotic.
     * Note that we do not check if b-a is a negative integer.
     */
    return hyperg_1F1_asymp_negx(a, b, x, result);
  }
  else if(   x > 100.0
          && GSL_MAX_DBL(fabs(bma),1.0)*GSL_MAX_DBL(fabs(1.0-a),1.0) < 0.99*fabs(x)
    ) {
    /* Large positive x asymptotic.
     * Note that we do not check if a is a negative integer.
     */
    return hyperg_1F1_asymp_posx(a, b, x, result);
  }
  else if(x > 0.0 && !(bma_integer && bma > 0.0)) {
    return hyperg_1F1_U(a, b, x, result);
  }
  else {
    /* FIXME:  if all else fails, try the series... BJG */
    if (x < 0.0) {
      /* Apply Kummer Transformation */
      int status = gsl_sf_hyperg_1F1_series_e(b-a, b, -x, result);
      double K_factor = exp(x);
      result->val *= K_factor;
      result->err *= K_factor;
      return status;
    } else {
      int status = gsl_sf_hyperg_1F1_series_e(a, b, x, result);
      return status;
    }

    /* Sadness... */
    /* result->val = 0.0; */
    /* result->err = 0.0; */
    /* GSL_ERROR ("error", GSL_EUNIMPL); */
  }
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_hyperg_1F1_int_e(const int a, const int b, const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(a == b) {
    return gsl_sf_exp_e(x, result);
  }
  else if(b == 0) {
    DOMAIN_ERROR(result);
  }
  else if(a == 0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(b < 0 && (a < b || a > 0)) {
    /* Standard domain error due to singularity. */
    DOMAIN_ERROR(result);
  }
  else if(x > 100.0  && GSL_MAX_DBL(1.0,fabs(b-a))*GSL_MAX_DBL(1.0,fabs(1-a)) < 0.5 * x) {
    /* x -> +Inf asymptotic */
    return hyperg_1F1_asymp_posx(a, b, x, result);
  }
  else if(x < -100.0 && GSL_MAX_DBL(1.0,fabs(a))*GSL_MAX_DBL(1.0,fabs(1+a-b)) < 0.5 * fabs(x)) {
    /* x -> -Inf asymptotic */
    return hyperg_1F1_asymp_negx(a, b, x, result);
  }
  else if(a < 0 && b < 0) {
    return hyperg_1F1_ab_negint(a, b, x, result);
  }
  else if(a < 0 && b > 0) {
    /* Use Kummer to reduce it to the positive integer case.
     * Note that b > a, strictly, since we already trapped b = a.
     */
    gsl_sf_result Kummer_1F1;
    int stat_K = hyperg_1F1_ab_posint(b-a, b, -x, &Kummer_1F1);
    int stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                      Kummer_1F1.val, Kummer_1F1.err,
                                      result); 
    return GSL_ERROR_SELECT_2(stat_e, stat_K);
  }
  else {
    /* a > 0 and b > 0 */
    return hyperg_1F1_ab_posint(a, b, x, result);
  }
}


int
gsl_sf_hyperg_1F1_e(const double a, const double b, const double x,
                       gsl_sf_result * result
                       )
{
  const double bma = b - a;
  const double rinta = floor(a + 0.5);
  const double rintb = floor(b + 0.5);
  const double rintbma = floor(bma + 0.5);
  const int a_integer   = ( fabs(a-rinta) < _1F1_INT_THRESHOLD && rinta > INT_MIN && rinta < INT_MAX );
  const int b_integer   = ( fabs(b-rintb) < _1F1_INT_THRESHOLD && rintb > INT_MIN && rintb < INT_MAX );
  const int bma_integer = ( fabs(bma-rintbma) < _1F1_INT_THRESHOLD && rintbma > INT_MIN && rintbma < INT_MAX );
  const int b_neg_integer   = ( b < -0.1 && b_integer );
  const int a_neg_integer   = ( a < -0.1 && a_integer );
  const int bma_neg_integer = ( bma < -0.1 &&  bma_integer );

  /* CHECK_POINTER(result) */

  if(x == 0.0) {
    /* Testing for this before testing a and b
     * is somewhat arbitrary. The result is that
     * we have 1F1(a,0,0) = 1.
     */
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(b == 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(a == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(a == b) {
    /* case: a==b; exp(x)
     * It's good to test exact equality now.
     * We also test approximate equality later.
     */
    return gsl_sf_exp_e(x, result);
  } else if(fabs(b) < _1F1_INT_THRESHOLD && fabs(a) < _1F1_INT_THRESHOLD) {
    /* a and b near zero: 1 + a/b (exp(x)-1)
     */

    /* Note that neither a nor b is zero, since
     * we eliminated that with the above tests.
     */
    
    gsl_sf_result exm1;
    int stat_e = gsl_sf_expm1_e(x, &exm1);
    double sa = ( a > 0.0 ? 1.0 : -1.0 );
    double sb = ( b > 0.0 ? 1.0 : -1.0 );
    double lnab = log(fabs(a/b)); /* safe */
    gsl_sf_result hx;
    int stat_hx = gsl_sf_exp_mult_err_e(lnab, GSL_DBL_EPSILON * fabs(lnab),
                                        sa * sb * exm1.val, exm1.err,
                                        &hx);
    result->val = (hx.val == GSL_DBL_MAX ? hx.val : 1.0 + hx.val);  /* FIXME: excessive paranoia ? what is DBL_MAX+1 ?*/
    result->err = hx.err;
    return GSL_ERROR_SELECT_2(stat_hx, stat_e);
  } else if (fabs(b) < _1F1_INT_THRESHOLD && fabs(x*a) < 1) {
      /* b near zero and a not near zero
       */
      const double m_arg = 1.0/(0.5*b);
      gsl_sf_result F_renorm;
      int stat_F = hyperg_1F1_renorm_b0(a, x, &F_renorm);
      int stat_m = gsl_sf_multiply_err_e(m_arg, 2.0 * GSL_DBL_EPSILON * m_arg,
                                            0.5*F_renorm.val, 0.5*F_renorm.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_m, stat_F);
  }
  else if(a_integer && b_integer) {
    /* Check for reduction to the integer case.
     * Relies on the arbitrary "near an integer" test.
     */
    return gsl_sf_hyperg_1F1_int_e((int)rinta, (int)rintb, x, result);
  }
  else if(b_neg_integer && !(a_neg_integer && a > b)) {
    /* Standard domain error due to
     * uncancelled singularity.
     */
    DOMAIN_ERROR(result);
  }
  else if(a_neg_integer) {
    return hyperg_1F1_a_negint_lag((int)rinta, b, x, result);
  }
  else if(b > 0.0) {
    if(-1.0 <= a && a <= 1.0) {
      /* Handle small a explicitly.
       */
      return hyperg_1F1_small_a_bgt0(a, b, x, result);
    }
    else if(bma_neg_integer) {
      /* Catch this now, to avoid problems in the
       * generic evaluation code.
       */
      gsl_sf_result Kummer_1F1;
      int stat_K = hyperg_1F1_a_negint_lag((int)rintbma, b, -x, &Kummer_1F1);
      int stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                            Kummer_1F1.val, Kummer_1F1.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_K);
    }
    else if(a < 0.0 && fabs(x) < 100.0) {
      /* Use Kummer to reduce it to the generic positive case.
       * Note that b > a, strictly, since we already trapped b = a.
       * Also b-(b-a)=a, and a is not a negative integer here,
       * so the generic evaluation is safe.
       */
      gsl_sf_result Kummer_1F1;
      int stat_K = hyperg_1F1_ab_pos(b-a, b, -x, &Kummer_1F1);
      int stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                            Kummer_1F1.val, Kummer_1F1.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_K);
    }
    else if (a > 0) {
      /* a > 0.0 */
      return hyperg_1F1_ab_pos(a, b, x, result);
    } else {
      return gsl_sf_hyperg_1F1_series_e(a, b, x, result);
    }
  }
  else {
    /* b < 0.0 */

    if(bma_neg_integer && x < 0.0) {
      /* Handle this now to prevent problems
       * in the generic evaluation.
       */
      gsl_sf_result K;
      int stat_K;
      int stat_e;
      if(a < 0.0) {
        /* Kummer transformed version of safe polynomial.
         * The condition a < 0 is equivalent to b < b-a,
         * which is the condition required for the series
         * to be positive definite here.
         */
        stat_K = hyperg_1F1_a_negint_poly((int)rintbma, b, -x, &K);
      }
      else {
        /* Generic eval for negative integer a. */
        stat_K = hyperg_1F1_a_negint_lag((int)rintbma, b, -x, &K);
      }
      stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                        K.val, K.err,
                                        result);
      return GSL_ERROR_SELECT_2(stat_e, stat_K);
    }
    else if(a > 0.0) {
      /* Use Kummer to reduce it to the generic negative case.
       */
      gsl_sf_result K;
      int stat_K = hyperg_1F1_ab_neg(b-a, b, -x, &K);
      int stat_e = gsl_sf_exp_mult_err_e(x, GSL_DBL_EPSILON * fabs(x),
                                            K.val, K.err,
                                            result);
      return GSL_ERROR_SELECT_2(stat_e, stat_K);
    }
    else {
      return hyperg_1F1_ab_neg(a, b, x, result);
    }
  }
}


  
#if 0  
    /* Luke in the canonical case.
   */
  if(x < 0.0 && !a_neg_integer && !bma_neg_integer) {
    double prec;
    return hyperg_1F1_luke(a, b, x, result, &prec);
  }


  /* Luke with Kummer transformation.
   */
  if(x > 0.0 && !a_neg_integer && !bma_neg_integer) {
    double prec;
    double Kummer_1F1;
    double ex;
    int stat_F = hyperg_1F1_luke(b-a, b, -x, &Kummer_1F1, &prec);
    int stat_e = gsl_sf_exp_e(x, &ex);
    if(stat_F == GSL_SUCCESS && stat_e == GSL_SUCCESS) {
      double lnr = log(fabs(Kummer_1F1)) + x;
      if(lnr < GSL_LOG_DBL_MAX) {
        *result = ex * Kummer_1F1;
        return GSL_SUCCESS;
      }
      else {
        *result = GSL_POSINF; 
        GSL_ERROR ("overflow", GSL_EOVRFLW);
      }
    }
    else if(stat_F != GSL_SUCCESS) {
      *result = 0.0;
      return stat_F;
    }
    else {
      *result = 0.0;
      return stat_e;
    }
  }
#endif



/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_hyperg_1F1_int(const int m, const int n, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_1F1_int_e(m, n, x, &result));
}

double gsl_sf_hyperg_1F1(double a, double b, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_1F1_e(a, b, x, &result));
}
