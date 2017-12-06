/* block/init_source.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

TYPE (gsl_block) *
FUNCTION (gsl_block, alloc) (const size_t n)
{
  TYPE (gsl_block) * b;

  if (n == 0)
    {
      GSL_ERROR_VAL ("block length n must be positive integer",
                        GSL_EINVAL, 0);
    }

  b = (TYPE (gsl_block) *) malloc (sizeof (TYPE (gsl_block)));

  if (b == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for block struct",
                        GSL_ENOMEM, 0);
    }

  b->data = (ATOMIC *) malloc (MULTIPLICITY * n * sizeof (ATOMIC));

  if (b->data == 0)
    {
      free (b);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for block data",
                        GSL_ENOMEM, 0);
    }

  b->size = n;

  return b;
}

TYPE (gsl_block) *
FUNCTION (gsl_block, calloc) (const size_t n)
{
  size_t i;

  TYPE (gsl_block) * b = FUNCTION (gsl_block, alloc) (n);

  if (b == 0)
    return 0;

  /* initialize block to zero */

  for (i = 0; i < MULTIPLICITY * n; i++)
    {
      b->data[i] = 0;
    }

  return b;
}

void
FUNCTION (gsl_block, free) (TYPE (gsl_block) * b)
{
  free (b->data);
  free (b);
}
