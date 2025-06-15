/* eigen/jacobi.c
 * 
 * Copyright (C) 2004, 2007 Brian Gough, Gerard Jungman
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
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_eigen.h"

/* Algorithm 8.4.3 - Cyclic Jacobi.  Golub & Van Loan, Matrix Computations */

static inline double
symschur2 (gsl_matrix * A, size_t p, size_t q, double *c, double *s)
{
  double Apq = gsl_matrix_get (A, p, q);

  if (Apq != 0.0)
    {
      double App = gsl_matrix_get (A, p, p);
      double Aqq = gsl_matrix_get (A, q, q);
      double tau = (Aqq - App) / (2.0 * Apq);
      double t, c1;

      if (tau >= 0.0)
        {
          t = 1.0 / (tau + hypot (1.0, tau));
        }
      else
        {
          t = -1.0 / (-tau + hypot (1.0, tau));
        }

      c1 = 1.0 / hypot (1.0, t);

      *c = c1;
      *s = t * c1;
    }
  else
    {
      *c = 1.0;
      *s = 0.0;
    }

  /* reduction in off(A) is 2*(A_pq)^2 */

  return fabs (Apq);
}

inline static void
apply_jacobi_L (gsl_matrix * A, size_t p, size_t q, double c, double s)
{
  size_t j;
  const size_t N = A->size2;

  /* Apply rotation to matrix A,  A' = J^T A */

  for (j = 0; j < N; j++)
    {
      double Apj = gsl_matrix_get (A, p, j);
      double Aqj = gsl_matrix_get (A, q, j);
      gsl_matrix_set (A, p, j, Apj * c - Aqj * s);
      gsl_matrix_set (A, q, j, Apj * s + Aqj * c);
    }
}

inline static void
apply_jacobi_R (gsl_matrix * A, size_t p, size_t q, double c, double s)
{
  size_t i;
  const size_t M = A->size1;

  /* Apply rotation to matrix A,  A' = A J */

  for (i = 0; i < M; i++)
    {
      double Aip = gsl_matrix_get (A, i, p);
      double Aiq = gsl_matrix_get (A, i, q);
      gsl_matrix_set (A, i, p, Aip * c - Aiq * s);
      gsl_matrix_set (A, i, q, Aip * s + Aiq * c);
    }
}

inline static double
norm (gsl_matrix * A)
{
  size_t i, j, M = A->size1, N = A->size2;
  double sum = 0.0, scale = 0.0, ssq = 1.0;

  for (i = 0; i < M; i++)
    {
      for (j = 0; j < N; j++)
        {
          double Aij = gsl_matrix_get (A, i, j);

          if (Aij != 0.0)
            {
              double ax = fabs (Aij);

              if (scale < ax)
                {
                  ssq = 1.0 + ssq * (scale / ax) * (scale / ax);
                  scale = ax;
                }
              else
                {
                  ssq += (ax / scale) * (ax / scale);
                }
            }

        }
    }

  sum = scale * sqrt (ssq);

  return sum;
}

int
gsl_eigen_jacobi (gsl_matrix * a,
                  gsl_vector * eval,
                  gsl_matrix * evec, unsigned int max_rot, unsigned int *nrot)
{
  size_t i, p, q;
  const size_t M = a->size1, N = a->size2;
  double red, redsum = 0.0;

  if (M != N)
    {
      GSL_ERROR ("eigenproblem requires square matrix", GSL_ENOTSQR);
    }
  else if (M != evec->size1 || M != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must match input matrix", GSL_EBADLEN);
    }
  else if (M != eval->size)
    {
      GSL_ERROR ("eigenvalue vector must match input matrix", GSL_EBADLEN);
    }

  gsl_vector_set_zero (eval);
  gsl_matrix_set_identity (evec);

  for (i = 0; i < max_rot; i++)
    {
      double nrm = norm (a);

      if (nrm == 0.0)
        break;

      for (p = 0; p < N; p++)
        {
          for (q = p + 1; q < N; q++)
            {
              double c, s;

              red = symschur2 (a, p, q, &c, &s);
              redsum += red;

              /* Compute A <- J^T A J */
              apply_jacobi_L (a, p, q, c, s);
              apply_jacobi_R (a, p, q, c, s);

              /* Compute V <- V J */
              apply_jacobi_R (evec, p, q, c, s);
            }
        }
    }

  *nrot = i;

  for (p = 0; p < N; p++)
    {
      double ep = gsl_matrix_get (a, p, p);
      gsl_vector_set (eval, p, ep);
    }

  if (i == max_rot)
    {
      return GSL_EMAXITER;
    }

  return GSL_SUCCESS;
}

int
gsl_eigen_invert_jacobi (const gsl_matrix * a,
                         gsl_matrix * ainv, unsigned int max_rot)
{
  if (a->size1 != a->size2 || ainv->size1 != ainv->size2)
    {
      GSL_ERROR("jacobi method requires square matrix", GSL_ENOTSQR);
    }
  else if (a->size1 != ainv->size2)
    {
     GSL_ERROR ("inverse matrix must match input matrix", GSL_EBADLEN);
    }
  
  {
    const size_t n = a->size2;
    size_t i,j,k;
    unsigned int nrot = 0;
    int status;

    gsl_vector * eval = gsl_vector_alloc(n);
    gsl_matrix * evec = gsl_matrix_alloc(n, n);
    gsl_matrix * tmp = gsl_matrix_alloc(n, n);

    gsl_matrix_memcpy (tmp, a);

    status = gsl_eigen_jacobi(tmp, eval, evec, max_rot, &nrot);
      
    for(i=0; i<n; i++) 
      {
        for(j=0; j<n; j++) 
          {
            double ainv_ij = 0.0;
            
            for(k = 0; k<n; k++)
              {
                double f = 1.0 / gsl_vector_get(eval, k);
                double vik = gsl_matrix_get (evec, i, k);
                double vjk = gsl_matrix_get (evec, j, k);
                ainv_ij += vik * vjk * f;
              }
            gsl_matrix_set (ainv, i, j, ainv_ij);
          }
      }

    gsl_vector_free(eval);
    gsl_matrix_free(evec);
    gsl_matrix_free(tmp);

    if (status)
      {
        return status;
      }
    else
      {
        return GSL_SUCCESS;
      }
  }
}
