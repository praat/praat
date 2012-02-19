/* gsl_histogram2d_oper.c
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
 * File gsl_histogram2d_oper.c: 
 * Routine to make operation on 2D histograms. 
 * Need GSL library and header.
 * Contains the routines:
 * gsl_histogram2d_same_binning check if two histograms have the same binning 
 * gsl_histogram2d_add          add two histogram
 * gsl_histogram2d_sub          subctract two histogram
 * gsl_histogram2d_mult         multiply two histogram
 * gsl_histogram2d_div          divide two histogram
 * gsl_histogram2d_scale        scale histogram contents
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
 * gsl_histogram2d_same_binning:
 * control if two histogram have the
 * same binning
 */
int
gsl_histogram2d_equal_bins_p (const gsl_histogram2d * h1,
                              const gsl_histogram2d * h2)
{

  if ((h1->nx != h2->nx) || (h1->ny != h2->ny))
    {
      return 0;
    }
  {
    size_t i;
    /* init ranges */
    for (i = 0; i <= (h1->nx); i++)
      {
        if (h1->xrange[i] != h2->xrange[i])
          {
            return 0;
          }
      }
    for (i = 0; i <= (h1->ny); i++)
      {
        if (h1->yrange[i] != h2->yrange[i])
          {
            return 0;
          }
      }
  }
  return 1;
}

/* 
 * gsl_histogram2d_add:
 * add two histogram
 */

int 
gsl_histogram2d_add (gsl_histogram2d * h1, const gsl_histogram2d * h2)
{
  size_t i;

  if (!gsl_histogram2d_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < (h1->nx) * (h1->ny); i++)
    {
      h1->bin[i] += h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram2d_sub:
 * subtract two histogram
 */

int 
gsl_histogram2d_sub (gsl_histogram2d * h1, const gsl_histogram2d * h2)
{
  size_t i;

  if (!gsl_histogram2d_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < (h1->nx) * (h1->ny); i++)
    {
      h1->bin[i] -= h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram2d_mult:
 * multiply two histogram
 */

int 
gsl_histogram2d_mul (gsl_histogram2d * h1, const gsl_histogram2d * h2)
{
  size_t i;

  if (!gsl_histogram2d_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < (h1->nx) * (h1->ny); i++)
    {
      h1->bin[i] *= h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram2d_div:
 * divide two histogram
 */

int 
gsl_histogram2d_div (gsl_histogram2d * h1, const gsl_histogram2d * h2)
{
  size_t i;

  if (!gsl_histogram2d_equal_bins_p (h1, h2))
    {
      GSL_ERROR ("histograms have different binning", GSL_EINVAL);
    }

  for (i = 0; i < (h1->nx) * (h1->ny); i++)
    {
      h1->bin[i] /= h2->bin[i];
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_histogram2d_scale:
 * scale a histogram by a numeric factor
 */

int 
gsl_histogram2d_scale (gsl_histogram2d * h, double scale)
{
  size_t i;

  for (i = 0; i < (h->nx) * (h->ny); i++)
    {
      h->bin[i] *= scale;
    }
  
  return GSL_SUCCESS;
}

/* 
 * gsl_histogram2d_shift:
 * shift a histogram by a numeric offset
 */

int 
gsl_histogram2d_shift (gsl_histogram2d * h, double shift)
{
  size_t i;

  for (i = 0; i < (h->nx) * (h->ny); i++)
    {
      h->bin[i] += shift;
    }
  
  return GSL_SUCCESS;
}

