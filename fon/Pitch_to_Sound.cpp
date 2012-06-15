/* Pitch_to_Sound.cpp
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
 * pb 2005/02/09 Pitch_to_Sound_sine
 * pb 2007/02/25 changed default sampling frequency to 44100 Hz
 * pb 2008/01/19 double
 * pb 2011/06/05 C++
 */

#include "Pitch_to_PointProcess.h"
#include "PointProcess_and_Sound.h"
#include "Pitch_to_Sound.h"
#include "PitchTier_to_Sound.h"
#include "Pitch_to_PitchTier.h"

Sound Pitch_to_Sound (Pitch me, double tmin, double tmax, int hum) {
	static double formant [1 + 6] = { 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static double bandwidth [1 + 6] = { 0, 50, 100, 200, 300, 400, 500 };
	try {
		autoPointProcess point = Pitch_to_PointProcess (me);
		autoSound sound = PointProcess_to_Sound_pulseTrain (point.peek(), 44100, 0.7, 0.05, 30);
		if (hum) {
			Sound_filterWithFormants (sound.peek(), tmin, tmax, 6, formant, bandwidth);
		}
		return sound.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

void Pitch_play (Pitch me, double tmin, double tmax) {
	try {
		autoSound sound = Pitch_to_Sound (me, tmin, tmax, FALSE);
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // autowindowing
		Sound_playPart (sound.peek(), tmin, tmax, NULL, NULL);
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

void Pitch_hum (Pitch me, double tmin, double tmax) {
	try {
		autoSound sound = Pitch_to_Sound (me, tmin, tmax, TRUE);
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // autowindowing
		Sound_playPart (sound.peek(), tmin, tmax, NULL, NULL);
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

Sound Pitch_to_Sound_sine (Pitch me, double tmin, double tmax, double samplingFrequency, int roundToNearestZeroCrossings) {
	try {
		autoPitchTier tier = Pitch_to_PitchTier (me);
		autoSound sound = PitchTier_to_Sound_sine (tier.peek(), tmin, tmax, samplingFrequency);
		long iframe = 1;
		double unvoicedMin = my xmin;
		double unvoicedMax = my x1 + (iframe - 1.5) * my dx;
		for (;;) {
			while (! Pitch_isVoiced_i (me, iframe)) {
				unvoicedMax = my x1 + (iframe - 0.5) * my dx;
				if (++ iframe > my nx) break;
			}
			if (unvoicedMax > unvoicedMin) {
				Sound_setZero (sound.peek(), unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
			}
			if (iframe > my nx) break;
			while (Pitch_isVoiced_i (me, iframe)) {
				unvoicedMin = my x1 + (iframe - 0.5) * my dx;
				if (++ iframe > my nx) break;
			}
			if (iframe > my nx) break;
		}
		unvoicedMax = my xmax;
		if (unvoicedMax > unvoicedMin) {
			Sound_setZero (sound.peek(), unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
		}
		return sound.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound (sine).");
	}
}

/* End of file Pitch_to_Sound.cpp */
