/* gsl_histogram_stat.c
 * Copyright (C) 2000  Simone Piccardi
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
 * File gsl_histogram_stat.c: 
 * Routines for statisticalcomputations on histograms. 
 * Need GSL library and header.
 * Contains the routines:
 * gsl_histogram_mean    compute histogram mean
 * gsl_histogram_sigma   compute histogram sigma
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include <stdlib.h>
#include <math.h>
#include "gsl_errno.h"
#include "gsl_histogram.h"

/* FIXME: We skip negative values in the histogram h->bin[i] < 0,
   since those correspond to negative weights (BJG) */

double
gsl_histogram_mean (const gsl_histogram * h)
{
  const size_t n = h->n;
  size_t i;

  /* Compute the bin-weighted arithmetic mean M of a histogram using the
     recurrence relation

     M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n))) 
     W(n) = W(n-1) + w(n)

   */

  long double wmean = 0;
  long double W = 0;

  for (i = 0; i < n; i++)
    {
      double xi = (h->range[i + 1] + h->range[i]) / 2;
      double wi = h->bin[i];

      if (wi > 0)
        {
          W += wi;
          wmean += (xi - wmean) * (wi / W);
        }
    }

  return wmean;
}

double
gsl_histogram_sigma (const gsl_histogram * h)
{
  const size_t n = h->n;
  size_t i;

  long double wvariance = 0 ;
  long double wmean = 0;
  long double W = 0;

  /* FIXME: should use a single pass formula here, as given in
     N.J.Higham 'Accuracy and Stability of Numerical Methods', p.12 */

  /* Compute the mean */

  for (i = 0; i < n; i++)
    {
      double xi = (h->range[i + 1] + h->range[i]) / 2;
      double wi = h->bin[i];

      if (wi > 0)
        {
          W += wi;
          wmean += (xi - wmean) * (wi / W);
        }
    }

  /* Compute the variance */

  W = 0.0;

  for (i = 0; i < n; i++)
    {
      double xi = ((h->range[i + 1]) + (h->range[i])) / 2;
      double wi = h->bin[i];

      if (wi > 0) {
        const long double delta = (xi - wmean);
        W += wi ;
        wvariance += (delta * delta - wvariance) * (wi / W);
      }
    }

  {
    double sigma = sqrt (wvariance) ;
    return sigma;
  }
}


/*
  sum up all bins of histogram
 */

double 
gsl_histogram_sum(const gsl_histogram * h)
{
  double sum=0;
  size_t i=0;
  size_t n;
  n=h->n;

  while(i < n)
    sum += h->bin[i++];

  return sum;
}

