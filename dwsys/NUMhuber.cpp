/* NUMhuber.cpp
 *
 * Copyright (C) 1994-2018 David Weenink
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

void NUMmad (constVEC x, double *inout_location, bool wantlocation, double *out_mad) {
	Melder_assert (inout_location);
	if (x.size == 1) {
		if (wantlocation)  *inout_location= x [1];
		if (out_mad) *out_mad = undefined;
		return;
	}
	autoVEC work = newVECcopy (x);
	
	if (wantlocation) {
		VECsort_inplace (work.get());
		*inout_location = NUMquantile (work.get(), 0.5);
	}
	if (out_mad) {
		for (integer i = 1; i <= x.size; i ++)
			work [i] = fabs (work [i] - *inout_location);
		VECsort_inplace (work.get());
		*out_mad = 1.4826 * NUMquantile (work.get(), 0.5);
	}
}

static double NUMgauss (double x) {
	return NUM1_sqrt2pi * exp (- 0.5 * x * x);
}

void NUMstatistics_huber (constVEC x, double *inout_location, bool wantlocation, double *inout_scale, bool wantscale, double k_stdev, double tol, integer maximumNumberOfiterations) {
	Melder_assert (inout_location && inout_scale);
	double theta = 2.0 * NUMgaussP (k_stdev) - 1.0;
	double beta = theta + k_stdev * k_stdev * (1.0 - theta) - 2.0 * k_stdev * NUMgauss (k_stdev);
	double scale;
	
	NUMmad (x, inout_location, wantlocation, & scale);
	Melder_require (scale != 0, U"All your data points are equal.");
	
	if (wantscale) *inout_scale = scale;

	double location0, location1 = *inout_location;
	double scale0, scale1 = *inout_scale;

	integer n1 = wantlocation ? x.size - 1 : x.size, iter = 0;
	bool locationCriterion, scaleCriterion;
	autoVEC work = newVECraw (x.size);
	do {
		location0 = location1;
		scale0 = scale1;

		double low  = location0 - k_stdev * scale0;
		double high = location0 + k_stdev * scale0;
		
		work.get ()  <<=  x;
		VECclip_inplace_inline (work.get (), low, high); // windsorize
		
		if (wantlocation)
			location1 = NUMmean (work.get());

		work.get() -= location1;
		double sumsq = NUMsumsq (work.get());
		scale1 = sqrt (sumsq / (n1 * beta));
		locationCriterion = wantlocation ? fabs (location0 - location1) > tol * location0 : true;
		scaleCriterion = fabs (scale0 - scale1) > tol * scale0;
	} while (++ iter < maximumNumberOfiterations && (scaleCriterion || locationCriterion));

	if (wantlocation) *inout_location = location1;
	if (wantscale) *inout_scale = scale1;
}
