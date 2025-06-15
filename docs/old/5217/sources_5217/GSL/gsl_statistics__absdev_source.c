/* statistics/absdev_source.c
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
FUNCTION(gsl_stats,absdev) (const BASE data[], const size_t stride, const size_t n)
{
  const double mean = FUNCTION(gsl_stats,mean)(data, stride, n);
  return FUNCTION(gsl_stats,absdev_m)(data, stride, n, mean);
}
    
double 
FUNCTION(gsl_stats,absdev_m) (const BASE data[], 
                              const size_t stride,
                              const size_t n, 
                              const double mean)
{
  /* takes a dataset and finds the absolute deviation */

  double sum = 0, absdev;
  size_t i;

  /* find the sum of the absolute deviations */
  for (i = 0; i < n; i++)
    {
      const double delta = fabs(data[i * stride] - mean);
      sum += delta;
    }

  absdev = sum / n;

  return absdev;
}

