/* specfunc/gsl_sf_dilog.h
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

/* Author:  G. Jungman */

#ifndef __GSL_SF_DILOG_H__
#define __GSL_SF_DILOG_H__

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


/* Real part of DiLogarithm(x), for real argument.
 * In Lewin's notation, this is Li_2(x).
 *
 *   Li_2(x) = - Re[ Integrate[ Log[1-s] / s, {s, 0, x}] ]
 *
 * The function in the complex plane has a branch point
 * at z = 1; we place the cut in the conventional way,
 * on [1, +infty). This means that the value for real x > 1
 * is a matter of definition; however, this choice does not
 * affect the real part and so is not relevant to the
 * interpretation of this implemented function.
 */
int     gsl_sf_dilog_e(const double x, gsl_sf_result * result);
double  gsl_sf_dilog(const double x);


/* DiLogarithm(z), for complex argument z = x + i y.
 * Computes the principal branch.
 *
 * Recall that the branch cut is on the real axis with x > 1.
 * The imaginary part of the computed value on the cut is given
 * by -Pi*log(x), which is the limiting value taken approaching
 * from y < 0. This is a conventional choice, though there is no
 * true standardized choice.
 *
 * Note that there is no canonical way to lift the defining
 * contour to the full Riemann surface because of the appearance
 * of a "hidden branch point" at z = 0 on non-principal sheets.
 * Experts will know the simple algebraic prescription for
 * obtaining the sheet they want; non-experts will not want
 * to know anything about it. This is why GSL chooses to compute
 * only on the principal branch.
 */
int
gsl_sf_complex_dilog_xy_e(
  const double x,
  const double y,
  gsl_sf_result * result_re,
  gsl_sf_result * result_im
  );



/* DiLogarithm(z), for complex argument z = r Exp[i theta].
 * Computes the principal branch, thereby assuming an
 * implicit reduction of theta to the range (-2 pi, 2 pi).
 *
 * If theta is identically zero, the imaginary part is computed
 * as if approaching from y > 0. For other values of theta no
 * special consideration is given, since it is assumed that
 * no other machine representations of multiples of pi will
 * produce y = 0 precisely. This assumption depends on some
 * subtle properties of the machine arithmetic, such as
 * correct rounding and monotonicity of the underlying
 * implementation of sin() and cos().
 *
 * This function is ok, but the interface is confusing since
 * it makes it appear that the branch structure is resolved.
 * Furthermore the handling of values close to the branch
 * cut is subtle. Perhap this interface should be deprecated.
 */
int
gsl_sf_complex_dilog_e(
  const double r,
  const double theta,
  gsl_sf_result * result_re,
  gsl_sf_result * result_im
  );



/* Spence integral; spence(s) := Li_2(1-s)
 *
 * This function has a branch point at 0; we place the
 * cut on (-infty,0). Because of our choice for the value
 * of Li_2(z) on the cut, spence(s) is continuous as
 * s approaches the cut from above. In other words,
 * we define spence(x) = spence(x + i 0+).
 */
int
gsl_sf_complex_spence_xy_e(
  const double x,
  const double y,
  gsl_sf_result * real_sp,
  gsl_sf_result * imag_sp
  );


__END_DECLS

#endif /* __GSL_SF_DILOG_H__ */
