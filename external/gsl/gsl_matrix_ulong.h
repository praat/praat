/* matrix/gsl_matrix_ulong.h
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

#ifndef __GSL_MATRIX_ULONG_H__
#define __GSL_MATRIX_ULONG_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_vector_ulong.h"

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
  size_t size1;
  size_t size2;
  size_t tda;
  unsigned long * data;
  gsl_block_ulong * block;
  int owner;
} gsl_matrix_ulong;

typedef struct
{
  gsl_matrix_ulong matrix;
} _gsl_matrix_ulong_view;

typedef _gsl_matrix_ulong_view gsl_matrix_ulong_view;

typedef struct
{
  gsl_matrix_ulong matrix;
} _gsl_matrix_ulong_const_view;

typedef const _gsl_matrix_ulong_const_view gsl_matrix_ulong_const_view;

/* Allocation */

gsl_matrix_ulong * 
gsl_matrix_ulong_alloc (const size_t n1, const size_t n2);

gsl_matrix_ulong * 
gsl_matrix_ulong_calloc (const size_t n1, const size_t n2);

gsl_matrix_ulong * 
gsl_matrix_ulong_alloc_from_block (gsl_block_ulong * b, 
                                   const size_t offset, 
                                   const size_t n1, 
                                   const size_t n2, 
                                   const size_t d2);

gsl_matrix_ulong * 
gsl_matrix_ulong_alloc_from_matrix (gsl_matrix_ulong * m,
                                    const size_t k1, 
                                    const size_t k2,
                                    const size_t n1, 
                                    const size_t n2);

gsl_vector_ulong * 
gsl_vector_ulong_alloc_row_from_matrix (gsl_matrix_ulong * m,
                                        const size_t i);

gsl_vector_ulong * 
gsl_vector_ulong_alloc_col_from_matrix (gsl_matrix_ulong * m,
                                        const size_t j);

void gsl_matrix_ulong_free (gsl_matrix_ulong * m);

/* Views */

_gsl_matrix_ulong_view 
gsl_matrix_ulong_submatrix (gsl_matrix_ulong * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

_gsl_vector_ulong_view 
gsl_matrix_ulong_row (gsl_matrix_ulong * m, const size_t i);

_gsl_vector_ulong_view 
gsl_matrix_ulong_column (gsl_matrix_ulong * m, const size_t j);

_gsl_vector_ulong_view 
gsl_matrix_ulong_diagonal (gsl_matrix_ulong * m);

_gsl_vector_ulong_view 
gsl_matrix_ulong_subdiagonal (gsl_matrix_ulong * m, const size_t k);

_gsl_vector_ulong_view 
gsl_matrix_ulong_superdiagonal (gsl_matrix_ulong * m, const size_t k);

_gsl_vector_ulong_view
gsl_matrix_ulong_subrow (gsl_matrix_ulong * m, const size_t i,
                         const size_t offset, const size_t n);

_gsl_vector_ulong_view
gsl_matrix_ulong_subcolumn (gsl_matrix_ulong * m, const size_t j,
                            const size_t offset, const size_t n);

_gsl_matrix_ulong_view
gsl_matrix_ulong_view_array (unsigned long * base,
                             const size_t n1, 
                             const size_t n2);

_gsl_matrix_ulong_view
gsl_matrix_ulong_view_array_with_tda (unsigned long * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);


_gsl_matrix_ulong_view
gsl_matrix_ulong_view_vector (gsl_vector_ulong * v,
                              const size_t n1, 
                              const size_t n2);

_gsl_matrix_ulong_view
gsl_matrix_ulong_view_vector_with_tda (gsl_vector_ulong * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


_gsl_matrix_ulong_const_view 
gsl_matrix_ulong_const_submatrix (const gsl_matrix_ulong * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

_gsl_vector_ulong_const_view 
gsl_matrix_ulong_const_row (const gsl_matrix_ulong * m, 
                            const size_t i);

_gsl_vector_ulong_const_view 
gsl_matrix_ulong_const_column (const gsl_matrix_ulong * m, 
                               const size_t j);

_gsl_vector_ulong_const_view
gsl_matrix_ulong_const_diagonal (const gsl_matrix_ulong * m);

_gsl_vector_ulong_const_view 
gsl_matrix_ulong_const_subdiagonal (const gsl_matrix_ulong * m, 
                                    const size_t k);

_gsl_vector_ulong_const_view 
gsl_matrix_ulong_const_superdiagonal (const gsl_matrix_ulong * m, 
                                      const size_t k);

_gsl_vector_ulong_const_view
gsl_matrix_ulong_const_subrow (const gsl_matrix_ulong * m, const size_t i,
                               const size_t offset, const size_t n);

_gsl_vector_ulong_const_view
gsl_matrix_ulong_const_subcolumn (const gsl_matrix_ulong * m, const size_t j,
                                  const size_t offset, const size_t n);

_gsl_matrix_ulong_const_view
gsl_matrix_ulong_const_view_array (const unsigned long * base,
                                   const size_t n1, 
                                   const size_t n2);

_gsl_matrix_ulong_const_view
gsl_matrix_ulong_const_view_array_with_tda (const unsigned long * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

_gsl_matrix_ulong_const_view
gsl_matrix_ulong_const_view_vector (const gsl_vector_ulong * v,
                                    const size_t n1, 
                                    const size_t n2);

_gsl_matrix_ulong_const_view
gsl_matrix_ulong_const_view_vector_with_tda (const gsl_vector_ulong * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

unsigned long   gsl_matrix_ulong_get(const gsl_matrix_ulong * m, const size_t i, const size_t j);
void    gsl_matrix_ulong_set(gsl_matrix_ulong * m, const size_t i, const size_t j, const unsigned long x);

unsigned long * gsl_matrix_ulong_ptr(gsl_matrix_ulong * m, const size_t i, const size_t j);
const unsigned long * gsl_matrix_ulong_const_ptr(const gsl_matrix_ulong * m, const size_t i, const size_t j);

void gsl_matrix_ulong_set_zero (gsl_matrix_ulong * m);
void gsl_matrix_ulong_set_identity (gsl_matrix_ulong * m);
void gsl_matrix_ulong_set_all (gsl_matrix_ulong * m, unsigned long x);

int gsl_matrix_ulong_fread (FILE * stream, gsl_matrix_ulong * m) ;
int gsl_matrix_ulong_fwrite (FILE * stream, const gsl_matrix_ulong * m) ;
int gsl_matrix_ulong_fscanf (FILE * stream, gsl_matrix_ulong * m);
int gsl_matrix_ulong_fprintf (FILE * stream, const gsl_matrix_ulong * m, const char * format);
 
int gsl_matrix_ulong_memcpy(gsl_matrix_ulong * dest, const gsl_matrix_ulong * src);
int gsl_matrix_ulong_swap(gsl_matrix_ulong * m1, gsl_matrix_ulong * m2);

int gsl_matrix_ulong_swap_rows(gsl_matrix_ulong * m, const size_t i, const size_t j);
int gsl_matrix_ulong_swap_columns(gsl_matrix_ulong * m, const size_t i, const size_t j);
int gsl_matrix_ulong_swap_rowcol(gsl_matrix_ulong * m, const size_t i, const size_t j);
int gsl_matrix_ulong_transpose (gsl_matrix_ulong * m);
int gsl_matrix_ulong_transpose_memcpy (gsl_matrix_ulong * dest, const gsl_matrix_ulong * src);

unsigned long gsl_matrix_ulong_max (const gsl_matrix_ulong * m);
unsigned long gsl_matrix_ulong_min (const gsl_matrix_ulong * m);
void gsl_matrix_ulong_minmax (const gsl_matrix_ulong * m, unsigned long * min_out, unsigned long * max_out);

void gsl_matrix_ulong_max_index (const gsl_matrix_ulong * m, size_t * imax, size_t *jmax);
void gsl_matrix_ulong_min_index (const gsl_matrix_ulong * m, size_t * imin, size_t *jmin);
void gsl_matrix_ulong_minmax_index (const gsl_matrix_ulong * m, size_t * imin, size_t * jmin, size_t * imax, size_t * jmax);

int gsl_matrix_ulong_isnull (const gsl_matrix_ulong * m);
int gsl_matrix_ulong_ispos (const gsl_matrix_ulong * m);
int gsl_matrix_ulong_isneg (const gsl_matrix_ulong * m);
int gsl_matrix_ulong_isnonneg (const gsl_matrix_ulong * m);

int gsl_matrix_ulong_add (gsl_matrix_ulong * a, const gsl_matrix_ulong * b);
int gsl_matrix_ulong_sub (gsl_matrix_ulong * a, const gsl_matrix_ulong * b);
int gsl_matrix_ulong_mul_elements (gsl_matrix_ulong * a, const gsl_matrix_ulong * b);
int gsl_matrix_ulong_div_elements (gsl_matrix_ulong * a, const gsl_matrix_ulong * b);
int gsl_matrix_ulong_scale (gsl_matrix_ulong * a, const double x);
int gsl_matrix_ulong_add_constant (gsl_matrix_ulong * a, const double x);
int gsl_matrix_ulong_add_diagonal (gsl_matrix_ulong * a, const double x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
int gsl_matrix_ulong_get_row(gsl_vector_ulong * v, const gsl_matrix_ulong * m, const size_t i);
int gsl_matrix_ulong_get_col(gsl_vector_ulong * v, const gsl_matrix_ulong * m, const size_t j);
int gsl_matrix_ulong_set_row(gsl_matrix_ulong * m, const size_t i, const gsl_vector_ulong * v);
int gsl_matrix_ulong_set_col(gsl_matrix_ulong * m, const size_t j, const gsl_vector_ulong * v);

/* inline functions if you are using GCC */

#ifdef HAVE_INLINE
extern inline 
unsigned long
gsl_matrix_ulong_get(const gsl_matrix_ulong * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (i >= m->size1)
    {
      GSL_ERROR_VAL("first index out of range", GSL_EINVAL, 0) ;
    }
  else if (j >= m->size2)
    {
      GSL_ERROR_VAL("second index out of range", GSL_EINVAL, 0) ;
    }
#endif
  return m->data[i * m->tda + j] ;
} 

extern inline 
void
gsl_matrix_ulong_set(gsl_matrix_ulong * m, const size_t i, const size_t j, const unsigned long x)
{
#if GSL_RANGE_CHECK
  if (i >= m->size1)
    {
      GSL_ERROR_VOID("first index out of range", GSL_EINVAL) ;
    }
  else if (j >= m->size2)
    {
      GSL_ERROR_VOID("second index out of range", GSL_EINVAL) ;
    }
#endif
  m->data[i * m->tda + j] = x ;
}

extern inline 
unsigned long *
gsl_matrix_ulong_ptr(gsl_matrix_ulong * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (i >= m->size1)
    {
      GSL_ERROR_NULL("first index out of range", GSL_EINVAL) ;
    }
  else if (j >= m->size2)
    {
      GSL_ERROR_NULL("second index out of range", GSL_EINVAL) ;
    }
#endif
  return (unsigned long *) (m->data + (i * m->tda + j)) ;
} 

extern inline 
const unsigned long *
gsl_matrix_ulong_const_ptr(const gsl_matrix_ulong * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (i >= m->size1)
    {
      GSL_ERROR_NULL("first index out of range", GSL_EINVAL) ;
    }
  else if (j >= m->size2)
    {
      GSL_ERROR_NULL("second index out of range", GSL_EINVAL) ;
    }
#endif
  return (const unsigned long *) (m->data + (i * m->tda + j)) ;
} 

#endif

__END_DECLS

#endif /* __GSL_MATRIX_ULONG_H__ */
