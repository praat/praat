/* PointProcess_and_Sound.cpp
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
 * pb 2003/04/15 improved handling of edges in Sound_getRms
 * pb 2003/07/20 moved shimmer measurements to VoiceAnalysis.c
 * pb 2005/07/07 glottal source signals
 * pb 2006/12/30 new Sound_create API
 * pb 2007/02/25 changed default sampling frequency to 44100 Hz
 * pb 2008/01/19 double
 * pb 2011/06/06 C++
 * pb 2011/06/19 removed a bug in PointProcess_to_Sound_phonation that caused a crash if a pulse came after the time domain
 */

#include "PointProcess_and_Sound.h"

Sound PointProcess_to_Sound_pulseTrain
	(PointProcess me, double samplingFrequency,
	 double adaptFactor, double adaptTime, long interpolationDepth)
{
	try {
		long sound_nt = 1 + floor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		double dt = 1.0 / samplingFrequency;
		double tmid = (my xmin + my xmax) / 2;
		double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		autoSound thee = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
		double *sound = thy z [1];
		for (long it = 1; it <= my nt; it ++) {
			double t = my t [it], amplitude = 0.9, angle, halfampsinangle;
			long mid = Sampled_xToNearestIndex (thee.peek(), t);
			if (it <= 2 || my t [it - 2] < my t [it] - adaptTime) {
				amplitude *= adaptFactor;
				if (it == 1 || my t [it - 1] < my t [it] - adaptTime)
					amplitude *= adaptFactor;
			}
			long begin = mid - interpolationDepth, end = mid + interpolationDepth;
			if (begin < 1) begin = 1;
			if (end > thy nx) end = thy nx;
			angle = NUMpi * (Sampled_indexToX (thee.peek(), begin) - t) / thy dx;
			halfampsinangle = 0.5 * amplitude * sin (angle);
			for (long j = begin; j <= end; j ++) {
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": pulse train not synthesized.");
	}
}

Sound PointProcess_to_Sound_phonation
	(PointProcess me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2)
{
	try {
		long sound_nt = 1 + floor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		double dt = 1.0 / samplingFrequency;
		double tmid = (my xmin + my xmax) / 2;
		double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		double a = (power1 + power2 + 1.0) / (power2 - power1);
		double re = openPhase - collisionPhase;
		autoSound thee = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
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
			for (int i = 1; i <= 50; i ++) {
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
		double *sound = thy z [1];
		for (long it = 1; it <= my nt; it ++) {
			double t = my t [it], amplitude = a;
			double period = NUMundefined, te, phase, flow;
			long midSample = Sampled_xToNearestIndex (thee.peek(), t);
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
			if (it == 1 || my t [it - 1] < my t [it] - maximumPeriod) {
				amplitude *= adaptFactor * adaptFactor;
			} else if (it == 2 || my t [it - 2] < my t [it - 1] - maximumPeriod) {
				amplitude *= adaptFactor;
			}
			/*
			 * Fill in the samples to the left of the current point.
			 */
			{// scope
				long beginSample = midSample - floor (te / thy dx);
				if (beginSample < 1) beginSample = 1;
				long endSample = midSample;
				if (endSample > thy nx) endSample = thy nx;
				for (long isamp = beginSample; isamp <= endSample; isamp ++) {
					double tsamp = thy x1 + (isamp - 1) * thy dx;
					phase = (tsamp - (t - te)) / (period * openPhase);
					if (phase > 0.0)
						sound [isamp] += amplitude * (power1 * pow (phase, power1 - 1.0) - power2 * pow (phase, power2 - 1.0));
				}
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
				long beginSample = midSample + 1;
				if (beginSample < 1) beginSample = 1;
				long endSample = midSample + floor (20 * ta / thy dx);
				if (endSample > thy nx) endSample = thy nx;
				for (long isamp = beginSample; isamp <= endSample; isamp ++) {
					sound [isamp] += value;
					value *= factorPerSample;
				}
			}
		}
		Vector_scale (thee.peek(), 0.9);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound (phonation).");
	}
}

void PointProcess_playPart (PointProcess me, double tmin, double tmax) {
	try {
		autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100, 0.7, 0.05, 30);
		Sound_playPart (sound.peek(), tmin, tmax, NULL, NULL);
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

void PointProcess_play (PointProcess me) {
	PointProcess_playPart (me, my xmin, my xmax);
}

void PointProcess_hum (PointProcess me, double tmin, double tmax) {
	static double formant [1 + 6] = { 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static double bandwidth [1 + 6] = { 0, 50, 100, 200, 300, 400, 500 };
	autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100, 0.7, 0.05, 30);
	Sound_filterWithFormants (sound.peek(), tmin, tmax, 6, formant, bandwidth);
	Sound_playPart (sound.peek(), tmin, tmax, NULL, NULL);
}

Sound PointProcess_to_Sound_hum (PointProcess me) {
	static double formant [1 + 6] = { 0, 600, 1400, 2400, 3400, 4500, 5500 };
	static double bandwidth [1 + 6] = { 0, 50, 100, 200, 300, 400, 500 };
	try {
		autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100, 0.7, 0.05, 30);
		Sound_filterWithFormants (sound.peek(), my xmin, my xmax, 6, formant, bandwidth);
		return sound.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound (hum).");
	}
}

/* End of file PointProcess_and_Sound.cpp */
