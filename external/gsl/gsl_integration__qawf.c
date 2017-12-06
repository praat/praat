/* integration/qawf.c
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
#include <math.h>
#include <float.h>
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_integration.h"

#include "gsl_integration__initialise.c"
#include "gsl_integration__append.c"
#include "gsl_integration__qelg.c"

int
gsl_integration_qawf (gsl_function * f,
                      const double a,
                      const double epsabs,
                      const size_t limit,
                      gsl_integration_workspace * workspace,
                      gsl_integration_workspace * cycle_workspace,
                      gsl_integration_qawo_table * wf,
                      double *result, double *abserr)
{
  double area, errsum;
  double res_ext, err_ext;
  double correc, total_error = 0.0, truncation_error;

  size_t ktmin = 0;
  size_t iteration = 0;

  struct extrapolation_table table;

  double cycle;
  double omega = wf->omega;

  const double p = 0.9;
  double factor = 1;
  double initial_eps, eps;
  int error_type = 0;

  /* Initialize results */

  initialise (workspace, a, a);

  *result = 0;
  *abserr = 0;

  if (limit > workspace->limit)
    {
      GSL_ERROR ("iteration limit exceeds available workspace", GSL_EINVAL) ;
    }

  /* Test on accuracy */

  if (epsabs <= 0)
    {
      GSL_ERROR ("absolute tolerance epsabs must be positive", GSL_EBADTOL) ;
    }

  if (omega == 0.0)
    {
      if (wf->sine == GSL_INTEG_SINE)
        {
          /* The function sin(w x) f(x) is always zero for w = 0 */

          *result = 0;
          *abserr = 0;

          return GSL_SUCCESS;
        }
      else
        {
          /* The function cos(w x) f(x) is always f(x) for w = 0 */

          int status = gsl_integration_qagiu (f, a, epsabs, 0.0,
                                              cycle_workspace->limit,
                                              cycle_workspace,
                                              result, abserr);
          return status;
        }
    }

  if (epsabs > GSL_DBL_MIN / (1 - p))
    {
      eps = epsabs * (1 - p);
    }
  else
    {
      eps = epsabs;
    }

  initial_eps = eps;

  area = 0;
  errsum = 0;

  res_ext = 0;
  err_ext = GSL_DBL_MAX;
  correc = 0;

  cycle = (2 * floor (fabs (omega)) + 1) * M_PI / fabs (omega);

  gsl_integration_qawo_table_set_length (wf, cycle);

  initialise_table (&table);

  for (iteration = 0; iteration < limit; iteration++)
    {
      double area1, error1, reseps, erreps;

      double a1 = a + iteration * cycle;
      double b1 = a1 + cycle;

      double epsabs1 = eps * factor;

      int status = gsl_integration_qawo (f, a1, epsabs1, 0.0, limit,
                                         cycle_workspace, wf,
                                         &area1, &error1);

      append_interval (workspace, a1, b1, area1, error1);

      factor *= p;

      area = area + area1;
      errsum = errsum + error1;

      /* estimate the truncation error as 50 times the final term */

      truncation_error = 50 * fabs (area1);

      total_error = errsum + truncation_error;

      if (total_error < epsabs && iteration > 4)
        {
          goto compute_result;
        }

      if (error1 > correc)
        {
          correc = error1;
        }

      if (status)
        {
          eps = GSL_MAX_DBL (initial_eps, correc * (1.0 - p));
        }

      if (status && total_error < 10 * correc && iteration > 3)
        {
          goto compute_result;
        }

      append_table (&table, area);

      if (table.n < 2)
        {
          continue;
        }

      qelg (&table, &reseps, &erreps);

      ktmin++;

      if (ktmin >= 15 && err_ext < 0.001 * total_error)
        {
          error_type = 4;
        }

      if (erreps < err_ext)
        {
          ktmin = 0;
          err_ext = erreps;
          res_ext = reseps;

          if (err_ext + 10 * correc <= epsabs)
            break;
          if (err_ext <= epsabs && 10 * correc >= epsabs)
            break;
        }

    }

  if (iteration == limit)
    error_type = 1;

  if (err_ext == GSL_DBL_MAX)
    goto compute_result;

  err_ext = err_ext + 10 * correc;

  *result = res_ext;
  *abserr = err_ext;

  if (error_type == 0)
    {
      return GSL_SUCCESS ;
    }

  if (res_ext != 0.0 && area != 0.0)
    {
      if (err_ext / fabs (res_ext) > errsum / fabs (area))
        goto compute_result;
    }
  else if (err_ext > errsum)
    {
      goto compute_result;
    }
  else if (area == 0.0)
    {
      goto return_error;
    }

  if (error_type == 4)
    {
      err_ext = err_ext + truncation_error;
    }

  goto return_error;

compute_result:

  *result = area;
  *abserr = total_error;

return_error:

  if (error_type > 2)
    error_type--;

  if (error_type == 0)
    {
      return GSL_SUCCESS;
    }
  else if (error_type == 1)
    {
      GSL_ERROR ("number of iterations was insufficient", GSL_EMAXITER);
    }
  else if (error_type == 2)
    {
      GSL_ERROR ("cannot reach tolerance because of roundoff error",
                 GSL_EROUND);
    }
  else if (error_type == 3)
    {
      GSL_ERROR ("bad integrand behavior found in the integration interval",
                 GSL_ESING);
    }
  else if (error_type == 4)
    {
      GSL_ERROR ("roundoff error detected in the extrapolation table",
                 GSL_EROUND);
    }
  else if (error_type == 5)
    {
      GSL_ERROR ("integral is divergent, or slowly convergent",
                 GSL_EDIVERGE);
    }
  else
    {
      GSL_ERROR ("could not integrate function", GSL_EFAILED);
    }

}

