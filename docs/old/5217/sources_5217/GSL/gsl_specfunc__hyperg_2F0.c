/* specfunc/hyperg_2F0.c
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
#include "gsl_sf_hyperg.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__hyperg.h"

int
gsl_sf_hyperg_2F0_e(const double a, const double b, const double x, gsl_sf_result * result)
{
  if(x < 0.0) {
    /* Use "definition" 2F0(a,b,x) = (-1/x)^a U(a,1+a-b,-1/x).
     */
    gsl_sf_result U;
    double pre = pow(-1.0/x, a);
    int stat_U = gsl_sf_hyperg_U_e(a, 1.0+a-b, -1.0/x, &U);
    result->val = pre * U.val;
    result->err = GSL_DBL_EPSILON * fabs(result->val) + pre * U.err;
    return stat_U;
  }
  else if(x == 0.0) {
    result->val = 1.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    /* Use asymptotic series. ??
     */
    /* return hyperg_2F0_series(a, b, x, -1, result, &prec); */
    DOMAIN_ERROR(result);
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_hyperg_2F0(const double a, const double b, const double x)
{
  EVAL_RESULT(gsl_sf_hyperg_2F0_e(a, b, x, &result));
}
