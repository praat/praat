/* gsl_histogram2d_calloc_range.c
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
 * File gsl_histogram2d_calloc_range.c: 
 * Routine to create a variable binning 2D histogram providing 
 * the input range vectors. Need GSL library and header.
 * Do range check and allocate the histogram data. 
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_errno.h"
#include "gsl_histogram2d.h"
/*
 * Routine that create a 2D histogram using the given 
 * values for X and Y ranges
 */
gsl_histogram2d *
gsl_histogram2d_calloc_range (size_t nx, size_t ny,
                              double *xrange,
                              double *yrange)
{
  size_t i, j;
  gsl_histogram2d *h;

  /* check arguments */

  if (nx == 0)
    {
      GSL_ERROR_VAL ("histogram length nx must be positive integer",
                        GSL_EDOM, 0);
    }

  if (ny == 0)
    {
      GSL_ERROR_VAL ("histogram length ny must be positive integer",
                        GSL_EDOM, 0);
    }

  /* init ranges */

  for (i = 0; i < nx; i++)
    {
      if (xrange[i] >= xrange[i + 1])
        {
          GSL_ERROR_VAL ("histogram xrange not in increasing order",
                            GSL_EDOM, 0);
        }
    }

  for (j = 0; j < ny; j++)
    {
      if (yrange[j] >= yrange[j + 1])
        {
          GSL_ERROR_VAL ("histogram yrange not in increasing order"
                            ,GSL_EDOM, 0);
        }
    }

  /* Allocate histogram  */

  h = (gsl_histogram2d *) malloc (sizeof (gsl_histogram2d));

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram struct",
                        GSL_ENOMEM, 0);
    }

  h->xrange = (double *) malloc ((nx + 1) * sizeof (double));

  if (h->xrange == 0)
    {
      /* exception in constructor, avoid memory leak */
      free (h);

      GSL_ERROR_VAL ("failed to allocate space for histogram xrange",
                        GSL_ENOMEM, 0);
    }

  h->yrange = (double *) malloc ((ny + 1) * sizeof (double));

  if (h->yrange == 0)
    {
      /* exception in constructor, avoid memory leak */
      free (h);

      GSL_ERROR_VAL ("failed to allocate space for histogram yrange",
                        GSL_ENOMEM, 0);
    }

  h->bin = (double *) malloc (nx * ny * sizeof (double));

  if (h->bin == 0)
    {
      /* exception in constructor, avoid memory leak */
      free (h->xrange);
      free (h->yrange);
      free (h);

      GSL_ERROR_VAL ("failed to allocate space for histogram bins",
                        GSL_ENOMEM, 0);
    }

  /* init histogram */

  /* init ranges */

  for (i = 0; i <= nx; i++)
    {
      h->xrange[i] = xrange[i];
    }

  for (j = 0; j <= ny; j++)
    {
      h->yrange[j] = yrange[j];
    }

  /* clear contents */

  for (i = 0; i < nx; i++)
    {
      for (j = 0; j < ny; j++)
        {
          h->bin[i * ny + j] = 0;
        }
    }

  h->nx = nx;
  h->ny = ny;

  return h;
}
