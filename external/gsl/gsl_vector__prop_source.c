/* vector/prop_source.c
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

int
FUNCTION (gsl_vector, isnull) (const TYPE (gsl_vector) * v)
{
  const size_t n = v->size;
  const size_t stride = v->stride ;
  
  size_t j;

  for (j = 0; j < n; j++)
    {
      size_t k;
      
      for (k = 0; k < MULTIPLICITY; k++) 
        {
          if (v->data[MULTIPLICITY * stride * j + k] != 0.0)
            {
              return 0;
            }
        }
    }

  return 1;
}

int
FUNCTION (gsl_vector, ispos) (const TYPE (gsl_vector) * v)
{
  const size_t n = v->size;
  const size_t stride = v->stride ;
  
  size_t j;

  for (j = 0; j < n; j++)
    {
      size_t k;
      
      for (k = 0; k < MULTIPLICITY; k++) 
        {
          if (v->data[MULTIPLICITY * stride * j + k] <= 0.0)
            {
              return 0;
            }
        }
    }

  return 1;
}

int
FUNCTION (gsl_vector, isneg) (const TYPE (gsl_vector) * v)
{
  const size_t n = v->size;
  const size_t stride = v->stride ;
  
  size_t j;

  for (j = 0; j < n; j++)
    {
      size_t k;
      
      for (k = 0; k < MULTIPLICITY; k++) 
        {
          if (v->data[MULTIPLICITY * stride * j + k] >= 0.0)
            {
              return 0;
            }
        }
    }

  return 1;
}

int
FUNCTION (gsl_vector, isnonneg) (const TYPE (gsl_vector) * v)
{
  const size_t n = v->size;
  const size_t stride = v->stride ;
  
  size_t j;

  for (j = 0; j < n; j++)
    {
      size_t k;
      
      for (k = 0; k < MULTIPLICITY; k++) 
        {
          if (v->data[MULTIPLICITY * stride * j + k] < 0.0)
            {
              return 0;
            }
        }
    }

  return 1;
}

