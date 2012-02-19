/* siman/gsl_siman.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Mark Galassi
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

#ifndef __GSL_SIMAN_H__
#define __GSL_SIMAN_H__
#include <stdlib.h>
#include "gsl_rng.h"

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

/* types for the function pointers passed to gsl_siman_solve */

typedef double (*gsl_siman_Efunc_t) (void *xp);
typedef void (*gsl_siman_step_t) (const gsl_rng *r, void *xp, double step_size);
typedef double (*gsl_siman_metric_t) (void *xp, void *yp);
typedef void (*gsl_siman_print_t) (void *xp);
typedef void (*gsl_siman_copy_t) (void *source, void *dest);
typedef void * (*gsl_siman_copy_construct_t) (void *xp);
typedef void (*gsl_siman_destroy_t) (void *xp);

/* this structure contains all the information needed to structure the
   search, beyond the energy function, the step function and the
   initial guess. */

typedef struct {
  int n_tries;          /* how many points to try for each step */
  int iters_fixed_T;    /* how many iterations at each temperature? */
  double step_size;     /* max step size in the random walk */
  /* the following parameters are for the Boltzmann distribution */
  double k, t_initial, mu_t, t_min;
} gsl_siman_params_t;

/* prototype for the workhorse function */

void gsl_siman_solve(const gsl_rng * r, 
                     void *x0_p, gsl_siman_Efunc_t Ef,
                     gsl_siman_step_t take_step,
                     gsl_siman_metric_t distance,
                     gsl_siman_print_t print_position,
                     gsl_siman_copy_t copyfunc,
                     gsl_siman_copy_construct_t copy_constructor,
                     gsl_siman_destroy_t destructor,
                     size_t element_size,
                     gsl_siman_params_t params);

void 
gsl_siman_solve_many (const gsl_rng * r, void *x0_p, gsl_siman_Efunc_t Ef,
                      gsl_siman_step_t take_step,
                      gsl_siman_metric_t distance,
                      gsl_siman_print_t print_position,
                      size_t element_size,
                      gsl_siman_params_t params);

__END_DECLS

#endif /* __GSL_SIMAN_H__ */
