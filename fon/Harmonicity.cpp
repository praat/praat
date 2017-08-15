/* Harmonicity.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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

#include "Graphics.h"
#include "Harmonicity.h"

Thing_implement (Harmonicity, Vector, 2);

double Harmonicity_getMean (Harmonicity me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long imin, imax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return undefined;
	real80 sum = 0.0;
	long nSounding = 0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200.0) {
			nSounding ++;
			sum += (real80) my z [1] [i];
		}
	}
	if (nSounding < 1) return undefined;
	return (real) sum / nSounding;
}

double Harmonicity_getStandardDeviation (Harmonicity me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long imin, imax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return undefined;
	real80 sum = 0.0;
	long nSounding = 0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200.0) {
			nSounding ++;
			sum += (real80) my z [1] [i];
		}
	}
	if (nSounding < 2) return undefined;
	real80 mean = sum / nSounding;
	real80 sumOfSquares = 0.0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200.0) {
			real80 d = (real80) my z [1] [i] - mean;
			sumOfSquares += d * d;
		}
	}
	return sqrt ((real) sumOfSquares / (nSounding - 1));
}

double Harmonicity_getQuantile (Harmonicity me, double quantile) {
	autoNUMvector <double> strengths (1, my nx);
	long nSounding = 0;
	for (long ix = 1; ix <= my nx; ix ++)
		if (my z [1] [ix] != -200.0)
			strengths [++ nSounding] = my z [1] [ix];
	double result = -200.0;
	if (nSounding >= 1) {
		NUMsort_d (nSounding, strengths.peek());
		result = NUMquantile (nSounding, strengths.peek(), quantile);
	}
	return result;
}

void structHarmonicity :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	autoNUMvector <double> strengths (1, nx);
	long nSounding = 0;
	for (long ix = 1; ix <= nx; ix ++)
		if (z [1] [ix] != -200.0)
			strengths [++ nSounding] = z [1] [ix];
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", nx, U" (", nSounding, U" sounding)");
	MelderInfo_writeLine (U"   Time step: ", dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", x1, U" seconds");
	if (nSounding) {
		MelderInfo_writeLine (U"Periodicity-to-noise ratios of sounding frames:");
		NUMsort_d (nSounding, strengths.peek());
		MelderInfo_writeLine (U"   Median ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.50)), U" dB");
		MelderInfo_writeLine (U"   10 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.10)), U" dB   90 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.90)), U" dB");
		MelderInfo_writeLine (U"   16 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.16)), U" dB   84 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.84)), U" dB");
		MelderInfo_writeLine (U"   25 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.25)), U" dB   75 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.75)), U" dB");
		MelderInfo_writeLine (U"Minimum: ", Melder_single (strengths [1]), U" dB");
		MelderInfo_writeLine (U"Maximum: ", Melder_single (strengths [nSounding]), U" dB");
		double sum = 0.0, sumOfSquares = 0.0;
		for (long i = 1; i <= nSounding; i ++) {
			double f = strengths [i];
			sum += f;
			sumOfSquares += f * f;
		}
		MelderInfo_writeLine (U"Average: ", Melder_single (sum / nSounding), U" dB");
		if (nSounding > 1) {
			double var = (sumOfSquares - sum * sum / nSounding) / (nSounding - 1);
			MelderInfo_writeLine (U"Standard deviation: ", Melder_single (var < 0.0 ? 0.0 : sqrt (var)), U" dB");
		}
	}
}

autoHarmonicity Harmonicity_create (double tmin, double tmax, long nt, double dt, double t1) {
	try {
		autoHarmonicity me = Thing_new (Harmonicity);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Harmonicity not created.");
	}
}

autoMatrix Harmonicity_to_Matrix (Harmonicity me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMvector_copyElements (my z [1], thy z [1], 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to Matrix.");
	}
}

autoHarmonicity Matrix_to_Harmonicity (Matrix me) {
	try {
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, my nx, my dx, my x1);
		NUMvector_copyElements (my z [1], thy z [1], 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to Harmonicity.");
	}
}

/* End of file Harmonicity.cpp */
