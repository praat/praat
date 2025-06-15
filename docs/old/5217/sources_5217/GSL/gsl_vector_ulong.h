/* vector/gsl_vector_ulong.h
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

#ifndef __GSL_VECTOR_ULONG_H__
#define __GSL_VECTOR_ULONG_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_block_ulong.h"

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
  unsigned long *data;
  gsl_block_ulong *block;
  int owner;
} 
gsl_vector_ulong;

typedef struct
{
  gsl_vector_ulong vector;
} _gsl_vector_ulong_view;

typedef _gsl_vector_ulong_view gsl_vector_ulong_view;

typedef struct
{
  gsl_vector_ulong vector;
} _gsl_vector_ulong_const_view;

typedef const _gsl_vector_ulong_const_view gsl_vector_ulong_const_view;


/* Allocation */

gsl_vector_ulong *gsl_vector_ulong_alloc (const size_t n);
gsl_vector_ulong *gsl_vector_ulong_calloc (const size_t n);

gsl_vector_ulong *gsl_vector_ulong_alloc_from_block (gsl_block_ulong * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

gsl_vector_ulong *gsl_vector_ulong_alloc_from_vector (gsl_vector_ulong * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

void gsl_vector_ulong_free (gsl_vector_ulong * v);

/* Views */

_gsl_vector_ulong_view 
gsl_vector_ulong_view_array (unsigned long *v, size_t n);

_gsl_vector_ulong_view 
gsl_vector_ulong_view_array_with_stride (unsigned long *base,
                                         size_t stride,
                                         size_t n);

_gsl_vector_ulong_const_view 
gsl_vector_ulong_const_view_array (const unsigned long *v, size_t n);

_gsl_vector_ulong_const_view 
gsl_vector_ulong_const_view_array_with_stride (const unsigned long *base,
                                               size_t stride,
                                               size_t n);

_gsl_vector_ulong_view 
gsl_vector_ulong_subvector (gsl_vector_ulong *v, 
                            size_t i, 
                            size_t n);

_gsl_vector_ulong_view 
gsl_vector_ulong_subvector_with_stride (gsl_vector_ulong *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

_gsl_vector_ulong_const_view 
gsl_vector_ulong_const_subvector (const gsl_vector_ulong *v, 
                                  size_t i, 
                                  size_t n);

_gsl_vector_ulong_const_view 
gsl_vector_ulong_const_subvector_with_stride (const gsl_vector_ulong *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

unsigned long gsl_vector_ulong_get (const gsl_vector_ulong * v, const size_t i);
void gsl_vector_ulong_set (gsl_vector_ulong * v, const size_t i, unsigned long x);

unsigned long *gsl_vector_ulong_ptr (gsl_vector_ulong * v, const size_t i);
const unsigned long *gsl_vector_ulong_const_ptr (const gsl_vector_ulong * v, const size_t i);

void gsl_vector_ulong_set_zero (gsl_vector_ulong * v);
void gsl_vector_ulong_set_all (gsl_vector_ulong * v, unsigned long x);
int gsl_vector_ulong_set_basis (gsl_vector_ulong * v, size_t i);

int gsl_vector_ulong_fread (FILE * stream, gsl_vector_ulong * v);
int gsl_vector_ulong_fwrite (FILE * stream, const gsl_vector_ulong * v);
int gsl_vector_ulong_fscanf (FILE * stream, gsl_vector_ulong * v);
int gsl_vector_ulong_fprintf (FILE * stream, const gsl_vector_ulong * v,
                              const char *format);

int gsl_vector_ulong_memcpy (gsl_vector_ulong * dest, const gsl_vector_ulong * src);

int gsl_vector_ulong_reverse (gsl_vector_ulong * v);

int gsl_vector_ulong_swap (gsl_vector_ulong * v, gsl_vector_ulong * w);
int gsl_vector_ulong_swap_elements (gsl_vector_ulong * v, const size_t i, const size_t j);

unsigned long gsl_vector_ulong_max (const gsl_vector_ulong * v);
unsigned long gsl_vector_ulong_min (const gsl_vector_ulong * v);
void gsl_vector_ulong_minmax (const gsl_vector_ulong * v, unsigned long * min_out, unsigned long * max_out);

size_t gsl_vector_ulong_max_index (const gsl_vector_ulong * v);
size_t gsl_vector_ulong_min_index (const gsl_vector_ulong * v);
void gsl_vector_ulong_minmax_index (const gsl_vector_ulong * v, size_t * imin, size_t * imax);

int gsl_vector_ulong_add (gsl_vector_ulong * a, const gsl_vector_ulong * b);
int gsl_vector_ulong_sub (gsl_vector_ulong * a, const gsl_vector_ulong * b);
int gsl_vector_ulong_mul (gsl_vector_ulong * a, const gsl_vector_ulong * b);
int gsl_vector_ulong_div (gsl_vector_ulong * a, const gsl_vector_ulong * b);
int gsl_vector_ulong_scale (gsl_vector_ulong * a, const double x);
int gsl_vector_ulong_add_constant (gsl_vector_ulong * a, const double x);

int gsl_vector_ulong_isnull (const gsl_vector_ulong * v);
int gsl_vector_ulong_ispos (const gsl_vector_ulong * v);
int gsl_vector_ulong_isneg (const gsl_vector_ulong * v);
int gsl_vector_ulong_isnonneg (const gsl_vector_ulong * v);

#ifdef HAVE_INLINE

extern inline
unsigned long
gsl_vector_ulong_get (const gsl_vector_ulong * v, const size_t i)
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
gsl_vector_ulong_set (gsl_vector_ulong * v, const size_t i, unsigned long x)
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
unsigned long *
gsl_vector_ulong_ptr (gsl_vector_ulong * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (unsigned long *) (v->data + i * v->stride);
}

extern inline
const unsigned long *
gsl_vector_ulong_const_ptr (const gsl_vector_ulong * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const unsigned long *) (v->data + i * v->stride);
}


#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_ULONG_H__ */


