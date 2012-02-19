/* sys/prec.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

/* Author:  G. Jungman */

#include "gsl__config.h"
#include "gsl_machine.h"
#include "gsl_precision.h"

const double gsl_prec_eps[_GSL_PREC_T_NUM] = {
  GSL_DBL_EPSILON,
  GSL_FLT_EPSILON,
  GSL_SFLT_EPSILON
};

const double gsl_prec_sqrt_eps[_GSL_PREC_T_NUM] = {
  GSL_SQRT_DBL_EPSILON,
  GSL_SQRT_FLT_EPSILON,
  GSL_SQRT_SFLT_EPSILON
};

const double gsl_prec_root3_eps[_GSL_PREC_T_NUM] = {
  GSL_ROOT3_DBL_EPSILON,
  GSL_ROOT3_FLT_EPSILON,
  GSL_ROOT3_SFLT_EPSILON
};

const double gsl_prec_root4_eps[_GSL_PREC_T_NUM] = {
  GSL_ROOT4_DBL_EPSILON,
  GSL_ROOT4_FLT_EPSILON,
  GSL_ROOT4_SFLT_EPSILON
};

const double gsl_prec_root5_eps[_GSL_PREC_T_NUM] = {
  GSL_ROOT5_DBL_EPSILON,
  GSL_ROOT5_FLT_EPSILON,
  GSL_ROOT5_SFLT_EPSILON
};

const double gsl_prec_root6_eps[_GSL_PREC_T_NUM] = {
  GSL_ROOT6_DBL_EPSILON,
  GSL_ROOT6_FLT_EPSILON,
  GSL_ROOT6_SFLT_EPSILON
};

typedef unsigned int gsl_mode_t;

#ifndef HIDE_INLINE_STATIC
/* We need this somewhere, in case the inline is ignored.
 */

unsigned int GSL_MODE_PREC(gsl_mode_t mt);

unsigned int
GSL_MODE_PREC(gsl_mode_t mt)
{ 
  return  (mt & (unsigned int)7); 
}
#endif
