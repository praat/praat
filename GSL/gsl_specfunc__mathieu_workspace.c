/* specfunc/mathieu_workspace.c
 * 
 * Copyright (C) 2003 Lowell Johnson
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
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_mathieu.h"


gsl_sf_mathieu_workspace *gsl_sf_mathieu_alloc(const size_t nn,
                                               const double qq)
{
  gsl_sf_mathieu_workspace *workspace;
  unsigned int even_order = nn/2 + 1, odd_order = (nn + 1)/2,
               extra_values;

  /* Compute the maximum number of extra terms required for 10^-18 root
     accuracy for a given value of q (contributed by Brian Gladman). */
  extra_values = (int)(2.1*pow(fabs(qq), 0.37)) + 9;
  
  if (nn + 1 == 0)
  {
      GSL_ERROR_NULL("matrix dimension must be positive integer", GSL_EINVAL);
  }

  workspace =
         (gsl_sf_mathieu_workspace *)malloc(sizeof(gsl_sf_mathieu_workspace));
  if (workspace == NULL)
  {
      GSL_ERROR_NULL("failed to allocate space for workspace", GSL_ENOMEM);
  }

  /* Extend matrices to ensure accuracy. */
  even_order += extra_values;
  odd_order += extra_values;
  
  workspace->size = nn;
  workspace->even_order = even_order;
  workspace->odd_order = odd_order;
  workspace->extra_values = extra_values;

  /* Allocate space for the characteristic values. */
  workspace->aa = (double *)malloc((nn+1)*sizeof(double));
  if (workspace->aa == NULL)
  {
      free(workspace);
      GSL_ERROR_NULL("Error allocating memory for characteristic a values",
                     GSL_ENOMEM);
  }

  workspace->bb = (double *)malloc((nn+1)*sizeof(double));
  if (workspace->bb == NULL)
  {
      free(workspace->aa);
      free(workspace);
      GSL_ERROR_NULL("Error allocating memory for characteristic b values",
                     GSL_ENOMEM);
  }

  /* Since even_order is always >= odd_order, dimension the arrays for
     even_order. */
  
  workspace->dd = (double *)malloc(even_order*sizeof(double));
  if (workspace->dd == NULL)
  {
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for diagonal", GSL_ENOMEM);
  }

  workspace->ee = (double *)malloc(even_order*sizeof(double));
  if (workspace->ee == NULL)
  {
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for diagonal", GSL_ENOMEM);
  }

  workspace->tt = (double *)malloc(3*even_order*sizeof(double));
  if (workspace->tt == NULL)
  {
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for diagonal", GSL_ENOMEM);
  }

  workspace->e2 = (double *)malloc(even_order*sizeof(double));
  if (workspace->e2 == NULL)
  {
      free(workspace->tt);
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for diagonal", GSL_ENOMEM);
  }

  workspace->zz = (double *)malloc(even_order*even_order*sizeof(double));
  if (workspace->zz == NULL)
  {
      free(workspace->e2);
      free(workspace->tt);
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for diagonal", GSL_ENOMEM);
  }
  
  workspace->eval = gsl_vector_alloc(even_order);

  if (workspace->eval == NULL)
    {
      free(workspace->zz);
      free(workspace->e2);
      free(workspace->tt);
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for eval", GSL_ENOMEM);
    }

  workspace->evec = gsl_matrix_alloc(even_order, even_order);

  if (workspace->evec == NULL)
    {
      gsl_vector_free (workspace->eval);
      free(workspace->zz);
      free(workspace->e2);
      free(workspace->tt);
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for evec", GSL_ENOMEM);
    }

  workspace->wmat = gsl_eigen_symmv_alloc(even_order);

  if (workspace->wmat == NULL)
    {
      gsl_matrix_free (workspace->evec);
      gsl_vector_free (workspace->eval);
      free(workspace->zz);
      free(workspace->e2);
      free(workspace->tt);
      free(workspace->ee);
      free(workspace->dd);
      free(workspace->aa);
      free(workspace->bb);
      free(workspace);
      GSL_ERROR_NULL("failed to allocate space for wmat", GSL_ENOMEM);
    }
  
  return workspace;
}


void gsl_sf_mathieu_free(gsl_sf_mathieu_workspace *workspace)
{
  gsl_vector_free(workspace->eval);
  gsl_matrix_free(workspace->evec);
  gsl_eigen_symmv_free(workspace->wmat);
  free(workspace->aa);
  free(workspace->bb);
  free(workspace->dd);
  free(workspace->ee);
  free(workspace->tt);
  free(workspace->e2);
  free(workspace->zz);
  free(workspace);
}
