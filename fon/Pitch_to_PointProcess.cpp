/* Pitch_to_PointProcess.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2014-2018,2020 Paul Boersma
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
 * pb 2003/02/26 Sound_Pitch_to_PointProcess_peaks
 * pb 2003/05/17 introduced silence threshold
 * pb 2003/05/20 removed bug in global peak
 * pb 2003/05/22 changed 1.2 to 1.25
 * pb 2004/05/11 undefined pitch is `undefined` rather than 0.0
 * pb 2004/11/01 Pitch_getVoicedIntervalAfter clips to my xmax
 * pb 2004/11/28 repaired memory leak in Pitch_to_PointProcess
 * pb 2004/11/28 truncated tleft in Pitch_getVoicedIntervalAfter to my xmin (otherwise, getValue can crash)
 * pb 2005/06/16 units
 * pb 2007/01/26 compatible with stereo sounds
 * pb 2008/01/19 double
 * pb 2011/06/04 C++
 */

#include "Pitch_to_PointProcess.h"
#include "PitchTier_to_PointProcess.h"
#include "Pitch_to_PitchTier.h"

autoPointProcess Pitch_to_PointProcess (Pitch pitch) {
	try {
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch);
		autoPointProcess point = PitchTier_Pitch_to_PointProcess (pitchTier.get(), pitch);
		return point;
	} catch (MelderError) {
		Melder_throw (pitch, U": not converted to PointProcess.");
	}
}

static int Pitch_getVoicedIntervalAfter (Pitch me, double after, double *tleft, double *tright) {
	integer ileft = Sampled_xToHighIndex (me, after), iright;
	if (ileft > my nx) return 0;   // offright
	if (ileft < 1) ileft = 1;   // offleft

	/* Search for first voiced frame. */
	for (; ileft <= my nx; ileft ++)
		if (Pitch_isVoiced_i (me, ileft)) break;
	if (ileft > my nx) return 0;   // offright

	/* Search for last voiced frame. */
	for (iright = ileft; iright <= my nx; iright ++)
		if (! Pitch_isVoiced_i (me, iright)) break;
	iright --;

	*tleft = Sampled_indexToX (me, ileft) - 0.5 * my dx;   // the whole frame is considered voiced
	*tright = Sampled_indexToX (me, iright) + 0.5 * my dx;
	if (*tleft >= my xmax - 0.5 * my dx) return 0;
	if (*tleft < my xmin) *tleft = my xmin;
	if (*tright > my xmax) *tright = my xmax;
	if (*tright <= after) return 0;
	return 1;
}

static double findExtremum_3 (double *channel1_base, double *channel2_base, integer d, integer n, int includeMaxima, int includeMinima) {
	double *channel1 = channel1_base + d, *channel2 = channel2_base ? channel2_base + d : nullptr;
	int includeAll = includeMaxima == includeMinima;
	integer imin = 1, imax = 1, i, iextr;
	double minimum, maximum;
	if (n < 3) {
		if (n <= 0) return 0.0;   // outside
		else if (n == 1) return 1.0;
		else {   // n == 2
			double x1 = channel2 ? 0.5 * (channel1 [1] + channel2 [1]) : channel1 [1];
			double x2 = channel2 ? 0.5 * (channel1 [2] + channel2 [2]) : channel1 [2];
			double xleft = includeAll ? fabs (x1) : includeMaxima ? x1 : - x1;
			double xright = includeAll ? fabs (x2) : includeMaxima ? x2 : - x2;
			if (xleft > xright) return 1.0;
			else if (xleft < xright) return 2.0;
			else return 1.5;
		}
	}
	minimum = maximum = channel2 ? 0.5 * (channel1 [1] + channel2 [1]) : channel1 [1];
	for (i = 2; i <= n; i ++) {
		double value = channel2 ? 0.5 * (channel1 [i] + channel2 [i]) : channel1 [i];
		if (value < minimum) { minimum = value; imin = i; }
		if (value > maximum) { maximum = value; imax = i; }
	}
	if (minimum == maximum) {
		return 0.5 * (n + 1.0);   // all equal
	}
	iextr = includeAll ? ( fabs (minimum) > fabs (maximum) ? imin : imax ) : includeMaxima ? imax : imin;
	if (iextr == 1) return 1.0;
	if (iextr == n) return (double) n;
	/* Parabolic interpolation. */
	/* We do NOT need fabs here: we look for a genuine extremum. */
	double valueMid = channel2 ? 0.5 * (channel1 [iextr] + channel2 [iextr]) : channel1 [iextr];
	double valueLeft = channel2 ? 0.5 * (channel1 [iextr - 1] + channel2 [iextr - 1]) : channel1 [iextr - 1];
	double valueRight = channel2 ? 0.5 * (channel1 [iextr + 1] + channel2 [iextr + 1]) : channel1 [iextr + 1];
	return iextr + 0.5 * (valueRight - valueLeft) / (2 * valueMid - valueLeft - valueRight);
}

static double Sound_findExtremum (Sound me, double tmin, double tmax, int includeMaxima, int includeMinima) {
	integer imin = Sampled_xToLowIndex (me, tmin), imax = Sampled_xToHighIndex (me, tmax);
	Melder_assert (isdefined (tmin));
	Melder_assert (isdefined (tmax));
	if (imin < 1) imin = 1;
	if (imax > my nx) imax = my nx;
	double iextremum = findExtremum_3 (& my z [1] [0], my ny > 1 ? & my z [2] [0] : nullptr, imin - 1, imax - imin + 1, includeMaxima, includeMinima);
	if (iextremum != 0.0)
		return my x1 + (imin - 1 + iextremum - 1) * my dx;
	else
		return (tmin + tmax) / 2;
}

static double Sound_findMaximumCorrelation (Sound me, double t1, double windowLength, double tmin2, double tmax2, double *tout, double *peak) {
	double maximumCorrelation = -1.0;   // smart 'impossible' starting value
	double r1_best = undefined, r3_best = undefined, ir = undefined;   // assignments not necessary, but extra safe
	double r1 = 0.0, r2 = 0.0, r3 = 0.0;
	double halfWindowLength = 0.5 * windowLength;
	integer ileft1 = Sampled_xToNearestIndex ((Sampled) me, t1 - halfWindowLength);
	integer iright1 = Sampled_xToNearestIndex ((Sampled) me, t1 + halfWindowLength);
	integer ileft2min = Sampled_xToLowIndex ((Sampled) me, tmin2 - halfWindowLength);
	integer ileft2max = Sampled_xToHighIndex ((Sampled) me, tmax2 - halfWindowLength);
	*peak = 0.0;   // default
	Melder_assert (ileft2max >= ileft2min);   // if the loop is never executed, the result will be garbage
	for (integer ileft2 = ileft2min; ileft2 <= ileft2max; ileft2 ++) {
		double norm1 = 0.0, norm2 = 0.0, product = 0.0, localPeak = 0.0;
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			for (integer i1 = ileft1, i2 = ileft2; i1 <= iright1; i1 ++, i2 ++) {
				if (i1 < 1 || i1 > my nx || i2 < 1 || i2 > my nx) continue;
				double amp1 = my z [ichan] [i1], amp2 = my z [ichan] [i2];
				norm1 += amp1 * amp1;
				norm2 += amp2 * amp2;
				product += amp1 * amp2;
				if (fabs (amp2) > localPeak)
					localPeak = fabs (amp2);
			}
		}
		r1 = r2;   // >= 0
		r2 = r3;   // >= 0
		r3 = ( product != 0.0 ? product / (sqrt (norm1 * norm2)) : 0.0 );   // >= 0
		if (r2 > maximumCorrelation /* true on first test */ && r2 >= r1 && r2 >= r3) {
			r1_best = r1;
			maximumCorrelation = r2;
			r3_best = r3;
			ir = ileft2 - 1;
			*peak = localPeak;  
		}
	}
	/*
	 * Improve the result by means of parabolic interpolation.
	 */
	if (maximumCorrelation > -1.0) {   // was maximumCorrelation ever assigned to?...
		// ...then r1_best and r3_best and ir must also have been assigned to:
		Melder_assert (isdefined (r1_best) && isdefined (r3_best) && isdefined (ir));
		double d2r = 2 * maximumCorrelation - r1_best - r3_best;
		if (d2r != 0.0) {
			double dr = 0.5 * (r3_best - r1_best);
			maximumCorrelation += 0.5 * dr * dr / d2r;
			ir += dr / d2r;
		}
		*tout = t1 + (ir - ileft1) * my dx;
	}
	return maximumCorrelation;
}

autoPointProcess Sound_Pitch_to_PointProcess_cc (Sound sound, Pitch pitch) {
	try {
		autoPointProcess point = PointProcess_create (sound -> xmin, sound -> xmax, 10);
		double t = pitch -> xmin;
		double addedRight = -1e308;
		const double globalPeak = Vector_getAbsoluteExtremum (sound, sound -> xmin, sound -> xmax, kVector_peakInterpolation :: NONE);
		
		/*
			Cycle over all voiced intervals.
		*/
		autoMelderProgress progress (U"Sound & Pitch: To PointProcess...");
		for (;;) {
			double tleft, tright;
			if (! Pitch_getVoicedIntervalAfter (pitch, t, & tleft, & tright))
				break;
			Melder_assert (tright > t);

			/*
			 * Go to the middle of the voice stretch.
			 */
			const double tmiddle = (tleft + tright) / 2.0;
			Melder_progress ((tmiddle - sound -> xmin) / (sound -> xmax - sound -> xmin), U"Sound & Pitch to PointProcess");
			const double f0middle = Pitch_getValueAtTime (pitch, tmiddle, kPitch_unit::HERTZ, Pitch_LINEAR);

			/*
				Our first point is near this middle.
			*/
			if (isundef (f0middle)) {
				Melder_fatal (U"Sound_Pitch_to_PointProcess_cc:"
					U" tleft ", tleft,
					U", tright ", tright,
					U", f0middle ", f0middle
				);
			}
			double tmax = Sound_findExtremum (sound, tmiddle - 0.5 / f0middle, tmiddle + 0.5 / f0middle, true, true);
			Melder_assert (isdefined (tmax));
			PointProcess_addPoint (point.get(), tmax);

			double tsave = tmax;
			for (;;) {
				const double f0 = Pitch_getValueAtTime (pitch, tmax, kPitch_unit::HERTZ, Pitch_LINEAR);
				if (isundef (f0))
					break;
				double peak;
				const double correlation = Sound_findMaximumCorrelation (sound, tmax, 1.0 / f0, tmax - 1.25 / f0, tmax - 0.8 / f0, & tmax, & peak);
				if (correlation == -1.0)
					/*break*/ tmax -= 1.0 / f0;   // this one period will drop out
				if (tmax < tleft) {
					if (correlation > 0.7 && peak > 0.023333 * globalPeak && tmax - addedRight > 0.8 / f0)
						PointProcess_addPoint (point.get(), tmax);
					break;
				}
				if (correlation > 0.3 && (peak == 0.0 || peak > 0.01 * globalPeak)) {
					if (tmax - addedRight > 0.8 / f0) {   // do not fill in a short originally unvoiced interval twice
						PointProcess_addPoint (point.get(), tmax);
					}
				}
			}
			tmax = tsave;
			for (;;) {
				const double f0 = Pitch_getValueAtTime (pitch, tmax, kPitch_unit::HERTZ, Pitch_LINEAR);
				if (isundef (f0))
					break;
				double peak;
				const double correlation = Sound_findMaximumCorrelation (sound, tmax, 1.0 / f0, tmax + 0.8 / f0, tmax + 1.25 / f0, & tmax, & peak);
				if (correlation == -1.0)
					/*break*/ tmax += 1.0 / f0;
				if (tmax > tright) {
					if (correlation > 0.7 && peak > 0.023333 * globalPeak) {
						PointProcess_addPoint (point.get(), tmax);
						addedRight = tmax;
					}
					break;
				}
				if (correlation > 0.3 && (peak == 0.0 || peak > 0.01 * globalPeak)) {
					PointProcess_addPoint (point.get(), tmax);
					addedRight = tmax;
				}
			}
			t = tright;
		}
		return point;
	} catch (MelderError) {
		Melder_throw (sound, U" & ", pitch, U": not converted to PointProcess (cc).");
	}
}

autoPointProcess Sound_Pitch_to_PointProcess_peaks (Sound sound, Pitch pitch, int includeMaxima, int includeMinima) {
	try {
		autoPointProcess point = PointProcess_create (sound -> xmin, sound -> xmax, 10);
		double t = pitch -> xmin;
		double addedRight = -1e308;
		/*
			Cycle over all voiced intervals.
		*/

		autoMelderProgress progress (U"Sound & Pitch: To PointProcess");
		for (;;) {
			double tleft, tright;
			if (! Pitch_getVoicedIntervalAfter (pitch, t, & tleft, & tright))
				break;
			/*
				Go to the middle of the voiced interval.
			*/
			const double tmiddle = (tleft + tright) / 2.0;
			Melder_progress ((tmiddle - sound -> xmin) / (sound -> xmax - sound -> xmin), U"Sound & Pitch: To PointProcess");
			const double f0middle = Pitch_getValueAtTime (pitch, tmiddle, kPitch_unit::HERTZ, Pitch_LINEAR);

			/*
				Our first point is near this middle.
			*/
			Melder_assert (isdefined (f0middle));
			double tmax = Sound_findExtremum (sound, tmiddle - 0.5 / f0middle, tmiddle + 0.5 / f0middle, includeMaxima, includeMinima);
			Melder_assert (isdefined (tmax));
			PointProcess_addPoint (point.get(), tmax);

			double tsave = tmax;
			for (;;) {
				const double f0 = Pitch_getValueAtTime (pitch, tmax, kPitch_unit::HERTZ, Pitch_LINEAR);
				if (isundef (f0))
					break;
				tmax = Sound_findExtremum (sound, tmax - 1.25 / f0, tmax - 0.8 / f0, includeMaxima, includeMinima);
				if (tmax < tleft) {
					if (tmax - addedRight > 0.8 / f0)
						PointProcess_addPoint (point.get(), tmax);
					break;
				}
				if (tmax - addedRight > 0.8 / f0)   // do not fill in a short originally unvoiced interval twice
					PointProcess_addPoint (point.get(), tmax);
			}
			tmax = tsave;
			for (;;) {
				const double f0 = Pitch_getValueAtTime (pitch, tmax, kPitch_unit::HERTZ, Pitch_LINEAR);
				if (isundef (f0))
					break;
				tmax = Sound_findExtremum (sound, tmax + 0.8 / f0, tmax + 1.25 / f0, includeMaxima, includeMinima);
				if (tmax > tright) {
					PointProcess_addPoint (point.get(), tmax);
					addedRight = tmax;
					break;
				}
				PointProcess_addPoint (point.get(), tmax);
				addedRight = tmax;
			}
			t = tright;
		}
		return point;
	} catch (MelderError) {
		Melder_throw (sound, U" & ", pitch, U": not converted to PointProcess (peaks).");
	}
}
	
/* End of file Pitch_to_PointProcess.cpp */
