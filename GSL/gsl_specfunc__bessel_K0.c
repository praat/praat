/* specfunc/bessel_K0.c
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
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC bk0(), bk0e() */

/* chebyshev expansions 

 series for bk0        on the interval  0.          to  4.00000d+00
                                        with weighted error   3.57e-19
                                         log weighted error  18.45
                               significant figures required  17.99
                                    decimal places required  18.97

 series for ak0        on the interval  1.25000d-01 to  5.00000d-01
                                        with weighted error   5.34e-17
                                         log weighted error  16.27
                               significant figures required  14.92
                                    decimal places required  16.89

 series for ak02       on the interval  0.          to  1.25000d-01
                                        with weighted error   2.34e-17
                                         log weighted error  16.63
                               significant figures required  14.67
                                    decimal places required  17.20
*/

static double bk0_data[11] = {
  -0.03532739323390276872,
   0.3442898999246284869, 
   0.03597993651536150163,
   0.00126461541144692592,
   0.00002286212103119451,
   0.00000025347910790261,
   0.00000000190451637722,
   0.00000000001034969525,
   0.00000000000004259816,
   0.00000000000000013744,
   0.00000000000000000035
};
static cheb_series bk0_cs = {
  bk0_data,
  10,
  -1, 1,
  10
};

static double ak0_data[17] = {
  -0.07643947903327941,
  -0.02235652605699819,
   0.00077341811546938,
  -0.00004281006688886,
   0.00000308170017386,
  -0.00000026393672220,
   0.00000002563713036,
  -0.00000000274270554,
   0.00000000031694296,
  -0.00000000003902353,
   0.00000000000506804,
  -0.00000000000068895,
   0.00000000000009744,
  -0.00000000000001427,
   0.00000000000000215,
  -0.00000000000000033,
   0.00000000000000005
};
static cheb_series ak0_cs = {
  ak0_data,
  16,
  -1, 1,
  10
};

static double ak02_data[14] = {
  -0.01201869826307592,
  -0.00917485269102569,
   0.00014445509317750,
  -0.00000401361417543,
   0.00000015678318108,
  -0.00000000777011043,
   0.00000000046111825,
  -0.00000000003158592,
   0.00000000000243501,
  -0.00000000000020743,
   0.00000000000001925,
  -0.00000000000000192,
   0.00000000000000020,
  -0.00000000000000002
};
static cheb_series ak02_cs = {
  ak02_data,
  13,
  -1, 1,
  8
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_K0_scaled_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x <= 2.0) {
    const double lx = log(x);
    const double ex = exp(x);
    int stat_I0;
    gsl_sf_result I0;
    gsl_sf_result c;
    cheb_eval_e(&bk0_cs, 0.5*x*x-1.0, &c);
    stat_I0 = gsl_sf_bessel_I0_e(x, &I0);
    result->val  = ex * ((-lx+M_LN2)*I0.val - 0.25 + c.val);
    result->err  = ex * ((M_LN2+fabs(lx))*I0.err + c.err);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_I0;
  }
  else if(x <= 8.0) {
    const double sx = sqrt(x);
    gsl_sf_result c;
    cheb_eval_e(&ak0_cs, (16.0/x-5.0)/3.0, &c);
    result->val  = (1.25 + c.val) / sx;
    result->err  = c.err / sx;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    const double sx = sqrt(x);
    gsl_sf_result c;
    cheb_eval_e(&ak02_cs, 16.0/x-1.0, &c);
    result->val  = (1.25 + c.val) / sx;
    result->err  = (c.err + GSL_DBL_EPSILON) / sx;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  } 
}


int gsl_sf_bessel_K0_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x <= 2.0) {
    const double lx = log(x);
    int stat_I0;
    gsl_sf_result I0;
    gsl_sf_result c;
    cheb_eval_e(&bk0_cs, 0.5*x*x-1.0, &c);
    stat_I0 = gsl_sf_bessel_I0_e(x, &I0);
    result->val  = (-lx+M_LN2)*I0.val - 0.25 + c.val;
    result->err  = (fabs(lx) + M_LN2) * I0.err + c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_I0;
  }
  else {
    gsl_sf_result K0_scaled;
    int stat_K0 = gsl_sf_bessel_K0_scaled_e(x, &K0_scaled);
    int stat_e  = gsl_sf_exp_mult_err_e(-x, GSL_DBL_EPSILON*fabs(x),
                                           K0_scaled.val, K0_scaled.err,
                                           result);
    return GSL_ERROR_SELECT_2(stat_e, stat_K0);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_K0_scaled(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_K0_scaled_e(x, &result));
}

double gsl_sf_bessel_K0(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_K0_e(x, &result));
}

