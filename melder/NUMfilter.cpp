/* NUMfilter.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2003/07/09 gsl
 * pb 2008/01/19 double
 * pb 2011/03/29 C++
 */

#include "melder.h"

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2) {
	*a1 = 2 * exp (- NUMpi * bandwidth * dt) * cos (2 * NUMpi * formant * dt);
	*a2 = exp (- 2 * NUMpi * bandwidth * dt);
}

void NUMfilterSecondOrderSection_a (double x [], integer n, double a1, double a2) {
	x [2] += a1 * x [1];
	for (integer i = 3; i <= n; i ++)
		x [i] += a1 * x [i - 1] - a2 * x [i - 2];
}

void NUMfilterSecondOrderSection_fb (double x [], integer n, double dt, double formant, double bandwidth) {
	double a1, a2;
	NUMfbtoa (formant, bandwidth, dt, & a1, & a2);
	NUMfilterSecondOrderSection_a (x, n, a1, a2);
}

double NUMftopreemphasis (double f, double dt) {
	return exp (- 2.0 * NUMpi * f * dt);
}

void NUMpreemphasize_a (double x [], integer n, double preemphasis) {
	for (integer i = n; i >= 2; i --)
		x [i] -= preemphasis * x [i - 1];
}

void NUMdeemphasize_a (double x [], integer n, double preemphasis) {
	for (integer i = 2; i <= n; i ++)
		x [i] += preemphasis * x [i - 1];
}

void NUMpreemphasize_f (double x [], integer n, double dt, double frequency) {
	NUMpreemphasize_a (x, n, NUMftopreemphasis (frequency, dt));
}

void NUMdeemphasize_f (double x [], integer n, double dt, double frequency) {
	NUMdeemphasize_a (x, n, NUMftopreemphasis (frequency, dt));
}

/* End of file NUMfilter.cpp */
