/* specfunc/mathieu_charv.c
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
#include <stdio.h>
#include <math.h>
#include "gsl_math.h"
#include "gsl_eigen.h"
#include "gsl_errno.h"
#include "gsl_sf_mathieu.h"


/* prototypes */
static double solve_cubic(double c2, double c1, double c0);


static double ceer(int order, double qq, double aa, int nterms)
{

  double term, term1;
  int ii, n1;
  

  if (order == 0)
      term = 0.0;
  else
  {      
      term = 2.0*qq*qq/aa;

      if (order != 2)
      {
          n1 = order/2 - 1;

          for (ii=0; ii<n1; ii++)
              term = qq*qq/(aa - 4.0*(ii+1)*(ii+1) - term);
      }
  }
  
  term += order*order;

  term1 = 0.0;

  for (ii=0; ii<nterms; ii++)
      term1 = qq*qq/
        (aa - (order + 2.0*(nterms - ii))*(order + 2.0*(nterms - ii)) - term1);

  if (order == 0)
      term1 *= 2.0;
  
  return (term + term1 - aa);
}


static double ceor(int order, double qq, double aa, int nterms)
{
  double term, term1;
  int ii, n1;

  term = qq;
  n1 = (int)((float)order/2.0 - 0.5);

  for (ii=0; ii<n1; ii++)
      term = qq*qq/(aa - (2.0*ii + 1.0)*(2.0*ii + 1.0) - term);
  term += order*order;

  term1 = 0.0;
  for (ii=0; ii<nterms; ii++)
      term1 = qq*qq/
        (aa - (order + 2.0*(nterms - ii))*(order + 2.0*(nterms - ii)) - term1);

  return (term + term1 - aa);
}


static double seer(int order, double qq, double aa, int nterms)
{
  double term, term1;
  int ii, n1;

  term = 0.0;
  n1 = order/2 - 1;

  for (ii=0; ii<n1; ii++)
      term = qq*qq/(aa - 4.0*(ii + 1)*(ii + 1) - term);
  term += order*order;

  term1 = 0.0;
  for (ii=0; ii<nterms; ii++)
      term1 = qq*qq/
        (aa - (order + 2.0*(nterms - ii))*(order + 2.0*(nterms - ii)) - term1);

  return (term + term1 - aa);
}


static double seor(int order, double qq, double aa, int nterms)
{
  double term, term1;
  int ii, n1;


  term = -1.0*qq;
  n1 = (int)((float)order/2.0 - 0.5);
  for (ii=0; ii<n1; ii++)
      term = qq*qq/(aa - (2.0*ii + 1.0)*(2.0*ii + 1.0) - term);
  term += order*order;

  term1 = 0.0;
  for (ii=0; ii<nterms; ii++)
      term1 = qq*qq/
        (aa - (order + 2.0*(nterms - ii))*(order + 2.0*(nterms - ii)) - term1);

  return (term + term1 - aa);
}


/*----------------------------------------------------------------------------
 * Asymptotic and approximation routines for the characteristic value.
 *
 * Adapted from F.A. Alhargan's paper,
 * "Algorithms for the Computation of All Mathieu Functions of Integer
 * Orders," ACM Transactions on Mathematical Software, Vol. 26, No. 3,
 * September 2000, pp. 390-407.
 *--------------------------------------------------------------------------*/
static double asymptotic(int order, double qq)
{
  double asymp;
  double nn, n2, n4, n6;
  double hh, ah, ah2, ah3, ah4, ah5;


  /* Set up temporary variables to simplify the readability. */
  nn = 2*order + 1;
  n2 = nn*nn;
  n4 = n2*n2;
  n6 = n4*n2;
  
  hh = 2*sqrt(qq);
  ah = 16*hh;
  ah2 = ah*ah;
  ah3 = ah2*ah;
  ah4 = ah3*ah;
  ah5 = ah4*ah;

  /* Equation 38, p. 397 of Alhargan's paper. */
  asymp = -2*qq + nn*hh - 0.125*(n2 + 1);
  asymp -= 0.25*nn*(                          n2 +     3)/ah;
  asymp -= 0.25*   (             5*n4 +    34*n2 +     9)/ah2;
  asymp -= 0.25*nn*(            33*n4 +   410*n2 +   405)/ah3;
  asymp -=         ( 63*n6 +  1260*n4 +  2943*n2 +   486)/ah4;
  asymp -=      nn*(527*n6 + 15617*n4 + 69001*n2 + 41607)/ah5;

  return asymp;
}


/* Solve the cubic x^3 + c2*x^2 + c1*x + c0 = 0 */
static double solve_cubic(double c2, double c1, double c0)
{
  double qq, rr, ww, ss, tt;

  
  qq = (3*c1 - c2*c2)/9;
  rr = (9*c2*c1 - 27*c0 - 2*c2*c2*c2)/54;
  ww = qq*qq*qq + rr*rr;
  
  if (ww >= 0)
  {
      double t1 = rr + sqrt(ww);
      ss = fabs(t1)/t1*pow(fabs(t1), 1/3.);
      t1 = rr - sqrt(ww);
      tt = fabs(t1)/t1*pow(fabs(t1), 1/3.);
  }
  else
  {
      double theta = acos(rr/sqrt(-qq*qq*qq));
      ss = 2*sqrt(-qq)*cos((theta + 4*M_PI)/3.);
      tt = 0.0;
  }
  
  return (ss + tt - c2/3);
}


/* Compute an initial approximation for the characteristic value. */
static double approx_c(int order, double qq)
{
  double approx;
  double c0, c1, c2;


  if (order < 0)
  {
    GSL_ERROR_VAL("Undefined order for Mathieu function", GSL_EINVAL, 0.0);
  }
  
  switch (order)
  {
      case 0:
          if (qq <= 4)
              return (2 - sqrt(4 + 2*qq*qq)); /* Eqn. 31 */
          else
              return asymptotic(order, qq);
          break;

      case 1:
          if (qq <= 4)
              return (5 + 0.5*(qq - sqrt(5*qq*qq - 16*qq + 64))); /* Eqn. 32 */
          else
              return asymptotic(order, qq);
          break;

      case 2:
          if (qq <= 3)
          {
              c2 = -8.0;  /* Eqn. 33 */
              c1 = -48 - 3*qq*qq;
              c0 = 20*qq*qq;
          }
          else
              return asymptotic(order, qq);
          break;

      case 3:
          if (qq <= 6.25)
          {
              c2 = -qq - 8;  /* Eqn. 34 */
              c1 = 16*qq - 128 - 2*qq*qq;
              c0 = qq*qq*(qq + 8);
          }
          else
              return asymptotic(order, qq);
          break;

      default:
          if (order < 70)
          {
              if (1.7*order > 2*sqrt(qq))
              {
                  /* Eqn. 30 */
                  double n2 = (double)(order*order);
                  double n22 = (double)((n2 - 1)*(n2 - 1));
                  double q2 = qq*qq;
                  double q4 = q2*q2;
                  approx = n2 + 0.5*q2/(n2 - 1);
                  approx += (5*n2 + 7)*q4/(32*n22*(n2 - 1)*(n2 - 4));
                  approx += (9*n2*n2 + 58*n2 + 29)*q4*q2/
                      (64*n22*n22*(n2 - 1)*(n2 - 4)*(n2 - 9));
                  if (1.4*order < 2*sqrt(qq))
                  {
                      approx += asymptotic(order, qq);
                      approx *= 0.5;
                  }
              }
              else
                  approx = asymptotic(order, qq);

              return approx;
          }
          else
              return order*order;
  }

  /* Solve the cubic x^3 + c2*x^2 + c1*x + c0 = 0 */
  approx = solve_cubic(c2, c1, c0);
      
  if ( approx < 0 && sqrt(qq) > 0.1*order )
      return asymptotic(order-1, qq);
  else
      return (order*order + fabs(approx));
}

  
static double approx_s(int order, double qq)
{
  double approx;
  double c0, c1, c2;

  
  if (order < 1)
  {
    GSL_ERROR_VAL("Undefined order for Mathieu function", GSL_EINVAL, 0.0);
  }
  
  switch (order)
  {
      case 1:
          if (qq <= 4)
              return (5 - 0.5*(qq + sqrt(5*qq*qq + 16*qq + 64))); /* Eqn. 35 */
          else
              return asymptotic(order-1, qq);
          break;

      case 2:
          if (qq <= 5)
              return (10 - sqrt(36 + qq*qq)); /* Eqn. 36 */
          else
              return asymptotic(order-1, qq);
          break;

      case 3:
          if (qq <= 6.25)
          {
              c2 = qq - 8; /* Eqn. 37 */
              c1 = -128 - 16*qq - 2*qq*qq;
              c0 = qq*qq*(8 - qq);
          }
          else
              return asymptotic(order-1, qq);
          break;

      default:
          if (order < 70)
          {
              if (1.7*order > 2*sqrt(qq))
              {
                  /* Eqn. 30 */
                  double n2 = (double)(order*order);
                  double n22 = (double)((n2 - 1)*(n2 - 1));
                  double q2 = qq*qq;
                  double q4 = q2*q2;
                  approx = n2 + 0.5*q2/(n2 - 1);
                  approx += (5*n2 + 7)*q4/(32*n22*(n2 - 1)*(n2 - 4));
                  approx += (9*n2*n2 + 58*n2 + 29)*q4*q2/
                      (64*n22*n22*(n2 - 1)*(n2 - 4)*(n2 - 9));
                  if (1.4*order < 2*sqrt(qq))
                  {
                      approx += asymptotic(order-1, qq);
                      approx *= 0.5;
                  }
              }
              else
                  approx = asymptotic(order-1, qq);

              return approx;
          }
          else
              return order*order;
  }

  /* Solve the cubic x^3 + c2*x^2 + c1*x + c0 = 0 */
  approx = solve_cubic(c2, c1, c0);
      
  if ( approx < 0 && sqrt(qq) > 0.1*order )
      return asymptotic(order-1, qq);
  else
      return (order*order + fabs(approx));
}


int gsl_sf_mathieu_a(int order, double qq, gsl_sf_result *result)
{
  int even_odd, nterms = 50, ii, counter = 0, maxcount = 200;
  double a1, a2, fa, fa1, dela, aa_orig, da = 0.025, aa;


  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;

  /* If the argument is 0, then the coefficient is simply the square of
     the order. */
  if (qq == 0)
  {
      result->val = order*order;
      result->err = 0.0;
      return GSL_SUCCESS;
  }

  /* Use symmetry characteristics of the functions to handle cases with
     negative order and/or argument q.  See Abramowitz & Stegun, 20.8.3. */
  if (order < 0)
      order *= -1;
  if (qq < 0.0)
  {
      if (even_odd == 0)
          return gsl_sf_mathieu_a(order, -qq, result);
      else
          return gsl_sf_mathieu_b(order, -qq, result);
  }
  
  /* Compute an initial approximation for the characteristic value. */
  aa = approx_c(order, qq);

  /* Save the original approximation for later comparison. */
  aa_orig = aa;
  
  /* Loop as long as the final value is not near the approximate value
     (with a max limit to avoid potential infinite loop). */
  while (counter < maxcount)
  {
      a1 = aa + 0.001;
      ii = 0;
      if (even_odd == 0)
          fa1 = ceer(order, qq, a1, nterms);
      else
          fa1 = ceor(order, qq, a1, nterms);

      for (;;)
      {
          if (even_odd == 0)
              fa = ceer(order, qq, aa, nterms);
          else
              fa = ceor(order, qq, aa, nterms);
      
          a2 = a1;
          a1 = aa;

          if (fa == fa1)
          {
              result->err = GSL_DBL_EPSILON;
              break;
          }
          aa -= (aa - a2)/(fa - fa1)*fa;
          dela = fabs(aa - a2);
          if (dela < GSL_DBL_EPSILON)
          {
              result->err = GSL_DBL_EPSILON;
              break;
          }
          if (ii > 20)
          {
              result->err = dela;
              break;
          }
          fa1 = fa;
          ii++;
      }

      /* If the solution found is not near the original approximation,
         tweak the approximate value, and try again. */
      if (fabs(aa - aa_orig) > (3 + 0.01*order*fabs(aa_orig)))
      {
          counter++;
          if (counter == maxcount)
          {
              result->err = fabs(aa - aa_orig);
              break;
          }
          if (aa > aa_orig)
              aa = aa_orig - da*counter;
          else
              aa = aa_orig + da*counter;

          continue;
      }
      else
          break;
  }

  result->val = aa;
      
  /* If we went through the maximum number of retries and still didn't
     find the solution, let us know. */
  if (counter == maxcount)
  {
      GSL_ERROR("Wrong characteristic Mathieu value", GSL_EFAILED);
  }
  
  return GSL_SUCCESS;
}


int gsl_sf_mathieu_b(int order, double qq, gsl_sf_result *result)
{
  int even_odd, nterms = 50, ii, counter = 0, maxcount = 200;
  double a1, a2, fa, fa1, dela, aa_orig, da = 0.025, aa;


  even_odd = 0;
  if (order % 2 != 0)
      even_odd = 1;

  /* The order cannot be 0. */
  if (order == 0)
  {
      GSL_ERROR("Characteristic value undefined for order 0", GSL_EFAILED);
  }

  /* If the argument is 0, then the coefficient is simply the square of
     the order. */
  if (qq == 0)
  {
      result->val = order*order;
      result->err = 0.0;
      return GSL_SUCCESS;
  }

  /* Use symmetry characteristics of the functions to handle cases with
     negative order and/or argument q.  See Abramowitz & Stegun, 20.8.3. */
  if (order < 0)
      order *= -1;
  if (qq < 0.0)
  {
      if (even_odd == 0)
          return gsl_sf_mathieu_b(order, -qq, result);
      else
          return gsl_sf_mathieu_a(order, -qq, result);
  }
  
  /* Compute an initial approximation for the characteristic value. */
  aa = approx_s(order, qq);
  
  /* Save the original approximation for later comparison. */
  aa_orig = aa;
  
  /* Loop as long as the final value is not near the approximate value
     (with a max limit to avoid potential infinite loop). */
  while (counter < maxcount)
  {
      a1 = aa + 0.001;
      ii = 0;
      if (even_odd == 0)
          fa1 = seer(order, qq, a1, nterms);
      else
          fa1 = seor(order, qq, a1, nterms);

      for (;;)
      {
          if (even_odd == 0)
              fa = seer(order, qq, aa, nterms);
          else
              fa = seor(order, qq, aa, nterms);
      
          a2 = a1;
          a1 = aa;

          if (fa == fa1)
          {
              result->err = GSL_DBL_EPSILON;
              break;
          }
          aa -= (aa - a2)/(fa - fa1)*fa;
          dela = fabs(aa - a2);
          if (dela < 1e-18)
          {
              result->err = GSL_DBL_EPSILON;
              break;
          }
          if (ii > 20)
          {
              result->err = dela;
              break;
          }
          fa1 = fa;
          ii++;
      }
      
      /* If the solution found is not near the original approximation,
         tweak the approximate value, and try again. */
      if (fabs(aa - aa_orig) > (3 + 0.01*order*fabs(aa_orig)))
      {
          counter++;
          if (counter == maxcount)
          {
              result->err = fabs(aa - aa_orig);
              break;
          }
          if (aa > aa_orig)
              aa = aa_orig - da*counter;
          else
              aa = aa_orig + da*counter;
          
          continue;
      }
      else
          break;
  }
  
  result->val = aa;
      
  /* If we went through the maximum number of retries and still didn't
     find the solution, let us know. */
  if (counter == maxcount)
  {
      GSL_ERROR("Wrong characteristic Mathieu value", GSL_EFAILED);
  }
  
  return GSL_SUCCESS;
}


/* Eigenvalue solutions for characteristic values below. */


/*  figi.c converted from EISPACK Fortran FIGI.F.
 *
 *   given a nonsymmetric tridiagonal matrix such that the products
 *    of corresponding pairs of off-diagonal elements are all
 *    non-negative, this subroutine reduces it to a symmetric
 *    tridiagonal matrix with the same eigenvalues.  if, further,
 *    a zero product only occurs when both factors are zero,
 *    the reduced matrix is similar to the original matrix.
 *
 *    on input
 *
 *       n is the order of the matrix.
 *
 *       t contains the input matrix.  its subdiagonal is
 *         stored in the last n-1 positions of the first column,
 *         its diagonal in the n positions of the second column,
 *         and its superdiagonal in the first n-1 positions of
 *         the third column.  t(1,1) and t(n,3) are arbitrary.
 *
 *    on output
 *
 *       t is unaltered.
 *
 *       d contains the diagonal elements of the symmetric matrix.
 *
 *       e contains the subdiagonal elements of the symmetric
 *         matrix in its last n-1 positions.  e(1) is not set.
 *
 *       e2 contains the squares of the corresponding elements of e.
 *         e2 may coincide with e if the squares are not needed.
 *
 *       ierr is set to
 *         zero       for normal return,
 *         n+i        if t(i,1)*t(i-1,3) is negative,
 *         -(3*n+i)   if t(i,1)*t(i-1,3) is zero with one factor
 *                    non-zero.  in this case, the eigenvectors of
 *                    the symmetric matrix are not simply related
 *                    to those of  t  and should not be sought.
 *
 *    questions and comments should be directed to burton s. garbow,
 *    mathematics and computer science div, argonne national laboratory
 *
 *    this version dated august 1983.
 */
static int figi(int nn, double *tt, double *dd, double *ee,
                double *e2)
{
  int ii;

  for (ii=0; ii<nn; ii++)
  {
      if (ii != 0)
      {
          e2[ii] = tt[3*ii]*tt[3*(ii-1)+2];

          if (e2[ii] < 0.0)
          {
              /* set error -- product of some pair of off-diagonal
                 elements is negative */
              return (nn + ii);
          }

          if (e2[ii] == 0.0 && (tt[3*ii] != 0.0 || tt[3*(ii-1)+2] != 0.0))
          {
              /* set error -- product of some pair of off-diagonal
                 elements is zero with one member non-zero */
              return (-1*(3*nn + ii));
          }

          ee[ii] = sqrt(e2[ii]);
      }

      dd[ii] = tt[3*ii+1];
  }

  return 0;
}


int gsl_sf_mathieu_a_array(int order_min, int order_max, double qq, gsl_sf_mathieu_workspace *work, double result_array[])
{
  unsigned int even_order = work->even_order, odd_order = work->odd_order,
      extra_values = work->extra_values, ii, jj;
  int status;
  double *tt = work->tt, *dd = work->dd, *ee = work->ee, *e2 = work->e2,
         *zz = work->zz, *aa = work->aa;
  gsl_matrix_view mat, evec;
  gsl_vector_view eval;
  gsl_eigen_symmv_workspace *wmat = work->wmat;
  
  if (order_max > work->size || order_max <= order_min || order_min < 0)
    {
      GSL_ERROR ("invalid range [order_min,order_max]", GSL_EINVAL);
    }
  
  /* Convert the nonsymmetric tridiagonal matrix to a symmetric tridiagonal
     form. */

  tt[0] = 0.0;
  tt[1] = 0.0;
  tt[2] = qq;
  for (ii=1; ii<even_order-1; ii++)
  {
      tt[3*ii] = qq;
      tt[3*ii+1] = 4*ii*ii;
      tt[3*ii+2] = qq;
  }
  tt[3*even_order-3] = qq;
  tt[3*even_order-2] = 4*(even_order - 1)*(even_order - 1);
  tt[3*even_order-1] = 0.0;

  tt[3] *= 2;
  
  status = figi((signed int)even_order, tt, dd, ee, e2);

  if (status) 
    {
      GSL_ERROR("Internal error in tridiagonal Mathieu matrix", GSL_EFAILED);
    }

  /* Fill the period \pi matrix. */
  for (ii=0; ii<even_order*even_order; ii++)
      zz[ii] = 0.0;

  zz[0] = dd[0];
  zz[1] = ee[1];
  for (ii=1; ii<even_order-1; ii++)
  {
      zz[ii*even_order+ii-1] = ee[ii];
      zz[ii*even_order+ii] = dd[ii];
      zz[ii*even_order+ii+1] = ee[ii+1];
  }
  zz[even_order*(even_order-1)+even_order-2] = ee[even_order-1];
  zz[even_order*even_order-1] = dd[even_order-1];
  
  /* Compute (and sort) the eigenvalues of the matrix. */
  mat = gsl_matrix_view_array(zz, even_order, even_order);
  eval = gsl_vector_subvector(work->eval, 0, even_order);
  evec = gsl_matrix_submatrix(work->evec, 0, 0, even_order, even_order);
  gsl_eigen_symmv(&mat.matrix, &eval.vector, &evec.matrix, wmat);
  gsl_eigen_symmv_sort(&eval.vector, &evec.matrix, GSL_EIGEN_SORT_VAL_ASC);
  
  for (ii=0; ii<even_order-extra_values; ii++)
      aa[2*ii] = gsl_vector_get(&eval.vector, ii);
  
  /* Fill the period 2\pi matrix. */
  for (ii=0; ii<odd_order*odd_order; ii++)
      zz[ii] = 0.0;
  for (ii=0; ii<odd_order; ii++)
      for (jj=0; jj<odd_order; jj++)
      {
          if (ii == jj)
              zz[ii*odd_order+jj] = (2*ii + 1)*(2*ii + 1);
          else if (ii == jj + 1 || ii + 1 == jj)
              zz[ii*odd_order+jj] = qq;
      }
  zz[0] += qq;

  /* Compute (and sort) the eigenvalues of the matrix. */
  mat = gsl_matrix_view_array(zz, odd_order, odd_order);
  eval = gsl_vector_subvector(work->eval, 0, odd_order);
  evec = gsl_matrix_submatrix(work->evec, 0, 0, odd_order, odd_order);
  gsl_eigen_symmv(&mat.matrix, &eval.vector, &evec.matrix, wmat);
  gsl_eigen_symmv_sort(&eval.vector, &evec.matrix, GSL_EIGEN_SORT_VAL_ASC);

  for (ii=0; ii<odd_order-extra_values; ii++)
      aa[2*ii+1] = gsl_vector_get(&eval.vector, ii);

  for (ii = order_min ; ii <= order_max ; ii++)
    {
      result_array[ii - order_min] = aa[ii];
    }
  
  return GSL_SUCCESS;
}


int gsl_sf_mathieu_b_array(int order_min, int order_max, double qq, gsl_sf_mathieu_workspace *work, double result_array[])
{
  unsigned int even_order = work->even_order-1, odd_order = work->odd_order,
      extra_values = work->extra_values, ii, jj;
  double *zz = work->zz, *bb = work->bb;
  gsl_matrix_view mat, evec;
  gsl_vector_view eval;
  gsl_eigen_symmv_workspace *wmat = work->wmat;

  if (order_max > work->size || order_max <= order_min || order_min < 0)
    {
      GSL_ERROR ("invalid range [order_min,order_max]", GSL_EINVAL);
    }

  /* Fill the period \pi matrix. */
  for (ii=0; ii<even_order*even_order; ii++)
      zz[ii] = 0.0;
  for (ii=0; ii<even_order; ii++)
      for (jj=0; jj<even_order; jj++)
      {
          if (ii == jj)
              zz[ii*even_order+jj] = 4*(ii + 1)*(ii + 1);
          else if (ii == jj + 1 || ii + 1 == jj)
              zz[ii*even_order+jj] = qq;
      }

  /* Compute (and sort) the eigenvalues of the matrix. */
  mat = gsl_matrix_view_array(zz, even_order, even_order);
  eval = gsl_vector_subvector(work->eval, 0, even_order);
  evec = gsl_matrix_submatrix(work->evec, 0, 0, even_order, even_order);
  gsl_eigen_symmv(&mat.matrix, &eval.vector, &evec.matrix, wmat);
  gsl_eigen_symmv_sort(&eval.vector, &evec.matrix, GSL_EIGEN_SORT_VAL_ASC);

  bb[0] = 0.0;
  for (ii=0; ii<even_order-extra_values; ii++)
      bb[2*(ii+1)] = gsl_vector_get(&eval.vector, ii);
  
  /* Fill the period 2\pi matrix. */
  for (ii=0; ii<odd_order*odd_order; ii++)
      zz[ii] = 0.0;
  for (ii=0; ii<odd_order; ii++)
      for (jj=0; jj<odd_order; jj++)
      {
          if (ii == jj)
              zz[ii*odd_order+jj] = (2*ii + 1)*(2*ii + 1);
          else if (ii == jj + 1 || ii + 1 == jj)
              zz[ii*odd_order+jj] = qq;
      }

  zz[0] -= qq;

  /* Compute (and sort) the eigenvalues of the matrix. */
  mat = gsl_matrix_view_array(zz, odd_order, odd_order);
  eval = gsl_vector_subvector(work->eval, 0, odd_order);
  evec = gsl_matrix_submatrix(work->evec, 0, 0, odd_order, odd_order);
  gsl_eigen_symmv(&mat.matrix, &eval.vector, &evec.matrix, wmat);
  gsl_eigen_symmv_sort(&eval.vector, &evec.matrix, GSL_EIGEN_SORT_VAL_ASC);
  
  for (ii=0; ii<odd_order-extra_values; ii++)
      bb[2*ii+1] = gsl_vector_get(&eval.vector, ii);  

  for (ii = order_min ; ii <= order_max ; ii++)
    {
      result_array[ii - order_min] = bb[ii];
    }

  return GSL_SUCCESS;
}
