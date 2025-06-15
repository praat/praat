/* ieee-utils/fp-tru64.c
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


/*
 * Under Compaq's Unix with the silly name, read the man pages for read_rnd,
 * write_rnd, and ieee(3) for more information on the functions used here.
 *
 * Note that enabling control of dynamic rounding mode (via write_rnd) requires
 * that you pass a special flag to your C compiler.  For Compaq's C compiler
 * the flag is `-fprm d', for gcc it's `-mfp-rounding-mode=d'.
 *
 * Enabling the trap control (via ieee_set_fp_control) also requires a
 * flag be passed to the C compiler.  The flag for Compaq's C compiler
 * is `-ieee' and for gcc it's `-mieee'.

 * We have not implemented the `inexact' case, since it is rarely used
 * and requires the library being built with an additional compiler
 * flag that can degrade performance for everything else. If you need
 * to add support for `inexact' the relevant flag for Compaq's
 * compiler is `-ieee_with_inexact', and the flag for gcc is
 * `-mieee-with-inexact'.
 *
 * Problem have been reported with the "fixed" float.h installed with
 * gcc-2.95 lacking some of the definitions in the system float.h (the
 * symptoms are errors like: `FP_RND_RN' undeclared). To work around
 * this we can include the system float.h before the gcc version, e.g. 
 *
 *  #include "/usr/include/float.h"
 *  #include <float.h>
 */

#include <float.h>

#ifndef FP_RND_RN
#  undef _FLOAT_H_
#  include /usr/include/float.h
#  undef _FLOAT_H_
#  include <float.h>
#endif

#include <machine/fpu.h>
#include <stdio.h>
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  unsigned long int mode = 0 ;
  unsigned int    rnd  = 0 ;

/* I'm actually not completely sure that the alpha only supports default
 * precisions rounding, but I couldn't find any information regarding this, so
 * it seems safe to assume this for now until it's proven otherwise.
 */

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      GSL_ERROR ("Tru64 Unix on the alpha only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      GSL_ERROR ("Tru64 Unix on the alpha only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      GSL_ERROR ("Tru64 Unix on the alpha only supports default precision rounding",
                 GSL_EUNSUP) ;
      break ;
    }


  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      rnd = FP_RND_RN ;
      write_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      rnd = FP_RND_RM ;
      write_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_UP:
      rnd = FP_RND_RP ;
      write_rnd (rnd) ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      rnd = FP_RND_RZ ;
      write_rnd (rnd) ;
      break ;
    default:
      rnd = FP_RND_RN ;
      write_rnd (rnd) ;
    }

  /* Turn on all the exceptions apart from 'inexact' */

  /* from the ieee(3) man page:
   * IEEE_TRAP_ENABLE_INV       ->      Invalid operation
   * IEEE_TRAP_ENABLE_DZE       ->      Divide by 0
   * IEEE_TRAP_ENABLE_OVF       ->      Overflow
   * IEEE_TRAP_ENABLE_UNF       ->      Underflow
   * IEEE_TRAP_ENABLE_INE       ->      Inexact (requires special option to C compiler)
   * IEEE_TRAP_ENABLE_DNO       ->      denormal operand
   * Note: IEEE_TRAP_ENABLE_DNO is not supported on OSF 3.x or Digital Unix
   * 4.0 - 4.0d(?).
   * IEEE_TRAP_ENABLE_MASK      ->      mask of all the trap enables
   * IEEE_MAP_DMZ                       ->      map denormal inputs to zero
   * IEEE_MAP_UMZ                       ->      map underflow results to zero
   */

  mode = IEEE_TRAP_ENABLE_INV | IEEE_TRAP_ENABLE_DZE | IEEE_TRAP_ENABLE_OVF
                | IEEE_TRAP_ENABLE_UNF ;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode &= ~ IEEE_TRAP_ENABLE_INV ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    {
#ifdef IEEE_TRAP_ENABLE_DNO
     mode &= ~ IEEE_TRAP_ENABLE_DNO ;
#else
     GSL_ERROR ("Sorry, this version of Digital Unix does not support denormalized operands", GSL_EUNSUP) ;
#endif
    }

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode &= ~ IEEE_TRAP_ENABLE_DZE ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode &= ~ IEEE_TRAP_ENABLE_OVF ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode &=  ~ IEEE_TRAP_ENABLE_UNF ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      /* To implement this would require a special flag to the C
       compiler which can cause degraded performance */

      GSL_ERROR ("Sorry, GSL does not implement trap-inexact for Tru64 Unix on the alpha - see fp-tru64.c for details", GSL_EUNSUP) ;

      /* In case you need to add it, the appropriate line would be 
       *  
       *  mode |= IEEE_TRAP_ENABLE_INE ; 
       *
       */

    }
  else
    {
      mode &= ~ IEEE_TRAP_ENABLE_INE ;
    }

  ieee_set_fp_control (mode) ;

  return GSL_SUCCESS ;
}
