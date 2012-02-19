/* eigen/schur.c
 * 
 * Copyright (C) 2006, 2007 Patrick Alken
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

#include "gsl__config.h"
#include "gsl_eigen.h"
#include "gsl_math.h"
#include "gsl_matrix.h"
#include "gsl_vector.h"
#include "gsl_vector_complex.h"
#include "gsl_blas.h"
#include "gsl_complex.h"
#include "gsl_complex_math.h"

/*
 * This module contains some routines related to manipulating the
 * Schur form of a matrix which are needed by the eigenvalue solvers
 *
 * This file contains routines based on original code from LAPACK
 * which is distributed under the modified BSD license.
 */

#define GSL_SCHUR_SMLNUM (2.0 * GSL_DBL_MIN)
#define GSL_SCHUR_BIGNUM ((1.0 - GSL_DBL_EPSILON) / GSL_SCHUR_SMLNUM)

/*
gsl_schur_gen_eigvals()
  Compute the eigenvalues of a 2-by-2 generalized block.

Inputs: A      - 2-by-2 matrix
        B      - 2-by-2 upper triangular matrix
        wr1    - (output) see notes
        wr2    - (output) see notes
        wi     - (output) see notes
        scale1 - (output) see notes
        scale2 - (output) see notes

Return: success

Notes:

1)

If the block contains real eigenvalues, then wi is set to 0,
and wr1, wr2, scale1, and scale2 are set such that:

eval1 = wr1 * scale1
eval2 = wr2 * scale2

If the block contains complex eigenvalues, then wr1, wr2, scale1,
scale2, and wi are set such that:

wr1 = wr2 = scale1 * Re(eval)
wi = scale1 * Im(eval)

wi is always non-negative

2) This routine is based on LAPACK DLAG2
*/

int
gsl_schur_gen_eigvals(const gsl_matrix *A, const gsl_matrix *B, double *wr1,
                      double *wr2, double *wi, double *scale1,
                      double *scale2)
{
  const double safemin = GSL_DBL_MIN * 1.0e2;
  const double safemax = 1.0 / safemin;
  const double rtmin = sqrt(safemin);
  const double rtmax = 1.0 / rtmin;
  double anorm, bnorm;
  double ascale, bscale, bsize;
  double s1, s2;
  double A11, A12, A21, A22;
  double B11, B12, B22;
  double binv11, binv22;
  double bmin;
  double as11, as12, as22, abi22;
  double pp, qq, shift, ss, discr, r;

  /* scale A */
  anorm = GSL_MAX(GSL_MAX(fabs(gsl_matrix_get(A, 0, 0)) +
                          fabs(gsl_matrix_get(A, 1, 0)),
                          fabs(gsl_matrix_get(A, 0, 1)) +
                          fabs(gsl_matrix_get(A, 1, 1))),
                  safemin);
  ascale = 1.0 / anorm;
  A11 = ascale * gsl_matrix_get(A, 0, 0);
  A12 = ascale * gsl_matrix_get(A, 0, 1);
  A21 = ascale * gsl_matrix_get(A, 1, 0);
  A22 = ascale * gsl_matrix_get(A, 1, 1);

  /* perturb B if necessary to ensure non-singularity */
  B11 = gsl_matrix_get(B, 0, 0);
  B12 = gsl_matrix_get(B, 0, 1);
  B22 = gsl_matrix_get(B, 1, 1);
  bmin = rtmin * GSL_MAX(fabs(B11),
                         GSL_MAX(fabs(B12), GSL_MAX(fabs(B22), rtmin)));
  if (fabs(B11) < bmin)
    B11 = GSL_SIGN(B11) * bmin;
  if (fabs(B22) < bmin)
    B22 = GSL_SIGN(B22) * bmin;

  /* scale B */
  bnorm = GSL_MAX(fabs(B11), GSL_MAX(fabs(B12) + fabs(B22), safemin));
  bsize = GSL_MAX(fabs(B11), fabs(B22));
  bscale = 1.0 / bsize;
  B11 *= bscale;
  B12 *= bscale;
  B22 *= bscale;

  /* compute larger eigenvalue */

  binv11 = 1.0 / B11;
  binv22 = 1.0 / B22;
  s1 = A11 * binv11;
  s2 = A22 * binv22;
  if (fabs(s1) <= fabs(s2))
    {
      as12 = A12 - s1 * B12;
      as22 = A22 - s1 * B22;
      ss = A21 * (binv11 * binv22);
      abi22 = as22 * binv22 - ss * B12;
      pp = 0.5 * abi22;
      shift = s1;
    }
  else
    {
      as12 = A12 - s2 * B12;
      as11 = A11 - s2 * B11;
      ss = A21 * (binv11 * binv22);
      abi22 = -ss * B12;
      pp = 0.5 * (as11 * binv11 + abi22);
      shift = s2;
    }

  qq = ss * as12;
  if (fabs(pp * rtmin) >= 1.0)
    {
      discr = (rtmin * pp) * (rtmin * pp) + qq * safemin;
      r = sqrt(fabs(discr)) * rtmax;
    }
  else if (pp * pp + fabs(qq) <= safemin)
    {
      discr = (rtmax * pp) * (rtmax * pp) + qq * safemax;
      r = sqrt(fabs(discr)) * rtmin;
    }
  else
    {
      discr = pp * pp + qq;
      r = sqrt(fabs(discr));
    }

  if (discr >= 0.0 || r == 0.0)
    {
      double sum = pp + GSL_SIGN(pp) * r;
      double diff = pp - GSL_SIGN(pp) * r;
      double wbig = shift + sum;
      double wsmall = shift + diff;

      /* compute smaller eigenvalue */

      if (0.5 * fabs(wbig) > GSL_MAX(fabs(wsmall), safemin))
        {
          double wdet = (A11*A22 - A12*A21) * (binv11 * binv22);
          wsmall = wdet / wbig;
        }

      /* choose (real) eigenvalue closest to 2,2 element of AB^{-1} for wr1 */
      if (pp > abi22)
        {
          *wr1 = GSL_MIN(wbig, wsmall);
          *wr2 = GSL_MAX(wbig, wsmall);
        }
      else
        {
          *wr1 = GSL_MAX(wbig, wsmall);
          *wr2 = GSL_MIN(wbig, wsmall);
        }
      *wi = 0.0;
    }
  else
    {
      /* complex eigenvalues */
      *wr1 = shift + pp;
      *wr2 = *wr1;
      *wi = r;
    }

  /* compute scaling */
  {
    const double fuzzy1 = 1.0 + 1.0e-5;
    double c1, c2, c3, c4, c5;
    double wabs, wsize, wscale;

    c1 = bsize * (safemin * GSL_MAX(1.0, ascale));
    c2 = safemin * GSL_MAX(1.0, bnorm);
    c3 = bsize * safemin;
    if (ascale <= 1.0 && bsize <= 1.0)
      c4 = GSL_MIN(1.0, (ascale / safemin) * bsize);
    else
      c4 = 1.0;

    if (ascale <= 1.0 || bsize <= 1.0)
      c5 = GSL_MIN(1.0, ascale * bsize);
    else
      c5 = 1.0;

    /* scale first eigenvalue */
    wabs = fabs(*wr1) + fabs(*wi);
    wsize = GSL_MAX(safemin,
              GSL_MAX(c1,
                GSL_MAX(fuzzy1 * (wabs*c2 + c3),
                  GSL_MIN(c4, 0.5 * GSL_MAX(wabs, c5)))));
    if (wsize != 1.0)
      {
        wscale = 1.0 / wsize;
        if (wsize > 1.0)
          {
            *scale1 = (GSL_MAX(ascale, bsize) * wscale) *
                      GSL_MIN(ascale, bsize);
          }
        else
          {
            *scale1 = (GSL_MIN(ascale, bsize) * wscale) *
                      GSL_MAX(ascale, bsize);
          }

        *wr1 *= wscale;
        if (*wi != 0.0)
          {
            *wi *= wscale;
            *wr2 = *wr1;
            *scale2 = *scale1;
          }
      }
    else
      {
        *scale1 = ascale * bsize;
        *scale2 = *scale1;
      }

    /* scale second eigenvalue if real */
    if (*wi == 0.0)
      {
        wsize = GSL_MAX(safemin,
                  GSL_MAX(c1,
                    GSL_MAX(fuzzy1 * (fabs(*wr2) * c2 + c3),
                      GSL_MIN(c4, 0.5 * GSL_MAX(fabs(*wr2), c5)))));
        if (wsize != 1.0)
          {
            wscale = 1.0 / wsize;
            if (wsize > 1.0)
              {
                *scale2 = (GSL_MAX(ascale, bsize) * wscale) *
                          GSL_MIN(ascale, bsize);
              }
            else
              {
                *scale2 = (GSL_MIN(ascale, bsize) * wscale) *
                          GSL_MAX(ascale, bsize);
              }

            *wr2 *= wscale;
          }
        else
          {
            *scale2 = ascale * bsize;
          }
      }
  }

  return GSL_SUCCESS;
} /* gsl_schur_gen_eigvals() */

/*
gsl_schur_solve_equation()

  Solve the equation which comes up in the back substitution
when computing eigenvectors corresponding to real eigenvalues.
The equation that is solved is:

(ca*A - z*D)*x = s*b

where

A is n-by-n with n = 1 or 2
D is a n-by-n diagonal matrix
b and x are n-by-1 real vectors
s is a scaling factor set by this function to prevent overflow in x

Inputs: ca    - coefficient multiplying A
        A     - square matrix (n-by-n)
        z     - real scalar (eigenvalue)
        d1    - (1,1) element in diagonal matrix D
        d2    - (2,2) element in diagonal matrix D
        b     - right hand side vector
        x     - (output) where to store solution
        s     - (output) scale factor
        xnorm - (output) infinity norm of X
        smin  - lower bound on singular values of A - if ca*A - z*D
                is less than this value, we'll use smin*I instead.
                This value should be a safe distance above underflow.

Return: success

Notes: 1) A and b are not changed on output
       2) Based on lapack routine DLALN2
*/

int
gsl_schur_solve_equation(double ca, const gsl_matrix *A, double z,
                         double d1, double d2, const gsl_vector *b,
                         gsl_vector *x, double *s, double *xnorm,
                         double smin)
{
  size_t N = A->size1;
  double bnorm;
  double scale = 1.0;
  
  if (N == 1)
    {
      double c,     /* denominator */
             cnorm; /* |c| */

      /* we have a 1-by-1 (real) scalar system to solve */

      c = ca * gsl_matrix_get(A, 0, 0) - z * d1;
      cnorm = fabs(c);

      if (cnorm < smin)
        {
          /* set c = smin*I */
          c = smin;
          cnorm = smin;
        }

      /* check scaling for x = b / c */
      bnorm = fabs(gsl_vector_get(b, 0));
      if (cnorm < 1.0 && bnorm > 1.0)
        {
          if (bnorm > GSL_SCHUR_BIGNUM*cnorm)
            scale = 1.0 / bnorm;
        }

      /* compute x */
      gsl_vector_set(x, 0, gsl_vector_get(b, 0) * scale / c);
      *xnorm = fabs(gsl_vector_get(x, 0));
    } /* if (N == 1) */
  else
    {
      double cr[2][2];
      double *crv;
      double cmax;
      size_t icmax, j;
      double bval1, bval2;
      double ur11, ur12, ur22, ur11r;
      double cr21, cr22;
      double lr21;
      double b1, b2, bbnd;
      double x1, x2;
      double temp;
      size_t ipivot[4][4] = { { 0, 1, 2, 3 },
                              { 1, 0, 3, 2 },
                              { 2, 3, 0, 1 },
                              { 3, 2, 1, 0 } };
      int rswap[4] = { 0, 1, 0, 1 };
      int zswap[4] = { 0, 0, 1, 1 };

      /*
       * we have a 2-by-2 real system to solve:
       *
       * ( ca * [ A11  A12 ] - z * [ D1 0  ] ) [ x1 ] = [ b1 ]
       * (      [ A21  A22 ]       [ 0  D2 ] ) [ x2 ]   [ b2 ]
       *
       * (z real)
       */

      crv = (double *) cr;

      /*
       * compute the real part of C = ca*A - z*D - use column ordering
       * here since porting from lapack
       */
      cr[0][0] = ca * gsl_matrix_get(A, 0, 0) - z * d1;
      cr[1][1] = ca * gsl_matrix_get(A, 1, 1) - z * d2;
      cr[0][1] = ca * gsl_matrix_get(A, 1, 0);
      cr[1][0] = ca * gsl_matrix_get(A, 0, 1);

      /* find the largest element in C */
      cmax = 0.0;
      icmax = 0;
      for (j = 0; j < 4; ++j)
        {
          if (fabs(crv[j]) > cmax)
            {
              cmax = fabs(crv[j]);
              icmax = j;
            }
        }

      bval1 = gsl_vector_get(b, 0);
      bval2 = gsl_vector_get(b, 1);

      /* if norm(C) < smin, use smin*I */

      if (cmax < smin)
        {
          bnorm = GSL_MAX(fabs(bval1), fabs(bval2));
          if (smin < 1.0 && bnorm > 1.0)
            {
              if (bnorm > GSL_SCHUR_BIGNUM*smin)
                scale = 1.0 / bnorm;
            }
          temp = scale / smin;
          gsl_vector_set(x, 0, temp * bval1);
          gsl_vector_set(x, 1, temp * bval2);
          *xnorm = temp * bnorm;
          *s = scale;
          return GSL_SUCCESS;
        }

      /* gaussian elimination with complete pivoting */
      ur11 = crv[icmax];
      cr21 = crv[ipivot[1][icmax]];
      ur12 = crv[ipivot[2][icmax]];
      cr22 = crv[ipivot[3][icmax]];
      ur11r = 1.0 / ur11;
      lr21 = ur11r * cr21;
      ur22 = cr22 - ur12 * lr21;

      /* if smaller pivot < smin, use smin */
      if (fabs(ur22) < smin)
        ur22 = smin;

      if (rswap[icmax])
        {
          b1 = bval2;
          b2 = bval1;
        }
      else
        {
          b1 = bval1;
          b2 = bval2;
        }

      b2 -= lr21 * b1;
      bbnd = GSL_MAX(fabs(b1 * (ur22 * ur11r)), fabs(b2));
      if (bbnd > 1.0 && fabs(ur22) < 1.0)
        {
          if (bbnd >= GSL_SCHUR_BIGNUM * fabs(ur22))
            scale = 1.0 / bbnd;
        }

      x2 = (b2 * scale) / ur22;
      x1 = (scale * b1) * ur11r - x2 * (ur11r * ur12);
      if (zswap[icmax])
        {
          gsl_vector_set(x, 0, x2);
          gsl_vector_set(x, 1, x1);
        }
      else
        {
          gsl_vector_set(x, 0, x1);
          gsl_vector_set(x, 1, x2);
        }

      *xnorm = GSL_MAX(fabs(x1), fabs(x2));

      /* further scaling if norm(A) norm(X) > overflow */
      if (*xnorm > 1.0 && cmax > 1.0)
        {
          if (*xnorm > GSL_SCHUR_BIGNUM / cmax)
            {
              temp = cmax / GSL_SCHUR_BIGNUM;
              gsl_blas_dscal(temp, x);
              *xnorm *= temp;
              scale *= temp;
            }
        }
    } /* if (N == 2) */

  *s = scale;
  return GSL_SUCCESS;
} /* gsl_schur_solve_equation() */

/*
gsl_schur_solve_equation_z()

  Solve the equation which comes up in the back substitution
when computing eigenvectors corresponding to complex eigenvalues.
The equation that is solved is:

(ca*A - z*D)*x = s*b

where

A is n-by-n with n = 1 or 2
D is a n-by-n diagonal matrix
b and x are n-by-1 complex vectors
s is a scaling factor set by this function to prevent overflow in x

Inputs: ca    - coefficient multiplying A
        A     - square matrix (n-by-n)
        z     - complex scalar (eigenvalue)
        d1    - (1,1) element in diagonal matrix D
        d2    - (2,2) element in diagonal matrix D
        b     - right hand side vector
        x     - (output) where to store solution
        s     - (output) scale factor
        xnorm - (output) infinity norm of X
        smin  - lower bound on singular values of A - if ca*A - z*D
                is less than this value, we'll use smin*I instead.
                This value should be a safe distance above underflow.

Notes: 1) A and b are not changed on output
       2) Based on lapack routine DLALN2
*/

int
gsl_schur_solve_equation_z(double ca, const gsl_matrix *A, gsl_complex *z,
                           double d1, double d2,
                           const gsl_vector_complex *b,
                           gsl_vector_complex *x, double *s, double *xnorm,
                           double smin)
{
  size_t N = A->size1;
  double scale = 1.0;
  double bnorm;

  if (N == 1)
    {
      double cr,    /* denominator */
             ci,
             cnorm; /* |c| */
      gsl_complex bval, c, xval, tmp;

      /* we have a 1-by-1 (complex) scalar system to solve */

      /* c = ca*a - z*d1 */
      cr = ca * gsl_matrix_get(A, 0, 0) - GSL_REAL(*z) * d1;
      ci = -GSL_IMAG(*z) * d1;
      cnorm = fabs(cr) + fabs(ci);

      if (cnorm < smin)
        {
          /* set c = smin*I */
          cr = smin;
          ci = 0.0;
          cnorm = smin;
        }

      /* check scaling for x = b / c */
      bval = gsl_vector_complex_get(b, 0);
      bnorm = fabs(GSL_REAL(bval)) + fabs(GSL_IMAG(bval));
      if (cnorm < 1.0 && bnorm > 1.0)
        {
          if (bnorm > GSL_SCHUR_BIGNUM*cnorm)
            scale = 1.0 / bnorm;
        }

      /* compute x */
      GSL_SET_COMPLEX(&tmp, scale*GSL_REAL(bval), scale*GSL_IMAG(bval));
      GSL_SET_COMPLEX(&c, cr, ci);
      xval = gsl_complex_div(tmp, c);

      gsl_vector_complex_set(x, 0, xval);

      *xnorm = fabs(GSL_REAL(xval)) + fabs(GSL_IMAG(xval));
    } /* if (N == 1) */
  else
    {
      double cr[2][2], ci[2][2];
      double *civ, *crv;
      double cmax;
      gsl_complex bval1, bval2;
      gsl_complex xval1, xval2;
      double xr1, xi1;
      size_t icmax;
      size_t j;
      double temp;
      double ur11, ur12, ur22, ui11, ui12, ui22, ur11r, ui11r;
      double ur12s, ui12s;
      double u22abs;
      double lr21, li21;
      double cr21, cr22, ci21, ci22;
      double br1, bi1, br2, bi2, bbnd;
      gsl_complex b1, b2;
      size_t ipivot[4][4] = { { 0, 1, 2, 3 },
                              { 1, 0, 3, 2 },
                              { 2, 3, 0, 1 },
                              { 3, 2, 1, 0 } };
      int rswap[4] = { 0, 1, 0, 1 };
      int zswap[4] = { 0, 0, 1, 1 };

      /*
       * complex 2-by-2 system:
       *
       * ( ca * [ A11 A12 ] - z * [ D1 0 ] ) [ X1 ] = [ B1 ]
       * (      [ A21 A22 ]       [ 0  D2] ) [ X2 ]   [ B2 ]
       *
       * (z complex)
       *
       * where the X and B values are complex.
       */

      civ = (double *) ci;
      crv = (double *) cr;

      /*
       * compute the real part of C = ca*A - z*D - use column ordering
       * here since porting from lapack
       */
      cr[0][0] = ca*gsl_matrix_get(A, 0, 0) - GSL_REAL(*z)*d1;
      cr[1][1] = ca*gsl_matrix_get(A, 1, 1) - GSL_REAL(*z)*d2;
      cr[0][1] = ca*gsl_matrix_get(A, 1, 0);
      cr[1][0] = ca*gsl_matrix_get(A, 0, 1);

      /* compute the imaginary part */
      ci[0][0] = -GSL_IMAG(*z) * d1;
      ci[0][1] = 0.0;
      ci[1][0] = 0.0;
      ci[1][1] = -GSL_IMAG(*z) * d2;

      cmax = 0.0;
      icmax = 0;

      for (j = 0; j < 4; ++j)
        {
          if (fabs(crv[j]) + fabs(civ[j]) > cmax)
            {
              cmax = fabs(crv[j]) + fabs(civ[j]);
              icmax = j;
            }
        }

      bval1 = gsl_vector_complex_get(b, 0);
      bval2 = gsl_vector_complex_get(b, 1);

      /* if norm(C) < smin, use smin*I */
      if (cmax < smin)
        {
          bnorm = GSL_MAX(fabs(GSL_REAL(bval1)) + fabs(GSL_IMAG(bval1)),
                          fabs(GSL_REAL(bval2)) + fabs(GSL_IMAG(bval2)));
          if (smin < 1.0 && bnorm > 1.0)
            {
              if (bnorm > GSL_SCHUR_BIGNUM*smin)
                scale = 1.0 / bnorm;
            }

          temp = scale / smin;
          xval1 = gsl_complex_mul_real(bval1, temp);
          xval2 = gsl_complex_mul_real(bval2, temp);
          gsl_vector_complex_set(x, 0, xval1);
          gsl_vector_complex_set(x, 1, xval2);
          *xnorm = temp * bnorm;
          *s = scale;
          return GSL_SUCCESS;
        }

      /* gaussian elimination with complete pivoting */
      ur11 = crv[icmax];
      ui11 = civ[icmax];
      cr21 = crv[ipivot[1][icmax]];
      ci21 = civ[ipivot[1][icmax]];
      ur12 = crv[ipivot[2][icmax]];
      ui12 = civ[ipivot[2][icmax]];
      cr22 = crv[ipivot[3][icmax]];
      ci22 = civ[ipivot[3][icmax]];

      if (icmax == 0 || icmax == 3)
        {
          /* off diagonals of pivoted C are real */
          if (fabs(ur11) > fabs(ui11))
            {
              temp = ui11 / ur11;
              ur11r = 1.0 / (ur11 * (1.0 + temp*temp));
              ui11r = -temp * ur11r;
            }
          else
            {
              temp = ur11 / ui11;
              ui11r = -1.0 / (ui11 * (1.0 + temp*temp));
              ur11r = -temp*ui11r;
            }
          lr21 = cr21 * ur11r;
          li21 = cr21 * ui11r;
          ur12s = ur12 * ur11r;
          ui12s = ur12 * ui11r;
          ur22 = cr22 - ur12 * lr21;
          ui22 = ci22 - ur12 * li21;
        }
      else
        {
          /* diagonals of pivoted C are real */
          ur11r = 1.0 / ur11;
          ui11r = 0.0;
          lr21 = cr21 * ur11r;
          li21 = ci21 * ur11r;
          ur12s = ur12 * ur11r;
          ui12s = ui12 * ur11r;
          ur22 = cr22 - ur12 * lr21 + ui12 * li21;
          ui22 = -ur12 * li21 - ui12 * lr21;
        }

      u22abs = fabs(ur22) + fabs(ui22);

      /* if smaller pivot < smin, use smin */
      if (u22abs < smin)
        {
          ur22 = smin;
          ui22 = 0.0;
        }

      if (rswap[icmax])
        {
          br2 = GSL_REAL(bval1);
          bi2 = GSL_IMAG(bval1);
          br1 = GSL_REAL(bval2);
          bi1 = GSL_IMAG(bval2);
        }
      else
        {
          br1 = GSL_REAL(bval1);
          bi1 = GSL_IMAG(bval1);
          br2 = GSL_REAL(bval2);
          bi2 = GSL_IMAG(bval2);
        }

      br2 += li21*bi1 - lr21*br1;
      bi2 -= li21*br1 + lr21*bi1;
      bbnd = GSL_MAX((fabs(br1) + fabs(bi1)) *
                     (u22abs * (fabs(ur11r) + fabs(ui11r))),
                     fabs(br2) + fabs(bi2));
      if (bbnd > 1.0 && u22abs < 1.0)
        {
          if (bbnd >= GSL_SCHUR_BIGNUM*u22abs)
            {
              scale = 1.0 / bbnd;
              br1 *= scale;
              bi1 *= scale;
              br2 *= scale;
              bi2 *= scale;
            }
        }

      GSL_SET_COMPLEX(&b1, br2, bi2);
      GSL_SET_COMPLEX(&b2, ur22, ui22);
      xval2 = gsl_complex_div(b1, b2);

      xr1 = ur11r*br1 - ui11r*bi1 - ur12s*GSL_REAL(xval2) + ui12s*GSL_IMAG(xval2);
      xi1 = ui11r*br1 + ur11r*bi1 - ui12s*GSL_REAL(xval2) - ur12s*GSL_IMAG(xval2);
      GSL_SET_COMPLEX(&xval1, xr1, xi1);

      if (zswap[icmax])
        {
          gsl_vector_complex_set(x, 0, xval2);
          gsl_vector_complex_set(x, 1, xval1);
        }
      else
        {
          gsl_vector_complex_set(x, 0, xval1);
          gsl_vector_complex_set(x, 1, xval2);
        }

      *xnorm = GSL_MAX(fabs(GSL_REAL(xval1)) + fabs(GSL_IMAG(xval1)),
                       fabs(GSL_REAL(xval2)) + fabs(GSL_IMAG(xval2)));

      /* further scaling if norm(A) norm(X) > overflow */
      if (*xnorm > 1.0 && cmax > 1.0)
        {
          if (*xnorm > GSL_SCHUR_BIGNUM / cmax)
            {
              temp = cmax / GSL_SCHUR_BIGNUM;
              gsl_blas_zdscal(temp, x);
              *xnorm *= temp;
              scale *= temp;
            }
        }
    } /* if (N == 2) */

  *s = scale;
  return GSL_SUCCESS;
} /* gsl_schur_solve_equation_z() */
