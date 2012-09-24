/* Harmonicity.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Graphics.h"
#include "Harmonicity.h"

Thing_implement (Harmonicity, Vector, 2);

double Harmonicity_getMean (Harmonicity me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long imin, imax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	double sum = 0.0;
	long nSounding = 0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			nSounding ++;
			sum += my z [1] [i];
		}
	}
	if (nSounding < 1) return NUMundefined;
	return sum / nSounding;
}

double Harmonicity_getStandardDeviation (Harmonicity me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long imin, imax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	double sum = 0.0;
	long nSounding = 0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			nSounding ++;
			sum += my z [1] [i];
		}
	}
	if (nSounding < 2) return NUMundefined;
	double mean = sum / nSounding;
	double sumOfSquares = 0.0;
	for (long i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			double d = my z [1] [i] - mean;
			sumOfSquares += d * d;
		}
	}
	return sqrt (sumOfSquares / (nSounding - 1));
}

double Harmonicity_getQuantile (Harmonicity me, double quantile) {
	autoNUMvector <double> strengths (1, my nx);
	long nSounding = 0;
	for (long ix = 1; ix <= my nx; ix ++)
		if (my z [1] [ix] != -200)
			strengths [++ nSounding] = my z [1] [ix];
	double result = -200.0;
	if (nSounding >= 1) {
		NUMsort_d (nSounding, strengths.peek());
		result = NUMquantile (nSounding, strengths.peek(), quantile);
	}
	return result;
}

void structHarmonicity :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	autoNUMvector <double> strengths (1, nx);
	long nSounding = 0;
	for (long ix = 1; ix <= nx; ix ++)
		if (z [1] [ix] != -200)
			strengths [++ nSounding] = z [1] [ix];
	MelderInfo_writeLine (L"Time sampling:");
	MelderInfo_writeLine (L"   Number of frames: ", Melder_integer (nx), L" (", Melder_integer (nSounding), L" sounding)");
	MelderInfo_writeLine (L"   Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"   First frame centred at: ", Melder_double (x1), L" seconds");
	if (nSounding) {
		double sum = 0, sumOfSquares = 0;
		MelderInfo_writeLine (L"Periodicity-to-noise ratios of sounding frames:");
		NUMsort_d (nSounding, strengths.peek());
		MelderInfo_writeLine (L"   Median ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.50)), L" dB");
		MelderInfo_writeLine (L"   10 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.10)), L" dB   90 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.90)), L" dB");
		MelderInfo_writeLine (L"   16 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.16)), L" dB   84 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.84)), L" dB");
		MelderInfo_writeLine (L"   25 % = ", Melder_single (NUMquantile (nSounding, strengths.peek(), 0.25)), L" dB   75 %% = ",
			Melder_single (NUMquantile (nSounding, strengths.peek(), 0.75)), L" dB");
		MelderInfo_writeLine (L"Minimum: ", Melder_single (strengths [1]), L" dB");
		MelderInfo_writeLine (L"Maximum: ", Melder_single (strengths [nSounding]), L" dB");
		for (long i = 1; i <= nSounding; i ++) {
			double f = strengths [i];
			sum += f;
			sumOfSquares += f * f;
		}
		MelderInfo_writeLine (L"Average: ", Melder_single (sum / nSounding), L" dB");
		if (nSounding > 1) {
			double var = (sumOfSquares - sum * sum / nSounding) / (nSounding - 1);
			MelderInfo_writeLine (L"Standard deviation: ", Melder_single (var < 0.0 ? 0.0 : sqrt (var)), L" dB");
		}
	}
}

Harmonicity Harmonicity_create (double tmin, double tmax, long nt, double dt, double t1) {
	try {
		autoHarmonicity me = Thing_new (Harmonicity);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, 1, 1, 1, 1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Harmonicity not created.");
	}
}

Matrix Harmonicity_to_Matrix (Harmonicity me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMvector_copyElements (my z [1], thy z [1], 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, "not converted to Matrix.");
	}
}

Harmonicity Matrix_to_Harmonicity (Matrix me) {
	try {
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, my nx, my dx, my x1);
		NUMvector_copyElements (my z [1], thy z [1], 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, "not converted to Harmonicity.");
	}
}

/* End of file Harmonicity.cpp */
