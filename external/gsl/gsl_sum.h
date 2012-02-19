/* sum/gsl_sum.h
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

/* Author:  G. Jungman */


#ifndef __GSL_SUM_H__
#define __GSL_SUM_H__

#include <stdlib.h>

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

/*  Workspace for Levin U Transform with error estimation,
 *   
 *   size        = number of terms the workspace can handle
 *   sum_plain   = simple sum of series
 *   q_num       = backward diagonal of numerator; length = size
 *   q_den       = backward diagonal of denominator; length = size
 *   dq_num      = table of numerator derivatives; length = size**2
 *   dq_den      = table of denominator derivatives; length = size**2
 *   dsum        = derivative of sum wrt term i; length = size
 */

typedef struct
{
  size_t size;
  size_t i;                     /* position in array */
  size_t terms_used;            /* number of calls */
  double sum_plain;
  double *q_num;
  double *q_den;
  double *dq_num;
  double *dq_den;
  double *dsum;
}
gsl_sum_levin_u_workspace;

gsl_sum_levin_u_workspace *gsl_sum_levin_u_alloc (size_t n);
void gsl_sum_levin_u_free (gsl_sum_levin_u_workspace * w);

/* Basic Levin-u acceleration method.
 *
 *   array       = array of series elements
 *   n           = size of array
 *   sum_accel   = result of summation acceleration
 *   err         = estimated error   
 *
 * See [Fessler et al., ACM TOMS 9, 346 (1983) and TOMS-602]
 */

int gsl_sum_levin_u_accel (const double *array,
                           const size_t n,
                           gsl_sum_levin_u_workspace * w,
                           double *sum_accel, double *abserr);

/* Basic Levin-u acceleration method with constraints on the terms
 * used,
 *
 *   array       = array of series elements
 *   n           = size of array
 *   min_terms   = minimum number of terms to sum
 *   max_terms   = maximum number of terms to sum
 *   sum_accel   = result of summation acceleration
 *   err         = estimated error   
 *
 * See [Fessler et al., ACM TOMS 9, 346 (1983) and TOMS-602] 
 */

int gsl_sum_levin_u_minmax (const double *array,
                            const size_t n,
                            const size_t min_terms,
                            const size_t max_terms,
                            gsl_sum_levin_u_workspace * w,
                            double *sum_accel, double *abserr);

/* Basic Levin-u step w/o reference to the array of terms.
 * We only need to specify the value of the current term
 * to execute the step. See TOMS-745.
 *
 * sum = t0 + ... + t_{n-1} + term;  term = t_{n}
 *
 *   term   = value of the series term to be added
 *   n      = position of term in series (starting from 0)
 *   sum_accel = result of summation acceleration
 *   sum_plain = simple sum of series
 */

int
gsl_sum_levin_u_step (const double term,
                      const size_t n,
                      const size_t nmax,
                      gsl_sum_levin_u_workspace * w, 
                      double *sum_accel);

/* The following functions perform the same calculation without
   estimating the errors. They require O(N) storage instead of O(N^2).
   This may be useful for summing many similar series where the size
   of the error has already been estimated reliably and is not
   expected to change.  */

typedef struct
{
  size_t size;
  size_t i;                     /* position in array */
  size_t terms_used;            /* number of calls */
  double sum_plain;
  double *q_num;
  double *q_den;
  double *dsum;
}
gsl_sum_levin_utrunc_workspace;

gsl_sum_levin_utrunc_workspace *gsl_sum_levin_utrunc_alloc (size_t n);
void gsl_sum_levin_utrunc_free (gsl_sum_levin_utrunc_workspace * w);

int gsl_sum_levin_utrunc_accel (const double *array,
                                const size_t n,
                                gsl_sum_levin_utrunc_workspace * w,
                                double *sum_accel, double *abserr_trunc);

int gsl_sum_levin_utrunc_minmax (const double *array,
                                 const size_t n,
                                 const size_t min_terms,
                                 const size_t max_terms,
                                 gsl_sum_levin_utrunc_workspace * w,
                                 double *sum_accel, double *abserr_trunc);

int gsl_sum_levin_utrunc_step (const double term,
                               const size_t n,
                               gsl_sum_levin_utrunc_workspace * w, 
                               double *sum_accel);

__END_DECLS

#endif /* __GSL_SUM_H__ */
