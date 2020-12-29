/* NUMhuber.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
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
#include "melder.h"

void NUMmad (constVEC x, double *inout_location, bool wantlocation, double *out_mad, VEC const& workSpace) {
	Melder_assert (inout_location);
	Melder_assert (workSpace.size >= x.size);
	if (x.size == 1) {
		if (wantlocation)
			*inout_location= x [1];
		if (out_mad)
			*out_mad = undefined;
		return;
	}
	
	VEC work = workSpace.part (1, x.size);
	work  <<=  x;
	
	if (wantlocation) {
		sort_VEC_inout (work);
		*inout_location = NUMquantile (work, 0.5);
	}
	if (out_mad) {
		for (integer i = 1; i <= x.size; i ++)
			work [i] = fabs (work [i] - *inout_location);
		sort_VEC_inout (work);
		*out_mad = 1.4826 * NUMquantile (work, 0.5);
	}
}

static double NUMgauss (double x) {
	return NUM1_sqrt2pi * exp (- 0.5 * x * x);
}

void NUMstatistics_huber (constVEC x, double *inout_location, bool wantlocation, double *inout_scale, bool wantscale, double k_stdev, double tol, integer maximumNumberOfiterations, VEC const& workSpace) {
	Melder_assert (inout_location && inout_scale);
	Melder_assert (workSpace.size >= x.size);
	
	const double theta = 2.0 * NUMgaussP (k_stdev) - 1.0;
	const double beta = theta + k_stdev * k_stdev * (1.0 - theta) - 2.0 * k_stdev * NUMgauss (k_stdev);
	double scale = *inout_scale, location = *inout_location;
	
	NUMmad (x, & location, wantlocation, & scale, workSpace);
	if (scale > 0.0) {
		VEC work = workSpace.part (1, x.size);
		const double degreesOfFreedom = ( wantlocation ? x.size - 1 : x.size );
		integer iter = 0;
		bool farFromLocation = false, farFromScale = false;
		do {
			const double previousLocation = location;
			const double previousScale = scale;

			work  <<=  x;
			VECclip_inplace (location - k_stdev * scale, work, location + k_stdev * scale); // winsorize
			
			if (wantlocation) {
				location = NUMmean (work);
				farFromLocation = ( fabs (location - previousLocation) > std::max (tol * fabs (location), NUMeps) );
			}
			if (wantscale) {
				work  -=  location;
				const double sumsq = NUMsum2 (work);
				scale = sqrt (sumsq / (degreesOfFreedom * beta));
				farFromScale = ( fabs (scale - previousScale) > std::max (tol * scale, NUMeps) );
			}
		} while (++ iter < maximumNumberOfiterations && (farFromScale || farFromLocation));
	}
	if (wantlocation)
		*inout_location = location;
	if (wantscale)
		*inout_scale = scale;
}
