/* specfunc/gsl_sf_elementary.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Author:  G. Jungman */

/* Miscellaneous elementary functions and operations.
 */
#ifndef __GSL_SF_ELEMENTARY_H__
#define __GSL_SF_ELEMENTARY_H__

#include "gsl_sf_result.h"




/* Multiplication.
 *
 * exceptions: GSL_EOVRFLW, GSL_EUNDRFLW
 */
int gsl_sf_multiply_e(const double x, const double y, gsl_sf_result * result);
double gsl_sf_multiply(const double x, const double y);


/* Multiplication of quantities with associated errors.
 */
int gsl_sf_multiply_err_e(const double x, const double dx, const double y, const double dy, gsl_sf_result * result);



#endif /* __GSL_SF_ELEMENTARY_H__ */
