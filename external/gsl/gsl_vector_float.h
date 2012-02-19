/* vector/gsl_vector_float.h
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

#ifndef __GSL_VECTOR_FLOAT_H__
#define __GSL_VECTOR_FLOAT_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_block_float.h"

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
  float *data;
  gsl_block_float *block;
  int owner;
} 
gsl_vector_float;

typedef struct
{
  gsl_vector_float vector;
} _gsl_vector_float_view;

typedef _gsl_vector_float_view gsl_vector_float_view;

typedef struct
{
  gsl_vector_float vector;
} _gsl_vector_float_const_view;

typedef const _gsl_vector_float_const_view gsl_vector_float_const_view;


/* Allocation */

gsl_vector_float *gsl_vector_float_alloc (const size_t n);
gsl_vector_float *gsl_vector_float_calloc (const size_t n);

gsl_vector_float *gsl_vector_float_alloc_from_block (gsl_block_float * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

gsl_vector_float *gsl_vector_float_alloc_from_vector (gsl_vector_float * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

void gsl_vector_float_free (gsl_vector_float * v);

/* Views */

_gsl_vector_float_view 
gsl_vector_float_view_array (float *v, size_t n);

_gsl_vector_float_view 
gsl_vector_float_view_array_with_stride (float *base,
                                         size_t stride,
                                         size_t n);

_gsl_vector_float_const_view 
gsl_vector_float_const_view_array (const float *v, size_t n);

_gsl_vector_float_const_view 
gsl_vector_float_const_view_array_with_stride (const float *base,
                                               size_t stride,
                                               size_t n);

_gsl_vector_float_view 
gsl_vector_float_subvector (gsl_vector_float *v, 
                            size_t i, 
                            size_t n);

_gsl_vector_float_view 
gsl_vector_float_subvector_with_stride (gsl_vector_float *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

_gsl_vector_float_const_view 
gsl_vector_float_const_subvector (const gsl_vector_float *v, 
                                  size_t i, 
                                  size_t n);

_gsl_vector_float_const_view 
gsl_vector_float_const_subvector_with_stride (const gsl_vector_float *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

float gsl_vector_float_get (const gsl_vector_float * v, const size_t i);
void gsl_vector_float_set (gsl_vector_float * v, const size_t i, float x);

float *gsl_vector_float_ptr (gsl_vector_float * v, const size_t i);
const float *gsl_vector_float_const_ptr (const gsl_vector_float * v, const size_t i);

void gsl_vector_float_set_zero (gsl_vector_float * v);
void gsl_vector_float_set_all (gsl_vector_float * v, float x);
int gsl_vector_float_set_basis (gsl_vector_float * v, size_t i);

int gsl_vector_float_fread (FILE * stream, gsl_vector_float * v);
int gsl_vector_float_fwrite (FILE * stream, const gsl_vector_float * v);
int gsl_vector_float_fscanf (FILE * stream, gsl_vector_float * v);
int gsl_vector_float_fprintf (FILE * stream, const gsl_vector_float * v,
                              const char *format);

int gsl_vector_float_memcpy (gsl_vector_float * dest, const gsl_vector_float * src);

int gsl_vector_float_reverse (gsl_vector_float * v);

int gsl_vector_float_swap (gsl_vector_float * v, gsl_vector_float * w);
int gsl_vector_float_swap_elements (gsl_vector_float * v, const size_t i, const size_t j);

float gsl_vector_float_max (const gsl_vector_float * v);
float gsl_vector_float_min (const gsl_vector_float * v);
void gsl_vector_float_minmax (const gsl_vector_float * v, float * min_out, float * max_out);

size_t gsl_vector_float_max_index (const gsl_vector_float * v);
size_t gsl_vector_float_min_index (const gsl_vector_float * v);
void gsl_vector_float_minmax_index (const gsl_vector_float * v, size_t * imin, size_t * imax);

int gsl_vector_float_add (gsl_vector_float * a, const gsl_vector_float * b);
int gsl_vector_float_sub (gsl_vector_float * a, const gsl_vector_float * b);
int gsl_vector_float_mul (gsl_vector_float * a, const gsl_vector_float * b);
int gsl_vector_float_div (gsl_vector_float * a, const gsl_vector_float * b);
int gsl_vector_float_scale (gsl_vector_float * a, const double x);
int gsl_vector_float_add_constant (gsl_vector_float * a, const double x);

int gsl_vector_float_isnull (const gsl_vector_float * v);
int gsl_vector_float_ispos (const gsl_vector_float * v);
int gsl_vector_float_isneg (const gsl_vector_float * v);
int gsl_vector_float_isnonneg (const gsl_vector_float * v);

#ifdef HAVE_INLINE

extern inline
float
gsl_vector_float_get (const gsl_vector_float * v, const size_t i)
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
gsl_vector_float_set (gsl_vector_float * v, const size_t i, float x)
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
float *
gsl_vector_float_ptr (gsl_vector_float * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (float *) (v->data + i * v->stride);
}

extern inline
const float *
gsl_vector_float_const_ptr (const gsl_vector_float * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const float *) (v->data + i * v->stride);
}


#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_FLOAT_H__ */


