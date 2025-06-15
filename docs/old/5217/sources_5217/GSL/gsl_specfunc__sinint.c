/* specfunc/sinint.c
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
#include "gsl_sf_expint.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC r9sifg.f, W. Fullerton */

/*
 series for f1   on the interval  2.00000e-02 to  6.25000e-02
                                        with weighted error   2.82e-17
                                         log weighted error  16.55
                               significant figures required  15.36
                                    decimal places required  17.20
*/
static double f1_data[20] = {
   -0.1191081969051363610,
   -0.0247823144996236248,
    0.0011910281453357821,
   -0.0000927027714388562,
    0.0000093373141568271,
   -0.0000011058287820557,
    0.0000001464772071460,
   -0.0000000210694496288,
    0.0000000032293492367,
   -0.0000000005206529618,
    0.0000000000874878885,
   -0.0000000000152176187,
    0.0000000000027257192,
   -0.0000000000005007053,
    0.0000000000000940241,
   -0.0000000000000180014,
    0.0000000000000035063,
   -0.0000000000000006935,
    0.0000000000000001391,
   -0.0000000000000000282
};
static cheb_series f1_cs = {
  f1_data,
  19,
  -1, 1,
  10
};

/*

 series for f2   on the interval  0.00000e+00 to  2.00000e-02
                                        with weighted error   4.32e-17
                                         log weighted error  16.36
                               significant figures required  14.75
                                    decimal places required  17.10
*/
static double f2_data[29] = {
   -0.0348409253897013234,
   -0.0166842205677959686,
    0.0006752901241237738,
   -0.0000535066622544701,
    0.0000062693421779007,
   -0.0000009526638801991,
    0.0000001745629224251,
   -0.0000000368795403065,
    0.0000000087202677705,
   -0.0000000022601970392,
    0.0000000006324624977,
   -0.0000000001888911889,
    0.0000000000596774674,
   -0.0000000000198044313,
    0.0000000000068641396,
   -0.0000000000024731020,
    0.0000000000009226360,
   -0.0000000000003552364,
    0.0000000000001407606,
   -0.0000000000000572623,
    0.0000000000000238654,
   -0.0000000000000101714,
    0.0000000000000044259,
   -0.0000000000000019634,
    0.0000000000000008868,
   -0.0000000000000004074,
    0.0000000000000001901,
   -0.0000000000000000900,
    0.0000000000000000432
};
static cheb_series f2_cs = {
  f2_data,
  28,
  -1, 1,
  14
};

/*

 series for g1   on the interval  2.00000e-02 to  6.25000e-02
                                        with weighted error   5.48e-17
                                         log weighted error  16.26
                               significant figures required  15.47
                                    decimal places required  16.92
*/
static double g1_data[21] = {
   -0.3040578798253495954,
   -0.0566890984597120588,
    0.0039046158173275644,
   -0.0003746075959202261,
    0.0000435431556559844,
   -0.0000057417294453025,
    0.0000008282552104503,
   -0.0000001278245892595,
    0.0000000207978352949,
   -0.0000000035313205922,
    0.0000000006210824236,
   -0.0000000001125215474,
    0.0000000000209088918,
   -0.0000000000039715832,
    0.0000000000007690431,
   -0.0000000000001514697,
    0.0000000000000302892,
   -0.0000000000000061400,
    0.0000000000000012601,
   -0.0000000000000002615,
    0.0000000000000000548
};
static cheb_series g1_cs = {
  g1_data,
  20,
  -1, 1,
  13
};

/*

 series for g2   on the interval  0.00000e+00 to  2.00000e-02
                                        with weighted error   5.01e-17
                                         log weighted error  16.30
                               significant figures required  15.12
                                    decimal places required  17.07
*/
static double g2_data[34] = {
   -0.0967329367532432218,
   -0.0452077907957459871,
    0.0028190005352706523,
   -0.0002899167740759160,
    0.0000407444664601121,
   -0.0000071056382192354,
    0.0000014534723163019,
   -0.0000003364116512503,
    0.0000000859774367886,
   -0.0000000238437656302,
    0.0000000070831906340,
   -0.0000000022318068154,
    0.0000000007401087359,
   -0.0000000002567171162,
    0.0000000000926707021,
   -0.0000000000346693311,
    0.0000000000133950573,
   -0.0000000000053290754,
    0.0000000000021775312,
   -0.0000000000009118621,
    0.0000000000003905864,
   -0.0000000000001708459,
    0.0000000000000762015,
   -0.0000000000000346151,
    0.0000000000000159996,
   -0.0000000000000075213,
    0.0000000000000035970,
   -0.0000000000000017530,
    0.0000000000000008738,
   -0.0000000000000004487,
    0.0000000000000002397,
   -0.0000000000000001347,
    0.0000000000000000801,
   -0.0000000000000000501
};
static cheb_series g2_cs = {
  g2_data,
  33,
  -1, 1,
  20
};


/* x >= 4.0 */
static void fg_asymp(const double x, gsl_sf_result * f, gsl_sf_result * g)
{
  /*
      xbig = sqrt (1.0/r1mach(3))
      xmaxf = exp (amin1(-alog(r1mach(1)), alog(r1mach(2))) - 0.01)
      xmaxg = 1.0/sqrt(r1mach(1))
      xbnd = sqrt(50.0)
  */
  const double xbig  = 1.0/GSL_SQRT_DBL_EPSILON;
  const double xmaxf = 1.0/GSL_DBL_MIN;
  const double xmaxg = 1.0/GSL_SQRT_DBL_MIN;
  const double xbnd  = 7.07106781187;

  const double x2 = x*x;

  if(x <= xbnd) {
    gsl_sf_result result_c1;
    gsl_sf_result result_c2;
    cheb_eval_e(&f1_cs, (1.0/x2-0.04125)/0.02125, &result_c1);
    cheb_eval_e(&g1_cs, (1.0/x2-0.04125)/0.02125, &result_c2);
    f->val = (1.0 + result_c1.val)/x;
    g->val = (1.0 + result_c2.val)/x2;
    f->err = result_c1.err/x  + 2.0 * GSL_DBL_EPSILON * fabs(f->val);
    g->err = result_c2.err/x2 + 2.0 * GSL_DBL_EPSILON * fabs(g->val);
  }
  else if(x <= xbig) {
    gsl_sf_result result_c1;
    gsl_sf_result result_c2;
    cheb_eval_e(&f2_cs, 100.0/x2-1.0, &result_c1);
    cheb_eval_e(&g2_cs, 100.0/x2-1.0, &result_c2);
    f->val = (1.0 + result_c1.val)/x;
    g->val = (1.0 + result_c2.val)/x2;
    f->err = result_c1.err/x  + 2.0 * GSL_DBL_EPSILON * fabs(f->val);
    g->err = result_c2.err/x2 + 2.0 * GSL_DBL_EPSILON * fabs(g->val);
  }
  else {
    f->val = (x < xmaxf ? 1.0/x  : 0.0);
    g->val = (x < xmaxg ? 1.0/x2 : 0.0);
    f->err = 2.0 * GSL_DBL_EPSILON * fabs(f->val);
    g->err = 2.0 * GSL_DBL_EPSILON * fabs(g->val);
  }

  return;
}


/* based on SLATEC si.f, W. Fullerton

 series for si   on the interval  0.00000e+00 to  1.60000e+01
                                        with weighted error   1.22e-17
                                         log weighted error  16.91
                               significant figures required  16.37
                                    decimal places required  17.45
*/

static double si_data[12] = {
  -0.1315646598184841929,
  -0.2776578526973601892,
   0.0354414054866659180,
  -0.0025631631447933978,
   0.0001162365390497009,
  -0.0000035904327241606,
   0.0000000802342123706,
  -0.0000000013562997693,
   0.0000000000179440722,
  -0.0000000000001908387,
   0.0000000000000016670,
  -0.0000000000000000122
};

static cheb_series si_cs = {
  si_data,
  11,
  -1, 1,
  9
};

/*
 series for ci   on the interval  0.00000e+00 to  1.60000e+01
                                        with weighted error   1.94e-18
                                         log weighted error  17.71
                               significant figures required  17.74
                                    decimal places required  18.27
*/
static double ci_data[13] = {
   -0.34004281856055363156,
   -1.03302166401177456807,
    0.19388222659917082877,
   -0.01918260436019865894,
    0.00110789252584784967,
   -0.00004157234558247209,
    0.00000109278524300229,
   -0.00000002123285954183,
    0.00000000031733482164,
   -0.00000000000376141548,
    0.00000000000003622653,
   -0.00000000000000028912,
    0.00000000000000000194
};
static cheb_series ci_cs = {
  ci_data,
  12,
  -1, 1,
  9
};


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_Si_e(const double x, gsl_sf_result * result)
{
  double ax = fabs(x);
  
  /* CHECK_POINTER(result) */

  if(ax < GSL_SQRT_DBL_EPSILON) {
    result->val = x;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(ax <= 4.0) {
    gsl_sf_result result_c;
    cheb_eval_e(&si_cs, (x*x-8.0)*0.125, &result_c);
    result->val  =  x * (0.75 + result_c.val);
    result->err  = ax * result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    /* Note there is no loss of precision
     * here bcause of the leading constant.
     */
    gsl_sf_result f;
    gsl_sf_result g;
    fg_asymp(ax, &f, &g);
    result->val  = 0.5 * M_PI - f.val*cos(ax) - g.val*sin(ax);
    result->err  = f.err + g.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    if(x < 0.0) result->val = -result->val;
    return GSL_SUCCESS;
  }
}


int gsl_sf_Ci_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x <= 4.0) {
    const double lx = log(x);
    const double y  = (x*x-8.0)*0.125;
    gsl_sf_result result_c;
    cheb_eval_e(&ci_cs, y, &result_c);
    result->val  = lx - 0.5 + result_c.val;
    result->err  = 2.0 * GSL_DBL_EPSILON * (fabs(lx) + 0.5) + result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    gsl_sf_result sin_result;
    gsl_sf_result cos_result;
    int stat_sin = gsl_sf_sin_e(x, &sin_result);
    int stat_cos = gsl_sf_cos_e(x, &cos_result);
    gsl_sf_result f;
    gsl_sf_result g;
    fg_asymp(x, &f, &g);
    result->val  = f.val*sin_result.val - g.val*cos_result.val;
    result->err  = fabs(f.err*sin_result.val);
    result->err += fabs(g.err*cos_result.val);
    result->err += fabs(f.val*sin_result.err);
    result->err += fabs(g.val*cos_result.err);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_ERROR_SELECT_2(stat_sin, stat_cos);
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_Si(const double x)
{
  EVAL_RESULT(gsl_sf_Si_e(x, &result));
}

double gsl_sf_Ci(const double x)
{
  EVAL_RESULT(gsl_sf_Ci_e(x, &result));
}
