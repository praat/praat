/* ieee-utils/fp-aix.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Tim Mooney
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
#include <fptrap.h>
#include <float.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  fptrap_t   mode = 0 ;
  fprnd_t    rnd  = 0 ;

  switch (precision)
    {

    /* I'm not positive about AIX only supporting default precision rounding,
     * but this is the best assumption until it's proven otherwise. */

    case GSL_IEEE_SINGLE_PRECISION:
      GSL_ERROR ("AIX only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      GSL_ERROR ("AIX only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      GSL_ERROR ("AIX only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    }

  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      rnd = FP_RND_RN ;
      fp_swap_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      rnd = FP_RND_RM ;
      fp_swap_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_UP:
      rnd = FP_RND_RP ;
      fp_swap_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      rnd = FP_RND_RZ ;
      fp_swap_rnd (rnd) ;
      break ;
    default:
      rnd = FP_RND_RN ;
      fp_swap_rnd (rnd) ;
    }

  /* Turn on all the exceptions apart from 'inexact' */

  mode = TRP_INVALID | TRP_DIV_BY_ZERO | TRP_OVERFLOW | TRP_UNDERFLOW ;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode &= ~ TRP_INVALID ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    {
      /* do nothing */
    }
  else 
    {
      GSL_ERROR ("AIX does not support the denormalized operand exception. "
                 "Use 'mask-denormalized' to work around this.",
                 GSL_EUNSUP) ;
    }

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode &= ~ TRP_DIV_BY_ZERO ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode &= ~ TRP_OVERFLOW ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode &=  ~ TRP_UNDERFLOW ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode |= TRP_INEXACT ;
    }
  else
    {
      mode &= ~ TRP_INEXACT ;
    }

  /* AIX appears to require two steps -- first enable floating point traps
   * in general... */
  fp_trap(FP_TRAP_SYNC);

  /* next, enable the traps we're interested in */
  fp_enable(mode);

  return GSL_SUCCESS ;

}
