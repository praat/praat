/* permutation/permutation.c
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
#include "gsl_errno.h"
#include "gsl_permutation.h"

size_t
gsl_permutation_size (const gsl_permutation * p)
{
  return p->size ;
}

size_t *
gsl_permutation_data (const gsl_permutation * p)
{
  return p->data ;
}

#ifndef HIDE_INLINE_STATIC
size_t
gsl_permutation_get (const gsl_permutation * p, const size_t i)
{
  if (gsl_check_range)
    {
      if (i >= p->size)         /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_VAL ("index out of range", GSL_EINVAL, 0);
        }
    }

  return p->data[i];
}
#endif

int
gsl_permutation_swap (gsl_permutation * p, const size_t i, const size_t j)
{
  const size_t size = p->size ;
  
  if (i >= size)
    {
      GSL_ERROR("first index is out of range", GSL_EINVAL);
    }

  if (j >= size)
    {
      GSL_ERROR("second index is out of range", GSL_EINVAL);
    }

  if (i != j)
    {
      size_t tmp = p->data[i];
      p->data[i] = p->data[j];
      p->data[j] = tmp;
    }
  
  return GSL_SUCCESS;
}


int
gsl_permutation_valid (gsl_permutation * p)
{
  const size_t size = p->size ;

  size_t i, j ;

  for (i = 0; i < size; i++) 
    {
      if (p->data[i] >= size)
        {
          GSL_ERROR("permutation index outside range", GSL_FAILURE) ;
        }

      for (j = 0; j < i; j++)
        {
          if (p->data[i] == p->data[j])
            {
              GSL_ERROR("duplicate permutation index", GSL_FAILURE) ;
            }
        }
    }
  
  return GSL_SUCCESS;
}

void
gsl_permutation_reverse (gsl_permutation * p)
{
  const size_t size = p->size ;

  size_t i ;
  
  for (i = 0; i < (size / 2); i++) 
    {
      size_t j = size - i - 1;

      size_t tmp = p->data[i] ;
      p->data[i] = p->data[j] ;
      p->data[j] = tmp ;
    }
}

int 
gsl_permutation_inverse (gsl_permutation * inv, const gsl_permutation * p)
{
  const size_t size = p->size ;

  size_t i ;

  if (inv->size != size)
    {
      GSL_ERROR("permutation lengths are not equal", GSL_EBADLEN);
    }
  
  for (i = 0; i < size; i++) 
    {
      inv->data[p->data[i]] = i ;
    }
  
  return GSL_SUCCESS ;
}

int
gsl_permutation_next (gsl_permutation * p)
{
  /* Replaces p with the next permutation (in the standard lexicographical
   * ordering).  Returns GSL_FAILURE if there is no next permutation.
   */
  const size_t size = p->size;
  size_t i, j, k;

  if (size < 2)
    {
      return GSL_FAILURE;
    }

  i = size - 2;

  while ((p->data[i] > p->data[i+1]) && (i != 0))
    {
      i--;
    }

  if ((i == 0) && (p->data[0] > p->data[1]))
    {
     return GSL_FAILURE;
    }

  k = i + 1;

  for (j = i + 2; j < size; j++ )
    {
      if ((p->data[j] > p->data[i]) && (p->data[j] < p->data[k]))
        {
          k = j;
        }
    }

  /* swap i and k */

  {
    size_t tmp = p->data[i];
    p->data[i] = p->data[k];
    p->data[k] = tmp;
  }

  for (j = i + 1; j <= ((size + i) / 2); j++)
    {
      size_t tmp = p->data[j];
      p->data[j] = p->data[size + i - j];
      p->data[size + i - j] = tmp;
    }

  return GSL_SUCCESS;
}

int
gsl_permutation_prev (gsl_permutation * p)
{
  const size_t size = p->size;
  size_t i, j, k;

  if (size < 2)
    {
      return GSL_FAILURE;
    }

  i = size - 2;

  while ((p->data[i] < p->data[i+1]) && (i != 0))
    {
      i--;
    }

  if ((i == 0) && (p->data[0] < p->data[1]))
    {
      return GSL_FAILURE;
    }

  k = i + 1;

  for (j = i + 2; j < size; j++ )
    {
      if ((p->data[j] < p->data[i]) && (p->data[j] > p->data[k]))
        {
          k = j;
        }
    }

  /* swap i and k */

  {
    size_t tmp = p->data[i];
    p->data[i] = p->data[k];
    p->data[k] = tmp;
  }

  for (j = i + 1; j <= ((size + i) / 2); j++)
    {
      size_t tmp = p->data[j];
      p->data[j] = p->data[size + i - j];
      p->data[size + i - j] = tmp;
    }

  return GSL_SUCCESS;
}

int
gsl_permutation_mul (gsl_permutation * p, const gsl_permutation * pa, const gsl_permutation * pb)
{
  size_t i;
  const size_t size = p->size;

  if (pa->size != size)
    {
      GSL_ERROR("size of result does not match size of pa", GSL_EINVAL);
    }

  if (pb->size != size)
    {
      GSL_ERROR("size of result does not match size of pb", GSL_EINVAL);
    }

  for (i = 0; i < size; i++)
    {
      p->data[i] = pb->data[pa->data[i]];
    }

  return GSL_SUCCESS;
}
int
gsl_permutation_memcpy (gsl_permutation * dest,
                        const gsl_permutation * src)
{
  const size_t src_size = src->size;
  const size_t dest_size = dest->size;

  if (src_size != dest_size)
    {
      GSL_ERROR ("permutation lengths are not equal", GSL_EBADLEN);
    }

  {
    size_t j;

    for (j = 0; j < src_size; j++)
      {
        dest->data[j] = src->data[j];
      }
  }

  return GSL_SUCCESS;
}
