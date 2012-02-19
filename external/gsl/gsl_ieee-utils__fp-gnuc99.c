/* ieee-utils/fp-gnuc99.c
 * 
 * Copyright (C) 2003, 2004, 2007 Brian Gough
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

#define _GNU_SOURCE 1

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
      GSL_ERROR ("single precision rounding is not supported by <fenv.h>",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      GSL_ERROR ("double precision rounding is not supported by <fenv.h>",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      GSL_ERROR ("extended precision rounding is not supported by <fenv.h>",
                 GSL_EUNSUP) ;
      break ;
    }


  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
#ifdef FE_TONEAREST
      fesetround (FE_TONEAREST) ;
#else
      GSL_ERROR ("round-to-nearest is not supported by <fenv.h>", GSL_EUNSUP) ;
#endif
      break ;
    case GSL_IEEE_ROUND_DOWN:
#ifdef FE_DOWNWARD
      fesetround (FE_DOWNWARD) ;
#else
      GSL_ERROR ("round-down is not supported by <fenv.h>", GSL_EUNSUP) ;
#endif
      break ;
    case GSL_IEEE_ROUND_UP:
#ifdef FE_UPWARD
      fesetround (FE_UPWARD) ;
#else
      GSL_ERROR ("round-up is not supported by <fenv.h>", GSL_EUNSUP) ;
#endif
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
#ifdef FE_TOWARDZERO
      fesetround (FE_TOWARDZERO) ;
#else
      GSL_ERROR ("round-toward-zero is not supported by <fenv.h>", GSL_EUNSUP) ;
#endif
      break ;
    default:
#ifdef FE_TONEAREST
      fesetround (FE_TONEAREST) ;
#else
      GSL_ERROR ("default round-to-nearest mode is not supported by <fenv.h>", GSL_EUNSUP) ;
#endif
    }

  /* Turn on all the exceptions apart from 'inexact' */

  mode = 0;

#ifdef FE_INVALID 
  mode |= FE_INVALID;
#endif

#ifdef FE_DIVBYZERO
  mode |= FE_DIVBYZERO;
#endif
  
#ifdef FE_OVERFLOW
  mode |= FE_OVERFLOW ;
#endif

#ifdef FE_UNDERFLOW
  mode |= FE_UNDERFLOW ;
#endif

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    {
#ifdef FE_INVALID
    mode &= ~ FE_INVALID ;
#else
    GSL_ERROR ("invalid operation exception not supported by <fenv.h>", 
               GSL_EUNSUP);
#endif
    }

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    {
      /* do nothing */
    }
  else
    {
      GSL_ERROR ("denormalized operand exception not supported by <fenv.h>. "
                 "Use 'mask-denormalized' to work around this.", GSL_EUNSUP) ;
    }

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    {
#ifdef FE_DIVBYZERO
      mode &= ~ FE_DIVBYZERO ;
#else
      GSL_ERROR ("division by zero exception not supported by <fenv.h>", 
                 GSL_EUNSUP);
#endif
    }

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    {
#ifdef FE_OVERFLOW
      mode &= ~ FE_OVERFLOW ;
#else
      GSL_ERROR ("overflow exception not supported by <fenv.h>", GSL_EUNSUP);
#endif
    }

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    {
#ifdef FE_UNDERFLOW
      mode &=  ~ FE_UNDERFLOW ;
#else
      GSL_ERROR ("underflow exception not supported by <fenv.h>", GSL_EUNSUP);
#endif
    }

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
#ifdef FE_INEXACT
      mode |= FE_INEXACT ;
#else
      GSL_ERROR ("inexact exception not supported by <fenv.h>", GSL_EUNSUP);
#endif
    }
  else
    {
#ifdef FE_INEXACT
      mode &= ~ FE_INEXACT ;
#else
      /* do nothing */
#endif
    }

#if HAVE_DECL_FEENABLEEXCEPT
  feenableexcept (mode) ;
#elif HAVE_DECL_FESETTRAPENABLE
  fesettrapenable (mode);
#else
  GSL_ERROR ("unknown exception trap method", GSL_EUNSUP)
#endif

  return GSL_SUCCESS ;
}
