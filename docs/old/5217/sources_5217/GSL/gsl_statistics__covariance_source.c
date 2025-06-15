/* statistics/covar_source.c
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
FUNCTION(compute,covariance) (const BASE data1[], const size_t stride1,
                              const BASE data2[], const size_t stride2,
                              const size_t n, 
                              const double mean1, const double mean2);

static double
FUNCTION(compute,covariance) (const BASE data1[], const size_t stride1,
                              const BASE data2[], const size_t stride2,
                              const size_t n, 
                              const double mean1, const double mean2)
{
  /* takes a dataset and finds the covariance */

  long double covariance = 0 ;

  size_t i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      const long double delta1 = (data1[i * stride1] - mean1);
      const long double delta2 = (data2[i * stride2] - mean2);
      covariance += (delta1 * delta2 - covariance) / (i + 1);
    }

  return covariance ;
}

double 
FUNCTION(gsl_stats,covariance_m) (const BASE data1[], const size_t stride1, 
                                  const BASE data2[], const size_t stride2, 
                                  const size_t n, 
                                  const double mean1, const double mean2)
{
  const double covariance = FUNCTION(compute,covariance) (data1, stride1,
                                                          data2, stride2,
                                                          n, 
                                                          mean1, mean2);
  
  return covariance * ((double)n / (double)(n - 1));
}

double 
FUNCTION(gsl_stats,covariance) (const BASE data1[], const size_t stride1,
                                const BASE data2[], const size_t stride2,
                                const size_t n)
{
  const double mean1 = FUNCTION(gsl_stats,mean) (data1, stride1, n);
  const double mean2 = FUNCTION(gsl_stats,mean) (data2, stride2, n);

  return FUNCTION(gsl_stats,covariance_m)(data1, stride1, 
                                          data2, stride2, 
                                          n, 
                                          mean1, mean2);
}

/*
gsl_stats_correlation()
  Calculate Pearson correlation = cov(X, Y) / (sigma_X * sigma_Y)
This routine efficiently computes the correlation in one pass of the
data and makes use of the algorithm described in:

B. P. Welford, "Note on a Method for Calculating Corrected Sums of
Squares and Products", Technometrics, Vol 4, No 3, 1962.

This paper derives a numerically stable recurrence to compute a sum
of products

S = sum_{i=1..N} [ (x_i - mu_x) * (y_i - mu_y) ]

with the relation

S_n = S_{n-1} + ((n-1)/n) * (x_n - mu_x_{n-1}) * (y_n - mu_y_{n-1})
*/

double
FUNCTION(gsl_stats,correlation) (const BASE data1[], const size_t stride1,
                                 const BASE data2[], const size_t stride2,
                                 const size_t n)
{
  size_t i;
  long double sum_xsq = 0.0;
  long double sum_ysq = 0.0;
  long double sum_cross = 0.0;
  long double ratio;
  long double delta_x, delta_y;
  long double mean_x, mean_y;
  long double r;

  /*
   * Compute:
   * sum_xsq = Sum [ (x_i - mu_x)^2 ],
   * sum_ysq = Sum [ (y_i - mu_y)^2 ] and
   * sum_cross = Sum [ (x_i - mu_x) * (y_i - mu_y) ]
   * using the above relation from Welford's paper
   */

  mean_x = data1[0 * stride1];
  mean_y = data2[0 * stride2];

  for (i = 1; i < n; ++i)
    {
      ratio = i / (i + 1.0);
      delta_x = data1[i * stride1] - mean_x;
      delta_y = data2[i * stride2] - mean_y;
      sum_xsq += delta_x * delta_x * ratio;
      sum_ysq += delta_y * delta_y * ratio;
      sum_cross += delta_x * delta_y * ratio;
      mean_x += delta_x / (i + 1.0);
      mean_y += delta_y / (i + 1.0);
    }

  r = sum_cross / (sqrt(sum_xsq) * sqrt(sum_ysq));

  return r;
}
