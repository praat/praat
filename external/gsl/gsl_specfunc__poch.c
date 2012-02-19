/* specfunc/poch.c
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
#include "gsl_sf_exp.h"
#include "gsl_sf_log.h"
#include "gsl_sf_pow_int.h"
#include "gsl_sf_psi.h"
#include "gsl_sf_gamma.h"

#include "gsl_specfunc__error.h"

static const double bern[21] = {
   0.0   /* no element 0 */,  
  +0.833333333333333333333333333333333e-01,
  -0.138888888888888888888888888888888e-02,
  +0.330687830687830687830687830687830e-04,
  -0.826719576719576719576719576719576e-06,
  +0.208767569878680989792100903212014e-07,
  -0.528419013868749318484768220217955e-09,
  +0.133825365306846788328269809751291e-10,
  -0.338968029632258286683019539124944e-12,
  +0.858606205627784456413590545042562e-14,
  -0.217486869855806187304151642386591e-15,
  +0.550900282836022951520265260890225e-17,
  -0.139544646858125233407076862640635e-18,
  +0.353470703962946747169322997780379e-20,
  -0.895351742703754685040261131811274e-22,
  +0.226795245233768306031095073886816e-23,
  -0.574472439520264523834847971943400e-24,
  +0.145517247561486490186626486727132e-26,
  -0.368599494066531017818178247990866e-28,
  +0.933673425709504467203255515278562e-30,
  -0.236502241570062993455963519636983e-31
};


/* ((a)_x - 1)/x in the "small x" region where
 * cancellation must be controlled.
 *
 * Based on SLATEC DPOCH1().
 */
/*
C When ABS(X) is so small that substantial cancellation will occur if
C the straightforward formula is used, we use an expansion due
C to Fields and discussed by Y. L. Luke, The Special Functions and Their
C Approximations, Vol. 1, Academic Press, 1969, page 34.
C
C The ratio POCH(A,X) = GAMMA(A+X)/GAMMA(A) is written by Luke as
C        (A+(X-1)/2)**X * polynomial in (A+(X-1)/2)**(-2) .
C In order to maintain significance in POCH1, we write for positive a
C        (A+(X-1)/2)**X = EXP(X*LOG(A+(X-1)/2)) = EXP(Q)
C                       = 1.0 + Q*EXPREL(Q) .
C Likewise the polynomial is written
C        POLY = 1.0 + X*POLY1(A,X) .
C Thus,
C        POCH1(A,X) = (POCH(A,X) - 1) / X
C                   = EXPREL(Q)*(Q/X + Q*POLY1(A,X)) + POLY1(A,X)
C
*/
static
int
pochrel_smallx(const double a, const double x, gsl_sf_result * result)
{
  /*
   SQTBIG = 1.0D0/SQRT(24.0D0*D1MACH(1))
   ALNEPS = LOG(D1MACH(3))
   */
  const double SQTBIG = 1.0/(2.0*M_SQRT2*M_SQRT3*GSL_SQRT_DBL_MIN);
  const double ALNEPS = GSL_LOG_DBL_EPSILON - M_LN2;

  if(x == 0.0) {
    return gsl_sf_psi_e(a, result);
  }
  else {
    const double bp   = (  (a < -0.5) ? 1.0-a-x : a );
    const int    incr = ( (bp < 10.0) ? 11.0-bp : 0 );
    const double b    = bp + incr;
    double dpoch1;
    gsl_sf_result dexprl;
    int stat_dexprl;
    int i;

    double var    = b + 0.5*(x-1.0);
    double alnvar = log(var);
    double q = x*alnvar;

    double poly1 = 0.0;

    if(var < SQTBIG) {
      const int nterms = (int)(-0.5*ALNEPS/alnvar + 1.0);
      const double var2 = (1.0/var)/var;
      const double rho  = 0.5 * (x + 1.0);
      double term = var2;
      double gbern[24];
      int k, j;

      gbern[1] = 1.0;
      gbern[2] = -rho/12.0;
      poly1 = gbern[2] * term;

      if(nterms > 20) {
        /* NTERMS IS TOO BIG, MAYBE D1MACH(3) IS BAD */
        /* nterms = 20; */
        result->val = 0.0;
        result->err = 0.0;
        GSL_ERROR ("error", GSL_ESANITY);
      }

      for(k=2; k<=nterms; k++) {
        double gbk = 0.0;
        for(j=1; j<=k; j++) {
          gbk += bern[k-j+1]*gbern[j];
        }
        gbern[k+1] = -rho*gbk/k;

        term  *= (2*k-2-x)*(2*k-1-x)*var2;
        poly1 += gbern[k+1]*term;
      }
    }

    stat_dexprl = gsl_sf_expm1_e(q, &dexprl);
    if(stat_dexprl != GSL_SUCCESS) {
      result->val = 0.0;
      result->err = 0.0;
      return stat_dexprl;
    }
    dexprl.val = dexprl.val/q;
    poly1 *= (x - 1.0);
    dpoch1 = dexprl.val * (alnvar + q * poly1) + poly1;

    for(i=incr-1; i >= 0; i--) {
      /*
       C WE HAVE DPOCH1(B,X), BUT BP IS SMALL, SO WE USE BACKWARDS RECURSION
       C TO OBTAIN DPOCH1(BP,X).
       */
      double binv = 1.0/(bp+i);
      dpoch1 = (dpoch1 - binv) / (1.0 + x*binv);
    }

    if(bp == a) {
      result->val = dpoch1;
      result->err = 2.0 * GSL_DBL_EPSILON * (fabs(incr) + 1.0) * fabs(result->val);
      return GSL_SUCCESS;
    }
    else {
      /*
       C WE HAVE DPOCH1(BP,X), BUT A IS LT -0.5.  WE THEREFORE USE A
       C REFLECTION FORMULA TO OBTAIN DPOCH1(A,X).
       */
      double sinpxx = sin(M_PI*x)/x;
      double sinpx2 = sin(0.5*M_PI*x);
      double t1 = sinpxx/tan(M_PI*b);
      double t2 = 2.0*sinpx2*(sinpx2/x);
      double trig  = t1 - t2;
      result->val  = dpoch1 * (1.0 + x*trig) + trig;
      result->err  = (fabs(dpoch1*x) + 1.0) * GSL_DBL_EPSILON * (fabs(t1) + fabs(t2));
      result->err += 2.0 * GSL_DBL_EPSILON * (fabs(incr) + 1.0) * fabs(result->val);
      return GSL_SUCCESS;
    }    
  }
}


/* Assumes a>0 and a+x>0.
 */
static
int
lnpoch_pos(const double a, const double x, gsl_sf_result * result)
{
  double absx = fabs(x);

  if(absx > 0.1*a || absx*log(GSL_MAX_DBL(a,2.0)) > 0.1) {
    if(a < GSL_SF_GAMMA_XMAX && a+x < GSL_SF_GAMMA_XMAX) {
      /* If we can do it by calculating the gamma functions
       * directly, then that will be more accurate than
       * doing the subtraction of the logs.
       */
      gsl_sf_result g1;
      gsl_sf_result g2;
      gsl_sf_gammainv_e(a,   &g1);
      gsl_sf_gammainv_e(a+x, &g2);
      result->val  = -log(g2.val/g1.val);
      result->err  = g1.err/fabs(g1.val) + g2.err/fabs(g2.val);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
    else {
      /* Otherwise we must do the subtraction.
       */
      gsl_sf_result lg1;
      gsl_sf_result lg2;
      int stat_1 = gsl_sf_lngamma_e(a,   &lg1);
      int stat_2 = gsl_sf_lngamma_e(a+x, &lg2);
      result->val  = lg2.val - lg1.val;
      result->err  = lg2.err + lg1.err;
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return GSL_ERROR_SELECT_2(stat_1, stat_2);
    }
  }
  else if(absx < 0.1*a && a > 15.0) {
    /* Be careful about the implied subtraction.
     * Note that both a+x and and a must be
     * large here since a is not small
     * and x is not relatively large.
     * So we calculate using Stirling for Log[Gamma(z)].
     *
     *   Log[Gamma(a+x)/Gamma(a)] = x(Log[a]-1) + (x+a-1/2)Log[1+x/a]
     *                              + (1/(1+eps)   - 1) / (12 a)
     *                              - (1/(1+eps)^3 - 1) / (360 a^3)
     *                              + (1/(1+eps)^5 - 1) / (1260 a^5)
     *                              - (1/(1+eps)^7 - 1) / (1680 a^7)
     *                              + ...
     */
    const double eps = x/a;
    const double den = 1.0 + eps;
    const double d3 = den*den*den;
    const double d5 = d3*den*den;
    const double d7 = d5*den*den;
    const double c1 = -eps/den;
    const double c3 = -eps*(3.0+eps*(3.0+eps))/d3;
    const double c5 = -eps*(5.0+eps*(10.0+eps*(10.0+eps*(5.0+eps))))/d5;
    const double c7 = -eps*(7.0+eps*(21.0+eps*(35.0+eps*(35.0+eps*(21.0+eps*(7.0+eps))))))/d7;
    const double p8 = gsl_sf_pow_int(1.0+eps,8);
    const double c8 = 1.0/p8             - 1.0;  /* these need not   */
    const double c9 = 1.0/(p8*(1.0+eps)) - 1.0;  /* be very accurate */
    const double a4 = a*a*a*a;
    const double a6 = a4*a*a;
    const double ser_1 = c1 + c3/(30.0*a*a) + c5/(105.0*a4) + c7/(140.0*a6);
    const double ser_2 = c8/(99.0*a6*a*a) - 691.0/360360.0 * c9/(a6*a4);
    const double ser = (ser_1 + ser_2)/ (12.0*a);

    double term1 = x * log(a/M_E);
    double term2;
    gsl_sf_result ln_1peps;
    gsl_sf_log_1plusx_e(eps, &ln_1peps);  /* log(1 + x/a) */
    term2 = (x + a - 0.5) * ln_1peps.val;

    result->val  = term1 + term2 + ser;
    result->err  = GSL_DBL_EPSILON*fabs(term1);
    result->err += fabs((x + a - 0.5)*ln_1peps.err);
    result->err += fabs(ln_1peps.val) * GSL_DBL_EPSILON * (fabs(x) + fabs(a) + 0.5);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result poch_rel;
    int stat_p = pochrel_smallx(a, x, &poch_rel);
    double eps = x*poch_rel.val;
    int stat_e = gsl_sf_log_1plusx_e(eps, result);
    result->err  = 2.0 * fabs(x * poch_rel.err / (1.0 + eps));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_e, stat_p);
  }
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_lnpoch_e(const double a, const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(a <= 0.0 || a+x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    return lnpoch_pos(a, x, result);
  }
}


int
gsl_sf_lnpoch_sgn_e(const double a, const double x,
                       gsl_sf_result * result, double * sgn)
{
  if(a == 0.0 || a+x == 0.0) {
    *sgn = 0.0;
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    *sgn = 1.0;
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(a > 0.0 && a+x > 0.0) {
    *sgn = 1.0;
    return lnpoch_pos(a, x, result);
  }
  else if(a < 0.0 && a+x < 0.0) {
    /* Reduce to positive case using reflection.
     */
    double sin_1 = sin(M_PI * (1.0 - a));
    double sin_2 = sin(M_PI * (1.0 - a - x));
    if(sin_1 == 0.0 || sin_2 == 0.0) {
      *sgn = 0.0;
      DOMAIN_ERROR(result);
    }
    else {
      gsl_sf_result lnp_pos;
      int stat_pp   = lnpoch_pos(1.0-a, -x, &lnp_pos);
      double lnterm = log(fabs(sin_1/sin_2));
      result->val  = lnterm - lnp_pos.val;
      result->err  = lnp_pos.err;
      result->err += 2.0 * GSL_DBL_EPSILON * (fabs(1.0-a) + fabs(1.0-a-x)) * fabs(lnterm);
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      *sgn = GSL_SIGN(sin_1*sin_2);
      return stat_pp;
    }
  }
  else {
    /* Evaluate gamma ratio directly.
     */
    gsl_sf_result lg_apn;
    gsl_sf_result lg_a;
    double s_apn, s_a;
    int stat_apn = gsl_sf_lngamma_sgn_e(a+x, &lg_apn, &s_apn);
    int stat_a   = gsl_sf_lngamma_sgn_e(a,   &lg_a,   &s_a);
    if(stat_apn == GSL_SUCCESS && stat_a == GSL_SUCCESS) {
      result->val  = lg_apn.val - lg_a.val;
      result->err  = lg_apn.err + lg_a.err;
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      *sgn = s_a * s_apn;
      return GSL_SUCCESS;
    }
    else if(stat_apn == GSL_EDOM || stat_a == GSL_EDOM){
      *sgn = 0.0;
      DOMAIN_ERROR(result);
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
      *sgn = 0.0;
      return GSL_FAILURE;
    }
  }
}


int
gsl_sf_poch_e(const double a, const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result lnpoch;
    double sgn;
    int stat_lnpoch = gsl_sf_lnpoch_sgn_e(a, x, &lnpoch, &sgn);
    int stat_exp    = gsl_sf_exp_err_e(lnpoch.val, lnpoch.err, result);
    result->val *= sgn;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_exp, stat_lnpoch);
  }
}


int
gsl_sf_pochrel_e(const double a, const double x, gsl_sf_result * result)
{
  const double absx = fabs(x);
  const double absa = fabs(a);

  /* CHECK_POINTER(result) */

  if(absx > 0.1*absa || absx*log(GSL_MAX(absa,2.0)) > 0.1) {
    gsl_sf_result lnpoch;
    double sgn;
    int stat_poch = gsl_sf_lnpoch_sgn_e(a, x, &lnpoch, &sgn);
    if(lnpoch.val > GSL_LOG_DBL_MAX) {
      OVERFLOW_ERROR(result);
    }
    else {
      const double el = exp(lnpoch.val);
      result->val  = (sgn*el - 1.0)/x;
      result->err  = fabs(result->val) * (lnpoch.err + 2.0 * GSL_DBL_EPSILON);
      result->err += 2.0 * GSL_DBL_EPSILON * (fabs(sgn*el) + 1.0) / fabs(x);
      return stat_poch;
    }
  }
  else {
    return pochrel_smallx(a, x, result);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_lnpoch(const double a, const double x)
{
  EVAL_RESULT(gsl_sf_lnpoch_e(a, x, &result));
}

double gsl_sf_poch(const double a, const double x)
{
  EVAL_RESULT(gsl_sf_poch_e(a, x, &result));
}

double gsl_sf_pochrel(const double a, const double x)
{
  EVAL_RESULT(gsl_sf_pochrel_e(a, x, &result));
}
