/* NUMhuber.cpp
 *
 * Copyright (C) 1994-2008, 2015-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 djmw 20030814 First version
 djmw 20080122 float -> double
*/

#include "NUM2.h"

void NUMmad (constVEC x, double *inout_location, bool wantlocation, double *out_mad, VEC *work) {
	Melder_assert (inout_location);
	if (x.size == 1) {
		if (wantlocation)  *inout_location= x [1];
		if (out_mad) *out_mad = undefined;
		return;
	}
	VEC tmp;
	autoVEC atmp;   // keep alive till end of function
	if (work) {
		tmp = *work;
		Melder_assert (tmp.size = x.size);
	} else {
		atmp = VECzero (x.size);
		tmp = atmp.get();
	}
	for (integer i = 1; i <= x.size; i ++)
		tmp [i] = x [i];
	if (wantlocation) {
		VECsort_inplace (tmp);
		*inout_location = NUMquantile (tmp, 0.5);
	}
	if (out_mad) {
		for (integer i = 1; i <= x.size; i ++)
			tmp [i] = fabs (tmp [i] - *inout_location);
		VECsort_inplace (tmp);
		*out_mad = 1.4826 * NUMquantile (tmp, 0.5);
	}
}

static double NUMgauss (double x) {
	return NUM1_sqrt2pi * exp (- 0.5 * x * x);
}

void NUMstatistics_huber (constVEC x, double *inout_location, bool wantlocation, double *inout_scale, bool wantscale, double k_stdev, double tol, VEC *work) {
	Melder_assert (inout_location && inout_scale);
	double theta = 2.0 * NUMgaussP (k_stdev) - 1.0;
	double beta = theta + k_stdev * k_stdev * (1.0 - theta) - 2.0 * k_stdev * NUMgauss (k_stdev);
	integer n1 = x.size;
	VEC tmp;
	autoVEC atmp;
	if (work) {
		tmp = *work;
		Melder_assert (tmp.size = x.size);
	} else {
		atmp = VECzero (x.size);
		tmp = atmp.get();
	}
	double mad;
	NUMmad (x, inout_location, wantlocation, & mad, & tmp);
	Melder_require (mad != 0, U"Scale is zero.");
	if (wantscale) {
		*inout_scale = mad;
	}

	double mu0, mu1 = *inout_location;
	double s0, s1 = *inout_scale;

	if (wantlocation) {
		n1 = x.size - 1;
	}

	do {
		mu0 = mu1;
		s0 = s1;

		double low  = mu0 - k_stdev * s0;
		double high = mu0 + k_stdev * s0;

		for (integer i = 1; i <= x.size; i ++) {
			if (x [i] < low) {
				tmp [i] = low;
			} else if (x [i] > high) {
				tmp [i] = high;
			} else {
				tmp [i] =  x [i];
			}
		}
		if (wantlocation)
			mu1 = NUMmean (tmp);
			
		if (wantscale) {
			s1 = 0.0;
			for (integer i = 1; i <= x.size; i ++) {
				double dx = tmp [i] - mu1;
				s1 += dx * dx;
			}
			s1 = sqrt (s1 / (n1 * beta));
		}
	} while (fabs (mu0 - mu1) > tol * s0 || fabs (s0 - s1) > tol * s0); //TODO fabs (mu0 - mu1) > tol * s0 ??

	if (wantlocation) {
		*inout_location = mu1;
	}
	if (wantscale) {
		*inout_scale = s1;
	}
}
