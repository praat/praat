/* specfunc/hyperg_2F1.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004 Gerard Jungman
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
#include "gsl_sf_exp.h"
#include "gsl_sf_pow_int.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_psi.h"
#include "gsl_sf_hyperg.h"

#include "gsl_specfunc__error.h"

#define locEPS (1000.0*GSL_DBL_EPSILON)


/* Assumes c != negative integer.
 */
static int
hyperg_2F1_series(const double a, const double b, const double c,
                  const double x, 
                  gsl_sf_result * result
                  )
{
  double sum_pos = 1.0;
  double sum_neg = 0.0;
  double del_pos = 1.0;
  double del_neg = 0.0;
  double del = 1.0;
  double k = 0.0;
  int i = 0;

  if(fabs(c) < GSL_DBL_EPSILON) {
    result->val = 0.0; /* FIXME: ?? */
    result->err = 1.0;
    GSL_ERROR ("error", GSL_EDOM);
  }

  do {
    if(++i > 30000) {
      result->val  = sum_pos - sum_neg;
      result->err  = del_pos + del_neg;
      result->err += 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
      result->err += 2.0 * GSL_DBL_EPSILON * (2.0*sqrt(k)+1.0) * fabs(result->val);
      GSL_ERROR ("error", GSL_EMAXITER);
    }
    del *= (a+k)*(b+k) * x / ((c+k) * (k+1.0));  /* Gauss series */

    if(del > 0.0) {
      del_pos  =  del;
      sum_pos +=  del;
    }
    else if(del == 0.0) {
      /* Exact termination (a or b was a negative integer).
       */
      del_pos = 0.0;
      del_neg = 0.0;
      break;
    }
    else {
      del_neg  = -del;
      sum_neg -=  del;
    }

    k += 1.0;
  } while(fabs((del_pos + del_neg)/(sum_pos-sum_neg)) > GSL_DBL_EPSILON);

  result->val  = sum_pos - sum_neg;
  result->err  = del_pos + del_neg;
  result->err += 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
  result->err += 2.0 * GSL_DBL_EPSILON * (2.0*sqrt(k) + 1.0) * fabs(result->val);

  return GSL_SUCCESS;
}


/* a = aR + i aI, b = aR - i aI */
static
int
hyperg_2F1_conj_series(const double aR, const double aI, const double c,
                       double x,
                       gsl_sf_result * result)
{
  if(c == 0.0) {
    result->val = 0.0; /* FIXME: should be Inf */
    result->err = 0.0;
    GSL_ERROR ("error", GSL_EDOM);
  }
  else {
    double sum_pos = 1.0;
    double sum_neg = 0.0;
    double del_pos = 1.0;
    double del_neg = 0.0;
    double del = 1.0;
    double k = 0.0;
    do {
      del *= ((aR+k)*(aR+k) + aI*aI)/((k+1.0)*(c+k)) * x;

      if(del >= 0.0) {
        del_pos  =  del;
        sum_pos +=  del;
      }
      else {
        del_neg  = -del;
        sum_neg -=  del;
      }

      if(k > 30000) {
        result->val  = sum_pos - sum_neg;
        result->err  = del_pos + del_neg;
        result->err += 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
        result->err += 2.0 * GSL_DBL_EPSILON * (2.0*sqrt(k)+1.0) * fabs(result->val);
        GSL_ERROR ("error", GSL_EMAXITER);
      }

      k += 1.0;
    } while(fabs((del_pos + del_neg)/(sum_pos - sum_neg)) > GSL_DBL_EPSILON);

    result->val  = sum_pos - sum_neg;
    result->err  = del_pos + del_neg;
    result->err += 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
    result->err += 2.0 * GSL_DBL_EPSILON * (2.0*sqrt(k) + 1.0) * fabs(result->val);

    return GSL_SUCCESS;
  }
}


/* Luke's rational approximation. The most accesible
 * discussion is in [Kolbig, CPC 23, 51 (1981)].
 * The convergence is supposedly guaranteed for x < 0.
 * You have to read Luke's books to see this and other
 * results. Unfortunately, the stability is not so
 * clear to me, although it seems very efficient when
 * it works.
 */
static
int
hyperg_2F1_luke(const double a, const double b, const double c,
                const double xin, 
                gsl_sf_result * result)
{
  int stat_iter;
  const double RECUR_BIG = 1.0e+50;
  const int nmax = 20000;
  int n = 3;
  const double x  = -xin;
  const double x3 = x*x*x;
  const double t0 = a*b/c;
  const double t1 = (a+1.0)*(b+1.0)/(2.0*c);
  const double t2 = (a+2.0)*(b+2.0)/(2.0*(c+1.0));
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
    double npbm1 = n + b - 1;
    double npcm1 = n + c - 1;
    double npam2 = n + a - 2;
    double npbm2 = n + b - 2;
    double npcm2 = n + c - 2;
    double tnm1  = 2*n - 1;
    double tnm3  = 2*n - 3;
    double tnm5  = 2*n - 5;
    double n2 = n*n;
    double F1 =  (3.0*n2 + (a+b-6)*n + 2 - a*b - 2*(a+b)) / (2*tnm3*npcm1);
    double F2 = -(3.0*n2 - (a+b+6)*n + 2 - a*b)*npam1*npbm1/(4*tnm1*tnm3*npcm2*npcm1);
    double F3 = (npam2*npam1*npbm2*npbm1*(n-a-2)*(n-b-2)) / (8*tnm3*tnm3*tnm5*(n+c-3)*npcm2*npcm1);
    double E  = -npam1*npbm1*(n-c-1) / (2*tnm3*npcm2*npcm1);

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
  result->err  = 2.0 * fabs(prec * F);
  result->err += 2.0 * GSL_DBL_EPSILON * (n+1.0) * fabs(F);

  /* FIXME: just a hack: there's a lot of shit going on here */
  result->err *= 8.0 * (fabs(a) + fabs(b) + 1.0);

  stat_iter = (n >= nmax ? GSL_EMAXITER : GSL_SUCCESS );

  return stat_iter;
}


/* Luke's rational approximation for the
 * case a = aR + i aI, b = aR - i aI.
 */
static
int
hyperg_2F1_conj_luke(const double aR, const double aI, const double c,
                     const double xin, 
                     gsl_sf_result * result)
{
  int stat_iter;
  const double RECUR_BIG = 1.0e+50;
  const int nmax = 10000;
  int n = 3;
  const double x = -xin;
  const double x3 = x*x*x;
  const double atimesb = aR*aR + aI*aI;
  const double apb     = 2.0*aR;
  const double t0 = atimesb/c;
  const double t1 = (atimesb +     apb + 1.0)/(2.0*c);
  const double t2 = (atimesb + 2.0*apb + 4.0)/(2.0*(c+1.0));
  double F = 1.0;
  double prec;

  double Bnm3 = 1.0;                                  /* B0 */
  double Bnm2 = 1.0 + t1 * x;                         /* B1 */
  double Bnm1 = 1.0 + t2 * x * (1.0 + t1/3.0 * x);    /* B2 */
 
  double Anm3 = 1.0;                                                      /* A0 */
  double Anm2 = Bnm2 - t0 * x;                                            /* A1 */
  double Anm1 = Bnm1 - t0*(1.0 + t2*x)*x + t0 * t1 * (c/(c+1.0)) * x*x;   /* A2 */

  while(1) {
    double nm1 = n - 1;
    double nm2 = n - 2;
    double npam1_npbm1 = atimesb + nm1*apb + nm1*nm1;
    double npam2_npbm2 = atimesb + nm2*apb + nm2*nm2;
    double npcm1 = nm1 + c;
    double npcm2 = nm2 + c;
    double tnm1  = 2*n - 1;
    double tnm3  = 2*n - 3;
    double tnm5  = 2*n - 5;
    double n2 = n*n;
    double F1 =  (3.0*n2 + (apb-6)*n + 2 - atimesb - 2*apb) / (2*tnm3*npcm1);
    double F2 = -(3.0*n2 - (apb+6)*n + 2 - atimesb)*npam1_npbm1/(4*tnm1*tnm3*npcm2*npcm1);
    double F3 = (npam2_npbm2*npam1_npbm1*(nm2*nm2 - nm2*apb + atimesb)) / (8*tnm3*tnm3*tnm5*(n+c-3)*npcm2*npcm1);
    double E  = -npam1_npbm1*(n-c-1) / (2*tnm3*npcm2*npcm1);

    double An = (1.0+F1*x)*Anm1 + (E + F2*x)*x*Anm2 + F3*x3*Anm3;
    double Bn = (1.0+F1*x)*Bnm1 + (E + F2*x)*x*Bnm2 + F3*x3*Bnm3;
    double r = An/Bn;

    prec = fabs(F - r)/fabs(F);
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
  result->err  = 2.0 * fabs(prec * F);
  result->err += 2.0 * GSL_DBL_EPSILON * (n+1.0) * fabs(F);

  /* FIXME: see above */
  result->err *= 8.0 * (fabs(aR) + fabs(aI) + 1.0);

  stat_iter = (n >= nmax ? GSL_EMAXITER : GSL_SUCCESS );

  return stat_iter;
}


/* Do the reflection described in [Moshier, p. 334].
 * Assumes a,b,c != neg integer.
 */
static
int
hyperg_2F1_reflect(const double a, const double b, const double c,
                   const double x, gsl_sf_result * result)
{
  const double d = c - a - b;
  const int intd  = floor(d+0.5);
  const int d_integer = ( fabs(d - intd) < locEPS );

  if(d_integer) {
    const double ln_omx = log(1.0 - x);
    const double ad = fabs(d);
    int stat_F2 = GSL_SUCCESS;
    double sgn_2;
    gsl_sf_result F1;
    gsl_sf_result F2;
    double d1, d2;
    gsl_sf_result lng_c;
    gsl_sf_result lng_ad2;
    gsl_sf_result lng_bd2;
    int stat_ad2;
    int stat_bd2;

    if(d >= 0.0) {
      d1 = d;
      d2 = 0.0;
    }
    else {
      d1 = 0.0;
      d2 = d;
    }

    stat_ad2 = gsl_sf_lngamma_e(a+d2, &lng_ad2);
    stat_bd2 = gsl_sf_lngamma_e(b+d2, &lng_bd2);
    (void) gsl_sf_lngamma_e(c,    &lng_c);

    /* Evaluate F1.
     */
    if(ad < GSL_DBL_EPSILON) {
      /* d = 0 */
      F1.val = 0.0;
      F1.err = 0.0;
    }
    else {
      gsl_sf_result lng_ad;
      gsl_sf_result lng_ad1;
      gsl_sf_result lng_bd1;
      int stat_ad  = gsl_sf_lngamma_e(ad,   &lng_ad);
      int stat_ad1 = gsl_sf_lngamma_e(a+d1, &lng_ad1);
      int stat_bd1 = gsl_sf_lngamma_e(b+d1, &lng_bd1);

      if(stat_ad1 == GSL_SUCCESS && stat_bd1 == GSL_SUCCESS && stat_ad == GSL_SUCCESS) {
        /* Gamma functions in the denominator are ok.
         * Proceed with evaluation.
         */
        int i;
        double sum1 = 1.0;
        double term = 1.0;
        double ln_pre1_val = lng_ad.val + lng_c.val + d2*ln_omx - lng_ad1.val - lng_bd1.val;
        double ln_pre1_err = lng_ad.err + lng_c.err + lng_ad1.err + lng_bd1.err + GSL_DBL_EPSILON * fabs(ln_pre1_val);
        int stat_e;

        /* Do F1 sum.
         */
        for(i=1; i<ad; i++) {
          int j = i-1;
          term *= (a + d2 + j) * (b + d2 + j) / (1.0 + d2 + j) / i * (1.0-x);
          sum1 += term;
        }
        
        stat_e = gsl_sf_exp_mult_err_e(ln_pre1_val, ln_pre1_err,
                                       sum1, GSL_DBL_EPSILON*fabs(sum1),
                                       &F1);
        if(stat_e == GSL_EOVRFLW) {
          OVERFLOW_ERROR(result);
        }
      }
      else {
        /* Gamma functions in the denominator were not ok.
         * So the F1 term is zero.
         */
        F1.val = 0.0;
        F1.err = 0.0;
      }
    } /* end F1 evaluation */


    /* Evaluate F2.
     */
    if(stat_ad2 == GSL_SUCCESS && stat_bd2 == GSL_SUCCESS) {
      /* Gamma functions in the denominator are ok.
       * Proceed with evaluation.
       */
      const int maxiter = 2000;
      double psi_1 = -M_EULER;
      gsl_sf_result psi_1pd; 
      gsl_sf_result psi_apd1;
      gsl_sf_result psi_bpd1;
      int stat_1pd  = gsl_sf_psi_e(1.0 + ad, &psi_1pd);
      int stat_apd1 = gsl_sf_psi_e(a + d1,   &psi_apd1);
      int stat_bpd1 = gsl_sf_psi_e(b + d1,   &psi_bpd1);
      int stat_dall = GSL_ERROR_SELECT_3(stat_1pd, stat_apd1, stat_bpd1);

      double psi_val = psi_1 + psi_1pd.val - psi_apd1.val - psi_bpd1.val - ln_omx;
      double psi_err = psi_1pd.err + psi_apd1.err + psi_bpd1.err + GSL_DBL_EPSILON*fabs(psi_val);
      double fact = 1.0;
      double sum2_val = psi_val;
      double sum2_err = psi_err;
      double ln_pre2_val = lng_c.val + d1*ln_omx - lng_ad2.val - lng_bd2.val;
      double ln_pre2_err = lng_c.err + lng_ad2.err + lng_bd2.err + GSL_DBL_EPSILON * fabs(ln_pre2_val);
      int stat_e;

      int j;

      /* Do F2 sum.
       */
      for(j=1; j<maxiter; j++) {
        /* values for psi functions use recurrence; Abramowitz+Stegun 6.3.5 */
        double term1 = 1.0/(double)j  + 1.0/(ad+j);
        double term2 = 1.0/(a+d1+j-1.0) + 1.0/(b+d1+j-1.0);
        double delta = 0.0;
        psi_val += term1 - term2;
        psi_err += GSL_DBL_EPSILON * (fabs(term1) + fabs(term2));
        fact *= (a+d1+j-1.0)*(b+d1+j-1.0)/((ad+j)*j) * (1.0-x);
        delta = fact * psi_val;
        sum2_val += delta;
        sum2_err += fabs(fact * psi_err) + GSL_DBL_EPSILON*fabs(delta);
        if(fabs(delta) < GSL_DBL_EPSILON * fabs(sum2_val)) break;
      }

      if(j == maxiter) stat_F2 = GSL_EMAXITER;

      if(sum2_val == 0.0) {
        F2.val = 0.0;
        F2.err = 0.0;
      }
      else {
        stat_e = gsl_sf_exp_mult_err_e(ln_pre2_val, ln_pre2_err,
                                       sum2_val, sum2_err,
                                       &F2);
        if(stat_e == GSL_EOVRFLW) {
          result->val = 0.0;
          result->err = 0.0;
          GSL_ERROR ("error", GSL_EOVRFLW);
        }
      }
      stat_F2 = GSL_ERROR_SELECT_2(stat_F2, stat_dall);
    }
    else {
      /* Gamma functions in the denominator not ok.
       * So the F2 term is zero.
       */
      F2.val = 0.0;
      F2.err = 0.0;
    } /* end F2 evaluation */

    sgn_2 = ( GSL_IS_ODD(intd) ? -1.0 : 1.0 );
    result->val  = F1.val + sgn_2 * F2.val;
    result->err  = F1.err + F2. err;
    result->err += 2.0 * GSL_DBL_EPSILON * (fabs(F1.val) + fabs(F2.val));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_F2;
  }
  else {
    /* d not an integer */

    gsl_sf_result pre1, pre2;
    double sgn1, sgn2;
    gsl_sf_result F1, F2;

    /* These gamma functions appear in the denominator, so we
     * catch their harmless domain errors and set the terms to zero.
     */
    gsl_sf_result ln_g1ca,  ln_g1cb,  ln_g2a,  ln_g2b;
    double sgn_g1ca, sgn_g1cb, sgn_g2a, sgn_g2b;
    int stat_1ca = gsl_sf_lngamma_sgn_e(c-a, &ln_g1ca, &sgn_g1ca);
    int stat_1cb = gsl_sf_lngamma_sgn_e(c-b, &ln_g1cb, &sgn_g1cb);
    int stat_2a  = gsl_sf_lngamma_sgn_e(a, &ln_g2a, &sgn_g2a);
    int stat_2b  = gsl_sf_lngamma_sgn_e(b, &ln_g2b, &sgn_g2b);
    int ok1 = (stat_1ca == GSL_SUCCESS && stat_1cb == GSL_SUCCESS);
    int ok2 = (stat_2a  == GSL_SUCCESS && stat_2b  == GSL_SUCCESS);
    
    gsl_sf_result ln_gc,  ln_gd,  ln_gmd;
    double sgn_gc, sgn_gd, sgn_gmd;
    gsl_sf_lngamma_sgn_e( c, &ln_gc,  &sgn_gc);
    gsl_sf_lngamma_sgn_e( d, &ln_gd,  &sgn_gd);
    gsl_sf_lngamma_sgn_e(-d, &ln_gmd, &sgn_gmd);
    
    sgn1 = sgn_gc * sgn_gd  * sgn_g1ca * sgn_g1cb;
    sgn2 = sgn_gc * sgn_gmd * sgn_g2a  * sgn_g2b;

    if(ok1 && ok2) {
      double ln_pre1_val = ln_gc.val + ln_gd.val  - ln_g1ca.val - ln_g1cb.val;
      double ln_pre2_val = ln_gc.val + ln_gmd.val - ln_g2a.val  - ln_g2b.val + d*log(1.0-x);
      double ln_pre1_err = ln_gc.err + ln_gd.err + ln_g1ca.err + ln_g1cb.err;
      double ln_pre2_err = ln_gc.err + ln_gmd.err + ln_g2a.err  + ln_g2b.err;
      if(ln_pre1_val < GSL_LOG_DBL_MAX && ln_pre2_val < GSL_LOG_DBL_MAX) {
        gsl_sf_exp_err_e(ln_pre1_val, ln_pre1_err, &pre1);
        gsl_sf_exp_err_e(ln_pre2_val, ln_pre2_err, &pre2);
        pre1.val *= sgn1;
        pre2.val *= sgn2;
      }
      else {
        OVERFLOW_ERROR(result);
      }
    }
    else if(ok1 && !ok2) {
      double ln_pre1_val = ln_gc.val + ln_gd.val - ln_g1ca.val - ln_g1cb.val;
      double ln_pre1_err = ln_gc.err + ln_gd.err + ln_g1ca.err + ln_g1cb.err;
      if(ln_pre1_val < GSL_LOG_DBL_MAX) {
        gsl_sf_exp_err_e(ln_pre1_val, ln_pre1_err, &pre1);
        pre1.val *= sgn1;
        pre2.val = 0.0;
        pre2.err = 0.0;
      }
      else {
        OVERFLOW_ERROR(result);
      }
    }
    else if(!ok1 && ok2) {
      double ln_pre2_val = ln_gc.val + ln_gmd.val - ln_g2a.val - ln_g2b.val + d*log(1.0-x);
      double ln_pre2_err = ln_gc.err + ln_gmd.err + ln_g2a.err + ln_g2b.err;
      if(ln_pre2_val < GSL_LOG_DBL_MAX) {
        pre1.val = 0.0;
        pre1.err = 0.0;
        gsl_sf_exp_err_e(ln_pre2_val, ln_pre2_err, &pre2);
        pre2.val *= sgn2;
      }
      else {
        OVERFLOW_ERROR(result);
      }
    }
    else {
      pre1.val = 0.0;
      pre2.val = 0.0;
      UNDERFLOW_ERROR(result);
    }

    (void) hyperg_2F1_series(  a,   b, 1.0-d, 1.0-x, &F1);
    (void) hyperg_2F1_series(c-a, c-b, 1.0+d, 1.0-x, &F2);

    result->val  = pre1.val*F1.val + pre2.val*F2.val;
    result->err  = fabs(pre1.val*F1.err) + fabs(pre2.val*F2.err);
    result->err += fabs(pre1.err*F1.val) + fabs(pre2.err*F2.val);
    result->err += 2.0 * GSL_DBL_EPSILON * (fabs(pre1.val*F1.val) + fabs(pre2.val*F2.val));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

    return GSL_SUCCESS;
  }
}


static int pow_omx(const double x, const double p, gsl_sf_result * result)
{
  double ln_omx;
  double ln_result;
  if(fabs(x) < GSL_ROOT5_DBL_EPSILON) {
    ln_omx = -x*(1.0 + x*(1.0/2.0 + x*(1.0/3.0 + x/4.0 + x*x/5.0)));
  }
  else {
    ln_omx = log(1.0-x);
  }
  ln_result = p * ln_omx;
  return gsl_sf_exp_err_e(ln_result, GSL_DBL_EPSILON * fabs(ln_result), result);
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_hyperg_2F1_e(double a, double b, const double c,
                       const double x,
                       gsl_sf_result * result)
{
  const double d = c - a - b;
  const double rinta = floor(a + 0.5);
  const double rintb = floor(b + 0.5);
  const double rintc = floor(c + 0.5);
  const int a_neg_integer = ( a < 0.0  &&  fabs(a - rinta) < locEPS );
  const int b_neg_integer = ( b < 0.0  &&  fabs(b - rintb) < locEPS );
  const int c_neg_integer = ( c < 0.0  &&  fabs(c - rintc) < locEPS );

  result->val = 0.0;
  result->err = 0.0;

  if(x < -1.0 || 1.0 <= x) {
    DOMAIN_ERROR(result);
  }

  if(c_neg_integer) {
    if(! (a_neg_integer && a > c + 0.1)) DOMAIN_ERROR(result);
    if(! (b_neg_integer && b > c + 0.1)) DOMAIN_ERROR(result);
  }

  if(fabs(c-b) < locEPS || fabs(c-a) < locEPS) {
    return pow_omx(x, d, result);  /* (1-x)^(c-a-b) */
  }

  if(a >= 0.0 && b >= 0.0 && c >=0.0 && x >= 0.0 && x < 0.995) {
    /* Series has all positive definite
     * terms and x is not close to 1.
     */
    return hyperg_2F1_series(a, b, c, x, result);
  }

  if(fabs(a) < 10.0 && fabs(b) < 10.0) {
    /* a and b are not too large, so we attempt
     * variations on the series summation.
     */
    if(a_neg_integer) {
      return hyperg_2F1_series(rinta, b, c, x, result);
    }
    if(b_neg_integer) {
      return hyperg_2F1_series(a, rintb, c, x, result);
    }

    if(x < -0.25) {
      return hyperg_2F1_luke(a, b, c, x, result);
    }
    else if(x < 0.5) {
      return hyperg_2F1_series(a, b, c, x, result);
    }
    else {
      if(fabs(c) > 10.0) {
        return hyperg_2F1_series(a, b, c, x, result);
      }
      else {
        return hyperg_2F1_reflect(a, b, c, x, result);
      }
    }
  }
  else {
    /* Either a or b or both large.
     * Introduce some new variables ap,bp so that bp is
     * the larger in magnitude.
     */
    double bp; 
    if(fabs(a) > fabs(b)) {
      bp = a;
    }
    else {
      bp = b;
    }

    if(x < 0.0) {
      /* What the hell, maybe Luke will converge.
       */
      return hyperg_2F1_luke(a, b, c, x, result);
    }

    if(GSL_MAX_DBL(fabs(a),1.0)*fabs(bp)*fabs(x) < 2.0*fabs(c)) {
      /* If c is large enough or x is small enough,
       * we can attempt the series anyway.
       */
      return hyperg_2F1_series(a, b, c, x, result);
    }

    if(fabs(bp*bp*x*x) < 0.001*fabs(bp) && fabs(a) < 10.0) {
      /* The famous but nearly worthless "large b" asymptotic.
       */
      int stat = gsl_sf_hyperg_1F1_e(a, c, bp*x, result);
      result->err = 0.001 * fabs(result->val);
      return stat;
    }

    /* We give up. */
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR ("error", GSL_EUNIMPL);
  }
}


int
gsl_sf_hyperg_2F1_conj_e(const double aR, const double aI, const double c,
                            const double x,
                            gsl_sf_result * result)
{
  const double ax = fabs(x);
  const double rintc = floor(c + 0.5);
  const int c_neg_integer = ( c < 0.0  &&  fabs(c - rintc) < locEPS );

  result->val = 0.0;
  result->err = 0.0;

  if(ax >= 1.0 || c_neg_integer || c == 0.0) {
    DOMAIN_ERROR(result);
  }

  if(   (ax < 0.25 && fabs(aR) < 20.0 && fabs(aI) < 20.0)
     || (c > 0.0 && x > 0.0)
    ) {
    return hyperg_2F1_conj_series(aR, aI, c, x, result);
  }
  else if(fabs(aR) < 10.0 && fabs(aI) < 10.0) {
    if(x < -0.25) {
      return hyperg_2F1_conj_luke(aR, aI, c, x, result);
    }
    else {
      return hyperg_2F1_conj_series(aR, aI, c, x, result);
    }
  }
  else {
    if(x < 0.0) {
      /* What the hell, maybe Luke will converge.
       */
      return hyperg_2F1_conj_luke(aR, aI, c, x, result); 
    }

    /* Give up. */
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR ("error", GSL_EUNIMPL);
  }
}


int
gsl_sf_hyperg_2F1_renorm_e(const double a, const double b, const double c,
                              const double x,
                              gsl_sf_result * result
                              )
{
  const double rinta = floor(a + 0.5);
  const double rintb = floor(b + 0.5);
  const double rintc = floor(c + 0.5);
  const int a_neg_integer = ( a < 0.0  &&  fabs(a - rinta) < locEPS );
  const int b_neg_integer = ( b < 0.0  &&  fabs(b - rintb) < locEPS );
  const int c_neg_integer = ( c < 0.0  &&  fabs(c - rintc) < locEPS );
  
  if(c_neg_integer) {
    if((a_neg_integer && a > c+0.1) || (b_neg_integer && b > c+0.1)) {
      /* 2F1 terminates early */
      result->val = 0.0;
      result->err = 0.0;
      return GSL_SUCCESS;
    }
    else {
      /* 2F1 does not terminate early enough, so something survives */
      /* [Abramowitz+Stegun, 15.1.2] */
      gsl_sf_result g1, g2, g3, g4, g5;
      double s1, s2, s3, s4, s5;
      int stat = 0;
      stat += gsl_sf_lngamma_sgn_e(a-c+1, &g1, &s1);
      stat += gsl_sf_lngamma_sgn_e(b-c+1, &g2, &s2);
      stat += gsl_sf_lngamma_sgn_e(a, &g3, &s3);
      stat += gsl_sf_lngamma_sgn_e(b, &g4, &s4);
      stat += gsl_sf_lngamma_sgn_e(-c+2, &g5, &s5);
      if(stat != 0) {
        DOMAIN_ERROR(result);
      }
      else {
        gsl_sf_result F;
        int stat_F = gsl_sf_hyperg_2F1_e(a-c+1, b-c+1, -c+2, x, &F);
        double ln_pre_val = g1.val + g2.val - g3.val - g4.val - g5.val;
        double ln_pre_err = g1.err + g2.err + g3.err + g4.err + g5.err;
        double sg  = s1 * s2 * s3 * s4 * s5;
        int stat_e = gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                              sg * F.val, F.err,
                                              result);
        return GSL_ERROR_SELECT_2(stat_e, stat_F);
      }
    }
  }
  else {
    /* generic c */
    gsl_sf_result F;
    gsl_sf_result lng;
    double sgn;
    int stat_g = gsl_sf_lngamma_sgn_e(c, &lng, &sgn);
    int stat_F = gsl_sf_hyperg_2F1_e(a, b, c, x, &F);
    int stat_e = gsl_sf_exp_mult_err_e(-lng.val, lng.err,
                                          sgn*F.val, F.err,
                                          result);
    return GSL_ERROR_SELECT_3(stat_e, stat_F, stat_g);
  }
}


int
gsl_sf_hyperg_2F1_conj_renorm_e(const double aR, const double aI, const double c,
                                   const double x,
                                   gsl_sf_result * result
                                   )
{
  const double rintc = floor(c  + 0.5);
  const double rinta = floor(aR + 0.5);
  const int a_neg_integer = ( aR < 0.0 && fabs(aR-rinta) < locEPS && aI == 0.0);
  const int c_neg_integer = (  c < 0.0 && fabs(c - rintc) < locEPS );

  if(c_neg_integer) {
    if(a_neg_integer && aR > c+0.1) {
      /* 2F1 terminates early */
      result->val = 0.0;
      result->err = 0.0;
      return GSL_SUCCESS;
    }
    else {
      /* 2F1 does not terminate early enough, so something survives */
      /* [Abramowitz+Stegun, 15.1.2] */
      gsl_sf_result g1, g2;
      gsl_sf_result g3;
      gsl_sf_result a1, a2;
      int stat = 0;
      stat += gsl_sf_lngamma_complex_e(aR-c+1, aI, &g1, &a1);
      stat += gsl_sf_lngamma_complex_e(aR, aI, &g2, &a2);
      stat += gsl_sf_lngamma_e(-c+2.0, &g3);
      if(stat != 0) {
        DOMAIN_ERROR(result);
      }
      else {
        gsl_sf_result F;
        int stat_F = gsl_sf_hyperg_2F1_conj_e(aR-c+1, aI, -c+2, x, &F);
        double ln_pre_val = 2.0*(g1.val - g2.val) - g3.val;
        double ln_pre_err = 2.0 * (g1.err + g2.err) + g3.err;
        int stat_e = gsl_sf_exp_mult_err_e(ln_pre_val, ln_pre_err,
                                              F.val, F.err,
                                              result);
        return GSL_ERROR_SELECT_2(stat_e, stat_F);
      }
    }
  }
  else {
    /* generic c */
    gsl_sf_result F;
    gsl_sf_result lng;
    double sgn;
    int stat_g = gsl_sf_lngamma_sgn_e(c, &lng, &sgn);
    int stat_F = gsl_sf_hyperg_2F1_conj_e(aR, aI, c, x, &F);
    int stat_e = gsl_sf_exp_mult_err_e(-lng.val, lng.err,
                                          sgn*F.val, F.err,
                                          result);
    return GSL_ERROR_SELECT_3(stat_e, stat_F, stat_g);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_hyperg_2F1(double a, double b, double c, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_2F1_e(a, b, c, x, &result));
}

double gsl_sf_hyperg_2F1_conj(double aR, double aI, double c, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_2F1_conj_e(aR, aI, c, x, &result));
}

double gsl_sf_hyperg_2F1_renorm(double a, double b, double c, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_2F1_renorm_e(a, b, c, x, &result));
}

double gsl_sf_hyperg_2F1_conj_renorm(double aR, double aI, double c, double x)
{
  EVAL_RESULT(gsl_sf_hyperg_2F1_conj_renorm_e(aR, aI, c, x, &result));
}
