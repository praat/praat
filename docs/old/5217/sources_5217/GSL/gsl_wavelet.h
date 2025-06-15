/* wavelet/gsl_wavelet.h
 * 
 * Copyright (C) 2004 Ivo Alxneit
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

#ifndef __GSL_WAVELET_H__
#define __GSL_WAVELET_H__
#include <stdlib.h>
#include "gsl_types.h"
#include "gsl_errno.h"

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS          /* empty */
# define __END_DECLS            /* empty */
#endif

__BEGIN_DECLS

#ifndef GSL_DISABLE_DEPRECATED
typedef enum {
  forward = 1, backward = -1,
  gsl_wavelet_forward = 1, gsl_wavelet_backward = -1
} 
gsl_wavelet_direction;
#else
typedef enum {
  gsl_wavelet_forward = 1, gsl_wavelet_backward = -1
} 
gsl_wavelet_direction;
#endif

typedef struct
{
  const char *name;
  int (*init) (const double **h1, const double **g1,
               const double **h2, const double **g2, size_t * nc,
               size_t * offset, size_t member);
}
gsl_wavelet_type;

typedef struct
{
  const gsl_wavelet_type *type;
  const double *h1;
  const double *g1;
  const double *h2;
  const double *g2;
  size_t nc;
  size_t offset;
}
gsl_wavelet;

typedef struct
{
  double *scratch;
  size_t n;
}
gsl_wavelet_workspace;

GSL_VAR const gsl_wavelet_type *gsl_wavelet_daubechies;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_daubechies_centered;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_haar;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_haar_centered;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_bspline;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_bspline_centered;

gsl_wavelet *gsl_wavelet_alloc (const gsl_wavelet_type * T, size_t k);
void gsl_wavelet_free (gsl_wavelet * w);
const char *gsl_wavelet_name (const gsl_wavelet * w);

gsl_wavelet_workspace *gsl_wavelet_workspace_alloc (size_t n);
void gsl_wavelet_workspace_free (gsl_wavelet_workspace * work);

int gsl_wavelet_transform (const gsl_wavelet * w, 
                           double *data, size_t stride, size_t n,
                           gsl_wavelet_direction dir, 
                           gsl_wavelet_workspace * work);

int gsl_wavelet_transform_forward (const gsl_wavelet * w, 
                                   double *data, size_t stride, size_t n, 
                                   gsl_wavelet_workspace * work);

int gsl_wavelet_transform_inverse (const gsl_wavelet * w, 
                                    double *data, size_t stride, size_t n, 
                                    gsl_wavelet_workspace * work);

__END_DECLS

#endif /* __GSL_WAVELET_H__ */
