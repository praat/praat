/* specfunc/bessel_sequence.c
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


#define DYDX_p(p,u,x) (-(p)/(x) + (((nu)*(nu))/((x)*(x))-1.0)*(u))
#define DYDX_u(p,u,x) (p)

static int
rk_step(double nu, double x, double dx, double * Jp, double * J)
{
  double p_0 = *Jp;
  double u_0 = *J;

  double p_1 = dx * DYDX_p(p_0, u_0, x);
  double u_1 = dx * DYDX_u(p_0, u_0, x);

  double p_2 = dx * DYDX_p(p_0 + 0.5*p_1, u_0 + 0.5*u_1, x + 0.5*dx);
  double u_2 = dx * DYDX_u(p_0 + 0.5*p_1, u_0 + 0.5*u_1, x + 0.5*dx);

  double p_3 = dx * DYDX_p(p_0 + 0.5*p_2, u_0 + 0.5*u_2, x + 0.5*dx);
  double u_3 = dx * DYDX_u(p_0 + 0.5*p_2, u_0 + 0.5*u_2, x + 0.5*dx);

  double p_4 = dx * DYDX_p(p_0 + p_3, u_0 + u_3, x + dx);
  double u_4 = dx * DYDX_u(p_0 + p_3, u_0 + u_3, x + dx);

  *Jp = p_0 + p_1/6.0 + p_2/3.0 + p_3/3.0 + p_4/6.0;
  *J  = u_0 + u_1/6.0 + u_2/3.0 + u_3/3.0 + u_4/6.0;

  return GSL_SUCCESS;
}


int
gsl_sf_bessel_sequence_Jnu_e(double nu, gsl_mode_t mode, size_t size, double * v)
{
  /* CHECK_POINTER(v) */

  if(nu < 0.0) {
    GSL_ERROR ("domain error", GSL_EDOM);
  }
  else if(size == 0) {
    GSL_ERROR ("error", GSL_EINVAL);
  }
  else {
    const gsl_prec_t goal   = GSL_MODE_PREC(mode);
    const double dx_array[] = { 0.001, 0.03, 0.1 }; /* double, single, approx */
    const double dx_nominal = dx_array[goal];

    const int cnu = (int) ceil(nu);
    const double nu13 = pow(nu,1.0/3.0);
    const double smalls[] = { 0.01, 0.02, 0.4, 0.7, 1.3, 2.0, 2.5, 3.2, 3.5, 4.5, 6.0 };
    const double x_small = ( nu >= 10.0 ? nu - nu13 : smalls[cnu] );

    gsl_sf_result J0, J1;
    double Jp, J;
    double x;
    size_t i = 0;

    /* Calculate the first point. */
    x = v[0];
    gsl_sf_bessel_Jnu_e(nu, x, &J0);
    v[0] = J0.val;
    ++i;

    /* Step over the idiot case where the
     * first point was actually zero.
     */
    if(x == 0.0) {
      if(v[1] <= x) {
        /* Strict ordering failure. */
        GSL_ERROR ("error", GSL_EFAILED);
      }
      x = v[1];
      gsl_sf_bessel_Jnu_e(nu, x, &J0);
      v[1] = J0.val;
      ++i;
    }

    /* Calculate directly as long as the argument
     * is small. This is necessary because the
     * integration is not very good there.
     */
    while(v[i] < x_small && i < size) {
      if(v[i] <= x) {
        /* Strict ordering failure. */
        GSL_ERROR ("error", GSL_EFAILED);
      }
      x = v[i];
      gsl_sf_bessel_Jnu_e(nu, x, &J0);
      v[i] = J0.val;
      ++i;
    }

    /* At this point we are ready to integrate.
     * The value of x is the last calculated
     * point, which has the value J0; v[i] is
     * the next point we need to calculate. We
     * calculate nu+1 at x as well to get
     * the derivative, then we go forward.
     */
    gsl_sf_bessel_Jnu_e(nu+1.0, x, &J1);
    J  = J0.val;
    Jp = -J1.val + nu/x * J0.val;

    while(i < size) {
      const double dv = v[i] - x;
      const int Nd    = (int) ceil(dv/dx_nominal);
      const double dx = dv / Nd;
      double xj;
      int j;

      if(v[i] <= x) {
        /* Strict ordering failure. */
        GSL_ERROR ("error", GSL_EFAILED);
      }

      /* Integrate over interval up to next sample point.
       */
      for(j=0, xj=x; j<Nd; j++, xj += dx) {
        rk_step(nu, xj, dx, &Jp, &J);
      }

      /* Go to next interval. */
      x = v[i];
      v[i] = J;
      ++i;
    }

    return GSL_SUCCESS;
  }
}
