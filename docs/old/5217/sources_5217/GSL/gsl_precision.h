/* gsl_precision.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  B. Gough and G. Jungman */

#ifndef __GSL_PRECISION_H__
#define __GSL_PRECISION_H__
#include "gsl_types.h"

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


/* A type for the precision indicator.
 * This is mainly for pedagogy.
 */
typedef  unsigned int  gsl_prec_t;


/* The number of precision types.
 * Remember that precision-mode
 * can index an array.
 */
#define _GSL_PREC_T_NUM 3


/* Arrays containing derived
 * precision constants for the
 * different precision levels.
 */
GSL_VAR const double gsl_prec_eps[];
GSL_VAR const double gsl_prec_sqrt_eps[];
GSL_VAR const double gsl_prec_root3_eps[];
GSL_VAR const double gsl_prec_root4_eps[];
GSL_VAR const double gsl_prec_root5_eps[];
GSL_VAR const double gsl_prec_root6_eps[];


__END_DECLS

#endif /* __GSL_PRECISION_H__ */
