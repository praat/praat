/* multimin/directional_minimize.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Fabrice Rossi
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

static void
take_step (const gsl_vector * x, const gsl_vector * p,
           double step, double lambda, gsl_vector * x1, gsl_vector * dx)
{
  gsl_vector_set_zero (dx);
  gsl_blas_daxpy (-step * lambda, p, dx);

  gsl_vector_memcpy (x1, x);
  gsl_blas_daxpy (1.0, dx, x1);
}

static void 
intermediate_point (gsl_multimin_function_fdf * fdf,
                    const gsl_vector * x, const gsl_vector * p,
                    double lambda, 
                    double pg,
                    double stepa, double stepc,
                    double fa, double fc,
                    gsl_vector * x1, gsl_vector * dx, gsl_vector * gradient,
                    double * step, double * f)
{
  double stepb, fb;

trial:
  {
    double u = fabs (pg * lambda * stepc);
    stepb = 0.5 * stepc * u / ((fc - fa) + u);
  }

  take_step (x, p, stepb, lambda, x1, dx);

  fb = GSL_MULTIMIN_FN_EVAL_F (fdf, x1);

#ifdef DEBUG
  printf ("trying stepb = %g  fb = %.18e\n", stepb, fb);
#endif

  if (fb >= fa  && stepb > 0.0)
    {
      /* downhill step failed, reduce step-size and try again */
      fc = fb;
      stepc = stepb;
      goto trial;
    }
#ifdef DEBUG
  printf ("ok!\n");
#endif

  *step = stepb;
  *f = fb;
  GSL_MULTIMIN_FN_EVAL_DF(fdf, x1, gradient);
}

static void
minimize (gsl_multimin_function_fdf * fdf,
          const gsl_vector * x, const gsl_vector * p,
          double lambda,
          double stepa, double stepb, double stepc,
          double fa, double fb, double fc, double tol,
          gsl_vector * x1, gsl_vector * dx1, 
          gsl_vector * x2, gsl_vector * dx2, gsl_vector * gradient,          
          double * step, double * f, double * gnorm)
{
  /* Starting at (x0, f0) move along the direction p to find a minimum
     f(x0 - lambda * p), returning the new point x1 = x0-lambda*p,
     f1=f(x1) and g1 = grad(f) at x1.  */

  double u = stepb;
  double v = stepa;
  double w = stepc;
  double fu = fb;
  double fv = fa;
  double fw = fc;

  double old2 = fabs(w - v);
  double old1 = fabs(v - u);

  double stepm, fm, pg, gnorm1;

  int iter = 0;

  gsl_vector_memcpy (x2, x1);
  gsl_vector_memcpy (dx2, dx1);

  *f = fb;
  *step = stepb;
  *gnorm = gsl_blas_dnrm2 (gradient);

mid_trial:

  iter++;

  if (iter > 10)
    {
      return;  /* MAX ITERATIONS */
    }

  {
    double dw = w - u;
    double dv = v - u;
    double du = 0.0;

    double e1 = ((fv - fu) * dw * dw + (fu - fw) * dv * dv);
    double e2 = 2.0 * ((fv - fu) * dw + (fu - fw) * dv);

    if (e2 != 0.0)
      {
        du = e1 / e2;
      }

    if (du > 0.0 && du < (stepc - stepb) && fabs(du) < 0.5 * old2)
      {
        stepm = u + du;
      }
    else if (du < 0.0 && du > (stepa - stepb) && fabs(du) < 0.5 * old2)
      {
        stepm = u + du;
      }
    else if ((stepc - stepb) > (stepb - stepa))
      {
        stepm = 0.38 * (stepc - stepb) + stepb;
      }
    else
      {
        stepm = stepb - 0.38 * (stepb - stepa);
      }
  }

  take_step (x, p, stepm, lambda, x1, dx1);

  fm = GSL_MULTIMIN_FN_EVAL_F (fdf, x1);

#ifdef DEBUG
  printf ("trying stepm = %g  fm = %.18e\n", stepm, fm);
#endif

  if (fm > fb)
    {
      if (fm < fv)
        {
          w = v;
          v = stepm;
          fw = fv;
          fv = fm;
        }
      else if (fm < fw)
        {
          w = stepm;
          fw = fm;
        }

      if (stepm < stepb)
        {
          stepa = stepm;
          fa = fm;
        }
      else
        {
          stepc = stepm;
          fc = fm;
        }
      goto mid_trial;
    }
  else if (fm <= fb)
    {
      old2 = old1;
      old1 = fabs(u - stepm);
      w = v;
      v = u;
      u = stepm;
      fw = fv;
      fv = fu;
      fu = fm;

      gsl_vector_memcpy (x2, x1);
      gsl_vector_memcpy (dx2, dx1);

      GSL_MULTIMIN_FN_EVAL_DF (fdf, x1, gradient);
      gsl_blas_ddot (p, gradient, &pg);
      gnorm1 = gsl_blas_dnrm2 (gradient);

#ifdef DEBUG
      printf ("p: "); gsl_vector_fprintf(stdout, p, "%g");
      printf ("g: "); gsl_vector_fprintf(stdout, gradient, "%g");
      printf ("gnorm: %.18e\n", gnorm1);
      printf ("pg: %.18e\n", pg);
      printf ("orth: %g\n", fabs (pg * lambda/ gnorm1));
#endif
      *f = fm;
      *step = stepm;
      *gnorm = gnorm1;

      if (fabs (pg * lambda / gnorm1) < tol)
        {
#ifdef DEBUG
          printf("ok!\n");
#endif
          return; /* SUCCESS */
        }

      if (stepm < stepb)
        {
          stepc = stepb;
          fc = fb;
          stepb = stepm;
          fb = fm;
        }
      else
        {
          stepa = stepb;
          fa = fb;
          stepb = stepm;
          fb = fm;
        }
      goto mid_trial;
    }
}
