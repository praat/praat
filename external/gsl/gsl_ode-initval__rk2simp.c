/* ode-initval/rk2simp.c
 * 
 * Copyright (C) 2004 Tuomo Keskitalo
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

/* Runge-Kutta 2, Gaussian implicit. Also known as implicit midpoint rule.

   Non-linear equations solved by linearization, LU-decomposition
   and matrix inversion. For reference, see eg.

   Ascher, U.M., Petzold, L.R., Computer methods for ordinary
   differential and differential-algebraic equations, SIAM,
   Philadelphia, 1998.
 */

#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_odeiv.h"
#include "gsl_linalg.h"

#include "gsl_ode-initval__odeiv_util.h"

typedef struct
{
  double *Y1;
  double *y0;
  double *y0_orig;
  double *ytmp;
  double *dfdy;                 /* Jacobian */
  double *dfdt;                 /* time derivatives, not used */
  double *y_onestep;
  gsl_permutation *p;
}
rk2simp_state_t;

static void *
rk2simp_alloc (size_t dim)
{
  rk2simp_state_t *state =
    (rk2simp_state_t *) malloc (sizeof (rk2simp_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk2simp_state",
                      GSL_ENOMEM);
    }

  state->Y1 = (double *) malloc (dim * sizeof (double));

  if (state->Y1 == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for Y1", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->Y1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  state->y0_orig = (double *) malloc (dim * sizeof (double));

  if (state->y0_orig == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0_orig", GSL_ENOMEM);
    }

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state->y0_orig);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp", GSL_ENOMEM);
    }

  state->dfdy = (double *) malloc (dim * dim * sizeof (double));

  if (state->dfdy == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state->y0_orig);
      free (state->ytmp);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for dfdy", GSL_ENOMEM);
    }

  state->dfdt = (double *) malloc (dim * sizeof (double));

  if (state->dfdt == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state->y0_orig);
      free (state->ytmp);
      free (state->dfdy);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for dfdt", GSL_ENOMEM);
    }

  state->y_onestep = (double *) malloc (dim * sizeof (double));

  if (state->y_onestep == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state->y0_orig);
      free (state->ytmp);
      free (state->dfdy);
      free (state->dfdt);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y_onestep", GSL_ENOMEM);
    }

  state->p = gsl_permutation_alloc (dim);

  if (state->p == 0)
    {
      free (state->Y1);
      free (state->y0);
      free (state->y0_orig);
      free (state->ytmp);
      free (state->dfdy);
      free (state->dfdt);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for p", GSL_ENOMEM);
    }

  return state;
}


static int
rk2simp_step (double *y, rk2simp_state_t * state,
              const double h, const double t,
              const size_t dim, const gsl_odeiv_system * sys)
{
  /* Makes a Runge-Kutta 2nd order semi-implicit advance with step size h.
     y0 is initial values of variables y. 

     The linearized semi-implicit equations to calculate are:

     Y1 = y0 + h/2 * (1 - h/2 * df/dy)^(-1) * f(t + h/2, y0)

     y = y0 + h * f(t + h/2, Y1)
   */

  const double *y0 = state->y0;
  double *Y1 = state->Y1;
  double *ytmp = state->ytmp;

  size_t i;
  int s, ps;

  gsl_matrix_view J = gsl_matrix_view_array (state->dfdy, dim, dim);

  /* First solve Y1. 
     Calculate the inverse matrix (1 - h/2 * df/dy)^-1 
   */

  /* Create matrix to J */

  s = GSL_ODEIV_JA_EVAL (sys, t, y0, state->dfdy, state->dfdt);

  if (s != GSL_SUCCESS)
    {
      return s;
    }

  gsl_matrix_scale (&J.matrix, -h / 2.0);
  gsl_matrix_add_diagonal(&J.matrix, 1.0);

  /* Invert it by LU-decomposition to invmat */

  s += gsl_linalg_LU_decomp (&J.matrix, state->p, &ps);

  if (s != GSL_SUCCESS)
    {
      return GSL_EFAILED;
    }

  /* Evaluate f(t + h/2, y0) */

  s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, y0, ytmp);

  if (s != GSL_SUCCESS)
    {
      return s;
    }

  /* Calculate Y1 = y0 + h/2 * ((1-h/2 * df/dy)^-1) ytmp */

  {
    gsl_vector_const_view y0_view = gsl_vector_const_view_array(y0, dim);
    gsl_vector_view ytmp_view = gsl_vector_view_array(ytmp, dim);
    gsl_vector_view Y1_view = gsl_vector_view_array(Y1, dim);

    s = gsl_linalg_LU_solve (&J.matrix, state->p, 
                             &ytmp_view.vector, &Y1_view.vector);
      
    gsl_vector_scale (&Y1_view.vector, 0.5 * h);
    gsl_vector_add (&Y1_view.vector, &y0_view.vector);
  }

  /* And finally evaluation of f(t + h/2, Y1) and calculation of y */

  s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, Y1, ytmp);

  if (s != GSL_SUCCESS)
    {
      return s;
    }

  for (i = 0; i < dim; i++)
    {
      y[i] = y0[i] + h * ytmp[i];
    }

  return s;
}

static int
rk2simp_apply (void *vstate, size_t dim, double t, double h,
               double y[], double yerr[], const double dydt_in[],
               double dydt_out[], const gsl_odeiv_system * sys)
{
  rk2simp_state_t *state = (rk2simp_state_t *) vstate;

  size_t i;

  double *y0 = state->y0;
  double *y0_orig = state->y0_orig;
  double *y_onestep = state->y_onestep;

  /* Error estimation is done by step doubling procedure */

  DBL_MEMCPY (y0, y, dim);

  /* Save initial values in case of failure */
  DBL_MEMCPY (y0_orig, y, dim);

  /* First traverse h with one step (save to y_onestep) */
  DBL_MEMCPY (y_onestep, y, dim);

  {
    int s = rk2simp_step (y_onestep, state, h, t, dim, sys);

    if (s != GSL_SUCCESS)
      {
        return s;
      }
  }

  /* Then with two steps with half step length (save to y) */

  {
    int s = rk2simp_step (y, state, h / 2.0, t, dim, sys);

    if (s != GSL_SUCCESS)
      {
        /* Restore original y vector */
        DBL_MEMCPY (y, y0_orig, dim);
        return s;
      }
  }

  DBL_MEMCPY (y0, y, dim);

  {
    int s = rk2simp_step (y, state, h / 2.0, t + h / 2.0, dim, sys);

    if (s != GSL_SUCCESS)
      {
        /* Restore original y vector */
        DBL_MEMCPY (y, y0_orig, dim);
        return s;
      }
  }

  /* Derivatives at output */

  if (dydt_out != NULL)
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t + h, y, dydt_out);

      if (s != GSL_SUCCESS)
        {
          /* Restore original y vector */
          DBL_MEMCPY (y, y0_orig, dim);
          return s;
        }
    }

  /* Error estimation */

  for (i = 0; i < dim; i++)
    {
      yerr[i] = 4.0 * (y[i] - y_onestep[i]) / 3.0;
    }

  return GSL_SUCCESS;
}


static int
rk2simp_reset (void *vstate, size_t dim)
{
  rk2simp_state_t *state = (rk2simp_state_t *) vstate;

  DBL_ZERO_MEMSET (state->Y1, dim);
  DBL_ZERO_MEMSET (state->y0, dim);
  DBL_ZERO_MEMSET (state->y0_orig, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);
  DBL_ZERO_MEMSET (state->dfdt, dim * dim);
  DBL_ZERO_MEMSET (state->dfdt, dim);
  DBL_ZERO_MEMSET (state->y_onestep, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk2simp_order (void *vstate)
{
  return 2;
}

static void
rk2simp_free (void *vstate)
{
  rk2simp_state_t *state = (rk2simp_state_t *) vstate;
  free (state->Y1);
  free (state->y0);
  free (state->y0_orig);
  free (state->ytmp);
  free (state->dfdy);
  free (state->dfdt);
  free (state->y_onestep);
  gsl_permutation_free (state->p);
  free (state);
}

static const gsl_odeiv_step_type rk2simp_type = {
  "rk2simp",                    /* name */
  0,                            /* can use dydt_in? */
  1,                            /* gives exact dydt_out? */
  &rk2simp_alloc,
  &rk2simp_apply,
  &rk2simp_reset,
  &rk2simp_order,
  &rk2simp_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk2simp = &rk2simp_type;
