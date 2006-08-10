/* Sound_to_PointProcess.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 1999/05/25
 * pb 2002/07/16 GPL
 * pb 2003/02/26 Sound_to_PointProcess_peaks
 * pb 2004/07/11 default time steps in Sound_to_Pitch
 */

#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PointProcess.h"

PointProcess Sound_to_PointProcess_extrema (Sound me, int interpolation, int includeMaxima, int includeMinima) {
	PointProcess thee = NULL;
	float *y = my z [1];
	long numberOfMaxima, numberOfMinima, i;

	/*
	 * Pass 1: count the extrema. There may be a maximum and minimum in the same interval!
	 */
	numberOfMaxima = numberOfMinima = 0;
	for (i = 2; i <= my nx - 1; i ++) {
		if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1]) numberOfMaxima ++;
		if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1]) numberOfMinima ++;
	}

	/*
	 * Create the empty result.
	 */	
	thee = PointProcess_create (my xmin, my xmax, numberOfMaxima + numberOfMinima);
	if (! thee) return NULL;

	/*
	 * Pass 2: compute and register the extrema.
	 */
	for (i = 2; i <= my nx - 1; i ++) {
		double time, value, i_real;
		if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1]) {
			value = NUMimproveMaximum_f (y, my nx, i, interpolation, & i_real);
			time = my x1 + (i_real - 1.0) * my dx;
			PointProcess_addPoint (thee, time);
		}
		if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1]) {
			value = NUMimproveMinimum_f (y, my nx, i, interpolation, & i_real);
			time = my x1 + (i_real - 1.0) * my dx;
			PointProcess_addPoint (thee, time);
		}
	}
	return thee;
}

PointProcess Sound_to_PointProcess_maxima (Sound me, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, interpolation, TRUE, FALSE); }
PointProcess Sound_to_PointProcess_minima (Sound me, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, interpolation, FALSE, TRUE); }
PointProcess Sound_to_PointProcess_allExtrema (Sound me, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, interpolation, TRUE, TRUE); }

PointProcess Sound_to_PointProcess_zeroes (Sound me, int includeRaisers, int includeFallers) {
	PointProcess thee = NULL;
	float *y = my z [1];
	long numberOfRaisers, numberOfFallers, i;

	/*
	 * Pass 1: count the zeroes.
	 */
	numberOfRaisers = numberOfFallers = 0;
	for (i = 2; i <= my nx; i ++) {
		if (includeRaisers && y [i - 1] < 0.0 && y [i] >= 0.0) numberOfRaisers ++;
		if (includeFallers && y [i - 1] >= 0.0 && y [i] < 0.0) numberOfFallers ++;
	}

	/*
	 * Create the empty result.
	 */	
	thee = PointProcess_create (my xmin, my xmax, numberOfRaisers + numberOfFallers);
	if (! thee) return NULL;

	/*
	 * Pass 2: compute and register the zeroes.
	 */
	for (i = 2; i <= my nx; i ++) {
		if ((includeRaisers && y [i - 1] < 0.0 && y [i] >= 0.0) ||
		    (includeFallers && y [i - 1] >= 0.0 && y [i] < 0.0)) {
			double time = Sampled_indexToX (me, i - 1) + my dx * y [i - 1] / (y [i - 1] - y [i]);   /* Linear. */
			PointProcess_addPoint (thee, time);
		}
	}
	return thee;
}

PointProcess Sound_to_PointProcess_periodic_cc (Sound me, double fmin, double fmax) {
	PointProcess thee = NULL;
	Pitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
	if (! pitch) goto error;
	if (! (thee = Sound_Pitch_to_PointProcess_cc (me, pitch))) goto error;
	forget (pitch);
	return thee;
error:
	forget (pitch);
	forget (thee);
	return Melder_errorp ("(Sound_to_PointProcess:) Not performed.");
}

PointProcess Sound_to_PointProcess_periodic_peaks (Sound me, double fmin, double fmax, int includeMaxima, int includeMinima) {
	PointProcess thee = NULL;
	Pitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
	if (! pitch) goto error;
	if (! (thee = Sound_Pitch_to_PointProcess_peaks (me, pitch, includeMaxima, includeMinima))) goto error;
	forget (pitch);
	return thee;
error:
	forget (pitch);
	forget (thee);
	return Melder_errorp ("(Sound_to_PointProcess:) Not performed.");
}

/* End of file Sound_to_PointProcess.c */
