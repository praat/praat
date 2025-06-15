/* ieee-utils/read.c
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
#include <stdlib.h>
#include <string.h>
#include "gsl_errno.h"
#include "gsl_ieee_utils.h"

static int 
lookup_string (const char * p, int * precision, int * rounding, 
               int * exception_mask) ;

int
gsl_ieee_read_mode_string (const char * description, 
                           int * precision, 
                           int * rounding, 
                           int * exception_mask)
{
  char * start ;
  char * end;
  char * p;

  int precision_count = 0 ;
  int rounding_count = 0 ;
  int exception_count = 0 ;

  start = (char *) malloc(strlen(description) + 1) ;

  if (start == 0) 
    {
      GSL_ERROR ("no memory to parse mode string", GSL_ENOMEM) ;
    }

  strcpy (start, description) ;

  p = start ;

  *precision = 0 ;
  *rounding = 0 ;
  *exception_mask = 0 ;

  do {
    int status ;
    int new_precision, new_rounding, new_exception ;

    end = strchr (p,',') ;

    if (end) 
      {
        *end = '\0' ;
        do 
          {
            end++ ;  /* skip over trailing whitespace */
          } 
        while (*end == ' ' || *end == ',') ;
      }
        
    new_precision = 0 ; 
    new_rounding = 0 ; 
    new_exception = 0 ;

    status = lookup_string (p, &new_precision, &new_rounding, &new_exception) ;

    if (status)
      GSL_ERROR ("unrecognized GSL_IEEE_MODE string.\nValid settings are:\n\n" 
                 "  single-precision double-precision extended-precision\n"
                 "  round-to-nearest round-down round-up round-to-zero\n"
                 "  mask-invalid mask-denormalized mask-division-by-zero\n"
                 "  mask-overflow mask-underflow mask-all\n"
                 "  trap-common trap-inexact\n"
                 "\n"
                 "separated by commas. "
                 "(e.g. GSL_IEEE_MODE=\"round-down,mask-underflow\")",
                 GSL_EINVAL) ;

    if (new_precision) 
      {
        *precision = new_precision ;
        precision_count ++ ;
        if (precision_count > 1)
          GSL_ERROR ("attempted to set IEEE precision twice", GSL_EINVAL) ;
      }

    if (new_rounding) 
      {
        *rounding = new_rounding ;
        rounding_count ++ ;
        if (rounding_count > 1)
          GSL_ERROR ("attempted to set IEEE rounding mode twice", GSL_EINVAL) ;
      }

    if (new_exception) 
      {
        *exception_mask |= new_exception ;
        exception_count ++ ;
      }

    p = end ; 

  } while (end && *p != '\0') ;

  free(start) ;

  return GSL_SUCCESS ;
}

static int 
lookup_string (const char * p, int * precision, int * rounding, 
               int * exception_mask)
{
  if (strcmp(p,"single-precision") == 0) 
    {
      *precision = GSL_IEEE_SINGLE_PRECISION ;
    }
  else if (strcmp(p,"double-precision") == 0) 
    {
      *precision = GSL_IEEE_DOUBLE_PRECISION ;
    }
  else if (strcmp(p,"extended-precision") == 0) 
    {
      *precision = GSL_IEEE_EXTENDED_PRECISION ;
    }
  else if (strcmp(p,"round-to-nearest") == 0) 
    {
      *rounding = GSL_IEEE_ROUND_TO_NEAREST ;
    }
  else if (strcmp(p,"round-down") == 0) 
    {
      *rounding = GSL_IEEE_ROUND_DOWN ;
    }
  else if (strcmp(p,"round-up") == 0) 
    {
      *rounding = GSL_IEEE_ROUND_UP ;
    }
  else if (strcmp(p,"round-to-zero") == 0) 
    {
      *rounding = GSL_IEEE_ROUND_TO_ZERO ;
    }
  else if (strcmp(p,"mask-all") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_ALL ;
    }
  else if (strcmp(p,"mask-invalid") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_INVALID ;
    }
  else if (strcmp(p,"mask-denormalized") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_DENORMALIZED ;
    }
  else if (strcmp(p,"mask-division-by-zero") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_DIVISION_BY_ZERO ;
    }
  else if (strcmp(p,"mask-overflow") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_OVERFLOW ;
    }
  else if (strcmp(p,"mask-underflow") == 0) 
    {
      *exception_mask = GSL_IEEE_MASK_UNDERFLOW ;
    }
  else if (strcmp(p,"trap-inexact") == 0) 
    {
      *exception_mask = GSL_IEEE_TRAP_INEXACT ;
    }
  else if (strcmp(p,"trap-common") == 0) 
    {
      return 0 ;
    }
  else
    {
      return 1 ;
    }

  return 0 ;
}
