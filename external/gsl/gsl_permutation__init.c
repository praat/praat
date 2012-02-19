/* permutation/init.c
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
#include "gsl_errno.h"
#include "gsl_permutation.h"

gsl_permutation *
gsl_permutation_alloc (const size_t n)
{
  gsl_permutation * p;

  if (n == 0)
    {
      GSL_ERROR_VAL ("permutation length n must be positive integer",
                        GSL_EDOM, 0);
    }

  p = (gsl_permutation *) malloc (sizeof (gsl_permutation));

  if (p == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for permutation struct",
                        GSL_ENOMEM, 0);
    }

  p->data = (size_t *) malloc (n * sizeof (size_t));

  if (p->data == 0)
    {
      free (p);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for permutation data",
                        GSL_ENOMEM, 0);
    }

  p->size = n;

  return p;
}

gsl_permutation *
gsl_permutation_calloc (const size_t n)
{
  size_t i;

  gsl_permutation * p =  gsl_permutation_alloc (n);

  if (p == 0)
    return 0;

  /* initialize permutation to identity */

  for (i = 0; i < n; i++)
    {
      p->data[i] = i;
    }

  return p;
}

void
gsl_permutation_init (gsl_permutation * p)
{
  const size_t n = p->size ;
  size_t i;

  /* initialize permutation to identity */

  for (i = 0; i < n; i++)
    {
      p->data[i] = i;
    }
}

void
gsl_permutation_free (gsl_permutation * p)
{
  free (p->data);
  free (p);
}
