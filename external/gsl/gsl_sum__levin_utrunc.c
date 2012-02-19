/* sum/levin_utrunc.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

/* Author:  G. Jungman */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_test.h"
#include "gsl_errno.h"
#include "gsl_sum.h"

int
gsl_sum_levin_utrunc_accel (const double *array,
                            const size_t array_size,
                            gsl_sum_levin_utrunc_workspace * w,
                            double *sum_accel, double *abserr_trunc)
{
  return gsl_sum_levin_utrunc_minmax (array, array_size,
                                      0, array_size - 1,
                                      w, sum_accel, abserr_trunc);
}


int
gsl_sum_levin_utrunc_minmax (const double *array,
                             const size_t array_size,
                             const size_t min_terms,
                             const size_t max_terms,
                             gsl_sum_levin_utrunc_workspace * w,
                             double *sum_accel, double *abserr_trunc)
{
  if (array_size == 0)
    {
      *sum_accel = 0.0;
      *abserr_trunc = 0.0;
      w->sum_plain = 0.0;
      w->terms_used = 0;
      return GSL_SUCCESS;
    }
  else if (array_size == 1)
    {
      *sum_accel = array[0];
      *abserr_trunc = GSL_POSINF;
      w->sum_plain = array[0];
      w->terms_used = 1;
      return GSL_SUCCESS;
    }
  else
    {
      const double SMALL = 0.01;
      const size_t nmax = GSL_MAX (max_terms, array_size) - 1;
      double trunc_n = 0.0, trunc_nm1 = 0.0;
      double actual_trunc_n = 0.0, actual_trunc_nm1 = 0.0;
      double result_n = 0.0, result_nm1 = 0.0;
      size_t n;
      int better = 0;
      int before = 0;
      int converging = 0;
      double least_trunc = GSL_DBL_MAX;
      double result_least_trunc;

      /* Calculate specified minimum number of terms. No convergence
         tests are made, and no truncation information is stored. */

      for (n = 0; n < min_terms; n++)
        {
          const double t = array[n];

          result_nm1 = result_n;
          gsl_sum_levin_utrunc_step (t, n, w, &result_n);
        }

      /* Assume the result after the minimum calculation is the best. */

      result_least_trunc = result_n;

      /* Calculate up to maximum number of terms. Check truncation
         condition. */

      for (; n <= nmax; n++)
        {
          const double t = array[n];

          result_nm1 = result_n;
          gsl_sum_levin_utrunc_step (t, n, w, &result_n);

          /* Compute the truncation error directly */

          actual_trunc_nm1 = actual_trunc_n;
          actual_trunc_n = fabs (result_n - result_nm1);

          /* Average results to make a more reliable estimate of the
             real truncation error */

          trunc_nm1 = trunc_n;
          trunc_n = 0.5 * (actual_trunc_n + actual_trunc_nm1);

          /* Determine if we are in the convergence region. */

          better = (trunc_n < trunc_nm1 || trunc_n < SMALL * fabs (result_n));
          converging = converging || (better && before);
          before = better;

          if (converging)
            {
              if (trunc_n < least_trunc)
                {
                  /* Found a low truncation point in the convergence
                     region. Save it. */

                  least_trunc = trunc_n;
                  result_least_trunc = result_n;
                }

              if (fabs (trunc_n / result_n) < 10.0 * GSL_MACH_EPS)
                break;
            }
        }

      if (converging)
        {
          /* Stopped in the convergence region. Return result and
             error estimate. */

          *sum_accel = result_least_trunc;
          *abserr_trunc = least_trunc;
          w->terms_used = n;
          return GSL_SUCCESS;
        }
      else
        {
          /* Never reached the convergence region. Use the last
             calculated values. */

          *sum_accel = result_n;
          *abserr_trunc = trunc_n;
          w->terms_used = n;
          return GSL_SUCCESS;
        }
    }
}

int
gsl_sum_levin_utrunc_step (const double term,
                           const size_t n,
                           gsl_sum_levin_utrunc_workspace * w, double *sum_accel)
{
  if (term == 0.0)
    {
      /* This is actually harmless when treated in this way. A term
         which is exactly zero is simply ignored; the state is not
         changed. We return GSL_EZERODIV as an indicator that this
         occured. */

      return GSL_EZERODIV;
    }
  else if (n == 0)
    {
      *sum_accel = term;
      w->sum_plain = term;
      w->q_den[0] = 1.0 / term;
      w->q_num[0] = 1.0;
      return GSL_SUCCESS;
    }
  else
    {
      double factor = 1.0;
      double ratio = (double) n / (n + 1.0);
      int j;

      w->sum_plain += term;
      w->q_den[n] = 1.0 / (term * (n + 1.0) * (n + 1.0));
      w->q_num[n] = w->sum_plain * w->q_den[n];

      for (j = n - 1; j >= 0; j--)
        {
          double c = factor * (j + 1) / (n + 1);
          factor *= ratio;
          w->q_den[j] = w->q_den[j + 1] - c * w->q_den[j];
          w->q_num[j] = w->q_num[j + 1] - c * w->q_num[j];
        }

      *sum_accel = w->q_num[0] / w->q_den[0];
      return GSL_SUCCESS;
    }
}
