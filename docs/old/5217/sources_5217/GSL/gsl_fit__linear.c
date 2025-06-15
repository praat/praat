/* fit/linear.c
 * 
 * Copyright (C) 2000, 2007 Brian Gough
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
#include "gsl_errno.h"
#include "gsl_fit.h"

/* Fit the data (x_i, y_i) to the linear relationship 

   Y = c0 + c1 x

   returning, 

   c0, c1  --  coefficients
   cov00, cov01, cov11  --  variance-covariance matrix of c0 and c1,
   sumsq   --   sum of squares of residuals 

   This fit can be used in the case where the errors for the data are
   uknown, but assumed equal for all points. The resulting
   variance-covariance matrix estimates the error in the coefficients
   from the observed variance of the points around the best fit line.
*/

int
gsl_fit_linear (const double *x, const size_t xstride,
                const double *y, const size_t ystride,
                const size_t n,
                double *c0, double *c1,
                double *cov_00, double *cov_01, double *cov_11, double *sumsq)
{
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;

  size_t i;

  for (i = 0; i < n; i++)
    {
      m_x += (x[i * xstride] - m_x) / (i + 1.0);
      m_y += (y[i * ystride] - m_y) / (i + 1.0);
    }

  for (i = 0; i < n; i++)
    {
      const double dx = x[i * xstride] - m_x;
      const double dy = y[i * ystride] - m_y;

      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }

  /* In terms of y = a + b x */

  {
    double s2 = 0, d2 = 0;
    double b = m_dxdy / m_dx2;
    double a = m_y - m_x * b;

    *c0 = a;
    *c1 = b;

    /* Compute chi^2 = \sum (y_i - (a + b * x_i))^2 */

    for (i = 0; i < n; i++)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        const double d = dy - b * dx;
        d2 += d * d;
      }

    s2 = d2 / (n - 2.0);        /* chisq per degree of freedom */

    *cov_00 = s2 * (1.0 / n) * (1 + m_x * m_x / m_dx2);
    *cov_11 = s2 * 1.0 / (n * m_dx2);

    *cov_01 = s2 * (-m_x) / (n * m_dx2);

    *sumsq = d2;
  }

  return GSL_SUCCESS;
}


/* Fit the weighted data (x_i, w_i, y_i) to the linear relationship 

   Y = c0 + c1 x

   returning, 

   c0, c1  --  coefficients
   s0, s1  --  the standard deviations of c0 and c1,
   r       --  the correlation coefficient between c0 and c1,
   chisq   --  weighted sum of squares of residuals */

int
gsl_fit_wlinear (const double *x, const size_t xstride,
                 const double *w, const size_t wstride,
                 const double *y, const size_t ystride,
                 const size_t n,
                 double *c0, double *c1,
                 double *cov_00, double *cov_01, double *cov_11,
                 double *chisq)
{

  /* compute the weighted means and weighted deviations from the means */

  /* wm denotes a "weighted mean", wm(f) = (sum_i w_i f_i) / (sum_i w_i) */

  double W = 0, wm_x = 0, wm_y = 0, wm_dx2 = 0, wm_dxdy = 0;

  size_t i;

  for (i = 0; i < n; i++)
    {
      const double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wm_x += (x[i * xstride] - wm_x) * (wi / W);
          wm_y += (y[i * ystride] - wm_y) * (wi / W);
        }
    }

  W = 0;                        /* reset the total weight */

  for (i = 0; i < n; i++)
    {
      const double wi = w[i * wstride];

      if (wi > 0)
        {
          const double dx = x[i * xstride] - wm_x;
          const double dy = y[i * ystride] - wm_y;

          W += wi;
          wm_dx2 += (dx * dx - wm_dx2) * (wi / W);
          wm_dxdy += (dx * dy - wm_dxdy) * (wi / W);
        }
    }

  /* In terms of y = a + b x */

  {
    double d2 = 0;
    double b = wm_dxdy / wm_dx2;
    double a = wm_y - wm_x * b;

    *c0 = a;
    *c1 = b;

    *cov_00 = (1 / W) * (1 + wm_x * wm_x / wm_dx2);
    *cov_11 = 1 / (W * wm_dx2);

    *cov_01 = -wm_x / (W * wm_dx2);

    /* Compute chi^2 = \sum w_i (y_i - (a + b * x_i))^2 */

    for (i = 0; i < n; i++)
      {
        const double wi = w[i * wstride];

        if (wi > 0)
          {
            const double dx = x[i * xstride] - wm_x;
            const double dy = y[i * ystride] - wm_y;
            const double d = dy - b * dx;
            d2 += wi * d * d;
          }
      }

    *chisq = d2;
  }

  return GSL_SUCCESS;
}



int
gsl_fit_linear_est (const double x,
                    const double c0, const double c1,
                    const double cov00, const double cov01, const double cov11,
                    double *y, double *y_err)
{
  *y = c0 + c1 * x;
  *y_err = sqrt (cov00 + x * (2 * cov01 + cov11 * x));
  return GSL_SUCCESS;
}


int
gsl_fit_mul (const double *x, const size_t xstride,
             const double *y, const size_t ystride,
             const size_t n, 
             double *c1, double *cov_11, double *sumsq)
{
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;

  size_t i;

  for (i = 0; i < n; i++)
    {
      m_x += (x[i * xstride] - m_x) / (i + 1.0);
      m_y += (y[i * ystride] - m_y) / (i + 1.0);
    }

  for (i = 0; i < n; i++)
    {
      const double dx = x[i * xstride] - m_x;
      const double dy = y[i * ystride] - m_y;

      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }

  /* In terms of y =  b x */

  {
    double s2 = 0, d2 = 0;
    double b = (m_x * m_y + m_dxdy) / (m_x * m_x + m_dx2);

    *c1 = b;

    /* Compute chi^2 = \sum (y_i -  b * x_i)^2 */

    for (i = 0; i < n; i++)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        const double d = (m_y - b * m_x) + dy - b * dx;
        d2 += d * d;
      }

    s2 = d2 / (n - 1.0);        /* chisq per degree of freedom */

    *cov_11 = s2 * 1.0 / (n * (m_x * m_x + m_dx2));

    *sumsq = d2;
  }

  return GSL_SUCCESS;
}


int
gsl_fit_wmul (const double *x, const size_t xstride,
              const double *w, const size_t wstride,
              const double *y, const size_t ystride,
              const size_t n, 
              double *c1, double *cov_11, double *chisq)
{

  /* compute the weighted means and weighted deviations from the means */

  /* wm denotes a "weighted mean", wm(f) = (sum_i w_i f_i) / (sum_i w_i) */

  double W = 0, wm_x = 0, wm_y = 0, wm_dx2 = 0, wm_dxdy = 0;

  size_t i;

  for (i = 0; i < n; i++)
    {
      const double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wm_x += (x[i * xstride] - wm_x) * (wi / W);
          wm_y += (y[i * ystride] - wm_y) * (wi / W);
        }
    }

  W = 0;                        /* reset the total weight */

  for (i = 0; i < n; i++)
    {
      const double wi = w[i * wstride];

      if (wi > 0)
        {
          const double dx = x[i * xstride] - wm_x;
          const double dy = y[i * ystride] - wm_y;

          W += wi;
          wm_dx2 += (dx * dx - wm_dx2) * (wi / W);
          wm_dxdy += (dx * dy - wm_dxdy) * (wi / W);
        }
    }

  /* In terms of y = b x */

  {
    double d2 = 0;
    double b = (wm_x * wm_y + wm_dxdy) / (wm_x * wm_x + wm_dx2);

    *c1 = b;

    *cov_11 = 1 / (W * (wm_x * wm_x + wm_dx2));

    /* Compute chi^2 = \sum w_i (y_i - b * x_i)^2 */

    for (i = 0; i < n; i++)
      {
        const double wi = w[i * wstride];

        if (wi > 0)
          {
            const double dx = x[i * xstride] - wm_x;
            const double dy = y[i * ystride] - wm_y;
            const double d = (wm_y - b * wm_x) + (dy - b * dx);
            d2 += wi * d * d;
          }
      }

    *chisq = d2;
  }

  return GSL_SUCCESS;
}

int
gsl_fit_mul_est (const double x, 
                 const double c1, const double cov11, 
                 double *y, double *y_err)
{
  *y = c1 * x;
  *y_err = sqrt (cov11) * fabs (x);
  return GSL_SUCCESS;
}
