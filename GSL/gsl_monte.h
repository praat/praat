/* monte/gsl_monte.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Michael Booth
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

/* Some things common to all the Monte-Carlo implementations */
/* Author: MJB */

#ifndef __GSL_MONTE_H__
#define __GSL_MONTE_H__

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

/* Hide the function type in a typedef so that we can use it in all our
   integration functions, and make it easy to change things.
*/

struct gsl_monte_function_struct {
  double (*f)(double * x_array, size_t dim, void * params);
  size_t dim;
  void * params;
};

typedef struct gsl_monte_function_struct gsl_monte_function;

#define GSL_MONTE_FN_EVAL(F,x) (*((F)->f))(x,(F)->dim,(F)->params)


__END_DECLS

#endif /* __GSL_MONTE_H__ */
