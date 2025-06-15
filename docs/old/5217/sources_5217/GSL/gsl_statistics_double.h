/* statistics/gsl_statistics_double.h
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

#ifndef __GSL_STATISTICS_DOUBLE_H__
#define __GSL_STATISTICS_DOUBLE_H__

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

double gsl_stats_mean (const double data[], const size_t stride, const size_t n);
double gsl_stats_variance (const double data[], const size_t stride, const size_t n);
double gsl_stats_sd (const double data[], const size_t stride, const size_t n);
double gsl_stats_variance_with_fixed_mean (const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_sd_with_fixed_mean (const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_absdev (const double data[], const size_t stride, const size_t n);
double gsl_stats_skew (const double data[], const size_t stride, const size_t n);
double gsl_stats_kurtosis (const double data[], const size_t stride, const size_t n);
double gsl_stats_lag1_autocorrelation (const double data[], const size_t stride, const size_t n);

double gsl_stats_covariance (const double data1[], const size_t stride1,const double data2[], const size_t stride2, const size_t n);
double gsl_stats_correlation (const double data1[], const size_t stride1,const double data2[], const size_t stride2, const size_t n);

double gsl_stats_variance_m (const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_sd_m (const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_absdev_m (const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_skew_m_sd (const double data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_kurtosis_m_sd (const double data[], const size_t stride, const size_t n, const double mean, const double sd);
double gsl_stats_lag1_autocorrelation_m (const double data[], const size_t stride, const size_t n, const double mean);

double gsl_stats_covariance_m (const double data1[], const size_t stride1,const double data2[], const size_t stride2, const size_t n, const double mean1, const double mean2);

/* DEFINED FOR FLOATING POINT TYPES ONLY */

double gsl_stats_wmean (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);
double gsl_stats_wvariance (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);
double gsl_stats_wsd (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);
double gsl_stats_wvariance_with_fixed_mean (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_wsd_with_fixed_mean (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double mean);
double gsl_stats_wabsdev (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);
double gsl_stats_wskew (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);
double gsl_stats_wkurtosis (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n);

double gsl_stats_wvariance_m (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double wmean);
double gsl_stats_wsd_m (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double wmean);
double gsl_stats_wabsdev_m (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double wmean);
double gsl_stats_wskew_m_sd (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double wmean, const double wsd);
double gsl_stats_wkurtosis_m_sd (const double w[], const size_t wstride, const double data[], const size_t stride, const size_t n, const double wmean, const double wsd);

/* END OF FLOATING POINT TYPES */

double gsl_stats_pvariance (const double data1[], const size_t stride1, const size_t n1, const double data2[], const size_t stride2, const size_t n2);
double gsl_stats_ttest (const double data1[], const size_t stride1, const size_t n1, const double data2[], const size_t stride2, const size_t n2);

double gsl_stats_max (const double data[], const size_t stride, const size_t n);
double gsl_stats_min (const double data[], const size_t stride, const size_t n);
void gsl_stats_minmax (double * min, double * max, const double data[], const size_t stride, const size_t n);

size_t gsl_stats_max_index (const double data[], const size_t stride, const size_t n);
size_t gsl_stats_min_index (const double data[], const size_t stride, const size_t n);
void gsl_stats_minmax_index (size_t * min_index, size_t * max_index, const double data[], const size_t stride, const size_t n);

double gsl_stats_median_from_sorted_data (const double sorted_data[], const size_t stride, const size_t n) ;
double gsl_stats_quantile_from_sorted_data (const double sorted_data[], const size_t stride, const size_t n, const double f) ;

__END_DECLS

#endif /* __GSL_STATISTICS_DOUBLE_H__ */
