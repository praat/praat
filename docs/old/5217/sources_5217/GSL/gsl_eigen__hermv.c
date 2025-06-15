/* eigen/hermv.c
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

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_complex_math.h"
#include "gsl_linalg.h"
#include "gsl_eigen.h"

/* Compute eigenvalues/eigenvectors of complex hermitian matrix using
   reduction to real symmetric tridiagonal form, followed by QR
   iteration with implicit shifts.

   See Golub & Van Loan, "Matrix Computations" (3rd ed), Section 8.3 */

#include "gsl_eigen__qrstep.c"

gsl_eigen_hermv_workspace * 
gsl_eigen_hermv_alloc (const size_t n)
{
  gsl_eigen_hermv_workspace * w ;

  if (n == 0)
    {
      GSL_ERROR_NULL ("matrix dimension must be positive integer", GSL_EINVAL);
    }
  
  w = (gsl_eigen_hermv_workspace *) malloc (sizeof(gsl_eigen_hermv_workspace));

  if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace", GSL_ENOMEM);
    }

  w->d = (double *) malloc (n * sizeof (double));

  if (w->d == 0)
    {
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for diagonal", GSL_ENOMEM);
    }

  w->sd = (double *) malloc (n * sizeof (double));

  if (w->sd == 0)
    {
      free (w->d);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for subdiagonal", GSL_ENOMEM);
    }

  w->tau = (double *) malloc (2 * n * sizeof (double));

  if (w->tau == 0)
    {
      free (w->sd);
      free (w->d);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for tau", GSL_ENOMEM);
    }

  w->gc = (double *) malloc (n * sizeof (double));

  if (w->gc == 0)
    {
      free (w->tau);
      free (w->sd);
      free (w->d);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for cosines", GSL_ENOMEM);
    }

  w->gs = (double *) malloc (n * sizeof (double));

  if (w->gs == 0)
    {
      free (w->gc);
      free (w->tau);
      free (w->sd);
      free (w->d);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for sines", GSL_ENOMEM);
    }

  w->size = n;

  return w;
}

void
gsl_eigen_hermv_free (gsl_eigen_hermv_workspace * w)
{
  free (w->gs);
  free (w->gc);
  free (w->tau);
  free (w->sd);
  free (w->d);
  free (w);
}

int
gsl_eigen_hermv (gsl_matrix_complex * A, gsl_vector * eval, 
                       gsl_matrix_complex * evec,
                       gsl_eigen_hermv_workspace * w)
{
  if (A->size1 != A->size2)
    {
      GSL_ERROR ("matrix must be square to compute eigenvalues", GSL_ENOTSQR);
    }
  else if (eval->size != A->size1)
    {
      GSL_ERROR ("eigenvalue vector must match matrix size", GSL_EBADLEN);
    }
  else if (evec->size1 != A->size1 || evec->size2 != A->size1)
    {
      GSL_ERROR ("eigenvector matrix must match matrix size", GSL_EBADLEN);
    }
  else
    {
      const size_t N = A->size1;
      double *const d = w->d;
      double *const sd = w->sd;

      size_t a, b;

      /* handle special case */

      if (N == 1)
        {
          gsl_complex A00 = gsl_matrix_complex_get (A, 0, 0);
          gsl_vector_set (eval, 0, GSL_REAL(A00));
          gsl_matrix_complex_set (evec, 0, 0, GSL_COMPLEX_ONE);
          return GSL_SUCCESS;
        }

      /* Transform the matrix into a symmetric tridiagonal form */

      {
        gsl_vector_view d_vec = gsl_vector_view_array (d, N);
        gsl_vector_view sd_vec = gsl_vector_view_array (sd, N - 1);
        gsl_vector_complex_view tau_vec = gsl_vector_complex_view_array (w->tau, N-1);
        gsl_linalg_hermtd_decomp (A, &tau_vec.vector);
        gsl_linalg_hermtd_unpack (A, &tau_vec.vector, evec, &d_vec.vector, &sd_vec.vector);
      }

      /* Make an initial pass through the tridiagonal decomposition
         to remove off-diagonal elements which are effectively zero */
      
      chop_small_elements (N, d, sd);
      
      /* Progressively reduce the matrix until it is diagonal */
      
      b = N - 1;
      
      while (b > 0)
        {
          if (sd[b - 1] == 0.0 || isnan(sd[b - 1]))
            {
              b--;
              continue;
            }
          
          /* Find the largest unreduced block (a,b) starting from b
             and working backwards */
          
          a = b - 1;
          
          while (a > 0)
            {
              if (sd[a - 1] == 0.0)
                {
                  break;
                }
              a--;
            }
          
          {
            size_t i;
            const size_t n_block = b - a + 1;
            double *d_block = d + a;
            double *sd_block = sd + a;
            double * const gc = w->gc;
            double * const gs = w->gs;
            
            /* apply QR reduction with implicit deflation to the
               unreduced block */
            
            qrstep (n_block, d_block, sd_block, gc, gs);
            
            /* Apply  Givens rotation Gij(c,s) to matrix Q,  Q <- Q G */
            
            for (i = 0; i < n_block - 1; i++)
              {
                const double c = gc[i], s = gs[i];
                size_t k;
                
                for (k = 0; k < N; k++)
                  {
                    gsl_complex qki = gsl_matrix_complex_get (evec, k, a + i);
                    gsl_complex qkj = gsl_matrix_complex_get (evec, k, a + i + 1);
                    /* qki <= qki * c - qkj * s */
                    /* qkj <= qki * s + qkj * c */
                    gsl_complex x1 = gsl_complex_mul_real(qki, c);
                    gsl_complex y1 = gsl_complex_mul_real(qkj, -s);
                    
                    gsl_complex x2 = gsl_complex_mul_real(qki, s);
                    gsl_complex y2 = gsl_complex_mul_real(qkj, c);
                    
                    gsl_complex qqki = gsl_complex_add(x1, y1);
                    gsl_complex qqkj = gsl_complex_add(x2, y2);
                    
                    gsl_matrix_complex_set (evec, k, a + i, qqki);
                    gsl_matrix_complex_set (evec, k, a + i + 1, qqkj);
                  }
              }
            
            /* remove any small off-diagonal elements */
            
            chop_small_elements (n_block, d_block, sd_block);
          }
        }
      
      {
        gsl_vector_view d_vec = gsl_vector_view_array (d, N);
        gsl_vector_memcpy (eval, &d_vec.vector);
      }
      
      return GSL_SUCCESS;
    }
}
