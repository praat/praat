/*
 * Implement Heap sort -- direct and indirect sorting
 * Based on descriptions in Sedgewick "Algorithms in C"
 *
 * Copyright (C) 1999  Thomas Walter
 *
 * 18 February 2000: Modified for GSL by Brian Gough
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

static inline void FUNCTION (index, downheap) (size_t * p, const BASE * data, const size_t stride, const size_t N, size_t k);

static inline void
FUNCTION (index, downheap) (size_t * p, const BASE * data, const size_t stride, const size_t N, size_t k)
{
  const size_t pki = p[k];

  while (k <= N / 2)
    {
      size_t j = 2 * k;

      if (j < N && data[p[j] * stride] < data[p[j + 1] * stride])
        {
          j++;
        }

      if (!(data[pki * stride] < data[p[j] * stride])) /* avoid infinite loop if nan */
        {
          break;
        }

      p[k] = p[j];

      k = j;
    }

  p[k] = pki;
}

void
FUNCTION (gsl_sort, index) (size_t * p, const BASE * data, const size_t stride, const size_t n)
{
  size_t N;
  size_t i, k;

  if (n == 0)
    {
      return;   /* No data to sort */
    }

  /* set permutation to identity */

  for (i = 0 ; i < n ; i++)
    {
      p[i] = i ;
    }

  /* We have n_data elements, last element is at 'n_data-1', first at
     '0' Set N to the last element number. */

  N = n - 1;

  k = N / 2;
  k++;                          /* Compensate the first use of 'k--' */
  do
    {
      k--;
      FUNCTION (index, downheap) (p, data, stride, N, k);
    }
  while (k > 0);

  while (N > 0)
    {
      /* first swap the elements */
      size_t tmp = p[0];
      p[0] = p[N];
      p[N] = tmp;

      /* then process the heap */
      N--;

      FUNCTION (index, downheap) (p, data, stride, N, 0);
    }
}

int
FUNCTION (gsl_sort_vector, index) (gsl_permutation * permutation, const TYPE (gsl_vector) * v)
{
  if (permutation->size != v->size)
    {
      GSL_ERROR ("permutation and vector lengths are not equal", GSL_EBADLEN);
    }

  FUNCTION (gsl_sort, index) (permutation->data, v->data, v->stride, v->size) ;
  
  return GSL_SUCCESS ;
}
