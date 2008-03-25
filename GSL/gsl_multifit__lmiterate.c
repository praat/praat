static int
iterate (void *vstate, gsl_multifit_function_fdf * fdf, gsl_vector * x, gsl_vector * f, gsl_matrix * J, gsl_vector * dx, int scale)
{
  lmder_state_t *state = (lmder_state_t *) vstate;

  gsl_matrix *r = state->r;
  gsl_vector *tau = state->tau;
  gsl_vector *diag = state->diag;
  gsl_vector *qtf = state->qtf;
  gsl_vector *x_trial = state->x_trial;
  gsl_vector *f_trial = state->f_trial;
  gsl_vector *rptdx = state->rptdx;
  gsl_vector *newton = state->newton;
  gsl_vector *gradient = state->gradient;
  gsl_vector *sdiag = state->sdiag;
  gsl_vector *w = state->w;
  gsl_vector *work1 = state->work1;
  gsl_permutation *perm = state->perm;

  double prered, actred;
  double pnorm, fnorm1, fnorm1p, gnorm;
  double ratio;
  double dirder;

  int iter = 0;

  double p1 = 0.1, p25 = 0.25, p5 = 0.5, p75 = 0.75, p0001 = 0.0001;

  if (state->fnorm == 0.0) 
    {
      return GSL_SUCCESS;
    }

  /* Compute qtf = Q^T f */

  gsl_vector_memcpy (qtf, f);
  gsl_linalg_QR_QTvec (r, tau, qtf);

  /* Compute norm of scaled gradient */

  compute_gradient_direction (r, perm, qtf, diag, gradient);

  { 
    size_t iamax = gsl_blas_idamax (gradient);

    gnorm = fabs(gsl_vector_get (gradient, iamax) / state->fnorm);
  }

  /* Determine the Levenberg-Marquardt parameter */

lm_iteration:
  
  iter++ ;

  {
    int status = lmpar (r, perm, qtf, diag, state->delta, &(state->par), newton, gradient, sdiag, dx, w);
    if (status)
      return status;
  }

  /* Take a trial step */

  gsl_vector_scale (dx, -1.0); /* reverse the step to go downhill */

  compute_trial_step (x, dx, state->x_trial);

  pnorm = scaled_enorm (diag, dx);

  if (state->iter == 1)
    {
      if (pnorm < state->delta)
        {
#ifdef DEBUG
          printf("set delta = pnorm = %g\n" , pnorm);
#endif
          state->delta = pnorm;
        }
    }

  /* Evaluate function at x + p */
  /* return immediately if evaluation raised error */
  {
    int status = GSL_MULTIFIT_FN_EVAL_F (fdf, x_trial, f_trial);
    if (status)
      return status;
  }

  fnorm1 = enorm (f_trial);

  /* Compute the scaled actual reduction */

  actred = compute_actual_reduction (state->fnorm, fnorm1);

#ifdef DEBUG
  printf("lmiterate: fnorm = %g fnorm1 = %g  actred = %g\n", state->fnorm, fnorm1, actred);
  printf("r = "); gsl_matrix_fprintf(stdout, r, "%g");
  printf("perm = "); gsl_permutation_fprintf(stdout, perm, "%d");
  printf("dx = "); gsl_vector_fprintf(stdout, dx, "%g");
#endif

  /* Compute rptdx = R P^T dx, noting that |J dx| = |R P^T dx| */

  compute_rptdx (r, perm, dx, rptdx);

#ifdef DEBUG
  printf("rptdx = "); gsl_vector_fprintf(stdout, rptdx, "%g");
#endif

  fnorm1p = enorm (rptdx);

  /* Compute the scaled predicted reduction = |J dx|^2 + 2 par |D dx|^2 */

  { 
    double t1 = fnorm1p / state->fnorm;
    double t2 = (sqrt(state->par) * pnorm) / state->fnorm;
    
    prered = t1 * t1 + t2 * t2 / p5;
    dirder = -(t1 * t1 + t2 * t2);
  }

  /* compute the ratio of the actual to predicted reduction */

  if (prered > 0)
    {
      ratio = actred / prered;
    }
  else
    {
      ratio = 0;
    }

#ifdef DEBUG
  printf("lmiterate: prered = %g dirder = %g ratio = %g\n", prered, dirder,ratio);
#endif


  /* update the step bound */

  if (ratio > p25)
    {
#ifdef DEBUG
      printf("ratio > p25\n");
#endif
      if (state->par == 0 || ratio >= p75)
        {
          state->delta = pnorm / p5;
          state->par *= p5;
#ifdef DEBUG
          printf("updated step bounds: delta = %g, par = %g\n", state->delta, state->par);
#endif
        }
    }
  else
    {
      double temp = (actred >= 0) ? p5 : p5*dirder / (dirder + p5 * actred);

#ifdef DEBUG
      printf("ratio < p25\n");
#endif

      if (p1 * fnorm1 >= state->fnorm || temp < p1 ) 
        {
          temp = p1;
        }

      state->delta = temp * GSL_MIN_DBL (state->delta, pnorm/p1);

      state->par /= temp;
#ifdef DEBUG
      printf("updated step bounds: delta = %g, par = %g\n", state->delta, state->par);
#endif
    }


  /* test for successful iteration, termination and stringent tolerances */

  if (ratio >= p0001)
    {
      gsl_vector_memcpy (x, x_trial);
      gsl_vector_memcpy (f, f_trial);

      /* return immediately if evaluation raised error */
      {
        int status = GSL_MULTIFIT_FN_EVAL_DF (fdf, x_trial, J);
        if (status)
          return status;
      }

      /* wa2_j  = diag_j * x_j */
      state->xnorm = scaled_enorm(diag, x);
      state->fnorm = fnorm1;
      state->iter++;

      /* Rescale if necessary */

      if (scale)
        {
          update_diag (J, diag);
        }

      {
        int signum;
        gsl_matrix_memcpy (r, J);
        gsl_linalg_QRPT_decomp (r, tau, perm, &signum, work1);
      }
      
      return GSL_SUCCESS;
    }
  else if (fabs(actred) <= GSL_DBL_EPSILON  && prered <= GSL_DBL_EPSILON 
           && p5 * ratio <= 1.0)
    {
      return GSL_ETOLF ;
    }
  else if (state->delta <= GSL_DBL_EPSILON * state->xnorm)
    {
      return GSL_ETOLX;
    }
  else if (gnorm <= GSL_DBL_EPSILON)
    {
      return GSL_ETOLG;
    }
  else if (iter < 10)
    {
      /* Repeat inner loop if unsuccessful */
      goto lm_iteration;
    }

  return GSL_CONTINUE;
}
