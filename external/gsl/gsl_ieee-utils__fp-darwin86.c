/* ieee-utils/fp-darwin86.c
 * 
 * Copyright (C) 2006 Erik Schnetter
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
#include "gsl_ieee_utils.h"
#include "gsl_errno.h"

/* Here is the dirty part. Set up your 387 through the control word
 * (cw) register.
 *
 *     15-13    12  11-10  9-8     7-6     5    4    3    2    1    0
 * | reserved | IC | RC  | PC | reserved | PM | UM | OM | ZM | DM | IM
 *
 * IM: Invalid operation mask
 * DM: Denormalized operand mask
 * ZM: Zero-divide mask
 * OM: Overflow mask
 * UM: Underflow mask
 * PM: Precision (inexact result) mask
 *
 * Mask bit is 1 means no interrupt.
 *
 * PC: Precision control
 * 11 - round to extended precision
 * 10 - round to double precision
 * 00 - round to single precision
 *
 * RC: Rounding control
 * 00 - rounding to nearest
 * 01 - rounding down (toward - infinity)
 * 10 - rounding up (toward + infinity)
 * 11 - rounding toward zero
 *
 * IC: Infinity control
 * That is for 8087 and 80287 only.
 *
 * The hardware default is 0x037f which we use.
 */

/* masking of interrupts */
#define _FPU_MASK_IM  0x01
#define _FPU_MASK_DM  0x02
#define _FPU_MASK_ZM  0x04
#define _FPU_MASK_OM  0x08
#define _FPU_MASK_UM  0x10
#define _FPU_MASK_PM  0x20

/* precision control */
#define _FPU_EXTENDED 0x300	/* libm requires double extended precision.  */
#define _FPU_DOUBLE   0x200
#define _FPU_SINGLE   0x0

/* rounding control */
#define _FPU_RC_NEAREST 0x0    /* RECOMMENDED */
#define _FPU_RC_DOWN    0x400
#define _FPU_RC_UP      0x800
#define _FPU_RC_ZERO    0xC00

#define _FPU_RESERVED 0xF0C0  /* Reserved bits in cw */


/* The fdlibm code requires strict IEEE double precision arithmetic,
   and no interrupts for exceptions, rounding to nearest.  */

#define _FPU_DEFAULT  0x037f

/* IEEE:  same as above.  */
#define _FPU_IEEE     0x037f

/* Type of the control word.  */
typedef unsigned int fpu_control_t __attribute__ ((__mode__ (__HI__)));

/* Macros for accessing the hardware control word.

   Note that the use of these macros is no sufficient anymore with
   recent hardware.  Some floating point operations are executed in
   the SSE/SSE2 engines which have their own control and status register.  */
#define _FPU_GETCW(cw) __asm__ __volatile__ ("fnstcw %0" : "=m" (*&cw))
#define _FPU_SETCW(cw) __asm__ __volatile__ ("fldcw %0" : : "m" (*&cw))

/* Default control word set at startup.  */
extern fpu_control_t __fpu_control;



#define _FPU_GETMXCSR(cw_sse) asm volatile ("stmxcsr %0" : "=m" (cw_sse))
#define _FPU_SETMXCSR(cw_sse) asm volatile ("ldmxcsr %0" : : "m" (cw_sse))



int
gsl_ieee_set_mode (int precision, int rounding, int exception_mask)
{
  fpu_control_t mode, mode_sse;

  _FPU_GETCW (mode) ;
  mode &= _FPU_RESERVED ;

  switch (precision)
    {
    case GSL_IEEE_SINGLE_PRECISION:
      mode |= _FPU_SINGLE ;
      break ;
    case GSL_IEEE_DOUBLE_PRECISION:
      mode |= _FPU_DOUBLE ;
      break ;
    case GSL_IEEE_EXTENDED_PRECISION:
      mode |= _FPU_EXTENDED ;
      break ;
    default:
      mode |= _FPU_EXTENDED ;
    }

  switch (rounding)
    {
    case GSL_IEEE_ROUND_TO_NEAREST:
      mode |= _FPU_RC_NEAREST ;
      break ;
    case GSL_IEEE_ROUND_DOWN:
      mode |= _FPU_RC_DOWN ;
      break ;
    case GSL_IEEE_ROUND_UP:
      mode |= _FPU_RC_UP ;
      break ;
    case GSL_IEEE_ROUND_TO_ZERO:
      mode |= _FPU_RC_ZERO ;
      break ;
    default:
      mode |= _FPU_RC_NEAREST ;
    }

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode |= _FPU_MASK_IM ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    mode |= _FPU_MASK_DM ;

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode |= _FPU_MASK_ZM ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode |= _FPU_MASK_OM ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode |= _FPU_MASK_UM ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode &= ~ _FPU_MASK_PM ;
    }
  else
    {
      mode |= _FPU_MASK_PM ;
    }

  _FPU_SETCW (mode) ;

  _FPU_GETMXCSR (mode_sse) ;
  mode_sse &= 0xFFFF0000 ;

  if (exception_mask & GSL_IEEE_MASK_INVALID)
    mode_sse |= _FPU_MASK_IM << 7 ;

  if (exception_mask & GSL_IEEE_MASK_DENORMALIZED)
    mode_sse |= _FPU_MASK_DM << 7 ;

  if (exception_mask & GSL_IEEE_MASK_DIVISION_BY_ZERO)
    mode_sse |= _FPU_MASK_ZM << 7 ;

  if (exception_mask & GSL_IEEE_MASK_OVERFLOW)
    mode_sse |= _FPU_MASK_OM << 7 ;

  if (exception_mask & GSL_IEEE_MASK_UNDERFLOW)
    mode_sse |= _FPU_MASK_UM << 7 ;

  if (exception_mask & GSL_IEEE_TRAP_INEXACT)
    {
      mode_sse &= ~ _FPU_MASK_PM << 7 ;
    }
  else
    {
      mode_sse |= _FPU_MASK_PM << 7 ;
    }

  _FPU_SETMXCSR (mode_sse) ;

  return GSL_SUCCESS ;
}
