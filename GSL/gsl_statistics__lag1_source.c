/* statistics/lag1_source.c
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
FUNCTION(gsl_stats,lag1_autocorrelation) (const BASE data[], const size_t stride, const size_t n)
{
  const double mean = FUNCTION(gsl_stats,mean) (data, stride, n);
  return FUNCTION(gsl_stats,lag1_autocorrelation_m)(data, stride, n, mean);
}

double 
FUNCTION(gsl_stats,lag1_autocorrelation_m) (const BASE data[], const size_t stride, const size_t size, const double mean)
{
  /* Compute the lag-1 autocorrelation of a dataset using the
     recurrence relation */

  size_t i;

  long double r1 ;
  long double q = 0 ;
  long double v = (data[0 * stride] - mean) * (data[0 * stride] - mean) ;

  for (i = 1; i < size ; i++)
    {
      const long double delta0 = (data[(i-1) * stride] - mean);
      const long double delta1 = (data[i * stride] - mean);
      q += (delta0 * delta1 - q)/(i + 1);
      v += (delta1 * delta1 - v)/(i + 1);
    }

  r1 = q / v ;

  return r1;
}
