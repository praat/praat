/* specfunc/bessel_I0.c
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
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/


/* based on SLATEC besi0 */

/* chebyshev expansions

 series for bi0        on the interval  0.          to  9.00000d+00
                                        with weighted error   2.46e-18
                                         log weighted error  17.61
                               significant figures required  17.90
                                    decimal places required  18.15

 series for ai0        on the interval  1.25000d-01 to  3.33333d-01
                                        with weighted error   7.87e-17
                                         log weighted error  16.10
                               significant figures required  14.69
                                    decimal places required  16.76


 series for ai02       on the interval  0.          to  1.25000d-01
                                        with weighted error   3.79e-17
                                         log weighted error  16.42
                               significant figures required  14.86
                                    decimal places required  17.09
*/

static double bi0_data[12] = {
  -.07660547252839144951,
  1.92733795399380827000,
   .22826445869203013390, 
   .01304891466707290428,
   .00043442709008164874,
   .00000942265768600193,
   .00000014340062895106,
   .00000000161384906966,
   .00000000001396650044,
   .00000000000009579451,
   .00000000000000053339,
   .00000000000000000245
};
static cheb_series bi0_cs = {
  bi0_data,
  11,
  -1, 1,
  11
};

static double ai0_data[21] = {
   .07575994494023796, 
   .00759138081082334,
   .00041531313389237,
   .00001070076463439,
  -.00000790117997921,
  -.00000078261435014,
   .00000027838499429,
   .00000000825247260,
  -.00000001204463945,
   .00000000155964859,
   .00000000022925563,
  -.00000000011916228,
   .00000000001757854,
   .00000000000112822,
  -.00000000000114684,
   .00000000000027155,
  -.00000000000002415,
  -.00000000000000608,
   .00000000000000314,
  -.00000000000000071,
   .00000000000000007
};
static cheb_series ai0_cs = {
  ai0_data,
  20,
  -1, 1,
  13
};

static double ai02_data[22] = {
   .05449041101410882,
   .00336911647825569,
   .00006889758346918,
   .00000289137052082,
   .00000020489185893,
   .00000002266668991,
   .00000000339623203,
   .00000000049406022,
   .00000000001188914,
  -.00000000003149915,
  -.00000000001321580,
  -.00000000000179419,
   .00000000000071801,
   .00000000000038529,
   .00000000000001539,
  -.00000000000004151,
  -.00000000000000954,
   .00000000000000382,
   .00000000000000176,
  -.00000000000000034,
  -.00000000000000027,
   .00000000000000003
};
static cheb_series ai02_cs = {
  ai02_data,
  21,
  -1, 1,
  11
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_I0_scaled_e(const double x, gsl_sf_result * result)
{
  double y = fabs(x);

  /* CHECK_POINTER(result) */

  if(y < 2.0 * GSL_SQRT_DBL_EPSILON) {
    result->val = 1.0 - y;
    result->err = 0.5*y*y;
    return GSL_SUCCESS;
  }
  else if(y <= 3.0) {
    const double ey = exp(-y);
    gsl_sf_result c;
    cheb_eval_e(&bi0_cs, y*y/4.5-1.0, &c);
    result->val = ey * (2.75 + c.val);
    result->err = GSL_DBL_EPSILON * fabs(result->val) + ey * c.err;
    return GSL_SUCCESS;
  }
  else if(y <= 8.0) {
    const double sy = sqrt(y);
    gsl_sf_result c;
    cheb_eval_e(&ai0_cs, (48.0/y-11.0)/5.0, &c);
    result->val  = (0.375 + c.val) / sy;
    result->err  = 2.0 * GSL_DBL_EPSILON * (0.375 + fabs(c.val)) / sy;
    result->err += c.err / sy;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    const double sy = sqrt(y);
    gsl_sf_result c;
    cheb_eval_e(&ai02_cs, 16.0/y-1.0, &c);
    result->val = (0.375 + c.val) / sy;
    result->err  = 2.0 * GSL_DBL_EPSILON * (0.375 + fabs(c.val)) / sy;
    result->err += c.err / sy;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}


int gsl_sf_bessel_I0_e(const double x, gsl_sf_result * result)
{
  double y = fabs(x);

  /* CHECK_POINTER(result) */

  if(y < 2.0 * GSL_SQRT_DBL_EPSILON) {
    result->val = 1.0;
    result->err = 0.5*y*y;
    return GSL_SUCCESS;
  }
  else if(y <= 3.0) {
    gsl_sf_result c;
    cheb_eval_e(&bi0_cs, y*y/4.5-1.0, &c);
    result->val  = 2.75 + c.val;
    result->err  = GSL_DBL_EPSILON * (2.75 + fabs(c.val));
    result->err += c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(y < GSL_LOG_DBL_MAX - 1.0) {
    const double ey = exp(y);
    gsl_sf_result b_scaled;
    gsl_sf_bessel_I0_scaled_e(x, &b_scaled);
    result->val  = ey * b_scaled.val;
    result->err  = ey * b_scaled.err + y*GSL_DBL_EPSILON*fabs(result->val);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_I0_scaled(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_I0_scaled_e(x, &result); ) 
}

double gsl_sf_bessel_I0(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_I0_e(x, &result); ) 
}
