/* histogram/find.c
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

/* determines whether to optimize for linear ranges  */
#define LINEAR_OPT 1

static int find (const size_t n, const double range[], 
                 const double x, size_t * i);

static int
find (const size_t n, const double range[], const double x, size_t * i)
{
  size_t i_linear, lower, upper, mid;

  if (x < range[0])
    {
      return -1;
    }

  if (x >= range[n])
    {
      return +1;
    }

  /* optimize for linear case */

#ifdef LINEAR_OPT
  {
    double u =  (x - range[0]) / (range[n] - range[0]);
    i_linear = (size_t) (u * n);
  }

  if (x >= range[i_linear] && x < range[i_linear + 1])
    {
      *i = i_linear;
      return 0;
    }
#endif

  /* perform binary search */

  upper = n ;
  lower = 0 ;

  while (upper - lower > 1)
    {
      mid = (upper + lower) / 2 ;
      
      if (x >= range[mid])
        {
          lower = mid ;
        }
      else
        {
          upper = mid ;
        }
    }

  *i = lower ;

  /* sanity check the result */

  if (x < range[lower] || x >= range[lower + 1])
    {
      GSL_ERROR ("x not found in range", GSL_ESANITY);
    }

  return 0;
}

