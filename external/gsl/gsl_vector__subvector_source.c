/* vector/subvector_source.c
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

QUALIFIED_VIEW(_gsl_vector, view)
FUNCTION(gsl_vector, subvector) (QUALIFIED_TYPE(gsl_vector) * v, size_t offset, size_t n)
{
  QUALIFIED_VIEW(_gsl_vector,view) view = NULL_VECTOR_VIEW;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer", 
                     GSL_EINVAL, view);
    }

  if (offset + (n - 1) >= v->size)
    {
      GSL_ERROR_VAL ("view would extend past end of vector", 
                     GSL_EINVAL, view);
    }

  {
    TYPE(gsl_vector) s = NULL_VECTOR;

    s.data = v->data +  MULTIPLICITY * v->stride * offset ;
    s.size = n;
    s.stride = v->stride;
    s.block = v->block;
    s.owner = 0;

    view.vector = s;
    return view;
  }
}

QUALIFIED_VIEW(_gsl_vector, view)
FUNCTION(gsl_vector, subvector_with_stride) (QUALIFIED_TYPE(gsl_vector) * v, size_t offset, size_t stride, size_t n)
{
  QUALIFIED_VIEW(_gsl_vector,view) view = NULL_VECTOR_VIEW;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer", 
                     GSL_EINVAL, view);
    }

  if (stride == 0)
    {
      GSL_ERROR_VAL ("stride must be positive integer", 
                     GSL_EINVAL, view);
    }

  if (offset + (n - 1) * stride >= v->size)
    {
      GSL_ERROR_VAL ("view would extend past end of vector", 
                     GSL_EINVAL, view);
    }

  {
    TYPE(gsl_vector) s = NULL_VECTOR;

    s.data = v->data + MULTIPLICITY * v->stride * offset ;
    s.size = n;
    s.stride = v->stride * stride;
    s.block = v->block;
    s.owner = 0;
    
    view.vector = s;
    return view;
  }
}
