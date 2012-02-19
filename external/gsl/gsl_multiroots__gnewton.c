/* multiroots/gnewton.c
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

#include "gsl_multiroots__enorm.c"

/* Simple globally convergent Newton method (rejects uphill steps) */

typedef struct
  {
    double phi;
    gsl_vector * x_trial;
    gsl_vector * d;
    gsl_matrix * lu;
    gsl_permutation * permutation;
  }
gnewton_state_t;

static int gnewton_alloc (void * vstate, size_t n);
static int gnewton_set (void * vstate, gsl_multiroot_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
static int gnewton_iterate (void * vstate, gsl_multiroot_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx);
static void gnewton_free (void * vstate);

static int
gnewton_alloc (void * vstate, size_t n)
{
  gnewton_state_t * state = (gnewton_state_t *) vstate;
  gsl_vector * d, * x_trial ;
  gsl_permutation * p;
  gsl_matrix * m;

  m = gsl_matrix_calloc (n,n);
  
  if (m == 0) 
    {
      GSL_ERROR ("failed to allocate space for lu", GSL_ENOMEM);
    }

  state->lu = m ;

  p = gsl_permutation_calloc (n);

  if (p == 0)
    {
      gsl_matrix_free(m);

      GSL_ERROR ("failed to allocate space for permutation", GSL_ENOMEM);
    }

  state->permutation = p ;

  d = gsl_vector_calloc (n);

  if (d == 0)
    {
      gsl_permutation_free(p);
      gsl_matrix_free(m);

      GSL_ERROR ("failed to allocate space for d", GSL_ENOMEM);
    }

  state->d = d;

  x_trial = gsl_vector_calloc (n);

  if (x_trial == 0)
    {
      gsl_vector_free(d);
      gsl_permutation_free(p);
      gsl_matrix_free(m);

      GSL_ERROR ("failed to allocate space for x_trial", GSL_ENOMEM);
    }

  state->x_trial = x_trial;

  return GSL_SUCCESS;
}


static int
gnewton_set (void * vstate, gsl_multiroot_function_fdf * FDF, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  gnewton_state_t * state = (gnewton_state_t *) vstate;
  size_t i, n = FDF->n ;

  GSL_MULTIROOT_FN_EVAL_F_DF (FDF, x, f, J);

  for (i = 0; i < n; i++)
    {
      gsl_vector_set (dx, i, 0.0);
    }

  state->phi = enorm(f);

  return GSL_SUCCESS;
}

static int
gnewton_iterate (void * vstate, gsl_multiroot_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx)
{
  gnewton_state_t * state = (gnewton_state_t *) vstate;
  
  int signum ;
  double t, phi0, phi1;

  size_t i;

  size_t n = fdf->n ;

  gsl_matrix_memcpy (state->lu, J);

  gsl_linalg_LU_decomp (state->lu, state->permutation, &signum);

  gsl_linalg_LU_solve (state->lu, state->permutation, f, state->d);

  t = 1;

  phi0 = state->phi;

new_step:

  for (i = 0; i < n; i++)
    {
      double di = gsl_vector_get (state->d, i);
      double xi = gsl_vector_get (x, i);
      gsl_vector_set (state->x_trial, i, xi - t*di);
    }
  
  { 
    int status = GSL_MULTIROOT_FN_EVAL_F (fdf, state->x_trial, f);
    
    if (status != GSL_SUCCESS)
      {
        return GSL_EBADFUNC;
      }
  }
  
  phi1 = enorm (f);

  if (phi1 > phi0 && t > GSL_DBL_EPSILON)  
    {
      /* full step goes uphill, take a reduced step instead */

      double theta = phi1 / phi0;
      double u = (sqrt(1.0 + 6.0 * theta) - 1.0) / (3.0 * theta);

      t *= u ;
     
      goto new_step;
    }

  /* copy x_trial into x */

  gsl_vector_memcpy (x, state->x_trial);

  for (i = 0; i < n; i++)
    {
      double di = gsl_vector_get (state->d, i);
      gsl_vector_set (dx, i, -t*di);
    }

  { 
    int status = GSL_MULTIROOT_FN_EVAL_DF (fdf, x, J);
    
    if (status != GSL_SUCCESS)
      {
        return GSL_EBADFUNC;
      }
  }

  state->phi = phi1;

  return GSL_SUCCESS;
}


static void
gnewton_free (void * vstate)
{
  gnewton_state_t * state = (gnewton_state_t *) vstate;

  gsl_vector_free(state->d);
  gsl_vector_free(state->x_trial);
  gsl_matrix_free(state->lu);
  gsl_permutation_free(state->permutation);
}


static const gsl_multiroot_fdfsolver_type gnewton_type =
{"gnewton",                             /* name */
 sizeof (gnewton_state_t),
 &gnewton_alloc,
 &gnewton_set,
 &gnewton_iterate,
 &gnewton_free};

const gsl_multiroot_fdfsolver_type  * gsl_multiroot_fdfsolver_gnewton = &gnewton_type;
