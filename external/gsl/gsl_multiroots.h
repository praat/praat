/* multiroots/gsl_multiroots.h
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

#ifndef __GSL_MULTIROOTS_H__
#define __GSL_MULTIROOTS_H__

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

/* Definition of vector-valued functions with parameters based on gsl_vector */

struct gsl_multiroot_function_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  size_t n;
  void * params;
};

typedef struct gsl_multiroot_function_struct gsl_multiroot_function ;

#define GSL_MULTIROOT_FN_EVAL(F,x,y) (*((F)->f))(x,(F)->params,(y))

int gsl_multiroot_fdjacobian (gsl_multiroot_function * F,
                              const gsl_vector * x, const gsl_vector * f,
                              double epsrel, gsl_matrix * jacobian);


typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n);
    int (*set) (void *state, gsl_multiroot_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    int (*iterate) (void *state, gsl_multiroot_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    void (*free) (void *state);
  }
gsl_multiroot_fsolver_type;

typedef struct
  {
    const gsl_multiroot_fsolver_type * type;
    gsl_multiroot_function * function ;
    gsl_vector * x ;
    gsl_vector * f ;
    gsl_vector * dx ;
    void *state;
  }
gsl_multiroot_fsolver;

gsl_multiroot_fsolver * 
gsl_multiroot_fsolver_alloc (const gsl_multiroot_fsolver_type * T, 
                                     size_t n); 

void gsl_multiroot_fsolver_free (gsl_multiroot_fsolver * s);

int gsl_multiroot_fsolver_set (gsl_multiroot_fsolver * s, 
                               gsl_multiroot_function * f, 
                               const gsl_vector * x);

int gsl_multiroot_fsolver_iterate (gsl_multiroot_fsolver * s);

const char * gsl_multiroot_fsolver_name (const gsl_multiroot_fsolver * s);
gsl_vector * gsl_multiroot_fsolver_root (const gsl_multiroot_fsolver * s);
gsl_vector * gsl_multiroot_fsolver_dx (const gsl_multiroot_fsolver * s);
gsl_vector * gsl_multiroot_fsolver_f (const gsl_multiroot_fsolver * s);

/* Definition of vector-valued functions and gradient with parameters
   based on gsl_vector */

struct gsl_multiroot_function_fdf_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  int (* df) (const gsl_vector * x, void * params, gsl_matrix * df);
  int (* fdf) (const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);
  size_t n;
  void * params;
};

typedef struct gsl_multiroot_function_fdf_struct gsl_multiroot_function_fdf ;

#define GSL_MULTIROOT_FN_EVAL_F(F,x,y) ((*((F)->f))(x,(F)->params,(y)))
#define GSL_MULTIROOT_FN_EVAL_DF(F,x,dy) ((*((F)->df))(x,(F)->params,(dy)))
#define GSL_MULTIROOT_FN_EVAL_F_DF(F,x,y,dy) ((*((F)->fdf))(x,(F)->params,(y),(dy)))

typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n);
    int (*set) (void *state, gsl_multiroot_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
    int (*iterate) (void *state, gsl_multiroot_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
    void (*free) (void *state);
  }
gsl_multiroot_fdfsolver_type;

typedef struct
  {
    const gsl_multiroot_fdfsolver_type * type;
    gsl_multiroot_function_fdf * fdf ;
    gsl_vector * x;
    gsl_vector * f;
    gsl_matrix * J;
    gsl_vector * dx;
    void *state;
  }
gsl_multiroot_fdfsolver;

gsl_multiroot_fdfsolver *
gsl_multiroot_fdfsolver_alloc (const gsl_multiroot_fdfsolver_type * T,
                                      size_t n);

int
gsl_multiroot_fdfsolver_set (gsl_multiroot_fdfsolver * s, 
                             gsl_multiroot_function_fdf * fdf,
                             const gsl_vector * x);

int
gsl_multiroot_fdfsolver_iterate (gsl_multiroot_fdfsolver * s);

void
gsl_multiroot_fdfsolver_free (gsl_multiroot_fdfsolver * s);

const char * gsl_multiroot_fdfsolver_name (const gsl_multiroot_fdfsolver * s);
gsl_vector * gsl_multiroot_fdfsolver_root (const gsl_multiroot_fdfsolver * s);
gsl_vector * gsl_multiroot_fdfsolver_dx (const gsl_multiroot_fdfsolver * s);
gsl_vector * gsl_multiroot_fdfsolver_f (const gsl_multiroot_fdfsolver * s);

int gsl_multiroot_test_delta (const gsl_vector * dx, const gsl_vector * x, 
                              double epsabs, double epsrel);

int gsl_multiroot_test_residual (const gsl_vector * f, double epsabs);

GSL_VAR const gsl_multiroot_fsolver_type * gsl_multiroot_fsolver_dnewton;
GSL_VAR const gsl_multiroot_fsolver_type * gsl_multiroot_fsolver_broyden;
GSL_VAR const gsl_multiroot_fsolver_type * gsl_multiroot_fsolver_hybrid;
GSL_VAR const gsl_multiroot_fsolver_type * gsl_multiroot_fsolver_hybrids;

GSL_VAR const gsl_multiroot_fdfsolver_type * gsl_multiroot_fdfsolver_newton;
GSL_VAR const gsl_multiroot_fdfsolver_type * gsl_multiroot_fdfsolver_gnewton;
GSL_VAR const gsl_multiroot_fdfsolver_type * gsl_multiroot_fdfsolver_hybridj;
GSL_VAR const gsl_multiroot_fdfsolver_type * gsl_multiroot_fdfsolver_hybridsj;


__END_DECLS

#endif /* __GSL_MULTIROOTS_H__ */
