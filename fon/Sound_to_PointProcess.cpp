/* Sound_to_PointProcess.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2003/02/26 Sound_to_PointProcess_peaks
 * pb 2004/07/11 default time steps in Sound_to_Pitch
 * pb 2007/01/28 made compatible with stereo sounds
 * pb 2008/01/19 double
 * pb 2011/03/09 C++
 */

#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PointProcess.h"

PointProcess Sound_to_PointProcess_extrema (Sound me, long channel, int interpolation, bool includeMaxima, bool includeMinima) {
	try {
		/*
		 * Pass 1: count the extrema. There may be a maximum and minimum in the same interval!
		 */
		long numberOfMaxima = 0;
		long numberOfMinima = 0;
		double *y = my z [channel];
		for (long i = 2; i <= my nx - 1; i ++) {
			if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1]) numberOfMaxima ++;
			if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1]) numberOfMinima ++;
		}
		/*
		 * Create the empty result.
		 */	
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfMaxima + numberOfMinima);
		/*
		 * Pass 2: compute and register the extrema.
		 */
		for (long i = 2; i <= my nx - 1; i ++) {
			double time, value, i_real;
			if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				value = NUMimproveMaximum (y, my nx, i, interpolation, & i_real);
				time = my x1 + (i_real - 1.0) * my dx;
				PointProcess_addPoint (thee.peek(), time);
			}
			if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1]) {
				value = NUMimproveMinimum (y, my nx, i, interpolation, & i_real);
				time = my x1 + (i_real - 1.0) * my dx;
				PointProcess_addPoint (thee.peek(), time);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": extrema not computed.");
	}
}

PointProcess Sound_to_PointProcess_maxima (Sound me, long channel, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, channel, interpolation, true, false); }
PointProcess Sound_to_PointProcess_minima (Sound me, long channel, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, channel, interpolation, false, true); }
PointProcess Sound_to_PointProcess_allExtrema (Sound me, long channel, int interpolation)
	{ return Sound_to_PointProcess_extrema (me, channel, interpolation, true, true); }

PointProcess Sound_to_PointProcess_zeroes (Sound me, long channel, bool includeRaisers, bool includeFallers) {
	try {
		/*
		 * Pass 1: count the zeroes.
		 */
		long numberOfRaisers = 0;
		long numberOfFallers = 0;
		double *y = my z [channel];
		for (long i = 2; i <= my nx; i ++) {
			if (includeRaisers && y [i - 1] < 0.0 && y [i] >= 0.0) numberOfRaisers ++;
			if (includeFallers && y [i - 1] >= 0.0 && y [i] < 0.0) numberOfFallers ++;
		}
		/*
		 * Create the empty result.
		 */	
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfRaisers + numberOfFallers);
		/*
		 * Pass 2: compute and register the zeroes.
		 */
		for (long i = 2; i <= my nx; i ++) {
			if ((includeRaisers && y [i - 1] < 0.0 && y [i] >= 0.0) ||
				(includeFallers && y [i - 1] >= 0.0 && y [i] < 0.0))
			{
				double time = Sampled_indexToX (me, i - 1) + my dx * y [i - 1] / (y [i - 1] - y [i]);   // linear
				PointProcess_addPoint (thee.peek(), time);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": zeroes not computed.");
	}
}

PointProcess Sound_to_PointProcess_periodic_cc (Sound me, double fmin, double fmax) {
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
		autoPointProcess thee = Sound_Pitch_to_PointProcess_cc (me, pitch.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": periodic pulses (cc) not computed.");
	}
}

PointProcess Sound_to_PointProcess_periodic_peaks (Sound me, double fmin, double fmax, bool includeMaxima, bool includeMinima) {
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
		autoPointProcess thee = Sound_Pitch_to_PointProcess_peaks (me, pitch.peek(), includeMaxima, includeMinima);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": periodic pulses (peaks) not computed.");
	}
}

/* End of file Sound_to_PointProcess.cpp */
