#ifndef _NUMspecfunc_h_
#define _NUMspecfunc_h_
/* NUMspecfunc.h
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

double NUMlnGamma (double x);
double NUMbeta (double z, double w);
double NUMbesselI (integer n, double x);   // precondition: n >= 0
double NUMbessel_i0_f (double x);
double NUMbessel_i1_f (double x);
double NUMbesselK (integer n, double x);   // preconditions: n >= 0 && x > 0.0
double NUMbessel_k0_f (double x);
double NUMbessel_k1_f (double x);
double NUMbesselK_f (integer n, double x);
double NUMsigmoid (double x);   // correct also for large positive or negative x
double NUMinvSigmoid (double x);
double NUMerfcc (double x);
double NUMgaussP (double z);
double NUMgaussQ (double z);
double NUMincompleteGammaP (double a, double x);
double NUMincompleteGammaQ (double a, double x);
double NUMchiSquareP (double chiSquare, double degreesOfFreedom);
double NUMchiSquareQ (double chiSquare, double degreesOfFreedom);
double NUMcombinations (integer n, integer k);
double NUMincompleteBeta (double a, double b, double x);   // incomplete beta function Ix(a,b). Preconditions: a, b > 0; 0 <= x <= 1
double NUMbinomialP (double p, double k, double n);
double NUMbinomialQ (double p, double k, double n);
double NUMinvBinomialP (double p, double k, double n);
double NUMinvBinomialQ (double p, double k, double n);

/* End of file NUMspecfunc.h */
#endif
