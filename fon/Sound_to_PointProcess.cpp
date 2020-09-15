/* Sound_to_PointProcess.cpp
 *
 * Copyright (C) 1992-2008,2011,2014-2018,2020 Paul Boersma
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

#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PointProcess.h"

autoPointProcess Sound_to_PointProcess_extrema (Sound me, integer channelNumber, kVector_peakInterpolation peakInterpolationType, bool includeMaxima, bool includeMinima) {
	try {
		/*
		 * Pass 1: count the extrema. There may be a maximum and minimum in the same interval!
		 */
		integer numberOfMaxima = 0;
		integer numberOfMinima = 0;
		constVEC y = my z.row (channelNumber);
		for (integer i = 2; i <= my nx - 1; i ++) {
			if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1])
				numberOfMaxima ++;
			if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1])
				numberOfMinima ++;
		}
		/*
		 * Create the empty result.
		 */	
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfMaxima + numberOfMinima);
		/*
		 * Pass 2: compute and register the extrema.
		 */
		const integer interpolationDepth = kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType);
		for (integer i = 2; i <= my nx - 1; i ++) {
			double time, i_real;
			if (includeMaxima && y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				(void) NUMimproveMaximum (y, i, interpolationDepth, & i_real);
				time = my x1 + (i_real - 1.0) * my dx;
				PointProcess_addPoint (thee.get(), time);
			}
			if (includeMinima && y [i] <= y [i - 1] && y [i] < y [i + 1]) {
				(void) NUMimproveMinimum (y, i, interpolationDepth, & i_real);
				time = my x1 + (i_real - 1.0) * my dx;
				PointProcess_addPoint (thee.get(), time);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": extrema not computed.");
	}
}

autoPointProcess Sound_to_PointProcess_maxima (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType) {
	return Sound_to_PointProcess_extrema (me, channel, peakInterpolationType, true, false);
}
autoPointProcess Sound_to_PointProcess_minima (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType) {
	return Sound_to_PointProcess_extrema (me, channel, peakInterpolationType, false, true);
}
autoPointProcess Sound_to_PointProcess_allExtrema (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType) {
	return Sound_to_PointProcess_extrema (me, channel, peakInterpolationType, true, true);
}

autoPointProcess Sound_to_PointProcess_zeroes (Sound me, integer channel, bool includeRaisers, bool includeFallers) {
	try {
		/*
		 * Pass 1: count the zeroes.
		 */
		integer numberOfRaisers = 0;
		integer numberOfFallers = 0;
		double *y = & my z [channel] [0];
		for (integer i = 2; i <= my nx; i ++) {
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
		for (integer i = 2; i <= my nx; i ++) {
			if ((includeRaisers && y [i - 1] < 0.0 && y [i] >= 0.0) ||
				(includeFallers && y [i - 1] >= 0.0 && y [i] < 0.0))
			{
				double time = Sampled_indexToX (me, i - 1) + my dx * y [i - 1] / (y [i - 1] - y [i]);   // linear
				PointProcess_addPoint (thee.get(), time);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": zeroes not computed.");
	}
}

autoPointProcess Sound_to_PointProcess_periodic_cc (Sound me, double fmin, double fmax) {
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
		autoPointProcess thee = Sound_Pitch_to_PointProcess_cc (me, pitch.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": periodic pulses (cc) not computed.");
	}
}

autoPointProcess Sound_to_PointProcess_periodic_peaks (Sound me, double fmin, double fmax, bool includeMaxima, bool includeMinima) {
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.0, fmin, fmax);
		autoPointProcess thee = Sound_Pitch_to_PointProcess_peaks (me, pitch.get(), includeMaxima, includeMinima);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": periodic pulses (peaks) not computed.");
	}
}

/* End of file Sound_to_PointProcess.cpp */
