/* specfunc/bessel_J1.c
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

#define ROOT_EIGHT (2.0*M_SQRT2)

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/


/* based on SLATEC besj1, 1983 version, w. fullerton */

/* chebyshev expansions

 series for bj1        on the interval  0.          to  1.60000d+01
                                        with weighted error   4.48e-17
                                         log weighted error  16.35
                               significant figures required  15.77
                                    decimal places required  16.89

*/
static double bj1_data[12] = {
  -0.11726141513332787,
  -0.25361521830790640,
   0.050127080984469569,
  -0.004631514809625081,
   0.000247996229415914,
  -0.000008678948686278,
   0.000000214293917143,
  -0.000000003936093079,
   0.000000000055911823,
  -0.000000000000632761,
   0.000000000000005840,
  -0.000000000000000044,
};
static cheb_series bj1_cs = {
  bj1_data,
  11,
  -1, 1,
  8
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_J1_e(const double x, gsl_sf_result * result)
{
  double y = fabs(x);

  /* CHECK_POINTER(result) */

  if(y == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(y < 2.0*GSL_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(y < ROOT_EIGHT * GSL_SQRT_DBL_EPSILON) {
    result->val = 0.5*x;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(y < 4.0) {
    gsl_sf_result c;
    cheb_eval_e(&bj1_cs, 0.125*y*y-1.0, &c);
    result->val = x * (0.25 + c.val);
    result->err = fabs(x * c.err);
    return GSL_SUCCESS;
  }
  else {
    /* Because the leading term in the phase is y,
     * which we assume is exactly known, the error
     * in the cos() evaluation is bounded.
     */
    const double z  = 32.0/(y*y) - 1.0;
    gsl_sf_result ca;
    gsl_sf_result ct;
    gsl_sf_result sp;
    const int stat_ca = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bm1_cs,  z, &ca);
    const int stat_ct = cheb_eval_e(&_gsl_sf_bessel_amp_phase_bth1_cs, z, &ct);
    const int stat_sp = gsl_sf_bessel_sin_pi4_e(y, ct.val/y, &sp);
    const double sqrty = sqrt(y);
    const double ampl  = (0.75 + ca.val) / sqrty;
    result->val  = (x < 0.0 ? -ampl : ampl) * sp.val;
    result->err  = fabs(sp.val) * ca.err/sqrty + fabs(ampl) * sp.err;
    result->err += GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_3(stat_ca, stat_ct, stat_sp);
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_J1(const double x)
{
  EVAL_RESULT(gsl_sf_bessel_J1_e(x, &result));
}
