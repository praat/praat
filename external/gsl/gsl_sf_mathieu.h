/* specfunc/gsl_sf_mathieu.h
 * 
 * Copyright (C) 2002 Lowell Johnson
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Author:  L. Johnson */

#ifndef __GSL_SF_MATHIEU_H__
#define __GSL_SF_MATHIEU_H__

#include "gsl_sf_result.h"
#include "gsl_eigen.h"

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

#define GSL_SF_MATHIEU_COEFF 100

typedef struct 
{
  size_t size;
  size_t even_order;
  size_t odd_order;
  int extra_values;
  double qa;   /* allow for caching of results: not implemented yet */
  double qb;   /* allow for caching of results: not implemented yet */
  double *aa;
  double *bb;
  double *dd;
  double *ee;
  double *tt;
  double *e2;
  double *zz;
  gsl_vector *eval;
  gsl_matrix *evec;
  gsl_eigen_symmv_workspace *wmat;
} gsl_sf_mathieu_workspace;


/* Compute an array of characteristic (eigen) values from the recurrence
   matrices for the Mathieu equations. */
int gsl_sf_mathieu_a_array(int order_min, int order_max, double qq, gsl_sf_mathieu_workspace *work, double result_array[]);
int gsl_sf_mathieu_b_array(int order_min, int order_max, double qq,  gsl_sf_mathieu_workspace *work, double result_array[]);

/* Compute the characteristic value for a Mathieu function of order n and
   type ntype. */
int gsl_sf_mathieu_a(int order, double qq, gsl_sf_result *result);
int gsl_sf_mathieu_b(int order, double qq, gsl_sf_result *result);

/* Compute the Fourier coefficients for a Mathieu function. */
int gsl_sf_mathieu_a_coeff(int order, double qq, double aa, double coeff[]);
int gsl_sf_mathieu_b_coeff(int order, double qq, double aa, double coeff[]);

/* Allocate computational storage space for eigenvalue solution. */
gsl_sf_mathieu_workspace *gsl_sf_mathieu_alloc(const size_t nn,
                                               const double qq);
void gsl_sf_mathieu_free(gsl_sf_mathieu_workspace *workspace);

/* Compute an angular Mathieu function. */
int gsl_sf_mathieu_ce(int order, double qq, double zz, gsl_sf_result *result);
int gsl_sf_mathieu_se(int order, double qq, double zz, gsl_sf_result *result);
int gsl_sf_mathieu_ce_array(int nmin, int nmax, double qq, double zz,
                            gsl_sf_mathieu_workspace *work,
                            double result_array[]);
int gsl_sf_mathieu_se_array(int nmin, int nmax, double qq, double zz,
                            gsl_sf_mathieu_workspace *work,
                            double result_array[]);

/* Compute a radial Mathieu function. */
int gsl_sf_mathieu_Mc(int kind, int order, double qq, double zz,
                      gsl_sf_result *result);
int gsl_sf_mathieu_Ms(int kind, int order, double qq, double zz,
                      gsl_sf_result *result);
int gsl_sf_mathieu_Mc_array(int kind, int nmin, int nmax, double qq,
                            double zz, gsl_sf_mathieu_workspace *work,
                            double result_array[]);
int gsl_sf_mathieu_Ms_array(int kind, int nmin, int nmax, double qq,
                            double zz, gsl_sf_mathieu_workspace *work,
                            double result_array[]);


__END_DECLS

#endif /* !__GSL_SF_MATHIEU_H__ */
