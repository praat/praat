/* ieee-utils/fp-hpux11.c
 * 
 * Copyright (C) 2001, 2007 Brian Gough
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

#include <math.h>
#include <stdio.h>
#include <fenv.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  int mode;

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      GSL_ERROR ("HPUX PA-RISC only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      GSL_ERROR ("HPUX PA-RISC only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      GSL_ERROR ("HPUX PA-RISC only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    }


  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      fesetround (FE_TONEAREST) ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      fesetround (FE_DOWNWARD) ;
      break ;
    case GSL_IEEE_ROUND_UP:
      fesetround (FE_UPWARD) ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      fesetround (FE_TOWARDZERO) ;
      break ;
    default:
      fesetround (FE_TONEAREST) ;
    }

  /* Turn on all the exceptions apart from 'inexact' */

  mode = FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW ;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode &= ~ FE_INVALID ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    {
      /* do nothing */
    }
  else
    {
      GSL_ERROR ("HP-UX does not support the denormalized operand exception. "
                 "Use 'mask-denormalized' to work around this.",
                 GSL_EUNSUP) ;
    }

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode &= ~ FE_DIVBYZERO ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode &= ~ FE_OVERFLOW ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode &=  ~ FE_UNDERFLOW ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode |= FE_INEXACT ;
    }
  else
    {
      mode &= ~ FE_INEXACT ;
    }

  fesettrapenable (mode) ;

  return GSL_SUCCESS ;
}
