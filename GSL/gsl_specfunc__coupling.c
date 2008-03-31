/* specfunc/coupling.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002 Gerard Jungman
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
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_coupling.h"

#include "gsl_specfunc__error.h"

inline
static
int locMax3(const int a, const int b, const int c)
{
  int d = GSL_MAX(a, b);
  return GSL_MAX(d, c);
}

inline
static
int locMin3(const int a, const int b, const int c)
{
  int d = GSL_MIN(a, b);
  return GSL_MIN(d, c);
}

inline
static
int locMin5(const int a, const int b, const int c, const int d, const int e)
{
  int f = GSL_MIN(a, b);
  int g = GSL_MIN(c, d);
  int h = GSL_MIN(f, g);
  return GSL_MIN(e, h);
}


/* See: [Thompson, Atlas for Computing Mathematical Functions] */

static
int
delta(int ta, int tb, int tc, gsl_sf_result * d)
{
  gsl_sf_result f1, f2, f3, f4;
  int status = 0;
  status += gsl_sf_fact_e((ta + tb - tc)/2, &f1);
  status += gsl_sf_fact_e((ta + tc - tb)/2, &f2);
  status += gsl_sf_fact_e((tb + tc - ta)/2, &f3);
  status += gsl_sf_fact_e((ta + tb + tc)/2 + 1, &f4);
  if(status != 0) {
    OVERFLOW_ERROR(d);
  }
  d->val = f1.val * f2.val * f3.val / f4.val;
  d->err = 4.0 * GSL_DBL_EPSILON * fabs(d->val);
  return GSL_SUCCESS;
}


static
int
triangle_selection_fails(int two_ja, int two_jb, int two_jc)
{
  return ((two_jb < abs(two_ja - two_jc)) || (two_jb > two_ja + two_jc));
}


static
int
m_selection_fails(int two_ja, int two_jb, int two_jc,
                  int two_ma, int two_mb, int two_mc)
{
  return (
         abs(two_ma) > two_ja 
      || abs(two_mb) > two_jb
      || abs(two_mc) > two_jc
      || GSL_IS_ODD(two_ja + two_ma)
      || GSL_IS_ODD(two_jb + two_mb)
      || GSL_IS_ODD(two_jc + two_mc)
      || (two_ma + two_mb + two_mc) != 0
          );
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/


int
gsl_sf_coupling_3j_e (int two_ja, int two_jb, int two_jc,
                      int two_ma, int two_mb, int two_mc,
                      gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(two_ja < 0 || two_jb < 0 || two_jc < 0) {
    DOMAIN_ERROR(result);
  }
  else if (   triangle_selection_fails(two_ja, two_jb, two_jc)
           || m_selection_fails(two_ja, two_jb, two_jc, two_ma, two_mb, two_mc)
     ) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    int jca  = (-two_ja + two_jb + two_jc) / 2,
        jcb  = ( two_ja - two_jb + two_jc) / 2,
        jcc  = ( two_ja + two_jb - two_jc) / 2,
        jmma = ( two_ja - two_ma) / 2,
        jmmb = ( two_jb - two_mb) / 2,
        jmmc = ( two_jc - two_mc) / 2,
        jpma = ( two_ja + two_ma) / 2,
        jpmb = ( two_jb + two_mb) / 2,
        jpmc = ( two_jc + two_mc) / 2,
        jsum = ( two_ja + two_jb + two_jc) / 2,
        kmin = locMax3 (0, jpmb - jmmc, jmma - jpmc),
        kmax = locMin3 (jcc, jmma, jpmb),
        k, sign = GSL_IS_ODD (kmin - jpma + jmmb) ? -1 : 1,
        status = 0;
    double sum_pos = 0.0, sum_neg = 0.0, norm, term;
    gsl_sf_result bc1, bc2, bc3, bcn1, bcn2, bcd1, bcd2, bcd3, bcd4;

    status += gsl_sf_choose_e (two_ja, jcc , &bcn1);
    status += gsl_sf_choose_e (two_jb, jcc , &bcn2);
    status += gsl_sf_choose_e (jsum+1, jcc , &bcd1);
    status += gsl_sf_choose_e (two_ja, jmma, &bcd2);
    status += gsl_sf_choose_e (two_jb, jmmb, &bcd3);
    status += gsl_sf_choose_e (two_jc, jpmc, &bcd4);
    
    if (status != 0) {
      OVERFLOW_ERROR (result);
    }
    
    norm = sqrt (bcn1.val * bcn2.val)
           / sqrt (bcd1.val * bcd2.val * bcd3.val * bcd4.val * ((double) two_jc + 1.0));

    for (k = kmin; k <= kmax; k++) {
      status += gsl_sf_choose_e (jcc, k, &bc1);
      status += gsl_sf_choose_e (jcb, jmma - k, &bc2);
      status += gsl_sf_choose_e (jca, jpmb - k, &bc3);
      
      if (status != 0) {
        OVERFLOW_ERROR (result);
      }
      
      term = bc1.val * bc2.val * bc3.val;
      
      if (sign < 0) {
        sum_neg += norm * term;
      } else {
        sum_pos += norm * term;
      }
      
      sign = -sign;
    }
    
    result->val  = sum_pos - sum_neg;
    result->err  = 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
    result->err += 2.0 * GSL_DBL_EPSILON * (kmax - kmin) * fabs(result->val);

    return GSL_SUCCESS;
  }
}
#if ! defined (GSL_DISABLE_DEPRECATED)
int
gsl_sf_coupling_6j_INCORRECT_e(int two_ja, int two_jb, int two_jc,
                               int two_jd, int two_je, int two_jf,
                               gsl_sf_result * result)
{
  return gsl_sf_coupling_6j_e(two_ja, two_jb, two_je, two_jd, two_jc, two_jf, result);
}
#endif

int
gsl_sf_coupling_6j_e(int two_ja, int two_jb, int two_jc,
                     int two_jd, int two_je, int two_jf,
                     gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(   two_ja < 0 || two_jb < 0 || two_jc < 0
     || two_jd < 0 || two_je < 0 || two_jf < 0
     ) {
    DOMAIN_ERROR(result);
  }
  else if(   triangle_selection_fails(two_ja, two_jb, two_jc)
          || triangle_selection_fails(two_ja, two_je, two_jf)
          || triangle_selection_fails(two_jb, two_jd, two_jf)
          || triangle_selection_fails(two_je, two_jd, two_jc)
     ) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result n1;
    gsl_sf_result d1, d2, d3, d4, d5, d6;
    double norm;
    int tk, tkmin, tkmax;
    double phase;
    double sum_pos = 0.0;
    double sum_neg = 0.0;
    double sumsq_err = 0.0;
    int status = 0;
    status += delta(two_ja, two_jb, two_jc, &d1);
    status += delta(two_ja, two_je, two_jf, &d2);
    status += delta(two_jb, two_jd, two_jf, &d3);
    status += delta(two_je, two_jd, two_jc, &d4);
    if(status != GSL_SUCCESS) {
      OVERFLOW_ERROR(result);
    }
    norm = sqrt(d1.val) * sqrt(d2.val) * sqrt(d3.val) * sqrt(d4.val);
    
    tkmin = locMax3(0,
                   two_ja + two_jd - two_jc - two_jf,
                   two_jb + two_je - two_jc - two_jf);

    tkmax = locMin5(two_ja + two_jb + two_je + two_jd + 2,
                    two_ja + two_jb - two_jc,
                    two_je + two_jd - two_jc,
                    two_ja + two_je - two_jf,
                    two_jb + two_jd - two_jf);

    phase = GSL_IS_ODD((two_ja + two_jb + two_je + two_jd + tkmin)/2)
            ? -1.0
            :  1.0;

    for(tk=tkmin; tk<=tkmax; tk += 2) {
      double term;
      double term_err;
      gsl_sf_result den_1, den_2;
      gsl_sf_result d1_a, d1_b;
      status = 0;

      status += gsl_sf_fact_e((two_ja + two_jb + two_je + two_jd - tk)/2 + 1, &n1);
      status += gsl_sf_fact_e(tk/2, &d1_a);
      status += gsl_sf_fact_e((two_jc + two_jf - two_ja - two_jd + tk)/2, &d1_b);
      status += gsl_sf_fact_e((two_jc + two_jf - two_jb - two_je + tk)/2, &d2);
      status += gsl_sf_fact_e((two_ja + two_jb - two_jc - tk)/2, &d3);
      status += gsl_sf_fact_e((two_je + two_jd - two_jc - tk)/2, &d4);
      status += gsl_sf_fact_e((two_ja + two_je - two_jf - tk)/2, &d5);
      status += gsl_sf_fact_e((two_jb + two_jd - two_jf - tk)/2, &d6);

      if(status != GSL_SUCCESS) {
        OVERFLOW_ERROR(result);
      }

      d1.val = d1_a.val * d1_b.val;
      d1.err = d1_a.err * fabs(d1_b.val) + fabs(d1_a.val) * d1_b.err;

      den_1.val  = d1.val*d2.val*d3.val;
      den_1.err  = d1.err * fabs(d2.val*d3.val);
      den_1.err += d2.err * fabs(d1.val*d3.val);
      den_1.err += d3.err * fabs(d1.val*d2.val);

      den_2.val  = d4.val*d5.val*d6.val;
      den_2.err  = d4.err * fabs(d5.val*d6.val);
      den_2.err += d5.err * fabs(d4.val*d6.val);
      den_2.err += d6.err * fabs(d4.val*d5.val);

      term  = phase * n1.val / den_1.val / den_2.val;
      phase = -phase;
      term_err  = n1.err / fabs(den_1.val) / fabs(den_2.val);
      term_err += fabs(term / den_1.val) * den_1.err;
      term_err += fabs(term / den_2.val) * den_2.err;

      if(term >= 0.0) {
        sum_pos += norm*term;
      }
      else {
        sum_neg -= norm*term;
      }

      sumsq_err += norm*norm * term_err*term_err;
    }

    result->val  = sum_pos - sum_neg;
    result->err  = 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
    result->err += sqrt(sumsq_err / (0.5*(tkmax-tkmin)+1.0));
    result->err += 2.0 * GSL_DBL_EPSILON * (tkmax - tkmin + 2.0) * fabs(result->val);

    return GSL_SUCCESS;
  }
}


int
gsl_sf_coupling_RacahW_e(int two_ja, int two_jb, int two_jc,
                         int two_jd, int two_je, int two_jf,
                         gsl_sf_result * result)
{
  int status = gsl_sf_coupling_6j_e(two_ja, two_jb, two_je, two_jd, two_jc, two_jf, result);
  int phase_sum = (two_ja + two_jb + two_jc + two_jd)/2;
  result->val *= ( GSL_IS_ODD(phase_sum) ? -1.0 : 1.0 );
  return status;
}


int
gsl_sf_coupling_9j_e(int two_ja, int two_jb, int two_jc,
                     int two_jd, int two_je, int two_jf,
                     int two_jg, int two_jh, int two_ji,
                     gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(   two_ja < 0 || two_jb < 0 || two_jc < 0
     || two_jd < 0 || two_je < 0 || two_jf < 0
     || two_jg < 0 || two_jh < 0 || two_ji < 0
     ) {
    DOMAIN_ERROR(result);
  }
  else if(   triangle_selection_fails(two_ja, two_jb, two_jc)
          || triangle_selection_fails(two_jd, two_je, two_jf)
          || triangle_selection_fails(two_jg, two_jh, two_ji)
          || triangle_selection_fails(two_ja, two_jd, two_jg)
          || triangle_selection_fails(two_jb, two_je, two_jh)
          || triangle_selection_fails(two_jc, two_jf, two_ji)
     ) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    int tk;
    int tkmin = locMax3(abs(two_ja-two_ji), abs(two_jh-two_jd), abs(two_jb-two_jf));
    int tkmax = locMin3(two_ja + two_ji, two_jh + two_jd, two_jb + two_jf);
    double sum_pos = 0.0;
    double sum_neg = 0.0;
    double sumsq_err = 0.0;
    double phase;
    for(tk=tkmin; tk<=tkmax; tk += 2) {
      gsl_sf_result s1, s2, s3;
      double term;
      double term_err;
      int status = 0;

      status += gsl_sf_coupling_6j_e(two_ja, two_ji, tk,  two_jh, two_jd, two_jg,  &s1);
      status += gsl_sf_coupling_6j_e(two_jb, two_jf, tk,  two_jd, two_jh, two_je,  &s2);
      status += gsl_sf_coupling_6j_e(two_ja, two_ji, tk,  two_jf, two_jb, two_jc,  &s3);

      if(status != GSL_SUCCESS) {
        OVERFLOW_ERROR(result);
      }
      term = s1.val * s2.val * s3.val;
      term_err  = s1.err * fabs(s2.val*s3.val);
      term_err += s2.err * fabs(s1.val*s3.val);
      term_err += s3.err * fabs(s1.val*s2.val);

      if(term >= 0.0) {
        sum_pos += (tk + 1) * term;
      }
      else {
        sum_neg -= (tk + 1) * term;
      }

      sumsq_err += ((tk+1) * term_err) * ((tk+1) * term_err);
    }

    phase = GSL_IS_ODD(tkmin) ? -1.0 : 1.0;

    result->val  = phase * (sum_pos - sum_neg);
    result->err  = 2.0 * GSL_DBL_EPSILON * (sum_pos + sum_neg);
    result->err += sqrt(sumsq_err / (0.5*(tkmax-tkmin)+1.0));
    result->err += 2.0 * GSL_DBL_EPSILON * (tkmax-tkmin + 2.0) * fabs(result->val);

    return GSL_SUCCESS;
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_coupling_3j(int two_ja, int two_jb, int two_jc,
                          int two_ma, int two_mb, int two_mc)
{
  EVAL_RESULT(gsl_sf_coupling_3j_e(two_ja, two_jb, two_jc,
                                   two_ma, two_mb, two_mc,
                                   &result));
}

#if ! defined (GSL_DISABLE_DEPRECATED)
double gsl_sf_coupling_6j_INCORRECT(int two_ja, int two_jb, int two_jc,
                                    int two_jd, int two_je, int two_jf)
{
  EVAL_RESULT(gsl_sf_coupling_6j_INCORRECT_e(two_ja, two_jb, two_jc,
                                             two_jd, two_je, two_jf,
                                             &result));
}
#endif

double gsl_sf_coupling_6j(int two_ja, int two_jb, int two_jc,
                          int two_jd, int two_je, int two_jf)
{
  EVAL_RESULT(gsl_sf_coupling_6j_e(two_ja, two_jb, two_jc,
                                   two_jd, two_je, two_jf,
                                   &result));
}


double gsl_sf_coupling_RacahW(int two_ja, int two_jb, int two_jc,
                          int two_jd, int two_je, int two_jf)
{
  EVAL_RESULT(gsl_sf_coupling_RacahW_e(two_ja, two_jb, two_jc,
                                      two_jd, two_je, two_jf,
                                      &result));
}


double gsl_sf_coupling_9j(int two_ja, int two_jb, int two_jc,
                          int two_jd, int two_je, int two_jf,
                          int two_jg, int two_jh, int two_ji)
{
  EVAL_RESULT(gsl_sf_coupling_9j_e(two_ja, two_jb, two_jc,
                                   two_jd, two_je, two_jf,
                                   two_jg, two_jh, two_ji,
                                   &result));
}
