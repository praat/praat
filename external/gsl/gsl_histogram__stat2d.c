/* histogram/stat2d.c
 * Copyright (C) 2002  Achim Gaedke
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/***************************************************************
 *
 * File histogram/stat2d.c:
 * Routine to return statistical values of the content of a 2D hisogram. 
 *
 * Contains the routines:
 * gsl_histogram2d_sum sum up all bin values
 * gsl_histogram2d_xmean determine mean of x values
 * gsl_histogram2d_ymean determine mean of y values
 *
 * Author: Achim Gaedke Achim.Gaedke@zpr.uni-koeln.de
 * Jan. 2002
 *
 ***************************************************************/

#include "gsl__config.h"
#include <math.h>
#include "gsl_errno.h"
#include "gsl_histogram2d.h"

/*
  sum up all bins of histogram2d
 */

double
gsl_histogram2d_sum (const gsl_histogram2d * h)
{
  const size_t n = h->nx * h->ny;
  double sum = 0;
  size_t i = 0;

  while (i < n)
    sum += h->bin[i++];

  return sum;
}

double
gsl_histogram2d_xmean (const gsl_histogram2d * h)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  size_t j;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wmean = 0;
  long double W = 0;

  for (i = 0; i < nx; i++)
    {
      double xi = (h->xrange[i + 1] + h->xrange[i]) / 2.0;
      double wi = 0;

      for (j = 0; j < ny; j++)
        {
          double wij = h->bin[i * ny + j];
          if (wij > 0)
            wi += wij;
        }
      if (wi > 0)
        {
          W += wi;
          wmean += (xi - wmean) * (wi / W);
        }
    }

  return wmean;
}

double
gsl_histogram2d_ymean (const gsl_histogram2d * h)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  size_t j;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wmean = 0;
  long double W = 0;

  for (j = 0; j < ny; j++)
    {
      double yj = (h->yrange[j + 1] + h->yrange[j]) / 2.0;
      double wj = 0;

      for (i = 0; i < nx; i++)
        {
          double wij = h->bin[i * ny + j];
          if (wij > 0)
            wj += wij;
        }

      if (wj > 0)
        {
          W += wj;
          wmean += (yj - wmean) * (wj / W);
        }
    }

  return wmean;
}

double
gsl_histogram2d_xsigma (const gsl_histogram2d * h)
{
  const double xmean = gsl_histogram2d_xmean (h);
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  size_t j;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wvariance = 0;
  long double W = 0;

  for (i = 0; i < nx; i++)
    {
      double xi = (h->xrange[i + 1] + h->xrange[i]) / 2 - xmean;
      double wi = 0;

      for (j = 0; j < ny; j++)
        {
          double wij = h->bin[i * ny + j];
          if (wij > 0)
            wi += wij;
        }

      if (wi > 0)
        {
          W += wi;
          wvariance += ((xi * xi) - wvariance) * (wi / W);
        }
    }

  {
    double xsigma = sqrt (wvariance);
    return xsigma;
  }
}

double
gsl_histogram2d_ysigma (const gsl_histogram2d * h)
{
  const double ymean = gsl_histogram2d_ymean (h);
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  size_t j;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wvariance = 0;
  long double W = 0;

  for (j = 0; j < ny; j++)
    {
      double yj = (h->yrange[j + 1] + h->yrange[j]) / 2.0 - ymean;
      double wj = 0;

      for (i = 0; i < nx; i++)
        {
          double wij = h->bin[i * ny + j];
          if (wij > 0)
            wj += wij;
        }
      if (wj > 0)
        {
          W += wj;
          wvariance += ((yj * yj) - wvariance) * (wj / W);
        }
    }

  {
    double ysigma = sqrt (wvariance);
    return ysigma;
  }
}

double
gsl_histogram2d_cov (const gsl_histogram2d * h)
{
  const double xmean = gsl_histogram2d_xmean (h);
  const double ymean = gsl_histogram2d_ymean (h);
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  size_t j;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wcovariance = 0;
  long double W = 0;

  for (j = 0; j < ny; j++)
    {
      for (i = 0; i < nx; i++)
        {
          double xi = (h->xrange[i + 1] + h->xrange[i]) / 2.0 - xmean;
          double yj = (h->yrange[j + 1] + h->yrange[j]) / 2.0 - ymean;
          double wij = h->bin[i * ny + j];

          if (wij > 0)
            {
              W += wij;
              wcovariance += ((xi * yj) - wcovariance) * (wij / W);
            }
        }
    }

  return wcovariance;

}
