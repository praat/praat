/* integration/qc25s.c
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

struct fn_qaws_params
{
  gsl_function *function;
  double a;
  double b;
  gsl_integration_qaws_table *table;
};

static double fn_qaws (double t, void *params);
static double fn_qaws_L (double x, void *params);
static double fn_qaws_R (double x, void *params);

static void
compute_result (const double * r, const double * cheb12, const double * cheb24,
                double * result12, double * result24);


static void
qc25s (gsl_function * f, double a, double b, double a1, double b1,
       gsl_integration_qaws_table * t,
       double *result, double *abserr, int *err_reliable);

static void
qc25s (gsl_function * f, double a, double b, double a1, double b1,
       gsl_integration_qaws_table * t,
       double *result, double *abserr, int *err_reliable)
{
  gsl_function weighted_function;
  struct fn_qaws_params fn_params;
  
  fn_params.function = f;
  fn_params.a = a;
  fn_params.b = b;
  fn_params.table = t;

  weighted_function.params = &fn_params;
    
  if (a1 == a && (t->alpha != 0.0 || t->mu != 0))
    {
      double cheb12[13], cheb24[25];

      double factor = pow(0.5 * (b1 - a1), t->alpha + 1.0);

      weighted_function.function = &fn_qaws_R;

      gsl_integration_qcheb (&weighted_function, a1, b1, cheb12, cheb24);

      if (t->mu == 0)
        {
          double res12 = 0, res24 = 0;
          double u = factor;

          compute_result (t->ri, cheb12, cheb24, &res12, &res24);

          *result = u * res24;
          *abserr = fabs(u * (res24 - res12));
        }
      else 
        {
          double res12a = 0, res24a = 0;
          double res12b = 0, res24b = 0;

          double u = factor * log(b1 - a1);
          double v = factor;

          compute_result (t->ri, cheb12, cheb24, &res12a, &res24a);
          compute_result (t->rg, cheb12, cheb24, &res12b, &res24b);

          *result = u * res24a + v * res24b;
          *abserr = fabs(u * (res24a - res12a)) + fabs(v * (res24b - res12b));
        }

      *err_reliable = 0;

      return;
    }
  else if (b1 == b && (t->beta != 0.0 || t->nu != 0))
    {
      double cheb12[13], cheb24[25];
      double factor = pow(0.5 * (b1 - a1), t->beta + 1.0);

      weighted_function.function = &fn_qaws_L;

      gsl_integration_qcheb (&weighted_function, a1, b1, cheb12, cheb24);

      if (t->nu == 0)
        {
          double res12 = 0, res24 = 0;
          double u = factor;

          compute_result (t->rj, cheb12, cheb24, &res12, &res24);

          *result = u * res24;
          *abserr = fabs(u * (res24 - res12));
        }
      else 
        {
          double res12a = 0, res24a = 0;
          double res12b = 0, res24b = 0;

          double u = factor * log(b1 - a1);
          double v = factor;

          compute_result (t->rj, cheb12, cheb24, &res12a, &res24a);
          compute_result (t->rh, cheb12, cheb24, &res12b, &res24b);

          *result = u * res24a + v * res24b;
          *abserr = fabs(u * (res24a - res12a)) + fabs(v * (res24b - res12b));
        }

      *err_reliable = 0;

      return;
    }
  else
    {
      double resabs, resasc;

      weighted_function.function = &fn_qaws;
  
      gsl_integration_qk15 (&weighted_function, a1, b1, result, abserr,
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

}

static double
fn_qaws (double x, void *params)
{
  struct fn_qaws_params *p = (struct fn_qaws_params *) params;
  gsl_function *f = p->function;
  gsl_integration_qaws_table *t = p->table;

  double factor = 1.0;
  
  if (t->alpha != 0.0)
    factor *= pow(x - p->a, t->alpha);

  if (t->beta != 0.0)
    factor *= pow(p->b - x, t->beta);

  if (t->mu == 1)
    factor *= log(x - p->a);

  if (t->nu == 1)
    factor *= log(p->b - x);

  return factor * GSL_FN_EVAL (f, x);
}

static double
fn_qaws_L (double x, void *params)
{
  struct fn_qaws_params *p = (struct fn_qaws_params *) params;
  gsl_function *f = p->function;
  gsl_integration_qaws_table *t = p->table;

  double factor = 1.0;
  
  if (t->alpha != 0.0)
    factor *= pow(x - p->a, t->alpha);

  if (t->mu == 1)
    factor *= log(x - p->a);

  return factor * GSL_FN_EVAL (f, x);
}

static double
fn_qaws_R (double x, void *params)
{
  struct fn_qaws_params *p = (struct fn_qaws_params *) params;
  gsl_function *f = p->function;
  gsl_integration_qaws_table *t = p->table;

  double factor = 1.0;
  
  if (t->beta != 0.0)
    factor *= pow(p->b - x, t->beta);

  if (t->nu == 1)
    factor *= log(p->b - x);

  return factor * GSL_FN_EVAL (f, x);
}


static void
compute_result (const double * r, const double * cheb12, const double * cheb24,
                double * result12, double * result24)
{
  size_t i;
  double res12 = 0;
  double res24 = 0;
  
  for (i = 0; i < 13; i++)
    {
      res12 += r[i] * cheb12[i];
    }
  
  for (i = 0; i < 25; i++)
    {
      res24 += r[i] * cheb24[i];
    }
  
  *result12 = res12;
  *result24 = res24;
}
