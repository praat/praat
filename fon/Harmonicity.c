/* Harmonicity.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2006/12/10 MelderInfo
 * pb 2007/03/17 domain quantity
 */

#include "Graphics.h"
#include "Harmonicity.h"

double Harmonicity_getMean (Harmonicity me, double tmin, double tmax) {
	long imin, imax, i, n, nSounding = 0;
	double sum = 0.0;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	for (i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			nSounding ++;
			sum += my z [1] [i];
		}
	}
	if (nSounding < 1) return NUMundefined;
	return sum / nSounding;
}

double Harmonicity_getStandardDeviation (Harmonicity me, double tmin, double tmax) {
	long imin, imax, i, n, nSounding = 0;
	double sum = 0.0, mean, sumOfSquares = 0.0;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	for (i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			nSounding ++;
			sum += my z [1] [i];
		}
	}
	if (nSounding < 2) return NUMundefined;
	mean = sum / nSounding;
	for (i = imin; i <= imax; i ++) {
		if (my z [1] [i] != -200) {
			double d = my z [1] [i] - mean;
			sumOfSquares += d * d;
		}
	}
	return sqrt (sumOfSquares / (nSounding - 1));
}

double Harmonicity_getQuantile (Harmonicity me, double quantile) {
	long ix, nSounding = 0;
	double *strengths = NUMdvector (1, my nx), result = -200.0;
	for (ix = 1; ix <= my nx; ix ++)
		if (my z [1] [ix] != -200)
			strengths [++ nSounding] = my z [1] [ix];
	if (nSounding >= 1) {
		NUMsort_d (nSounding, strengths);
		result = NUMquantile_d (nSounding, strengths, quantile);
	}
	NUMdvector_free (strengths, 1);
	return result;
}

static void info (I) {
	iam (Harmonicity);
	classData -> info (me);
	MelderInfo_writeLine1 ("Time domain:");
	MelderInfo_writeLine3 ("   Start time: ", Melder_double (my xmin), " seconds");
	MelderInfo_writeLine3 ("   End time: ", Melder_double (my xmax), " seconds");
	MelderInfo_writeLine3 ("   Total duration: ", Melder_double (my xmax - my xmin), " seconds");
	long nSounding = 0;
	double *strengths = NUMdvector (1, my nx);
	for (long ix = 1; ix <= my nx; ix ++)
		if (my z [1] [ix] != -200)
			strengths [++ nSounding] = my z [1] [ix];
	MelderInfo_writeLine1 ("Time sampling:");
	MelderInfo_writeLine5 ("   Number of frames: ", Melder_integer (my nx), " (", Melder_integer (nSounding), " sounding)");
	MelderInfo_writeLine3 ("   Time step: ", Melder_double (my dx), " seconds");
	MelderInfo_writeLine3 ("   First frame centred at: ", Melder_double (my x1), " seconds");
	if (nSounding) {
		double sum = 0, sumOfSquares = 0;
		MelderInfo_writeLine1 ("Periodicity-to-noise ratios of sounding frames:");
		NUMsort_d (nSounding, strengths);
		MelderInfo_writeLine3 ("   Median ", Melder_single (NUMquantile_d (nSounding, strengths, 0.50)), " dB");
		MelderInfo_writeLine5 ("   10 % = ", Melder_single (NUMquantile_d (nSounding, strengths, 0.10)), " dB   90 %% = ",
			Melder_single (NUMquantile_d (nSounding, strengths, 0.90)), " dB");
		MelderInfo_writeLine5 ("   16 % = ", Melder_single (NUMquantile_d (nSounding, strengths, 0.16)), " dB   84 %% = ",
			Melder_single (NUMquantile_d (nSounding, strengths, 0.84)), " dB");
		MelderInfo_writeLine5 ("   25 % = ", Melder_single (NUMquantile_d (nSounding, strengths, 0.25)), " dB   75 %% = ",
			Melder_single (NUMquantile_d (nSounding, strengths, 0.75)), " dB");
		MelderInfo_writeLine3 ("Minimum: ", Melder_single (strengths [1]), " dB");
		MelderInfo_writeLine3 ("Maximum: ", Melder_single (strengths [nSounding]), " dB");
		for (long i = 1; i <= nSounding; i ++) {
			double f = strengths [i];
			sum += f;
			sumOfSquares += f * f;
		}
		MelderInfo_writeLine3 ("Average: ", Melder_single (sum / nSounding), " dB");
		if (nSounding > 1) {
			double var = (sumOfSquares - sum * sum / nSounding) / (nSounding - 1);
			MelderInfo_writeLine3 ("Standard deviation: ", Melder_single (var < 0.0 ? 0.0 : sqrt (var)), " dB");
		}
	}
	NUMdvector_free (strengths, 1);
}

class_methods (Harmonicity, Vector)
	class_method (info)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

Harmonicity Harmonicity_create (double tmin, double tmax, long nt, double dt, double t1) {
	Harmonicity me = new (Harmonicity);
	if (! me || ! Matrix_init (me, tmin, tmax, nt, dt, t1, 1, 1, 1, 1, 1)) return NULL;
	return me;
}

Matrix Harmonicity_to_Matrix (Harmonicity me) {
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
						my ymin, my ymax, my ny, my dy, my y1);
	NUMfvector_copyElements (my z [1], thy z [1], 1, my nx);
	return thee;
}

Harmonicity Matrix_to_Harmonicity (I) {
	iam (Matrix);
	Harmonicity thee = Harmonicity_create (my xmin, my xmax, my nx, my dx, my x1);
	if (! thee) return NULL;
	NUMfvector_copyElements (my z [1], thy z [1], 1, my nx);
	return thee;
}

/* End of file Harmonicity.c */
