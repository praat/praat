/* specfunc/shint.c
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
#include "gsl_sf_expint.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC shi.f, W. Fullerton

 series for shi  on the interval  0.00000e+00 to  1.40625e-01
                                        with weighted error   4.67e-20
                                         log weighted error  19.33
                               significant figures required  17.07
                                    decimal places required  19.75
*/
static double shi_data[7] = {
   0.0078372685688900950695,
   0.0039227664934234563973,
   0.0000041346787887617267,
   0.0000000024707480372883,
   0.0000000000009379295591,
   0.0000000000000002451817,
   0.0000000000000000000467
};
static cheb_series shi_cs = {
  shi_data,
  6,
  -1, 1,
  6
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_Shi_e(const double x, gsl_sf_result * result)
{
  const double xsml = GSL_SQRT_DBL_EPSILON;  /* sqrt (r1mach(3)) */
  const double ax   = fabs(x);

  if(ax < xsml) {
    result->val = x;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(ax <= 0.375) {
    gsl_sf_result result_c;
    cheb_eval_e(&shi_cs, 128.0*x*x/9.0-1.0, &result_c);
    result->val  = x * (1.0 + result_c.val);
    result->err  = x * result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result result_Ei;
    gsl_sf_result result_E1;
    int status_Ei = gsl_sf_expint_Ei_e(x, &result_Ei);
    int status_E1 = gsl_sf_expint_E1_e(x, &result_E1);
    result->val  = 0.5*(result_Ei.val + result_E1.val);
    result->err  = 0.5*(result_Ei.err + result_E1.err);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    if(status_Ei == GSL_EUNDRFLW && status_E1 == GSL_EUNDRFLW) {
      GSL_ERROR ("underflow", GSL_EUNDRFLW);
    }
    else if(status_Ei == GSL_EOVRFLW || status_E1 == GSL_EOVRFLW) {
      GSL_ERROR ("overflow", GSL_EOVRFLW);
    }
    else {
      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_Chi_e(const double x, gsl_sf_result * result)
{
  gsl_sf_result result_Ei;
  gsl_sf_result result_E1;
  int status_Ei = gsl_sf_expint_Ei_e(x, &result_Ei);
  int status_E1 = gsl_sf_expint_E1_e(x, &result_E1);
  if(status_Ei == GSL_EDOM || status_E1 == GSL_EDOM) {
    DOMAIN_ERROR(result);
  }
  else if(status_Ei == GSL_EUNDRFLW && status_E1 == GSL_EUNDRFLW) {
    UNDERFLOW_ERROR(result);
  }
  else if(status_Ei == GSL_EOVRFLW || status_E1 == GSL_EOVRFLW) {
    OVERFLOW_ERROR(result);
  }
  else {
    result->val  = 0.5 * (result_Ei.val - result_E1.val);
    result->err  = 0.5 * (result_Ei.err + result_E1.err);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_Shi(const double x)
{
  EVAL_RESULT(gsl_sf_Shi_e(x, &result));
}

double gsl_sf_Chi(const double x)
{
  EVAL_RESULT(gsl_sf_Chi_e(x, &result));
}
