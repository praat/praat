/* Harmonicity.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 2002/06/04
 * pb 2002/07/16 GPL
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
	char info [1000], infootje [200];
	long ix, nSounding = 0, i;
	double *strengths = NUMdvector (1, my nx);
	for (ix = 1; ix <= my nx; ix ++)
		if (my z [1] [ix] != -200)
			strengths [++ nSounding] = my z [1] [ix];
	sprintf (info, "Harmonicity info:\n"
		"Starting time: %.16g seconds\n"
		"End time: %.16g seconds\n"
		"Number of frames: %ld (%ld sounding)\n"
		"Time step: %.16g seconds\n"
		"First frame at: %.16g seconds\n",
		my xmin, my xmax, my nx, nSounding, my dx, my x1);
	if (nSounding) {
		double sum = 0, sumOfSquares = 0;
		strcat (info, "\nPeriodicity-to-noise ratios of sounding frames:");
		NUMsort_d (nSounding, strengths);
		sprintf (infootje, "\nMedian %.1f dB"
								 "\n10 %% = %.1f dB   90 %% = %.1f dB"
								 "\n16 %% = %.1f dB   84 %% = %.1f dB",
			NUMquantile_d (nSounding, strengths, 0.5),
			NUMquantile_d (nSounding, strengths, 0.1),
			NUMquantile_d (nSounding, strengths, 0.9),
			NUMquantile_d (nSounding, strengths, 0.16),
			NUMquantile_d (nSounding, strengths, 0.84));
		strcat (info, infootje);
		for (i = 1; i <= nSounding; i ++) {
			double f = strengths [i];
			sum += f;
			sumOfSquares += f * f;
		}
		sprintf (infootje, "\nMinimum %.1f dB, maximum %.1f dB\nAverage %.1f dB",
			strengths [1], strengths [nSounding], sum / nSounding);
		strcat (info, infootje);
		if (nSounding > 1) {
			double var = (sumOfSquares - sum * sum / nSounding) / (nSounding - 1);
			sprintf (infootje, ", standard deviation %.1f dB",
				var < 0.0 ? 0.0 : sqrt (var));
			strcat (info, infootje);
		}
	}
	NUMdvector_free (strengths, 1);
	Melder_information ("%s", info);
}

class_methods (Harmonicity, Vector)
	class_method (info)
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
