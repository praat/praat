/* specfunc/beta.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Author:  G. Jungman */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_log.h"
#include "gsl_sf_psi.h"
#include "gsl_sf_gamma.h"

#include "gsl_sf__error.h"

int
gsl_sf_lnbeta_e(const double x, const double y, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0 || y <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else {
    const double max = GSL_MAX(x,y);
    const double min = GSL_MIN(x,y);
    const double rat = min/max;

    if(rat < 0.2) {
      /* min << max, so be careful
       * with the subtraction
       */
      double lnpre_val;
      double lnpre_err;
      double lnpow_val;
      double lnpow_err;
      double t1, t2, t3;
      gsl_sf_result lnopr;
      gsl_sf_result gsx, gsy, gsxy;
      gsl_sf_gammastar_e(x, &gsx);
      gsl_sf_gammastar_e(y, &gsy);
      gsl_sf_gammastar_e(x+y, &gsxy);
      gsl_sf_log_1plusx_e(rat, &lnopr);
      lnpre_val = log(gsx.val*gsy.val/gsxy.val * M_SQRT2*M_SQRTPI);
      lnpre_err = gsx.err/gsx.val + gsy.err/gsy.val + gsxy.err/gsxy.val;
      t1 = min*log(rat);
      t2 = 0.5*log(min);
      t3 = (x+y-0.5)*lnopr.val;
      lnpow_val  = t1 - t2 - t3;
      lnpow_err  = GSL_DBL_EPSILON * (fabs(t1) + fabs(t2) + fabs(t3));
      lnpow_err += fabs(x+y-0.5) * lnopr.err;
      result->val  = lnpre_val + lnpow_val;
      result->err  = lnpre_err + lnpow_err;
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
    else {
      gsl_sf_result lgx, lgy, lgxy;
      int stat_gx  = gsl_sf_lngamma_e(x, &lgx);
      int stat_gy  = gsl_sf_lngamma_e(y, &lgy);
      int stat_gxy = gsl_sf_lngamma_e(x+y, &lgxy);
      result->val  = lgx.val + lgy.val - lgxy.val;
      result->err  = lgx.err + lgy.err + lgxy.err;
      result->err += GSL_DBL_EPSILON * (fabs(lgx.val) + fabs(lgy.val) + fabs(lgxy.val));
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return GSL_ERROR_SELECT_3(stat_gx, stat_gy, stat_gxy);
    }
  }
}


int
gsl_sf_beta_e(const double x, const double y, gsl_sf_result * result)
{
  if(x < 50.0 && y < 50.0) {
    gsl_sf_result gx, gy, gxy;
    gsl_sf_gamma_e(x, &gx);
    gsl_sf_gamma_e(y, &gy);
    gsl_sf_gamma_e(x+y, &gxy);
    result->val  = (gx.val*gy.val)/gxy.val;
    result->err  = gx.err * gy.val/gxy.val;
    result->err += gy.err * gx.val/gxy.val;
    result->err += (gx.val*gy.val)/(gxy.val*gxy.val) * gxy.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result lb;
    int stat_lb = gsl_sf_lnbeta_e(x, y, &lb);
    if(stat_lb == GSL_SUCCESS) {
      return gsl_sf_exp_err_e(lb.val, lb.err, result);
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
      return stat_lb;
    }
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_sf__eval.h"

double gsl_sf_lnbeta(const double x, const double y)
{
  EVAL_RESULT(gsl_sf_lnbeta_e(x, y, &result));
}

double gsl_sf_beta(const double x, const double y)
{
  EVAL_RESULT(gsl_sf_beta_e(x, y, &result));
}
