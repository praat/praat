/* fit/gsl_fit.h
 * 
 * Copyright (C) 2000, 2007 Brian Gough
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

#ifndef __GSL_FIT_H__
#define __GSL_FIT_H__

#include <stdlib.h>
#include "gsl_math.h"

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

int gsl_fit_linear (const double * x, const size_t xstride,
                    const double * y, const size_t ystride,
                    const size_t n,
                    double * c0, double * c1, 
                    double * cov00, double * cov01, double * cov11, 
                    double * sumsq);


int gsl_fit_wlinear (const double * x, const size_t xstride,
                     const double * w, const size_t wstride,
                     const double * y, const size_t ystride,
                     const size_t n,
                     double * c0, double * c1, 
                     double * cov00, double * cov01, double * cov11, 
                     double * chisq);

int
gsl_fit_linear_est (const double x, 
                    const double c0, const double c1, 
                    const double cov00, const double cov01, const double cov11,
                    double *y, double *y_err);


int gsl_fit_mul (const double * x, const size_t xstride,
                 const double * y, const size_t ystride,
                 const size_t n,
                 double * c1, 
                 double * cov11, 
                 double * sumsq);

int gsl_fit_wmul (const double * x, const size_t xstride,
                  const double * w, const size_t wstride,
                  const double * y, const size_t ystride,
                  const size_t n,
                  double * c1, 
                  double * cov11, 
                  double * sumsq);


int
gsl_fit_mul_est (const double x, 
                 const double c1, 
                 const double cov11,
                 double *y, double *y_err);

__END_DECLS

#endif /* __GSL_FIT_H__ */
