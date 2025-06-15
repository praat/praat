/* specfunc/bessel_Y1.c
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
#include "gsl_sf_trig.h"
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel.h"
#include "gsl_specfunc__bessel_amp_phase.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC besy1, 1977 version, w. fullerton */

/* chebyshev expansions

 series for by1        on the interval  0.          to  1.60000d+01
                                        with weighted error   1.87e-18
                                         log weighted error  17.73
                               significant figures required  17.83
                                    decimal places required  18.30
*/

static double by1_data[14] = {
  0.03208047100611908629,
  1.262707897433500450,
  0.00649996189992317500,
 -0.08936164528860504117,
  0.01325088122175709545,
 -0.00089790591196483523,
  0.00003647361487958306,
 -0.00000100137438166600,
  0.00000001994539657390,
 -0.00000000030230656018,
  0.00000000000360987815,
 -0.00000000000003487488,
  0.00000000000000027838,
 -0.00000000000000000186
};
static cheb_series by1_cs = {
  by1_data,
  13,
  -1, 1,
  10
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_Y1_e(const double x, gsl_sf_result * result)
{
  const double two_over_pi = 2.0/M_PI;
  const double xmin = 1.571*GSL_DBL_MIN; /*exp ( amax1(alog(r1mach(1)), -alog(r1mach(2)))+.01) */
  const double x_small = 2.0 * GSL_SQRT_DBL_EPSILON;
  const double xmax    = 1.0/GSL_DBL_EPSILON;

  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < xmin) {
    OVERFLOW_ERROR(result);
  }
  else if(x < x_small) {
    const double lnterm = log(0.5*x);
    gsl_sf_result J1;
    gsl_sf_result c;
    int status = gsl_sf_bessel_J1_e(x, &J1);
    cheb_eval_e(&by1_cs, -1.0, &c);
    result->val = two_over_pi * lnterm * J1.val + (0.5 + c.val)/x;
    result->err = fabs(lnterm) * (fabs(GSL_DBL_EPSILON * J1.val) + J1.err) + c.err/x;
    return status;
  }
  else if(x < 4.0) {
    const double lnterm = log(0.5*x);
    int status;
    gsl_sf_result J1;
    gsl_sf_result c;
    cheb_eval_e(&by1_cs, 0.125*x*x-1.0, &c);
    status = gsl_sf_bessel_J1_e(x, &J1);
    result->val = two_over_pi * lnterm * J1.val + (0.5 + c.val)/x;
    result->err = fabs(lnterm) * (fabs(GSL_DBL_EPSILON * J1.val) + J1.err) + c.err/x;
    return status;
  }
  else if(x < xmax) {
    const double z = 32.0/(x*x) - 1.0;
    gsl_sf_result ca;
    gsl_sf_result ct;
    gsl_sf_result cp;
    const int stat_ca = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bm1_cs,  z, &ca);
    const int stat_ct = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bth1_cs, z, &ct);
    const int stat_cp = gsl_sf_bessel_cos_pi4_e(x, ct.val/x, &cp);
    const double sqrtx = sqrt(x);
    const double ampl  = (0.75 + ca.val) / sqrtx;
    result->val  = -ampl * cp.val;
    result->err  = fabs(cp.val) * ca.err/sqrtx + fabs(ampl) * cp.err;
    result->err += GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_3(stat_ca, stat_ct, stat_cp);
  }
  else {
    UNDERFLOW_ERROR(result);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Y1(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Y1_e(x, &result));
}
