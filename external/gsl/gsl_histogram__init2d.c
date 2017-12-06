/* histogram/init2d.c
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
#include <math.h>
#include "gsl_errno.h"
#include "gsl_histogram2d.h"

gsl_histogram2d *
gsl_histogram2d_alloc (const size_t nx, const size_t ny)
{
  gsl_histogram2d *h;

  if (nx == 0)
    {
      GSL_ERROR_VAL ("histogram2d length nx must be positive integer",
                        GSL_EDOM, 0);
    }

  if (ny == 0)
    {
      GSL_ERROR_VAL ("histogram2d length ny must be positive integer",
                        GSL_EDOM, 0);
    }

  h = (gsl_histogram2d *) malloc (sizeof (gsl_histogram2d));

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram2d struct",
                        GSL_ENOMEM, 0);
    }

  h->xrange = (double *) malloc ((nx + 1) * sizeof (double));

  if (h->xrange == 0)
    {
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram2d x ranges",
                        GSL_ENOMEM, 0);
    }

  h->yrange = (double *) malloc ((ny + 1) * sizeof (double));

  if (h->yrange == 0)
    {
      free (h->xrange);
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram2d y ranges",
                        GSL_ENOMEM, 0);
    }

  h->bin = (double *) malloc (nx * ny * sizeof (double));

  if (h->bin == 0)
    {
      free (h->xrange);
      free (h->yrange);
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram bins",
                        GSL_ENOMEM, 0);
    }

  h->nx = nx;
  h->ny = ny;

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

gsl_histogram2d *
gsl_histogram2d_calloc_uniform (const size_t nx, const size_t ny,
                                const double xmin, const double xmax,
                                const double ymin, const double ymax)
{
  gsl_histogram2d *h;

  if (xmin >= xmax)
    {
      GSL_ERROR_VAL ("xmin must be less than xmax", GSL_EINVAL, 0);
    }

  if (ymin >= ymax)
    {
      GSL_ERROR_VAL ("ymin must be less than ymax", GSL_EINVAL, 0);
    }

  h = gsl_histogram2d_calloc (nx, ny);

  if (h == 0)
    {
      return h;
    }

  make_uniform (h->xrange, nx, xmin, xmax);
  make_uniform (h->yrange, ny, ymin, ymax);

  return h;
}

gsl_histogram2d *
gsl_histogram2d_calloc (const size_t nx, const size_t ny)
{
  gsl_histogram2d *h;

  if (nx == 0)
    {
      GSL_ERROR_VAL ("histogram2d length nx must be positive integer",
                        GSL_EDOM, 0);
    }

  if (ny == 0)
    {
      GSL_ERROR_VAL ("histogram2d length ny must be positive integer",
                        GSL_EDOM, 0);
    }

  h = (gsl_histogram2d *) malloc (sizeof (gsl_histogram2d));

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram2d struct",
                        GSL_ENOMEM, 0);
    }

  h->xrange = (double *) malloc ((nx + 1) * sizeof (double));

  if (h->xrange == 0)
    {
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram2d x ranges",
                        GSL_ENOMEM, 0);
    }

  h->yrange = (double *) malloc ((ny + 1) * sizeof (double));

  if (h->yrange == 0)
    {
      free (h->xrange);
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram2d y ranges",
                        GSL_ENOMEM, 0);
    }

  h->bin = (double *) malloc (nx * ny * sizeof (double));

  if (h->bin == 0)
    {
      free (h->xrange);
      free (h->yrange);
      free (h);         /* exception in constructor, avoid memory leak */

      GSL_ERROR_VAL ("failed to allocate space for histogram bins",
                        GSL_ENOMEM, 0);
    }

  {
    size_t i;

    for (i = 0; i < nx + 1; i++)
      {
        h->xrange[i] = i;
      }

    for (i = 0; i < ny + 1; i++)
      {
        h->yrange[i] = i;
      }

    for (i = 0; i < nx * ny; i++)
      {
        h->bin[i] = 0;
      }
  }

  h->nx = nx;
  h->ny = ny;

  return h;
}


void
gsl_histogram2d_free (gsl_histogram2d * h)
{
  free (h->xrange);
  free (h->yrange);
  free (h->bin);
  free (h);
}


int 
gsl_histogram2d_set_ranges_uniform (gsl_histogram2d * h, 
                                    double xmin, double xmax,
                                    double ymin, double ymax)
{
  size_t i;
  const size_t nx = h->nx, ny = h->ny;

  if (xmin >= xmax)
    {
      GSL_ERROR_VAL ("xmin must be less than xmax", GSL_EINVAL, 0);
    }

  if (ymin >= ymax)
    {
      GSL_ERROR_VAL ("ymin must be less than ymax", GSL_EINVAL, 0);
    }

  /* initialize ranges */

  make_uniform (h->xrange, nx, xmin, xmax);
  make_uniform (h->yrange, ny, ymin, ymax);

  /* clear contents */

  for (i = 0; i < nx * ny; i++)
    {
      h->bin[i] = 0;
    }

  return GSL_SUCCESS;
}

int 
gsl_histogram2d_set_ranges (gsl_histogram2d * h, 
                            const double xrange[], size_t xsize,
                            const double yrange[], size_t ysize)
{
  size_t i;
  const size_t nx = h->nx, ny = h->ny;

  if (xsize != (nx + 1))
    {
      GSL_ERROR_VAL ("size of xrange must match size of histogram", 
                     GSL_EINVAL, 0);
    }

  if (ysize != (ny + 1))
    {
      GSL_ERROR_VAL ("size of yrange must match size of histogram", 
                     GSL_EINVAL, 0);
    }

  /* initialize ranges */

  for (i = 0; i <= nx; i++)
    {
      h->xrange[i] = xrange[i];
    }

  for (i = 0; i <= ny; i++)
    {
      h->yrange[i] = yrange[i];
    }

  /* clear contents */

  for (i = 0; i < nx * ny; i++)
    {
      h->bin[i] = 0;
    }

  return GSL_SUCCESS;
}
