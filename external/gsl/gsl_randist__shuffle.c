/* randist/shuffle.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
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
#include <math.h>
#include "gsl_rng.h"
#include "gsl_randist.h"

/* Inline swap and copy functions for moving objects around */

static inline 
void swap (void * base, size_t size, size_t i, size_t j)
{
  register char * a = size * i + (char *) base ;
  register char * b = size * j + (char *) base ;
  register size_t s = size ;

  if (i == j)
    return ;
  
  do                                            
    {                                           
      char tmp = *a;                            
      *a++ = *b;                                
      *b++ = tmp;                               
    } 
  while (--s > 0);                              
}

static inline void 
copy (void * dest, size_t i, void * src, size_t j, size_t size)
{
  register char * a = size * i + (char *) dest ;
  register char * b = size * j + (char *) src ;
  register size_t s = size ;
  
  do                                            
    {                                           
      *a++ = *b++;                              
    } 
  while (--s > 0);                              
}

/* Randomly permute (shuffle) N indices

   Supply an array x[N] with nmemb members, each of size size and on
   return it will be shuffled into a random order.  The algorithm is
   from Knuth, SemiNumerical Algorithms, v2, p139, who cites Moses and
   Oakford, and Durstenfeld */

void
gsl_ran_shuffle (const gsl_rng * r, void * base, size_t n, size_t size)
{
  size_t i ;

  for (i = n - 1; i > 0; i--)
    {
      size_t j = gsl_rng_uniform_int(r, i+1); /* originally (i + 1) * gsl_rng_uniform (r) */

      swap (base, size, i, j) ;
    }
}

int
gsl_ran_choose (const gsl_rng * r, void * dest, size_t k, void * src, 
                 size_t n, size_t size)
{
  size_t i, j = 0;

  /* Choose k out of n items, return an array x[] of the k items.
     These items will prevserve the relative order of the original
     input -- you can use shuffle() to randomize the output if you
     wish */

  if (k > n)
    {
      GSL_ERROR ("k is greater than n, cannot sample more than n items",
                 GSL_EINVAL) ;
    }

  for (i = 0; i < n && j < k; i++)
    {
      if ((n - i) * gsl_rng_uniform (r) < k - j)
        {
          copy (dest, j, src, i, size) ;
          j++ ;
        }
    }

  return GSL_SUCCESS;
}

void
gsl_ran_sample (const gsl_rng * r, void * dest, size_t k, void * src, 
                size_t n, size_t size)
{
  size_t i, j = 0;

  /* Choose k out of n items, with replacement */

  for (i = 0; i < k; i++)
    {
      j = gsl_rng_uniform_int (r, n);  /* originally n * gsl_rng_uniform (r) */

      copy (dest, i, src, j, size) ;
    }
}
