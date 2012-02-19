/* statistics/gsl_statistics_uchar.h
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

#ifndef __GSL_STATISTICS_UCHAR_H__
#define __GSL_STATISTICS_UCHAR_H__

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

double gsl_stats_uchar_mean (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_variance (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_sd (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_variance_with_fixed_mean (const unsigned char data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_uchar_sd_with_fixed_mean (const unsigned char data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_uchar_absdev (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_skew (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_kurtosis (const unsigned char data[], const size_t stride, const size_t n);
double gsl_stats_uchar_lag1_autocorrelation (const unsigned char data[], const size_t stride, const size_t n);

double gsl_stats_uchar_covariance (const unsigned char data1[], const size_t stride1,const unsigned char data2[], const size_t stride2, const size_t n);
double gsl_stats_uchar_correlation (const unsigned char data1[], const size_t stride1,const unsigned char data2[], const size_t stride2, const size_t n);

double gsl_stats_uchar_variance_m (const unsigned char data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_uchar_sd_m (const unsigned char data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_uchar_absdev_m (const unsigned char data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_uchar_skew_m_sd (const unsigned char data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_uchar_kurtosis_m_sd (const unsigned char data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_uchar_lag1_autocorrelation_m (const unsigned char data[], const size_t stride, const size_t n, const double mean);

double gsl_stats_uchar_covariance_m (const unsigned char data1[], const size_t stride1,const unsigned char data2[], const size_t stride2, const size_t n, const double mean1, const double mean2);


double gsl_stats_uchar_pvariance (const unsigned char data1[], const size_t stride1, const size_t n1, const unsigned char data2[], const size_t stride2, const size_t n2);
double gsl_stats_uchar_ttest (const unsigned char data1[], const size_t stride1, const size_t n1, const unsigned char data2[], const size_t stride2, const size_t n2);

unsigned char gsl_stats_uchar_max (const unsigned char data[], const size_t stride, const size_t n);
unsigned char gsl_stats_uchar_min (const unsigned char data[], const size_t stride, const size_t n);
void gsl_stats_uchar_minmax (unsigned char * min, unsigned char * max, const unsigned char data[], const size_t stride, const size_t n);

size_t gsl_stats_uchar_max_index (const unsigned char data[], const size_t stride, const size_t n);
size_t gsl_stats_uchar_min_index (const unsigned char data[], const size_t stride, const size_t n);
void gsl_stats_uchar_minmax_index (size_t * min_index, size_t * max_index, const unsigned char data[], const size_t stride, const size_t n);

double gsl_stats_uchar_median_from_sorted_data (const unsigned char sorted_data[], const size_t stride, const size_t n) ;
double gsl_stats_uchar_quantile_from_sorted_data (const unsigned char sorted_data[], const size_t stride, const size_t n, const double f) ;

__END_DECLS

#endif /* __GSL_STATISTICS_UCHAR_H__ */
