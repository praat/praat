/* monte/gsl_monte_plain.h
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

/* Plain Monte-Carlo. */

/* Author: MJB */

#ifndef __GSL_MONTE_PLAIN_H__
#define __GSL_MONTE_PLAIN_H__

#include <stdio.h>
#include "gsl_monte.h"
#include "gsl_rng.h"

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

typedef struct {
  size_t dim;
  double *x;
} gsl_monte_plain_state;

int
gsl_monte_plain_integrate (const gsl_monte_function * f,
                           const double xl[], const double xu[],
                           const size_t dim,
                           const size_t calls, 
                           gsl_rng * r,
                           gsl_monte_plain_state * state,
                           double *result, double *abserr);

gsl_monte_plain_state* gsl_monte_plain_alloc(size_t dim);

int gsl_monte_plain_init(gsl_monte_plain_state* state);

void gsl_monte_plain_free (gsl_monte_plain_state* state);

__END_DECLS

#endif /* __GSL_MONTE_PLAIN_H__ */
