/* PointProcess_and_Sound.c
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
 * pb 2003/04/15 improved handling of edges in Sound_getRms
 * pb 2003/07/20 moved shimmer measurements to VoiceAnalysis.c
 * pb 2005/07/07 glottal source signals
 */

#include "PointProcess_and_Sound.h"

Sound PointProcess_to_Sound_pulseTrain
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

Sound PointProcess_to_Sound_phonation
	(PointProcess me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2)
{
	Sound thee;
	long it;
	long sound_nt = 1 + floor ((my xmax - my xmin) * samplingFrequency);   /* >= 1 */
	double dt = 1.0 / samplingFrequency;
	double tmid = (my xmin + my xmax) / 2;
	double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
	double a = (power1 + power2 + 1.0) / (power2 - power1);
	double re = openPhase - collisionPhase;
	float *sound;
	thee = Sound_create (my xmin, my xmax, sound_nt, dt, t1);
	if (! thee) return NULL;
	/*
	 * Compute "re" by iteration.
	 */
	if (collisionPhase <= 0.0) {
		re = openPhase;
	} else {
		double xmaxFlow = pow (power1 / power2, 1.0 / (power2 - power1));
		double xleft = xmaxFlow;
		double gleft = pow (xleft, power1) - pow (xleft, power2);
		double gderivleft = power1 * pow (xleft, power1 - 1.0) - power2 * pow (xleft, power2 - 1.0);
		double fleft = - gleft / gderivleft;
		double xright = 1.0;
		double gright = pow (xright, power1) - pow (xright, power2);
		double gderivright = power1 * pow (xright, power1 - 1.0) - power2 * pow (xright, power2 - 1.0);
		double fright = - gright / gderivright;
		int i;
		for (i = 1; i <= 50; i ++) {
			double xmid = 0.5 * (xleft + xright);
			double gmid = pow (xmid, power1) - pow (xmid, power2);
			double gderivmid = power1 * pow (xmid, power1 - 1.0) - power2 * pow (xmid, power2 - 1.0);
			double fmid = - gmid / gderivmid;
			if (fmid > collisionPhase / openPhase) {
				xleft = xmid;
				fleft = fmid;
			} else {
				xright = xmid;
				fright = fmid;
			}
			re = xmid * openPhase;
		}
	}
	/*
	 * Cycle through the points. Each will become a period.
	 */
	sound = thy z [1];
	for (it = 1; it <= my nt; it ++) {
		double t = my t [it], amplitude = a;
		double period = NUMundefined, te, phase, flow;
		long midSample = Sampled_xToNearestIndex (thee, t), beginSample, isamp;
		/*
		 * Determine the period: first look left (because that's where the open phase is),
		 * then right.
		 */
		if (it >= 2) {
			period = my t [it] - my t [it - 1];
			if (period > maximumPeriod) {
				period = NUMundefined;
			}
		}
		if (period == NUMundefined) {
			if (it < my nt) {
				period = my t [it + 1] - my t [it];
				if (period > maximumPeriod) {
					period = NUMundefined;
				}
			}
			if (period == NUMundefined) {
				period = 0.5 * maximumPeriod;   /* Some default value. */
			}
		}
		te = re * period;
		/*
		 * Determine the amplitude of this peak.
		 */
		amplitude /= period * openPhase;
		if (it <= 2 || my t [it - 2] < my t [it - 1] - maximumPeriod) {
			amplitude *= adaptFactor;
			if (it == 1 || my t [it - 1] < my t [it] - maximumPeriod)
				amplitude *= adaptFactor;
		}
		/*
		 * Fill in the samples to the left of the current point.
		 */
		beginSample = midSample - floor (te / thy dx);
		if (beginSample < 1) beginSample = 1;
		for (isamp = beginSample; isamp <= midSample; isamp ++) {
			double tsamp = thy x1 + (isamp - 1) * thy dx;
			phase = (tsamp - (t - te)) / (period * openPhase);
			if (phase > 0.0)
				sound [isamp] += amplitude * (power1 * pow (phase, power1 - 1.0) - power2 * pow (phase, power2 - 1.0));
		}
		/*
		 * Determine the signal parameters at the current point.
		 */
		phase = te / (period * openPhase);
		flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));
		/*
		 * Fill in the samples to the right of the current point.
		 */
		if (flow > 0.0) {
			double flowDerivative = amplitude * (power1 * pow (phase, power1 - 1.0) - power2 * pow (phase, power2 - 1.0));
			double ta = - flow / flowDerivative;
			double factorPerSample = exp (- thy dx / ta);
			double value = flowDerivative * factorPerSample;
			long endSample = midSample + floor (20 * ta / thy dx);
			if (endSample > thy nx) endSample = thy nx;
			for (isamp = midSample + 1; isamp <= endSample; isamp ++) {
				sound [isamp] += value;
				value *= factorPerSample;
			}
		}
	}
	Vector_scale (thee, 0.9);
	return thee;
}

int PointProcess_playPart (PointProcess me, double tmin, double tmax) {
	Sound sound = PointProcess_to_Sound_pulseTrain (me, 22050, 0.7, 0.05, 30);
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
	Sound sound = PointProcess_to_Sound_pulseTrain (me, 22050, 0.7, 0.05, 30);
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
	Sound sound = PointProcess_to_Sound_pulseTrain (me, 22050, 0.7, 0.05, 30);
	if (! sound) return 0;
	if (! Sound_filterWithFormants (sound, my xmin, my xmax, 6, formant, bandwidth)) return 0;
	return sound;
}

/* End of file PointProcess_and_Sound.c */
