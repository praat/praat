/* vector/gsl_vector_uchar.h
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

#ifndef __GSL_VECTOR_UCHAR_H__
#define __GSL_VECTOR_UCHAR_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_block_uchar.h"

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
  unsigned char *data;
  gsl_block_uchar *block;
  int owner;
} 
gsl_vector_uchar;

typedef struct
{
  gsl_vector_uchar vector;
} _gsl_vector_uchar_view;

typedef _gsl_vector_uchar_view gsl_vector_uchar_view;

typedef struct
{
  gsl_vector_uchar vector;
} _gsl_vector_uchar_const_view;

typedef const _gsl_vector_uchar_const_view gsl_vector_uchar_const_view;


/* Allocation */

gsl_vector_uchar *gsl_vector_uchar_alloc (const size_t n);
gsl_vector_uchar *gsl_vector_uchar_calloc (const size_t n);

gsl_vector_uchar *gsl_vector_uchar_alloc_from_block (gsl_block_uchar * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

gsl_vector_uchar *gsl_vector_uchar_alloc_from_vector (gsl_vector_uchar * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

void gsl_vector_uchar_free (gsl_vector_uchar * v);

/* Views */

_gsl_vector_uchar_view 
gsl_vector_uchar_view_array (unsigned char *v, size_t n);

_gsl_vector_uchar_view 
gsl_vector_uchar_view_array_with_stride (unsigned char *base,
                                         size_t stride,
                                         size_t n);

_gsl_vector_uchar_const_view 
gsl_vector_uchar_const_view_array (const unsigned char *v, size_t n);

_gsl_vector_uchar_const_view 
gsl_vector_uchar_const_view_array_with_stride (const unsigned char *base,
                                               size_t stride,
                                               size_t n);

_gsl_vector_uchar_view 
gsl_vector_uchar_subvector (gsl_vector_uchar *v, 
                            size_t i, 
                            size_t n);

_gsl_vector_uchar_view 
gsl_vector_uchar_subvector_with_stride (gsl_vector_uchar *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

_gsl_vector_uchar_const_view 
gsl_vector_uchar_const_subvector (const gsl_vector_uchar *v, 
                                  size_t i, 
                                  size_t n);

_gsl_vector_uchar_const_view 
gsl_vector_uchar_const_subvector_with_stride (const gsl_vector_uchar *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

unsigned char gsl_vector_uchar_get (const gsl_vector_uchar * v, const size_t i);
void gsl_vector_uchar_set (gsl_vector_uchar * v, const size_t i, unsigned char x);

unsigned char *gsl_vector_uchar_ptr (gsl_vector_uchar * v, const size_t i);
const unsigned char *gsl_vector_uchar_const_ptr (const gsl_vector_uchar * v, const size_t i);

void gsl_vector_uchar_set_zero (gsl_vector_uchar * v);
void gsl_vector_uchar_set_all (gsl_vector_uchar * v, unsigned char x);
int gsl_vector_uchar_set_basis (gsl_vector_uchar * v, size_t i);

int gsl_vector_uchar_fread (FILE * stream, gsl_vector_uchar * v);
int gsl_vector_uchar_fwrite (FILE * stream, const gsl_vector_uchar * v);
int gsl_vector_uchar_fscanf (FILE * stream, gsl_vector_uchar * v);
int gsl_vector_uchar_fprintf (FILE * stream, const gsl_vector_uchar * v,
                              const char *format);

int gsl_vector_uchar_memcpy (gsl_vector_uchar * dest, const gsl_vector_uchar * src);

int gsl_vector_uchar_reverse (gsl_vector_uchar * v);

int gsl_vector_uchar_swap (gsl_vector_uchar * v, gsl_vector_uchar * w);
int gsl_vector_uchar_swap_elements (gsl_vector_uchar * v, const size_t i, const size_t j);

unsigned char gsl_vector_uchar_max (const gsl_vector_uchar * v);
unsigned char gsl_vector_uchar_min (const gsl_vector_uchar * v);
void gsl_vector_uchar_minmax (const gsl_vector_uchar * v, unsigned char * min_out, unsigned char * max_out);

size_t gsl_vector_uchar_max_index (const gsl_vector_uchar * v);
size_t gsl_vector_uchar_min_index (const gsl_vector_uchar * v);
void gsl_vector_uchar_minmax_index (const gsl_vector_uchar * v, size_t * imin, size_t * imax);

int gsl_vector_uchar_add (gsl_vector_uchar * a, const gsl_vector_uchar * b);
int gsl_vector_uchar_sub (gsl_vector_uchar * a, const gsl_vector_uchar * b);
int gsl_vector_uchar_mul (gsl_vector_uchar * a, const gsl_vector_uchar * b);
int gsl_vector_uchar_div (gsl_vector_uchar * a, const gsl_vector_uchar * b);
int gsl_vector_uchar_scale (gsl_vector_uchar * a, const double x);
int gsl_vector_uchar_add_constant (gsl_vector_uchar * a, const double x);

int gsl_vector_uchar_isnull (const gsl_vector_uchar * v);
int gsl_vector_uchar_ispos (const gsl_vector_uchar * v);
int gsl_vector_uchar_isneg (const gsl_vector_uchar * v);
int gsl_vector_uchar_isnonneg (const gsl_vector_uchar * v);

#ifdef HAVE_INLINE

extern inline
unsigned char
gsl_vector_uchar_get (const gsl_vector_uchar * v, const size_t i)
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
gsl_vector_uchar_set (gsl_vector_uchar * v, const size_t i, unsigned char x)
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
unsigned char *
gsl_vector_uchar_ptr (gsl_vector_uchar * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (unsigned char *) (v->data + i * v->stride);
}

extern inline
const unsigned char *
gsl_vector_uchar_const_ptr (const gsl_vector_uchar * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (i >= v->size)
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const unsigned char *) (v->data + i * v->stride);
}


#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_UCHAR_H__ */


