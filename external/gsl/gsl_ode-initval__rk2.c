/* ode-initval/rk2.c
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

/* Runge-Kutta 2(3), Euler-Cauchy */

/* Author:  G. Jungman
 */

/* Reference: Abramowitz & Stegun, section 25.5. Runge-Kutta 2nd (25.5.7)
   and 3rd (25.5.8) order methods */

#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_errno.h"
#include "gsl_odeiv.h"

#include "gsl_ode-initval__odeiv_util.h"

typedef struct
{
  double *k1;
  double *k2;
  double *k3;
  double *ytmp;
}
rk2_state_t;

static void *
rk2_alloc (size_t dim)
{
  rk2_state_t *state = (rk2_state_t *) malloc (sizeof (rk2_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk2_state", GSL_ENOMEM);
    }

  state->k1 = (double *) malloc (dim * sizeof (double));

  if (state->k1 == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k1", GSL_ENOMEM);
    }

  state->k2 = (double *) malloc (dim * sizeof (double));

  if (state->k2 == 0)
    {
      free (state->k1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k2", GSL_ENOMEM);
    }

  state->k3 = (double *) malloc (dim * sizeof (double));
  
  if (state->k3 == 0)
    {
      free (state->k2);
      free (state->k1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k2", GSL_ENOMEM);
    }

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state->k3);
      free (state->k2);
      free (state->k1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k2", GSL_ENOMEM);
    }

  return state;
}


static int
rk2_apply (void *vstate,
           size_t dim,
           double t,
           double h,
           double y[],
           double yerr[],
           const double dydt_in[],
           double dydt_out[], 
           const gsl_odeiv_system * sys)
{
  rk2_state_t *state = (rk2_state_t *) vstate;

  size_t i;

  double *const k1 = state->k1;
  double *const k2 = state->k2;
  double *const k3 = state->k3;
  double *const ytmp = state->ytmp;

  /* k1 step */
  /* k1 = f(t,y) */

  if (dydt_in != NULL)
    {
      DBL_MEMCPY (k1, dydt_in, dim);
    }
  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t, y, k1);

      if (s != GSL_SUCCESS)
	{
	  return s;
	}
    }

  /* k2 step */
  /* k2 = f(t + 0.5*h, y + 0.5*k1) */

  for (i = 0; i < dim; i++)
    {
      ytmp[i] = y[i] + 0.5 * h * k1[i];
    }

  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, ytmp, k2);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  /* k3 step */
  /* for 3rd order estimates, is used for error estimation
     k3 = f(t + h, y - k1 + 2*k2) */
 
  for (i = 0; i < dim; i++)
    {
      ytmp[i] = y[i] + h * (-k1[i] + 2.0 * k2[i]);
    }

  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + h, ytmp, k3);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  /* final sum */
  
  for (i = 0; i < dim; i++)
    {
      /* Save original values if derivative evaluation below fails */
      ytmp[i] = y[i];

      {
	const double ksum3 = (k1[i] + 4.0 * k2[i] + k3[i]) / 6.0;
	y[i] += h * ksum3;
      }
    }
  
  /* Derivatives at output */

  if (dydt_out != NULL)
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t + h, y, dydt_out);
      
      if (s != GSL_SUCCESS)
	{
	  /* Restore original values */
	  DBL_MEMCPY (y, ytmp, dim);
	  
	  return s;
	}
    }

  /* Error estimation */

  for (i = 0; i < dim; i++)
    {
      const double ksum3 = (k1[i] + 4.0 * k2[i] + k3[i]) / 6.0;
      yerr[i] = h * (k2[i] - ksum3);
    }
  
  return GSL_SUCCESS;
}

static int
rk2_reset (void *vstate, size_t dim)
{
  rk2_state_t *state = (rk2_state_t *) vstate;

  DBL_ZERO_MEMSET (state->k1, dim);
  DBL_ZERO_MEMSET (state->k2, dim);
  DBL_ZERO_MEMSET (state->k3, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk2_order (void *vstate)
{
  return 2;
}

static void
rk2_free (void *vstate)
{
  rk2_state_t *state = (rk2_state_t *) vstate;
  free (state->k1);
  free (state->k2);
  free (state->k3);
  free (state->ytmp);
  free (state);
}

static const gsl_odeiv_step_type rk2_type = { "rk2",    /* name */
  1,                           /* can use dydt_in */
  1,                           /* gives exact dydt_out */
  &rk2_alloc,
  &rk2_apply,
  &rk2_reset,
  &rk2_order,
  &rk2_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk2 = &rk2_type;
