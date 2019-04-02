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
#include "TextGrid.h"

#include "Electroglottogram_enums.h"

Thing_define (Electroglottogram, Vector) {
	void v_info ()
		override;
	/*bool v_hasGetMatrix ()
		override { return true; }
	double v_getMatrix (integer irow, integer icol)
		override;
	bool v_hasGetFunction2 ()
		override { return true; }
	double v_getFunction2 (double x, double y)
		override;*/
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoElectroglottogram Electroglottogram_create (double xmin, double xmax, integer nx, double dx, double x1);

autoElectroglottogram Sound_extractElectroglottogram (Sound me, integer channelNumber);

autoAmplitudeTier Electroglottogram_and_AmplitudeTiers_getLevels (Electroglottogram me, AmplitudeTier peaks, AmplitudeTier valleys, double closingLevelPercentage);

// TODO better name
autoTextTier Electroglottogram_to_TextTier_closedGlottis (Electroglottogram me, double pitchFloor, double PitchCeiling, double closingThreshold, double silenceThreshold, kElectroglottogram_findClosedIntervalMethod method);

autoAmplitudeTier Electroglottogram_to_AmplitudeTier_levels (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, autoAmplitudeTier *out_peaks, autoAmplitudeTier *out_valleys);

autoIntervalTier Electroglottogram_to_TextTier_peaks (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, double silenceThreshold);

autoElectroglottogram Electroglottogram_derivative (Electroglottogram me, double lowPassFrequency, double smoothing);

/* End of file Electroglottogram.h */

#endif
