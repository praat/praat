/* matrix/gsl_matrix_complex_float.h
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

#ifndef __GSL_MATRIX_COMPLEX_FLOAT_H__
#define __GSL_MATRIX_COMPLEX_FLOAT_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_complex.h"
#include "gsl_check_range.h"
#include "gsl_vector_complex_float.h"

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
  float * data;
  gsl_block_complex_float * block;
  int owner;
} gsl_matrix_complex_float ;

typedef struct
{
  gsl_matrix_complex_float matrix;
} _gsl_matrix_complex_float_view;

typedef _gsl_matrix_complex_float_view gsl_matrix_complex_float_view;

typedef struct
{
  gsl_matrix_complex_float matrix;
} _gsl_matrix_complex_float_const_view;

typedef const _gsl_matrix_complex_float_const_view gsl_matrix_complex_float_const_view;


/* Allocation */

gsl_matrix_complex_float * 
gsl_matrix_complex_float_alloc (const size_t n1, const size_t n2);

gsl_matrix_complex_float * 
gsl_matrix_complex_float_calloc (const size_t n1, const size_t n2);

gsl_matrix_complex_float * 
gsl_matrix_complex_float_alloc_from_block (gsl_block_complex_float * b, 
                                           const size_t offset, 
                                           const size_t n1, const size_t n2, const size_t d2);

gsl_matrix_complex_float * 
gsl_matrix_complex_float_alloc_from_matrix (gsl_matrix_complex_float * b,
                                            const size_t k1, const size_t k2,
                                            const size_t n1, const size_t n2);

gsl_vector_complex_float * 
gsl_vector_complex_float_alloc_row_from_matrix (gsl_matrix_complex_float * m,
                                                const size_t i);

gsl_vector_complex_float * 
gsl_vector_complex_float_alloc_col_from_matrix (gsl_matrix_complex_float * m,
                                                const size_t j);

void gsl_matrix_complex_float_free (gsl_matrix_complex_float * m);

/* Views */

_gsl_matrix_complex_float_view 
gsl_matrix_complex_float_submatrix (gsl_matrix_complex_float * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

_gsl_vector_complex_float_view 
gsl_matrix_complex_float_row (gsl_matrix_complex_float * m, const size_t i);

_gsl_vector_complex_float_view 
gsl_matrix_complex_float_column (gsl_matrix_complex_float * m, const size_t j);

_gsl_vector_complex_float_view 
gsl_matrix_complex_float_diagonal (gsl_matrix_complex_float * m);

_gsl_vector_complex_float_view 
gsl_matrix_complex_float_subdiagonal (gsl_matrix_complex_float * m, const size_t k);

_gsl_vector_complex_float_view 
gsl_matrix_complex_float_superdiagonal (gsl_matrix_complex_float * m, const size_t k);

_gsl_vector_complex_float_view
gsl_matrix_complex_float_subrow (gsl_matrix_complex_float * m,
                                 const size_t i, const size_t offset,
                                 const size_t n);

_gsl_vector_complex_float_view
gsl_matrix_complex_float_subcolumn (gsl_matrix_complex_float * m,
                                    const size_t j, const size_t offset,
                                    const size_t n);

_gsl_matrix_complex_float_view
gsl_matrix_complex_float_view_array (float * base,
                             const size_t n1, 
                             const size_t n2);

_gsl_matrix_complex_float_view
gsl_matrix_complex_float_view_array_with_tda (float * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);

_gsl_matrix_complex_float_view
gsl_matrix_complex_float_view_vector (gsl_vector_complex_float * v,
                              const size_t n1, 
                              const size_t n2);

_gsl_matrix_complex_float_view
gsl_matrix_complex_float_view_vector_with_tda (gsl_vector_complex_float * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


_gsl_matrix_complex_float_const_view 
gsl_matrix_complex_float_const_submatrix (const gsl_matrix_complex_float * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

_gsl_vector_complex_float_const_view 
gsl_matrix_complex_float_const_row (const gsl_matrix_complex_float * m, 
                            const size_t i);

_gsl_vector_complex_float_const_view 
gsl_matrix_complex_float_const_column (const gsl_matrix_complex_float * m, 
                               const size_t j);

_gsl_vector_complex_float_const_view
gsl_matrix_complex_float_const_diagonal (const gsl_matrix_complex_float * m);

_gsl_vector_complex_float_const_view 
gsl_matrix_complex_float_const_subdiagonal (const gsl_matrix_complex_float * m, 
                                    const size_t k);

_gsl_vector_complex_float_const_view 
gsl_matrix_complex_float_const_superdiagonal (const gsl_matrix_complex_float * m, 
                                      const size_t k);

_gsl_vector_complex_float_const_view
gsl_matrix_complex_float_const_subrow (const gsl_matrix_complex_float * m,
                                       const size_t i, const size_t offset,
                                       const size_t n);

_gsl_vector_complex_float_const_view
gsl_matrix_complex_float_const_subcolumn (const gsl_matrix_complex_float * m,
                                          const size_t j, const size_t offset,
                                          const size_t n);

_gsl_matrix_complex_float_const_view
gsl_matrix_complex_float_const_view_array (const float * base,
                                   const size_t n1, 
                                   const size_t n2);

_gsl_matrix_complex_float_const_view
gsl_matrix_complex_float_const_view_array_with_tda (const float * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

_gsl_matrix_complex_float_const_view
gsl_matrix_complex_float_const_view_vector (const gsl_vector_complex_float * v,
                                    const size_t n1, 
                                    const size_t n2);

_gsl_matrix_complex_float_const_view
gsl_matrix_complex_float_const_view_vector_with_tda (const gsl_vector_complex_float * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

gsl_complex_float gsl_matrix_complex_float_get(const gsl_matrix_complex_float * m, const size_t i, const size_t j);
void gsl_matrix_complex_float_set(gsl_matrix_complex_float * m, const size_t i, const size_t j, const gsl_complex_float x);

gsl_complex_float * gsl_matrix_complex_float_ptr(gsl_matrix_complex_float * m, const size_t i, const size_t j);
const gsl_complex_float * gsl_matrix_complex_float_const_ptr(const gsl_matrix_complex_float * m, const size_t i, const size_t j);

void gsl_matrix_complex_float_set_zero (gsl_matrix_complex_float * m);
void gsl_matrix_complex_float_set_identity (gsl_matrix_complex_float * m);
void gsl_matrix_complex_float_set_all (gsl_matrix_complex_float * m, gsl_complex_float x);

int gsl_matrix_complex_float_fread (FILE * stream, gsl_matrix_complex_float * m) ;
int gsl_matrix_complex_float_fwrite (FILE * stream, const gsl_matrix_complex_float * m) ;
int gsl_matrix_complex_float_fscanf (FILE * stream, gsl_matrix_complex_float * m);
int gsl_matrix_complex_float_fprintf (FILE * stream, const gsl_matrix_complex_float * m, const char * format);

int gsl_matrix_complex_float_memcpy(gsl_matrix_complex_float * dest, const gsl_matrix_complex_float * src);
int gsl_matrix_complex_float_swap(gsl_matrix_complex_float * m1, gsl_matrix_complex_float * m2);

int gsl_matrix_complex_float_swap_rows(gsl_matrix_complex_float * m, const size_t i, const size_t j);
int gsl_matrix_complex_float_swap_columns(gsl_matrix_complex_float * m, const size_t i, const size_t j);
int gsl_matrix_complex_float_swap_rowcol(gsl_matrix_complex_float * m, const size_t i, const size_t j);

int gsl_matrix_complex_float_transpose (gsl_matrix_complex_float * m);
int gsl_matrix_complex_float_transpose_memcpy (gsl_matrix_complex_float * dest, const gsl_matrix_complex_float * src);

int gsl_matrix_complex_float_isnull (const gsl_matrix_complex_float * m);
int gsl_matrix_complex_float_ispos (const gsl_matrix_complex_float * m);
int gsl_matrix_complex_float_isneg (const gsl_matrix_complex_float * m);
int gsl_matrix_complex_float_isnonneg (const gsl_matrix_complex_float * m);

int gsl_matrix_complex_float_add (gsl_matrix_complex_float * a, const gsl_matrix_complex_float * b);
int gsl_matrix_complex_float_sub (gsl_matrix_complex_float * a, const gsl_matrix_complex_float * b);
int gsl_matrix_complex_float_mul_elements (gsl_matrix_complex_float * a, const gsl_matrix_complex_float * b);
int gsl_matrix_complex_float_div_elements (gsl_matrix_complex_float * a, const gsl_matrix_complex_float * b);
int gsl_matrix_complex_float_scale (gsl_matrix_complex_float * a, const gsl_complex_float x);
int gsl_matrix_complex_float_add_constant (gsl_matrix_complex_float * a, const gsl_complex_float x);
int gsl_matrix_complex_float_add_diagonal (gsl_matrix_complex_float * a, const gsl_complex_float x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
int gsl_matrix_complex_float_get_row(gsl_vector_complex_float * v, const gsl_matrix_complex_float * m, const size_t i);
int gsl_matrix_complex_float_get_col(gsl_vector_complex_float * v, const gsl_matrix_complex_float * m, const size_t j);
int gsl_matrix_complex_float_set_row(gsl_matrix_complex_float * m, const size_t i, const gsl_vector_complex_float * v);
int gsl_matrix_complex_float_set_col(gsl_matrix_complex_float * m, const size_t j, const gsl_vector_complex_float * v);

#ifdef HAVE_INLINE

extern inline 
gsl_complex_float
gsl_matrix_complex_float_get(const gsl_matrix_complex_float * m, 
                     const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  gsl_complex_float zero = {{0,0}};

  if (i >= m->size1)
    {
      GSL_ERROR_VAL("first index out of range", GSL_EINVAL, zero) ;
    }
  else if (j >= m->size2)
    {
      GSL_ERROR_VAL("second index out of range", GSL_EINVAL, zero) ;
    }
#endif
  return *(gsl_complex_float *)(m->data + 2*(i * m->tda + j)) ;
} 

extern inline 
void
gsl_matrix_complex_float_set(gsl_matrix_complex_float * m, 
                     const size_t i, const size_t j, const gsl_complex_float x)
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
  *(gsl_complex_float *)(m->data + 2*(i * m->tda + j)) = x ;
}

extern inline 
gsl_complex_float *
gsl_matrix_complex_float_ptr(gsl_matrix_complex_float * m, 
                             const size_t i, const size_t j)
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
  return (gsl_complex_float *)(m->data + 2*(i * m->tda + j)) ;
} 

extern inline 
const gsl_complex_float *
gsl_matrix_complex_float_const_ptr(const gsl_matrix_complex_float * m, 
                                   const size_t i, const size_t j)
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
  return (const gsl_complex_float *)(m->data + 2*(i * m->tda + j)) ;
} 

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_MATRIX_COMPLEX_FLOAT_H__ */
