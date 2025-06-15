/* gsl_histogram_oper.c
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
 * File gsl_histogram_oper.c: 
 * Routine to make operation on histograms. 
 * Need GSL library and header.
 * Contains the routines:
 * gsl_histogram_same_binning check if two histograms have the same binning 
 * gsl_histogram_add          add two histograms
 * gsl_histogram_sub          subctract two histograms
 * gsl_histogram_mult         multiply two histograms
 * gsl_histogram_div          divide two histograms
 * gsl_histogram_scale        scale histogram contents
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
 * gsl_histogram_same_binning:
 * control if two histograms have the
 * same binning
 */

int
gsl_histogram_equal_bins_p (const gsl_histogram * h1, const gsl_histogram * h2)
{
  if (h1->n != h2->n)
    {
      return 0;
    }

  {
    size_t i;
    /* init ranges */

    for (i = 0; i <= h1->n; i++)
      {
        if (h1->range[i] != h2->range[i])
          {
            return 0;
          }
      }
  }

  return 1;
}

/* 
 * gsl_histogram_add:
 * add two histograms
 */
int 
gsl_histogram_add (gsl_histogram * h1, const gsl_histogram * h2)
{
  size_t i;

  if (!gsl_histogram_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < h1->n; i++)
    {
      h1->bin[i] += h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram_sub:
 * subtract two histograms
 */

int 
gsl_histogram_sub (gsl_histogram * h1, const gsl_histogram * h2)
{
  size_t i;

  if (!gsl_histogram_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < h1->n; i++)
    {
      h1->bin[i] -= h2->bin[i];
    }

  return GSL_SUCCESS;

}

/* 
 * gsl_histogram_mult:
 * multiply two histograms
 */

int 
gsl_histogram_mul (gsl_histogram * h1, const gsl_histogram * h2)
{
  size_t i;

  if (!gsl_histogram_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < h1->n; i++)
    {
      h1->bin[i] *= h2->bin[i];
    }

  return GSL_SUCCESS;
}
/* 
 * gsl_histogram_div:
 * divide two histograms
 */
int 
gsl_histogram_div (gsl_histogram * h1, const gsl_histogram * h2)
{
  size_t i;

  if (!gsl_histogram_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < h1->n; i++)
    {
      h1->bin[i] /= h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram_scale:
 * scale a histogram by a numeric factor
 */

int 
gsl_histogram_scale (gsl_histogram * h, double scale)
{
  size_t i;

  for (i = 0; i < h->n; i++)
    {
      h->bin[i] *= scale;
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram_shift:
 * shift a histogram by a numeric offset
 */

int 
gsl_histogram_shift (gsl_histogram * h, double shift)
{
  size_t i;

  for (i = 0; i < h->n; i++)
    {
      h->bin[i] += shift;
    }

  return GSL_SUCCESS;
}
