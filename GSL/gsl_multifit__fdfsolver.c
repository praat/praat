/* multifit/fdfsolver.c
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
#include <stdlib.h>
#include <string.h>
#include "gsl_errno.h"
#include "gsl_multifit_nlin.h"

gsl_multifit_fdfsolver *
gsl_multifit_fdfsolver_alloc (const gsl_multifit_fdfsolver_type * T, 
                              size_t n, size_t p)
{
  int status;

  gsl_multifit_fdfsolver * s;

  if (n < p)
    {
      GSL_ERROR_VAL ("insufficient data points, n < p", GSL_EINVAL, 0);
    }

  s = (gsl_multifit_fdfsolver *) malloc (sizeof (gsl_multifit_fdfsolver));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for multifit solver struct",
                     GSL_ENOMEM, 0);
    }

  s->x = gsl_vector_calloc (p);

  if (s->x == 0) 
    {
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for x", GSL_ENOMEM, 0);
    }

  s->f = gsl_vector_calloc (n);

  if (s->f == 0) 
    {
      gsl_vector_free (s->x);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for f", GSL_ENOMEM, 0);
    }

  s->J = gsl_matrix_calloc (n,p);

  if (s->J == 0) 
    {
      gsl_vector_free (s->x);
      gsl_vector_free (s->f);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for g", GSL_ENOMEM, 0);
    }

  s->dx = gsl_vector_calloc (p);

  if (s->dx == 0) 
    {
      gsl_matrix_free (s->J);
      gsl_vector_free (s->x);
      gsl_vector_free (s->f);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for dx", GSL_ENOMEM, 0);
    }

  s->state = malloc (T->size);

  if (s->state == 0)
    {
      gsl_vector_free (s->dx);
      gsl_vector_free (s->x);
      gsl_vector_free (s->f);
      gsl_matrix_free (s->J);
      free (s);         /* exception in constructor, avoid memory leak */
      
      GSL_ERROR_VAL ("failed to allocate space for multifit solver state",
                        GSL_ENOMEM, 0);
    }

  s->type = T ;

  status = (s->type->alloc)(s->state, n, p);

  if (status != GSL_SUCCESS)
    {
      free (s->state);
      gsl_vector_free (s->dx);
      gsl_vector_free (s->x);
      gsl_vector_free (s->f);
      gsl_matrix_free (s->J);
      free (s);         /* exception in constructor, avoid memory leak */
      
      GSL_ERROR_VAL ("failed to set solver", status, 0);
    }

  s->fdf = NULL;

  return s;
}

int
gsl_multifit_fdfsolver_set (gsl_multifit_fdfsolver * s, 
                             gsl_multifit_function_fdf * f, 
                             const gsl_vector * x)
{
  if (s->f->size != f->n)
    {
      GSL_ERROR ("function size does not match solver", GSL_EBADLEN);
    }

  if (s->x->size != x->size)
    {
      GSL_ERROR ("vector length does not match solver", GSL_EBADLEN);
    }  

  s->fdf = f;
  gsl_vector_memcpy(s->x,x);
  
  return (s->type->set) (s->state, s->fdf, s->x, s->f, s->J, s->dx);
}

int
gsl_multifit_fdfsolver_iterate (gsl_multifit_fdfsolver * s)
{
  return (s->type->iterate) (s->state, s->fdf, s->x, s->f, s->J, s->dx);
}

void
gsl_multifit_fdfsolver_free (gsl_multifit_fdfsolver * s)
{
  (s->type->free) (s->state);
  free (s->state);
  gsl_vector_free (s->dx);
  gsl_vector_free (s->x);
  gsl_vector_free (s->f);
  gsl_matrix_free (s->J);
  free (s);
}

const char *
gsl_multifit_fdfsolver_name (const gsl_multifit_fdfsolver * s)
{
  return s->type->name;
}

gsl_vector *
gsl_multifit_fdfsolver_position (const gsl_multifit_fdfsolver * s)
{
  return s->x;
}

