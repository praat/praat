/* specfunc/mathieu_coeff.c
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
#include "gsl_sf_mathieu.h"


/*****************************************************************************
 * backward_recurse
 *
 * Purpose:
 ****************************************************************************/
static void backward_recurse_c(double aa, double qq, double xx, double *ff,
                               double *gx, int even_odd, int ni)
{
  int ii, nn;
  double g1;


  g1 = *gx;
  ff[ni] = xx;

  if (even_odd == 0)
  {
      for (ii=0; ii<ni; ii++)
      {
          nn = GSL_SF_MATHIEU_COEFF - ii - 1;
          ff[ni-ii-1] = -1.0/((4*nn*nn - aa)/qq + ff[ni-ii]);
      }
      if (ni == GSL_SF_MATHIEU_COEFF - 1)
          ff[0] *= 2.0;
  }  
  else
  {
      for (ii=0; ii<ni; ii++)
      {
          nn = GSL_SF_MATHIEU_COEFF - ii - 1;
          ff[ni-ii-1] = -1.0/(((2*nn + 1)*(2*nn + 1) - aa)/qq + ff[ni-ii]);
      }
  }

  *gx = ff[0] - g1;
}


static void backward_recurse_s(double aa, double qq, double xx, double *ff,
                               double *gx, int even_odd, int ni)
{
  int ii, nn;
  double g1;


  g1 = *gx;
  ff[ni] = xx;

  if (even_odd == 0)
  {
      for (ii=0; ii<ni; ii++)
      {
          nn = GSL_SF_MATHIEU_COEFF - ii - 1;
          ff[ni-ii-1] = -1.0/((4*(nn + 1)*(nn + 1) - aa)/qq + ff[ni-ii]);
      }
  }
  else
  {
      for (ii=0; ii<ni; ii++)
      {
          nn = GSL_SF_MATHIEU_COEFF - ii - 1;
          ff[ni-ii-1] = -1.0/(((2*nn + 1)*(2*nn + 1) - aa)/qq + ff[ni-ii]);
      }
  }

  *gx = ff[0] - g1;
}


int gsl_sf_mathieu_a_coeff(int order, double qq, double aa, double coeff[])
{
  int ni, nn, ii, even_odd;
  double eps, g1, g2, x1, x2, e1, e2, de, xh, sum, ratio,
         ff[GSL_SF_MATHIEU_COEFF];


  eps = 1e-14;
  coeff[0] = 1.0;
  
  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;

  /* If the coefficient array is not large enough to hold all necessary
     coefficients, error out. */
  if (order > GSL_SF_MATHIEU_COEFF)
      return GSL_FAILURE;
  
  /* Handle the trivial case where q = 0. */
  if (qq == 0.0)
  {
      for (ii=0; ii<GSL_SF_MATHIEU_COEFF; ii++)
          coeff[ii] = 0.0;

      coeff[order/2] = 1.0;
      
      return GSL_SUCCESS;
  }
  
  if (order < 5)
  {
      nn = 0;
      sum = 0.0;
      if (even_odd == 0)
          ratio = aa/qq;
      else
          ratio = (aa - 1 - qq)/qq;
  }
  else
  {
      if (even_odd == 0)
      {
          coeff[1] = aa/qq;
          coeff[2] = (aa - 4)/qq*coeff[1] - 2;
          sum = coeff[0] + coeff[1] + coeff[2];
          for (ii=3; ii<order/2+1; ii++)
          {
              coeff[ii] = (aa - 4*(ii - 1)*(ii - 1))/qq*coeff[ii-1] -
                                                                  coeff[ii-2];
              sum += coeff[ii];
          }
      }
      else
      {
          coeff[1] = (aa - 1)/qq - 1;
          sum = coeff[0] + coeff[1];
          for (ii=2; ii<order/2+1; ii++)
          {
              coeff[ii] = (aa - (2*ii - 1)*(2*ii - 1))/qq*coeff[ii-1] -
                                                                  coeff[ii-2];
              sum += coeff[ii];
          }
      }

      nn = ii - 1;

      ratio = coeff[nn]/coeff[nn-1];
  }
  
  ni = GSL_SF_MATHIEU_COEFF - nn - 1;

  /* Compute first two points to start root-finding. */
  if (even_odd == 0)
      x1 = -qq/(4.0*GSL_SF_MATHIEU_COEFF*GSL_SF_MATHIEU_COEFF);
  else
      x1 = -qq/((2.0*GSL_SF_MATHIEU_COEFF + 1.0)*(2.0*GSL_SF_MATHIEU_COEFF + 1.0));
  g1 = ratio;
  backward_recurse_c(aa, qq, x1, ff, &g1, even_odd, ni);
  x2 = g1;
  g2 = ratio;
  backward_recurse_c(aa, qq, x2, ff, &g2, even_odd, ni);

  /* Find the root. */
  while (1)
  {
      /* Compute the relative error. */
      e1 = g1 - x1;
      e2 = g2 - x2;
      de = e1 - e2;

      /* If we are close enough to the root, break... */
      if (fabs(de) < eps)
          break;

      /* Otherwise, determine the next guess and try again. */
      xh = (e1*x2 - e2*x1)/de;
      x1 = x2;
      g1 = g2;
      x2 = xh;
      g2 = ratio;
      backward_recurse_c(aa, qq, x2, ff, &g2, even_odd, ni);
  }

  /* Compute the rest of the coefficients. */
  sum += coeff[nn];
  for (ii=nn+1; ii<GSL_SF_MATHIEU_COEFF; ii++)
  {
      coeff[ii] = ff[ii-nn-1]*coeff[ii-1];
      sum += coeff[ii];

      /* If the coefficients are getting really small, set the remainder
         to zero. */
      if (fabs(coeff[ii]) < 1e-20)
      {
          for (; ii<GSL_SF_MATHIEU_COEFF;)
              coeff[ii++] = 0.0;
      }
  }
  
  /* Normalize the coefficients. */
  for (ii=0; ii<GSL_SF_MATHIEU_COEFF; ii++)
      coeff[ii] /= sum;

  return GSL_SUCCESS;
}


int gsl_sf_mathieu_b_coeff(int order, double qq, double aa, double coeff[])
{
  int ni, nn, ii, even_odd;
  double eps, g1, g2, x1, x2, e1, e2, de, xh, sum, ratio,
         ff[GSL_SF_MATHIEU_COEFF];


  eps = 1e-10;
  coeff[0] = 1.0;
  
  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;

  /* If the coefficient array is not large enough to hold all necessary
     coefficients, error out. */
  if (order > GSL_SF_MATHIEU_COEFF)
      return GSL_FAILURE;
  
  /* Handle the trivial case where q = 0. */
  if (qq == 0.0)
  {
      for (ii=0; ii<GSL_SF_MATHIEU_COEFF; ii++)
          coeff[ii] = 0.0;

      coeff[(order-1)/2] = 1.0;
      
      return GSL_SUCCESS;
  }
  
  if (order < 5)
  {
      nn = 0;
      sum = 0.0;
      if (even_odd == 0)
          ratio = (aa - 4)/qq;
      else
          ratio = (aa - 1 - qq)/qq;
  }
  else
  {
      if (even_odd == 0)
      {
          coeff[1] = (aa - 4)/qq;
          sum = 2*coeff[0] + 4*coeff[1];
          for (ii=2; ii<order/2; ii++)
          {
              coeff[ii] = (aa - 4*ii*ii)/qq*coeff[ii-1] - coeff[ii-2];
              sum += 2*(ii + 1)*coeff[ii];
          }
      }
      else
      {
          coeff[1] = (aa - 1)/qq + 1;
          sum = coeff[0] + 3*coeff[1];
          for (ii=2; ii<order/2+1; ii++)
          {
              coeff[ii] = (aa - (2*ii - 1)*(2*ii - 1))/qq*coeff[ii-1] -
                                                                  coeff[ii-2];
              sum += (2*(ii + 1) - 1)*coeff[ii];
          }
      }

      nn = ii - 1;

      ratio = coeff[nn]/coeff[nn-1];
  }
  
  ni = GSL_SF_MATHIEU_COEFF - nn - 1;

  /* Compute first two points to start root-finding. */
  if (even_odd == 0)
      x1 = -qq/(4.0*(GSL_SF_MATHIEU_COEFF + 1.0)*(GSL_SF_MATHIEU_COEFF + 1.0));
  else
      x1 = -qq/((2.0*GSL_SF_MATHIEU_COEFF + 1.0)*(2.0*GSL_SF_MATHIEU_COEFF + 1.0));
  g1 = ratio;
  backward_recurse_s(aa, qq, x1, ff, &g1, even_odd, ni);
  x2 = g1;
  g2 = ratio;
  backward_recurse_s(aa, qq, x2, ff, &g2, even_odd, ni);

  /* Find the root. */
  while (1)
  {
      /* Compute the relative error. */
      e1 = g1 - x1;
      e2 = g2 - x2;
      de = e1 - e2;

      /* If we are close enough to the root, break... */
      if (fabs(de) < eps)
          break;

      /* Otherwise, determine the next guess and try again. */
      xh = (e1*x2 - e2*x1)/de;
      x1 = x2;
      g1 = g2;
      x2 = xh;
      g2 = ratio;
      backward_recurse_s(aa, qq, x2, ff, &g2, even_odd, ni);
  }

  /* Compute the rest of the coefficients. */
  sum += 2*(nn + 1)*coeff[nn];
  for (ii=nn+1; ii<GSL_SF_MATHIEU_COEFF; ii++)
  {
      coeff[ii] = ff[ii-nn-1]*coeff[ii-1];
      sum += 2*(ii + 1)*coeff[ii];

      /* If the coefficients are getting really small, set the remainder
         to zero. */
      if (fabs(coeff[ii]) < 1e-20)
      {
          for (; ii<GSL_SF_MATHIEU_COEFF;)
              coeff[ii++] = 0.0;
      }
  }
  
  /* Normalize the coefficients. */
  for (ii=0; ii<GSL_SF_MATHIEU_COEFF; ii++)
      coeff[ii] /= sum;

  return GSL_SUCCESS;
}
