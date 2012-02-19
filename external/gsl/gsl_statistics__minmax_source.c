/* statistics/minmax_source.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Jim Davies, Brian Gough
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
FUNCTION (gsl_stats,max) (const BASE data[], const size_t stride,
                          const size_t n)
{
  /* finds the largest member of a dataset */

  BASE max = data[0 * stride];
  size_t i;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi > max)
        max = xi;
#ifdef FP
      if (isnan (xi))
        return xi;
#endif
    }

  return max;
}

BASE
FUNCTION (gsl_stats,min) (const BASE data[], const size_t stride,
                          const size_t n)
{
  /* finds the smallest member of a dataset */

  BASE min = data[0 * stride];
  size_t i;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi < min)
        min = xi;
#ifdef FP
      if (isnan (xi))
        return xi;
#endif
    }

  return min;

}

void
FUNCTION (gsl_stats,minmax) (BASE * min_out, BASE * max_out,
                             const BASE data[], const size_t stride,
                             const size_t n)
{
  /* finds the smallest and largest members of a dataset */

  BASE min = data[0 * stride];
  BASE max = data[0 * stride];
  size_t i;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi < min)
        min = xi;

      if (xi > max)
        max = xi;

#ifdef FP
      if (isnan (xi))
        {
          min = xi;
          max = xi;
          break;
        }
#endif

    }

  *min_out = min;
  *max_out = max;
}

size_t
FUNCTION (gsl_stats,max_index) (const BASE data[], const size_t stride,
                                const size_t n)
{
  /* finds the index of the largest member of a dataset */
  /* if there is more than one largest value then we choose the first */

  BASE max = data[0 * stride];
  size_t i, max_index = 0;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi > max)
        {
          max = xi;
          max_index = i;
        }

#ifdef FP
      if (isnan (xi))
        {
          return i;
        }
#endif
    }

  return max_index;
}

size_t
FUNCTION (gsl_stats,min_index) (const BASE data[], const size_t stride,
                                const size_t n)
{
  /* finds the index of the smallest member of a dataset */
  /* if there is more than one largest value then we choose the first  */

  BASE min = data[0 * stride];
  size_t i, min_index = 0;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi < min)
        {
          min = xi;
          min_index = i;
        }

#ifdef FP
      if (isnan (xi))
        {
          return i;
        }
#endif
    }

  return min_index;
}

void
FUNCTION (gsl_stats,minmax_index) (size_t * min_index_out,
                                   size_t * max_index_out, const BASE data[],
                                   const size_t stride, const size_t n)
{
  /* finds the smallest and largest members of a dataset */

  BASE min = data[0 * stride];
  BASE max = data[0 * stride];
  size_t i, min_index = 0, max_index = 0;

  for (i = 0; i < n; i++)
    {
      BASE xi = data[i * stride];

      if (xi < min)
        {
          min = xi;
          min_index = i;
        }

      if (xi > max)
        {
          max = xi;
          max_index = i;
        }

#ifdef FP
      if (isnan (xi))
        {
          min_index = i;
          max_index = i;
          break;
        }
#endif
    }

  *min_index_out = min_index;
  *max_index_out = max_index;
}
