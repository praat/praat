/* ode-initval/bsimp.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004 Gerard Jungman
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

/* Bulirsch-Stoer Implicit */

/* Author:  G. Jungman
 */
/* Bader-Deuflhard implicit extrapolative stepper.
 * [Numer. Math., 41, 373 (1983)]
 */
#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_linalg.h"
#include "gsl_odeiv.h"

#include "gsl_ode-initval__odeiv_util.h"

#define SEQUENCE_COUNT 8
#define SEQUENCE_MAX   7

/* Bader-Deuflhard extrapolation sequence */
static const int bd_sequence[SEQUENCE_COUNT] =
  { 2, 6, 10, 14, 22, 34, 50, 70 };

typedef struct
{
  gsl_matrix *d;                /* workspace for extrapolation         */
  gsl_matrix *a_mat;            /* workspace for linear system matrix  */
  gsl_permutation *p_vec;       /* workspace for LU permutation        */

  double x[SEQUENCE_MAX];       /* workspace for extrapolation */

  /* state info */
  size_t k_current;
  size_t k_choice;
  double h_next;
  double eps;

  /* workspace for extrapolation step */
  double *yp;
  double *y_save;
  double *yerr_save;
  double *y_extrap_save;
  double *y_extrap_sequence;
  double *extrap_work;
  double *dfdt;
  double *y_temp;
  double *delta_temp;
  double *weight;
  gsl_matrix *dfdy;

  /* workspace for the basic stepper */
  double *rhs_temp;
  double *delta;

  /* order of last step */
  size_t order;
}
bsimp_state_t;

/* Compute weighting factor */

static void
compute_weights (const double y[], double w[], size_t dim)
{
  size_t i;
  for (i = 0; i < dim; i++)
    {
      double u = fabs(y[i]);
      w[i] = (u > 0.0) ? u : 1.0;
    }
}

/* Calculate a choice for the "order" of the method, using the
 * Deuflhard criteria.  
 */

static size_t
bsimp_deuf_kchoice (double eps, size_t dimension)
{
  const double safety_f = 0.25;
  const double small_eps = safety_f * eps;

  double a_work[SEQUENCE_COUNT];
  double alpha[SEQUENCE_MAX][SEQUENCE_MAX];

  int i, k;

  a_work[0] = bd_sequence[0] + 1.0;

  for (k = 0; k < SEQUENCE_MAX; k++)
    {
      a_work[k + 1] = a_work[k] + bd_sequence[k + 1];
    }

  for (i = 0; i < SEQUENCE_MAX; i++)
    {
      alpha[i][i] = 1.0;
      for (k = 0; k < i; k++)
        {
          const double tmp1 = a_work[k + 1] - a_work[i + 1];
          const double tmp2 = (a_work[i + 1] - a_work[0] + 1.0) * (2 * k + 1);
          alpha[k][i] = pow (small_eps, tmp1 / tmp2);
        }
    }

  a_work[0] += dimension;

  for (k = 0; k < SEQUENCE_MAX; k++)
    {
      a_work[k + 1] = a_work[k] + bd_sequence[k + 1];
    }

  for (k = 0; k < SEQUENCE_MAX - 1; k++)
    {
      if (a_work[k + 2] > a_work[k + 1] * alpha[k][k + 1])
        break;
    }

  return k;
}

static void
poly_extrap (gsl_matrix * d,
             const double x[],
             const unsigned int i_step,
             const double x_i,
             const double y_i[],
             double y_0[], double y_0_err[], double work[], const size_t dim)
{
  size_t j, k;

  DBL_MEMCPY (y_0_err, y_i, dim);
  DBL_MEMCPY (y_0, y_i, dim);

  if (i_step == 0)
    {
      for (j = 0; j < dim; j++)
        {
          gsl_matrix_set (d, 0, j, y_i[j]);
        }
    }
  else
    {
      DBL_MEMCPY (work, y_i, dim);

      for (k = 0; k < i_step; k++)
        {
          double delta = 1.0 / (x[i_step - k - 1] - x_i);
          const double f1 = delta * x_i;
          const double f2 = delta * x[i_step - k - 1];

          for (j = 0; j < dim; j++)
            {
              const double q_kj = gsl_matrix_get (d, k, j);
              gsl_matrix_set (d, k, j, y_0_err[j]);
              delta = work[j] - q_kj;
              y_0_err[j] = f1 * delta;
              work[j] = f2 * delta;
              y_0[j] += y_0_err[j];
            }
        }

      for (j = 0; j < dim; j++)
        {
          gsl_matrix_set (d, i_step, j, y_0_err[j]);
        }
    }
}

/* Basic implicit Bulirsch-Stoer step.  Divide the step h_total into
 * n_step smaller steps and do the Bader-Deuflhard semi-implicit
 * iteration.  */

static int
bsimp_step_local (void *vstate,
                  size_t dim,
                  const double t0,
                  const double h_total,
                  const unsigned int n_step,
                  const double y[],
                  const double yp[],
                  const double dfdt[],
                  const gsl_matrix * dfdy,
                  double y_out[], 
                  const gsl_odeiv_system * sys)
{
  bsimp_state_t *state = (bsimp_state_t *) vstate;

  gsl_matrix *const a_mat = state->a_mat;
  gsl_permutation *const p_vec = state->p_vec;

  double *const delta = state->delta;
  double *const y_temp = state->y_temp;
  double *const delta_temp = state->delta_temp;
  double *const rhs_temp = state->rhs_temp;
  double *const w = state->weight;

  gsl_vector_view y_temp_vec = gsl_vector_view_array (y_temp, dim);
  gsl_vector_view delta_temp_vec = gsl_vector_view_array (delta_temp, dim);
  gsl_vector_view rhs_temp_vec = gsl_vector_view_array (rhs_temp, dim);

  const double h = h_total / n_step;
  double t = t0 + h;

  double sum;

  /* This is the factor sigma referred to in equation 3.4 of the
     paper.  A relative change in y exceeding sigma indicates a
     runaway behavior. According to the authors suitable values for
     sigma are >>1.  I have chosen a value of 100*dim. BJG */

  const double max_sum = 100.0 * dim;

  int signum, status;
  size_t i, j;
  size_t n_inter;

  /* Calculate the matrix for the linear system. */
  for (i = 0; i < dim; i++)
    {
      for (j = 0; j < dim; j++)
        {
          gsl_matrix_set (a_mat, i, j, -h * gsl_matrix_get (dfdy, i, j));
        }
      gsl_matrix_set (a_mat, i, i, gsl_matrix_get (a_mat, i, i) + 1.0);
    }

  /* LU decomposition for the linear system. */

  gsl_linalg_LU_decomp (a_mat, p_vec, &signum);

  /* Compute weighting factors */

  compute_weights (y, w, dim);

  /* Initial step. */

  for (i = 0; i < dim; i++)
    {
      y_temp[i] = h * (yp[i] + h * dfdt[i]);
    }

  gsl_linalg_LU_solve (a_mat, p_vec, &y_temp_vec.vector, &delta_temp_vec.vector);

  sum = 0.0;

  for (i = 0; i < dim; i++)
    {
      const double di = delta_temp[i];
      delta[i] = di;
      y_temp[i] = y[i] + di;
      sum += fabs(di) / w[i];
    }

  if (sum > max_sum) 
    {
      return GSL_EFAILED ;
    }

  /* Intermediate steps. */

  status = GSL_ODEIV_FN_EVAL (sys, t, y_temp, y_out);

  if (status)
    {
      return status;
    }

  for (n_inter = 1; n_inter < n_step; n_inter++)
    {
      for (i = 0; i < dim; i++)
        {
          rhs_temp[i] = h * y_out[i] - delta[i];
        }

      gsl_linalg_LU_solve (a_mat, p_vec, &rhs_temp_vec.vector, &delta_temp_vec.vector);

      sum = 0.0;

      for (i = 0; i < dim; i++)
        {
          delta[i] += 2.0 * delta_temp[i];
          y_temp[i] += delta[i];
          sum += fabs(delta[i]) / w[i];
        }

      if (sum > max_sum) 
        {
          return GSL_EFAILED ;
        }

      t += h;

      status = GSL_ODEIV_FN_EVAL (sys, t, y_temp, y_out);

      if (status)
        {
          return status;
        }
    }


  /* Final step. */

  for (i = 0; i < dim; i++)
    {
      rhs_temp[i] = h * y_out[i] - delta[i];
    }

  gsl_linalg_LU_solve (a_mat, p_vec, &rhs_temp_vec.vector, &delta_temp_vec.vector);

  sum = 0.0;

  for (i = 0; i < dim; i++)
    {
      y_out[i] = y_temp[i] + delta_temp[i];
      sum += fabs(delta_temp[i]) / w[i];
    }

  if (sum > max_sum) 
    {
      return GSL_EFAILED ;
    }

  return GSL_SUCCESS;
}


static void *
bsimp_alloc (size_t dim)
{
  bsimp_state_t *state = (bsimp_state_t *) malloc (sizeof (bsimp_state_t));

  state->d = gsl_matrix_alloc (SEQUENCE_MAX, dim);
  state->a_mat = gsl_matrix_alloc (dim, dim);
  state->p_vec = gsl_permutation_alloc (dim);

  state->yp = (double *) malloc (dim * sizeof (double));
  state->y_save = (double *) malloc (dim * sizeof (double));
  state->yerr_save = (double *) malloc (dim * sizeof (double));
  state->y_extrap_save = (double *) malloc (dim * sizeof (double));
  state->y_extrap_sequence = (double *) malloc (dim * sizeof (double));
  state->extrap_work = (double *) malloc (dim * sizeof (double));
  state->dfdt = (double *) malloc (dim * sizeof (double));
  state->y_temp = (double *) malloc (dim * sizeof (double));
  state->delta_temp = (double *) malloc (dim * sizeof(double));
  state->weight = (double *) malloc (dim * sizeof(double));

  state->dfdy = gsl_matrix_alloc (dim, dim);

  state->rhs_temp = (double *) malloc (dim * sizeof(double));
  state->delta = (double *) malloc (dim * sizeof (double));

  {
    size_t k_choice = bsimp_deuf_kchoice (GSL_SQRT_DBL_EPSILON, dim); /*FIXME: choice of epsilon? */
    state->k_choice = k_choice;
    state->k_current = k_choice;
    state->order = 2 * k_choice;
  }

  state->h_next = -GSL_SQRT_DBL_MAX;

  return state;
}

/* Perform the basic semi-implicit extrapolation
 * step, of size h, at a Deuflhard determined order.
 */
static int
bsimp_apply (void *vstate,
             size_t dim,
             double t,
             double h,
             double y[],
             double yerr[],
             const double dydt_in[],
             double dydt_out[], 
             const gsl_odeiv_system * sys)
{
  bsimp_state_t *state = (bsimp_state_t *) vstate;

  double *const x = state->x;
  double *const yp = state->yp;
  double *const y_save = state->y_save;
  double *const yerr_save = state->yerr_save;
  double *const y_extrap_sequence = state->y_extrap_sequence;
  double *const y_extrap_save = state->y_extrap_save;
  double *const extrap_work = state->extrap_work;
  double *const dfdt = state->dfdt;
  gsl_matrix *d = state->d;
  gsl_matrix *dfdy = state->dfdy;

  const double t_local = t;
  size_t i, k;

  if (h + t_local == t_local)
    {
      return GSL_EUNDRFLW;      /* FIXME: error condition */
    }

  DBL_MEMCPY (y_extrap_save, y, dim);

  /* Save inputs */
  DBL_MEMCPY (y_save, y, dim);
  DBL_MEMCPY (yerr_save, yerr, dim);
  
  /* Evaluate the derivative. */
  if (dydt_in != NULL)
    {
      DBL_MEMCPY (yp, dydt_in, dim);
    }
  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t_local, y, yp);

      if (s != GSL_SUCCESS)
	{
          return s;
	}
    }

  /* Evaluate the Jacobian for the system. */
  {
    int s = GSL_ODEIV_JA_EVAL (sys, t_local, y, dfdy->data, dfdt);
  
    if (s != GSL_SUCCESS)
      {
        return s;
      }
  }

  /* Make a series of refined extrapolations,
   * up to the specified maximum order, which
   * was calculated based on the Deuflhard
   * criterion upon state initialization.  */
  for (k = 0; k <= state->k_current; k++)
    {
      const unsigned int N = bd_sequence[k];
      const double r = (h / N);
      const double x_k = r * r;

      int status = bsimp_step_local (state,
                                     dim, t_local, h, N,
                                     y_extrap_save, yp,
                                     dfdt, dfdy,
                                     y_extrap_sequence, 
                                     sys);

      if (status == GSL_EFAILED)
        {
          /* If the local step fails, set the error to infinity in
             order to force a reduction in the step size */
	  
          for (i = 0; i < dim; i++)
            {
              yerr[i] = GSL_POSINF;
            }

          break;
        }
      
      else if (status != GSL_SUCCESS)
	{
	  return status;
	}
      
      x[k] = x_k;

      poly_extrap (d, x, k, x_k, y_extrap_sequence, y, yerr, extrap_work, dim);
    }

  /* Evaluate dydt_out[]. */

  if (dydt_out != NULL)
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t + h, y, dydt_out);

      if (s != GSL_SUCCESS)
        {
          DBL_MEMCPY (y, y_save, dim);
          DBL_MEMCPY (yerr, yerr_save, dim);
          return s;
        }
    }

  return GSL_SUCCESS;
}

static unsigned int
bsimp_order (void *vstate)
{
  bsimp_state_t *state = (bsimp_state_t *) vstate;
  return state->order;
}

static int
bsimp_reset (void *vstate, size_t dim)
{
  bsimp_state_t *state = (bsimp_state_t *) vstate;

  state->h_next = 0;

  DBL_ZERO_MEMSET (state->yp, dim);

  return GSL_SUCCESS;
}


static void
bsimp_free (void * vstate)
{
  bsimp_state_t *state = (bsimp_state_t *) vstate;

  free (state->delta);
  free (state->rhs_temp);

  gsl_matrix_free (state->dfdy);

  free (state->weight);
  free (state->delta_temp);
  free (state->y_temp);
  free (state->dfdt);
  free (state->extrap_work);
  free (state->y_extrap_sequence);
  free (state->y_extrap_save);
  free (state->y_save);
  free (state->yerr_save);
  free (state->yp);

  gsl_permutation_free (state->p_vec);
  gsl_matrix_free (state->a_mat);
  gsl_matrix_free (state->d);
  free (state);
}

static const gsl_odeiv_step_type bsimp_type = { 
  "bsimp",                      /* name */
  1,                            /* can use dydt_in */
  1,                            /* gives exact dydt_out */
  &bsimp_alloc,
  &bsimp_apply,
  &bsimp_reset,
  &bsimp_order,
  &bsimp_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_bsimp = &bsimp_type;
