/* multfit/lmder.c
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
#include "gsl_multifit_nlin.h"
#include "gsl_blas.h"
#include "gsl_linalg.h"
#include "gsl_permutation.h"


typedef struct
  {
    size_t iter;
    double xnorm;
    double fnorm;
    double delta;
    double par;
    gsl_matrix *r;
    gsl_vector *tau;
    gsl_vector *diag;
    gsl_vector *qtf;
    gsl_vector *newton;
    gsl_vector *gradient;
    gsl_vector *x_trial;
    gsl_vector *f_trial;
    gsl_vector *df;
    gsl_vector *sdiag;
    gsl_vector *rptdx;
    gsl_vector *w;
    gsl_vector *work1;
    gsl_permutation * perm;
  }
lmder_state_t;

static int lmder_alloc (void *vstate, size_t n, size_t p);
static int lmder_set (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
static int lmsder_set (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
static int set (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx, int scale);
static int lmder_iterate (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
static void lmder_free (void *vstate);
static int iterate (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx, int scale);

#include "gsl_multifit__lmutil.c"
#include "gsl_multifit__lmpar.c"
#include "gsl_multifit__lmset.c"
#include "gsl_multifit__lmiterate.c"


static int
lmder_alloc (void *vstate, size_t n, size_t p)
{
  lmder_state_t *state = (lmder_state_t *) vstate;
  gsl_matrix *r;
  gsl_vector *tau, *diag, *qtf, *newton, *gradient, *x_trial, *f_trial,
   *df, *sdiag, *rptdx, *w, *work1;
  gsl_permutation *perm;

  r = gsl_matrix_calloc (n, p);

  if (r == 0)
    {
      GSL_ERROR ("failed to allocate space for r", GSL_ENOMEM);
    }

  state->r = r;

  tau = gsl_vector_calloc (GSL_MIN(n, p));

  if (tau == 0)
    {
      gsl_matrix_free (r);

      GSL_ERROR ("failed to allocate space for tau", GSL_ENOMEM);
    }

  state->tau = tau;

  diag = gsl_vector_calloc (p);

  if (diag == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);

      GSL_ERROR ("failed to allocate space for diag", GSL_ENOMEM);
    }

  state->diag = diag;

  qtf = gsl_vector_calloc (n);

  if (qtf == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);

      GSL_ERROR ("failed to allocate space for qtf", GSL_ENOMEM);
    }

  state->qtf = qtf;

  newton = gsl_vector_calloc (p);

  if (newton == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);

      GSL_ERROR ("failed to allocate space for newton", GSL_ENOMEM);
    }

  state->newton = newton;

  gradient = gsl_vector_calloc (p);

  if (gradient == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);

      GSL_ERROR ("failed to allocate space for gradient", GSL_ENOMEM);
    }

  state->gradient = gradient;

  x_trial = gsl_vector_calloc (p);

  if (x_trial == 0)
    {
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

  sdiag = gsl_vector_calloc (p);

  if (sdiag == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);

      GSL_ERROR ("failed to allocate space for sdiag", GSL_ENOMEM);
    }

  state->sdiag = sdiag;


  rptdx = gsl_vector_calloc (n);

  if (rptdx == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (sdiag);

      GSL_ERROR ("failed to allocate space for rptdx", GSL_ENOMEM);
    }

  state->rptdx = rptdx;

  w = gsl_vector_calloc (n);

  if (w == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (sdiag);
      gsl_vector_free (rptdx);

      GSL_ERROR ("failed to allocate space for w", GSL_ENOMEM);
    }

  state->w = w;

  work1 = gsl_vector_calloc (p);

  if (work1 == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (sdiag);
      gsl_vector_free (rptdx);
      gsl_vector_free (w);

      GSL_ERROR ("failed to allocate space for work1", GSL_ENOMEM);
    }

  state->work1 = work1;

  perm = gsl_permutation_calloc (p);

  if (perm == 0)
    {
      gsl_matrix_free (r);
      gsl_vector_free (tau);
      gsl_vector_free (diag);
      gsl_vector_free (qtf);
      gsl_vector_free (newton);
      gsl_vector_free (gradient);
      gsl_vector_free (x_trial);
      gsl_vector_free (f_trial);
      gsl_vector_free (df);
      gsl_vector_free (sdiag);
      gsl_vector_free (rptdx);
      gsl_vector_free (w);
      gsl_vector_free (work1);

      GSL_ERROR ("failed to allocate space for perm", GSL_ENOMEM);
    }

  state->perm = perm;

  return GSL_SUCCESS;
}

static int
lmder_set (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  int status = set (vstate, fdf, x, f, J, dx, 0);
  return status ;
}

static int
lmsder_set (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  int status = set (vstate, fdf, x, f, J, dx, 1);
  return status ;
}

static int
lmder_iterate (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  int status = iterate (vstate, fdf, x, f, J, dx, 0);
  return status;
}

static int
lmsder_iterate (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  int status = iterate (vstate, fdf, x, f, J, dx, 1);
  return status;
}

static void
lmder_free (void *vstate)
{
  lmder_state_t *state = (lmder_state_t *) vstate;

  gsl_permutation_free (state->perm);
  gsl_vector_free (state->work1);
  gsl_vector_free (state->w);
  gsl_vector_free (state->rptdx);
  gsl_vector_free (state->sdiag);
  gsl_vector_free (state->df);
  gsl_vector_free (state->f_trial);
  gsl_vector_free (state->x_trial);
  gsl_vector_free (state->gradient);
  gsl_vector_free (state->newton);
  gsl_vector_free (state->qtf);
  gsl_vector_free (state->diag);
  gsl_vector_free (state->tau);
  gsl_matrix_free (state->r);
}

static const gsl_multifit_fdfsolver_type lmder_type =
{
  "lmder",                      /* name */
  sizeof (lmder_state_t),
  &lmder_alloc,
  &lmder_set,
  &lmder_iterate,
  &lmder_free
};

static const gsl_multifit_fdfsolver_type lmsder_type =
{
  "lmsder",                     /* name */
  sizeof (lmder_state_t),
  &lmder_alloc,
  &lmsder_set,
  &lmsder_iterate,
  &lmder_free
};

const gsl_multifit_fdfsolver_type *gsl_multifit_fdfsolver_lmder = &lmder_type;
const gsl_multifit_fdfsolver_type *gsl_multifit_fdfsolver_lmsder = &lmsder_type;
