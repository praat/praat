/* linalg/tridiag.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2002 Gerard Jungman,
 * Brian Gough, David Necas
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

/* Author:  G. Jungman
 */
/* Low level tridiagonal solvers.
 * Used internally in other areas of GSL.
 */
#ifndef __GSL_TRIDIAG_H__
#define __GSL_TRIDIAG_H__

#include <stdlib.h>

static
int solve_tridiag_nonsym(
  const double diag[], size_t d_stride,
  const double abovediag[], size_t a_stride,
  const double belowdiag[], size_t b_stride,
  const double rhs[], size_t r_stride,
  double x[], size_t x_stride,
  size_t N
  );

static
int solve_tridiag(
  const double diag[], size_t d_stride,
  const double offdiag[], size_t o_stride,
  const double b[], size_t b_stride,
  double x[], size_t x_stride,
  size_t N);

static
int solve_cyc_tridiag(
  const double diag[], size_t d_stride,
  const double offdiag[], size_t o_stride,
  const double b[], size_t b_stride,
  double x[], size_t x_stride,
  size_t N
  );

static
int solve_cyc_tridiag_nonsym(
  const double diag[], size_t d_stride,
  const double abovediag[], size_t a_stride,
  const double belowdiag[], size_t b_stride,
  const double rhs[], size_t r_stride,
  double x[], size_t x_stride,
  size_t N);

#endif /* __GSL_TRIDIAG_H__ */
