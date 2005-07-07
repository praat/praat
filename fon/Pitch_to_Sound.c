/* Pitch_to_Sound.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/07/07
 */

#include "Pitch_to_PointProcess.h"
#include "PointProcess_and_Sound.h"
#include "Pitch_to_Sound.h"
#include "PitchTier_to_Sound.h"
#include "Pitch_to_PitchTier.h"

Sound Pitch_to_Sound (I, double tmin, double tmax, int hum) {
	iam (Pitch);
	static float formant [1 + 6] =
		{ 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static float bandwidth [1 + 6] =
		{ 0, 50, 100, 200, 300, 400, 500 };
	PointProcess point = NULL;
	Sound sound = NULL;
	if (! (point = Pitch_to_PointProcess (me))) goto error;
	if (! (sound = PointProcess_to_Sound_pulseTrain (point, 22050, 0.7, 0.05, 30))) goto error;
	if (hum && ! Sound_filterWithFormants (sound, tmin, tmax, 6, formant, bandwidth)) goto error;
	forget (point);
	return sound;
error:
	forget (point);
	forget (sound);
	return Melder_errorp ("Pitch_to_Sound: not performed.");
}

int Pitch_play (I, double tmin, double tmax) {
	iam (Pitch);
	Sound sound = Pitch_to_Sound (me, tmin, tmax, FALSE);
	if (! sound) return Melder_error ("Pitch_play: not played.");
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

int Pitch_hum (I, double tmin, double tmax) {
	iam (Pitch);
	Sound sound = Pitch_to_Sound (me, tmin, tmax, TRUE);
	if (! sound) return Melder_error ("Pitch_hum: not played.");
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

Sound Pitch_to_Sound_sine (Pitch me, double tmin, double tmax, double samplingFrequency, int roundToNearestZeroCrossings) {
	PitchTier tier = NULL;
	Sound sound = NULL;
	double unvoicedMin, unvoicedMax;
	long iframe;

	tier = Pitch_to_PitchTier (me); cherror
	sound = PitchTier_to_Sound_sine (tier, tmin, tmax, samplingFrequency); cherror
	iframe = 1;
	unvoicedMin = my xmin;
	unvoicedMax = my x1 + (iframe - 1.5) * my dx;
	for (;;) {
		while (! Pitch_isVoiced_i (me, iframe)) {
			unvoicedMax = my x1 + (iframe - 0.5) * my dx;
			if (++ iframe > my nx) break;
		}
		if (unvoicedMax > unvoicedMin) {
			Sound_setZero (sound, unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
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
		Sound_setZero (sound, unvoicedMin, unvoicedMax, roundToNearestZeroCrossings);
	}
end:
	forget (tier);
	iferror forget (sound);
	return sound;
}

/* End of file Pitch_to_Sound.c */
