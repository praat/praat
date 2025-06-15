/* statistics/wabsdev_source.c
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
FUNCTION(gsl_stats,wabsdev) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n)
{
  const double wmean = FUNCTION(gsl_stats,wmean)(w, wstride, data, stride, n);
  return FUNCTION(gsl_stats,wabsdev_m)(w, wstride, data, stride, n, wmean);
}
    
double 
FUNCTION(gsl_stats,wabsdev_m) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  /* Compute the weighted absolute deviation of a dataset */

  long double wabsdev = 0;
  long double W = 0;

  size_t i;

  /* find the sum of the absolute deviations */
  for (i = 0; i < n; i++)
    {
      BASE wi = w[i * wstride];
      
      if (wi > 0) {
        const long double delta = fabs(data[i * stride] - wmean);
        W += wi ;
        wabsdev += (delta - wabsdev) * (wi / W);
      }
    }

  return wabsdev;
}

