/* gsl_histogram_maxval.c
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
 * File gsl_histogram_maxval.c: 
 * Routine to find maximum and minumum content of a hisogram. 
 * Need GSL library and header.
 * Contains the routines:
 * gsl_histogram_max_val find max content values
 * gsl_histogram_min_val find min content values
 * gsl_histogram_bin_max find coordinates of max contents bin
 * gsl_histogram_bin_min find coordinates of min contents bin
 *
 * Author: S. Piccardi
 * Jan. 2000
 *
 ***************************************************************/
#include "gsl__config.h"
#include "gsl_errno.h"
#include "gsl_histogram.h"

double
gsl_histogram_max_val (const gsl_histogram * h)
{
  const size_t n = h->n;
  size_t i;
  double max = h->bin[0];
  for (i = 0; i < n; i++)
    {
      if (h->bin[i] > max)
        {
          max = h->bin[i];
        }
    }
  return max;
}

size_t
gsl_histogram_max_bin (const gsl_histogram * h)
{
  size_t i;
  size_t imax = 0;
  double max = h->bin[0];
  for (i = 0; i < h->n; i++)
    {
      if (h->bin[i] > max)
        {
          max = h->bin[i];
          imax = i;
        }
    }
  return imax;
}

double
gsl_histogram_min_val (const gsl_histogram * h)
{
  size_t i;
  double min = h->bin[0];
  for (i = 0; i < h->n; i++)
    {
      if (h->bin[i] < min)
        {
          min = h->bin[i];
        }
    }
  return min;
}

size_t
gsl_histogram_min_bin (const gsl_histogram * h)
{
  size_t i;
  size_t imin = 0;
  double min = h->bin[0];
  for (i = 0; i < h->n; i++)
    {
      if (h->bin[i] < min)
        {
          min = h->bin[i];
          imin = i;
        }
    }
  return imin;
}
