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

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_heapsort.h"

static inline void swap (void *base, size_t size, size_t i, size_t j);
static inline void downheap (void *data, const size_t size, const size_t N, size_t k, gsl_comparison_fn_t compare);

/* Inline swap function for moving objects around */

static inline void
swap (void *base, size_t size, size_t i, size_t j)
{
  register char *a = size * i + (char *) base;
  register char *b = size * j + (char *) base;
  register size_t s = size;

  if (i == j)
    return;

  do
    {
      char tmp = *a;
      *a++ = *b;
      *b++ = tmp;
    }
  while (--s > 0);
}

#define CMP(data,size,j,k) (compare((char *)(data) + (size) * (j), (char *)(data) + (size) * (k)))

static inline void
downheap (void *data, const size_t size, const size_t N, size_t k, gsl_comparison_fn_t compare)
{
  while (k <= N / 2)
    {
      size_t j = 2 * k;

      if (j < N && CMP (data, size, j, j + 1) < 0)
        {
          j++;
        }

      if (CMP (data, size, k, j) < 0)
        {
          swap (data, size, j, k);
        }
      else
        {
          break;
        }

      k = j;
    }
}

void
gsl_heapsort (void *data, size_t count, size_t size, gsl_comparison_fn_t compare)
{
  /* Sort the array in ascending order. This is a true inplace
     algorithm with N log N operations. Worst case (an already sorted
     array) is something like 20% slower */

  size_t N;
  size_t k;

  if (count == 0)
    {
      return;                   /* No data to sort */
    }

  /* We have n_data elements, last element is at 'n_data-1', first at
     '0' Set N to the last element number. */

  N = count - 1;

  k = N / 2;
  k++;                          /* Compensate the first use of 'k--' */
  do
    {
      k--;
      downheap (data, size, N, k, compare);
    }
  while (k > 0);

  while (N > 0)
    {
      /* first swap the elements */
      swap (data, size, 0, N);

      /* then process the heap */
      N--;

      downheap (data, size, N, 0, compare);
    }
}
