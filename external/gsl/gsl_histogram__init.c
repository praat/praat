/* histogram/init.c
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
#include <stdlib.h>
#include "gsl_errno.h"
#include "gsl_histogram.h"

gsl_histogram *
gsl_histogram_alloc (size_t n)
{
  gsl_histogram *h;

  if (n == 0)
    {
      GSL_ERROR_VAL ("histogram length n must be positive integer",
                        GSL_EDOM, 0);
    }

  h = (gsl_histogram *) malloc (sizeof (gsl_histogram));

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram struct",
                        GSL_ENOMEM, 0);
    }

  h->range = (double *) malloc ((n + 1) * sizeof (double));

  if (h->range == 0)
    {
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram ranges",
                        GSL_ENOMEM, 0);
    }

  h->bin = (double *) malloc (n * sizeof (double));

  if (h->bin == 0)
    {
      free (h->range);
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram bins",
                        GSL_ENOMEM, 0);
    }

  h->n = n;

  return h;
}

static void
make_uniform (double range[], size_t n, double xmin, double xmax)
{
  size_t i;

  for (i = 0; i <= n; i++)
    {
      double f1 = ((double) (n-i) / (double) n);
      double f2 = ((double) i / (double) n);
      range[i] = f1 * xmin +  f2 * xmax;
    }
}

gsl_histogram *
gsl_histogram_calloc_uniform (const size_t n, const double xmin,
                              const double xmax)
{
  gsl_histogram *h;

  if (xmin >= xmax)
    {
      GSL_ERROR_VAL ("xmin must be less than xmax", GSL_EINVAL, 0);
    }

  h = gsl_histogram_calloc (n);

  if (h == 0)
    {
      return h;
    }

  make_uniform (h->range, n, xmin, xmax);

  return h;
}

gsl_histogram *
gsl_histogram_calloc (size_t n)
{
  gsl_histogram * h = gsl_histogram_alloc (n);

  if (h == 0)
    {
      return h;
    }

  {
    size_t i;

    for (i = 0; i < n + 1; i++)
      {
        h->range[i] = i;
      }

    for (i = 0; i < n; i++)
      {
        h->bin[i] = 0;
      }
  }

  h->n = n;

  return h;
}


void
gsl_histogram_free (gsl_histogram * h)
{
  free (h->range);
  free (h->bin);
  free (h);
}

/* These initialization functions suggested by Achim Gaedke */

int 
gsl_histogram_set_ranges_uniform (gsl_histogram * h, double xmin, double xmax)
{
  size_t i;
  const size_t n = h->n;

  if (xmin >= xmax)
    {
      GSL_ERROR ("xmin must be less than xmax", GSL_EINVAL);
    }

  /* initialize ranges */

  make_uniform (h->range, n, xmin, xmax);

  /* clear contents */

  for (i = 0; i < n; i++)
    {
      h->bin[i] = 0;
    }

  return GSL_SUCCESS;
}

int 
gsl_histogram_set_ranges (gsl_histogram * h, const double range[], size_t size)
{
  size_t i;
  const size_t n = h->n;

  if (size != (n+1))
    {
      GSL_ERROR ("size of range must match size of histogram", GSL_EINVAL);
    }

  /* initialize ranges */

  for (i = 0; i <= n; i++)
    {
      h->range[i] = range[i];
    }

  /* clear contents */

  for (i = 0; i < n; i++)
    {
      h->bin[i] = 0;
    }

  return GSL_SUCCESS;
}

