/* specfunc/expint3.c
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

/* Author: G. Jungman */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_expint.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

static double expint3_data[24] = {
  1.269198414221126014,
 -0.248846446384140982,
  0.80526220717231041e-01,
 -0.25772733251968330e-01,
  0.7599878873073774e-02,
 -0.2030695581940405e-02,
  0.490834586699330e-03,
 -0.107682239142021e-03,
  0.21551726264290e-04,
 -0.3956705137384e-05,
  0.6699240933896e-06,
 -0.105132180807e-06,
  0.15362580199e-07,
 -0.20990960364e-08,
  0.2692109538e-09,
 -0.325195242e-10,
  0.37114816e-11,
 -0.4013652e-12,
  0.412334e-13,
 -0.40338e-14,
  0.3766e-15,
 -0.336e-16,
  0.29e-17,
 -0.2e-18
};
static cheb_series expint3_cs = {
  expint3_data,
  23,
  -1.0, 1.0,
  15
};

static double expint3a_data[23] = {
   1.9270464955068273729,
  -0.349293565204813805e-01,
   0.14503383718983009e-02,
  -0.8925336718327903e-04,
   0.70542392191184e-05,
  -0.6671727454761e-06,
   0.724267589982e-07,
  -0.87825825606e-08,
   0.11672234428e-08,
  -0.1676631281e-09,
   0.257550158e-10,
  -0.41957888e-11,
   0.7201041e-12,
  -0.1294906e-12,
   0.24287e-13,
  -0.47331e-14,
   0.95531e-15,
  -0.1991e-15,
   0.428e-16,
  -0.94e-17,
   0.21e-17,
  -0.5e-18,
   0.1e-18
};
static cheb_series expint3a_cs = {
  expint3a_data,
  22,
  -1.0, 1.0,
  10
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_expint_3_e(const double x, gsl_sf_result * result)
{
  const double val_infinity = 0.892979511569249211;

  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < 1.6*GSL_ROOT3_DBL_EPSILON) {
    result->val = x;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x <= 2.0) {
    const double t = x*x*x/4.0 - 1.0;
    gsl_sf_result result_c;
    cheb_eval_e(&expint3_cs, t, &result_c);
    result->val = x * result_c.val;
    result->err = x * result_c.err;
    return GSL_SUCCESS;
  }
  else if(x < pow(-GSL_LOG_DBL_EPSILON, 1.0/3.0)) {
    const double t = 16.0/(x*x*x) - 1.0;
    const double s = exp(-x*x*x)/(3.0*x*x);
    gsl_sf_result result_c;
    cheb_eval_e(&expint3a_cs, t, &result_c);
    result->val = val_infinity - result_c.val * s;
    result->err = val_infinity * GSL_DBL_EPSILON + s * result_c.err;
    return GSL_SUCCESS;
  }
  else {
    result->val = val_infinity;
    result->err = val_infinity * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_expint_3(double x)
{
  EVAL_RESULT(gsl_sf_expint_3_e(x, &result));
}
