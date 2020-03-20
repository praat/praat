/* ode-initval/gear1.c
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

/* Gear 1. This is the implicit Euler a.k.a backward Euler method. */

/* Author:  G. Jungman
 */

/* Error estimation by step doubling, see eg. Ascher, U.M., Petzold,
   L.R., Computer methods for ordinary differential and
   differential-algebraic equations, SIAM, Philadelphia, 1998.
   The method is also described in eg. this reference.
*/

#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_odeiv.h"

#include "gsl_ode-initval__odeiv_util.h"

typedef struct
{
  double *k;
  double *y0;
  double *y0_orig;
  double *y_onestep;
}
gear1_state_t;

static void *
gear1_alloc (size_t dim)
{
  gear1_state_t *state = (gear1_state_t *) malloc (sizeof (gear1_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for gear1_state", GSL_ENOMEM);
    }

  state->k = (double *) malloc (dim * sizeof (double));

  if (state->k == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->k);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  state->y0_orig = (double *) malloc (dim * sizeof (double));

  if (state->y0_orig == 0)
    {
      free (state->y0);
      free (state->k);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0_orig", GSL_ENOMEM);
    }

  state->y_onestep = (double *) malloc (dim * sizeof (double));

  if (state->y_onestep == 0)
    {
      free (state->y0_orig);
      free (state->y0);
      free (state->k);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y_onestep", GSL_ENOMEM);
    }

  return state;
}

static int
gear1_step (double *y, gear1_state_t *state, 
	    const double h, const double t, 
	    const size_t dim, const gsl_odeiv_system *sys)
{
  /* Makes an implicit Euler advance with step size h.
     y0 is the initial values of variables y. 

     The implicit matrix equations to solve are:

     k = y0 + h * f(t + h, k)

     y = y0 + h * f(t + h, k)
  */

  const int iter_steps = 3;
  int nu;
  size_t i;
  double *y0 = state->y0;
  double *k = state->k;

  /* Iterative solution of k = y0 + h * f(t + h, k)

     Note: This method does not check for convergence of the
     iterative solution! 
  */

  for (nu = 0; nu < iter_steps; nu++) 
    {
      int s = GSL_ODEIV_FN_EVAL(sys, t + h, y, k);

      if (s != GSL_SUCCESS)
	{
	  return s;
	}     

      for (i=0; i<dim; i++) 
	{
	  y[i] = y0[i] + h * k[i];
	}
    }
  
  return GSL_SUCCESS;
}

static int
gear1_apply(void * vstate,
            size_t dim,
            double t,
            double h,
            double y[],
            double yerr[],
            const double dydt_in[],
            double dydt_out[],
            const gsl_odeiv_system * sys)
{
  gear1_state_t *state = (gear1_state_t *) vstate;

  size_t i;

  double *y0 = state->y0;
  double *y0_orig = state->y0_orig;
  double *y_onestep = state->y_onestep;

  /* initialization */
  DBL_MEMCPY(y0, y, dim);

  /* Save initial values for possible failures */
  DBL_MEMCPY (y0_orig, y, dim);

  /* First traverse h with one step (save to y_onestep) */
  DBL_MEMCPY (y_onestep, y, dim);

  {
    int s = gear1_step (y_onestep, state, h, t, dim, sys);

    if (s != GSL_SUCCESS) 
      {
        return s;
      }
  }    

 /* Then with two steps with half step length (save to y) */ 
  {
    int s = gear1_step (y, state, h / 2.0, t, dim, sys);

    if (s != GSL_SUCCESS) 
      {
        /* Restore original y vector */
        DBL_MEMCPY (y, y0_orig, dim);
        return s;
      }
  }

  DBL_MEMCPY (y0, y, dim);

  {
    int s = gear1_step (y, state, h / 2.0, t + h / 2.0, dim, sys);

    if (s != GSL_SUCCESS) 
      {
        /* Restore original y vector */
        DBL_MEMCPY (y, y0_orig, dim);
        return s;
      }
  }
  
  /* Cleanup update */

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
      yerr[i] = 4.0 * (y[i] - y_onestep[i]);
    }

  return GSL_SUCCESS;
}

static int
gear1_reset (void *vstate, size_t dim)
{
  gear1_state_t *state = (gear1_state_t *) vstate;

  DBL_ZERO_MEMSET (state->y_onestep, dim);
  DBL_ZERO_MEMSET (state->y0_orig, dim);
  DBL_ZERO_MEMSET (state->y0, dim);
  DBL_ZERO_MEMSET (state->k, dim);
  return GSL_SUCCESS;
}

static unsigned int
gear1_order (void *vstate)
{
  return 1;
}

static void
gear1_free (void *vstate)
{
  gear1_state_t *state = (gear1_state_t *) vstate;
  free (state->y_onestep);
  free (state->y0_orig);
  free (state->y0);
  free (state->k);
  free (state);
}

static const gsl_odeiv_step_type gear1_type = { "gear1",        /* name */
  0,                            /* can use dydt_in? */
  1,                            /* gives exact dydt_out? */
  &gear1_alloc,
  &gear1_apply,
  &gear1_reset,
  &gear1_order,
  &gear1_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_gear1 = &gear1_type;
