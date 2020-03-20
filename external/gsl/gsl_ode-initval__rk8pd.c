/* ode-initval/rk8pd.c
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

/* Runge-Kutta 8(9), Prince-Dormand 
 *
 * High Order Embedded Runge-Kutta Formulae
 * P.J. Prince and J.R. Dormand
 * J. Comp. Appl. Math.,7, pp. 67-75, 1981
 */

/* Author:  G. Jungman
 */
#include "gsl__config.h"
#include <stdlib.h>
#include <string.h>
#include "gsl_errno.h"
#include "gsl_odeiv.h"

#include "gsl_ode-initval__odeiv_util.h"

/* Prince-Dormand constants */

static const double Abar[] = {
  14005451.0 / 335480064.0,
  0.0,
  0.0,
  0.0,
  0.0,
  -59238493.0 / 1068277825.0,
  181606767.0 / 758867731.0,
  561292985.0 / 797845732.0,
  -1041891430.0 / 1371343529.0,
  760417239.0 / 1151165299.0,
  118820643.0 / 751138087.0,
  -528747749.0 / 2220607170.0,
  1.0 / 4.0
};

static const double A[] = {
  13451932.0 / 455176623.0,
  0.0,
  0.0,
  0.0,
  0.0,
  -808719846.0 / 976000145.0,
  1757004468.0 / 5645159321.0,
  656045339.0 / 265891186.0,
  -3867574721.0 / 1518517206.0,
  465885868.0 / 322736535.0,
  53011238.0 / 667516719.0,
  2.0 / 45.0
};

static const double ah[] = {
  1.0 / 18.0,
  1.0 / 12.0,
  1.0 / 8.0,
  5.0 / 16.0,
  3.0 / 8.0,
  59.0 / 400.0,
  93.0 / 200.0,
  5490023248.0 / 9719169821.0,
  13.0 / 20.0,
  1201146811.0 / 1299019798.0
};

static const double b21 = 1.0 / 18.0;
static const double b3[] = { 1.0 / 48.0, 1.0 / 16.0 };
static const double b4[] = { 1.0 / 32.0, 0.0, 3.0 / 32.0 };
static const double b5[] = { 5.0 / 16.0, 0.0, -75.0 / 64.0, 75.0 / 64.0 };
static const double b6[] = { 3.0 / 80.0, 0.0, 0.0, 3.0 / 16.0, 3.0 / 20.0 };
static const double b7[] = {
  29443841.0 / 614563906.0,
  0.0,
  0.0,
  77736538.0 / 692538347.0,
  -28693883.0 / 1125000000.0,
  23124283.0 / 1800000000.0
};
static const double b8[] = {
  16016141.0 / 946692911.0,
  0.0,
  0.0,
  61564180.0 / 158732637.0,
  22789713.0 / 633445777.0,
  545815736.0 / 2771057229.0,
  -180193667.0 / 1043307555.0
};
static const double b9[] = {
  39632708.0 / 573591083.0,
  0.0,
  0.0,
  -433636366.0 / 683701615.0,
  -421739975.0 / 2616292301.0,
  100302831.0 / 723423059.0,
  790204164.0 / 839813087.0,
  800635310.0 / 3783071287.0
};
static const double b10[] = {
  246121993.0 / 1340847787.0,
  0.0,
  0.0,
  -37695042795.0 / 15268766246.0,
  -309121744.0 / 1061227803.0,
  -12992083.0 / 490766935.0,
  6005943493.0 / 2108947869.0,
  393006217.0 / 1396673457.0,
  123872331.0 / 1001029789.0
};
static const double b11[] = {
  -1028468189.0 / 846180014.0,
  0.0,
  0.0,
  8478235783.0 / 508512852.0,
  1311729495.0 / 1432422823.0,
  -10304129995.0 / 1701304382.0,
  -48777925059.0 / 3047939560.0,
  15336726248.0 / 1032824649.0,
  -45442868181.0 / 3398467696.0,
  3065993473.0 / 597172653.0
};
static const double b12[] = {
  185892177.0 / 718116043.0,
  0.0,
  0.0,
  -3185094517.0 / 667107341.0,
  -477755414.0 / 1098053517.0,
  -703635378.0 / 230739211.0,
  5731566787.0 / 1027545527.0,
  5232866602.0 / 850066563.0,
  -4093664535.0 / 808688257.0,
  3962137247.0 / 1805957418.0,
  65686358.0 / 487910083.0
};
static const double b13[] = {
  403863854.0 / 491063109.0,
  0.0,
  0.0,
  -5068492393.0 / 434740067.0,
  -411421997.0 / 543043805.0,
  652783627.0 / 914296604.0,
  11173962825.0 / 925320556.0,
  -13158990841.0 / 6184727034.0,
  3936647629.0 / 1978049680.0,
  -160528059.0 / 685178525.0,
  248638103.0 / 1413531060.0,
  0.0
};

typedef struct
{
  double *k[13];
  double *ytmp;
  double *y0;
}
rk8pd_state_t;

static void *
rk8pd_alloc (size_t dim)
{
  rk8pd_state_t *state = (rk8pd_state_t *) malloc (sizeof (rk8pd_state_t));
  int i, j;

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk8pd_state", GSL_ENOMEM);
    }

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->ytmp);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  for (i = 0; i < 13; i++)
    {
      state->k[i] = (double *) malloc (dim * sizeof (double));

      if (state->k[i] == 0)
        {
          for (j = 0; j < i; j++)
            {
              free (state->k[j]);
            }
          free (state->y0);
          free (state->ytmp);
          free (state);
          GSL_ERROR_NULL ("failed to allocate space for k's", GSL_ENOMEM);
        }
    }

  return state;
}


static int
rk8pd_apply (void *vstate,
             size_t dim,
             double t,
             double h,
             double y[],
             double yerr[],
             const double dydt_in[],
             double dydt_out[], const gsl_odeiv_system * sys)
{
  rk8pd_state_t *state = (rk8pd_state_t *) vstate;

  size_t i;

  double *const ytmp = state->ytmp;
  double *const y0 = state->y0;
  /* Note that k1 is stored in state->k[0] due to zero-based indexing */
  double *const k1 = state->k[0];
  double *const k2 = state->k[1];
  double *const k3 = state->k[2];
  double *const k4 = state->k[3];
  double *const k5 = state->k[4];
  double *const k6 = state->k[5];
  double *const k7 = state->k[6];
  double *const k8 = state->k[7];
  double *const k9 = state->k[8];
  double *const k10 = state->k[9];
  double *const k11 = state->k[10];
  double *const k12 = state->k[11];
  double *const k13 = state->k[12];

  DBL_MEMCPY (y0, y, dim);

  /* k1 step */
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

  for (i = 0; i < dim; i++)
    ytmp[i] = y[i] + b21 * h * k1[i];

  /* k2 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[0] * h, ytmp, k2);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] = y[i] + h * (b3[0] * k1[i] + b3[1] * k2[i]);

  /* k3 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[1] * h, ytmp, k3);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] = y[i] + h * (b4[0] * k1[i] + b4[2] * k3[i]);

  /* k4 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[2] * h, ytmp, k4);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] = y[i] + h * (b5[0] * k1[i] + b5[2] * k3[i] + b5[3] * k4[i]);

  /* k5 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[3] * h, ytmp, k5);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] = y[i] + h * (b6[0] * k1[i] + b6[3] * k4[i] + b6[4] * k5[i]);

  /* k6 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[4] * h, ytmp, k6);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b7[0] * k1[i] + b7[3] * k4[i] + b7[4] * k5[i] +
                  b7[5] * k6[i]);

  /* k7 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[5] * h, ytmp, k7);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b8[0] * k1[i] + b8[3] * k4[i] + b8[4] * k5[i] +
                  b8[5] * k6[i] + b8[6] * k7[i]);

  /* k8 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[6] * h, ytmp, k8);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b9[0] * k1[i] + b9[3] * k4[i] + b9[4] * k5[i] +
                  b9[5] * k6[i] + b9[6] * k7[i] + b9[7] * k8[i]);

  /* k9 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[7] * h, ytmp, k9);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b10[0] * k1[i] + b10[3] * k4[i] + b10[4] * k5[i] +
                  b10[5] * k6[i] + b10[6] * k7[i] + b10[7] * k8[i] +
                  b10[8] * k9[i]);

  /* k10 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[8] * h, ytmp, k10);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b11[0] * k1[i] + b11[3] * k4[i] + b11[4] * k5[i] +
                  b11[5] * k6[i] + b11[6] * k7[i] + b11[7] * k8[i] +
                  b11[8] * k9[i] + b11[9] * k10[i]);

  /* k11 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + ah[9] * h, ytmp, k11);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b12[0] * k1[i] + b12[3] * k4[i] + b12[4] * k5[i] +
                  b12[5] * k6[i] + b12[6] * k7[i] + b12[7] * k8[i] +
                  b12[8] * k9[i] + b12[9] * k10[i] + b12[10] * k11[i]);

  /* k12 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + h, ytmp, k12);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  for (i = 0; i < dim; i++)
    ytmp[i] =
      y[i] + h * (b13[0] * k1[i] + b13[3] * k4[i] + b13[4] * k5[i] +
                  b13[5] * k6[i] + b13[6] * k7[i] + b13[7] * k8[i] +
                  b13[8] * k9[i] + b13[9] * k10[i] + b13[10] * k11[i] +
                  b13[11] * k12[i]);

  /* k13 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + h, ytmp, k13);

    if (s != GSL_SUCCESS)
      {
	return s;
      }
  }

  /* final sum  */
  for (i = 0; i < dim; i++)
    {
      const double ksum8 =
        Abar[0] * k1[i] + Abar[5] * k6[i] + Abar[6] * k7[i] +
        Abar[7] * k8[i] + Abar[8] * k9[i] + Abar[9] * k10[i] +
        Abar[10] * k11[i] + Abar[11] * k12[i] + Abar[12] * k13[i];
      y[i] += h * ksum8;
    }

  /* Evaluate dydt_out[]. */

  if (dydt_out != NULL)
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t + h, y, dydt_out);
      
      if (s != GSL_SUCCESS)
	{
	  /* Restore initial values */
	  DBL_MEMCPY (y, y0, dim);
	  return s;
	}
    }

  /* error estimate */
  for (i = 0; i < dim; i++)
    {
      const double ksum8 =
        Abar[0] * k1[i] + Abar[5] * k6[i] + Abar[6] * k7[i] +
        Abar[7] * k8[i] + Abar[8] * k9[i] + Abar[9] * k10[i] +
        Abar[10] * k11[i] + Abar[11] * k12[i] + Abar[12] * k13[i];
      const double ksum7 =
        A[0] * k1[i] + A[5] * k6[i] + A[6] * k7[i] + A[7] * k8[i] +
        A[8] * k9[i] + A[9] * k10[i] + A[10] * k11[i] + A[11] * k12[i];
      yerr[i] = h * (ksum7 - ksum8);
    }

  return GSL_SUCCESS;
}

static int
rk8pd_reset (void *vstate, size_t dim)
{
  rk8pd_state_t *state = (rk8pd_state_t *) vstate;

  int i;

  for (i = 0; i < 13; i++)
    {
      DBL_ZERO_MEMSET (state->k[i], dim);
    }

  DBL_ZERO_MEMSET (state->y0, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk8pd_order (void *vstate)
{
  return 8;
}

static void
rk8pd_free (void *vstate)
{
  rk8pd_state_t *state = (rk8pd_state_t *) vstate;
  int i;

  for (i = 0; i < 13; i++)
    {
      free (state->k[i]);
    }
  free (state->y0);
  free (state->ytmp);
  free (state);
}

static const gsl_odeiv_step_type rk8pd_type = { "rk8pd",        /* name */
  1,                            /* can use dydt_in */
  1,                            /* gives exact dydt_out */
  &rk8pd_alloc,
  &rk8pd_apply,
  &rk8pd_reset,
  &rk8pd_order,
  &rk8pd_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk8pd = &rk8pd_type;
