#ifndef _NUMmath_h_
#define _NUMmath_h_
/* NUMmath.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/* More mathematical and numerical things than there are in <math.h>. */

/********** Inherit all the ANSI functions from math.h **********/

/* On the sgi, math.h declares some bessel functions. */
/* The following statements suppress these declarations */
/* so that the compiler will issue no warnings */
/* when you redeclare y0 etc. in your code. */
#ifdef sgi
	#define y0 sgi_y0
	#define y1 sgi_y1
	#define yn sgi_yn
	#define j0 sgi_j0
	#define j1 sgi_j1
	#define jn sgi_jn
#endif
#include <math.h>
#ifdef sgi
	#undef y0
	#undef y1
	#undef yn
	#undef j0
	#undef j1
	#undef jn
#endif

#undef M_PI   /* M_PI is a GNU extension, so not always available under -std=c++17 (one can also use NUMpi) */
#define M_PI  3.1415926535897932384626433832795028841972

void NUMshift (double *x, double xfrom, double xto);
void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto);

/********** Constants **********
 * Forty-digit constants computed by e.g.:
 *    bc -l
 *       scale=42
 *       print e(1)
 * Then rounding away the last two digits.
 */
//      print e(1)
constexpr double NUMe = 2.7182818284590452353602874713526624977572;
//      print 1/l(2)
constexpr double NUMlog2e = 1.4426950408889634073599246810018921374266;
//      print l(10)/l(2)
constexpr double NUMlog2_10 = 3.3219280948873623478703194294893901758648;
//      print 1/l(10)
constexpr double NUMlog10e = 0.4342944819032518276511289189166050822944;
//      print l(2)/l(10)
constexpr double NUMlog10_2 = 0.3010299956639811952137388947244930267682;
//      print l(2)
constexpr double NUMln2 = 0.6931471805599453094172321214581765680755;
//      print l(10)
constexpr double NUMln10 = 2.3025850929940456840179914546843642076011;
//      print a(1)*8
constexpr double NUM2pi = 6.2831853071795864769252867665590057683943;
//      print a(1)*4
constexpr double NUMpi = 3.1415926535897932384626433832795028841972;
//      print a(1)*2
constexpr double NUMpi_2 = 1.5707963267948966192313216916397514420986;
//      print a(1)
constexpr double NUMpi_4 = 0.7853981633974483096156608458198757210493;
//      print 0.25/a(1)
constexpr double NUM1_pi = 0.3183098861837906715377675267450287240689;
//      print 0.5/a(1)
constexpr double NUM2_pi = 0.6366197723675813430755350534900574481378;
//      print sqrt(a(1)*4)
constexpr double NUMsqrtpi = 1.7724538509055160272981674833411451827975;
//      print sqrt(a(1)*8)
constexpr double NUMsqrt2pi = 2.5066282746310005024157652848110452530070;
//      print 1/sqrt(a(1)*8)
constexpr double NUM1_sqrt2pi = 0.3989422804014326779399460599343818684759;
//      print 1/sqrt(a(1))
constexpr double NUM2_sqrtpi = 1.1283791670955125738961589031215451716881;
//      print l(a(1)*4)
constexpr double NUMlnpi = 1.1447298858494001741434273513530587116473;
//      print sqrt(2)
constexpr double NUMsqrt2 = 1.4142135623730950488016887242096980785697;
//      print sqrt(0.5)
constexpr double NUMsqrt1_2 = 0.7071067811865475244008443621048490392848;
//      print sqrt(3)
constexpr double NUMsqrt3 = 1.7320508075688772935274463415058723669428;
//      print sqrt(5)
constexpr double NUMsqrt5 = 2.2360679774997896964091736687312762354406;
//      print sqrt(6)
constexpr double NUMsqrt6 = 2.4494897427831780981972840747058913919659;
//      print sqrt(7)
constexpr double NUMsqrt7 = 2.6457513110645905905016157536392604257102;
//      print sqrt(8)
constexpr double NUMsqrt8 = 2.8284271247461900976033774484193961571393;
//      print sqrt(10)
constexpr double NUMsqrt10 = 3.1622776601683793319988935444327185337196;
//      print sqrt(5)/2-0.5
constexpr double NUM_goldenSection = 0.6180339887498948482045868343656381177203;
// The Euler-Mascheroni constant cannot be computed by bc.
// Instead we use the 40 digits computed by Johann von Soldner in 1809.
constexpr double NUM_euler = 0.5772156649015328606065120900824024310422;

/* End of file NUMmath.h */
#endif
