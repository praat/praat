/* statistics/p_variance_source.c
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
FUNCTION(gsl_stats,pvariance) (const BASE data1[], 
                               const size_t stride1, const size_t n1, 
                               const BASE data2[], 
                               const size_t stride2, const size_t n2)
{
  /* Find the pooled variance of two datasets */

  const double var1 = FUNCTION(gsl_stats,variance) (data1, stride1, n1);
  const double var2 = FUNCTION(gsl_stats,variance) (data2, stride2, n2);

  /* calculate the pooled variance */

  const double pooled_variance = 
    (((n1 - 1) * var1) + ((n2 - 1) * var2)) / (n1 + n2 - 2);

  return pooled_variance;
}

