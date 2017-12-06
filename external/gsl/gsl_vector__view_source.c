/* vector/view_source.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2007 Gerard Jungman, Brian Gough
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

QUALIFIED_VIEW(_gsl_vector,view)
FUNCTION(gsl_vector, view_array) (QUALIFIER ATOMIC * base, size_t n)
{
  QUALIFIED_VIEW(_gsl_vector,view) view = NULL_VECTOR_VIEW;

  if (n == 0)
    {
      GSL_ERROR_VAL ("vector length n must be positive integer", 
                     GSL_EINVAL, view);
    }

  {
    TYPE(gsl_vector) v = NULL_VECTOR;

    v.data = (ATOMIC *)base  ;
    v.size = n;
    v.stride = 1;
    v.block = 0;
    v.owner = 0;

    view.vector = v;
    return view;
  }
}

QUALIFIED_VIEW(_gsl_vector,view)
FUNCTION(gsl_vector, view_array_with_stride) (QUALIFIER ATOMIC * base, 
                                              size_t stride,
                                              size_t n)
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

  {
    TYPE(gsl_vector) v = NULL_VECTOR;
    
    v.data = (ATOMIC *)base ;
    v.size = n;
    v.stride = stride;
    v.block = 0;
    v.owner = 0;

    view.vector = v;
    return view;
  }
}
