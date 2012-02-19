/* histogram/ntuple.c
 * 
 * Copyright (C) 2000 Simone Piccardi
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

/* Jan/2001 Modified by Brian Gough. Minor changes for GSL */

#include "gsl__config.h"
#include <errno.h>
#include "gsl_errno.h"
#include "gsl_ntuple.h"

/* 
 * gsl_ntuple_open:
 * Initialize an ntuple structure and create the related file
 */

gsl_ntuple *
gsl_ntuple_create (char *filename, void *ntuple_data, size_t size)
{
  gsl_ntuple *ntuple = (gsl_ntuple *)malloc (sizeof (gsl_ntuple));

  if (ntuple == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for ntuple struct",
                     GSL_ENOMEM, 0);
    }

  ntuple->ntuple_data = ntuple_data;
  ntuple->size = size;

  ntuple->file = fopen (filename, "wb");

  if (ntuple->file == 0)
    {
      free (ntuple);
      GSL_ERROR_VAL ("unable to create ntuple file", GSL_EFAILED, 0);
    }

  return ntuple;
}

/* 
 * gsl_ntuple_open:
 * Initialize an ntuple structure and open the related file
 */

gsl_ntuple *
gsl_ntuple_open (char *filename, void *ntuple_data, size_t size)
{
  gsl_ntuple *ntuple = (gsl_ntuple *)malloc (sizeof (gsl_ntuple));

  if (ntuple == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for ntuple struct",
                     GSL_ENOMEM, 0);
    }

  ntuple->ntuple_data = ntuple_data;
  ntuple->size = size;

  ntuple->file = fopen (filename, "rb");

  if (ntuple->file == 0)
    {
      free (ntuple);
      GSL_ERROR_VAL ("unable to open ntuple file for reading", 
                     GSL_EFAILED, 0);
    }

  return ntuple;
}

/* 
 * gsl_ntuple_write:
 * write to file a data row, must be used in a loop!
 */

int
gsl_ntuple_write (gsl_ntuple * ntuple)
{
  size_t nwrite;

  nwrite = fwrite (ntuple->ntuple_data, ntuple->size,
                   1, ntuple->file);

  if (nwrite != 1)
    {
      GSL_ERROR ("failed to write ntuple entry to file", GSL_EFAILED);
    }

  return GSL_SUCCESS;
}

/* the following function is a synonym for gsl_ntuple_write */

int
gsl_ntuple_bookdata (gsl_ntuple * ntuple)
{
  return gsl_ntuple_write (ntuple);
}

/* 
 * gsl_ntuple_read:
 * read form file a data row, must be used in a loop!
 */

int
gsl_ntuple_read (gsl_ntuple * ntuple)
{
  size_t nread;

  nread = fread (ntuple->ntuple_data, ntuple->size, 1, ntuple->file);

  if (nread == 0 && feof(ntuple->file))
    {
      return GSL_EOF;
    }

  if (nread != 1)
    {
      GSL_ERROR ("failed to read ntuple entry from file", GSL_EFAILED);
    }

  return GSL_SUCCESS;
}

/* 
 * gsl_ntuple_project:
 * fill an histogram with an ntuple file contents, use
 * SelVal and SelFunc user defined functions to get 
 * the value to book and the selection funtion
 */

#define EVAL(f,x) ((*((f)->function))(x,(f)->params))

int
gsl_ntuple_project (gsl_histogram * h, gsl_ntuple * ntuple,
                    gsl_ntuple_value_fn * value_func, 
                    gsl_ntuple_select_fn * select_func)
{
  size_t nread;

  do
    {
      nread = fread (ntuple->ntuple_data, ntuple->size,
                     1, ntuple->file);

      if (nread == 0 && feof(ntuple->file))
        {
          break ;
        }
      
      if (nread != 1) 
        {
          GSL_ERROR ("failed to read ntuple for projection", GSL_EFAILED);
        }

      if (EVAL(select_func, ntuple->ntuple_data))
        {
          gsl_histogram_increment (h, EVAL(value_func, ntuple->ntuple_data));
        }
    }
  while (1);

  return GSL_SUCCESS;
}


/* 
 * gsl_ntuple_close:
 * close the ntuple file and free the memory
 */

int
gsl_ntuple_close (gsl_ntuple * ntuple)
{
  int status = fclose (ntuple->file);
  
  if (status)
    {
      GSL_ERROR ("failed to close ntuple file", GSL_EFAILED);
    }

  free (ntuple);

  return GSL_SUCCESS;
}
