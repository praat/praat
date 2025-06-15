/* roots/roots.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
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

/* roots.h -- declarations for internal root finding and RF support stuff. */

#ifndef __ROOTS_H__
#define __ROOTS_H__

/* Call the pointed-to function with argument x, put its result in y, and 
   return an error if the function value is Inf/Nan. */

#define SAFE_FUNC_CALL(f, x, yp) \
do { \
  *yp = GSL_FN_EVAL(f,x); \
  if (!gsl_finite(*yp)) \
    GSL_ERROR("function value is not finite", GSL_EBADFUNC); \
} while (0)

#endif /* __ROOTS_H__ */


