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

static inline void FUNCTION (my, downheap) (BASE * data, const size_t stride, const size_t N, size_t k);

static inline void
FUNCTION (my, downheap) (BASE * data, const size_t stride, const size_t N, size_t k)
{
  BASE v = data[k * stride];

  while (k <= N / 2)
    {
      size_t j = 2 * k;

      if (j < N && data[j * stride] < data[(j + 1) * stride])
        {
          j++;
        }

      if (!(v < data[j * stride]))  /* avoid infinite loop if nan */
        {
          break;
        }

      data[k * stride] = data[j * stride];

      k = j;
    }

  data[k * stride] = v;
}

void
TYPE (gsl_sort) (BASE * data, const size_t stride, const size_t n)
{
  size_t N;
  size_t k;

  if (n == 0)
    {
      return;                   /* No data to sort */
    }

  /* We have n_data elements, last element is at 'n_data-1', first at
     '0' Set N to the last element number. */

  N = n - 1;

  k = N / 2;
  k++;                          /* Compensate the first use of 'k--' */
  do
    {
      k--;
      FUNCTION (my, downheap) (data, stride, N, k);
    }
  while (k > 0);

  while (N > 0)
    {
      /* first swap the elements */
      BASE tmp = data[0 * stride];
      data[0 * stride] = data[N * stride];
      data[N * stride] = tmp;

      /* then process the heap */
      N--;

      FUNCTION (my, downheap) (data, stride, N, 0);
    }
}

void
TYPE (gsl_sort_vector) (TYPE (gsl_vector) * v)
{
  TYPE (gsl_sort) (v->data, v->stride, v->size) ;
}

