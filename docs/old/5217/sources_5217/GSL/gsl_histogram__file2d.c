/* histogram/file2d.c
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
#include "gsl_histogram2d.h"

int
gsl_histogram2d_fread (FILE * stream, gsl_histogram2d * h)
{
  int status = gsl_block_raw_fread (stream, h->xrange, h->nx + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fread (stream, h->yrange, h->ny + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fread (stream, h->bin, h->nx * h->ny, 1);

  return status;
}

int
gsl_histogram2d_fwrite (FILE * stream, const gsl_histogram2d * h)
{
  int status = gsl_block_raw_fwrite (stream, h->xrange, h->nx + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fwrite (stream, h->yrange, h->ny + 1, 1);

  if (status)
    return status;

  status = gsl_block_raw_fwrite (stream, h->bin, h->nx * h->ny, 1);
  return status;
}

int
gsl_histogram2d_fprintf (FILE * stream, const gsl_histogram2d * h,
                         const char *range_format, const char *bin_format)
{
  size_t i, j;
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  int status;

  for (i = 0; i < nx; i++)
    {
      for (j = 0; j < ny; j++)
        {
          status = fprintf (stream, range_format, h->xrange[i]);

          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }

          status = putc (' ', stream);

          if (status == EOF)
            {
              GSL_ERROR ("putc failed", GSL_EFAILED);
            }

          status = fprintf (stream, range_format, h->xrange[i + 1]);

          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }

          status = putc (' ', stream);

          if (status == EOF)
            {
              GSL_ERROR ("putc failed", GSL_EFAILED);
            }

          status = fprintf (stream, range_format, h->yrange[j]);

          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }

          status = putc (' ', stream);

          if (status == EOF)
            {
              GSL_ERROR ("putc failed", GSL_EFAILED);
            }

          status = fprintf (stream, range_format, h->yrange[j + 1]);

          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }

          status = putc (' ', stream);

          if (status == EOF)
            {
              GSL_ERROR ("putc failed", GSL_EFAILED);
            }

          status = fprintf (stream, bin_format, h->bin[i * ny + j]);

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
      status = putc ('\n', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }
    }

  return GSL_SUCCESS;
}

int
gsl_histogram2d_fscanf (FILE * stream, gsl_histogram2d * h)
{
  size_t i, j;
  const size_t nx = h->nx;
  const size_t ny = h->ny;
  double xupper, yupper;

  for (i = 0; i < nx; i++)
    {
      for (j = 0; j < ny; j++)
        {
          int status = fscanf (stream,
                               "%lg %lg %lg %lg %lg",
                               h->xrange + i, &xupper,
                               h->yrange + j, &yupper,
                               h->bin + i * ny + j);

          if (status != 5)
            {
              GSL_ERROR ("fscanf failed", GSL_EFAILED);
            }
        }
      h->yrange[ny] = yupper;
    }

  h->xrange[nx] = xupper;

  return GSL_SUCCESS;
}
