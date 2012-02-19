/* ode-initval/gsl_odeiv.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  G. Jungman
 */
#ifndef __GSL_ODEIV_H__
#define __GSL_ODEIV_H__

#include <stdio.h>
#include <stdlib.h>
#include "gsl_types.h"

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


/* Description of a system of ODEs.
 *
 * y' = f(t,y) = dydt(t, y)
 *
 * The system is specified by giving the right-hand-side
 * of the equation and possibly a jacobian function.
 *
 * Some methods require the jacobian function, which calculates
 * the matrix dfdy and the vector dfdt. The matrix dfdy conforms
 * to the GSL standard, being a continuous range of floating point
 * values, in row-order.
 *
 * As with GSL function objects, user-supplied parameter
 * data is also present. 
 */

typedef struct  
{
  int (* function) (double t, const double y[], double dydt[], void * params);
  int (* jacobian) (double t, const double y[], double * dfdy, double dfdt[], void * params);
  size_t dimension;
  void * params;
}
gsl_odeiv_system;

#define GSL_ODEIV_FN_EVAL(S,t,y,f)  (*((S)->function))(t,y,f,(S)->params)
#define GSL_ODEIV_JA_EVAL(S,t,y,dfdy,dfdt)  (*((S)->jacobian))(t,y,dfdy,dfdt,(S)->params)


/* General stepper object.
 *
 * Opaque object for stepping an ODE system from t to t+h.
 * In general the object has some state which facilitates
 * iterating the stepping operation.
 */

typedef struct 
{
  const char * name;
  int can_use_dydt_in;
  int gives_exact_dydt_out;
  void * (*alloc) (size_t dim);
  int  (*apply)  (void * state, size_t dim, double t, double h, double y[], double yerr[], const double dydt_in[], double dydt_out[], const gsl_odeiv_system * dydt);
  int  (*reset) (void * state, size_t dim);
  unsigned int  (*order) (void * state);
  void (*free)  (void * state);
}
gsl_odeiv_step_type;

typedef struct {
  const gsl_odeiv_step_type * type;
  size_t dimension;
  void * state;
}
gsl_odeiv_step;


/* Available stepper types.
 *
 * rk2    : embedded 2nd(3rd) Runge-Kutta
 * rk4    : 4th order (classical) Runge-Kutta
 * rkck   : embedded 4th(5th) Runge-Kutta, Cash-Karp
 * rk8pd  : embedded 8th(9th) Runge-Kutta, Prince-Dormand
 * rk2imp : implicit 2nd order Runge-Kutta at Gaussian points
 * rk4imp : implicit 4th order Runge-Kutta at Gaussian points
 * gear1  : M=1 implicit Gear method
 * gear2  : M=2 implicit Gear method
 */

GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk2;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk4;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rkf45;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rkck;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk8pd;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk2imp;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk2simp;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_rk4imp;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_bsimp;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_gear1;
GSL_VAR const gsl_odeiv_step_type *gsl_odeiv_step_gear2;


/* Constructor for specialized stepper objects.
 */
gsl_odeiv_step * gsl_odeiv_step_alloc(const gsl_odeiv_step_type * T, size_t dim);
int  gsl_odeiv_step_reset(gsl_odeiv_step * s);
void gsl_odeiv_step_free(gsl_odeiv_step * s);

/* General stepper object methods.
 */
const char * gsl_odeiv_step_name(const gsl_odeiv_step *);
unsigned int gsl_odeiv_step_order(const gsl_odeiv_step * s);

int  gsl_odeiv_step_apply(gsl_odeiv_step *, double t, double h, double y[], double yerr[], const double dydt_in[], double dydt_out[], const gsl_odeiv_system * dydt);

/* General step size control object.
 *
 * The hadjust() method controls the adjustment of
 * step size given the result of a step and the error.
 * Valid hadjust() methods must return one of the codes below.
 *
 * The general data can be used by specializations
 * to store state and control their heuristics.
 */

typedef struct 
{
  const char * name;
  void * (*alloc) (void);
  int  (*init) (void * state, double eps_abs, double eps_rel, double a_y, double a_dydt);
  int  (*hadjust) (void * state, size_t dim, unsigned int ord, const double y[], const double yerr[], const double yp[], double * h);
  void (*free) (void * state);
}
gsl_odeiv_control_type;

typedef struct 
{
  const gsl_odeiv_control_type * type;
  void * state;
}
gsl_odeiv_control;

/* Possible return values for an hadjust() evolution method.
 */
#define GSL_ODEIV_HADJ_INC   1  /* step was increased */
#define GSL_ODEIV_HADJ_NIL   0  /* step unchanged     */
#define GSL_ODEIV_HADJ_DEC (-1) /* step decreased     */

gsl_odeiv_control * gsl_odeiv_control_alloc(const gsl_odeiv_control_type * T);
int gsl_odeiv_control_init(gsl_odeiv_control * c, double eps_abs, double eps_rel, double a_y, double a_dydt);
void gsl_odeiv_control_free(gsl_odeiv_control * c);
int gsl_odeiv_control_hadjust (gsl_odeiv_control * c, gsl_odeiv_step * s, const double y[], const double yerr[], const double dydt[], double * h);
const char * gsl_odeiv_control_name(const gsl_odeiv_control * c);

/* Available control object constructors.
 *
 * The standard control object is a four parameter heuristic
 * defined as follows:
 *    D0 = eps_abs + eps_rel * (a_y |y| + a_dydt h |y'|)
 *    D1 = |yerr|
 *    q  = consistency order of method (q=4 for 4(5) embedded RK)
 *    S  = safety factor (0.9 say)
 *
 *                      /  (D0/D1)^(1/(q+1))  D0 >= D1
 *    h_NEW = S h_OLD * |
 *                      \  (D0/D1)^(1/q)      D0 < D1
 *
 * This encompasses all the standard error scaling methods.
 *
 * The y method is the standard method with a_y=1, a_dydt=0.
 * The yp method is the standard method with a_y=0, a_dydt=1.
 */

gsl_odeiv_control * gsl_odeiv_control_standard_new(double eps_abs, double eps_rel, double a_y, double a_dydt);
gsl_odeiv_control * gsl_odeiv_control_y_new(double eps_abs, double eps_rel);
gsl_odeiv_control * gsl_odeiv_control_yp_new(double eps_abs, double eps_rel);

/* This controller computes errors using different absolute errors for
 * each component
 *
 *    D0 = eps_abs * scale_abs[i] + eps_rel * (a_y |y| + a_dydt h |y'|)
 */
gsl_odeiv_control * gsl_odeiv_control_scaled_new(double eps_abs, double eps_rel, double a_y, double a_dydt, const double scale_abs[], size_t dim);

/* General evolution object.
 */
typedef struct {
  size_t dimension;
  double * y0;
  double * yerr;
  double * dydt_in;
  double * dydt_out;
  double last_step;
  unsigned long int count;
  unsigned long int failed_steps;
}
gsl_odeiv_evolve;

/* Evolution object methods.
 */
gsl_odeiv_evolve * gsl_odeiv_evolve_alloc(size_t dim);
int gsl_odeiv_evolve_apply(gsl_odeiv_evolve *, gsl_odeiv_control * con, gsl_odeiv_step * step, const gsl_odeiv_system * dydt, double * t, double t1, double * h, double y[]);
int gsl_odeiv_evolve_reset(gsl_odeiv_evolve *);
void gsl_odeiv_evolve_free(gsl_odeiv_evolve *);


__END_DECLS

#endif /* __GSL_ODEIV_H__ */
