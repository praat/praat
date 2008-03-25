/* roots/gsl_roots.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
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

#ifndef __GSL_ROOTS_H__
#define __GSL_ROOTS_H__

#include <stdlib.h>
#include "gsl_types.h"
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

typedef struct
  {
    const char *name;
    size_t size;
    int (*set) (void *state, gsl_function * f, double * root, double x_lower, double x_upper);
    int (*iterate) (void *state, gsl_function * f, double * root, double * x_lower, double * x_upper);
  }
gsl_root_fsolver_type;

typedef struct
  {
    const gsl_root_fsolver_type * type;
    gsl_function * function ;
    double root ;
    double x_lower;
    double x_upper;
    void *state;
  }
gsl_root_fsolver;

typedef struct
  {
    const char *name;
    size_t size;
    int (*set) (void *state, gsl_function_fdf * f, double * root);
    int (*iterate) (void *state, gsl_function_fdf * f, double * root);
  }
gsl_root_fdfsolver_type;

typedef struct
  {
    const gsl_root_fdfsolver_type * type;
    gsl_function_fdf * fdf ;
    double root ;
    void *state;
  }
gsl_root_fdfsolver;

gsl_root_fsolver *
gsl_root_fsolver_alloc (const gsl_root_fsolver_type * T);
void gsl_root_fsolver_free (gsl_root_fsolver * s);

int gsl_root_fsolver_set (gsl_root_fsolver * s,
                          gsl_function * f, 
                          double x_lower, double x_upper);

int gsl_root_fsolver_iterate (gsl_root_fsolver * s);

const char * gsl_root_fsolver_name (const gsl_root_fsolver * s);
double gsl_root_fsolver_root (const gsl_root_fsolver * s);
double gsl_root_fsolver_x_lower (const gsl_root_fsolver * s);
double gsl_root_fsolver_x_upper (const gsl_root_fsolver * s);


gsl_root_fdfsolver *
gsl_root_fdfsolver_alloc (const gsl_root_fdfsolver_type * T);

int
gsl_root_fdfsolver_set (gsl_root_fdfsolver * s, 
                         gsl_function_fdf * fdf, double root);

int
gsl_root_fdfsolver_iterate (gsl_root_fdfsolver * s);

void
gsl_root_fdfsolver_free (gsl_root_fdfsolver * s);

const char * gsl_root_fdfsolver_name (const gsl_root_fdfsolver * s);
double gsl_root_fdfsolver_root (const gsl_root_fdfsolver * s);

int
gsl_root_test_interval (double x_lower, double x_upper, double epsabs, double epsrel);

int
gsl_root_test_residual (double f, double epsabs);

int
gsl_root_test_delta (double x1, double x0, double epsabs, double epsrel);

GSL_VAR const gsl_root_fsolver_type  * gsl_root_fsolver_bisection;
GSL_VAR const gsl_root_fsolver_type  * gsl_root_fsolver_brent;
GSL_VAR const gsl_root_fsolver_type  * gsl_root_fsolver_falsepos;
GSL_VAR const gsl_root_fdfsolver_type  * gsl_root_fdfsolver_newton;
GSL_VAR const gsl_root_fdfsolver_type  * gsl_root_fdfsolver_secant;
GSL_VAR const gsl_root_fdfsolver_type  * gsl_root_fdfsolver_steffenson;

__END_DECLS

#endif /* __GSL_ROOTS_H__ */
