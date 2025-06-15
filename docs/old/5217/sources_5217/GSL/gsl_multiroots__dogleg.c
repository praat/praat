/* multiroots/dogleg.c
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

#include "gsl_multiroots__enorm.c"

static void compute_diag (const gsl_matrix * J, gsl_vector * diag);
static void update_diag (const gsl_matrix * J, gsl_vector * diag);
static double compute_delta (gsl_vector * diag, gsl_vector * x);
static void compute_df (const gsl_vector * f_trial, const gsl_vector * f, gsl_vector * df);
static void compute_wv (const gsl_vector * qtdf, const gsl_vector *rdx, const gsl_vector *dx, const gsl_vector *diag, double pnorm, gsl_vector * w, gsl_vector * v);

static double scaled_enorm (const gsl_vector * d, const gsl_vector * f);

static double scaled_enorm (const gsl_vector * d, const gsl_vector * f) {
  double e2 = 0 ;
  size_t i, n = f->size ;
  for (i = 0; i < n ; i++) {
    double fi= gsl_vector_get(f, i);
    double di= gsl_vector_get(d, i);
    double u = di * fi;
    e2 += u * u ;
  }
  return sqrt(e2);
}

static double enorm_sum (const gsl_vector * a, const gsl_vector * b);

static double enorm_sum (const gsl_vector * a, const gsl_vector * b) {
  double e2 = 0 ;
  size_t i, n = a->size ;
  for (i = 0; i < n ; i++) {
    double ai= gsl_vector_get(a, i);
    double bi= gsl_vector_get(b, i);
    double u = ai + bi;
    e2 += u * u ;
  }
  return sqrt(e2);
}

static void
compute_wv (const gsl_vector * qtdf, const gsl_vector *rdx, const gsl_vector *dx, const gsl_vector *diag, double pnorm, gsl_vector * w, gsl_vector * v)
{
  size_t i, n = qtdf->size;

  for (i = 0; i < n; i++)
    {
      double qtdfi = gsl_vector_get (qtdf, i);
      double rdxi = gsl_vector_get (rdx, i);
      double dxi = gsl_vector_get (dx, i);
      double diagi = gsl_vector_get (diag, i);

      gsl_vector_set (w, i, (qtdfi - rdxi) / pnorm);
      gsl_vector_set (v, i, diagi * diagi * dxi / pnorm);
    }
}


static void
compute_df (const gsl_vector * f_trial, const gsl_vector * f, gsl_vector * df)
{
  size_t i, n = f->size;

  for (i = 0; i < n; i++)
    {
      double dfi = gsl_vector_get (f_trial, i) - gsl_vector_get (f, i);
      gsl_vector_set (df, i, dfi);
    }
}

static void
compute_diag (const gsl_matrix * J, gsl_vector * diag)
{
  size_t i, j, n = diag->size;

  for (j = 0; j < n; j++)
    {
      double sum = 0;
      for (i = 0; i < n; i++)
        {
          double Jij = gsl_matrix_get (J, i, j);
          sum += Jij * Jij;
        }
      if (sum == 0)
        sum = 1.0;

      gsl_vector_set (diag, j, sqrt (sum));
    }
}

static void
update_diag (const gsl_matrix * J, gsl_vector * diag)
{
  size_t i, j, n = diag->size;

  for (j = 0; j < n; j++)
    {
      double cnorm, diagj, sum = 0;
      for (i = 0; i < n; i++)
        {
          double Jij = gsl_matrix_get (J, i, j);
          sum += Jij * Jij;
        }
      if (sum == 0)
        sum = 1.0;

      cnorm = sqrt (sum);
      diagj = gsl_vector_get (diag, j);

      if (cnorm > diagj)
        gsl_vector_set (diag, j, cnorm);
    }
}

static double
compute_delta (gsl_vector * diag, gsl_vector * x)
{
  double Dx = scaled_enorm (diag, x);
  double factor = 100;

  return (Dx > 0) ? factor * Dx : factor;
}

static double
compute_actual_reduction (double fnorm, double fnorm1)
{
  double actred;

  if (fnorm1 < fnorm)
    {
      double u = fnorm1 / fnorm;
      actred = 1 - u * u;
    }
  else
    {
      actred = -1;
    }

  return actred;
}

static double
compute_predicted_reduction (double fnorm, double fnorm1)
{
  double prered;

  if (fnorm1 < fnorm)
    {
      double u = fnorm1 / fnorm;
      prered = 1 - u * u;
    }
  else
    {
      prered = 0;
    }

  return prered;
}

static void 
compute_qtf (const gsl_matrix * q, const gsl_vector * f, gsl_vector * qtf)
{
  size_t i, j, N = f->size ;

  for (j = 0; j < N; j++)
    {
      double sum = 0;
      for (i = 0; i < N; i++)
        sum += gsl_matrix_get (q, i, j) * gsl_vector_get (f, i);

      gsl_vector_set (qtf, j, sum);
    }
}

static void 
compute_rdx (const gsl_matrix * r, const gsl_vector * dx, gsl_vector * rdx)
{
  size_t i, j, N = dx->size ;

  for (i = 0; i < N; i++)
    {
      double sum = 0;

      for (j = i; j < N; j++)
        {
          sum += gsl_matrix_get (r, i, j) * gsl_vector_get (dx, j);
        }

      gsl_vector_set (rdx, i, sum);
    }
}


static void
compute_trial_step (gsl_vector *x, gsl_vector * dx, gsl_vector * x_trial)
{
  size_t i, N = x->size;

  for (i = 0; i < N; i++)
    {
      double pi = gsl_vector_get (dx, i);
      double xi = gsl_vector_get (x, i);
      gsl_vector_set (x_trial, i, xi + pi);
    }
}

static int
newton_direction (const gsl_matrix * r, const gsl_vector * qtf, gsl_vector * p)
{
  const size_t N = r->size2;
  size_t i;
  int status;

  status = gsl_linalg_R_solve (r, qtf, p);

#ifdef DEBUG
  printf("rsolve status = %d\n", status);
#endif

  for (i = 0; i < N; i++)
    {
      double pi = gsl_vector_get (p, i);
      gsl_vector_set (p, i, -pi);
    }

  return status;
}

static void
gradient_direction (const gsl_matrix * r, const gsl_vector * qtf,
                    const gsl_vector * diag, gsl_vector * g)
{
  const size_t M = r->size1;
  const size_t N = r->size2;

  size_t i, j;

  for (j = 0; j < M; j++)
    {
      double sum = 0;
      double dj;

      for (i = 0; i < N; i++)
        {
          sum += gsl_matrix_get (r, i, j) * gsl_vector_get (qtf, i);
        }

      dj = gsl_vector_get (diag, j);
      gsl_vector_set (g, j, -sum / dj);
    }
}

static void
minimum_step (double gnorm, const gsl_vector * diag, gsl_vector * g)
{
  const size_t N = g->size;
  size_t i;

  for (i = 0; i < N; i++)
    {
      double gi = gsl_vector_get (g, i);
      double di = gsl_vector_get (diag, i);
      gsl_vector_set (g, i, (gi / gnorm) / di);
    }
}

static void
compute_Rg (const gsl_matrix * r, const gsl_vector * gradient, gsl_vector * Rg)
{
  const size_t N = r->size2;
  size_t i, j;

  for (i = 0; i < N; i++)
    {
      double sum = 0;

      for (j = i; j < N; j++)
        {
          double gj = gsl_vector_get (gradient, j);
          double rij = gsl_matrix_get (r, i, j);
          sum += rij * gj;
        }

      gsl_vector_set (Rg, i, sum);
    }
}

static void
scaled_addition (double alpha, gsl_vector * newton, double beta, gsl_vector * gradient, gsl_vector * p)
{
  const size_t N = p->size;
  size_t i;

  for (i = 0; i < N; i++)
    {
      double ni = gsl_vector_get (newton, i);
      double gi = gsl_vector_get (gradient, i);
      gsl_vector_set (p, i, alpha * ni + beta * gi);
    }
}

static int
dogleg (const gsl_matrix * r, const gsl_vector * qtf,
        const gsl_vector * diag, double delta,
        gsl_vector * newton, gsl_vector * gradient, gsl_vector * p)
{
  double qnorm, gnorm, sgnorm, bnorm, temp;

  newton_direction (r, qtf, newton);

#ifdef DEBUG
  printf("newton = "); gsl_vector_fprintf(stdout, newton, "%g"); printf("\n");
#endif

  qnorm = scaled_enorm (diag, newton);

  if (qnorm <= delta)
    {
      gsl_vector_memcpy (p, newton);
#ifdef DEBUG
      printf("took newton (qnorm = %g  <=   delta = %g)\n", qnorm, delta);
#endif
      return GSL_SUCCESS;
    }

  gradient_direction (r, qtf, diag, gradient);

#ifdef DEBUG
  printf("grad = "); gsl_vector_fprintf(stdout, gradient, "%g"); printf("\n");
#endif

  gnorm = enorm (gradient);

  if (gnorm == 0)
    {
      double alpha = delta / qnorm;
      double beta = 0;
      scaled_addition (alpha, newton, beta, gradient, p);
#ifdef DEBUG
      printf("took scaled newton because gnorm = 0\n");
#endif
      return GSL_SUCCESS;
    }

  minimum_step (gnorm, diag, gradient);

  compute_Rg (r, gradient, p);  /* Use p as temporary space to compute Rg */

#ifdef DEBUG
  printf("mingrad = "); gsl_vector_fprintf(stdout, gradient, "%g"); printf("\n");
  printf("Rg = "); gsl_vector_fprintf(stdout, p, "%g"); printf("\n");
#endif

  temp = enorm (p);
  sgnorm = (gnorm / temp) / temp;

  if (sgnorm > delta)
    {
      double alpha = 0;
      double beta = delta;
      scaled_addition (alpha, newton, beta, gradient, p);
#ifdef DEBUG
      printf("took gradient\n");
#endif
      return GSL_SUCCESS;
    }

  bnorm = enorm (qtf);

  {
    double bg = bnorm / gnorm;
    double bq = bnorm / qnorm;
    double dq = delta / qnorm;
    double dq2 = dq * dq;
    double sd = sgnorm / delta;
    double sd2 = sd * sd;

    double t1 = bg * bq * sd;
    double u = t1 - dq;
    double t2 = t1 - dq * sd2 + sqrt (u * u + (1-dq2) * (1 - sd2));

    double alpha = dq * (1 - sd2) / t2;
    double beta = (1 - alpha) * sgnorm;

#ifdef DEBUG
    printf("bnorm = %g\n", bnorm);
    printf("gnorm = %g\n", gnorm);
    printf("qnorm = %g\n", qnorm);
    printf("delta = %g\n", delta);
    printf("alpha = %g   beta = %g\n", alpha, beta);
    printf("took scaled combination of newton and gradient\n");
#endif

    scaled_addition (alpha, newton, beta, gradient, p);
  }

  return GSL_SUCCESS;
}
