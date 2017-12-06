/* gsl_histogram_calloc_range.c
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
 * File gsl_histogram_calloc_range.c: 
 * Routine to create a variable binning histogram providing 
 * an input range vector. Need GSL library and header.
 * Do range check and allocate the histogram data. 
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_errno.h"
#include "gsl_histogram.h"

gsl_histogram *
gsl_histogram_calloc_range (size_t n, double *range)
{
  size_t i;
  gsl_histogram *h;

  /* check arguments */

  if (n == 0)
    {
      GSL_ERROR_VAL ("histogram length n must be positive integer",
                        GSL_EDOM, 0);
    }

  /* check ranges */

  for (i = 0; i < n; i++)
    {
      if (range[i] >= range[i + 1])
        {
          GSL_ERROR_VAL ("histogram bin extremes  must be "
                            "in increasing order", GSL_EDOM, 0);
        }
    }

  /* Allocate histogram  */

  h = (gsl_histogram *) malloc (sizeof (gsl_histogram));

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram struct",
                        GSL_ENOMEM, 0);
    }

  h->range = (double *) malloc ((n + 1) * sizeof (double));

  if (h->range == 0)
    {
      /* exception in constructor, avoid memory leak */
      free (h);
      GSL_ERROR_VAL ("failed to allocate space for histogram ranges",
                        GSL_ENOMEM, 0);
    }

  h->bin = (double *) malloc (n * sizeof (double));

  if (h->bin == 0)
    {
      /* exception in constructor, avoid memory leak */
      free (h->range);
      free (h);
      GSL_ERROR_VAL ("failed to allocate space for histogram bins",
                        GSL_ENOMEM, 0);
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

  h->n = n;

  return h;
}
