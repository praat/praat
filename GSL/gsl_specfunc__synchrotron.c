/* specfunc/synchrotron.c
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
#include "gsl_sf_pow_int.h"
#include "gsl_sf_synchrotron.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

static double synchrotron1_data[13] = {
  30.364682982501076273,
  17.079395277408394574,
   4.560132133545072889,
   0.549281246730419979,
   0.372976075069301172e-01,
   0.161362430201041242e-02,
   0.481916772120371e-04,
   0.10512425288938e-05,
   0.174638504670e-07,
   0.22815486544e-09,
   0.240443082e-11,
   0.2086588e-13,
   0.15167e-15
};
static cheb_series synchrotron1_cs = {
  synchrotron1_data,
  12,
  -1.0, 1.0,
  9
};

static double synchrotron2_data[12] = {
  0.4490721623532660844,
  0.898353677994187218e-01,
  0.81044573772151290e-02,
  0.4261716991089162e-03,
  0.147609631270746e-04,
  0.3628633615300e-06,
  0.66634807498e-08,
  0.949077166e-10,
  0.1079125e-11,
  0.10022e-13,
  0.77e-16,
  0.5e-18
};
static cheb_series synchrotron2_cs = {
  synchrotron2_data,
  11,
  -1.0, 1.0,
  7
};

static double synchrotron1a_data[23] = {
  2.1329305161355000985,
  0.741352864954200240e-01,
  0.86968099909964198e-02,
  0.11703826248775692e-02,
  0.1645105798619192e-03,
  0.240201021420640e-04,
  0.35827756389389e-05,
  0.5447747626984e-06,
  0.838802856196e-07,
  0.13069882684e-07,
  0.2053099071e-08,
  0.325187537e-09,
  0.517914041e-10,
  0.83002988e-11,
  0.13352728e-11,
  0.2159150e-12,
  0.349967e-13,
  0.56994e-14,
  0.9291e-15,
  0.152e-15,
  0.249e-16,
  0.41e-17,
  0.7e-18
};
static cheb_series synchrotron1a_cs = {
  synchrotron1a_data,
  22,
  -1.0, 1.0,
  11
};

static double synchrotron21_data[13] = {
  38.617839923843085480,
  23.037715594963734597,
  5.3802499868335705968,
  0.6156793806995710776,
  0.406688004668895584e-01,
  0.17296274552648414e-02,
  0.51061258836577e-04,
  0.110459595022e-05,
  0.18235530206e-07,
  0.2370769803e-09,
  0.24887296e-11,
  0.21529e-13,
  0.156e-15
};
static cheb_series synchrotron21_cs = {
  synchrotron21_data,
  12,
  -1.0, 1.0,
  9
};

static double synchrotron22_data[13] = {
   7.9063148270660804288,
   3.1353463612853425684,
   0.4854879477453714538,
   0.394816675827237234e-01,
   0.19661622334808802e-02,
   0.659078932293042e-04,
   0.15857561349856e-05,
   0.286865301123e-07,
   0.4041202360e-09,
   0.45568444e-11,
   0.420459e-13,
   0.3232e-15,
   0.21e-17
};
static cheb_series synchrotron22_cs = {
  synchrotron22_data,
  12,
  -1.0, 1.0,
  8
};

static double synchrotron2a_data[17] = {
  2.020337094170713600,
  0.10956237121807404e-01,
  0.8542384730114676e-03,
  0.723430242132822e-04,
  0.63124427962699e-05,
  0.5648193141174e-06,
  0.512832480138e-07,
  0.47196532914e-08,
  0.4380744214e-09,
  0.410268149e-10,
  0.38623072e-11,
  0.3661323e-12,
  0.348023e-13,
  0.33301e-14,
  0.319e-15,
  0.307e-16,
  0.3e-17
};
static cheb_series synchrotron2a_cs = {
  synchrotron2a_data,
  16,
  -1.0, 1.0,
  8
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_synchrotron_1_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < 2.0*M_SQRT2 * GSL_SQRT_DBL_EPSILON) {
    /* BJG: added first order correction term.  The taylor series
       is  S1(x) = ((4pi)/(sqrt(3)gamma(1/3))) * (x/2)^(1/3) 
       * (1 - (gamma(1/3)/2)*(x/2)^2/3 + (3/4) * (x/2)^2 ....) */
    double z = pow(x, 1.0/3.0);
    double cf = 1 - 8.43812762813205e-01 * z * z;
    result->val = 2.14952824153447863671 * z * cf;
    result->err = GSL_DBL_EPSILON * result->val;
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    const double c0   = M_PI/M_SQRT3;
    const double px   = pow(x,1.0/3.0);
    const double px11 = gsl_sf_pow_int(px,11);
    const double t = x*x/8.0 - 1.0;
    gsl_sf_result result_c1;
    gsl_sf_result result_c2;
    cheb_eval_e(&synchrotron1_cs, t, &result_c1);
    cheb_eval_e(&synchrotron2_cs, t, &result_c2);
    result->val  = px * result_c1.val - px11 * result_c2.val - c0 * x;
    result->err  = px * result_c1.err + px11 * result_c2.err + c0 * x * GSL_DBL_EPSILON;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -8.0*GSL_LOG_DBL_MIN/7.0) {
    const double c0 = 0.2257913526447274323630976; /* log(sqrt(pi/2)) */
    const double t = (12.0 - x) / (x + 4.0);
    gsl_sf_result result_c1;
    cheb_eval_e(&synchrotron1a_cs, t, &result_c1);
    result->val = sqrt(x) * result_c1.val * exp(c0 - x);
    result->err = 2.0 * GSL_DBL_EPSILON * result->val * (fabs(c0-x)+1.0);
    return GSL_SUCCESS;
  }
  else {
    UNDERFLOW_ERROR(result);
  }
}


int gsl_sf_synchrotron_2_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x < 2.0*M_SQRT2*GSL_SQRT_DBL_EPSILON) {
    /* BJG: added first order correction term.  The taylor series
       is  S2(x) = ((2pi)/(sqrt(3)*gamma(1/3))) * (x/2)^(1/3) 
       * (1 - (gamma(1/3)/gamma(4/3))*(x/2)^(4/3) + (gamma(1/3)/gamma(4/3))*(x/2)^2...) */

    double z = pow(x, 1.0/3.0);
    double cf = 1 - 1.17767156510235e+00 * z * x;
    result->val = 1.07476412076723931836 * z * cf ;
    result->err = 2.0 * GSL_DBL_EPSILON * result->val;
    return GSL_SUCCESS;
  }
  else if(x <= 4.0) {
    const double px  = pow(x, 1.0/3.0);
    const double px5 = gsl_sf_pow_int(px,5);
    const double t = x*x/8.0 - 1.0;
    gsl_sf_result cheb1;
    gsl_sf_result cheb2;
    cheb_eval_e(&synchrotron21_cs, t, &cheb1);
    cheb_eval_e(&synchrotron22_cs, t, &cheb2);
    result->val  = px * cheb1.val - px5 * cheb2.val;
    result->err  = px * cheb1.err + px5 * cheb2.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < -8.0*GSL_LOG_DBL_MIN/7.0) {
    const double c0 = 0.22579135264472743236;   /* log(sqrt(pi/2)) */
    const double t  = (10.0 - x) / (x + 2.0);
    gsl_sf_result cheb1;
    cheb_eval_e(&synchrotron2a_cs, t, &cheb1);
    result->val = sqrt(x) * exp(c0-x) * cheb1.val;
    result->err = GSL_DBL_EPSILON * result->val * (fabs(c0-x)+1.0);
    return GSL_SUCCESS;
  }
  else {
    UNDERFLOW_ERROR(result);
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_synchrotron_1(const double x)
{
  EVAL_RESULT(gsl_sf_synchrotron_1_e(x, &result));
}

double gsl_sf_synchrotron_2(const double x)
{
  EVAL_RESULT(gsl_sf_synchrotron_2_e(x, &result));
}
