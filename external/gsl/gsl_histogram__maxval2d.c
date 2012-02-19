/* gsl_histogram2d_maxval.c
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
 * File gsl_histogram2d_maxval.c: 
 * Routine to find maximum and minumum content of a 2D hisogram. 
 * Need GSL library and header.
 * Contains the routines:
 * gsl_histogram2d_max_val find max content values
 * gsl_histogram2d_min_val find min content values
 * gsl_histogram2d_bin_max find coordinates of max contents bin
 * gsl_histogram2d_bin_min find coordinates of min contents bin
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include "gsl_errno.h"
#include "gsl_histogram2d.h"
/*
 * Return the maximum contents value of a 2D histogram
 */
double
gsl_histogram2d_max_val (const gsl_histogram2d * h)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  double max = h->bin[0 * ny + 0];

  for (i = 0; i < nx * ny; i++)
    {
      if (h->bin[i] > max)
        {
          max = h->bin[i];
        }
    }
  return max;
}

/*
 * Find the bin index for maximum value of a 2D histogram
 */

void
gsl_histogram2d_max_bin (const gsl_histogram2d * h, size_t * imax_out, size_t * jmax_out)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t imax = 0, jmax = 0, i, j;
  double max = h->bin[0 * ny + 0];

  for (i = 0; i < nx; i++)
    {
      for (j = 0; j < ny; j++)
        {
          double x = h->bin[i * ny + j];

          if (x > max)
            {
              max = x;
              imax = i;
              jmax = j;
            }
        }
    }

  *imax_out = imax;
  *jmax_out = jmax;
}

/*
 * Return the minimum contents value of a 2D histogram
 */

double
gsl_histogram2d_min_val (const gsl_histogram2d * h)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t i;
  double min = h->bin[0 * ny + 0];

  for (i = 0; i < nx * ny; i++)
    {
      if (h->bin[i] < min)
        {
          min = h->bin[i];
        }
    }

  return min;
}
/*
 * Find the bin index for minimum value of a 2D histogram
 */
void
gsl_histogram2d_min_bin (const gsl_histogram2d * h, size_t * imin_out, size_t * jmin_out)
{
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  size_t imin = 0, jmin = 0, i, j;
  double min = h->bin[0 * ny + 0];

  for (i = 0; i < nx; i++)
    {
      for (j = 0; j < ny; j++)
        {
          double x = h->bin[i * ny + j];

          if (x < min)
            {
              min = x;
              imin = i;
              jmin = j;
            }
        }
    }

  *imin_out = imin;
  *jmin_out = jmin;
}

