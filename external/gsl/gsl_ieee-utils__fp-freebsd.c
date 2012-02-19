/* ieee-utils/fp-freebsd.c
 * 
 * Copyright (C) 2000 Vladimir Kushnir
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

#include <ieeefp.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  fp_prec_t prec = 0 ;
  fp_except_t mode = 0 ;
  fp_rnd_t    rnd  = 0 ;

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      prec = FP_PS;
      fpsetprec(prec);      
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      prec = FP_PD;
      fpsetprec(prec);
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      prec = FP_PE;
      fpsetprec(prec);
      break ;
    }

  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      rnd = FP_RN ;
      fpsetround (rnd) ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      rnd = FP_RM ;
      fpsetround (rnd) ;
      break ;
    case GSL_IEEE_ROUND_UP:
      rnd = FP_RP ;
      fpsetround (rnd) ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      rnd = FP_RZ ;
      fpsetround (rnd) ;
      break ;
    default:
      rnd = FP_RN ;
      fpsetround (rnd) ;
    }

  /* Turn on all the exceptions apart from 'inexact' */

  mode = FP_X_INV | FP_X_DNML | FP_X_DZ | FP_X_OFL | FP_X_UFL ;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode &= ~ FP_X_INV ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    mode &= ~ FP_X_DNML ;

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode &= ~ FP_X_DZ ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode &= ~ FP_X_OFL ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode &=  ~ FP_X_UFL ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode |= FP_X_IMP ;
    }
  else
    {
      mode &= ~ FP_X_IMP ;
    }

  fpsetmask (mode) ;

  return GSL_SUCCESS ;

}
