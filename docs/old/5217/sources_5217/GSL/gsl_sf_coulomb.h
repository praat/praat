/* specfunc/gsl_sf_coulomb.h
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

/* Author:  G. Jungman */

#ifndef __GSL_SF_COULOMB_H__
#define __GSL_SF_COULOMB_H__

#include "gsl_mode.h"
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


/* Normalized hydrogenic bound states, radial dependence. */

/* R_1 := 2Z sqrt(Z) exp(-Z r)
 */
int gsl_sf_hydrogenicR_1_e(const double Z, const double r, gsl_sf_result * result);
double gsl_sf_hydrogenicR_1(const double Z, const double r);

/* R_n := norm exp(-Z r/n) (2Z/n)^l Laguerre[n-l-1, 2l+1, 2Z/n r]
 *
 * normalization such that psi(n,l,r) = R_n Y_{lm}
 */
int gsl_sf_hydrogenicR_e(const int n, const int l, const double Z, const double r, gsl_sf_result * result);
double gsl_sf_hydrogenicR(const int n, const int l, const double Z, const double r);


/* Coulomb wave functions F_{lam_F}(eta,x), G_{lam_G}(eta,x)
 * and their derivatives; lam_G := lam_F - k_lam_G
 *
 * lam_F, lam_G > -0.5
 * x > 0.0
 *
 * Conventions of Abramowitz+Stegun.
 *
 * Because there can be a large dynamic range of values,
 * overflows are handled gracefully. If an overflow occurs,
 * GSL_EOVRFLW is signalled and exponent(s) are returned
 * through exp_F, exp_G. These are such that
 *
 *   F_L(eta,x)  =  fc[k_L] * exp(exp_F)
 *   G_L(eta,x)  =  gc[k_L] * exp(exp_G)
 *   F_L'(eta,x) = fcp[k_L] * exp(exp_F)
 *   G_L'(eta,x) = gcp[k_L] * exp(exp_G)
 */
int
gsl_sf_coulomb_wave_FG_e(const double eta, const double x,
                            const double lam_F,
                            const int  k_lam_G,
                            gsl_sf_result * F, gsl_sf_result * Fp,
                            gsl_sf_result * G, gsl_sf_result * Gp,
                            double * exp_F, double * exp_G);


/* F_L(eta,x) as array */
int gsl_sf_coulomb_wave_F_array(
  double lam_min, int kmax,
  double eta, double x,
  double * fc_array,
  double * F_exponent
  );

/* F_L(eta,x), G_L(eta,x) as arrays */
int gsl_sf_coulomb_wave_FG_array(double lam_min, int kmax,
                                double eta, double x,
                                double * fc_array, double * gc_array,
                                double * F_exponent,
                                double * G_exponent
                                );

/* F_L(eta,x), G_L(eta,x), F'_L(eta,x), G'_L(eta,x) as arrays */
int gsl_sf_coulomb_wave_FGp_array(double lam_min, int kmax,
                                double eta, double x,
                                double * fc_array, double * fcp_array,
                                double * gc_array, double * gcp_array,
                                double * F_exponent,
                                double * G_exponent
                                );

/* Coulomb wave function divided by the argument,
 * F(eta, x)/x. This is the function which reduces to
 * spherical Bessel functions in the limit eta->0.
 */
int gsl_sf_coulomb_wave_sphF_array(double lam_min, int kmax,
                                        double eta, double x,
                                        double * fc_array,
                                        double * F_exponent
                                        );


/* Coulomb wave function normalization constant.
 * [Abramowitz+Stegun 14.1.8, 14.1.9]
 */
int gsl_sf_coulomb_CL_e(double L, double eta, gsl_sf_result * result);
int gsl_sf_coulomb_CL_array(double Lmin, int kmax, double eta, double * cl);


__END_DECLS

#endif /* __GSL_SF_COULOMB_H__ */
