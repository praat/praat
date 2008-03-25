/* statistics/gsl_statistics_int.h
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

#ifndef __GSL_STATISTICS_INT_H__
#define __GSL_STATISTICS_INT_H__

#include <stddef.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

double gsl_stats_int_mean (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_variance (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_sd (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_variance_with_fixed_mean (const int data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_int_sd_with_fixed_mean (const int data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_int_absdev (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_skew (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_kurtosis (const int data[], const size_t stride, const size_t n);
double gsl_stats_int_lag1_autocorrelation (const int data[], const size_t stride, const size_t n);

double gsl_stats_int_covariance (const int data1[], const size_t stride1,const int data2[], const size_t stride2, const size_t n);
double gsl_stats_int_correlation (const int data1[], const size_t stride1,const int data2[], const size_t stride2, const size_t n);

double gsl_stats_int_variance_m (const int data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_int_sd_m (const int data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_int_absdev_m (const int data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_int_skew_m_sd (const int data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_int_kurtosis_m_sd (const int data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_int_lag1_autocorrelation_m (const int data[], const size_t stride, const size_t n, const double mean);

double gsl_stats_int_covariance_m (const int data1[], const size_t stride1,const int data2[], const size_t stride2, const size_t n, const double mean1, const double mean2);


double gsl_stats_int_pvariance (const int data1[], const size_t stride1, const size_t n1, const int data2[], const size_t stride2, const size_t n2);
double gsl_stats_int_ttest (const int data1[], const size_t stride1, const size_t n1, const int data2[], const size_t stride2, const size_t n2);

int gsl_stats_int_max (const int data[], const size_t stride, const size_t n);
int gsl_stats_int_min (const int data[], const size_t stride, const size_t n);
void gsl_stats_int_minmax (int * min, int * max, const int data[], const size_t stride, const size_t n);

size_t gsl_stats_int_max_index (const int data[], const size_t stride, const size_t n);
size_t gsl_stats_int_min_index (const int data[], const size_t stride, const size_t n);
void gsl_stats_int_minmax_index (size_t * min_index, size_t * max_index, const int data[], const size_t stride, const size_t n);

double gsl_stats_int_median_from_sorted_data (const int sorted_data[], const size_t stride, const size_t n) ;
double gsl_stats_int_quantile_from_sorted_data (const int sorted_data[], const size_t stride, const size_t n, const double f) ;

__END_DECLS

#endif /* __GSL_STATISTICS_INT_H__ */
