#ifndef _Electroglottogram_h_
#define _Electroglottogram_h_
/* Electroglottogram.h
 *
 * Copyright (C) 2019 David Weenink
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

#include "Sound.h"
#include "AmplitudeTier.h"
#include "TextGrid.h"

#include "Electroglottogram_enums.h"

Thing_define (Electroglottogram, Sound) {
	void v_info ()
			override;
};

/*
	An Electroglottogram represets the degree of contact between the (vibrating) vocal folds during voice production.
	It is measured at the throat.
	It will be represented as a one channel Sound.
	There might be an accompanying sound. They are both sampled at the same sampling frequency.
*/

autoElectroglottogram Electroglottogram_create (double xmin, double xmax, integer nx, double dx, double x1);

void Electroglottogram_drawStylized (Graphics g, bool marks, bool levels);

autoElectroglottogram Sound_extractElectroglottogram (Sound me, integer channel, bool invert);

autoAmplitudeTier Electroglottogram_and_AmplitudeTiers_getLevels (Electroglottogram me, AmplitudeTier peaks, AmplitudeTier valleys, double closingLevelPercentage);

// TODO better name
autoTextTier Electroglottogram_to_TextTier_closedGlottis (Electroglottogram me, double pitchFloor, double PitchCeiling, double closingThreshold, double silenceThreshold, kElectroglottogram_findClosedIntervalMethod method);

autoAmplitudeTier Electroglottogram_to_AmplitudeTier_levels (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, autoAmplitudeTier *out_peaks, autoAmplitudeTier *out_valleys);

autoIntervalTier Electroglottogram_getClosedGlottisIntervals (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, double peakThresholdFraction);

autoSound Electroglottogram_derivative (Electroglottogram me, double lowPassFrequency, double smoothing, bool peak99);
/* The real derivative */

autoSound Electroglottogram_firstCentralDifference (Electroglottogram me, bool peak99);
/* d(EEG)/dt [col] = z[col+1]-z[col-1] */

autoElectroglottogram Electroglottogram_highPassFilter (Electroglottogram me, double fromFrequency, double smoothing);
/*
	To remove drift.
*/

autoSound Electroglottogram_to_Sound (Electroglottogram me);

#endif

/* End of file Electroglottogram.h */

