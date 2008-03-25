/* statistics/wvariance_source.c
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

static double 
FUNCTION(compute,wvariance) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean);

static double
FUNCTION(compute,factor) (const BASE w[], const size_t wstride, const size_t n);

static double
FUNCTION(compute,wvariance) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  /* takes a dataset and finds the weighted variance */

  long double wvariance = 0 ;
  long double W = 0;

  size_t i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      BASE wi = w[i * wstride];

      if (wi > 0) {
        const long double delta = (data[i * stride] - wmean);
        W += wi ;
        wvariance += (delta * delta - wvariance) * (wi / W);
      }
    }

  return wvariance ;
}

static double
FUNCTION(compute,factor) (const BASE w[], const size_t wstride, const size_t n)
{
  /* Find the factor ``N/(N-1)'' which multiplies the raw std dev */

  long double a = 0 ;
  long double b = 0;
  long double factor;

  size_t i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      BASE wi = w[i * wstride];

      if (wi > 0)
        {
          a += wi ;
          b += wi * wi ;
        }
    }

  factor = (a*a) / ((a*a) - b);

  return factor ;
}

double 
FUNCTION(gsl_stats,wvariance_with_fixed_mean) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  const double wvariance = FUNCTION(compute,wvariance) (w, wstride, data, stride, n, wmean);
  return wvariance;
}

double 
FUNCTION(gsl_stats,wsd_with_fixed_mean) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  const double wvariance = FUNCTION(compute,wvariance) (w, wstride, data, stride, n, wmean);
  const double wsd = sqrt (wvariance);

  return wsd;
}


double 
FUNCTION(gsl_stats,wvariance_m) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  const double variance = FUNCTION(compute,wvariance) (w, wstride, data, stride, n, wmean);
  const double scale = FUNCTION(compute,factor)(w, wstride, n);
  
  return scale * variance;
}

double 
FUNCTION(gsl_stats,wsd_m) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n, const double wmean)
{
  const double variance = FUNCTION(compute,wvariance) (w, wstride, data, stride, n, wmean);
  const double scale = FUNCTION(compute,factor)(w, wstride, n);
  const double wsd = sqrt(scale * variance) ;
  
  return wsd;
}

double 
FUNCTION(gsl_stats,wsd) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n)
{
  const double wmean = FUNCTION(gsl_stats,wmean) (w, wstride, data, stride, n);
  return FUNCTION(gsl_stats,wsd_m) (w, wstride, data, stride, n, wmean) ;
}

double 
FUNCTION(gsl_stats,wvariance) (const BASE w[], const size_t wstride, const BASE data[], const size_t stride, const size_t n)
{
  const double wmean = FUNCTION(gsl_stats,wmean) (w, wstride, data, stride, n);
  return FUNCTION(gsl_stats,wvariance_m)(w, wstride, data, stride, n, wmean);
}
