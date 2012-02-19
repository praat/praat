/* ieee-utils/fp-darwin.c
 * 
 * Copyright (C) 2001 Rodney Sparapani <rsparapa@mcw.edu>
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

#include <architecture/ppc/fp_regs.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  ppc_fp_scr_t fp_scr = get_fp_scr() ;

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      GSL_ERROR ("powerpc only supports default precision rounding", GSL_EUNSUP);
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      GSL_ERROR ("powerpc only supports default precision rounding", GSL_EUNSUP);
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      GSL_ERROR ("powerpc only supports default precision rounding", GSL_EUNSUP);
      break ;
    }

  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      fp_scr.rn = RN_NEAREST ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      fp_scr.rn = RN_TOWARD_MINUS ;
      break ;
    case GSL_IEEE_ROUND_UP:
      fp_scr.rn = RN_TOWARD_PLUS ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      fp_scr.rn = RN_TOWARD_ZERO ;
      break ;
    default:
      fp_scr.rn = RN_NEAREST ;
    }

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    fp_scr.ve = 0 ;                             //ve bit:  invalid op exception enable

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    {
      /* do nothing */
    }
  else
    {
      GSL_ERROR ("powerpc does not support the denormalized operand exception. "
                 "Use 'mask-denormalized' to work around this.", GSL_EUNSUP) ;
    }

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    fp_scr.ze = 0 ;                             //ze bit:  zero divide exception enable

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    fp_scr.oe = 0 ;                             //oe bit:  overflow exception enable

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    fp_scr.ue  = 0 ;                            //ue bit:  underflow exception enable

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      fp_scr.xe = 1 ;                           //xe bit:  inexact exception enable
    }
  else
    {
      fp_scr.xe = 01 ;                  
    }

  set_fp_scr(fp_scr);

  return GSL_SUCCESS ;

}
