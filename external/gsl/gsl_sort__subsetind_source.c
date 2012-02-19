/* sort/subsetind_source.c  
 * 
 * Copyright (C) 1999,2000,2001  Thomas Walter, Brian Gough
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option) any
 * later version.
 *
 * This source is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

/* find the k-th smallest elements of the vector data, in ascending order */

int
FUNCTION (gsl_sort, smallest_index) (size_t * p, const size_t k,
                                     const BASE * src, const size_t stride,
                                     const size_t n)
{
  size_t i, j;
  BASE xbound;

  if (k > n)
    {
      GSL_ERROR ("subset length k exceeds vector length n", GSL_EINVAL);
    }

  if (k == 0 || n == 0)
    {
      return GSL_SUCCESS;
    }

  /* take the first element */

  j = 1;
  xbound = src[0 * stride];
  p[0] = 0;

  /* examine the remaining elements */

  for (i = 1; i < n; i++)
    {
      size_t i1;

      BASE xi = src[i * stride];

      if (j < k)
        {
          j++;
        }
      else if (xi >= xbound)
        {
          continue;
        }

      for (i1 = j - 1; i1 > 0 ; i1--)
        {
          if (xi > src[p[i1 - 1] * stride])
            break;

          p[i1] = p[i1 - 1];
        }

      p[i1] = i;

      xbound = src[p[j-1] * stride];
    }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_sort_vector,smallest_index) (size_t * p, const size_t k, 
                                           const TYPE (gsl_vector) * v)
{
  return FUNCTION (gsl_sort, smallest_index) (p, k, v->data, v->stride, v->size);
}

int
FUNCTION (gsl_sort, largest_index) (size_t * p, const size_t k,
                                    const BASE * src, const size_t stride,
                                    const size_t n)
{
  size_t i, j;
  BASE xbound;

  if (k > n)
    {
      GSL_ERROR ("subset length k exceeds vector length n", GSL_EINVAL);
    }

  if (k == 0 || n == 0)
    {
      return GSL_SUCCESS;
    }

  /* take the first element */

  j = 1;
  xbound = src[0 * stride];
  p[0] = 0;

  /* examine the remaining elements */

  for (i = 1; i < n; i++)
    {
      size_t i1;

      BASE xi = src[i * stride];

      if (j < k)
        {
          j++;
        }
      else if (xi <= xbound)
        {
          continue;
        }

      for (i1 = j - 1; i1 > 0 ; i1--)
        {
          if (xi < src[stride * p[i1 - 1]])
            break;

          p[i1] = p[i1 - 1];
        }

      p[i1] = i;

      xbound = src[stride * p[j-1]];
    }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_sort_vector,largest_index) (size_t * p, const size_t k, 
                                          const TYPE (gsl_vector) * v)
{
  return FUNCTION (gsl_sort, largest_index) (p, k, v->data, v->stride, v->size);
}
