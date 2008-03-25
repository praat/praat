/* combination/file.c
 * based on permutation/file.c by Brian Gough
 * 
 * Copyright (C) 2001 Szymon Jaroszewicz
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
#include "gsl_combination.h"

#define IN_FORMAT "%lu"

int
gsl_combination_fread (FILE * stream, gsl_combination * c)
{
  size_t k = c->k ;

  size_t * data = c->data ;

  size_t items = fread (data, sizeof (size_t), k, stream);
  
  if (items != k)
    {
      GSL_ERROR ("fread failed", GSL_EFAILED);
    }
      
  return GSL_SUCCESS;
}

int
gsl_combination_fwrite (FILE * stream, const gsl_combination * c)
{
  size_t k = c->k ;

  size_t * data = c->data ;
  
  size_t items = fwrite (data, sizeof (size_t), k, stream);
  
  if (items != k)
    {
      GSL_ERROR ("fwrite failed", GSL_EFAILED);
    }

  return GSL_SUCCESS;
}

int
gsl_combination_fprintf (FILE * stream, const gsl_combination * c, const char *format)
{
  size_t k = c->k ;
  
  size_t * data = c->data ;
  
  size_t i;

  for (i = 0; i < k; i++)
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
gsl_combination_fscanf (FILE * stream, gsl_combination * c)
{
  size_t k = c->k ;
  
  size_t * data = c->data ;

  size_t i;

  for (i = 0; i < k; i++)
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

