/* specfunc/bessel_amp_phase.c
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
#include "gsl_sf_bessel.h"
#include "gsl_specfunc__bessel_amp_phase.h"

/* chebyshev expansions for amplitude and phase
   functions used in bessel evaluations

   These are the same for J0,Y0 and for J1,Y1, so
   they sit outside those functions.
*/
        
static double bm0_data[21] = {
   0.09284961637381644,
  -0.00142987707403484,
   0.00002830579271257,
  -0.00000143300611424,
   0.00000012028628046,
  -0.00000001397113013,
   0.00000000204076188,
  -0.00000000035399669,
   0.00000000007024759,
  -0.00000000001554107,
   0.00000000000376226,
  -0.00000000000098282,
   0.00000000000027408,
  -0.00000000000008091,
   0.00000000000002511,
  -0.00000000000000814,
   0.00000000000000275,
  -0.00000000000000096,
   0.00000000000000034,
  -0.00000000000000012,
   0.00000000000000004
}; 
const cheb_series _gsl_sf_bessel_amp_phase_bm0_cs = {
  bm0_data,
  20,
  -1, 1,
  10
};
      
static double bth0_data[24] = {
  -0.24639163774300119,
   0.001737098307508963,
  -0.000062183633402968,
   0.000004368050165742,
  -0.000000456093019869,
   0.000000062197400101,
  -0.000000010300442889,
   0.000000001979526776,
  -0.000000000428198396,
   0.000000000102035840,
  -0.000000000026363898,
   0.000000000007297935,
  -0.000000000002144188,
   0.000000000000663693,
  -0.000000000000215126,
   0.000000000000072659,
  -0.000000000000025465,
   0.000000000000009229,
  -0.000000000000003448,
   0.000000000000001325,
  -0.000000000000000522,
   0.000000000000000210,
  -0.000000000000000087,
   0.000000000000000036
};
const cheb_series _gsl_sf_bessel_amp_phase_bth0_cs = {
  bth0_data,
  23,
  -1, 1,
  12
};


static double bm1_data[21] = {
   0.1047362510931285, 
   0.00442443893702345,
  -0.00005661639504035,
   0.00000231349417339,
  -0.00000017377182007,
   0.00000001893209930,
  -0.00000000265416023,
   0.00000000044740209,
  -0.00000000008691795,
   0.00000000001891492,
  -0.00000000000451884,
   0.00000000000116765,
  -0.00000000000032265,
   0.00000000000009450,
  -0.00000000000002913,
   0.00000000000000939,
  -0.00000000000000315,
   0.00000000000000109,
  -0.00000000000000039,
   0.00000000000000014,
  -0.00000000000000005,
}; 
const cheb_series _gsl_sf_bessel_amp_phase_bm1_cs = {
  bm1_data,
  20,
  -1, 1,
  10
};

static double bth1_data[24] = {
   0.74060141026313850, 
  -0.004571755659637690,
   0.000119818510964326,
  -0.000006964561891648,
   0.000000655495621447,
  -0.000000084066228945,
   0.000000013376886564,
  -0.000000002499565654,
   0.000000000529495100,
  -0.000000000124135944,
   0.000000000031656485,
  -0.000000000008668640,
   0.000000000002523758,
  -0.000000000000775085,
   0.000000000000249527,
  -0.000000000000083773,
   0.000000000000029205,
  -0.000000000000010534,
   0.000000000000003919,
  -0.000000000000001500,
   0.000000000000000589,
  -0.000000000000000237,
   0.000000000000000097,
  -0.000000000000000040,
};
const cheb_series _gsl_sf_bessel_amp_phase_bth1_cs = {
  bth1_data,
  23,
  -1, 1,
  12
};


int
gsl_sf_bessel_asymp_Mnu_e(const double nu, const double x, double * result)
{
  const double r  = 2.0*nu/x;
  const double r2 = r*r;
  const double x2 = x*x;
  const double term1 = (r2-1.0/x2)/8.0;
  const double term2 = (r2-1.0/x2)*(r2-9.0/x2)*3.0/128.0;
  const double Mnu2_c = 2.0/(M_PI) * (1.0 + term1 + term2);
  *result = sqrt(Mnu2_c)/sqrt(x); /* will never underflow this way */
  return GSL_SUCCESS;
}


int
gsl_sf_bessel_asymp_thetanu_corr_e(const double nu, const double x, double * result)
{
  const double r  = 2.0*nu/x;
  const double r2 = r*r;
  const double x2 = x*x;
  const double term1 = x*(r2 - 1.0/x2)/8.0;
  const double term2 = x*(r2 - 1.0/x2)*(r2 - 25.0/x2)/384.0;
  *result = (-0.25*M_PI + term1 + term2);
  return GSL_SUCCESS;
}
