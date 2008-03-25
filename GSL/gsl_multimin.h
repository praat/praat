/* multimin/gsl_multimin.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Fabrice Rossi
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

/* Modified by Tuomo Keskitalo to include fminimizer and 
   Nelder Mead related lines */

#ifndef __GSL_MULTIMIN_H__
#define __GSL_MULTIMIN_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_math.h"
#include "gsl_vector.h"
#include "gsl_matrix.h"
#include "gsl_min.h"

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

/* Definition of an arbitrary real-valued function with gsl_vector input and */
/* parameters */
struct gsl_multimin_function_struct 
{
  double (* f) (const gsl_vector * x, void * params);
  size_t n;
  void * params;
};

typedef struct gsl_multimin_function_struct gsl_multimin_function;

#define GSL_MULTIMIN_FN_EVAL(F,x) (*((F)->f))(x,(F)->params)

/* Definition of an arbitrary differentiable real-valued function */
/* with gsl_vector input and parameters */
struct gsl_multimin_function_fdf_struct 
{
  double (* f) (const gsl_vector  * x, void * params);
  void (* df) (const gsl_vector * x, void * params,gsl_vector * df);
  void (* fdf) (const gsl_vector * x, void * params,double *f,gsl_vector * df);
  size_t n;
  void * params;
};

typedef struct gsl_multimin_function_fdf_struct gsl_multimin_function_fdf;

#define GSL_MULTIMIN_FN_EVAL_F(F,x) (*((F)->f))(x,(F)->params)
#define GSL_MULTIMIN_FN_EVAL_DF(F,x,g) (*((F)->df))(x,(F)->params,(g))
#define GSL_MULTIMIN_FN_EVAL_F_DF(F,x,y,g) (*((F)->fdf))(x,(F)->params,(y),(g))

int gsl_multimin_diff (const gsl_multimin_function * f,
                       const gsl_vector * x, gsl_vector * g);

/* minimization of non-differentiable functions */

typedef struct 
{
  const char *name;
  size_t size;
  int (*alloc) (void *state, size_t n);
  int (*set) (void *state, gsl_multimin_function * f,
              const gsl_vector * x, 
              double * size,
              const gsl_vector * step_size);
  int (*iterate) (void *state, gsl_multimin_function * f, 
                  gsl_vector * x, 
                  double * size,
                  double * fval);
  void (*free) (void *state);
}
gsl_multimin_fminimizer_type;

typedef struct 
{
  /* multi dimensional part */
  const gsl_multimin_fminimizer_type *type;
  gsl_multimin_function *f;

  double fval;
  gsl_vector * x;
  
  double size;

  void *state;
}
gsl_multimin_fminimizer;

gsl_multimin_fminimizer *
gsl_multimin_fminimizer_alloc(const gsl_multimin_fminimizer_type *T,
                              size_t n);

int 
gsl_multimin_fminimizer_set (gsl_multimin_fminimizer * s,
                             gsl_multimin_function * f,
                             const gsl_vector * x,
                             const gsl_vector * step_size);

void
gsl_multimin_fminimizer_free(gsl_multimin_fminimizer *s);

const char * 
gsl_multimin_fminimizer_name (const gsl_multimin_fminimizer * s);

int
gsl_multimin_fminimizer_iterate(gsl_multimin_fminimizer *s);

gsl_vector * 
gsl_multimin_fminimizer_x (const gsl_multimin_fminimizer * s);

double 
gsl_multimin_fminimizer_minimum (const gsl_multimin_fminimizer * s);

double
gsl_multimin_fminimizer_size (const gsl_multimin_fminimizer * s);

/* Convergence test functions */

int
gsl_multimin_test_gradient(const gsl_vector * g,double epsabs);

int
gsl_multimin_test_size(const double size ,double epsabs);

/* minimisation of differentiable functions */

typedef struct 
{
  const char *name;
  size_t size;
  int (*alloc) (void *state, size_t n);
  int (*set) (void *state, gsl_multimin_function_fdf * fdf,
              const gsl_vector * x, double * f, 
              gsl_vector * gradient, double step_size, double tol);
  int (*iterate) (void *state,gsl_multimin_function_fdf * fdf, 
                  gsl_vector * x, double * f, 
                  gsl_vector * gradient, gsl_vector * dx);
  int (*restart) (void *state);
  void (*free) (void *state);
}
gsl_multimin_fdfminimizer_type;

typedef struct 
{
  /* multi dimensional part */
  const gsl_multimin_fdfminimizer_type *type;
  gsl_multimin_function_fdf *fdf;

  double f;
  gsl_vector * x;
  gsl_vector * gradient;
  gsl_vector * dx;

  void *state;
}
gsl_multimin_fdfminimizer;

gsl_multimin_fdfminimizer *
gsl_multimin_fdfminimizer_alloc(const gsl_multimin_fdfminimizer_type *T,
                                size_t n);

int 
gsl_multimin_fdfminimizer_set (gsl_multimin_fdfminimizer * s,
                               gsl_multimin_function_fdf *fdf,
                               const gsl_vector * x,
                               double step_size, double tol);

void
gsl_multimin_fdfminimizer_free(gsl_multimin_fdfminimizer *s);

const char * 
gsl_multimin_fdfminimizer_name (const gsl_multimin_fdfminimizer * s);

int
gsl_multimin_fdfminimizer_iterate(gsl_multimin_fdfminimizer *s);

int
gsl_multimin_fdfminimizer_restart(gsl_multimin_fdfminimizer *s);

gsl_vector * 
gsl_multimin_fdfminimizer_x (gsl_multimin_fdfminimizer * s);

gsl_vector * 
gsl_multimin_fdfminimizer_dx (gsl_multimin_fdfminimizer * s);

gsl_vector * 
gsl_multimin_fdfminimizer_gradient (gsl_multimin_fdfminimizer * s);

double 
gsl_multimin_fdfminimizer_minimum (gsl_multimin_fdfminimizer * s);

GSL_VAR const gsl_multimin_fdfminimizer_type *gsl_multimin_fdfminimizer_steepest_descent;
GSL_VAR const gsl_multimin_fdfminimizer_type *gsl_multimin_fdfminimizer_conjugate_pr;
GSL_VAR const gsl_multimin_fdfminimizer_type *gsl_multimin_fdfminimizer_conjugate_fr;
GSL_VAR const gsl_multimin_fdfminimizer_type *gsl_multimin_fdfminimizer_vector_bfgs;
GSL_VAR const gsl_multimin_fdfminimizer_type *gsl_multimin_fdfminimizer_vector_bfgs2;
GSL_VAR const gsl_multimin_fminimizer_type *gsl_multimin_fminimizer_nmsimplex;


__END_DECLS

#endif /* __GSL_MULTIMIN_H__ */
