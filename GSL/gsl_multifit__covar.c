/* multifit/covar.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Brian Gough
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
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_permutation.h"
#include "gsl_linalg.h"
#include "gsl_multifit_nlin.h"

/* Compute the covariance matrix

   cov = inv (J^T J)

   by QRP^T decomposition of J
*/

int
gsl_multifit_covar (const gsl_matrix * J, double epsrel, gsl_matrix * covar)
{
  double tolr;

  size_t i, j, k;
  size_t kmax = 0;

  gsl_matrix * r;
  gsl_vector * tau;
  gsl_vector * norm;
  gsl_permutation * perm;

  size_t m = J->size1, n = J->size2 ;
  
  if (m < n) 
    {
      GSL_ERROR ("Jacobian be rectangular M x N with M >= N", GSL_EBADLEN);
    }

  if (covar->size1 != covar->size2 || covar->size1 != n)
    {
      GSL_ERROR ("covariance matrix must be square and match second dimension of jacobian", GSL_EBADLEN);
    }

  r = gsl_matrix_alloc (m, n);
  tau = gsl_vector_alloc (n);
  perm = gsl_permutation_alloc (n) ;
  norm = gsl_vector_alloc (n) ;
  
  {
    int signum = 0;
    gsl_matrix_memcpy (r, J);
    gsl_linalg_QRPT_decomp (r, tau, perm, &signum, norm);
  }
  
  
  /* Form the inverse of R in the full upper triangle of R */

  tolr = epsrel * fabs(gsl_matrix_get(r, 0, 0));

  for (k = 0 ; k < n ; k++)
    {
      double rkk = gsl_matrix_get(r, k, k);

      if (fabs(rkk) <= tolr)
        {
          break;
        }

      gsl_matrix_set(r, k, k, 1.0/rkk);

      for (j = 0; j < k ; j++)
        {
          double t = gsl_matrix_get(r, j, k) / rkk;
          gsl_matrix_set (r, j, k, 0.0);

          for (i = 0; i <= j; i++)
            {
              double rik = gsl_matrix_get (r, i, k);
              double rij = gsl_matrix_get (r, i, j);
              
              gsl_matrix_set (r, i, k, rik - t * rij);
            }
        }
      kmax = k;
    }

  /* Form the full upper triangle of the inverse of R^T R in the full
     upper triangle of R */

  for (k = 0; k <= kmax ; k++)
    {
      for (j = 0; j < k; j++)
        {
          double rjk = gsl_matrix_get (r, j, k);

          for (i = 0; i <= j ; i++)
            {
              double rij = gsl_matrix_get (r, i, j);
              double rik = gsl_matrix_get (r, i, k);

              gsl_matrix_set (r, i, j, rij + rjk * rik);
            }
        }
      
      {
        double t = gsl_matrix_get (r, k, k);

        for (i = 0; i <= k; i++)
          {
            double rik = gsl_matrix_get (r, i, k);

            gsl_matrix_set (r, i, k, t * rik);
          };
      }
    }

  /* Form the full lower triangle of the covariance matrix in the
     strict lower triangle of R and in w */

  for (j = 0 ; j < n ; j++)
    {
      size_t pj = gsl_permutation_get (perm, j);
      
      for (i = 0; i <= j; i++)
        {
          size_t pi = gsl_permutation_get (perm, i);

          double rij;

          if (j > kmax)
            {
              gsl_matrix_set (r, i, j, 0.0);
              rij = 0.0 ;
            }
          else 
            {
              rij = gsl_matrix_get (r, i, j);
            }

          if (pi > pj)
            {
              gsl_matrix_set (r, pi, pj, rij); 
            } 
          else if (pi < pj)
            {
              gsl_matrix_set (r, pj, pi, rij);
            }

        }
      
      { 
        double rjj = gsl_matrix_get (r, j, j);
        gsl_matrix_set (covar, pj, pj, rjj);
      }
    }

     
  /* symmetrize the covariance matrix */

  for (j = 0 ; j < n ; j++)
    {
      for (i = 0; i < j ; i++)
        {
          double rji = gsl_matrix_get (r, j, i);

          gsl_matrix_set (covar, j, i, rji);
          gsl_matrix_set (covar, i, j, rji);
        }
    }

  gsl_matrix_free (r);
  gsl_permutation_free (perm);
  gsl_vector_free (tau);
  gsl_vector_free (norm);

  return GSL_SUCCESS;
}
