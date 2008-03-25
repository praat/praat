/* multiroots/fdjac.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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
#include "gsl_multiroots.h"

int
gsl_multiroot_fdjacobian (gsl_multiroot_function * F,
                           const gsl_vector * x, const gsl_vector * f,
                           double epsrel, gsl_matrix * jacobian)
{
  const size_t n = x->size;
  const size_t m = f->size;
  const size_t n1 = jacobian->size1;
  const size_t n2 = jacobian->size2;
  int status = 0;

  if (m != n1 || n != n2)
    {
      GSL_ERROR ("function and jacobian are not conformant", GSL_EBADLEN);
    }

  {
    size_t i,j;
    gsl_vector *x1, *f1;

    x1 = gsl_vector_alloc (n);

    if (x1 == 0)
      {
        GSL_ERROR ("failed to allocate space for x1 workspace", GSL_ENOMEM);
      }

    f1 = gsl_vector_alloc (m);

    if (f1 == 0)
      {
        gsl_vector_free (x1);

        GSL_ERROR ("failed to allocate space for f1 workspace", GSL_ENOMEM);
      }

    gsl_vector_memcpy (x1, x);  /* copy x into x1 */

    for (j = 0; j < n; j++)
      {
        double xj = gsl_vector_get (x, j);
        double dx = epsrel * fabs (xj);

        if (dx == 0)
          {
            dx = epsrel;
          }

        gsl_vector_set (x1, j, xj + dx);
        
        {
          int f_stat = GSL_MULTIROOT_FN_EVAL (F, x1, f1);

          if (f_stat != GSL_SUCCESS) 
            {
              status = GSL_EBADFUNC;
              break; /* n.b. avoid memory leak for x1,f1 */
            }
        }

        gsl_vector_set (x1, j, xj);

        for (i = 0; i < m; i++)
          {
            double g1 = gsl_vector_get (f1, i);
            double g0 = gsl_vector_get (f, i);
            gsl_matrix_set (jacobian, i, j, (g1 - g0) / dx);
          }

        {
          gsl_vector_view col = gsl_matrix_column (jacobian, j);
          int null_col = gsl_vector_isnull (&col.vector);
          /* if column is null, return an error - this may be due to
             dx being too small. Try increasing epsrel */
          if (null_col) {
            status = GSL_ESING;
          }
        }
      }

    gsl_vector_free (x1);
    gsl_vector_free (f1);
  }

  if (status)
    return status;
  else
    return GSL_SUCCESS;
}
