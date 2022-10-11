/* PointProcess_and_Sound.cpp
 *
 * Copyright (C) 1992-2012,2014-2018,2021 Paul Boersma
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

#include "PointProcess_and_Sound.h"

autoSound PointProcess_to_Sound_pulseTrain
	(PointProcess me, double samplingFrequency,
	 double adaptFactor, double adaptTime, integer interpolationDepth)
{
	try {
		const integer sound_nt = 1 + Melder_ifloor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		const double dt = 1.0 / samplingFrequency;
		const double tmid = 0.5 * (my xmin + my xmax);
		const double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		autoSound thee = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
		const VEC sound = thy z.row (1);
		for (integer it = 1; it <= my nt; it ++) {
			const double t = my t [it];
			double amplitude = 0.9;
			integer mid = Sampled_xToNearestIndex (thee.get(), t);
			if (it <= 2 || my t [it - 2] < my t [it] - adaptTime) {
				amplitude *= adaptFactor;
				if (it == 1 || my t [it - 1] < my t [it] - adaptTime)
					amplitude *= adaptFactor;
			}
			integer begin = mid - interpolationDepth, end = mid + interpolationDepth;
			if (begin < 1) begin = 1;
			if (end > thy nx) end = thy nx;
			double angle = NUMpi * (Sampled_indexToX (thee.get(), begin) - t) / thy dx;
			double halfampsinangle = 0.5 * amplitude * sin (angle);
			for (integer j = begin; j <= end; j ++) {
				if (fabs (angle) < 1e-6)
					sound [j] += amplitude;
				else if (angle < 0.0)
					sound [j] += halfampsinangle *
						(1.0 + cos (angle / (mid - begin + 1))) / angle;
				else
					sound [j] += halfampsinangle *
						(1.0 + cos (angle / (end - mid + 1))) / angle;
				angle += NUMpi;
				halfampsinangle = - halfampsinangle;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": pulse train not synthesized.");
	}
}

autoSound PointProcess_to_Sound_phonation
	(PointProcess me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2)
{
	try {
		const integer sound_nt = 1 + Melder_ifloor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		const double dt = 1.0 / samplingFrequency;
		const double tmid = 0.5 * (my xmin + my xmax);
		const double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		const double a = (power1 + power2 + 1.0) / (power2 - power1);
		autoSound thee = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
		/*
			Compute "re" by iteration.
		*/
		double re = openPhase - collisionPhase;
		if (collisionPhase <= 0.0) {
			re = openPhase;
		} else {
			const double xmaxFlow = pow (power1 / power2, 1.0 / (power2 - power1));
			/* mutable by iteration */ double xleft = xmaxFlow;
			/* mutable by iteration */ double xright = 1.0;
			for (int i = 1; i <= 50; i ++) {
				const double xmid = 0.5 * (xleft + xright);
				const double gmid = pow (xmid, power1) - pow (xmid, power2);
				const double gderivmid = power1 * pow (xmid, power1 - 1.0) - power2 * pow (xmid, power2 - 1.0);
				const double fmid = - gmid / gderivmid;
				if (fmid > collisionPhase / openPhase)
					xleft = xmid;
				else
					xright = xmid;
				re = xmid * openPhase;
			}
		}
		/*
			Cycle through the points. Each will become a period.
		*/
		VEC sound = thy z.row (1);
		for (integer it = 1; it <= my nt; it ++) {
			const double t = my t [it];
			const integer midSample = Sampled_xToNearestIndex (thee.get(), t);
			/*
				Determine the period: first look left (because that's where the open phase is),
				then right.
			*/
			double period = undefined;
			if (it >= 2) {
				period = my t [it] - my t [it - 1];
				if (period > maximumPeriod)
					period = undefined;
			}
			if (isundef (period)) {
				if (it < my nt) {
					period = my t [it + 1] - my t [it];
					if (period > maximumPeriod)
						period = undefined;
				}
				if (isundef (period))
					period = 0.5 * maximumPeriod;   // some default value
			}
			const double te = re * period;
			/*
				Determine the amplitude of this peak.
			*/
			const double unadaptedAmplitude = a / (period * openPhase);
			const double amplitude = unadaptedAmplitude * (
				it == 1 || my t [it - 1] < my t [it] - maximumPeriod ?
					adaptFactor * adaptFactor
				: it == 2 || my t [it - 2] < my t [it - 1] - maximumPeriod ?
					adaptFactor
				:
					1.0
			);
			/*
				Fill in the samples to the left of the current point.
			*/
			{// scope
				const integer beginSample = Melder_clippedLeft (1_integer, midSample - Melder_ifloor (te / thy dx));
				const integer endSample = Melder_clippedRight (midSample, thy nx);
				for (integer isamp = beginSample; isamp <= endSample; isamp ++) {
					const double tsamp = thy x1 + (isamp - 1) * thy dx;
					const double phase = (tsamp - (t - te)) / (period * openPhase);
					if (phase > 0.0)
						sound [isamp] += amplitude * (power1 * pow (phase, power1 - 1.0) - power2 * pow (phase, power2 - 1.0));
				}
			}
			/*
				Determine the signal parameters at the current point.
			*/
			const double phase = te / (period * openPhase);
			const double flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));
			/*
				Fill in the samples to the right of the current point.
			*/
			if (flow > 0.0) {
				const double flowDerivative = amplitude * (power1 * pow (phase, power1 - 1.0) - power2 * pow (phase, power2 - 1.0));
				const double ta = - flow / flowDerivative;
				const double factorPerSample = exp (- thy dx / ta);
				const integer beginSample = Melder_clippedLeft (1_integer, midSample + 1);
				const integer endSample = Melder_clippedRight (midSample + Melder_ifloor (20.0 * ta / thy dx), thy nx);
				double value = flowDerivative * factorPerSample;
				for (integer isamp = beginSample; isamp <= endSample; isamp ++) {
					sound [isamp] += value;
					value *= factorPerSample;
				}
			}
		}
		Vector_scale (thee.get(), 0.9);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound (phonation).");
	}
}

void PointProcess_playPart (PointProcess me, double tmin, double tmax, Sound_PlayCallback callback, Thing boss) {
	try {
		autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100.0, 0.7, 0.05, 30);
		Sound_playPart (sound.get(), tmin, tmax, callback, boss);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void PointProcess_play (PointProcess me, Sound_PlayCallback callback, Thing boss) {
	PointProcess_playPart (me, my xmin, my xmax, callback, boss);
}

void PointProcess_hum (PointProcess me, double tmin, double tmax, Sound_PlayCallback callback, Thing boss) {
	static double formant [1 + 6] = { 0, 600.0, 1400.0, 2400.0, 3400.0, 4500.0, 5500.0 };
	static double bandwidth [1 + 6] = { 0, 50.0, 100.0, 200.0, 300.0, 400.0, 500.0 };
	autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100, 0.7, 0.05, 30);
	Sound_filterWithFormants (sound.get(), tmin, tmax, 6, formant, bandwidth);
	Sound_playPart (sound.get(), tmin, tmax, callback, boss);
}

autoSound PointProcess_to_Sound_hum (PointProcess me) {
	static double formant [1 + 6] = { 0, 600.0, 1400.0, 2400.0, 3400.0, 4500.0, 5500.0 };
	static double bandwidth [1 + 6] = { 0, 50.0, 100.0, 200.0, 300.0, 400.0, 500.0 };
	try {
		autoSound sound = PointProcess_to_Sound_pulseTrain (me, 44100.0, 0.7, 0.05, 30);
		Sound_filterWithFormants (sound.get(), my xmin, my xmax, 6, formant, bandwidth);
		return sound;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound (hum).");
	}
}

/* End of file PointProcess_and_Sound.cpp */
