/* specfunc/elljac.c
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
#include "gsl_sf_pow_int.h"
#include "gsl_sf_elljac.h"


/* GJ: See [Thompson, Atlas for Computing Mathematical Functions] */

/* BJG 2005-07: New algorithm based on Algorithm 5 from Numerische
   Mathematik 7, 78-90 (1965) "Numerical Calculation of Elliptic
   Integrals and Elliptic Functions" R. Bulirsch.

   Minor tweak is to avoid division by zero when sin(x u_l) = 0 by
   computing reflected values sn(K-u) cn(K-u) dn(K-u) and using
   transformation from Abramowitz & Stegun table 16.8 column "K-u"*/

int
gsl_sf_elljac_e(double u, double m, double * sn, double * cn, double * dn)
{
  if(fabs(m) > 1.0) {
    *sn = 0.0;
    *cn = 0.0;
    *dn = 0.0;
    GSL_ERROR ("|m| > 1.0", GSL_EDOM);
  }
  else if(fabs(m) < 2.0*GSL_DBL_EPSILON) {
    *sn = sin(u);
    *cn = cos(u);
    *dn = 1.0;
    return GSL_SUCCESS;
  }
  else if(fabs(m - 1.0) < 2.0*GSL_DBL_EPSILON) {
    *sn = tanh(u);
    *cn = 1.0/cosh(u);
    *dn = *cn;
    return GSL_SUCCESS;
  }
  else {
    int status = GSL_SUCCESS;
    const int N = 16;
    double mu[16];
    double nu[16];
    double c[16];
    double d[16];
    double sin_umu, cos_umu, t, r;
    int n = 0;

    mu[0] = 1.0;
    nu[0] = sqrt(1.0 - m);

    while( fabs(mu[n] - nu[n]) > 4.0 * GSL_DBL_EPSILON * fabs(mu[n]+nu[n])) {
      mu[n+1] = 0.5 * (mu[n] + nu[n]);
      nu[n+1] = sqrt(mu[n] * nu[n]);
      ++n;
      if(n >= N - 1) {
        status = GSL_EMAXITER;
        break;
      }
    }

    sin_umu = sin(u * mu[n]);
    cos_umu = cos(u * mu[n]);

    /* Since sin(u*mu(n)) can be zero we switch to computing sn(K-u),
       cn(K-u), dn(K-u) when |sin| < |cos| */

    if (fabs(sin_umu) < fabs(cos_umu))
      {
        t = sin_umu / cos_umu;
        
        c[n] = mu[n] * t;
        d[n] = 1.0;
        
        while(n > 0) {
          n--;
          c[n] = d[n+1] * c[n+1];
          r = (c[n+1] * c[n+1]) / mu[n+1];
          d[n] = (r + nu[n]) / (r + mu[n]);
          }
        
        *dn = sqrt(1.0-m) / d[n];
        *cn = (*dn) * GSL_SIGN(cos_umu) / gsl_hypot(1.0, c[n]);
        *sn = (*cn) * c[n] /sqrt(1.0-m);
      }
    else
      {
        t = cos_umu / sin_umu;
        
        c[n] = mu[n] * t;
        d[n] = 1.0;
        
        while(n > 0) {
          --n;
          c[n] = d[n+1] * c[n+1];
          r = (c[n+1] * c[n+1]) / mu[n+1];
          d[n] = (r + nu[n]) / (r + mu[n]);
        }
        
        *dn = d[n];
        *sn = GSL_SIGN(sin_umu) / gsl_hypot(1.0, c[n]);
        *cn = c[n] * (*sn);
      }
    
    return status;
  }
}
