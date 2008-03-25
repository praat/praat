/* block/gsl_block_complex_double.h
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

#ifndef __GSL_BLOCK_COMPLEX_DOUBLE_H__
#define __GSL_BLOCK_COMPLEX_DOUBLE_H__

#include <stdlib.h>
#include "gsl_errno.h"

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

struct gsl_block_complex_struct
{
  size_t size;
  double *data;
};

typedef struct gsl_block_complex_struct gsl_block_complex;

gsl_block_complex *gsl_block_complex_alloc (const size_t n);
gsl_block_complex *gsl_block_complex_calloc (const size_t n);
void gsl_block_complex_free (gsl_block_complex * b);

int gsl_block_complex_fread (FILE * stream, gsl_block_complex * b);
int gsl_block_complex_fwrite (FILE * stream, const gsl_block_complex * b);
int gsl_block_complex_fscanf (FILE * stream, gsl_block_complex * b);
int gsl_block_complex_fprintf (FILE * stream, const gsl_block_complex * b, const char *format);

int gsl_block_complex_raw_fread (FILE * stream, double * b, const size_t n, const size_t stride);
int gsl_block_complex_raw_fwrite (FILE * stream, const double * b, const size_t n, const size_t stride);
int gsl_block_complex_raw_fscanf (FILE * stream, double * b, const size_t n, const size_t stride);
int gsl_block_complex_raw_fprintf (FILE * stream, const double * b, const size_t n, const size_t stride, const char *format);

size_t gsl_block_complex_size (const gsl_block_complex * b);
double * gsl_block_complex_data (const gsl_block_complex * b);

__END_DECLS

#endif /* __GSL_BLOCK_COMPLEX_DOUBLE_H__ */
