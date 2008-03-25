/* histogram/file.c
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
#include <stdio.h>
#include "gsl_errno.h"
#include "gsl_block.h"
#include "gsl_histogram.h"

int
gsl_histogram_fread (FILE * stream, gsl_histogram * h)
{
  int status = gsl_block_raw_fread (stream, h->range, h->n + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fread (stream, h->bin, h->n, 1);
  return status;
}

int
gsl_histogram_fwrite (FILE * stream, const gsl_histogram * h)
{
  int status = gsl_block_raw_fwrite (stream, h->range, h->n + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fwrite (stream, h->bin, h->n, 1);
  return status;
}

int
gsl_histogram_fprintf (FILE * stream, const gsl_histogram * h,
                       const char *range_format, const char *bin_format)
{
  size_t i;
  const size_t n = h->n;

  for (i = 0; i < n; i++)
    {
      int status = fprintf (stream, range_format, h->range[i]);

      if (status < 0)
        {
          GSL_ERROR ("fprintf failed", GSL_EFAILED);
        }

      status = putc (' ', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }

      status = fprintf (stream, range_format, h->range[i + 1]);

      if (status < 0)
        {
          GSL_ERROR ("fprintf failed", GSL_EFAILED);
        }

      status = putc (' ', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }

      status = fprintf (stream, bin_format, h->bin[i]);

      if (status < 0)
        {
          GSL_ERROR ("fprintf failed", GSL_EFAILED);
        }

      status = putc ('\n', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }
    }

  return GSL_SUCCESS;
}

int
gsl_histogram_fscanf (FILE * stream, gsl_histogram * h)
{
  size_t i;
  const size_t n = h->n;
  double upper;

  for (i = 0; i < n; i++)
    {
      int status = fscanf (stream,
                           "%lg %lg %lg", h->range + i, &upper,
                           h->bin + i);

      if (status != 3)
        {
          GSL_ERROR ("fscanf failed", GSL_EFAILED);
        }
    }

  h->range[n] = upper;

  return GSL_SUCCESS;
}
