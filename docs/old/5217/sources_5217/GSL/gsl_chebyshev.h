/* cheb/gsl_chebyshev.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

#ifndef __GSL_CHEBYSHEV_H__
#define __GSL_CHEBYSHEV_H__

#include "gsl_math.h"
#include "gsl_mode.h"

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


/* data for a Chebyshev series over a given interval */

struct gsl_cheb_series_struct {

  double * c;   /* coefficients                */
  size_t order; /* order of expansion          */
  double a;     /* lower interval point        */
  double b;     /* upper interval point        */

  /* The following exists (mostly) for the benefit
   * of the implementation. It is an effective single
   * precision order, for use in single precision
   * evaluation. Users can use it if they like, but
   * only they know how to calculate it, since it is
   * specific to the approximated function. By default,
   * order_sp = order.
   * It is used explicitly only by the gsl_cheb_eval_mode
   * functions, which are not meant for casual use.
   */
  size_t order_sp;

  /* Additional elements not used by specfunc */

  double * f;   /* function evaluated at chebyschev points  */
};
typedef struct gsl_cheb_series_struct gsl_cheb_series;


/* Calculate a Chebyshev series of specified order over
 * a specified interval, for a given function.
 * Return 0 on failure.
 */
gsl_cheb_series * gsl_cheb_alloc(const size_t order);

/* Free a Chebyshev series previously calculated with gsl_cheb_alloc().
 */
void gsl_cheb_free(gsl_cheb_series * cs);

/* Calculate a Chebyshev series using the storage provided.
 * Uses the interval (a,b) and the order with which it
 * was initially created.
 *
 */
int gsl_cheb_init(gsl_cheb_series * cs, const gsl_function * func,
                  const double a, const double b);


/* Evaluate a Chebyshev series at a given point.
 * No errors can occur for a struct obtained from gsl_cheb_new().
 */
double gsl_cheb_eval(const gsl_cheb_series * cs, const double x);
int gsl_cheb_eval_err(const gsl_cheb_series * cs, const double x, 
                      double * result, double * abserr);


/* Evaluate a Chebyshev series at a given point, to (at most) the given order.
 * No errors can occur for a struct obtained from gsl_cheb_new().
 */
double gsl_cheb_eval_n(const gsl_cheb_series * cs, const size_t order, 
                       const double x);
int gsl_cheb_eval_n_err(const gsl_cheb_series * cs, const size_t order, 
                        const double x, double * result, double * abserr);


/* Evaluate a Chebyshev series at a given point, using the default
 * order for double precision mode(s) and the single precision
 * order for other modes.
 * No errors can occur for a struct obtained from gsl_cheb_new().
 */
double gsl_cheb_eval_mode(const gsl_cheb_series * cs, const double x, gsl_mode_t mode);
int gsl_cheb_eval_mode_e(const gsl_cheb_series * cs, const double x, gsl_mode_t mode, double * result, double * abserr);



/* Compute the derivative of a Chebyshev series.
 */
int gsl_cheb_calc_deriv(gsl_cheb_series * deriv, const gsl_cheb_series * cs);

/* Compute the integral of a Chebyshev series. The
 * integral is fixed by the condition that it equals zero at
 * the left end-point, ie it is precisely
 *       Integrate[cs(t; a,b), {t, a, x}]
 */
int gsl_cheb_calc_integ(gsl_cheb_series * integ, const gsl_cheb_series * cs);




__END_DECLS

#endif /* __GSL_CHEBYSHEV_H__ */
