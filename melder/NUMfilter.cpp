/* NUMfilter.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014-2020 Paul Boersma
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

#include "melder.h"

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2) {
	*a1 = 2.0 * exp (- NUMpi * bandwidth * dt) * cos (2.0 * NUMpi * formant * dt);
	*a2 = exp (- 2.0 * NUMpi * bandwidth * dt);
}

void VECfilterSecondOrderSection_a_inplace (VECVU const& x, double a1, double a2) {
	x [2] += a1 * x [1];
	for (integer i = 3; i <= x.size; i ++)
		x [i] += a1 * x [i - 1] - a2 * x [i - 2];
}

void VECfilterSecondOrderSection_fb_inplace (VECVU const& x, double dt, double formant, double bandwidth) {
	double a1, a2;
	NUMfbtoa (formant, bandwidth, dt, & a1, & a2);
	VECfilterSecondOrderSection_a_inplace (x, a1, a2);
}

double NUMftopreemphasis (double f, double dt) {
	return exp (- 2.0 * NUMpi * f * dt);
}

void VECpreemphasize_a_inplace (VECVU const& x, double preemphasis) {
	for (integer i = x.size; i >= 2; i --)
		x [i] -= preemphasis * x [i - 1];
}

void VECdeemphasize_a_inplace (VECVU const& x, double preemphasis) {
	for (integer i = 2; i <= x.size; i ++)
		x [i] += preemphasis * x [i - 1];
}

void VECpreemphasize_f_inplace (VECVU const& x, double dt, double frequency) {
	VECpreemphasize_a_inplace (x, NUMftopreemphasis (frequency, dt));
}

void VECdeemphasize_f_inplace (VECVU const& x, double dt, double frequency) {
	VECdeemphasize_a_inplace (x, NUMftopreemphasis (frequency, dt));
}

/* End of file NUMfilter.cpp */
