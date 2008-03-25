/* histogram/params2d.c
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

double
gsl_histogram2d_xmax (const gsl_histogram2d * h)
{
  const int nx = h->nx;
  return h->xrange[nx];
}

double
gsl_histogram2d_xmin (const gsl_histogram2d * h)
{
  return h->xrange[0];
}

double
gsl_histogram2d_ymax (const gsl_histogram2d * h)
{
  const int ny = h->ny;
  return h->yrange[ny];
}

double
gsl_histogram2d_ymin (const gsl_histogram2d * h)
{
  return h->yrange[0];
}

size_t
gsl_histogram2d_nx (const gsl_histogram2d * h)
{
  return h->nx;
}

size_t
gsl_histogram2d_ny (const gsl_histogram2d * h)
{
  return h->ny;
}
