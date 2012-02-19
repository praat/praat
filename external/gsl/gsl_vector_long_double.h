/* vector/gsl_vector_long_double.h
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

#ifndef __GSL_VECTOR_LONG_DOUBLE_H__
#define __GSL_VECTOR_LONG_DOUBLE_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_block_long_double.h"

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
  size_t size;
  size_t stride;
  long double *data;
  gsl_block_long_double *block;
  int owner;
} 
gsl_vector_long_double;

typedef struct
{
  gsl_vector_long_double vector;
} _gsl_vector_long_double_view;

typedef _gsl_vector_long_double_view gsl_vector_long_double_view;

typedef struct
{
  gsl_vector_long_double vector;
} _gsl_vector_long_double_const_view;

typedef const _gsl_vector_long_double_const_view gsl_vector_long_double_const_view;


/* Allocation */

gsl_vector_long_double *gsl_vector_long_double_alloc (const size_t n);
gsl_vector_long_double *gsl_vector_long_double_calloc (const size_t n);

gsl_vector_long_double *gsl_vector_long_double_alloc_from_block (gsl_block_long_double * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

gsl_vector_long_double *gsl_vector_long_double_alloc_from_vector (gsl_vector_long_double * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

void gsl_vector_long_double_free (gsl_vector_long_double * v);

/* Views */

_gsl_vector_long_double_view 
gsl_vector_long_double_view_array (long double *v, size_t n);

_gsl_vector_long_double_view 
gsl_vector_long_double_view_array_with_stride (long double *base,
                                         size_t stride,
                                         size_t n);

_gsl_vector_long_double_const_view 
gsl_vector_long_double_const_view_array (const long double *v, size_t n);

_gsl_vector_long_double_const_view 
gsl_vector_long_double_const_view_array_with_stride (const long double *base,
                                               size_t stride,
                                               size_t n);

_gsl_vector_long_double_view 
gsl_vector_long_double_subvector (gsl_vector_long_double *v, 
                            size_t i, 
                            size_t n);

_gsl_vector_long_double_view 
gsl_vector_long_double_subvector_with_stride (gsl_vector_long_double *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

_gsl_vector_long_double_const_view 
gsl_vector_long_double_const_subvector (const gsl_vector_long_double *v, 
                                  size_t i, 
                                  size_t n);

_gsl_vector_long_double_const_view 
gsl_vector_long_double_const_subvector_with_stride (const gsl_vector_long_double *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

long double gsl_vector_long_double_get (const gsl_vector_long_double * v, const size_t i);
void gsl_vector_long_double_set (gsl_vector_long_double * v, const size_t i, long double x);

long double *gsl_vector_long_double_ptr (gsl_vector_long_double * v, const size_t i);
const long double *gsl_vector_long_double_const_ptr (const gsl_vector_long_double * v, const size_t i);

void gsl_vector_long_double_set_zero (gsl_vector_long_double * v);
void gsl_vector_long_double_set_all (gsl_vector_long_double * v, long double x);
int gsl_vector_long_double_set_basis (gsl_vector_long_double * v, size_t i);

int gsl_vector_long_double_fread (FILE * stream, gsl_vector_long_double * v);
int gsl_vector_long_double_fwrite (FILE * stream, const gsl_vector_long_double * v);
int gsl_vector_long_double_fscanf (FILE * stream, gsl_vector_long_double * v);
int gsl_vector_long_double_fprintf (FILE * stream, const gsl_vector_long_double * v,
                              const char *format);

int gsl_vector_long_double_memcpy (gsl_vector_long_double * dest, const gsl_vector_long_double * src);

int gsl_vector_long_double_reverse (gsl_vector_long_double * v);

int gsl_vector_long_double_swap (gsl_vector_long_double * v, gsl_vector_long_double * w);
int gsl_vector_long_double_swap_elements (gsl_vector_long_double * v, const size_t i, const size_t j);

long double gsl_vector_long_double_max (const gsl_vector_long_double * v);
long double gsl_vector_long_double_min (const gsl_vector_long_double * v);
void gsl_vector_long_double_minmax (const gsl_vector_long_double * v, long double * min_out, long double * max_out);

size_t gsl_vector_long_double_max_index (const gsl_vector_long_double * v);
size_t gsl_vector_long_double_min_index (const gsl_vector_long_double * v);
void gsl_vector_long_double_minmax_index (const gsl_vector_long_double * v, size_t * imin, size_t * imax);

int gsl_vector_long_double_add (gsl_vector_long_double * a, const gsl_vector_long_double * b);
int gsl_vector_long_double_sub (gsl_vector_long_double * a, const gsl_vector_long_double * b);
int gsl_vector_long_double_mul (gsl_vector_long_double * a, const gsl_vector_long_double * b);
int gsl_vector_long_double_div (gsl_vector_long_double * a, const gsl_vector_long_double * b);
int gsl_vector_long_double_scale (gsl_vector_long_double * a, const double x);
int gsl_vector_long_double_add_constant (gsl_vector_long_double * a, const double x);

int gsl_vector_long_double_isnull (const gsl_vector_long_double * v);
int gsl_vector_long_double_ispos (const gsl_vector_long_double * v);
int gsl_vector_long_double_isneg (const gsl_vector_long_double * v);
int gsl_vector_long_double_isnonneg (const gsl_vector_long_double * v);

#ifdef HAVE_INLINE

extern inline
long double
gsl_vector_long_double_get (const gsl_vector_long_double * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_VAL ("index out of range", GSL_EINVAL, 0);
    }
#endif
  return v->data[i * v->stride];
}

extern inline
void
gsl_vector_long_double_set (gsl_vector_long_double * v, const size_t i, long double x)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_VOID ("index out of range", GSL_EINVAL);
    }
#endif
  v->data[i * v->stride] = x;
}

extern inline
long double *
gsl_vector_long_double_ptr (gsl_vector_long_double * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (long double *) (v->data + i * v->stride);
}

extern inline
const long double *
gsl_vector_long_double_const_ptr (const gsl_vector_long_double * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const long double *) (v->data + i * v->stride);
}


#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_LONG_DOUBLE_H__ */


