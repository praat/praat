/* PointProcess_and_Sound.c
 *
 * Copyright (C) 1992-2003 Paul Boersma
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
 * pb 2001/03/09
 * pb 2002/07/16 GPL
 * pb 2003/04/15 improved handling of edges in Sound_getRms
 * pb 2003/07/20 moved shimmer measurements to VoiceAnalysis.c
 */

#include "PointProcess_and_Sound.h"

Sound PointProcess_to_Sound
	(PointProcess me, double samplingFrequency,
	 double adaptFactor, double adaptTime, long interpolationDepth)
{
	Sound thee;
	long it;
	long sound_nt = 1 + floor ((my xmax - my xmin) * samplingFrequency);   /* >= 1 */
	double dt = 1.0 / samplingFrequency;
	double tmid = (my xmin + my xmax) / 2;
	double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
	float *sound;
	thee = Sound_create (my xmin, my xmax, sound_nt, dt, t1);
	if (! thee) return NULL;
	sound = thy z [1];
	for (it = 1; it <= my nt; it ++) {
		double t = my t [it], amplitude = 0.9, angle, halfampsinangle;
		long mid = Sampled_xToNearestIndex (thee, t), j;
		long begin = mid - interpolationDepth, end = mid + interpolationDepth;
		if (it <= 2 || my t [it - 2] < my t [it] - adaptTime) {
			amplitude *= adaptFactor;
			if (it == 1 || my t [it - 1] < my t [it] - adaptTime)
				amplitude *= adaptFactor;
		}
		if (begin < 1) begin = 1;
		if (end > thy nx) end = thy nx;
		angle = NUMpi * (Sampled_indexToX (thee, begin) - t) / thy dx;
		halfampsinangle = 0.5 * amplitude * sin (angle);
		for (j = begin; j <= end; j ++) {
			if (fabs (angle) < 1e-6)
				sound [j] += amplitude;
			else if (angle < 0.0)
				sound [j] += halfampsinangle *
					(1 + cos (angle / (mid - begin + 1))) / angle;
			else
				sound [j] += halfampsinangle *
					(1 + cos (angle / (end - mid + 1))) / angle;
			angle += NUMpi;
			halfampsinangle = - halfampsinangle;
		}
	}
	return thee;
}

int PointProcess_playPart (PointProcess me, double tmin, double tmax) {
	Sound sound = PointProcess_to_Sound (me, 22050, 0.7, 0.05, 30);
	if (! sound) return 0;
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

int PointProcess_play (PointProcess me) {
	return PointProcess_playPart (me, my xmin, my xmax);
}

int PointProcess_hum (PointProcess me, double tmin, double tmax) {
	static float formant [1 + 6] =
		{ 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static float bandwidth [1 + 6] =
		{ 0, 50, 100, 200, 300, 400, 500 };
	Sound sound = PointProcess_to_Sound (me, 22050, 0.7, 0.05, 30);
	if (! sound) return 0;
	if (! Sound_filterWithFormants (sound, tmin, tmax, 6, formant, bandwidth)) return 0;
	Sound_playPart (sound, tmin, tmax, NULL, NULL);
	forget (sound);
	return 1;
}

Sound PointProcess_to_Sound_hum (PointProcess me) {
	static float formant [1 + 6] =
		{ 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static float bandwidth [1 + 6] =
		{ 0, 50, 100, 200, 300, 400, 500 };
	Sound sound = PointProcess_to_Sound (me, 22050, 0.7, 0.05, 30);
	if (! sound) return 0;
	if (! Sound_filterWithFormants (sound, my xmin, my xmax, 6, formant, bandwidth)) return 0;
	return sound;
}

/* End of file PointProcess_and_Sound.c */
