/* permutation/file.c
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
#include "gsl_permutation.h"

#define IN_FORMAT "%lu"

int
gsl_permutation_fread (FILE * stream, gsl_permutation * p)
{
  size_t n = p->size ;

  size_t * data = p->data ;

  size_t items = fread (data, sizeof (size_t), n, stream);
  
  if (items != n)
    {
      GSL_ERROR ("fread failed", GSL_EFAILED);
    }
      
  return GSL_SUCCESS;
}

int
gsl_permutation_fwrite (FILE * stream, const gsl_permutation * p)
{
  size_t n = p->size ;

  size_t * data = p->data ;
  
  size_t items = fwrite (data, sizeof (size_t), n, stream);
  
  if (items != n)
    {
      GSL_ERROR ("fwrite failed", GSL_EFAILED);
    }

  return GSL_SUCCESS;
}

int
gsl_permutation_fprintf (FILE * stream, const gsl_permutation * p, const char *format)
{
  size_t n = p->size ;
  
  size_t * data = p->data ;
  
  size_t i;

  for (i = 0; i < n; i++)
    {
      int status = fprintf (stream, format, data[i]);

      if (status < 0)
        {
          GSL_ERROR ("fprintf failed", GSL_EFAILED);
        }
    }

  return GSL_SUCCESS;
}

int
gsl_permutation_fscanf (FILE * stream, gsl_permutation * p)
{
  size_t n = p->size ;
  
  size_t * data = p->data ;

  size_t i;

  for (i = 0; i < n; i++)
    {
      unsigned long j ;  

      /* FIXME: what if size_t != unsigned long ??? 

         want read in size_t but have to read in unsigned long to avoid
         error from compiler */

      int status = fscanf (stream, IN_FORMAT, &j);  

      if (status != 1)
        {
          GSL_ERROR ("fscanf failed", GSL_EFAILED);
        }

      data[i] = j;
    }

  return GSL_SUCCESS;
}

