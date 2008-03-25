/* bspline/bspline.c
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

#include "gsl__config.h"
#include "gsl_errno.h"
#include "gsl_bspline.h"

/*
 * This module contains routines related to calculating B-splines.
 * The algorithms used are described in
 *
 * [1] Carl de Boor, "A Practical Guide to Splines", Springer
 *     Verlag, 1978.
 */

static int bspline_eval_all(const double x, gsl_vector *B, size_t *idx,
                            gsl_bspline_workspace *w);

static inline size_t bspline_find_interval(const double x, int *flag,
                                           gsl_bspline_workspace *w);

/*
gsl_bspline_alloc()
  Allocate space for a bspline workspace. The size of the
workspace is O(5k + nbreak)

Inputs: k      - spline order (cubic = 4)
        nbreak - number of breakpoints

Return: pointer to workspace
*/

gsl_bspline_workspace *
gsl_bspline_alloc(const size_t k, const size_t nbreak)
{
  if (k == 0)
    {
      GSL_ERROR_NULL("k must be at least 1", GSL_EINVAL);
    }
  else if (nbreak < 2)
    {
      GSL_ERROR_NULL("nbreak must be at least 2", GSL_EINVAL);
    }
  else
    {
      gsl_bspline_workspace *w;

      w = (gsl_bspline_workspace *)
          calloc(1, sizeof(gsl_bspline_workspace));
      if (w == 0)
        {
          GSL_ERROR_NULL("failed to allocate space for workspace", GSL_ENOMEM);
        }

      w->k = k;
      w->km1 = k - 1;
      w->nbreak = nbreak;
      w->l = nbreak - 1;
      w->n = w->l + k - 1;

      w->knots = gsl_vector_alloc(w->n + k);
      if (w->knots == 0)
        {
          gsl_bspline_free(w);
          GSL_ERROR_NULL("failed to allocate space for knots vector", GSL_ENOMEM);
        }

      w->deltal = gsl_vector_alloc(k);
      w->deltar = gsl_vector_alloc(k);
      if (!w->deltal || !w->deltar)
        {
          gsl_bspline_free(w);
          GSL_ERROR_NULL("failed to allocate space for delta vectors", GSL_ENOMEM);
        }

      w->B = gsl_vector_alloc(k);
      if (w->B == 0)
        {
          gsl_bspline_free(w);
          GSL_ERROR_NULL("failed to allocate space for temporary spline vector", GSL_ENOMEM);
        }

      return (w);
    }
} /* gsl_bspline_alloc() */

/* Return number of coefficients */
size_t
gsl_bspline_ncoeffs (gsl_bspline_workspace * w)
{
  return w->n;
}

/* Return order */
size_t
gsl_bspline_order (gsl_bspline_workspace * w)
{
  return w->k;
}

/* Return number of breakpoints */
size_t
gsl_bspline_nbreak (gsl_bspline_workspace * w)
{
  return w->nbreak;
}

double
gsl_bspline_breakpoint (size_t i, gsl_bspline_workspace * w)
{
  size_t j = i + w->k - 1;
  return gsl_vector_get(w->knots, j);
}

/*
gsl_bspline_free()
  Free a bspline workspace

Inputs: w - workspace to free

Return: none
*/

void
gsl_bspline_free(gsl_bspline_workspace *w)
{
  if (!w)
    return;

  if (w->knots)
    gsl_vector_free(w->knots);

  if (w->deltal)
    gsl_vector_free(w->deltal);

  if (w->deltar)
    gsl_vector_free(w->deltar);

  if (w->B)
    gsl_vector_free(w->B);

  free(w);
} /* gsl_bspline_free() */

/*
gsl_bspline_knots()
  Compute the knots from the given breakpoints:

knots(1:k) = breakpts(1)
knots(k+1:k+l-1) = breakpts(i), i = 2 .. l
knots(n+1:n+k) = breakpts(l + 1)

where l is the number of polynomial pieces (l = nbreak - 1) and
n = k + l - 1
(using matlab syntax for the arrays)

The repeated knots at the beginning and end of the interval
correspond to the continuity condition there. See pg. 119
of [1].

Inputs: breakpts - breakpoints
        w        - bspline workspace

Return: success or error
*/

int
gsl_bspline_knots(const gsl_vector *breakpts, gsl_bspline_workspace *w)
{
  if (breakpts->size != w->nbreak)
    {
      GSL_ERROR("breakpts vector has wrong size", GSL_EBADLEN);
    }
  else
    {
      size_t i; /* looping */

      for (i = 0; i < w->k; ++i)
        gsl_vector_set(w->knots, i, gsl_vector_get(breakpts, 0));

      for (i = 1; i < w->l; ++i)
        {
          gsl_vector_set(w->knots, w->k - 1 + i,
                         gsl_vector_get(breakpts, i));
        }

      for (i = w->n; i < w->n + w->k; ++i)
        gsl_vector_set(w->knots, i, gsl_vector_get(breakpts, w->l));

      return GSL_SUCCESS;
    }
} /* gsl_bspline_knots() */

/*
gsl_bspline_knots_uniform()
  Construct uniformly spaced knots on the interval [a,b] using
the previously specified number of breakpoints. 'a' is the position
of the first breakpoint and 'b' is the position of the last
breakpoint.

Inputs: a - left side of interval
        b - right side of interval
        w - bspline workspace

Return: success or error

Notes: 1) w->knots is modified to contain the uniformly spaced
          knots

       2) The knots vector is set up as follows (using octave syntax):

          knots(1:k) = a
          knots(k+1:k+l-1) = a + i*delta, i = 1 .. l - 1
          knots(n+1:n+k) = b
*/

int
gsl_bspline_knots_uniform(const double a, const double b,
                          gsl_bspline_workspace *w)
{
  size_t i;     /* looping */
  double delta; /* interval spacing */
  double x;

  delta = (b - a) / (double) w->l;

  for (i = 0; i < w->k; ++i)
    gsl_vector_set(w->knots, i, a);

  x = a + delta;
  for (i = 0; i < w->l - 1; ++i)
    {
      gsl_vector_set(w->knots, w->k + i, x);
      x += delta;
    }

  for (i = w->n; i < w->n + w->k; ++i)
    gsl_vector_set(w->knots, i, b);

  return GSL_SUCCESS;
} /* gsl_bspline_knots_uniform() */

/*
gsl_bspline_eval()
  Evaluate the basis functions B_i(x) for all i. This is
basically a wrapper function for bspline_eval_all()
which formats the output in a nice way.

Inputs: x - point for evaluation
        B - (output) where to store B_i(x) values
            the length of this vector is
            n = nbreak + k - 2 = l + k - 1 = w->n
        w - bspline workspace

Return: success or error

Notes: The w->knots vector must be initialized prior to calling
       this function (see gsl_bspline_knots())
*/

int
gsl_bspline_eval(const double x, gsl_vector *B,
                 gsl_bspline_workspace *w)
{
  if (B->size != w->n)
    {
      GSL_ERROR("B vector length does not match n", GSL_EBADLEN);
    }
  else
    {
      size_t i;     /* looping */
      size_t idx = 0;
      size_t start; /* first non-zero spline */

      /* find all non-zero B_i(x) values */
      bspline_eval_all(x, w->B, &idx, w);

      /* store values in appropriate part of given vector */

      start = idx - w->k + 1;
      for (i = 0; i < start; ++i)
        gsl_vector_set(B, i, 0.0);

      for (i = start; i <= idx; ++i)
        gsl_vector_set(B, i, gsl_vector_get(w->B, i - start));

      for (i = idx + 1; i < w->n; ++i)
        gsl_vector_set(B, i, 0.0);

      return GSL_SUCCESS;
    }
} /* gsl_bspline_eval() */

/****************************************
 *          INTERNAL ROUTINES           *
 ****************************************/

/*
bspline_eval_all()
  Evaluate all non-zero B-splines B_i(x) using algorithm (8)
of chapter X of [1]

The idea is something like this. Given x \in [t_i, t_{i+1}]
with t_i < t_{i+1} and the t_i are knots, the values of the
B-splines not automatically zero fit into a triangular
array as follows:
                         0
            0
0                        B_{i-2,3}
            B_{i-1,2}
B_{i,1}                  B_{i-1,3}       ...
            B_{i,2}
0                        B_{i,3}
            0
                         0

where B_{i,k} is the ith B-spline of order k. The boundary 0s
indicate that those B-splines not in the table vanish at x.

To compute the non-zero B-splines of a given order k, we use
Eqs. (4) and (5) of chapter X of [1]:

(4) B_{i,1}(x) = { 1, t_i <= x < t_{i+1}
                   0, else }

(5) B_{i,k}(x) =     (x - t_i)
                 ----------------- B_{i,k-1}(x)
                 (t_{i+k-1} - t_i)

                      t_{i+k} - x
                 + ----------------- B_{i+1,k-1}(x)
                   t_{i+k} - t_{i+1}

So (4) gives us the first column of the table and we can use
the recurrence relation (5) to get the rest of the columns.

Inputs: x   - point at which to evaluate splines
        B   - (output) where to store B-spline values (length k)
        idx - (output) B-spline function index of last output
              value (B_{idx}(x) is stored in the last slot of 'B')
        w   - bspline workspace

Return: success or error

Notes: 1) the w->knots vector must be initialized before calling
          this function

       2) On output, B contains:

       B = [B_{i-k+1,k}, B_{i-k+2,k}, ..., B_{i-1,k}, B_{i,k}]

          where 'i' is stored in 'idx' on output

       3) based on PPPACK bsplvb
*/

static int
bspline_eval_all(const double x, gsl_vector *B, size_t *idx,
                 gsl_bspline_workspace *w)
{
  if (B->size != w->k)
    {
      GSL_ERROR("B vector not of length k", GSL_EBADLEN);
    }
  else
    {
      size_t i;  /* spline index */
      size_t j;  /* looping */
      size_t ii; /* looping */
      int flag = 0;  /* error flag */
      double saved;
      double term;

      i = bspline_find_interval(x, &flag, w);

      if (flag == -1)
        {
          GSL_ERROR("x outside of knot interval", GSL_EINVAL);
        }
      else if (flag == 1)
        {
          if (x <= gsl_vector_get(w->knots, i) + GSL_DBL_EPSILON)
            {
              --i;
            }
          else
            {
              GSL_ERROR("x outside of knot interval", GSL_EINVAL);
            }
        }

      *idx = i;

      gsl_vector_set(B, 0, 1.0);

      for (j = 0; j < w->k - 1; ++j)
        {
          gsl_vector_set(w->deltar, j,
                         gsl_vector_get(w->knots, i + j + 1) - x);
          gsl_vector_set(w->deltal, j,
                         x - gsl_vector_get(w->knots, i - j));

          saved = 0.0;

          for (ii = 0; ii <= j; ++ii)
            {
              term = gsl_vector_get(B, ii) /
                     (gsl_vector_get(w->deltar, ii) +
                      gsl_vector_get(w->deltal, j - ii));

              gsl_vector_set(B, ii,
                             saved +
                             gsl_vector_get(w->deltar, ii) * term);

              saved = gsl_vector_get(w->deltal, j - ii) * term;
            }

          gsl_vector_set(B, j + 1, saved);
        }

      return GSL_SUCCESS;
    }
} /* bspline_eval_all() */

/*
bspline_find_interval()
  Find knot interval such that

  t_i <= x < t_{i + 1}

where the t_i are knot values.

Inputs: x    - x value
        flag - (output) error flag
        w    - bspline workspace

Return: i (index in w->knots corresponding to left limit of interval)

Notes: The error conditions are reported as follows:

Condition             Return value        Flag
---------             ------------        ----
x < t_0                    0               -1
t_i <= x < t_{i+1}         i                0
t_{n+k-1} <= x           l+k-1             +1
*/

static inline size_t
bspline_find_interval(const double x, int *flag,
                      gsl_bspline_workspace *w)
{
  size_t i;

  if (x < gsl_vector_get(w->knots, 0))
    {
      *flag = -1;
      return 0;
    }

  /* find i such that t_i <= x < t_{i+1} */
  for (i = w->k - 1; i < w->k + w->l - 1; ++i)
    {
      const double ti = gsl_vector_get(w->knots, i);
      const double tip1 = gsl_vector_get(w->knots, i + 1);

      if (tip1 < ti)
        {
          GSL_ERROR("knots vector is not increasing", GSL_EINVAL);
        }

      if (ti <= x && x < tip1)
        break;
    }

  if (i == w->k + w->l - 1)
    *flag = 1;
  else
    *flag = 0;

  return i;
} /* bspline_find_interval() */
