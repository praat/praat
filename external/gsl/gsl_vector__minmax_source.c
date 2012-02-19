/* vector/minmax_source.c
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

BASE 
FUNCTION(gsl_vector,max) (const TYPE(gsl_vector) * v)
{
  /* finds the largest element of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  BASE max = v->data[0 * stride];
  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x > max)
        max = x;
#ifdef FP
      if (isnan (x))
        return x;
#endif
    }

  return max;
}

BASE 
FUNCTION(gsl_vector,min) (const TYPE(gsl_vector) * v)
{
  /* finds the smallest element of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  BASE min = v->data[0 * stride];
  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x < min)
        min = x;
#ifdef FP
      if (isnan (x))
        return x;
#endif
    }

  return min;
}

void
FUNCTION(gsl_vector,minmax) (const TYPE(gsl_vector) * v,
                             BASE * min_out, 
                             BASE * max_out)
{
  /* finds the smallest and largest elements of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  BASE max = v->data[0 * stride];
  BASE min = v->data[0 * stride];

  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x < min)
        {
          min = x;
        }
      if (x > max)
        {
          max = x;
        }
#ifdef FP
      if (isnan (x))
        {
          min = x;
          max = x;
          break;
        }
#endif
    }

  *min_out = min;
  *max_out = max;
}


size_t 
FUNCTION(gsl_vector,max_index) (const TYPE(gsl_vector) * v)
{
  /* finds the largest element of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  BASE max = v->data[0 * stride];
  size_t imax = 0;
  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x > max)
        {
          max = x;
          imax = i;
        }
#ifdef FP
      if (isnan (x))
        {
          return i;
        }
#endif
    }

  return imax;
}

size_t 
FUNCTION(gsl_vector,min_index) (const TYPE(gsl_vector) * v)
{
  /* finds the smallest element of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  BASE min = v->data[0 * stride];
  size_t imin = 0;
  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x < min)
        {
          min = x;
          imin = i;
        }
#ifdef FP
      if (isnan (x))
        {
          return i;
        }
#endif
    }

  return imin;
}


void
FUNCTION(gsl_vector,minmax_index) (const TYPE(gsl_vector) * v,
                                   size_t * imin_out, 
                                   size_t * imax_out)
{
  /* finds the smallest and largest elements of a vector */

  const size_t N = v->size ;
  const size_t stride = v->stride ;

  size_t imin = 0, imax = 0;
  BASE max = v->data[0 * stride];
  BASE min = v->data[0 * stride];

  size_t i;

  for (i = 0; i < N; i++)
    {
      BASE x = v->data[i*stride];
      if (x < min)
        {
          min = x;
          imin = i;
        }
      if (x > max)
        {
          max = x;
          imax = i;
        }
#ifdef FP
      if (isnan (x))
        {
          imin = i;
          imax = i;
          break;
        }
#endif
    }

  *imin_out = imin;
  *imax_out = imax;
}


