/* ieee-utils/env.c
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
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

void
gsl_ieee_env_setup (void)
{
  const char * p = getenv("GSL_IEEE_MODE") ;

  int precision = 0, rounding = 0, exception_mask = 0 ;

  int comma = 0 ;

  if (p == 0)  /* GSL_IEEE_MODE environment variable is not set */
    return ;

  if (*p == '\0') /* GSL_IEEE_MODE environment variable is empty */
    return ;

  gsl_ieee_read_mode_string (p, &precision, &rounding, &exception_mask) ;

  gsl_ieee_set_mode (precision, rounding, exception_mask) ;
  
  fprintf(stderr, "GSL_IEEE_MODE=\"") ;

  /* Print string with a preceeding comma if the list has already begun */

#define PRINTC(x) do {if(comma) fprintf(stderr,","); fprintf(stderr,x); comma++ ;} while(0)
  
  switch (precision) 
    {
    case GSL_IEEE_SINGLE_PRECISION:
      PRINTC("single-precision") ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      PRINTC("double-precision") ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      PRINTC("extended-precision") ;
      break ;
    }

  switch (rounding) 
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      PRINTC("round-to-nearest") ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      PRINTC("round-down") ;
      break ;
    case GSL_IEEE_ROUND_UP:
      PRINTC("round-up") ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      PRINTC("round-to-zero") ;
      break ;
    }

  if ((exception_mask & GSL_IEEE_MASK_ALL) == GSL_IEEE_MASK_ALL)
    {
      PRINTC("mask-all") ;
    }
  else if ((exception_mask & GSL_IEEE_MASK_ALL) == 0)
    {
      PRINTC("trap-common") ;
    }
  else 
    {
      if (exception_mask & GSL_IEEE_MASK_INVALID)
        PRINTC("mask-invalid") ;
      
      if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
        PRINTC("mask-denormalized") ;
      
      if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
        PRINTC("mask-division-by-zero") ;
      
      if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
        PRINTC("mask-overflow") ;
      
      if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
        PRINTC("mask-underflow") ;
    }

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    PRINTC("trap-inexact") ;
  
  fprintf(stderr,"\"\n") ;
}





