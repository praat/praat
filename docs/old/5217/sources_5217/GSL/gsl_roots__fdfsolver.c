/* roots/fdfsolver.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
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
#include "gsl_roots.h"

gsl_root_fdfsolver *
gsl_root_fdfsolver_alloc (const gsl_root_fdfsolver_type * T)
{

  gsl_root_fdfsolver * s = (gsl_root_fdfsolver *) malloc (sizeof (gsl_root_fdfsolver));

  if (s == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for root solver struct",
                        GSL_ENOMEM, 0);
    };

  s->state = malloc (T->size);

  if (s->state == 0)
    {
      free (s);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for root solver state",
                        GSL_ENOMEM, 0);
    };

  s->type = T ;
  s->fdf = NULL;

  return s;
}

int
gsl_root_fdfsolver_set (gsl_root_fdfsolver * s, gsl_function_fdf * f, double root)
{
  s->fdf = f;
  s->root = root;

  return (s->type->set) (s->state, s->fdf, &(s->root));
}

int
gsl_root_fdfsolver_iterate (gsl_root_fdfsolver * s)
{
  return (s->type->iterate) (s->state, s->fdf, &(s->root));
}

void
gsl_root_fdfsolver_free (gsl_root_fdfsolver * s)
{
  free (s->state);
  free (s);
}

const char *
gsl_root_fdfsolver_name (const gsl_root_fdfsolver * s)
{
  return s->type->name;
}

double
gsl_root_fdfsolver_root (const gsl_root_fdfsolver * s)
{
  return s->root;
}


