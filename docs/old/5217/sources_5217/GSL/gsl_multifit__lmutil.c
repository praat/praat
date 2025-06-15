static void compute_diag (const gsl_matrix * J, gsl_vector * diag);
static void update_diag (const gsl_matrix * J, gsl_vector * diag);
static double compute_delta (gsl_vector * diag, gsl_vector * x);
static double scaled_enorm (const gsl_vector * d, const gsl_vector * f);
static double enorm (const gsl_vector * f);

static double
enorm (const gsl_vector * f)
{
  return gsl_blas_dnrm2 (f);
}

static double
scaled_enorm (const gsl_vector * d, const gsl_vector * f)
{
  double e2 = 0;
  size_t i, n = f->size;
  for (i = 0; i < n; i++)
    {
      double fi = gsl_vector_get (f, i);
      double di = gsl_vector_get (d, i);
      double u = di * fi;
      e2 += u * u;
    }
  return sqrt (e2);
}

static double
compute_delta (gsl_vector * diag, gsl_vector * x)
{
  double Dx = scaled_enorm (diag, x);
  double factor = 100;  /* generally recommended value from MINPACK */

  return (Dx > 0) ? factor * Dx : factor;
}

static double
compute_actual_reduction (double fnorm, double fnorm1)
{
  double actred;

  if (0.1 * fnorm1 < fnorm)
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

static void
compute_diag (const gsl_matrix * J, gsl_vector * diag)
{
  size_t i, j, n = J->size1, p = J->size2;

  for (j = 0; j < p; j++)
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

static void
compute_rptdx (const gsl_matrix * r, const gsl_permutation * p,
               const gsl_vector * dx, gsl_vector * rptdx)
{
  size_t i, j, N = dx->size;

  for (i = 0; i < N; i++)
    {
      double sum = 0;

      for (j = i; j < N; j++)
        {
          size_t pj = gsl_permutation_get (p, j);

          sum += gsl_matrix_get (r, i, j) * gsl_vector_get (dx, pj);
        }

      gsl_vector_set (rptdx, i, sum);
    }
}


static void
compute_trial_step (gsl_vector * x, gsl_vector * dx, gsl_vector * x_trial)
{
  size_t i, N = x->size;

  for (i = 0; i < N; i++)
    {
      double pi = gsl_vector_get (dx, i);
      double xi = gsl_vector_get (x, i);
      gsl_vector_set (x_trial, i, xi + pi);
    }
}

