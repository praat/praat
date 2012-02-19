/* roots/fsolver.c
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

gsl_root_fsolver *
gsl_root_fsolver_alloc (const gsl_root_fsolver_type * T)
{
  gsl_root_fsolver * s = (gsl_root_fsolver *) malloc (sizeof (gsl_root_fsolver));

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
  s->function = NULL ;

  return s;
}

int
gsl_root_fsolver_set (gsl_root_fsolver * s, gsl_function * f, double x_lower, double x_upper)
{
  if (x_lower > x_upper)
    {
      GSL_ERROR ("invalid interval (lower > upper)", GSL_EINVAL);
    }

  s->function = f;
  s->root = 0.5 * (x_lower + x_upper);  /* initial estimate */
  s->x_lower = x_lower;
  s->x_upper = x_upper;

  return (s->type->set) (s->state, s->function, &(s->root), x_lower, x_upper);
}

int
gsl_root_fsolver_iterate (gsl_root_fsolver * s)
{
  return (s->type->iterate) (s->state, 
                             s->function, &(s->root), 
                             &(s->x_lower), &(s->x_upper));
}

void
gsl_root_fsolver_free (gsl_root_fsolver * s)
{
  free (s->state);
  free (s);
}

const char *
gsl_root_fsolver_name (const gsl_root_fsolver * s)
{
  return s->type->name;
}

double
gsl_root_fsolver_root (const gsl_root_fsolver * s)
{
  return s->root;
}

double
gsl_root_fsolver_x_lower (const gsl_root_fsolver * s)
{
  return s->x_lower;
}

double
gsl_root_fsolver_x_upper (const gsl_root_fsolver * s)
{
  return s->x_upper;
}

