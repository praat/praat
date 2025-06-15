/* specfunc/elementary.c
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
#include "gsl_sf_elementary.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__check.h"

int
gsl_sf_multiply_e(const double x, const double y, gsl_sf_result * result)
{
  const double ax = fabs(x);
  const double ay = fabs(y);

  if(x == 0.0 || y == 0.0) {
    /* It is necessary to eliminate this immediately.
     */
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if((ax <= 1.0 && ay >= 1.0) || (ay <= 1.0 && ax >= 1.0)) {
    /* Straddling 1.0 is always safe.
     */
    result->val = x*y;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    const double f = 1.0 - 2.0 * GSL_DBL_EPSILON;
    const double min = GSL_MIN_DBL(fabs(x), fabs(y));
    const double max = GSL_MAX_DBL(fabs(x), fabs(y));
    if(max < 0.9 * GSL_SQRT_DBL_MAX || min < (f * DBL_MAX)/max) {
      result->val = GSL_COERCE_DBL(x*y);
      result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      CHECK_UNDERFLOW(result);
      return GSL_SUCCESS;
    }
    else {
      OVERFLOW_ERROR(result);
    }
  }
}


int
gsl_sf_multiply_err_e(const double x, const double dx,
                         const double y, const double dy,
                         gsl_sf_result * result)
{
  int status = gsl_sf_multiply_e(x, y, result);
  result->err += fabs(dx*y) + fabs(dy*x);
  return status;
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_multiply(const double x, const double y)
{
  EVAL_RESULT(gsl_sf_multiply_e(x, y, &result));
}

