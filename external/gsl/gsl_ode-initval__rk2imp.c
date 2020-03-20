/* ode-initval/rk2imp.c
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

/* Runge-Kutta 2, Gaussian implicit. Also known as the implicit
   midpoint rule. */

/* Author:  G. Jungman */

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
  double *Y1;
  double *y0;
  double *ytmp;
  double *y_onestep;
  double *y0_orig;
}
rk2imp_state_t;

static void *
rk2imp_alloc (size_t dim)
{
  rk2imp_state_t *state = (rk2imp_state_t *) malloc (sizeof (rk2imp_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk2imp_state",
                      GSL_ENOMEM);
    }

  state->Y1 = (double *) malloc (dim * sizeof (double));

  if (state->Y1 == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for Y1", GSL_ENOMEM);
    }

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state->Y1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->Y1);
      free (state->ytmp);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  state->y_onestep = (double *) malloc (dim * sizeof (double));

  if (state->y_onestep == 0)
    {
      free (state->Y1);
      free (state->ytmp);
      free (state->y0);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y_onestep", GSL_ENOMEM);
    }

  state->y0_orig = (double *) malloc (dim * sizeof (double));

  if (state->y0_orig == 0)
    {
      free (state->y_onestep);
      free (state->Y1);
      free (state->ytmp);
      free (state->y0);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0_orig", GSL_ENOMEM);
    }

  return state;
}

static int
rk2imp_step (double *y, rk2imp_state_t *state, 
	     const double h, const double t, 
	     const size_t dim, const gsl_odeiv_system *sys)
{
  /* Makes a Runge-Kutta 2nd order implicit advance with step size h.
     y0 is initial values of variables y. 

     The implicit matrix equations to solve are:

     Y1 = y0 + h/2 * f(t + h/2, Y1)

     y = y0 + h * f(t + h/2, Y1)
  */

  const double *y0 = state->y0;
  double *Y1 = state->Y1;
  double *ytmp = state->ytmp;
  int max_iter=3;
  int nu;
  size_t i;

  /* iterative solution of Y1 = y0 + h/2 * f(t + h/2, Y1) 
     Y1 should include initial values at call.

     Note: This method does not check for convergence of the
     iterative solution! 
  */

  for (nu = 0; nu < max_iter; nu++)
    {
      for (i = 0; i < dim; i++)
        {
          ytmp[i] = y0[i] + 0.5 * h * Y1[i];
        }

      {
	int s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, ytmp, Y1);
	
	if (s != GSL_SUCCESS)
	  {
	    return s;
	  }    
      }
    }
  
  /* assignment */

  for (i = 0; i < dim; i++)
    {
      y[i] = y0[i] + h * Y1[i];
    }

  return GSL_SUCCESS;
}

static int
rk2imp_apply (void *vstate,
              size_t dim,
              double t,
              double h,
              double y[],
              double yerr[],
              const double dydt_in[],
              double dydt_out[], const gsl_odeiv_system * sys)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;

  size_t i;

  double *Y1 = state->Y1;
  double *y0 = state->y0;
  double *y_onestep = state->y_onestep;
  double *y0_orig = state->y0_orig;

  /* Error estimation is done by step doubling procedure */

  /* initialization step */

  DBL_MEMCPY (y0, y, dim);

  /* Save initial values for possible failures */
  DBL_MEMCPY (y0_orig, y, dim);

  if (dydt_in != NULL)
    {
      DBL_MEMCPY (Y1, dydt_in, dim);
    }

  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t, y, Y1);
      
      if (s != GSL_SUCCESS)
	{
	  return s;
	}
    }

  /* First traverse h with one step (save to y_onestep) */

  DBL_MEMCPY (y_onestep, y, dim);

  {
    int s = rk2imp_step (y_onestep, state, h, t, dim, sys);

    if (s != GSL_SUCCESS) 
      {
	return s;
      }
  }

 /* Then with two steps with half step length (save to y) */ 

  {  
    int s = rk2imp_step (y, state, h / 2.0, t, dim, sys);

    if (s != GSL_SUCCESS)
      {
	/* Restore original y vector */
	DBL_MEMCPY (y, y0_orig, dim);

	return s;
      }
  }

  DBL_MEMCPY (y0, y, dim);

  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + h / 2.0, y, Y1);
      
    if (s != GSL_SUCCESS)
      {
	/* Restore original y vector */
	DBL_MEMCPY (y, y0_orig, dim);

	return s;
      }
  }

  {
    int s = rk2imp_step (y, state, h / 2.0, t + h / 2.0, dim, sys);

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
rk2imp_reset (void *vstate, size_t dim)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;

  DBL_ZERO_MEMSET (state->Y1, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);
  DBL_ZERO_MEMSET (state->y0, dim);
  DBL_ZERO_MEMSET (state->y_onestep, dim);
  DBL_ZERO_MEMSET (state->y0_orig, dim);
  
  return GSL_SUCCESS;
}

static unsigned int
rk2imp_order (void *vstate)
{
  return 2;
}

static void
rk2imp_free (void *vstate)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;

  free (state->Y1);
  free (state->ytmp);
  free (state->y0);
  free (state->y_onestep);
  free (state->y0_orig);
  free (state);
}

static const gsl_odeiv_step_type rk2imp_type = { "rk2imp",      /* name */
  1,                            /* can use dydt_in */
  1,                            /* gives exact dydt_out */
  &rk2imp_alloc,
  &rk2imp_apply,
  &rk2imp_reset,
  &rk2imp_order,
  &rk2imp_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk2imp = &rk2imp_type;
