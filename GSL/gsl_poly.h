/* poly/gsl_poly.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Brian Gough
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

#ifndef __GSL_POLY_H__
#define __GSL_POLY_H__

#include <stdlib.h>
#include "gsl_complex.h"

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


/* Evaluate polynomial
 *
 * c[0] + c[1] x + c[2] x^2 + ... + c[len-1] x^(len-1)
 *
 * exceptions: none
 */
double gsl_poly_eval(const double c[], const int len, const double x);


#ifdef HAVE_INLINE
extern inline
double 
gsl_poly_eval(const double c[], const int len, const double x)
{
  int i;
  double ans = c[len-1];
  for(i=len-1; i>0; i--) ans = c[i-1] + x * ans;
  return ans;
}
#endif /* HAVE_INLINE */

/* Work with divided-difference polynomials, Abramowitz & Stegun 25.2.26 */

int
gsl_poly_dd_init (double dd[], const double x[], const double y[],
                  size_t size);

double
gsl_poly_dd_eval (const double dd[], const double xa[], const size_t size, const double x);

#ifdef HAVE_INLINE
extern inline
double 
gsl_poly_dd_eval(const double dd[], const double xa[], const size_t size, const double x)
{
  size_t i;
  double y = dd[size - 1];
  for (i = size - 1; i--;) y = dd[i] + (x - xa[i]) * y;
  return y;
}
#endif /* HAVE_INLINE */


int
gsl_poly_dd_taylor (double c[], double xp,
                    const double dd[], const double x[], size_t size,
                    double w[]);

/* Solve for real or complex roots of the standard quadratic equation,
 * returning the number of real roots.
 *
 * Roots are returned ordered.
 */
int gsl_poly_solve_quadratic (double a, double b, double c, 
                              double * x0, double * x1);

int 
gsl_poly_complex_solve_quadratic (double a, double b, double c, 
                                  gsl_complex * z0, gsl_complex * z1);


/* Solve for real roots of the cubic equation
 * x^3 + a x^2 + b x + c = 0, returning the
 * number of real roots.
 *
 * Roots are returned ordered.
 */
int gsl_poly_solve_cubic (double a, double b, double c, 
                          double * x0, double * x1, double * x2);

int 
gsl_poly_complex_solve_cubic (double a, double b, double c, 
                              gsl_complex * z0, gsl_complex * z1, 
                              gsl_complex * z2);


/* Solve for the complex roots of a general real polynomial */

typedef struct 
{ 
  size_t nc ;
  double * matrix ; 
} 
gsl_poly_complex_workspace ;

gsl_poly_complex_workspace * gsl_poly_complex_workspace_alloc (size_t n);
void gsl_poly_complex_workspace_free (gsl_poly_complex_workspace * w);

int
gsl_poly_complex_solve (const double * a, size_t n, 
                        gsl_poly_complex_workspace * w,
                        gsl_complex_packed_ptr z);

__END_DECLS

#endif /* __GSL_POLY_H__ */
