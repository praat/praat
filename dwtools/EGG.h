#ifndef _EGG_h_
#define _EGG_h_
/* EGG.h
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

#include "EGG_enums.h"

Thing_define (EGG, Vector) {
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

autoEGG EGG_create (double xmin, double xmax, integer nx, double dx, double x1);

autoEGG Sound_extractEGG (Sound me, integer channelNumber);

autoAmplitudeTier EGG_and_AmplitudeTiers_getLevels (EGG me, AmplitudeTier peaks, AmplitudeTier valleys, double closingLevelPercentage);

Thing_define (DEGG, Vector) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoDEGG DEGG_create (double xmin, double xmax, integer nx, double dx, double x1);

// TODO better name
autoTextTier EGG_to_TextTier_closedGlottis (EGG me, double pitchFloor, double PitchCeiling, double closingThreshold, double silenceThreshold, kEGG_findClosedIntervalMethod method);

autoIntervalTier EGG_to_TextTier_peaks (EGG me, double pitchFloor, double pitchCeiling, double closingThreshold, double silenceThreshold);

autoTextTier EGG_and_DEGG_to_TextTier (EGG me, DEGG thee, double pitchFloor, double PitchCeiling, double closingThreshold, double silenceThreshold);

autoDEGG EGG_to_DEGG (EGG me, double fromFrequency, double toFrequency, double smoothing);

/* End of file EGG.h */

#endif
