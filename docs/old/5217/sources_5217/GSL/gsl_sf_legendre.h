/* specfunc/gsl_sf_legendre.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2004 Gerard Jungman
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

#ifndef __GSL_SF_LEGENDRE_H__
#define __GSL_SF_LEGENDRE_H__

#include "gsl_sf_result.h"

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


/* P_l(x)   l >= 0; |x| <= 1
 *
 * exceptions: GSL_EDOM
 */
int     gsl_sf_legendre_Pl_e(const int l, const double x, gsl_sf_result * result);
double  gsl_sf_legendre_Pl(const int l, const double x);


/* P_l(x) for l=0,...,lmax; |x| <= 1
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_Pl_array(
  const int lmax, const double x,
  double * result_array
  );


/* P_l(x) and P_l'(x) for l=0,...,lmax; |x| <= 1
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_Pl_deriv_array(
  const int lmax, const double x,
  double * result_array,
  double * result_deriv_array
  );


/* P_l(x), l=1,2,3
 *
 * exceptions: none
 */
int gsl_sf_legendre_P1_e(double x, gsl_sf_result * result);
int gsl_sf_legendre_P2_e(double x, gsl_sf_result * result);
int gsl_sf_legendre_P3_e(double x, gsl_sf_result * result);
double gsl_sf_legendre_P1(const double x);
double gsl_sf_legendre_P2(const double x);
double gsl_sf_legendre_P3(const double x);


/* Q_0(x), x > -1, x != 1
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_Q0_e(const double x, gsl_sf_result * result);
double gsl_sf_legendre_Q0(const double x);


/* Q_1(x), x > -1, x != 1
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_Q1_e(const double x, gsl_sf_result * result);
double gsl_sf_legendre_Q1(const double x);


/* Q_l(x), x > -1, x != 1, l >= 0
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_Ql_e(const int l, const double x, gsl_sf_result * result);
double gsl_sf_legendre_Ql(const int l, const double x);


/* P_l^m(x)  m >= 0; l >= m; |x| <= 1.0
 *
 * Note that this function grows combinatorially with l.
 * Therefore we can easily generate an overflow for l larger
 * than about 150.
 *
 * There is no trouble for small m, but when m and l are both large,
 * then there will be trouble. Rather than allow overflows, these
 * functions refuse to calculate when they can sense that l and m are
 * too big.
 *
 * If you really want to calculate a spherical harmonic, then DO NOT
 * use this. Instead use legendre_sphPlm() below, which  uses a similar
 * recursion, but with the normalized functions.
 *
 * exceptions: GSL_EDOM, GSL_EOVRFLW
 */
int     gsl_sf_legendre_Plm_e(const int l, const int m, const double x, gsl_sf_result * result);
double  gsl_sf_legendre_Plm(const int l, const int m, const double x);


/* P_l^m(x)  m >= 0; l >= m; |x| <= 1.0
 * l=|m|,...,lmax
 *
 * exceptions: GSL_EDOM, GSL_EOVRFLW
 */
int gsl_sf_legendre_Plm_array(
  const int lmax, const int m, const double x,
  double * result_array
  );


/* P_l^m(x)  and d(P_l^m(x))/dx;  m >= 0; lmax >= m; |x| <= 1.0
 * l=|m|,...,lmax
 *
 * exceptions: GSL_EDOM, GSL_EOVRFLW
 */
int gsl_sf_legendre_Plm_deriv_array(
  const int lmax, const int m, const double x,
  double * result_array,
  double * result_deriv_array
  );


/* P_l^m(x), normalized properly for use in spherical harmonics
 * m >= 0; l >= m; |x| <= 1.0
 *
 * There is no overflow problem, as there is for the
 * standard normalization of P_l^m(x).
 *
 * Specifically, it returns:
 *
 *        sqrt((2l+1)/(4pi)) sqrt((l-m)!/(l+m)!) P_l^m(x)
 *
 * exceptions: GSL_EDOM
 */
int     gsl_sf_legendre_sphPlm_e(const int l, int m, const double x, gsl_sf_result * result);
double  gsl_sf_legendre_sphPlm(const int l, const int m, const double x);


/* sphPlm(l,m,x) values
 * m >= 0; l >= m; |x| <= 1.0
 * l=|m|,...,lmax
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_sphPlm_array(
  const int lmax, int m, const double x,
  double * result_array
  );


/* sphPlm(l,m,x) and d(sphPlm(l,m,x))/dx values
 * m >= 0; l >= m; |x| <= 1.0
 * l=|m|,...,lmax
 *
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_sphPlm_deriv_array(
  const int lmax, const int m, const double x,
  double * result_array,
  double * result_deriv_array
  );



/* size of result_array[] needed for the array versions of Plm
 * (lmax - m + 1)
 */
int gsl_sf_legendre_array_size(const int lmax, const int m);


/* Irregular Spherical Conical Function
 * P^{1/2}_{-1/2 + I lambda}(x)
 *
 * x > -1.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_half_e(const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_half(const double lambda, const double x);


/* Regular Spherical Conical Function
 * P^{-1/2}_{-1/2 + I lambda}(x)
 *
 * x > -1.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_mhalf_e(const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_mhalf(const double lambda, const double x);


/* Conical Function
 * P^{0}_{-1/2 + I lambda}(x)
 *
 * x > -1.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_0_e(const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_0(const double lambda, const double x);


/* Conical Function
 * P^{1}_{-1/2 + I lambda}(x)
 *
 * x > -1.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_1_e(const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_1(const double lambda, const double x);


/* Regular Spherical Conical Function
 * P^{-1/2-l}_{-1/2 + I lambda}(x)
 *
 * x > -1.0, l >= -1
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_sph_reg_e(const int l, const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_sph_reg(const int l, const double lambda, const double x);


/* Regular Cylindrical Conical Function
 * P^{-m}_{-1/2 + I lambda}(x)
 *
 * x > -1.0, m >= -1
 * exceptions: GSL_EDOM
 */
int gsl_sf_conicalP_cyl_reg_e(const int m, const double lambda, const double x, gsl_sf_result * result);
double gsl_sf_conicalP_cyl_reg(const int m, const double lambda, const double x);


/* The following spherical functions are specializations
 * of Legendre functions which give the regular eigenfunctions
 * of the Laplacian on a 3-dimensional hyperbolic space.
 * Of particular interest is the flat limit, which is
 * Flat-Lim := {lambda->Inf, eta->0, lambda*eta fixed}.
 */
  
/* Zeroth radial eigenfunction of the Laplacian on the
 * 3-dimensional hyperbolic space.
 *
 * legendre_H3d_0(lambda,eta) := sin(lambda*eta)/(lambda*sinh(eta))
 * 
 * Normalization:
 * Flat-Lim legendre_H3d_0(lambda,eta) = j_0(lambda*eta)
 *
 * eta >= 0.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_H3d_0_e(const double lambda, const double eta, gsl_sf_result * result);
double gsl_sf_legendre_H3d_0(const double lambda, const double eta);


/* First radial eigenfunction of the Laplacian on the
 * 3-dimensional hyperbolic space.
 *
 * legendre_H3d_1(lambda,eta) :=
 *    1/sqrt(lambda^2 + 1) sin(lam eta)/(lam sinh(eta))
 *    (coth(eta) - lambda cot(lambda*eta))
 * 
 * Normalization:
 * Flat-Lim legendre_H3d_1(lambda,eta) = j_1(lambda*eta)
 *
 * eta >= 0.0
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_H3d_1_e(const double lambda, const double eta, gsl_sf_result * result);
double gsl_sf_legendre_H3d_1(const double lambda, const double eta);


/* l'th radial eigenfunction of the Laplacian on the
 * 3-dimensional hyperbolic space.
 *
 * Normalization:
 * Flat-Lim legendre_H3d_l(l,lambda,eta) = j_l(lambda*eta)
 *
 * eta >= 0.0, l >= 0
 * exceptions: GSL_EDOM
 */
int gsl_sf_legendre_H3d_e(const int l, const double lambda, const double eta, gsl_sf_result * result);
double gsl_sf_legendre_H3d(const int l, const double lambda, const double eta);


/* Array of H3d(ell),  0 <= ell <= lmax
 */
int gsl_sf_legendre_H3d_array(const int lmax, const double lambda, const double eta, double * result_array);


#ifdef HAVE_INLINE
extern inline
int
gsl_sf_legendre_array_size(const int lmax, const int m)
{
  return lmax-m+1;
}
#endif /* HAVE_INLINE */


__END_DECLS

#endif /* __GSL_SF_LEGENDRE_H__ */
