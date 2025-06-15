/* histogram/get2d.c
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
#include "gsl_errno.h"
#include "gsl_histogram2d.h"

#include "gsl_histogram__find.c"

double
gsl_histogram2d_get (const gsl_histogram2d * h, const size_t i, const size_t j)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;

  if (i >= nx)
    {
      GSL_ERROR_VAL ("index i lies outside valid range of 0 .. nx - 1",
                        GSL_EDOM, 0);
    }

  if (j >= ny)
    {
      GSL_ERROR_VAL ("index j lies outside valid range of 0 .. ny - 1",
                        GSL_EDOM, 0);
    }

  return h->bin[i * ny + j];
}

int
gsl_histogram2d_get_xrange (const gsl_histogram2d * h, const size_t i,
                            double *xlower, double *xupper)
{
  const size_t nx = h->nx;

  if (i >= nx)
    {
      GSL_ERROR ("index i lies outside valid range of 0 .. nx - 1", GSL_EDOM);
    }

  *xlower = h->xrange[i];
  *xupper = h->xrange[i + 1];

  return GSL_SUCCESS;
}

int
gsl_histogram2d_get_yrange (const gsl_histogram2d * h, const size_t j,
                            double *ylower, double *yupper)
{
  const size_t ny = h->ny;

  if (j >= ny)
    {
      GSL_ERROR ("index j lies outside valid range of 0 .. ny - 1", GSL_EDOM);
    }

  *ylower = h->yrange[j];
  *yupper = h->yrange[j + 1];

  return GSL_SUCCESS;
}

int
gsl_histogram2d_find (const gsl_histogram2d * h,
                      const double x, const double y,
                      size_t * i, size_t * j)
{
  int status = find (h->nx, h->xrange, x, i);

  if (status)
    {
      GSL_ERROR ("x not found in range of h", GSL_EDOM);
    }

  status = find (h->ny, h->yrange, y, j);

  if (status)
    {
      GSL_ERROR ("y not found in range of h", GSL_EDOM);
    }

  return GSL_SUCCESS;
}
