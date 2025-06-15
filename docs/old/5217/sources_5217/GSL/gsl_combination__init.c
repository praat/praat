/* combination/init.c
 * based on permutation/init.c by Brian Gough
 * 
 * Copyright (C) 2001 Szymon Jaroszewicz
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
#include "gsl_combination.h"

gsl_combination *
gsl_combination_alloc (const size_t n, const size_t k)
{
  gsl_combination * c;

  if (n == 0)
    {
      GSL_ERROR_VAL ("combination parameter n must be positive integer",
                        GSL_EDOM, 0);
    }
  if (k > n)
    {
      GSL_ERROR_VAL ("combination length k must be an integer less than or equal to n",
                        GSL_EDOM, 0);
    }
  c = (gsl_combination *) malloc (sizeof (gsl_combination));

  if (c == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for combination struct",
                        GSL_ENOMEM, 0);
    }

  if (k > 0)
    {
      c->data = (size_t *) malloc (k * sizeof (size_t));

      if (c->data == 0)
        {
          free (c);             /* exception in constructor, avoid memory leak */

          GSL_ERROR_VAL ("failed to allocate space for combination data",
                         GSL_ENOMEM, 0);
        }
    }
  else
    {
      c->data = 0;
    }

  c->n = n;
  c->k = k;

  return c;
}

gsl_combination *
gsl_combination_calloc (const size_t n, const size_t k)
{
  size_t i;

  gsl_combination * c =  gsl_combination_alloc (n, k);

  if (c == 0)
    return 0;

  /* initialize combination to identity */

  for (i = 0; i < k; i++)
    {
      c->data[i] = i;
    }

  return c;
}

void
gsl_combination_init_first (gsl_combination * c)
{
  const size_t k = c->k ;
  size_t i;

  /* initialize combination to identity */

  for (i = 0; i < k; i++)
    {
      c->data[i] = i;
    }
}

void
gsl_combination_init_last (gsl_combination * c)
{
  const size_t k = c->k ;
  size_t i;
  size_t n = c->n;

  /* initialize combination to identity */

  for (i = 0; i < k; i++)
    {
      c->data[i] = n - k + i;
    }
}

void
gsl_combination_free (gsl_combination * c)
{
  if (c->k > 0) free (c->data);
  free (c);
}
