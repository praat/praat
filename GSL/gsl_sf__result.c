/* specfunc/result.c
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
#include "gsl_sf_result.h"


int
gsl_sf_result_smash_e(const gsl_sf_result_e10 * re, gsl_sf_result * r)
{
  if(re->e10 == 0) {
    /* nothing to smash */
    r->val = re->val;
    r->err = re->err;
    return GSL_SUCCESS;
  }
  else {
    const double av = fabs(re->val);
    const double ae = fabs(re->err);

    if(   GSL_SQRT_DBL_MIN < av && av < GSL_SQRT_DBL_MAX
       && GSL_SQRT_DBL_MIN < ae && ae < GSL_SQRT_DBL_MAX
       && 0.49*GSL_LOG_DBL_MIN  < re->e10 && re->e10 < 0.49*GSL_LOG_DBL_MAX
       ) {
      const double scale = exp(re->e10 * M_LN10);
      r->val = re->val * scale;
      r->err = re->err * scale;
      return GSL_SUCCESS;
    }
    else {
      return gsl_sf_exp_mult_err_e(re->e10*M_LN10, 0.0, re->val, re->err, r);
    }
  }
/*
  int stat_v;
  int stat_e;

  if(re->val == 0.0) {
    r->val = 0.0;
    stat_v = GSL_SUCCESS;
  }
  else {
    gsl_sf_result r_val;
    const double s = GSL_SIGN(re->val);
    const double x_v = re->e10*M_LN10 + log(fabs(re->val));
    stat_v = gsl_sf_exp_e(x_v, &r_val);
    r->val = s * r_val.val;
  }

  if(re->err == 0.0) {
    r->err = 0.0;
    stat_e = GSL_SUCCESS;
  }
  else if(re->val != 0.0) {
    r->err = fabs(r->val * re->err/re->val);
    stat_e = GSL_SUCCESS;
  }
  else {
    gsl_sf_result r_err;
    const double x_e = re->e10*M_LN10 + log(fabs(re->err));
    stat_e = gsl_sf_exp_e(x_e, &r_err);
    r->err = r_err.val;
  }

  return GSL_ERROR_SELECT_2(stat_v, stat_e);
*/
}

