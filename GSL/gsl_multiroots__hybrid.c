/* multiroots/hybrid.c
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

#include "gsl__config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_multiroots.h"
#include "gsl_linalg.h"

#include "gsl_multiroots__dogleg.c"

typedef struct
{
  size_t iter;
  size_t ncfail;
  size_t ncsuc;
  size_t nslow1;
  size_t nslow2;
  double fnorm;
  double delta;
  gsl_matrix *J;
  gsl_matrix *q;
  gsl_matrix *r;
  gsl_vector *tau;
  gsl_vector *diag;
  gsl_vector *qtf;
  gsl_vector *newton;
  gsl_vector *gradient;
  gsl_vector *x_trial;
  gsl_vector *f_trial;
  gsl_vector *df;
  gsl_vector *qtdf;
  gsl_vector *rdx;
  gsl_vector *w;
  gsl_vector *v;
}
hybrid_state_t;

static int hybrid_alloc (void *vstate, size_t n);
static int hybrid_set (void *vstate, gsl_multiroot_function * func,
                       gsl_vector * x, gsl_vector * f, gsl_vector * dx);
static int hybrids_set (void *vstate, gsl_multiroot_function * func,
                        gsl_vector * x, gsl_vector * f, gsl_vector * dx);
static int set (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
                gsl_vector * f, gsl_vector * dx, int scale);
static int hybrid_iterate (void *vstate, gsl_multiroot_function * func,
                           gsl_vector * x, gsl_vector * f, gsl_vector * dx);
static void hybrid_free (void *vstate);
static int iterate (void *vstate, gsl_multiroot_function * func,
                    gsl_vector * x, gsl_vector * f, gsl_vector * dx,
                    int scale);

static int
hybrid_alloc (void *vstate, size_t n)
{
  hybrid_state_t *state = (hybrid_state_t *) vstate;
  gsl_matrix *J, *q, *r;
  gsl_vector *tau, *diag, *qtf, *newton, *gradient, *x_trial, *f_trial,
    *df, *qtdf, *rdx, *w, *v;

  J = gsl_matrix_calloc (n, n);

  if (J == 0)
    {
      GSL_ERROR ("failed to allocate space for J", GSL_ENOMEM);
    }

  state->J = J;

  q = gsl_matrix_calloc (n, n);

  if (q == 0)
    {
      gsl_matrix_free (J);

      GSL_ERROR ("failed to allocate space for q", GSL_ENOMEM);
    }

  state->q = q;

  r = gsl_matrix_calloc (n, n);

  if (r == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);

      GSL_ERROR ("failed to allocate space for r", GSL_ENOMEM);
    }

  state->r = r;

  tau = gsl_vector_calloc (n);

  if (tau == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);

      GSL_ERROR ("failed to allocate space for tau", GSL_ENOMEM);
    }

  state->tau = tau;

  diag = gsl_vector_calloc (n);

  if (diag == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);

      GSL_ERROR ("failed to allocate space for diag", GSL_ENOMEM);
    }

  state->diag = diag;

  qtf = gsl_vector_calloc (n);

  if (qtf == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);

      GSL_ERROR ("failed to allocate space for qtf", GSL_ENOMEM);
    }

  state->qtf = qtf;

  newton = gsl_vector_calloc (n);

  if (newton == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);

      GSL_ERROR ("failed to allocate space for newton", GSL_ENOMEM);
    }

  state->newton = newton;

  gradient = gsl_vector_calloc (n);

  if (gradient == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);

      GSL_ERROR ("failed to allocate space for gradient", GSL_ENOMEM);
    }

  state->gradient = gradient;

  x_trial = gsl_vector_calloc (n);

  if (x_trial == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);

      GSL_ERROR ("failed to allocate space for x_trial", GSL_ENOMEM);
    }

  state->x_trial = x_trial;

  f_trial = gsl_vector_calloc (n);

  if (f_trial == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);

      GSL_ERROR ("failed to allocate space for f_trial", GSL_ENOMEM);
    }

  state->f_trial = f_trial;

  df = gsl_vector_calloc (n);

  if (df == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);

      GSL_ERROR ("failed to allocate space for df", GSL_ENOMEM);
    }

  state->df = df;

  qtdf = gsl_vector_calloc (n);

  if (qtdf == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);

      GSL_ERROR ("failed to allocate space for qtdf", GSL_ENOMEM);
    }

  state->qtdf = qtdf;


  rdx = gsl_vector_calloc (n);

  if (rdx == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (qtdf);

      GSL_ERROR ("failed to allocate space for rdx", GSL_ENOMEM);
    }

  state->rdx = rdx;

  w = gsl_vector_calloc (n);

  if (w == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (qtdf);
      gsl_vector_free (rdx);

      GSL_ERROR ("failed to allocate space for w", GSL_ENOMEM);
    }

  state->w = w;

  v = gsl_vector_calloc (n);

  if (v == 0)
    {
      gsl_matrix_free (J);
      gsl_matrix_free (q);
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (qtdf);
      gsl_vector_free (rdx);
      gsl_vector_free (w);

      GSL_ERROR ("failed to allocate space for v", GSL_ENOMEM);
    }

  state->v = v;

  return GSL_SUCCESS;
}

static int
hybrid_set (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
            gsl_vector * f, gsl_vector * dx)
{
  int status = set (vstate, func, x, f, dx, 0);
  return status;
}

static int
hybrids_set (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
             gsl_vector * f, gsl_vector * dx)
{
  int status = set (vstate, func, x, f, dx, 1);
  return status;
}

static int
set (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
     gsl_vector * f, gsl_vector * dx, int scale)
{
  hybrid_state_t *state = (hybrid_state_t *) vstate;

  gsl_matrix *J = state->J;
  gsl_matrix *q = state->q;
  gsl_matrix *r = state->r;
  gsl_vector *tau = state->tau;
  gsl_vector *diag = state->diag;

  int status;

  status = GSL_MULTIROOT_FN_EVAL (func, x, f);

  if (status)
    {
      return status;
    }

  status = gsl_multiroot_fdjacobian (func, x, f, GSL_SQRT_DBL_EPSILON, J);

  if (status)
    {
      return status;
    }

  state->iter = 1;
  state->fnorm = enorm (f);
  state->ncfail = 0;
  state->ncsuc = 0;
  state->nslow1 = 0;
  state->nslow2 = 0;

  gsl_vector_set_all (dx, 0.0);

  /* Store column norms in diag */

  if (scale)
    compute_diag (J, diag);
  else
    gsl_vector_set_all (diag, 1.0);

  /* Set delta to factor |D x| or to factor if |D x| is zero */

  state->delta = compute_delta (diag, x);

  /* Factorize J into QR decomposition */

  status = gsl_linalg_QR_decomp (J, tau);

  if (status)
    {
      return status;
    }

  status = gsl_linalg_QR_unpack (J, tau, q, r);

  return status;
}

static int
hybrid_iterate (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
                gsl_vector * f, gsl_vector * dx)
{
  int status = iterate (vstate, func, x, f, dx, 0);
  return status;
}

static int
hybrids_iterate (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
                 gsl_vector * f, gsl_vector * dx)
{
  int status = iterate (vstate, func, x, f, dx, 1);
  return status;
}

static int
iterate (void *vstate, gsl_multiroot_function * func, gsl_vector * x,
         gsl_vector * f, gsl_vector * dx, int scale)
{
  hybrid_state_t *state = (hybrid_state_t *) vstate;

  const double fnorm = state->fnorm;

  gsl_matrix *J = state->J;
  gsl_matrix *q = state->q;
  gsl_matrix *r = state->r;
  gsl_vector *tau = state->tau;
  gsl_vector *diag = state->diag;
  gsl_vector *qtf = state->qtf;
  gsl_vector *x_trial = state->x_trial;
  gsl_vector *f_trial = state->f_trial;
  gsl_vector *df = state->df;
  gsl_vector *qtdf = state->qtdf;
  gsl_vector *rdx = state->rdx;
  gsl_vector *w = state->w;
  gsl_vector *v = state->v;

  double prered, actred;
  double pnorm, fnorm1, fnorm1p;
  double ratio;
  double p1 = 0.1, p5 = 0.5, p001 = 0.001, p0001 = 0.0001;

  /* Compute qtf = Q^T f */

  compute_qtf (q, f, qtf);

  /* Compute dogleg step */

  dogleg (r, qtf, diag, state->delta, state->newton, state->gradient, dx);

  /* Take a trial step */

  compute_trial_step (x, dx, state->x_trial);

  pnorm = scaled_enorm (diag, dx);

  if (state->iter == 1)
    {
      if (pnorm < state->delta)
        {
          state->delta = pnorm;
        }
    }

  /* Evaluate function at x + p */

  {
    int status = GSL_MULTIROOT_FN_EVAL (func, x_trial, f_trial);

    if (status != GSL_SUCCESS)
      {
        return GSL_EBADFUNC;
      }
  }

  /* Set df = f_trial - f */

  compute_df (f_trial, f, df);

  /* Compute the scaled actual reduction */

  fnorm1 = enorm (f_trial);

  actred = compute_actual_reduction (fnorm, fnorm1);

  /* Compute rdx = R dx */

  compute_rdx (r, dx, rdx);

  /* Compute the scaled predicted reduction phi1p = |Q^T f + R dx| */

  fnorm1p = enorm_sum (qtf, rdx);

  prered = compute_predicted_reduction (fnorm, fnorm1p);

  /* Compute the ratio of the actual to predicted reduction */

  if (prered > 0)
    {
      ratio = actred / prered;
    }
  else
    {
      ratio = 0;
    }

  /* Update the step bound */

  if (ratio < p1)
    {
      state->ncsuc = 0;
      state->ncfail++;
      state->delta *= p5;
    }
  else
    {
      state->ncfail = 0;
      state->ncsuc++;

      if (ratio >= p5 || state->ncsuc > 1)
        state->delta = GSL_MAX (state->delta, pnorm / p5);
      if (fabs (ratio - 1) <= p1)
        state->delta = pnorm / p5;
    }

  /* Test for successful iteration */

  if (ratio >= p0001)
    {
      gsl_vector_memcpy (x, x_trial);
      gsl_vector_memcpy (f, f_trial);
      state->fnorm = fnorm1;
      state->iter++;
    }

  /* Determine the progress of the iteration */

  state->nslow1++;
  if (actred >= p001)
    state->nslow1 = 0;

  if (actred >= p1)
    state->nslow2 = 0;

  if (state->ncfail == 2)
    {
      gsl_multiroot_fdjacobian (func, x, f, GSL_SQRT_DBL_EPSILON, J);

      state->nslow2++;

      if (state->iter == 1)
        {
          if (scale)
            compute_diag (J, diag);
          state->delta = compute_delta (diag, x);
        }
      else
        {
          if (scale)
            update_diag (J, diag);
        }

      /* Factorize J into QR decomposition */

      gsl_linalg_QR_decomp (J, tau);
      gsl_linalg_QR_unpack (J, tau, q, r);

      return GSL_SUCCESS;
    }

  /* Compute qtdf = Q^T df, w = (Q^T df - R dx)/|dx|,  v = D^2 dx/|dx| */

  compute_qtf (q, df, qtdf);

  compute_wv (qtdf, rdx, dx, diag, pnorm, w, v);

  /* Rank-1 update of the jacobian Q'R' = Q(R + w v^T) */

  gsl_linalg_QR_update (q, r, w, v);

  /* No progress as measured by jacobian evaluations */

  if (state->nslow2 == 5)
    {
      return GSL_ENOPROGJ;
    }

  /* No progress as measured by function evaluations */

  if (state->nslow1 == 10)
    {
      return GSL_ENOPROG;
    }

  return GSL_SUCCESS;
}


static void
hybrid_free (void *vstate)
{
  hybrid_state_t *state = (hybrid_state_t *) vstate;

  gsl_vector_free (state->v);
  gsl_vector_free (state->w);
  gsl_vector_free (state->rdx);
  gsl_vector_free (state->qtdf);
  gsl_vector_free (state->df);
  gsl_vector_free (state->f_trial);
  gsl_vector_free (state->x_trial);
  gsl_vector_free (state->gradient);
  gsl_vector_free (state->newton);
  gsl_vector_free (state->qtf);
  gsl_vector_free (state->diag);
  gsl_vector_free (state->tau);
  gsl_matrix_free (state->r);
  gsl_matrix_free (state->q);
  gsl_matrix_free (state->J);
}

static const gsl_multiroot_fsolver_type hybrid_type = {
  "hybrid",                     /* name */
  sizeof (hybrid_state_t),
  &hybrid_alloc,
  &hybrid_set,
  &hybrid_iterate,
  &hybrid_free
};

static const gsl_multiroot_fsolver_type hybrids_type = {
  "hybrids",                    /* name */
  sizeof (hybrid_state_t),
  &hybrid_alloc,
  &hybrids_set,
  &hybrids_iterate,
  &hybrid_free
};

const gsl_multiroot_fsolver_type *gsl_multiroot_fsolver_hybrid = &hybrid_type;
const gsl_multiroot_fsolver_type *gsl_multiroot_fsolver_hybrids =
  &hybrids_type;
