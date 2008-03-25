/* vector/init_source.c
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

TYPE (gsl_vector) *
FUNCTION (gsl_vector, alloc) (const size_t n)
{
  TYPE (gsl_block) * block;
  TYPE (gsl_vector) * v;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer",
                        GSL_EINVAL, 0);
    }

  v = (TYPE (gsl_vector) *) malloc (sizeof (TYPE (gsl_vector)));

  if (v == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for vector struct",
                        GSL_ENOMEM, 0);
    }

  block = FUNCTION (gsl_block,alloc) (n);

  if (block == 0)
    {
      free (v) ;

      GSL_ERROR_VAL ("failed to allocate space for block",
                        GSL_ENOMEM, 0);
    }
      
  v->data = block->data ;
  v->size = n;
  v->stride = 1;
  v->block = block;
  v->owner = 1;

  return v;
}

TYPE (gsl_vector) *
FUNCTION (gsl_vector, calloc) (const size_t n)
{
  size_t i;

  TYPE (gsl_vector) * v = FUNCTION (gsl_vector, alloc) (n);

  if (v == 0)
    return 0;

  /* initialize vector to zero */

  for (i = 0; i < MULTIPLICITY * n; i++)
    {
      v->data[i] = 0;
    }

  return v;
}

TYPE (gsl_vector) *
FUNCTION (gsl_vector, alloc_from_block) (TYPE(gsl_block) * block, 
                                         const size_t offset, 
                                         const size_t n, 
                                         const size_t stride)
{
  TYPE (gsl_vector) * v;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer",
                        GSL_EINVAL, 0);
    }

  if (stride == 0)
    {
      GSL_ERROR_VAL ("stride must be positive integer", GSL_EINVAL, 0);
    }

  if (block->size <= offset + (n - 1) * stride)
    {
      GSL_ERROR_VAL ("vector would extend past end of block", GSL_EINVAL, 0);
    }

  v = (TYPE (gsl_vector) *) malloc (sizeof (TYPE (gsl_vector)));

  if (v == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for vector struct",
                        GSL_ENOMEM, 0);
    }

  v->data = block->data + MULTIPLICITY * offset ;
  v->size = n;
  v->stride = stride;
  v->block = block;
  v->owner = 0;

  return v;
}

TYPE (gsl_vector) *
FUNCTION (gsl_vector, alloc_from_vector) (TYPE(gsl_vector) * w, 
                                         const size_t offset, 
                                         const size_t n, 
                                         const size_t stride)
{
  TYPE (gsl_vector) * v;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer",
                        GSL_EINVAL, 0);
    }

  if (stride == 0)
    {
      GSL_ERROR_VAL ("stride must be positive integer", GSL_EINVAL, 0);
    }

  if (offset + (n - 1) * stride >= w->size)
    {
      GSL_ERROR_VAL ("vector would extend past end of block", GSL_EINVAL, 0);
    }

  v = (TYPE (gsl_vector) *) malloc (sizeof (TYPE (gsl_vector)));

  if (v == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for vector struct",
                        GSL_ENOMEM, 0);
    }

  v->data = w->data + MULTIPLICITY * w->stride * offset ;
  v->size = n;
  v->stride = stride * w->stride;
  v->block = w->block;
  v->owner = 0;

  return v;
}


void
FUNCTION (gsl_vector, free) (TYPE (gsl_vector) * v)
{
  if (v->owner)
    {
      FUNCTION(gsl_block, free) (v->block) ;
    }
  free (v);
}


void
FUNCTION (gsl_vector, set_all) (TYPE (gsl_vector) * v, BASE x)
{
  ATOMIC * const data = v->data;
  const size_t n = v->size;
  const size_t stride = v->stride;

  size_t i;

  for (i = 0; i < n; i++)
    {
      *(BASE *) (data + MULTIPLICITY * i * stride) = x;
    }
}

void
FUNCTION (gsl_vector, set_zero) (TYPE (gsl_vector) * v)
{
  ATOMIC * const data = v->data;
  const size_t n = v->size;
  const size_t stride = v->stride;
  const BASE zero = ZERO ;

  size_t i;

  for (i = 0; i < n; i++)
    {
      *(BASE *) (data + MULTIPLICITY * i * stride) = zero;
    }
}

int
FUNCTION (gsl_vector, set_basis) (TYPE (gsl_vector) * v, size_t i)
{
  ATOMIC * const data = v->data;
  const size_t n = v->size;
  const size_t stride = v->stride;
  const BASE zero = ZERO ;
  const BASE one = ONE;

  size_t k;

  if (i >= n)
    {
      GSL_ERROR ("index out of range", GSL_EINVAL);
    }

  for (k = 0; k < n; k++)
    {
      *(BASE *) (data + MULTIPLICITY * k * stride) = zero;
    }

  *(BASE *) (data + MULTIPLICITY * i * stride) = one;

  return GSL_SUCCESS;
}
