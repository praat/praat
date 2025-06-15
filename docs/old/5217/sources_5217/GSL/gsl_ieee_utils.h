/* ieee-utils/gsl_ieee_utils.h
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

#ifndef __GSL_IEEE_UTILS_H__
#define __GSL_IEEE_UTILS_H__
#include <stdio.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

enum {
  GSL_IEEE_TYPE_NAN = 1,
  GSL_IEEE_TYPE_INF = 2,
  GSL_IEEE_TYPE_NORMAL = 3,
  GSL_IEEE_TYPE_DENORMAL = 4,
  GSL_IEEE_TYPE_ZERO = 5
} ;

typedef struct  {
  int sign ;
  char mantissa[24] ; /* Actual bits are 0..22, element 23 is \0 */
  int exponent ;
  int type ;
} gsl_ieee_float_rep ;

typedef struct  {
  int sign ;
  char mantissa[53] ; /* Actual bits are 0..51, element 52 is \0 */
  int exponent ;
  int type ;
} gsl_ieee_double_rep ;


void gsl_ieee_printf_float (const float * x) ;
void gsl_ieee_printf_double (const double * x) ;

void gsl_ieee_fprintf_float (FILE * stream, const float * x) ;
void gsl_ieee_fprintf_double (FILE * stream, const double * x) ;

void gsl_ieee_float_to_rep (const float * x, gsl_ieee_float_rep * r) ;
void gsl_ieee_double_to_rep (const double * x, gsl_ieee_double_rep * r) ;

enum {
  GSL_IEEE_SINGLE_PRECISION = 1,
  GSL_IEEE_DOUBLE_PRECISION = 2,
  GSL_IEEE_EXTENDED_PRECISION = 3
} ;

enum {
  GSL_IEEE_ROUND_TO_NEAREST = 1,
  GSL_IEEE_ROUND_DOWN = 2,
  GSL_IEEE_ROUND_UP = 3,
  GSL_IEEE_ROUND_TO_ZERO = 4
} ;

enum {
  GSL_IEEE_MASK_INVALID = 1,
  GSL_IEEE_MASK_DENORMALIZED = 2,
  GSL_IEEE_MASK_DIVISION_BY_ZERO = 4,
  GSL_IEEE_MASK_OVERFLOW = 8,
  GSL_IEEE_MASK_UNDERFLOW = 16,
  GSL_IEEE_MASK_ALL = 31,
  GSL_IEEE_TRAP_INEXACT = 32
} ;

void gsl_ieee_env_setup (void) ;
int gsl_ieee_read_mode_string (const char * description, int * precision,
                               int * rounding, int * exception_mask) ;
int gsl_ieee_set_mode (int precision, int rounding, int exception_mask) ;

__END_DECLS

#endif /* __GSL_IEEE_UTILS_H__ */

