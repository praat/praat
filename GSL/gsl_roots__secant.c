/* roots/secant.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
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

/* secant.c -- secant root finding algorithm 

   The secant algorithm is a variant of the Newton algorithm with the
   derivative term replaced by a numerical estimate from the last two
   function evaluations.

   x[i+1] = x[i] - f(x[i]) / f'_est

   where f'_est = (f(x[i]) - f(x[i-1])) / (x[i] - x[i-1])

   The exact derivative is used for the initial value of f'_est.

*/

#include "gsl__config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_roots.h"

#include "gsl_roots__roots.h"

typedef struct
  {
    double f;
    double df;
  }
secant_state_t;

static int secant_init (void * vstate, gsl_function_fdf * fdf, double * root);
static int secant_iterate (void * vstate, gsl_function_fdf * fdf, double * root);

static int
secant_init (void * vstate, gsl_function_fdf * fdf, double * root)
{
  secant_state_t * state = (secant_state_t *) vstate;

  const double x = *root;

  GSL_FN_FDF_EVAL_F_DF (fdf, x, &(state->f), &(state->df));
  
  return GSL_SUCCESS;

}

static int
secant_iterate (void * vstate, gsl_function_fdf * fdf, double * root)
{
  secant_state_t * state = (secant_state_t *) vstate;
  
  const double x = *root ;
  const double f = state->f;
  const double df = state->df;

  double x_new, f_new, df_new;

  if (state->df == 0.0)
    {
      GSL_ERROR("derivative is zero", GSL_EZERODIV);
    }

  x_new = x - (f / df);

  f_new = GSL_FN_FDF_EVAL_F(fdf, x_new) ;
  df_new = (f_new - f) / (x_new - x) ;

  *root = x_new ;

  state->f = f_new ;
  state->df = df_new ;

  if (!gsl_finite (f_new))
    {
      GSL_ERROR ("function value is not finite", GSL_EBADFUNC);
    }

  if (!gsl_finite (df_new))
    {
      GSL_ERROR ("derivative value is not finite", GSL_EBADFUNC);
    }
      
  return GSL_SUCCESS;
}


static const gsl_root_fdfsolver_type secant_type =
{"secant",                              /* name */
 sizeof (secant_state_t),
 &secant_init,
 &secant_iterate};

const gsl_root_fdfsolver_type  * gsl_root_fdfsolver_secant = &secant_type;
