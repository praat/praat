/* statistics/median_source.c
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


double
FUNCTION(gsl_stats,median_from_sorted_data) (const BASE sorted_data[],
                                             const size_t stride,
                                             const size_t n)
{
  double median ;
  const size_t lhs = (n - 1) / 2 ;
  const size_t rhs = n / 2 ;
  
  if (n == 0)
    return 0.0 ;

  if (lhs == rhs)
    {
      median = sorted_data[lhs * stride] ;
    }
  else 
    {
      median = (sorted_data[lhs * stride] + sorted_data[rhs * stride])/2.0 ;
    }

  return median ;
}

