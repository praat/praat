/* multiroots/fdfsolver.c
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
#include "gsl_multiroots.h"

gsl_multiroot_fdfsolver *
gsl_multiroot_fdfsolver_alloc (const gsl_multiroot_fdfsolver_type * T, 
                                       size_t n)
{
  int status;

  gsl_multiroot_fdfsolver * s;

  s = (gsl_multiroot_fdfsolver *) malloc (sizeof (gsl_multiroot_fdfsolver));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for multiroot solver struct",
                        GSL_ENOMEM, 0);
    }

  s->x = gsl_vector_calloc (n);

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

  s->J = gsl_matrix_calloc (n,n);

  if (s->J == 0) 
    {
      gsl_vector_free (s->x);
      gsl_vector_free (s->f);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for g", GSL_ENOMEM, 0);
    }

  s->dx = gsl_vector_calloc (n);

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
      
      GSL_ERROR_VAL ("failed to allocate space for multiroot solver state",
                        GSL_ENOMEM, 0);
    }

  s->type = T ;

  status = (s->type->alloc)(s->state, n);

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
gsl_multiroot_fdfsolver_set (gsl_multiroot_fdfsolver * s, 
                             gsl_multiroot_function_fdf * f, 
                             const gsl_vector * x)
{
  if (s->x->size != f->n)
    {
      GSL_ERROR ("function incompatible with solver size", GSL_EBADLEN);
    }
  
  if (x->size != f->n) 
    {
      GSL_ERROR ("vector length not compatible with function", GSL_EBADLEN);
    }  
    
  s->fdf = f;
  gsl_vector_memcpy(s->x,x);
  
  return (s->type->set) (s->state, s->fdf, s->x, s->f, s->J, s->dx);
}

int
gsl_multiroot_fdfsolver_iterate (gsl_multiroot_fdfsolver * s)
{
  return (s->type->iterate) (s->state, s->fdf, s->x, s->f, s->J, s->dx);
}

void
gsl_multiroot_fdfsolver_free (gsl_multiroot_fdfsolver * s)
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
gsl_multiroot_fdfsolver_name (const gsl_multiroot_fdfsolver * s)
{
  return s->type->name;
}

gsl_vector *
gsl_multiroot_fdfsolver_root (const gsl_multiroot_fdfsolver * s)
{
  return s->x;
}

gsl_vector *
gsl_multiroot_fdfsolver_dx (const gsl_multiroot_fdfsolver * s)
{
  return s->dx;
}

gsl_vector *
gsl_multiroot_fdfsolver_f (const gsl_multiroot_fdfsolver * s)
{
  return s->f;
}
