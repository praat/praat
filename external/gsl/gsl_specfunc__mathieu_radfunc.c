/* specfunc/mathieu_radfunc.c
 * 
 * Copyright (C) 2002 Lowell Johnson
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Author:  L. Johnson */

#include "gsl__config.h"
#include <stdlib.h>
#include <math.h>
#include "gsl_math.h"
#include "gsl_sf.h"
#include "gsl_sf_mathieu.h"


int gsl_sf_mathieu_Mc(int kind, int order, double qq, double zz,
                      gsl_sf_result *result)
{
  int even_odd, kk, status;
  double maxerr = 1e-14, amax, pi = M_PI, fn, factor;
  double coeff[GSL_SF_MATHIEU_COEFF], fc;
  double j1c, z2c, j1pc, z2pc;
  double u1, u2;
  gsl_sf_result aa;


  /* Check for out of bounds parameters. */
  if (qq <= 0.0)
  {
      GSL_ERROR("q must be greater than zero", GSL_EINVAL);
  }
  if (kind < 1 || kind > 2)
  {
      GSL_ERROR("kind must be 1 or 2", GSL_EINVAL);
  }

  amax = 0.0;
  fn = 0.0;
  u1 = sqrt(qq)*exp(-1.0*zz);
  u2 = sqrt(qq)*exp(zz);
  
  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;

  /* Compute the characteristic value. */
  status = gsl_sf_mathieu_a(order, qq, &aa);
  if (status != GSL_SUCCESS)
  {
      return status;
  }
  
  /* Compute the series coefficients. */
  status = gsl_sf_mathieu_a_coeff(order, qq, aa.val, coeff);
  if (status != GSL_SUCCESS)
  {
      return status;
  }

  if (even_odd == 0)
  {
      for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
      {
          amax = GSL_MAX(amax, fabs(coeff[kk]));
          if (fabs(coeff[kk])/amax < maxerr)
              break;

          j1c = gsl_sf_bessel_Jn(kk, u1);
          if (kind == 1)
          {
              z2c = gsl_sf_bessel_Jn(kk, u2);
          }
          else /* kind = 2 */
          {
              z2c = gsl_sf_bessel_Yn(kk, u2);
          }
              
          fc = pow(-1.0, 0.5*order+kk)*coeff[kk];
          fn += fc*j1c*z2c;
      }

      fn *= sqrt(pi/2.0)/coeff[0];
  }
  else
  {
      for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
      {
          amax = GSL_MAX(amax, fabs(coeff[kk]));
          if (fabs(coeff[kk])/amax < maxerr)
              break;

          j1c = gsl_sf_bessel_Jn(kk, u1);
          j1pc = gsl_sf_bessel_Jn(kk+1, u1);
          if (kind == 1)
          {
              z2c = gsl_sf_bessel_Jn(kk, u2);
              z2pc = gsl_sf_bessel_Jn(kk+1, u2);
          }
          else /* kind = 2 */
          {
              z2c = gsl_sf_bessel_Yn(kk, u2);
              z2pc = gsl_sf_bessel_Yn(kk+1, u2);
          }
          fc = pow(-1.0, 0.5*(order-1)+kk)*coeff[kk];
          fn += fc*(j1c*z2pc + j1pc*z2c);
      }

      fn *= sqrt(pi/2.0)/coeff[0];
  }

  result->val = fn;
  result->err = 2.0*GSL_DBL_EPSILON;
  factor = fabs(fn);
  if (factor > 1.0)
      result->err *= factor;
  
  return GSL_SUCCESS;
}


int gsl_sf_mathieu_Ms(int kind, int order, double qq, double zz,
                      gsl_sf_result *result)
{
  int even_odd, kk, status;
  double maxerr = 1e-14, amax, pi = M_PI, fn, factor;
  double coeff[GSL_SF_MATHIEU_COEFF], fc;
  double j1c, z2c, j1mc, z2mc, j1pc, z2pc;
  double u1, u2;
  gsl_sf_result aa;


  /* Check for out of bounds parameters. */
  if (qq <= 0.0)
  {
      GSL_ERROR("q must be greater than zero", GSL_EINVAL);
  }
  if (kind < 1 || kind > 2)
  {
      GSL_ERROR("kind must be 1 or 2", GSL_EINVAL);
  }

  amax = 0.0;
  fn = 0.0;
  u1 = sqrt(qq)*exp(-1.0*zz);
  u2 = sqrt(qq)*exp(zz);
  
  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;
  
  /* Compute the characteristic value. */
  status = gsl_sf_mathieu_b(order, qq, &aa);
  if (status != GSL_SUCCESS)
  {
      return status;
  }
  
  /* Compute the series coefficients. */
  status = gsl_sf_mathieu_b_coeff(order, qq, aa.val, coeff);
  if (status != GSL_SUCCESS)
  {
      return status;
  }

  if (even_odd == 0)
  {
      for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
      {
          amax = GSL_MAX(amax, fabs(coeff[kk]));
          if (fabs(coeff[kk])/amax < maxerr)
              break;

          j1mc = gsl_sf_bessel_Jn(kk, u1);
          j1pc = gsl_sf_bessel_Jn(kk+2, u1);
          if (kind == 1)
          {
              z2mc = gsl_sf_bessel_Jn(kk, u2);
              z2pc = gsl_sf_bessel_Jn(kk+2, u2);
          }
          else /* kind = 2 */
          {
              z2mc = gsl_sf_bessel_Yn(kk, u2);
              z2pc = gsl_sf_bessel_Yn(kk+2, u2);
          }
          
          fc = pow(-1.0, 0.5*order+kk+1)*coeff[kk];
          fn += fc*(j1mc*z2pc - j1pc*z2mc);
      }

      fn *= sqrt(pi/2.0)/coeff[0];
  }
  else
  {
      for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
      {
          amax = GSL_MAX(amax, fabs(coeff[kk]));
          if (fabs(coeff[kk])/amax < maxerr)
              break;

          j1c = gsl_sf_bessel_Jn(kk, u1);
          j1pc = gsl_sf_bessel_Jn(kk+1, u1);
          if (kind == 1)
          {
              z2c = gsl_sf_bessel_Jn(kk, u2);
              z2pc = gsl_sf_bessel_Jn(kk+1, u2);
          }
          else /* kind = 2 */
          {
              z2c = gsl_sf_bessel_Yn(kk, u2);
              z2pc = gsl_sf_bessel_Yn(kk+1, u2);
          }
          
          fc = pow(-1.0, 0.5*(order-1)+kk)*coeff[kk];
          fn += fc*(j1c*z2pc - j1pc*z2c);
      }

      fn *= sqrt(pi/2.0)/coeff[0];
  }

  result->val = fn;
  result->err = 2.0*GSL_DBL_EPSILON;
  factor = fabs(fn);
  if (factor > 1.0)
      result->err *= factor;
  
  return GSL_SUCCESS;
}


int gsl_sf_mathieu_Mc_array(int kind, int nmin, int nmax, double qq,
                            double zz, gsl_sf_mathieu_workspace *work,
                            double result_array[])
{
  int even_odd, order, ii, kk, status;
  double maxerr = 1e-14, amax, pi = M_PI, fn;
  double coeff[GSL_SF_MATHIEU_COEFF], fc;
  double j1c, z2c, j1pc, z2pc;
  double u1, u2;
  double *aa = work->aa;


  /* Initialize the result array to zeroes. */
  for (ii=0; ii<nmax-nmin+1; ii++)
      result_array[ii] = 0.0;
  
  /* Check for out of bounds parameters. */
  if (qq <= 0.0)
  {
      GSL_ERROR("q must be greater than zero", GSL_EINVAL);
  }
  if (kind < 1 || kind > 2)
  {
      GSL_ERROR("kind must be 1 or 2", GSL_EINVAL);
  }

  amax = 0.0;
  fn = 0.0;
  u1 = sqrt(qq)*exp(-1.0*zz);
  u2 = sqrt(qq)*exp(zz);
  
  /* Compute all eigenvalues up to nmax. */
  gsl_sf_mathieu_a_array(0, nmax, qq, work, aa);
  
  for (ii=0, order=nmin; order<=nmax; ii++, order++)
  {
      even_odd = 0;
      if (order % 2 != 0)
          even_odd = 1;

      /* Compute the series coefficients. */
      status = gsl_sf_mathieu_a_coeff(order, qq, aa[order], coeff);
      if (status != GSL_SUCCESS)
      {
          return status;
      }

      if (even_odd == 0)
      {
          for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
          {
              amax = GSL_MAX(amax, fabs(coeff[kk]));
              if (fabs(coeff[kk])/amax < maxerr)
                  break;

              j1c = gsl_sf_bessel_Jn(kk, u1);
              if (kind == 1)
              {
                  z2c = gsl_sf_bessel_Jn(kk, u2);
              }
              else /* kind = 2 */
              {
                  z2c = gsl_sf_bessel_Yn(kk, u2);
              }
              
              fc = pow(-1.0, 0.5*order+kk)*coeff[kk];
              fn += fc*j1c*z2c;
          }

          fn *= sqrt(pi/2.0)/coeff[0];
      }
      else
      {
          for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
          {
              amax = GSL_MAX(amax, fabs(coeff[kk]));
              if (fabs(coeff[kk])/amax < maxerr)
                  break;

              j1c = gsl_sf_bessel_Jn(kk, u1);
              j1pc = gsl_sf_bessel_Jn(kk+1, u1);
              if (kind == 1)
              {
                  z2c = gsl_sf_bessel_Jn(kk, u2);
                  z2pc = gsl_sf_bessel_Jn(kk+1, u2);
              }
              else /* kind = 2 */
              {
                  z2c = gsl_sf_bessel_Yn(kk, u2);
                  z2pc = gsl_sf_bessel_Yn(kk+1, u2);
              }
              fc = pow(-1.0, 0.5*(order-1)+kk)*coeff[kk];
              fn += fc*(j1c*z2pc + j1pc*z2c);
          }

          fn *= sqrt(pi/2.0)/coeff[0];
      }

      result_array[ii] = fn;
  } /* order loop */
  
  return GSL_SUCCESS;
}


int gsl_sf_mathieu_Ms_array(int kind, int nmin, int nmax, double qq,
                            double zz, gsl_sf_mathieu_workspace *work,
                            double result_array[])
{
  int even_odd, order, ii, kk, status;
  double maxerr = 1e-14, amax, pi = M_PI, fn;
  double coeff[GSL_SF_MATHIEU_COEFF], fc;
  double j1c, z2c, j1mc, z2mc, j1pc, z2pc;
  double u1, u2;
  double *bb = work->bb;


  /* Initialize the result array to zeroes. */
  for (ii=0; ii<nmax-nmin+1; ii++)
      result_array[ii] = 0.0;
  
  /* Check for out of bounds parameters. */
  if (qq <= 0.0)
  {
      GSL_ERROR("q must be greater than zero", GSL_EINVAL);
  }
  if (kind < 1 || kind > 2)
  {
      GSL_ERROR("kind must be 1 or 2", GSL_EINVAL);
  }

  amax = 0.0;
  fn = 0.0;
  u1 = sqrt(qq)*exp(-1.0*zz);
  u2 = sqrt(qq)*exp(zz);
  
  /* Compute all eigenvalues up to nmax. */
  gsl_sf_mathieu_b_array(0, nmax, qq, work, bb);
  
  for (ii=0, order=nmin; order<=nmax; ii++, order++)
  {
      even_odd = 0;
      if (order % 2 != 0)
          even_odd = 1;
  
      /* Compute the series coefficients. */
      status = gsl_sf_mathieu_b_coeff(order, qq, bb[order], coeff);
      if (status != GSL_SUCCESS)
      {
          return status;
      }

      if (even_odd == 0)
      {
          for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
          {
              amax = GSL_MAX(amax, fabs(coeff[kk]));
              if (fabs(coeff[kk])/amax < maxerr)
                  break;

              j1mc = gsl_sf_bessel_Jn(kk, u1);
              j1pc = gsl_sf_bessel_Jn(kk+2, u1);
              if (kind == 1)
              {
                  z2mc = gsl_sf_bessel_Jn(kk, u2);
                  z2pc = gsl_sf_bessel_Jn(kk+2, u2);
              }
              else /* kind = 2 */
              {
                  z2mc = gsl_sf_bessel_Yn(kk, u2);
                  z2pc = gsl_sf_bessel_Yn(kk+2, u2);
              }
          
              fc = pow(-1.0, 0.5*order+kk+1)*coeff[kk];
              fn += fc*(j1mc*z2pc - j1pc*z2mc);
          }

          fn *= sqrt(pi/2.0)/coeff[0];
      }
      else
      {
          for (kk=0; kk<GSL_SF_MATHIEU_COEFF; kk++)
          {
              amax = GSL_MAX(amax, fabs(coeff[kk]));
              if (fabs(coeff[kk])/amax < maxerr)
                  break;

              j1c = gsl_sf_bessel_Jn(kk, u1);
              j1pc = gsl_sf_bessel_Jn(kk+1, u1);
              if (kind == 1)
              {
                  z2c = gsl_sf_bessel_Jn(kk, u2);
                  z2pc = gsl_sf_bessel_Jn(kk+1, u2);
              }
              else /* kind = 2 */
              {
                  z2c = gsl_sf_bessel_Yn(kk, u2);
                  z2pc = gsl_sf_bessel_Yn(kk+1, u2);
              }
          
              fc = pow(-1.0, 0.5*(order-1)+kk)*coeff[kk];
              fn += fc*(j1c*z2pc - j1pc*z2c);
          }

          fn *= sqrt(pi/2.0)/coeff[0];
      }

      result_array[ii] = fn;
  } /* order loop */
  
  return GSL_SUCCESS;
}
