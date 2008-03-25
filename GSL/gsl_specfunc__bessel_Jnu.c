/* specfunc/bessel_Jnu.c
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

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel.h"
#include "gsl_specfunc__bessel_olver.h"
#include "gsl_specfunc__bessel_temme.h"


/* Evaluate at large enough nu to apply asymptotic
 * results and apply backward recurrence.
 */
#if 0
static
int
bessel_J_recur_asymp(const double nu, const double x,
                     gsl_sf_result * Jnu, gsl_sf_result * Jnup1)
{
  const double nu_cut = 25.0;
  int n;
  int steps = ceil(nu_cut - nu) + 1;

  gsl_sf_result r_Jnp1;
  gsl_sf_result r_Jn;
  int stat_O1 = gsl_sf_bessel_Jnu_asymp_Olver_e(nu + steps + 1.0, x, &r_Jnp1);
  int stat_O2 = gsl_sf_bessel_Jnu_asymp_Olver_e(nu + steps,       x, &r_Jn);
  double r_fe = fabs(r_Jnp1.err/r_Jnp1.val) + fabs(r_Jn.err/r_Jn.val);
  double Jnp1 = r_Jnp1.val;
  double Jn   = r_Jn.val;
  double Jnm1;
  double Jnp1_save;

  for(n=steps; n>0; n--) {
    Jnm1 = 2.0*(nu+n)/x * Jn - Jnp1;
    Jnp1 = Jn;
    Jnp1_save = Jn;
    Jn   = Jnm1;
  }

  Jnu->val = Jn;
  Jnu->err = (r_fe + GSL_DBL_EPSILON * (steps + 1.0)) * fabs(Jn);
  Jnup1->val = Jnp1_save;
  Jnup1->err = (r_fe + GSL_DBL_EPSILON * (steps + 1.0)) * fabs(Jnp1_save);

  return GSL_ERROR_SELECT_2(stat_O1, stat_O2);
}
#endif


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int
gsl_sf_bessel_Jnu_e(const double nu, const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x < 0.0 || nu < 0.0) {
    DOMAIN_ERROR(result);
  }
  else if(x == 0.0) {
    if(nu == 0.0) {
      result->val = 1.0;
      result->err = 0.0;
    }
    else {
      result->val = 0.0;
      result->err = 0.0;
    }
    return GSL_SUCCESS;
  }
  else if(x*x < 10.0*(nu+1.0)) {
    return gsl_sf_bessel_IJ_taylor_e(nu, x, -1, 100, GSL_DBL_EPSILON, result);
  }
  else if(nu > 50.0) {
    return gsl_sf_bessel_Jnu_asymp_Olver_e(nu, x, result);
  }
  else if(x > 1000.0)
  {
    /* We need this to avoid feeding large x to CF1; note that
     * due to the above check, we know that n <= 50. See similar
     * block in bessel_Jn.c.
     */
    return gsl_sf_bessel_Jnu_asympx_e(nu, x, result);
  }
  else {
    /* -1/2 <= mu <= 1/2 */
    int N = (int)(nu + 0.5);
    double mu = nu - N;

    /* Determine the J ratio at nu.
     */
    double Jnup1_Jnu;
    double sgn_Jnu;
    const int stat_CF1 = gsl_sf_bessel_J_CF1(nu, x, &Jnup1_Jnu, &sgn_Jnu);

    if(x < 2.0) {
      /* Determine Y_mu, Y_mup1 directly and recurse forward to nu.
       * Then use the CF1 information to solve for J_nu and J_nup1.
       */
      gsl_sf_result Y_mu, Y_mup1;
      const int stat_mu = gsl_sf_bessel_Y_temme(mu, x, &Y_mu, &Y_mup1);
      
      double Ynm1 = Y_mu.val;
      double Yn   = Y_mup1.val;
      double Ynp1 = 0.0;
      int n;
      for(n=1; n<N; n++) {
        Ynp1 = 2.0*(mu+n)/x * Yn - Ynm1;
        Ynm1 = Yn;
        Yn   = Ynp1;
      }

      result->val = 2.0/(M_PI*x) / (Jnup1_Jnu*Yn - Ynp1);
      result->err = GSL_DBL_EPSILON * (N + 2.0) * fabs(result->val);
      return GSL_ERROR_SELECT_2(stat_mu, stat_CF1);
    }
    else {
      /* Recurse backward from nu to mu, determining the J ratio
       * at mu. Use this together with a Steed method CF2 to
       * determine the actual J_mu, and thus obtain the normalization.
       */
      double Jmu;
      double Jmup1_Jmu;
      double sgn_Jmu;
      double Jmuprime_Jmu;
      double P, Q;
      const int stat_CF2 = gsl_sf_bessel_JY_steed_CF2(mu, x, &P, &Q);
      double gamma;
 
      double Jnp1 = sgn_Jnu * GSL_SQRT_DBL_MIN * Jnup1_Jnu;
      double Jn   = sgn_Jnu * GSL_SQRT_DBL_MIN;
      double Jnm1;
      int n;
      for(n=N; n>0; n--) {
        Jnm1 = 2.0*(mu+n)/x * Jn - Jnp1;
        Jnp1 = Jn;
        Jn   = Jnm1;
      }
      Jmup1_Jmu = Jnp1/Jn;
      sgn_Jmu   = GSL_SIGN(Jn);
      Jmuprime_Jmu = mu/x - Jmup1_Jmu;

      gamma = (P - Jmuprime_Jmu)/Q;
      Jmu   = sgn_Jmu * sqrt(2.0/(M_PI*x) / (Q + gamma*(P-Jmuprime_Jmu)));

      result->val = Jmu * (sgn_Jnu * GSL_SQRT_DBL_MIN) / Jn;
      result->err = 2.0 * GSL_DBL_EPSILON * (N + 2.0) * fabs(result->val);

      return GSL_ERROR_SELECT_2(stat_CF2, stat_CF1);
    }
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_Jnu(const double nu, const double x)
{
  EVAL_RESULT(gsl_sf_bessel_Jnu_e(nu, x, &result));
}
