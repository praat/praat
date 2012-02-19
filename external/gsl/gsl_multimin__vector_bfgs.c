/* multimin/vector_bfgs.c
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

/* vector_bfgs.c -- Limited memory Broyden-Fletcher-Goldfarb-Shanno method */

/* Modified by Brian Gough to use single iteration structure */

#include "gsl__config.h"
#include "gsl_multimin.h"
#include "gsl_blas.h"

#include "gsl_multimin__directional_minimize.c"

typedef struct
{
  int iter;
  double step;
  double max_step;
  double tol;
  gsl_vector *x1;
  gsl_vector *dx1;
  gsl_vector *x2;
  double g0norm;
  double pnorm;
  gsl_vector *p;
  gsl_vector *x0;
  gsl_vector *g0;
  gsl_vector *dx0;
  gsl_vector *dg0;
}
vector_bfgs_state_t;

static int
vector_bfgs_alloc (void *vstate, size_t n)
{
  vector_bfgs_state_t *state = (vector_bfgs_state_t *) vstate;

  state->x1 = gsl_vector_calloc (n);

  if (state->x1 == 0)
    {
      GSL_ERROR ("failed to allocate space for x1", GSL_ENOMEM);
    }

  state->dx1 = gsl_vector_calloc (n);

  if (state->dx1 == 0)
    {
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for dx1", GSL_ENOMEM);
    }

  state->x2 = gsl_vector_calloc (n);

  if (state->x2 == 0)
    {
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for x2", GSL_ENOMEM);
    }

  state->p = gsl_vector_calloc (n);

  if (state->p == 0)
    {
      gsl_vector_free (state->x2);
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for p", GSL_ENOMEM);
    }

  state->x0 = gsl_vector_calloc (n);

  if (state->x0 == 0)
    {
      gsl_vector_free (state->p);
      gsl_vector_free (state->x2);
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->g0 = gsl_vector_calloc (n);

  if (state->g0 == 0)
    {
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      gsl_vector_free (state->x2);
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->dx0 = gsl_vector_calloc (n);

  if (state->dx0 == 0)
    {
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      gsl_vector_free (state->x2);
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  state->dg0 = gsl_vector_calloc (n);

  if (state->dg0 == 0)
    {
      gsl_vector_free (state->dx0);
      gsl_vector_free (state->g0);
      gsl_vector_free (state->x0);
      gsl_vector_free (state->p);
      gsl_vector_free (state->x2);
      gsl_vector_free (state->dx1);
      gsl_vector_free (state->x1);
      GSL_ERROR ("failed to allocate space for g0", GSL_ENOMEM);
    }

  return GSL_SUCCESS;
}

static int
vector_bfgs_set (void *vstate, gsl_multimin_function_fdf * fdf,
                 const gsl_vector * x, double *f, gsl_vector * gradient,
                 double step_size, double tol)
{
  vector_bfgs_state_t *state = (vector_bfgs_state_t *) vstate;

  state->iter = 0;
  state->step = step_size;
  state->max_step = step_size;
  state->tol = tol;

  GSL_MULTIMIN_FN_EVAL_F_DF (fdf, x, f, gradient);

  /* Use the gradient as the initial direction */

  gsl_vector_memcpy (state->x0, x);
  gsl_vector_memcpy (state->p, gradient);
  gsl_vector_memcpy (state->g0, gradient);

  {
    double gnorm = gsl_blas_dnrm2 (gradient);
    state->pnorm = gnorm;
    state->g0norm = gnorm;
  }

  return GSL_SUCCESS;
}

static void
vector_bfgs_free (void *vstate)
{
  vector_bfgs_state_t *state = (vector_bfgs_state_t *) vstate;

  gsl_vector_free (state->dg0);
  gsl_vector_free (state->dx0);
  gsl_vector_free (state->g0);
  gsl_vector_free (state->x0);
  gsl_vector_free (state->p);
  gsl_vector_free (state->x2);
  gsl_vector_free (state->dx1);
  gsl_vector_free (state->x1);
}

static int
vector_bfgs_restart (void *vstate)
{
  vector_bfgs_state_t *state = (vector_bfgs_state_t *) vstate;

  state->iter = 0;
  return GSL_SUCCESS;
}

static int
vector_bfgs_iterate (void *vstate, gsl_multimin_function_fdf * fdf,
                     gsl_vector * x, double *f,
                     gsl_vector * gradient, gsl_vector * dx)
{
  vector_bfgs_state_t *state = (vector_bfgs_state_t *) vstate;

  gsl_vector *x1 = state->x1;
  gsl_vector *dx1 = state->dx1;
  gsl_vector *x2 = state->x2;
  gsl_vector *p = state->p;
  gsl_vector *g0 = state->g0;
  gsl_vector *x0 = state->x0;

  double pnorm = state->pnorm;
  double g0norm = state->g0norm;

  double fa = *f, fb, fc;
  double dir;
  double stepa = 0.0, stepb, stepc = state->step, tol = state->tol;

  double g1norm;
  double pg;

  if (pnorm == 0.0 || g0norm == 0.0)
    {
      gsl_vector_set_zero (dx);
      return GSL_ENOPROG;
    }

  /* Determine which direction is downhill, +p or -p */

  gsl_blas_ddot (p, gradient, &pg);

  dir = (pg >= 0.0) ? +1.0 : -1.0;

  /* Compute new trial point at x_c= x - step * p, where p is the
     current direction */

  take_step (x, p, stepc, dir / pnorm, x1, dx);

  /* Evaluate function and gradient at new point xc */

  fc = GSL_MULTIMIN_FN_EVAL_F (fdf, x1);

  if (fc < fa)
    {
      /* Success, reduced the function value */
      state->step = stepc * 2.0;
      *f = fc;
      gsl_vector_memcpy (x, x1);
      GSL_MULTIMIN_FN_EVAL_DF (fdf, x1, gradient);
      return GSL_SUCCESS;
    }

#ifdef DEBUG
  printf ("got stepc = %g fc = %g\n", stepc, fc);
#endif

  /* Do a line minimisation in the region (xa,fa) (xc,fc) to find an
     intermediate (xb,fb) satisifying fa > fb < fc.  Choose an initial
     xb based on parabolic interpolation */

  intermediate_point (fdf, x, p, dir / pnorm, pg,
                      stepa, stepc, fa, fc, x1, dx1, gradient, &stepb, &fb);

  if (stepb == 0.0)
    {
      return GSL_ENOPROG;
    }

  minimize (fdf, x, p, dir / pnorm,
            stepa, stepb, stepc, fa, fb, fc, tol,
            x1, dx1, x2, dx, gradient, &(state->step), f, &g1norm);

  gsl_vector_memcpy (x, x2);

  /* Choose a new direction for the next step */

  state->iter = (state->iter + 1) % x->size;

  if (state->iter == 0)
    {
      gsl_vector_memcpy (p, gradient);
      state->pnorm = g1norm;
    }
  else
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

      state->pnorm = gsl_blas_dnrm2 (p);
    }

  gsl_vector_memcpy (g0, gradient);
  gsl_vector_memcpy (x0, x);
  state->g0norm = gsl_blas_dnrm2 (g0);

#ifdef DEBUG
  printf ("updated directions\n");
  printf ("p: ");
  gsl_vector_fprintf (stdout, p, "%g");
  printf ("g: ");
  gsl_vector_fprintf (stdout, gradient, "%g");
#endif

  return GSL_SUCCESS;
}

static const gsl_multimin_fdfminimizer_type vector_bfgs_type = {
  "vector_bfgs",                /* name */
  sizeof (vector_bfgs_state_t),
  &vector_bfgs_alloc,
  &vector_bfgs_set,
  &vector_bfgs_iterate,
  &vector_bfgs_restart,
  &vector_bfgs_free
};

const gsl_multimin_fdfminimizer_type
  * gsl_multimin_fdfminimizer_vector_bfgs = &vector_bfgs_type;
