/* bspline/gsl_bspline.h
 * 
 * Copyright (C) 2006 Patrick Alken
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

#ifndef __GSL_BSPLINE_H__
#define __GSL_BSPLINE_H__

#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_vector.h"

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

typedef struct
{
  size_t k;      /* spline order */
  size_t km1;    /* k - 1 (polynomial order) */
  size_t l;      /* number of polynomial pieces on interval */
  size_t nbreak; /* number of breakpoints (l + 1) */
  size_t n;      /* number of bspline basis functions (l + k - 1) */

  gsl_vector *knots;  /* knots vector */
  gsl_vector *deltal; /* left delta */
  gsl_vector *deltar; /* right delta */
  gsl_vector *B;      /* temporary spline results */
} gsl_bspline_workspace;

gsl_bspline_workspace *
gsl_bspline_alloc(const size_t k, const size_t nbreak);

void gsl_bspline_free(gsl_bspline_workspace *w);

size_t gsl_bspline_ncoeffs (gsl_bspline_workspace * w);
size_t gsl_bspline_order (gsl_bspline_workspace * w);
size_t gsl_bspline_nbreak (gsl_bspline_workspace * w);
double gsl_bspline_breakpoint (size_t i, gsl_bspline_workspace * w);

int
gsl_bspline_knots(const gsl_vector *breakpts, gsl_bspline_workspace *w);

int gsl_bspline_knots_uniform(const double a, const double b,
                              gsl_bspline_workspace *w);

int
gsl_bspline_eval(const double x, gsl_vector *B,
                 gsl_bspline_workspace *w);

__END_DECLS

#endif /* __GSL_BSPLINE_H__ */
