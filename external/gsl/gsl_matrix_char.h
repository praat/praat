/* matrix/gsl_matrix_char.h
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

#ifndef __GSL_MATRIX_CHAR_H__
#define __GSL_MATRIX_CHAR_H__

#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"
#include "gsl_check_range.h"
#include "gsl_vector_char.h"

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
  char * data;
  gsl_block_char * block;
  int owner;
} gsl_matrix_char;

typedef struct
{
  gsl_matrix_char matrix;
} _gsl_matrix_char_view;

typedef _gsl_matrix_char_view gsl_matrix_char_view;

typedef struct
{
  gsl_matrix_char matrix;
} _gsl_matrix_char_const_view;

typedef const _gsl_matrix_char_const_view gsl_matrix_char_const_view;

/* Allocation */

gsl_matrix_char * 
gsl_matrix_char_alloc (const size_t n1, const size_t n2);

gsl_matrix_char * 
gsl_matrix_char_calloc (const size_t n1, const size_t n2);

gsl_matrix_char * 
gsl_matrix_char_alloc_from_block (gsl_block_char * b, 
                                   const size_t offset, 
                                   const size_t n1, 
                                   const size_t n2, 
                                   const size_t d2);

gsl_matrix_char * 
gsl_matrix_char_alloc_from_matrix (gsl_matrix_char * m,
                                    const size_t k1, 
                                    const size_t k2,
                                    const size_t n1, 
                                    const size_t n2);

gsl_vector_char * 
gsl_vector_char_alloc_row_from_matrix (gsl_matrix_char * m,
                                        const size_t i);

gsl_vector_char * 
gsl_vector_char_alloc_col_from_matrix (gsl_matrix_char * m,
                                        const size_t j);

void gsl_matrix_char_free (gsl_matrix_char * m);

/* Views */

_gsl_matrix_char_view 
gsl_matrix_char_submatrix (gsl_matrix_char * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

_gsl_vector_char_view 
gsl_matrix_char_row (gsl_matrix_char * m, const size_t i);

_gsl_vector_char_view 
gsl_matrix_char_column (gsl_matrix_char * m, const size_t j);

_gsl_vector_char_view 
gsl_matrix_char_diagonal (gsl_matrix_char * m);

_gsl_vector_char_view 
gsl_matrix_char_subdiagonal (gsl_matrix_char * m, const size_t k);

_gsl_vector_char_view 
gsl_matrix_char_superdiagonal (gsl_matrix_char * m, const size_t k);

_gsl_vector_char_view
gsl_matrix_char_subrow (gsl_matrix_char * m, const size_t i,
                         const size_t offset, const size_t n);

_gsl_vector_char_view
gsl_matrix_char_subcolumn (gsl_matrix_char * m, const size_t j,
                            const size_t offset, const size_t n);

_gsl_matrix_char_view
gsl_matrix_char_view_array (char * base,
                             const size_t n1, 
                             const size_t n2);

_gsl_matrix_char_view
gsl_matrix_char_view_array_with_tda (char * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);


_gsl_matrix_char_view
gsl_matrix_char_view_vector (gsl_vector_char * v,
                              const size_t n1, 
                              const size_t n2);

_gsl_matrix_char_view
gsl_matrix_char_view_vector_with_tda (gsl_vector_char * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


_gsl_matrix_char_const_view 
gsl_matrix_char_const_submatrix (const gsl_matrix_char * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

_gsl_vector_char_const_view 
gsl_matrix_char_const_row (const gsl_matrix_char * m, 
                            const size_t i);

_gsl_vector_char_const_view 
gsl_matrix_char_const_column (const gsl_matrix_char * m, 
                               const size_t j);

_gsl_vector_char_const_view
gsl_matrix_char_const_diagonal (const gsl_matrix_char * m);

_gsl_vector_char_const_view 
gsl_matrix_char_const_subdiagonal (const gsl_matrix_char * m, 
                                    const size_t k);

_gsl_vector_char_const_view 
gsl_matrix_char_const_superdiagonal (const gsl_matrix_char * m, 
                                      const size_t k);

_gsl_vector_char_const_view
gsl_matrix_char_const_subrow (const gsl_matrix_char * m, const size_t i,
                               const size_t offset, const size_t n);

_gsl_vector_char_const_view
gsl_matrix_char_const_subcolumn (const gsl_matrix_char * m, const size_t j,
                                  const size_t offset, const size_t n);

_gsl_matrix_char_const_view
gsl_matrix_char_const_view_array (const char * base,
                                   const size_t n1, 
                                   const size_t n2);

_gsl_matrix_char_const_view
gsl_matrix_char_const_view_array_with_tda (const char * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

_gsl_matrix_char_const_view
gsl_matrix_char_const_view_vector (const gsl_vector_char * v,
                                    const size_t n1, 
                                    const size_t n2);

_gsl_matrix_char_const_view
gsl_matrix_char_const_view_vector_with_tda (const gsl_vector_char * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

char   gsl_matrix_char_get(const gsl_matrix_char * m, const size_t i, const size_t j);
void    gsl_matrix_char_set(gsl_matrix_char * m, const size_t i, const size_t j, const char x);

char * gsl_matrix_char_ptr(gsl_matrix_char * m, const size_t i, const size_t j);
const char * gsl_matrix_char_const_ptr(const gsl_matrix_char * m, const size_t i, const size_t j);

void gsl_matrix_char_set_zero (gsl_matrix_char * m);
void gsl_matrix_char_set_identity (gsl_matrix_char * m);
void gsl_matrix_char_set_all (gsl_matrix_char * m, char x);

int gsl_matrix_char_fread (FILE * stream, gsl_matrix_char * m) ;
int gsl_matrix_char_fwrite (FILE * stream, const gsl_matrix_char * m) ;
int gsl_matrix_char_fscanf (FILE * stream, gsl_matrix_char * m);
int gsl_matrix_char_fprintf (FILE * stream, const gsl_matrix_char * m, const char * format);
 
int gsl_matrix_char_memcpy(gsl_matrix_char * dest, const gsl_matrix_char * src);
int gsl_matrix_char_swap(gsl_matrix_char * m1, gsl_matrix_char * m2);

int gsl_matrix_char_swap_rows(gsl_matrix_char * m, const size_t i, const size_t j);
int gsl_matrix_char_swap_columns(gsl_matrix_char * m, const size_t i, const size_t j);
int gsl_matrix_char_swap_rowcol(gsl_matrix_char * m, const size_t i, const size_t j);
int gsl_matrix_char_transpose (gsl_matrix_char * m);
int gsl_matrix_char_transpose_memcpy (gsl_matrix_char * dest, const gsl_matrix_char * src);

char gsl_matrix_char_max (const gsl_matrix_char * m);
char gsl_matrix_char_min (const gsl_matrix_char * m);
void gsl_matrix_char_minmax (const gsl_matrix_char * m, char * min_out, char * max_out);

void gsl_matrix_char_max_index (const gsl_matrix_char * m, size_t * imax, size_t *jmax);
void gsl_matrix_char_min_index (const gsl_matrix_char * m, size_t * imin, size_t *jmin);
void gsl_matrix_char_minmax_index (const gsl_matrix_char * m, size_t * imin, size_t * jmin, size_t * imax, size_t * jmax);

int gsl_matrix_char_isnull (const gsl_matrix_char * m);
int gsl_matrix_char_ispos (const gsl_matrix_char * m);
int gsl_matrix_char_isneg (const gsl_matrix_char * m);
int gsl_matrix_char_isnonneg (const gsl_matrix_char * m);

int gsl_matrix_char_add (gsl_matrix_char * a, const gsl_matrix_char * b);
int gsl_matrix_char_sub (gsl_matrix_char * a, const gsl_matrix_char * b);
int gsl_matrix_char_mul_elements (gsl_matrix_char * a, const gsl_matrix_char * b);
int gsl_matrix_char_div_elements (gsl_matrix_char * a, const gsl_matrix_char * b);
int gsl_matrix_char_scale (gsl_matrix_char * a, const double x);
int gsl_matrix_char_add_constant (gsl_matrix_char * a, const double x);
int gsl_matrix_char_add_diagonal (gsl_matrix_char * a, const double x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
int gsl_matrix_char_get_row(gsl_vector_char * v, const gsl_matrix_char * m, const size_t i);
int gsl_matrix_char_get_col(gsl_vector_char * v, const gsl_matrix_char * m, const size_t j);
int gsl_matrix_char_set_row(gsl_matrix_char * m, const size_t i, const gsl_vector_char * v);
int gsl_matrix_char_set_col(gsl_matrix_char * m, const size_t j, const gsl_vector_char * v);

/* inline functions if you are using GCC */

#ifdef HAVE_INLINE
extern inline 
char
gsl_matrix_char_get(const gsl_matrix_char * m, const size_t i, const size_t j)
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
gsl_matrix_char_set(gsl_matrix_char * m, const size_t i, const size_t j, const char x)
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
char *
gsl_matrix_char_ptr(gsl_matrix_char * m, const size_t i, const size_t j)
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
  return (char *) (m->data + (i * m->tda + j)) ;
} 

extern inline 
const char *
gsl_matrix_char_const_ptr(const gsl_matrix_char * m, const size_t i, const size_t j)
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
  return (const char *) (m->data + (i * m->tda + j)) ;
} 

#endif

__END_DECLS

#endif /* __GSL_MATRIX_CHAR_H__ */
