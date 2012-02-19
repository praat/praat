/* specfunc/transport.c
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
#include "gsl_sf_transport.h"

#include "gsl_specfunc__error.h"
#include "gsl_specfunc__check.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

static double transport2_data[18] = {
   1.671760446434538503,
  -0.147735359946794490,
   0.148213819946936338e-01,
  -0.14195330326305613e-02,
   0.1306541324415708e-03,
  -0.117155795867579e-04,
   0.10333498445756e-05,
  -0.901911304223e-07,
   0.78177169833e-08,
  -0.6744565684e-09,
   0.579946394e-10,
  -0.49747619e-11,
   0.425961e-12,
  -0.36422e-13,
   0.3111e-14,
  -0.265e-15,
   0.23e-16,
  -0.19e-17
};
static cheb_series transport2_cs = {
  transport2_data,
  17,
  -1, 1,
  9
};

static double transport3_data[18] = {
   0.762012543243872007,
  -0.105674387705058533,
   0.119778084819657810e-01,
  -0.12144015203698307e-02,
   0.1155099769392855e-03,
  -0.105815992124423e-04,
   0.9474663385302e-06,
  -0.836221212858e-07,
   0.73109099278e-08,
  -0.6350594779e-09,
   0.549118282e-10,
  -0.47321395e-11,
   0.4067695e-12,
  -0.348971e-13,
   0.29892e-14,
  -0.256e-15,
   0.219e-16,
  -0.19e-17
};
static cheb_series transport3_cs = {
  transport3_data,
  17,
  -1, 1,
  9
};


static double transport4_data[18] = {
  0.4807570994615110579,
 -0.8175378810321083956e-01,
  0.1002700665975162973e-01,
 -0.10599339359820151e-02,
  0.1034506245030405e-03,
 -0.96442705485899e-05,
  0.8745544408515e-06,
 -0.779321207981e-07,
  0.68649886141e-08,
 -0.5999571076e-09,
  0.521366241e-10,
 -0.45118382e-11,
  0.3892159e-12,
 -0.334936e-13,
  0.28767e-14,
 -0.2467e-15,
  0.211e-16,
 -0.18e-17
};
static cheb_series transport4_cs = {
  transport4_data,
  17,
  -1, 1,
  9
};


static double transport5_data[18] = {
   0.347777777133910789,
  -0.66456988976050428e-01,
   0.8611072656883309e-02,
  -0.9396682223755538e-03,
   0.936324806081513e-04,
  -0.88571319340833e-05,
   0.811914989145e-06,
  -0.72957654233e-07,
   0.646971455e-08,
  -0.568490283e-09,
   0.49625598e-10,
  -0.4310940e-11,
   0.373100e-12,
  -0.32198e-13,
   0.2772e-14,
  -0.238e-15,
   0.21e-16,
  -0.18e-17
};
static cheb_series transport5_cs = {
  transport5_data,
  17,
  -1, 1,
  9
};


static
double
transport_sumexp(const int numexp, const int order, const double t, double x)
{
  double rk = (double)numexp;
  double sumexp = 0.0;
  int k;
  for(k=1; k<=numexp; k++) {
    double sum2 = 1.0;
    double xk  = 1.0/(rk*x);
    double xk1 = 1.0;
    int j;
    for(j=1; j<=order; j++) {
      sum2 = sum2*xk1*xk + 1.0;
      xk1 += 1.0;
    }
    sumexp *= t;
    sumexp += sum2;
    rk -= 1.0;
  }
  return sumexp;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_transport_2_e(const double x, gsl_sf_result * result)
{
  const double val_infinity = 3.289868133696452873;

  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < 3.0*GSL_SQRT_DBL_EPSILON) {
    result->val = x;
    result->err = GSL_DBL_EPSILON*fabs(x) + x*x/2.0;
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    double t = (x*x/8.0 - 0.5) - 0.5;
    gsl_sf_result result_c;
    cheb_eval_e(&transport2_cs, t, &result_c);
    result->val  = x * result_c.val;
    result->err  = x * result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -GSL_LOG_DBL_EPSILON) {
    const int    numexp = (int)((-GSL_LOG_DBL_EPSILON)/x) + 1;
    const double sumexp = transport_sumexp(numexp, 2, exp(-x), x);
    const double t = 2.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val  = val_infinity;
      result->err  = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + fabs(t) * et);
    }
    return GSL_SUCCESS;
  }
  else if(x < 2.0/GSL_DBL_EPSILON) {
    const int    numexp = 1;
    const double sumexp = transport_sumexp(numexp, 2, 1.0, x);
    const double t = 2.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else {
    const double t = 2.0 * log(x) - x;
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
}


int
gsl_sf_transport_3_e(const double x, gsl_sf_result * result)
{ 
  const double val_infinity = 7.212341418957565712;

  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x < 3.0*GSL_SQRT_DBL_EPSILON) {
    result->val = 0.5*x*x;
    result->err = 2.0 * GSL_DBL_EPSILON * result->val;
    CHECK_UNDERFLOW(result);
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    const double x2 = x*x;
    const double t = (x2/8.0 - 0.5) - 0.5;
    gsl_sf_result result_c;
    cheb_eval_e(&transport3_cs, t, &result_c);
    result->val  = x2 * result_c.val;
    result->err  = x2 * result_c.err;
    result->err += GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -GSL_LOG_DBL_EPSILON) {
    const int    numexp = (int)((-GSL_LOG_DBL_EPSILON)/x) + 1;
    const double sumexp = transport_sumexp(numexp, 3, exp(-x), x);
    const double t = 3.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + fabs(t) * et);
    }
    return GSL_SUCCESS;
  }
  else if(x < 3.0/GSL_DBL_EPSILON) {
    const int    numexp = 1;
    const double sumexp = transport_sumexp(numexp, 3, 1.0, x);
    const double t = 3.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else {
    const double t = 3.0 * log(x) - x;
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
}


int
gsl_sf_transport_4_e(const double x, gsl_sf_result * result)
{
  const double val_infinity = 25.97575760906731660;

  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x < 3.0*GSL_SQRT_DBL_EPSILON) {
    result->val = x*x*x/3.0;
    result->err = 3.0 * GSL_DBL_EPSILON * result->val;
    CHECK_UNDERFLOW(result);
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    const double x2 = x*x;
    const double t = (x2/8.0 - 0.5) - 0.5;
    gsl_sf_result result_c;
    cheb_eval_e(&transport4_cs, t, &result_c);
    result->val  = x2*x * result_c.val;
    result->err  = x2*x * result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -GSL_LOG_DBL_EPSILON) {
    const int    numexp = (int)((-GSL_LOG_DBL_EPSILON)/x) + 1;
    const double sumexp = transport_sumexp(numexp, 4, exp(-x), x);
    const double t = 4.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else if(x < 3.0/GSL_DBL_EPSILON) {
    const int    numexp = 1;
    const double sumexp = transport_sumexp(numexp, 4, 1.0, x);
    const double t = 4.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else {
    const double t = 4.0 * log(x) - x;
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
}


int
gsl_sf_transport_5_e(const double x, gsl_sf_result * result)
{
  const double val_infinity = 124.4313306172043912;

  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x < 3.0*GSL_SQRT_DBL_EPSILON) {
    result->val = x*x*x*x/4.0;
    result->err = 4.0 * GSL_DBL_EPSILON * result->val;
    CHECK_UNDERFLOW(result);
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    const double x2 = x*x;
    const double t = (x2/8.0 - 0.5) - 0.5;
    gsl_sf_result result_c;
    cheb_eval_e(&transport5_cs, t, &result_c);
    result->val  = x2*x2 * result_c.val;
    result->err  = x2*x2 * result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -GSL_LOG_DBL_EPSILON) {
    const int    numexp = (int)((-GSL_LOG_DBL_EPSILON)/x) + 1;
    const double sumexp = transport_sumexp(numexp, 5, exp(-x), x);
    const double t = 5.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else if(x < 3.0/GSL_DBL_EPSILON) {
    const int    numexp = 1;
    const double sumexp = transport_sumexp(numexp, 5, 1.0, x);
    const double t = 5.0 * log(x) - x + log(sumexp);
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
  else {
    const double t = 5.0 * log(x) - x;
    if(t < GSL_LOG_DBL_EPSILON) {
      result->val = val_infinity;
      result->err = 2.0 * GSL_DBL_EPSILON * val_infinity;
    }
    else {
      const double et = exp(t);
      result->val = val_infinity - et;
      result->err = 2.0 * GSL_DBL_EPSILON * (val_infinity + (fabs(t)+1.0) * et);
    }
    return GSL_SUCCESS;
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_transport_2(const double x)
{
  EVAL_RESULT(gsl_sf_transport_2_e(x, &result));
}

double gsl_sf_transport_3(const double x)
{
  EVAL_RESULT(gsl_sf_transport_3_e(x, &result));
}

double gsl_sf_transport_4(const double x)
{
  EVAL_RESULT(gsl_sf_transport_4_e(x, &result));
}

double gsl_sf_transport_5(const double x)
{
  EVAL_RESULT(gsl_sf_transport_5_e(x, &result));
}
