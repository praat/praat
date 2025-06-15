/* integration/qc25c.c
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

struct fn_cauchy_params
{
  gsl_function *function;
  double singularity;
};

static double fn_cauchy (double t, void *params);

static void compute_moments (double cc, double *moment);

static void
qc25c (gsl_function * f, double a, double b, double c, 
       double *result, double *abserr, int *err_reliable);

static void
qc25c (gsl_function * f, double a, double b, double c, 
       double *result, double *abserr, int *err_reliable)
{
  double cc = (2 * c - b - a) / (b - a);

  if (fabs (cc) > 1.1)
    {
      double resabs, resasc;

      gsl_function weighted_function;
      struct fn_cauchy_params fn_params;

      fn_params.function = f;
      fn_params.singularity = c;

      weighted_function.function = &fn_cauchy;
      weighted_function.params = &fn_params;

      gsl_integration_qk15 (&weighted_function, a, b, result, abserr,
                            &resabs, &resasc);
      
      if (*abserr == resasc)
        {
          *err_reliable = 0;
        }
      else 
        {
          *err_reliable = 1;
        }

      return;
    }
  else
    {
      double cheb12[13], cheb24[25], moment[25];
      double res12 = 0, res24 = 0;
      size_t i;
      gsl_integration_qcheb (f, a, b, cheb12, cheb24);
      compute_moments (cc, moment);

      for (i = 0; i < 13; i++)
        {
          res12 += cheb12[i] * moment[i];
        }

      for (i = 0; i < 25; i++)
        {
          res24 += cheb24[i] * moment[i];
        }

      *result = res24;
      *abserr = fabs(res24 - res12) ;
      *err_reliable = 0;

      return;
    }
}

static double
fn_cauchy (double x, void *params)
{
  struct fn_cauchy_params *p = (struct fn_cauchy_params *) params;
  gsl_function *f = p->function;
  double c = p->singularity;
  return GSL_FN_EVAL (f, x) / (x - c);
}

static void
compute_moments (double cc, double *moment)
{
  size_t k;

  double a0 = log (fabs ((1.0 - cc) / (1.0 + cc)));
  double a1 = 2 + a0 * cc;

  moment[0] = a0;
  moment[1] = a1;

  for (k = 2; k < 25; k++)
    {
      double a2;

      if ((k % 2) == 0)
        {
          a2 = 2.0 * cc * a1 - a0;
        }
      else
        {
          const double km1 = k - 1.0;
          a2 = 2.0 * cc * a1 - a0 - 4.0 / (km1 * km1 - 1.0);
        }

      moment[k] = a2;

      a0 = a1;
      a1 = a2;
    }
}
