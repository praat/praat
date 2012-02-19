/* interpolation/gsl_spline.h
 * 
 * Copyright (C) 2001, 2007 Brian Gough
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

#ifndef __GSL_SPLINE_H__
#define __GSL_SPLINE_H__
#include <stdlib.h>
#include "gsl_interp.h"

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


/* general interpolation object */
typedef struct {
  gsl_interp * interp;
  double  * x;
  double  * y;
  size_t  size;
} gsl_spline;

gsl_spline *
gsl_spline_alloc(const gsl_interp_type * T, size_t size);
     
int
gsl_spline_init(gsl_spline * spline, const double xa[], const double ya[], size_t size);

const char * gsl_spline_name(const gsl_spline * spline);
unsigned int gsl_spline_min_size(const gsl_spline * spline);


int
gsl_spline_eval_e(const gsl_spline * spline, double x,
                  gsl_interp_accel * a, double * y);

double
gsl_spline_eval(const gsl_spline * spline, double x, gsl_interp_accel * a);

int
gsl_spline_eval_deriv_e(const gsl_spline * spline,
                        double x,
                        gsl_interp_accel * a,
                        double * y);

double
gsl_spline_eval_deriv(const gsl_spline * spline,
                      double x,
                      gsl_interp_accel * a);

int
gsl_spline_eval_deriv2_e(const gsl_spline * spline,
                         double x,
                         gsl_interp_accel * a,
                         double * y);

double
gsl_spline_eval_deriv2(const gsl_spline * spline,
                       double x,
                       gsl_interp_accel * a);

int
gsl_spline_eval_integ_e(const gsl_spline * spline,
                        double a, double b,
                        gsl_interp_accel * acc,
                        double * y);

double
gsl_spline_eval_integ(const gsl_spline * spline,
                      double a, double b,
                      gsl_interp_accel * acc);

void
gsl_spline_free(gsl_spline * spline);

__END_DECLS

#endif /* __GSL_INTERP_H__ */
