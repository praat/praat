/* roots/newton.c
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

/* newton.c -- newton root finding algorithm 

   This is the classical Newton-Raphson iteration.

   x[i+1] = x[i] - f(x[i])/f'(x[i])

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
    double f, df;
  }
newton_state_t;

static int newton_init (void * vstate, gsl_function_fdf * fdf, double * root);
static int newton_iterate (void * vstate, gsl_function_fdf * fdf, double * root);

static int
newton_init (void * vstate, gsl_function_fdf * fdf, double * root)
{
  newton_state_t * state = (newton_state_t *) vstate;

  const double x = *root ;

  state->f = GSL_FN_FDF_EVAL_F (fdf, x);
  state->df = GSL_FN_FDF_EVAL_DF (fdf, x) ;

  return GSL_SUCCESS;

}

static int
newton_iterate (void * vstate, gsl_function_fdf * fdf, double * root)
{
  newton_state_t * state = (newton_state_t *) vstate;
  
  double root_new, f_new, df_new;

  if (state->df == 0.0)
    {
      GSL_ERROR("derivative is zero", GSL_EZERODIV);
    }

  root_new = *root - (state->f / state->df);

  *root = root_new ;
  
  GSL_FN_FDF_EVAL_F_DF(fdf, root_new, &f_new, &df_new);

  state->f = f_new ;
  state->df = df_new ;

  if (!gsl_finite(f_new))
    {
      GSL_ERROR ("function value is not finite", GSL_EBADFUNC);
    }

  if (!gsl_finite (df_new))
    {
      GSL_ERROR ("derivative value is not finite", GSL_EBADFUNC);
    }
      
  return GSL_SUCCESS;
}


static const gsl_root_fdfsolver_type newton_type =
{"newton",                              /* name */
 sizeof (newton_state_t),
 &newton_init,
 &newton_iterate};

const gsl_root_fdfsolver_type  * gsl_root_fdfsolver_newton = &newton_type;
