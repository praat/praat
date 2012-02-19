/* multifit_nlin/gsl_multifit_nlin.h
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

#ifndef __GSL_MULTIFIT_NLIN_H__
#define __GSL_MULTIFIT_NLIN_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"

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

int gsl_multifit_gradient (const gsl_matrix * J, const gsl_vector * f,
                           gsl_vector * g);

int gsl_multifit_covar (const gsl_matrix * J, double epsrel, gsl_matrix * covar);


/* Definition of vector-valued functions with parameters based on gsl_vector */

struct gsl_multifit_function_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  size_t n;   /* number of functions */
  size_t p;   /* number of independent variables */
  void * params;
};

typedef struct gsl_multifit_function_struct gsl_multifit_function ;

#define GSL_MULTIFIT_FN_EVAL(F,x,y) (*((F)->f))(x,(F)->params,(y))

typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n, size_t p);
    int (*set) (void *state, gsl_multifit_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    int (*iterate) (void *state, gsl_multifit_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    void (*free) (void *state);
  }
gsl_multifit_fsolver_type;

typedef struct
  {
    const gsl_multifit_fsolver_type * type;
    gsl_multifit_function * function ;
    gsl_vector * x ;
    gsl_vector * f ;
    gsl_vector * dx ;
    void *state;
  }
gsl_multifit_fsolver;

gsl_multifit_fsolver *
gsl_multifit_fsolver_alloc (const gsl_multifit_fsolver_type * T, 
                            size_t n, size_t p);

void gsl_multifit_fsolver_free (gsl_multifit_fsolver * s);

int gsl_multifit_fsolver_set (gsl_multifit_fsolver * s, 
                                   gsl_multifit_function * f, 
                                   const gsl_vector * x);

int gsl_multifit_fsolver_iterate (gsl_multifit_fsolver * s);

const char * gsl_multifit_fsolver_name (const gsl_multifit_fsolver * s);
gsl_vector * gsl_multifit_fsolver_position (const gsl_multifit_fsolver * s);

/* Definition of vector-valued functions and gradient with parameters
   based on gsl_vector */

struct gsl_multifit_function_fdf_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  int (* df) (const gsl_vector * x, void * params, gsl_matrix * df);
  int (* fdf) (const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);
  size_t n;   /* number of functions */
  size_t p;   /* number of independent variables */
  void * params;
};

typedef struct gsl_multifit_function_fdf_struct gsl_multifit_function_fdf ;

#define GSL_MULTIFIT_FN_EVAL_F(F,x,y) ((*((F)->f))(x,(F)->params,(y)))
#define GSL_MULTIFIT_FN_EVAL_DF(F,x,dy) ((*((F)->df))(x,(F)->params,(dy)))
#define GSL_MULTIFIT_FN_EVAL_F_DF(F,x,y,dy) ((*((F)->fdf))(x,(F)->params,(y),(dy)))

typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n, size_t p);
    int (*set) (void *state, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
    int (*iterate) (void *state, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
    void (*free) (void *state);
  }
gsl_multifit_fdfsolver_type;

typedef struct
  {
    const gsl_multifit_fdfsolver_type * type;
    gsl_multifit_function_fdf * fdf ;
    gsl_vector * x;
    gsl_vector * f;
    gsl_matrix * J;
    gsl_vector * dx;
    void *state;
  }
gsl_multifit_fdfsolver;


gsl_multifit_fdfsolver *
gsl_multifit_fdfsolver_alloc (const gsl_multifit_fdfsolver_type * T, 
                              size_t n, size_t p);

int
gsl_multifit_fdfsolver_set (gsl_multifit_fdfsolver * s, 
                                 gsl_multifit_function_fdf * fdf,
                                 const gsl_vector * x);

int
gsl_multifit_fdfsolver_iterate (gsl_multifit_fdfsolver * s);

void
gsl_multifit_fdfsolver_free (gsl_multifit_fdfsolver * s);

const char * gsl_multifit_fdfsolver_name (const gsl_multifit_fdfsolver * s);
gsl_vector * gsl_multifit_fdfsolver_position (const gsl_multifit_fdfsolver * s);

int gsl_multifit_test_delta (const gsl_vector * dx, const gsl_vector * x, 
                             double epsabs, double epsrel);

int gsl_multifit_test_gradient (const gsl_vector * g, double epsabs);

/* extern const gsl_multifit_fsolver_type * gsl_multifit_fsolver_gradient; */

GSL_VAR const gsl_multifit_fdfsolver_type * gsl_multifit_fdfsolver_lmder;
GSL_VAR const gsl_multifit_fdfsolver_type * gsl_multifit_fdfsolver_lmsder;


__END_DECLS

#endif /* __GSL_MULTIFIT_NLIN_H__ */
