/*
 * Implement Heap sort -- direct and indirect sorting
 * Based on descriptions in Sedgewick "Algorithms in C"
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

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_heapsort.h"

static inline void downheap (size_t * p, const void *data, const size_t size, const size_t N, size_t k, gsl_comparison_fn_t compare);

#define CMP(data,size,j,k) (compare((const char *)(data) + (size) * (j), (const char *)(data) + (size) * (k)))

static inline void
downheap (size_t * p, const void *data, const size_t size, const size_t N, size_t k, gsl_comparison_fn_t compare)
{
  const size_t pki = p[k];

  while (k <= N / 2)
    {
      size_t j = 2 * k;

      if (j < N && CMP (data, size, p[j], p[j + 1]) < 0)
        {
          j++;
        }

      if (CMP (data, size, pki, p[j]) >= 0)
        {
          break;
        }

      p[k] = p[j];

      k = j;
    }

  p[k] = pki;
}

int
gsl_heapsort_index (size_t * p, const void *data, size_t count, size_t size, gsl_comparison_fn_t compare)
{
  /* Sort the array in ascending order. This is a true inplace
     algorithm with N log N operations. Worst case (an already sorted
     array) is something like 20% slower */

  size_t i, k, N;

  if (count == 0)
    {
      return GSL_SUCCESS;       /* No data to sort */
    }

  for (i = 0; i < count; i++)
    {
      p[i] = i ;                /* set permutation to identity */
    }

  /* We have n_data elements, last element is at 'n_data-1', first at
     '0' Set N to the last element number. */

  N = count - 1;

  k = N / 2;
  k++;                          /* Compensate the first use of 'k--' */
  do
    {
      k--;
      downheap (p, data, size, N, k, compare);
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

      downheap (p, data, size, N, 0, compare);
    }

  return GSL_SUCCESS;
}
