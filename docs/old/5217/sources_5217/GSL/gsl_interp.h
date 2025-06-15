/* interpolation/gsl_interp.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004 Gerard Jungman
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

/* Author:  G. Jungman
 */
#ifndef __GSL_INTERP_H__
#define __GSL_INTERP_H__
#include <stdlib.h>
#include "gsl_types.h"

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

/* evaluation accelerator */
typedef struct {
  size_t  cache;        /* cache of index   */
  size_t  miss_count;   /* keep statistics  */
  size_t  hit_count;
}
gsl_interp_accel;


/* interpolation object type */
typedef struct {
  const char * name;
  unsigned int min_size;
  void *  (*alloc) (size_t size);
  int     (*init)    (void *, const double xa[], const double ya[], size_t size);
  int     (*eval)    (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y);
  int     (*eval_deriv)  (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y_p);
  int     (*eval_deriv2) (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y_pp);
  int     (*eval_integ)  (const void *, const double xa[], const double ya[], size_t size, gsl_interp_accel *, double a, double b, double * result);
  void    (*free)         (void *);

} gsl_interp_type;


/* general interpolation object */
typedef struct {
  const gsl_interp_type * type;
  double  xmin;
  double  xmax;
  size_t  size;
  void * state;
} gsl_interp;


/* available types */
GSL_VAR const gsl_interp_type * gsl_interp_linear;
GSL_VAR const gsl_interp_type * gsl_interp_polynomial;
GSL_VAR const gsl_interp_type * gsl_interp_cspline;
GSL_VAR const gsl_interp_type * gsl_interp_cspline_periodic;
GSL_VAR const gsl_interp_type * gsl_interp_akima;
GSL_VAR const gsl_interp_type * gsl_interp_akima_periodic;

gsl_interp_accel *
gsl_interp_accel_alloc(void);

size_t
gsl_interp_accel_find(gsl_interp_accel * a, const double x_array[], size_t size, double x);

int
gsl_interp_accel_reset (gsl_interp_accel * a);

void
gsl_interp_accel_free(gsl_interp_accel * a);

gsl_interp *
gsl_interp_alloc(const gsl_interp_type * T, size_t n);
     
int
gsl_interp_init(gsl_interp * obj, const double xa[], const double ya[], size_t size);

const char * gsl_interp_name(const gsl_interp * interp);
unsigned int gsl_interp_min_size(const gsl_interp * interp);


int
gsl_interp_eval_e(const gsl_interp * obj,
                  const double xa[], const double ya[], double x,
                  gsl_interp_accel * a, double * y);

double
gsl_interp_eval(const gsl_interp * obj,
                const double xa[], const double ya[], double x,
                gsl_interp_accel * a);

int
gsl_interp_eval_deriv_e(const gsl_interp * obj,
                        const double xa[], const double ya[], double x,
                        gsl_interp_accel * a,
                        double * d);

double
gsl_interp_eval_deriv(const gsl_interp * obj,
                      const double xa[], const double ya[], double x,
                      gsl_interp_accel * a);

int
gsl_interp_eval_deriv2_e(const gsl_interp * obj,
                         const double xa[], const double ya[], double x,
                         gsl_interp_accel * a,
                         double * d2);

double
gsl_interp_eval_deriv2(const gsl_interp * obj,
                       const double xa[], const double ya[], double x,
                       gsl_interp_accel * a);

int
gsl_interp_eval_integ_e(const gsl_interp * obj,
                        const double xa[], const double ya[],
                        double a, double b,
                        gsl_interp_accel * acc,
                        double * result);

double
gsl_interp_eval_integ(const gsl_interp * obj,
                      const double xa[], const double ya[],
                      double a, double b,
                      gsl_interp_accel * acc);

void
gsl_interp_free(gsl_interp * interp);

size_t gsl_interp_bsearch(const double x_array[], double x,
                          size_t index_lo, size_t index_hi);

#ifdef HAVE_INLINE
extern inline size_t
gsl_interp_bsearch(const double x_array[], double x,
                   size_t index_lo, size_t index_hi);

extern inline size_t
gsl_interp_bsearch(const double x_array[], double x,
                   size_t index_lo, size_t index_hi)
{
  size_t ilo = index_lo;
  size_t ihi = index_hi;
  while(ihi > ilo + 1) {
    size_t i = (ihi + ilo)/2;
    if(x_array[i] > x)
      ihi = i;
    else
      ilo = i;
  }
  
  return ilo;
}
#endif

#ifdef HAVE_INLINE
extern inline size_t
gsl_interp_accel_find(gsl_interp_accel * a, const double xa[], size_t len, double x)
{
  size_t x_index = a->cache;
 
  if(x < xa[x_index]) {
    a->miss_count++;
    a->cache = gsl_interp_bsearch(xa, x, 0, x_index);
  }
  else if(x > xa[x_index + 1]) {
    a->miss_count++;
    a->cache = gsl_interp_bsearch(xa, x, x_index, len-1);
  }
  else {
    a->hit_count++;
  }
  
  return a->cache;
}
#endif /* HAVE_INLINE */


__END_DECLS

#endif /* __GSL_INTERP_H__ */
