/* vector/oper_source.c
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

int 
FUNCTION(gsl_vector, add) (TYPE(gsl_vector) * a, const TYPE(gsl_vector) * b)
{
  const size_t N = a->size;

  if (b->size != N)
    {
      GSL_ERROR ("vectors must have same length", GSL_EBADLEN);
    }
  else 
    {
      const size_t stride_a = a->stride;
      const size_t stride_b = b->stride;

      size_t i;

      for (i = 0; i < N; i++)
        {
          a->data[i * stride_a] += b->data[i * stride_b];
        }
      
      return GSL_SUCCESS;
    }
}

int 
FUNCTION(gsl_vector, sub) (TYPE(gsl_vector) * a, const TYPE(gsl_vector) * b)
{
  const size_t N = a->size;

  if (b->size != N)
    {
      GSL_ERROR ("vectors must have same length", GSL_EBADLEN);
    }
  else 
    {
      const size_t stride_a = a->stride;
      const size_t stride_b = b->stride;

      size_t i;

      for (i = 0; i < N; i++)
        {
          a->data[i * stride_a] -= b->data[i * stride_b];
        }
      
      return GSL_SUCCESS;
    }
}

int 
FUNCTION(gsl_vector, mul) (TYPE(gsl_vector) * a, const TYPE(gsl_vector) * b)
{
  const size_t N = a->size;

  if (b->size != N)
    {
      GSL_ERROR ("vectors must have same length", GSL_EBADLEN);
    }
  else 
    {
      const size_t stride_a = a->stride;
      const size_t stride_b = b->stride;

      size_t i;

      for (i = 0; i < N; i++)
        {
          a->data[i * stride_a] *= b->data[i * stride_b];
        }
      
      return GSL_SUCCESS;
    }
}

int 
FUNCTION(gsl_vector, div) (TYPE(gsl_vector) * a, const TYPE(gsl_vector) * b)
{
  const size_t N = a->size;

  if (b->size != N)
    {
      GSL_ERROR ("vectors must have same length", GSL_EBADLEN);
    }
  else 
    {
      const size_t stride_a = a->stride;
      const size_t stride_b = b->stride;

      size_t i;

      for (i = 0; i < N; i++)
        {
          a->data[i * stride_a] /= b->data[i * stride_b];
        }
      
      return GSL_SUCCESS;
    }
}

int 
FUNCTION(gsl_vector, scale) (TYPE(gsl_vector) * a, const double x)
{
  const size_t N = a->size;
  const size_t stride = a->stride;
  
  size_t i;
  
  for (i = 0; i < N; i++)
    {
      a->data[i * stride] *= x;
    }
  
  return GSL_SUCCESS;
}

int 
FUNCTION(gsl_vector, add_constant) (TYPE(gsl_vector) * a, const double x)
{
  const size_t N = a->size;
  const size_t stride = a->stride;
  
  size_t i;
  
  for (i = 0; i < N; i++)
    {
      a->data[i * stride] += x;
    }
  
  return GSL_SUCCESS;
}
