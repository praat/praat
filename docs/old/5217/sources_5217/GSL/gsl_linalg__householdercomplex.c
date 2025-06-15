/* linalg/householdercomplex.c
 * 
 * Copyright (C) 2001, 2007 Brian Gough
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

/* Computes a householder transformation matrix H such that
 *
 *       H' v = -/+ |v| e_1
 *
 * where e_1 is the first unit vector.  On exit the matrix H can be
 * computed from the return values (tau, v)
 *
 *       H = I - tau * w * w'
 *
 * where w = (1, v(2), ..., v(N)). The nonzero element of the result
 * vector -/+|v| e_1 is stored in v(1).
 *
 * Note that the matrix H' in the householder transformation is the
 * hermitian conjugate of H.  To compute H'v, pass the conjugate of
 * tau as the first argument to gsl_linalg_householder_hm() rather
 * than tau itself. See the LAPACK function CLARFG for details of this
 * convention.  */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_blas.h"
#include "gsl_complex_math.h"

#include "gsl_linalg.h"

gsl_complex
gsl_linalg_complex_householder_transform (gsl_vector_complex * v)
{
  /* replace v[0:n-1] with a householder vector (v[0:n-1]) and
     coefficient tau that annihilate v[1:n-1] */

  const size_t n = v->size ;
  
  if (n == 1)
    {
      gsl_complex alpha = gsl_vector_complex_get (v, 0) ;      
      double absa = gsl_complex_abs (alpha);
      double beta_r = - (GSL_REAL(alpha) >= 0 ? +1 : -1) * absa ;

      gsl_complex tau;

      if (beta_r == 0.0)
        {
          GSL_REAL(tau) = 0.0;
          GSL_IMAG(tau) = 0.0;
        }
      else 
        {
          GSL_REAL(tau) = (beta_r - GSL_REAL(alpha)) / beta_r ;
          GSL_IMAG(tau) = - GSL_IMAG(alpha) / beta_r ;

          {
            gsl_complex beta = gsl_complex_rect (beta_r, 0.0);
            gsl_vector_complex_set (v, 0, beta) ;
          }
        }
      
      return tau;
    }
  else
    { 
      gsl_complex tau ;
      double beta_r;

      gsl_vector_complex_view x = gsl_vector_complex_subvector (v, 1, n - 1) ; 
      gsl_complex alpha = gsl_vector_complex_get (v, 0) ;            
      double absa = gsl_complex_abs (alpha);
      double xnorm = gsl_blas_dznrm2 (&x.vector);
      
      if (xnorm == 0 && GSL_IMAG(alpha) == 0) 
        {
          gsl_complex zero = gsl_complex_rect(0.0, 0.0);
          return zero; /* tau = 0 */
        }
      
      beta_r = - (GSL_REAL(alpha) >= 0 ? +1 : -1) * hypot(absa, xnorm) ;

      GSL_REAL(tau) = (beta_r - GSL_REAL(alpha)) / beta_r ;
      GSL_IMAG(tau) = - GSL_IMAG(alpha) / beta_r ;

      {
        gsl_complex amb = gsl_complex_sub_real(alpha, beta_r);
        gsl_complex s = gsl_complex_inverse(amb);
        gsl_blas_zscal (s, &x.vector);
      }
      
      {
        gsl_complex beta = gsl_complex_rect (beta_r, 0.0);
        gsl_vector_complex_set (v, 0, beta) ;
      }
      
      return tau;
    }
}

int
gsl_linalg_complex_householder_hm (gsl_complex tau, const gsl_vector_complex * v, gsl_matrix_complex * A)
{
  /* applies a householder transformation v,tau to matrix m */

  size_t i, j;

  if (GSL_REAL(tau) == 0.0 && GSL_IMAG(tau) == 0.0)
    {
      return GSL_SUCCESS;
    }

  /* w = (v' A)^T */

  for (j = 0; j < A->size2; j++)
    {
      gsl_complex tauwj;
      gsl_complex wj = gsl_matrix_complex_get(A,0,j);  

      for (i = 1; i < A->size1; i++)  /* note, computed for v(0) = 1 above */
        {
          gsl_complex Aij = gsl_matrix_complex_get(A,i,j);
          gsl_complex vi = gsl_vector_complex_get(v,i);
          gsl_complex Av = gsl_complex_mul (Aij, gsl_complex_conjugate(vi));
          wj = gsl_complex_add (wj, Av);
        }

      tauwj = gsl_complex_mul (tau, wj);

      /* A = A - v w^T */
      
      {
        gsl_complex A0j = gsl_matrix_complex_get (A, 0, j);
        gsl_complex Atw = gsl_complex_sub (A0j, tauwj);
        /* store A0j - tau  * wj */
        gsl_matrix_complex_set (A, 0, j, Atw);
      }
      
      for (i = 1; i < A->size1; i++)
        {
          gsl_complex vi = gsl_vector_complex_get (v, i);
          gsl_complex tauvw = gsl_complex_mul(vi, tauwj);
          gsl_complex Aij = gsl_matrix_complex_get (A, i, j);
          gsl_complex Atwv = gsl_complex_sub (Aij, tauvw);
          /* store Aij - tau * vi * wj */
          gsl_matrix_complex_set (A, i, j, Atwv);
        }
    }
      
  return GSL_SUCCESS;
}

int
gsl_linalg_complex_householder_mh (gsl_complex tau, const gsl_vector_complex * v, gsl_matrix_complex * A)
{
  /* applies a householder transformation v,tau to matrix m on the right */

  size_t i, j;

  if (GSL_REAL(tau) == 0.0 && GSL_IMAG(tau) == 0.0)
    {
      return GSL_SUCCESS;
    }

  /* A -> A - A*tau*v*v^h */

  for (i = 0; i < A->size1; i++)
    {
      gsl_complex tauwi;
      gsl_complex Ai0 = gsl_matrix_complex_get (A, i, 0);
      gsl_complex wi = Ai0;

      /* compute w = A v */
      for (j = 1; j < A->size2; j++)  /* note, computed for v(0) = 1 above */
        {
          gsl_complex Aij = gsl_matrix_complex_get(A, i, j);
          gsl_complex vj = gsl_vector_complex_get(v, j);
          gsl_complex Av = gsl_complex_mul (Aij, vj);
          wi = gsl_complex_add (wi, Av);
        }

      tauwi = gsl_complex_mul (tau, wi);

      /* A = A - w v^H */
      
      {
        gsl_complex Atw = gsl_complex_sub (Ai0, tauwi);
        /* store Ai0 - tau  * wi */
        gsl_matrix_complex_set (A, i, 0, Atw);
      }
      
      for (j = 1; j < A->size2; j++)
        {
          gsl_complex vj = gsl_vector_complex_get (v, j);
          gsl_complex tauwv = gsl_complex_mul(gsl_complex_conjugate(vj), tauwi);
          gsl_complex Aij = gsl_matrix_complex_get (A, i, j);
          gsl_complex Atwv = gsl_complex_sub (Aij, tauwv);
          /* store Aij - tau * wi * conj(vj) */
          gsl_matrix_complex_set (A, i, j, Atwv);
        }
    }
      
  return GSL_SUCCESS;
}

int
gsl_linalg_complex_householder_hv (gsl_complex tau, const gsl_vector_complex * v, gsl_vector_complex *  w)
{
  const size_t N = v->size;

  if (GSL_REAL(tau) == 0.0 && GSL_IMAG(tau) == 0.0)
      return GSL_SUCCESS;

  {
    /* compute z = v'w */

    gsl_complex z0 = gsl_vector_complex_get(w,0);
    gsl_complex z1, z;
    gsl_complex tz, ntz;
    
    gsl_vector_complex_const_view v1 = gsl_vector_complex_const_subvector(v, 1, N-1);
    gsl_vector_complex_view w1 = gsl_vector_complex_subvector(w, 1, N-1);

    gsl_blas_zdotc(&v1.vector, &w1.vector, &z1);
    
    z = gsl_complex_add (z0, z1);

    tz = gsl_complex_mul(tau, z);
    ntz = gsl_complex_negative (tz);

    /* compute w = w - tau * (v'w) * v   */

    {
      gsl_complex w0 = gsl_vector_complex_get(w, 0);
      gsl_complex w0ntz = gsl_complex_add (w0, ntz);
      gsl_vector_complex_set (w, 0, w0ntz);
    }

    gsl_blas_zaxpy(ntz, &v1.vector, &w1.vector);
  }

  return GSL_SUCCESS;
}
