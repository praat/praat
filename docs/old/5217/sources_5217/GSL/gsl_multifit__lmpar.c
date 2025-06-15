/* multifit/lmpar.c
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

#include "gsl_permute_vector_double.h"

#include "gsl_multifit__qrsolv.c"

static size_t
count_nsing (const gsl_matrix * r)
{
  /* Count the number of nonsingular entries. Returns the index of the
     first entry which is singular. */

  size_t n = r->size2;
  size_t i;

  for (i = 0; i < n; i++)
    {
      double rii = gsl_matrix_get (r, i, i);

      if (rii == 0)
        {
          break;
        }
    }

  return i;
}


static void
compute_newton_direction (const gsl_matrix * r, const gsl_permutation * perm,
                          const gsl_vector * qtf, gsl_vector * x)
{

  /* Compute and store in x the Gauss-Newton direction. If the
     Jacobian is rank-deficient then obtain a least squares
     solution. */

  const size_t n = r->size2;
  size_t i, j, nsing;

  for (i = 0 ; i < n ; i++)
    {
      double qtfi = gsl_vector_get (qtf, i);
      gsl_vector_set (x, i,  qtfi);
    }

  nsing = count_nsing (r);

#ifdef DEBUG
  printf("nsing = %d\n", nsing);
  printf("r = "); gsl_matrix_fprintf(stdout, r, "%g"); printf("\n");
  printf("qtf = "); gsl_vector_fprintf(stdout, x, "%g"); printf("\n");
#endif

  for (i = nsing; i < n; i++)
    {
      gsl_vector_set (x, i, 0.0);
    }

  if (nsing > 0)
    {
      for (j = nsing; j > 0 && j--;)
        {
          double rjj = gsl_matrix_get (r, j, j);
          double temp = gsl_vector_get (x, j) / rjj;
          
          gsl_vector_set (x, j, temp);
          
          for (i = 0; i < j; i++)
            {
              double rij = gsl_matrix_get (r, i, j);
              double xi = gsl_vector_get (x, i);
              gsl_vector_set (x, i, xi - rij * temp);
            }
        }
    }

  gsl_permute_vector_inverse (perm, x);
}

static void
compute_newton_correction (const gsl_matrix * r, const gsl_vector * sdiag,
                           const gsl_permutation * p, gsl_vector * x,
                           double dxnorm,
                           const gsl_vector * diag, gsl_vector * w)
{
  size_t n = r->size2;
  size_t i, j;

  for (i = 0; i < n; i++)
    {
      size_t pi = gsl_permutation_get (p, i);

      double dpi = gsl_vector_get (diag, pi);
      double xpi = gsl_vector_get (x, pi);

      gsl_vector_set (w, i, dpi * (dpi * xpi) / dxnorm);
    }

  for (j = 0; j < n; j++)
    {
      double sj = gsl_vector_get (sdiag, j);
      double wj = gsl_vector_get (w, j);

      double tj = wj / sj;

      gsl_vector_set (w, j, tj);

      for (i = j + 1; i < n; i++)
        {
          double rij = gsl_matrix_get (r, i, j);
          double wi = gsl_vector_get (w, i);

          gsl_vector_set (w, i, wi - rij * tj);
        }
    }
}

static void
compute_newton_bound (const gsl_matrix * r, const gsl_vector * x, 
                      double dxnorm,  const gsl_permutation * perm, 
                      const gsl_vector * diag, gsl_vector * w)
{
  /* If the jacobian is not rank-deficient then the Newton step
     provides a lower bound for the zero of the function. Otherwise
     set this bound to zero. */

  size_t n = r->size2;

  size_t i, j;

  size_t nsing = count_nsing (r);

  if (nsing < n)
    {
      gsl_vector_set_zero (w);
      return;
    }

  for (i = 0; i < n; i++)
    {
      size_t pi = gsl_permutation_get (perm, i);

      double dpi = gsl_vector_get (diag, pi);
      double xpi = gsl_vector_get (x, pi);

      gsl_vector_set (w, i, dpi * (dpi * xpi / dxnorm));
    }

  for (j = 0; j < n; j++)
    {
      double sum = 0;

      for (i = 0; i < j; i++)
        {
          sum += gsl_matrix_get (r, i, j) * gsl_vector_get (w, i);
        }

      {
        double rjj = gsl_matrix_get (r, j, j);
        double wj = gsl_vector_get (w, j);

        gsl_vector_set (w, j, (wj - sum) / rjj);
      }
    }
}

static void
compute_gradient_direction (const gsl_matrix * r, const gsl_permutation * p,
                            const gsl_vector * qtf, const gsl_vector * diag,
                            gsl_vector * g)
{
  const size_t n = r->size2;

  size_t i, j;

  for (j = 0; j < n; j++)
    {
      double sum = 0;

      for (i = 0; i <= j; i++)
        {
          sum += gsl_matrix_get (r, i, j) * gsl_vector_get (qtf, i);
        }

      {
        size_t pj = gsl_permutation_get (p, j);
        double dpj = gsl_vector_get (diag, pj);

        gsl_vector_set (g, j, sum / dpj);
      }
    }
}

static int
lmpar (gsl_matrix * r, const gsl_permutation * perm, const gsl_vector * qtf,
       const gsl_vector * diag, double delta, double * par_inout,
       gsl_vector * newton, gsl_vector * gradient, gsl_vector * sdiag, 
       gsl_vector * x, gsl_vector * w)
{
  double dxnorm, gnorm, fp, fp_old, par_lower, par_upper, par_c;

  double par = *par_inout;

  size_t iter = 0;

#ifdef DEBUG
  printf("ENTERING lmpar\n");
#endif


  compute_newton_direction (r, perm, qtf, newton);

#ifdef DEBUG
  printf ("newton = ");
  gsl_vector_fprintf (stdout, newton, "%g");
  printf ("\n");

  printf ("diag = ");
  gsl_vector_fprintf (stdout, diag, "%g");
  printf ("\n");
#endif

  /* Evaluate the function at the origin and test for acceptance of
     the Gauss-Newton direction. */

  dxnorm = scaled_enorm (diag, newton);

  fp = dxnorm - delta;

#ifdef DEBUG
  printf ("dxnorm = %g, delta = %g, fp = %g\n", dxnorm, delta, fp);
#endif

  if (fp <= 0.1 * delta)
    {
      gsl_vector_memcpy (x, newton);
#ifdef DEBUG
      printf ("took newton (fp = %g, delta = %g)\n", fp, delta);
#endif

      *par_inout = 0;

      return GSL_SUCCESS;
    }

#ifdef DEBUG
  printf ("r = ");
  gsl_matrix_fprintf (stdout, r, "%g");
  printf ("\n");

  printf ("newton = ");
  gsl_vector_fprintf (stdout, newton, "%g");
  printf ("\n");

  printf ("dxnorm = %g\n", dxnorm);
#endif


  compute_newton_bound (r, newton, dxnorm, perm, diag, w);

#ifdef DEBUG
  printf("perm = "); gsl_permutation_fprintf(stdout, perm, "%d");

  printf ("diag = ");
  gsl_vector_fprintf (stdout, diag, "%g");
  printf ("\n");

  printf ("w = ");
  gsl_vector_fprintf (stdout, w, "%g");
  printf ("\n");
#endif


  {
    double wnorm = enorm (w);
    double phider = wnorm * wnorm;

    /* w == zero if r rank-deficient, 
       then set lower bound to zero form MINPACK, lmder.f 
       Hans E. Plesser 2002-02-25 (hans.plesser@itf.nlh.no) */
    if ( wnorm > 0 )
      par_lower = fp / (delta * phider);
    else
      par_lower = 0.0;
  }

#ifdef DEBUG
  printf("par       = %g\n", par      );
  printf("par_lower = %g\n", par_lower);
#endif

  compute_gradient_direction (r, perm, qtf, diag, gradient);

  gnorm = enorm (gradient);

#ifdef DEBUG
  printf("gradient = "); gsl_vector_fprintf(stdout, gradient, "%g"); printf("\n");
  printf("gnorm = %g\n", gnorm);
#endif

  par_upper =  gnorm / delta;

  if (par_upper == 0)
    {
      par_upper = GSL_DBL_MIN / GSL_MIN_DBL(delta, 0.1);
    }

#ifdef DEBUG
  printf("par_upper = %g\n", par_upper);
#endif

  if (par > par_upper)
    {
#ifdef DEBUG
  printf("set par to par_upper\n");
#endif

      par = par_upper;
    }
  else if (par < par_lower)
    {
#ifdef DEBUG
  printf("set par to par_lower\n");
#endif

      par = par_lower;
    }

  if (par == 0)
    {
      par = gnorm / dxnorm;
#ifdef DEBUG
      printf("set par to gnorm/dxnorm = %g\n", par);
#endif

    }

  /* Beginning of iteration */

iteration:

  iter++;

#ifdef DEBUG
  printf("lmpar iteration = %d\n", iter);
#endif

#ifdef BRIANSFIX
  /* Seems like this is described in the paper but not in the MINPACK code */

  if (par < par_lower || par > par_upper) 
    {
      par = GSL_MAX_DBL (0.001 * par_upper, sqrt(par_lower * par_upper));
    }
#endif

  /* Evaluate the function at the current value of par */

  if (par == 0)
    {
      par = GSL_MAX_DBL (0.001 * par_upper, GSL_DBL_MIN);
#ifdef DEBUG
      printf("par = 0, set par to  = %g\n", par);
#endif

    }

  /* Compute the least squares solution of [ R P x - Q^T f, sqrt(par) D x]
     for A = Q R P^T */

#ifdef DEBUG
  printf ("calling qrsolv with par = %g\n", par);
#endif

  {
    double sqrt_par = sqrt(par);

    qrsolv (r, perm, sqrt_par, diag, qtf, x, sdiag, w);
  }

  dxnorm = scaled_enorm (diag, x);

  fp_old = fp;

  fp = dxnorm - delta;

#ifdef DEBUG
  printf ("After qrsolv dxnorm = %g, delta = %g, fp = %g\n", dxnorm, delta, fp);
  printf ("sdiag = ") ; gsl_vector_fprintf(stdout, sdiag, "%g"); printf("\n");
  printf ("x = ") ; gsl_vector_fprintf(stdout, x, "%g"); printf("\n");
  printf ("r = ") ; gsl_matrix_fprintf(stdout, r, "%g"); printf("\nXXX\n");
#endif

  /* If the function is small enough, accept the current value of par */

  if (fabs (fp) <= 0.1 * delta)
    goto line220;

  if (par_lower == 0 && fp <= fp_old && fp_old < 0)
    goto line220;

  /* Check for maximum number of iterations */

  if (iter == 10)
    goto line220;

  /* Compute the Newton correction */

  compute_newton_correction (r, sdiag, perm, x, dxnorm, diag, w);

#ifdef DEBUG
  printf ("newton_correction = ");
  gsl_vector_fprintf(stdout, w, "%g"); printf("\n");
#endif

  {
    double wnorm = enorm (w);
    par_c = fp / (delta * wnorm * wnorm);
  }

#ifdef DEBUG
  printf("fp = %g\n", fp);
  printf("par_lower = %g\n", par_lower);
  printf("par_upper = %g\n", par_upper);
  printf("par_c = %g\n", par_c);
#endif


  /* Depending on the sign of the function, update par_lower or par_upper */

  if (fp > 0)
    {
      if (par > par_lower)
        {
          par_lower = par;
#ifdef DEBUG
      printf("fp > 0: set par_lower = par = %g\n", par);
#endif

        }
    }
  else if (fp < 0)
    {
      if (par < par_upper)
        {
#ifdef DEBUG
      printf("fp < 0: set par_upper = par = %g\n", par);
#endif
          par_upper = par;
        }
    }

  /* Compute an improved estimate for par */

#ifdef DEBUG
      printf("improved estimate par = MAX(%g, %g) \n", par_lower, par+par_c);
#endif

  par = GSL_MAX_DBL (par_lower, par + par_c);

#ifdef DEBUG
      printf("improved estimate par = %g \n", par);
#endif


  goto iteration;

line220:

#ifdef DEBUG
  printf("LEAVING lmpar, par = %g\n", par);
#endif

  *par_inout = par;

  return GSL_SUCCESS;
}



