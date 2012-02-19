/* integration/qmomo.c
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
#include <stdlib.h>
#include "gsl_integration.h"
#include "gsl_errno.h"

static void
initialise (double * ri, double * rj, double * rg, double * rh,
            double alpha, double beta);

gsl_integration_qaws_table * 
gsl_integration_qaws_table_alloc (double alpha, double beta, int mu, int nu)
{
  gsl_integration_qaws_table * t;

  if (alpha < -1.0)
    {
      GSL_ERROR_VAL ("alpha must be greater than -1.0", GSL_EINVAL, 0);
    }

  if (beta < -1.0)
    {
      GSL_ERROR_VAL ("beta must be greater than -1.0", GSL_EINVAL, 0);
    }

  if (mu != 0 && mu != 1)
    {
      GSL_ERROR_VAL ("mu must be 0 or 1", GSL_EINVAL, 0);
    }

  if (nu != 0 && nu != 1)
    {
      GSL_ERROR_VAL ("nu must be 0 or 1", GSL_EINVAL, 0);
    }

  t = (gsl_integration_qaws_table *) 
    malloc(sizeof(gsl_integration_qaws_table));

  if (t == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for qaws_table struct",
                        GSL_ENOMEM, 0);
    }

  t->alpha = alpha;
  t->beta = beta;
  t->mu = mu;
  t->nu = nu;
  
  initialise (t->ri, t->rj, t->rg, t->rh, alpha, beta);
  
  return t;
}


int
gsl_integration_qaws_table_set (gsl_integration_qaws_table * t,
                                double alpha, double beta, int mu, int nu)
{
  if (alpha < -1.0)
    {
      GSL_ERROR ("alpha must be greater than -1.0", GSL_EINVAL);
    }

  if (beta < -1.0)
    {
      GSL_ERROR ("beta must be greater than -1.0", GSL_EINVAL);
    }

  if (mu != 0 && mu != 1)
    {
      GSL_ERROR ("mu must be 0 or 1", GSL_EINVAL);
    }

  if (nu != 0 && nu != 1)
    {
      GSL_ERROR ("nu must be 0 or 1", GSL_EINVAL);
    }

  t->alpha = alpha;
  t->beta = beta;
  t->mu = mu;
  t->nu = nu;
  
  initialise (t->ri, t->rj, t->rg, t->rh, alpha, beta);

  return GSL_SUCCESS;
}


void
gsl_integration_qaws_table_free (gsl_integration_qaws_table * t)
{
  free (t);
}

static void
initialise (double * ri, double * rj, double * rg, double * rh,
            double alpha, double beta)
{
  const double alpha_p1 = alpha + 1.0;
  const double beta_p1 = beta + 1.0;

  const double alpha_p2 = alpha + 2.0;
  const double beta_p2 = beta + 2.0;

  const double r_alpha = pow (2.0, alpha_p1);
  const double r_beta = pow (2.0, beta_p1);

  size_t i;
  
  double an, anm1;

  ri[0] = r_alpha / alpha_p1;
  ri[1] = ri[0] * alpha / alpha_p2;

  an = 2.0;
  anm1 = 1.0;

  for (i = 2; i < 25; i++)
    {
      ri[i] = -(r_alpha + an * (an - alpha_p2) * ri[i - 1])
        / (anm1 * (an + alpha_p1));
      anm1 = an;
      an = an + 1.0;
    }

  rj[0] = r_beta / beta_p1;
  rj[1] = rj[0] * beta / beta_p2;

  an = 2.0;
  anm1 = 1.0;

  for (i = 2; i < 25; i++)
    {
      rj[i] = -(r_beta + an * (an - beta_p2) * rj[i - 1])
        / (anm1 * (an + beta_p1));
      anm1 = an;
      an = an + 1.0;
    }

  rg[0] = -ri[0] / alpha_p1;
  rg[1] = -rg[0] - 2.0 * r_alpha / (alpha_p2 * alpha_p2);

  an = 2.0;
  anm1 = 1.0;

  for (i = 2; i < 25; i++)
    {
      rg[i] = -(an * (an - alpha_p2) * rg[i - 1] - an * ri[i - 1]
                + anm1 * ri[i]) / (anm1 * (an + alpha_p1));
      anm1 = an;
      an = an + 1.0;
    }

  rh[0] = -rj[0] / beta_p1;
  rh[1] = -rh[0] - 2.0 * r_beta / (beta_p2 * beta_p2);

  an = 2.0;
  anm1 = 1.0;

  for (i = 2; i < 25; i++)
    {
      rh[i] = -(an * (an - beta_p2) * rh[i - 1] - an * rj[i - 1]
                + anm1 * rj[i]) / (anm1 * (an + beta_p1));
      anm1 = an;
      an = an + 1.0;
    }

  for (i = 1; i < 25; i += 2)
    {
      rj[i] *= -1;
      rh[i] *= -1;
    }
}
