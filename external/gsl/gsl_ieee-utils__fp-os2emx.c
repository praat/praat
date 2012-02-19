/* ieee-utils/fp-os2.c
 * 
 * Copyright (C) 2001 Henry Sobotka <sobotka@axess.com>
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

#include <float.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  unsigned mode = 0;

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      _control87(PC_24, MCW_PC);    
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      _control87(PC_53, MCW_PC);
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      _control87(PC_64, MCW_PC);
      break ;
    }

  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      _control87(RC_NEAR, MCW_RC);
      break ;
    case GSL_IEEE_ROUND_DOWN:
      _control87(RC_DOWN, MCW_RC);
      break ;
    case GSL_IEEE_ROUND_UP:
      _control87(RC_UP, MCW_RC);
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      _control87(RC_CHOP, MCW_RC);
      break ;
    default:
      _control87(RC_NEAR, MCW_RC);
    }

  /* Turn on all the exceptions apart from 'inexact' */

  mode = EM_INVALID | EM_DENORMAL | EM_ZERODIVIDE | EM_OVERFLOW | EM_UNDERFLOW;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode &= ~ EM_INVALID;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    mode &= ~ EM_DENORMAL;

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode &= ~ EM_ZERODIVIDE;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode &= ~ EM_OVERFLOW;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode &= ~ EM_UNDERFLOW;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode |= EM_INEXACT;
    }
  else
    {
      mode &= ~ EM_INEXACT;
    }

  _control87(mode, MCW_EM);

  return GSL_SUCCESS ;
}
