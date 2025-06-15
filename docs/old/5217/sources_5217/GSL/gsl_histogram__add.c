/* histogram/add.c
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
#include "gsl_histogram.h"

#include "gsl_histogram__find.c"

int
gsl_histogram_increment (gsl_histogram * h, double x)
{
  int status = gsl_histogram_accumulate (h, x, 1.0);
  return status;
}

int
gsl_histogram_accumulate (gsl_histogram * h, double x, double weight)
{
  const size_t n = h->n;
  size_t index = 0;

  int status = find (h->n, h->range, x, &index);

  if (status)
    {
      return GSL_EDOM;
    }

  if (index >= n)
    {
      GSL_ERROR ("index lies outside valid range of 0 .. n - 1",
                 GSL_ESANITY);
    }

  h->bin[index] += weight;

  return GSL_SUCCESS;
}
