/* histogram/gsl_histogram2d.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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

#ifndef __GSL_HISTOGRAM2D_H__
#define __GSL_HISTOGRAM2D_H__

#include <stdlib.h>
#include <stdio.h>

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

typedef struct {
  size_t nx, ny ;
  double * xrange ;
  double * yrange ;
  double * bin ;
} gsl_histogram2d ;

typedef struct {
  size_t nx, ny ;
  double * xrange ;
  double * yrange ;
  double * sum ;
} gsl_histogram2d_pdf ;

gsl_histogram2d * gsl_histogram2d_alloc (const size_t nx, const size_t ny);
gsl_histogram2d * gsl_histogram2d_calloc (const size_t nx, const size_t ny);
gsl_histogram2d * gsl_histogram2d_calloc_uniform (const size_t nx, const size_t ny,
                                             const double xmin, const double xmax,
                                             const double ymin, const double ymax);

void gsl_histogram2d_free (gsl_histogram2d * h);

int gsl_histogram2d_increment (gsl_histogram2d * h, double x, double y);
int gsl_histogram2d_accumulate (gsl_histogram2d * h, 
                                double x, double y, double weight);
int gsl_histogram2d_find (const gsl_histogram2d * h, 
                          const double x, const double y, size_t * i, size_t * j);

double gsl_histogram2d_get (const gsl_histogram2d * h, const size_t i, const size_t j);
int gsl_histogram2d_get_xrange (const gsl_histogram2d * h, const size_t i,
                                double * xlower, double * xupper);
int gsl_histogram2d_get_yrange (const gsl_histogram2d * h, const size_t j,
                                double * ylower, double * yupper);

                                     
double gsl_histogram2d_xmax (const gsl_histogram2d * h);
double gsl_histogram2d_xmin (const gsl_histogram2d * h);
size_t gsl_histogram2d_nx (const gsl_histogram2d * h);

double gsl_histogram2d_ymax (const gsl_histogram2d * h);
double gsl_histogram2d_ymin (const gsl_histogram2d * h);
size_t gsl_histogram2d_ny (const gsl_histogram2d * h);

void gsl_histogram2d_reset (gsl_histogram2d * h);

gsl_histogram2d * 
gsl_histogram2d_calloc_range(size_t nx, size_t ny, 
                             double *xrange, double *yrange);

int 
gsl_histogram2d_set_ranges_uniform (gsl_histogram2d * h, 
                                    double xmin, double xmax,
                                    double ymin, double ymax);

int 
gsl_histogram2d_set_ranges (gsl_histogram2d * h, 
                            const double xrange[], size_t xsize,
                            const double yrange[], size_t ysize);

int 
gsl_histogram2d_memcpy(gsl_histogram2d *dest, const gsl_histogram2d *source);

gsl_histogram2d *
gsl_histogram2d_clone(const gsl_histogram2d * source);

double
gsl_histogram2d_max_val(const gsl_histogram2d *h);

void
gsl_histogram2d_max_bin (const gsl_histogram2d *h, size_t *i, size_t *j);

double
gsl_histogram2d_min_val(const gsl_histogram2d *h);

void
gsl_histogram2d_min_bin (const gsl_histogram2d *h, size_t *i, size_t *j);

double
gsl_histogram2d_xmean (const gsl_histogram2d * h);

double
gsl_histogram2d_ymean (const gsl_histogram2d * h);

double
gsl_histogram2d_xsigma (const gsl_histogram2d * h);

double
gsl_histogram2d_ysigma (const gsl_histogram2d * h);

double
gsl_histogram2d_cov (const gsl_histogram2d * h);

double
gsl_histogram2d_sum (const gsl_histogram2d *h);

int 
gsl_histogram2d_equal_bins_p(const gsl_histogram2d *h1,
                             const gsl_histogram2d *h2) ;

int
gsl_histogram2d_add(gsl_histogram2d *h1, const gsl_histogram2d *h2);

int
gsl_histogram2d_sub(gsl_histogram2d *h1, const gsl_histogram2d *h2);

int
gsl_histogram2d_mul(gsl_histogram2d *h1, const gsl_histogram2d *h2);

int
gsl_histogram2d_div(gsl_histogram2d *h1, const gsl_histogram2d *h2);

int
gsl_histogram2d_scale(gsl_histogram2d *h, double scale);

int
gsl_histogram2d_shift(gsl_histogram2d *h, double shift);

int gsl_histogram2d_fwrite (FILE * stream, const gsl_histogram2d * h) ;
int gsl_histogram2d_fread (FILE * stream, gsl_histogram2d * h);
int gsl_histogram2d_fprintf (FILE * stream, const gsl_histogram2d * h, 
                             const char * range_format,
                             const char * bin_format);
int gsl_histogram2d_fscanf (FILE * stream, gsl_histogram2d * h);

gsl_histogram2d_pdf * gsl_histogram2d_pdf_alloc (const size_t nx, const size_t ny);
int gsl_histogram2d_pdf_init (gsl_histogram2d_pdf * p, const gsl_histogram2d * h);
void gsl_histogram2d_pdf_free (gsl_histogram2d_pdf * p);
int gsl_histogram2d_pdf_sample (const gsl_histogram2d_pdf * p, 
                                   double r1, double r2, 
                                   double * x, double * y);

__END_DECLS

#endif /* __GSL_HISTOGRAM2D_H__ */

