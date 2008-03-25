/* specfunc/bessel_Y0.c
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

/* based on SLATEC besy0, 1980 version, w. fullerton */

/* chebyshev expansions

 series for by0        on the interval  0.          to  1.60000d+01
                                        with weighted error   1.20e-17
                                         log weighted error  16.92
                               significant figures required  16.15
                                    decimal places required  17.48
*/

static double by0_data[13] = {
  -0.011277839392865573,
  -0.128345237560420350,
  -0.104378847997942490,
   0.023662749183969695,
  -0.002090391647700486,
   0.000103975453939057,
  -0.000003369747162423,
   0.000000077293842676,
  -0.000000001324976772,
   0.000000000017648232,
  -0.000000000000188105,
   0.000000000000001641,
  -0.000000000000000011
};
static cheb_series by0_cs = {
  by0_data,
  12,
  -1, 1,
  8
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_Y0_e(const double x, gsl_sf_result * result)
{
  const double two_over_pi = 2.0/M_PI;
  const double xmax        = 1.0/GSL_DBL_EPSILON;

  /* CHECK_POINTER(result) */

  if (x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < 4.0) {
    gsl_sf_result J0;
    gsl_sf_result c;
    int stat_J0 = gsl_sf_bessel_J0_e(x, &J0);
    cheb_eval_e(&by0_cs, 0.125*x*x-1.0, &c);
    result->val = two_over_pi*(-M_LN2 + log(x))*J0.val + 0.375 + c.val;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val) + c.err;
    return stat_J0;
  }
  else if(x < xmax) {
    /* Leading behaviour of phase is x, which is exact,
     * so the error is bounded.
     */
    const double z  = 32.0/(x*x) - 1.0;
    gsl_sf_result c1;
    gsl_sf_result c2;
    gsl_sf_result sp;
    const int stat_c1 = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bm0_cs,  z, &c1);
    const int stat_c2 = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bth0_cs, z, &c2);
    const int stat_sp = gsl_sf_bessel_sin_pi4_e(x, c2.val/x, &sp);
    const double sqrtx = sqrt(x);
    const double ampl  = (0.75 + c1.val) / sqrtx;
    result->val  = ampl * sp.val;
    result->err  = fabs(sp.val) * c1.err/sqrtx + fabs(ampl) * sp.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_3(stat_sp, stat_c1, stat_c2);
  }
  else {
    UNDERFLOW_ERROR(result);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Y0(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Y0_e(x, &result));
}
