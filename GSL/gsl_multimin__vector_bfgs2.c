/* multimin/vector_bfgs2.c
 * 
 * Copyright (C) 2007 Brian Gough
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/* vector_bfgs2.c -- Fletcher's implementation of the BFGS method,
   from R.Fletcher, "Practical Method's of Optimization", Second
   Edition, ISBN 0471915475.  Algorithms 2.6.2 and 2.6.4. */

/* Thanks to Alan Irwin irwin@beluga.phys.uvic.ca. for suggesting this
   algorithm and providing sample fortran benchmarks */

#include "gsl__config.h"
#include "gsl_multimin.h"
#include "gsl_blas.h"

#include "gsl_multimin__linear_minimize.c"
#include "gsl_multimin__linear_wrapper.c"

typedef struct
{
  int iter;
  double step;
  double g0norm;
  double pnorm;
  double delta_f;
  double fp0;                   /* f'(0) for f(x-alpha*p) */
  gsl_vector *x0;
  gsl_vector *g0;
  gsl_vector *p;
  /* work space */
  gsl_vector *dx0;
  gsl_vector *dg0;
  gsl_vector *x_alpha;
  gsl_vector *g_alpha;
  /* wrapper function */
  wrapper_t wrap;
  /* minimization parameters */
  double rho;
  double sigma;
  double tau1;
  double tau2;
  double tau3;
  int order;
}
vector_bfgs2_state_t;

static int
vector_bfgs2_alloc (void *vstate, size_t n)
{
  vector_bfgs2_state_t *state = (vector_bfgs2_state_t *) vstate;

  state->p = gsl_vector_calloc (n);

  if (state->p == 0)
    {
      GSL_ERROR ("failed to allocate space for p", GSL_ENOMEM);
    }

  state->x0 = gsl_vector_calloc (n);

  if (state->x0 == 0)
    {
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->g0 = gsl_vector_calloc (n);

  if (state->g0 == 0)
    {
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->dx0 = gsl_vector_calloc (n);

  if (state->dx0 == 0)
    {
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->dg0 = gsl_vector_calloc (n);

  if (state->dg0 == 0)
    {
      gsl_vector_free (state->dx0);
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->x_alpha = gsl_vector_calloc (n);

  if (state->x_alpha == 0)
    {
      gsl_vector_free (state->dg0);
      gsl_vector_free (state->dx0);
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->g_alpha = gsl_vector_calloc (n);

  if (state->g_alpha == 0)
    {
      gsl_vector_free (state->x_alpha);
      gsl_vector_free (state->dg0);
      gsl_vector_free (state->dx0);
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  return GSL_SUCCESS;
}

static int
vector_bfgs2_set (void *vstate, gsl_multimin_function_fdf * fdf,
                  const gsl_vector * x, double *f, gsl_vector * gradient,
                  double step_size, double tol)
{
  vector_bfgs2_state_t *state = (vector_bfgs2_state_t *) vstate;

  state->iter = 0;
  state->step = step_size;
  state->delta_f = 0;

  GSL_MULTIMIN_FN_EVAL_F_DF (fdf, x, f, gradient);

  /* Use the gradient as the initial direction */

  gsl_vector_memcpy (state->x0, x);
  gsl_vector_memcpy (state->g0, gradient);
  state->g0norm = gsl_blas_dnrm2 (state->g0);

  gsl_vector_memcpy (state->p, gradient);
  gsl_blas_dscal (-1 / state->g0norm, state->p);
  state->pnorm = gsl_blas_dnrm2 (state->p);     /* should be 1 */
  state->fp0 = -state->g0norm;

  /* Prepare the wrapper */

  prepare_wrapper (&state->wrap, fdf,
                   state->x0, *f, state->g0,
                   state->p, state->x_alpha, state->g_alpha);

  /* Prepare 1d minimisation parameters */

  state->rho = 0.01;
  state->sigma = tol;
  state->tau1 = 9;
  state->tau2 = 0.05;
  state->tau3 = 0.5;
  state->order = 3;  /* use cubic interpolation where possible */

  return GSL_SUCCESS;
}

static void
vector_bfgs2_free (void *vstate)
{
  vector_bfgs2_state_t *state = (vector_bfgs2_state_t *) vstate;

  gsl_vector_free (state->x_alpha);
  gsl_vector_free (state->g_alpha);
  gsl_vector_free (state->dg0);
  gsl_vector_free (state->dx0);
  gsl_vector_free (state->g0);
  gsl_vector_free (state->x0);
  gsl_vector_free (state->p);
}

static int
vector_bfgs2_restart (void *vstate)
{
  vector_bfgs2_state_t *state = (vector_bfgs2_state_t *) vstate;

  state->iter = 0;
  return GSL_SUCCESS;
}

static int
vector_bfgs2_iterate (void *vstate, gsl_multimin_function_fdf * fdf,
                      gsl_vector * x, double *f,
                      gsl_vector * gradient, gsl_vector * dx)
{
  vector_bfgs2_state_t *state = (vector_bfgs2_state_t *) vstate;
  double alpha = 0.0, alpha1;
  gsl_vector *x0 = state->x0;
  gsl_vector *g0 = state->g0;
  gsl_vector *p = state->p;

  double g0norm = state->g0norm;
  double pnorm = state->pnorm;
  double delta_f = state->delta_f;
  double pg, dir;
  int status;

  double f0 = *f;

  if (pnorm == 0.0 || g0norm == 0.0 || state->fp0 == 0)
    {
      gsl_vector_set_zero (dx);
      return GSL_ENOPROG;
    }

  if (delta_f < 0)
    {
      double del = GSL_MAX_DBL (-delta_f, 10 * GSL_DBL_EPSILON * fabs(f0));
      alpha1 = GSL_MIN_DBL (1.0, 2.0 * del / (-state->fp0));
    }
  else
    {
      alpha1 = fabs(state->step);
    }

  /* line minimisation, with cubic interpolation (order = 3) */

  status = minimize (&state->wrap.fdf_linear, state->rho, state->sigma, 
                     state->tau1, state->tau2, state->tau3, state->order,
                     alpha1,  &alpha);

  if (status != GSL_SUCCESS)
    {
      return status;
    }

  update_position (&(state->wrap), alpha, x, f, gradient);
  
  state->delta_f = *f - f0;

  /* Choose a new direction for the next step */

  {
    /* This is the BFGS update: */
    /* p' = g1 - A dx - B dg */
    /* A = - (1+ dg.dg/dx.dg) B + dg.g/dx.dg */
    /* B = dx.g/dx.dg */

    gsl_vector *dx0 = state->dx0;
    gsl_vector *dg0 = state->dg0;

    double dxg, dgg, dxdg, dgnorm, A, B;

    /* dx0 = x - x0 */
    gsl_vector_memcpy (dx0, x);
    gsl_blas_daxpy (-1.0, x0, dx0);

    gsl_vector_memcpy (dx, dx0);  /* keep a copy */

    /* dg0 = g - g0 */
    gsl_vector_memcpy (dg0, gradient);
    gsl_blas_daxpy (-1.0, g0, dg0);

    gsl_blas_ddot (dx0, gradient, &dxg);
    gsl_blas_ddot (dg0, gradient, &dgg);
    gsl_blas_ddot (dx0, dg0, &dxdg);

    dgnorm = gsl_blas_dnrm2 (dg0);

    if (dxdg != 0)
      {
        B = dxg / dxdg;
        A = -(1.0 + dgnorm * dgnorm / dxdg) * B + dgg / dxdg;
      }
    else
      {
        B = 0;
        A = 0;
      }

    gsl_vector_memcpy (p, gradient);
    gsl_blas_daxpy (-A, dx0, p);
    gsl_blas_daxpy (-B, dg0, p);
  }

  gsl_vector_memcpy (g0, gradient);
  gsl_vector_memcpy (x0, x);
  state->g0norm = gsl_blas_dnrm2 (g0);
  state->pnorm = gsl_blas_dnrm2 (p);

  /* update direction and fp0 */

  gsl_blas_ddot (p, gradient, &pg);
  dir = (pg >= 0.0) ? -1.0 : +1.0;
  gsl_blas_dscal (dir / state->pnorm, p);
  state->pnorm = gsl_blas_dnrm2 (p);
  gsl_blas_ddot (p, g0, &state->fp0);

  change_direction (&state->wrap);

  return GSL_SUCCESS;
}

static const gsl_multimin_fdfminimizer_type vector_bfgs2_type = {
  "vector_bfgs2",               /* name */
  sizeof (vector_bfgs2_state_t),
  &vector_bfgs2_alloc,
  &vector_bfgs2_set,
  &vector_bfgs2_iterate,
  &vector_bfgs2_restart,
  &vector_bfgs2_free
};

const gsl_multimin_fdfminimizer_type
  * gsl_multimin_fdfminimizer_vector_bfgs2 = &vector_bfgs2_type;
