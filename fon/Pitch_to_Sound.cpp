/* Pitch_to_Sound.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2017 Paul Boersma
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

autoSound Pitch_to_Sound (Pitch me, double tmin, double tmax, bool hum) {
	static double formant [1 + 6] { 0.0, 600.0, 1400.0, 2400.0, 3400.0, 4500.0, 5500.0 };
	static double bandwidth [1 + 6] { 0.0, 50.0, 100.0, 200.0, 300.0, 400.0, 500.0 };
	try {
		autoPointProcess point = Pitch_to_PointProcess (me);
		autoSound sound = PointProcess_to_Sound_pulseTrain (point.get(), 44100.0, 0.7, 0.05, 30);
		if (hum) {
			Sound_filterWithFormants (sound.get(), tmin, tmax, 6, formant, bandwidth);
		}
		return sound;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

void Pitch_play (Pitch me, double tmin, double tmax) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		autoSound sound = Pitch_to_Sound (me, tmin, tmax, false);
		Sound_playPart (sound.get(), tmin, tmax, nullptr, nullptr);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void Pitch_hum (Pitch me, double tmin, double tmax) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		autoSound sound = Pitch_to_Sound (me, tmin, tmax, true);
		Sound_playPart (sound.get(), tmin, tmax, nullptr, nullptr);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

autoSound Pitch_to_Sound_sine (Pitch me, double tmin, double tmax, double samplingFrequency, bool roundToNearestZeroCrossings) {
	try {
		autoPitchTier tier = Pitch_to_PitchTier (me);
		autoSound sound = PitchTier_to_Sound_sine (tier.get(), tmin, tmax, samplingFrequency);
		integer iframe = 1;
		double unvoicedMin = my xmin;
		double unvoicedMax = my x1 + (iframe - 1.5) * my dx;
		for (;;) {
			while (! Pitch_isVoiced_i (me, iframe)) {
				unvoicedMax = my x1 + (iframe - 0.5) * my dx;
				if (++ iframe > my nx) break;
			}
			if (unvoicedMax > unvoicedMin) {
				Sound_setZero (sound.get(), unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
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
			Sound_setZero (sound.get(), unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
		}
		return sound;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound (sine).");
	}
}

/* End of file Pitch_to_Sound.cpp */
