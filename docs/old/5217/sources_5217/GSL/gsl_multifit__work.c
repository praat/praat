/* multifit/work.c
 * 
 * Copyright (C) 2000, 2007 Brian Gough
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
#include "gsl_errno.h"
#include "gsl_multifit.h"

gsl_multifit_linear_workspace *
gsl_multifit_linear_alloc (size_t n, size_t p)
{
  gsl_multifit_linear_workspace *w;

  w = (gsl_multifit_linear_workspace *)
    malloc (sizeof (gsl_multifit_linear_workspace));

  if (w == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for multifit_linear struct",
                     GSL_ENOMEM, 0);
    }

  w->n = n;                     /* number of observations */
  w->p = p;                     /* number of parameters */

  w->A = gsl_matrix_alloc (n, p);

  if (w->A == 0)
    {
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for A", GSL_ENOMEM, 0);
    }

  w->Q = gsl_matrix_alloc (p, p);

  if (w->Q == 0)
    {
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for Q", GSL_ENOMEM, 0);
    }

  w->QSI = gsl_matrix_alloc (p, p);

  if (w->QSI == 0)
    {
      gsl_matrix_free (w->Q);
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for QSI", GSL_ENOMEM, 0);
    }

  w->S = gsl_vector_alloc (p);

  if (w->S == 0)
    {
      gsl_matrix_free (w->QSI);
      gsl_matrix_free (w->Q);
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for S", GSL_ENOMEM, 0);
    }

  w->t = gsl_vector_alloc (n);

  if (w->t == 0)
    {
      gsl_vector_free (w->S);
      gsl_matrix_free (w->QSI);
      gsl_matrix_free (w->Q);
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for t", GSL_ENOMEM, 0);
    }

  w->xt = gsl_vector_calloc (p);

  if (w->xt == 0)
    {
      gsl_vector_free (w->t);
      gsl_vector_free (w->S);
      gsl_matrix_free (w->QSI);
      gsl_matrix_free (w->Q);
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for xt", GSL_ENOMEM, 0);
    }

  w->D = gsl_vector_calloc (p);

  if (w->D == 0)
    {
      gsl_vector_free (w->D);
      gsl_vector_free (w->t);
      gsl_vector_free (w->S);
      gsl_matrix_free (w->QSI);
      gsl_matrix_free (w->Q);
      gsl_matrix_free (w->A);
      free (w);
      GSL_ERROR_VAL ("failed to allocate space for xt", GSL_ENOMEM, 0);
    }

  return w;
}

void
gsl_multifit_linear_free (gsl_multifit_linear_workspace * work)
{
  gsl_matrix_free (work->A);
  gsl_matrix_free (work->Q);
  gsl_matrix_free (work->QSI);
  gsl_vector_free (work->S);
  gsl_vector_free (work->t);
  gsl_vector_free (work->xt);
  gsl_vector_free (work->D);
  free (work);
}

