/* multimin/fdfminimizer.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Fabrice Rossi
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
#include "gsl_multimin.h"

gsl_multimin_fdfminimizer *
gsl_multimin_fdfminimizer_alloc (const gsl_multimin_fdfminimizer_type * T,
                                 size_t n)
{
  int status;

  gsl_multimin_fdfminimizer *s =
    (gsl_multimin_fdfminimizer *) malloc (sizeof (gsl_multimin_fdfminimizer));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for minimizer struct",
                     GSL_ENOMEM, 0);
    }

  s->type = T;

  s->x = gsl_vector_calloc (n);

  if (s->x == 0) 
    {
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for x", GSL_ENOMEM, 0);
    }

  s->gradient = gsl_vector_calloc (n);

  if (s->gradient == 0) 
    {
      gsl_vector_free (s->x);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for gradient", GSL_ENOMEM, 0);
    }

  s->dx = gsl_vector_calloc (n);

  if (s->dx == 0) 
    {
      gsl_vector_free (s->x);
      gsl_vector_free (s->gradient);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for dx", GSL_ENOMEM, 0);
    }

  s->state = malloc (T->size);

  if (s->state == 0)
    {
      gsl_vector_free (s->x);
      gsl_vector_free (s->gradient);
      gsl_vector_free (s->dx);
      free (s);
      GSL_ERROR_VAL ("failed to allocate space for minimizer state",
                     GSL_ENOMEM, 0);
    }

  status = (T->alloc) (s->state, n);

  if (status != GSL_SUCCESS)
    {
      free (s->state);
      gsl_vector_free (s->x);
      gsl_vector_free (s->gradient);
      gsl_vector_free (s->dx);
      free (s);

      GSL_ERROR_VAL ("failed to initialize minimizer state", GSL_ENOMEM, 0);
    }

  return s;
}

int
gsl_multimin_fdfminimizer_set (gsl_multimin_fdfminimizer * s,
                               gsl_multimin_function_fdf * fdf,
                               const gsl_vector * x,
                               double step_size, double tol)
{
  if (s->x->size != fdf->n)
    {
      GSL_ERROR ("function incompatible with solver size", GSL_EBADLEN);
    }
  
  if (x->size != fdf->n) 
    {
      GSL_ERROR ("vector length not compatible with function", GSL_EBADLEN);
    }  
    
  s->fdf = fdf;

  gsl_vector_memcpy (s->x,x);
  gsl_vector_set_zero (s->dx);
  
  return (s->type->set) (s->state, s->fdf, s->x, &(s->f), s->gradient, step_size, tol);
}

void
gsl_multimin_fdfminimizer_free (gsl_multimin_fdfminimizer * s)
{
  (s->type->free) (s->state);
  free (s->state);
  gsl_vector_free (s->dx);
  gsl_vector_free (s->gradient);
  gsl_vector_free (s->x);
  free (s);
}

int
gsl_multimin_fdfminimizer_iterate (gsl_multimin_fdfminimizer * s)
{
  return (s->type->iterate) (s->state, s->fdf, s->x, &(s->f), s->gradient, s->dx);
}

int
gsl_multimin_fdfminimizer_restart (gsl_multimin_fdfminimizer * s)
{
  return (s->type->restart) (s->state);
}

const char * 
gsl_multimin_fdfminimizer_name (const gsl_multimin_fdfminimizer * s)
{
  return s->type->name;
}


gsl_vector * 
gsl_multimin_fdfminimizer_x (gsl_multimin_fdfminimizer * s)
{
  return s->x;
}

gsl_vector * 
gsl_multimin_fdfminimizer_dx (gsl_multimin_fdfminimizer * s)
{
  return s->dx;
}

gsl_vector * 
gsl_multimin_fdfminimizer_gradient (gsl_multimin_fdfminimizer * s)
{
  return s->gradient;
}

double 
gsl_multimin_fdfminimizer_minimum (gsl_multimin_fdfminimizer * s)
{
  return s->f;
}

