/* gsl_histogram_copy.c
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
 * File gsl_histogram_copy.c: 
 * Routine to copy an histogram. 
 * Need GSL library and headers.
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_errno.h"
#include "gsl_histogram.h"

/*
 * gsl_histogram_copy:
 * copy the contents of an histogram into another
 */

int
gsl_histogram_memcpy (gsl_histogram * dest, const gsl_histogram * src)
{
  size_t n = src->n;
  size_t i;

  if (dest->n != src->n)
    {
      GSL_ERROR ("histograms have different sizes, cannot copy",
                 GSL_EINVAL);
    }

  for (i = 0; i <= n; i++)
    {
      dest->range[i] = src->range[i];
    }

  for (i = 0; i < n; i++)
    {
      dest->bin[i] = src->bin[i];
    }

  return GSL_SUCCESS;
}

/*
 * gsl_histogram_duplicate:
 * duplicate an histogram creating
 * an identical new one
 */

gsl_histogram *
gsl_histogram_clone (const gsl_histogram * src)
{
  size_t n = src->n;
  size_t i;
  gsl_histogram *h;

  h = gsl_histogram_calloc_range (n, src->range);

  if (h == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for histogram struct",
                        GSL_ENOMEM, 0);
    }

  for (i = 0; i < n; i++)
    {
      h->bin[i] = src->bin[i];
    }

  return h;
}
