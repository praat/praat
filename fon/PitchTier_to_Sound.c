/* PitchTier_to_Sound.c
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
 * pb 2004/10/03 sine wave generation
 */

#include "PitchTier_to_Sound.h"
#include "PitchTier_to_PointProcess.h"
#include "PointProcess_and_Sound.h"

Sound PitchTier_to_Sound_pulseTrain (PitchTier me, double samplingFrequency,
	 double adaptFactor, double adaptTime, long interpolationDepth, int hum)
{
	static float formant [1 + 6] =
		{ 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static float bandwidth [1 + 6] =
		{ 0, 50, 100, 200, 300, 400, 500 };
	PointProcess point = PitchTier_to_PointProcess (me);
	Sound sound;
	if (! point) return NULL;
	sound = PointProcess_to_Sound (point, samplingFrequency, adaptFactor,
		adaptTime, interpolationDepth);
	if (hum && ! Sound_filterWithFormants (sound, 0, 0, 6, formant, bandwidth)) { forget (point); return NULL; }
	forget (point);
	return sound;
}

int PitchTier_playPart (PitchTier me, double tmin, double tmax, int hum) {
	Sound sound = PitchTier_to_Sound_pulseTrain (me, 22050.0, 0.7, 0.05, 30, hum);
	if (! sound) return 0;
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

int PitchTier_play (PitchTier me) {
	return PitchTier_playPart (me, my xmin, my xmax, FALSE);
}

int PitchTier_hum (PitchTier me) {
	return PitchTier_playPart (me, my xmin, my xmax, TRUE);
}

Sound PitchTier_to_Sound_sine (I, double tmin, double tmax, double samplingFrequency) {
	iam (PitchTier);
	Sound thee = NULL;
	long numberOfSamples, isamp;
	double samplingPeriod, tmid, t1, phase = 0.0;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;
	numberOfSamples = 1 + floor ((my xmax - my xmin) * samplingFrequency);   /* >= 1 */
	samplingPeriod = 1.0 / samplingFrequency;
	tmid = (tmin + tmax) / 2;
	t1 = tmid - 0.5 * (numberOfSamples - 1) * samplingPeriod;
	thee = Sound_create (tmin, tmax, numberOfSamples, samplingPeriod, t1); cherror
	for (isamp = 2; isamp <= numberOfSamples; isamp ++) {
		double tleft = t1 + (isamp - 1.5) * samplingPeriod;
		double fleft = RealTier_getValueAtTime (me, tleft);
		phase += fleft * thy dx;
		thy z [1] [isamp] = 0.5 * sin (2.0 * NUMpi * phase);
	}
end:
	iferror forget (thee);
	return thee;
}

int PitchTier_playPart_sine (I, double tmin, double tmax) {
	iam (PitchTier);
	Sound sound = PitchTier_to_Sound_sine (me, tmin, tmax, 22050.0);
	if (! sound) return Melder_error ("PitchTier_play: not played.");
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

/* End of file PitchTier_to_Sound.c */
