/* Pitch_to_PointProcess.c
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
 * pb 2002/07/16 GPL
 * pb 2003/02/26 Sound_Pitch_to_PointProcess_peaks
 * pb 2003/05/17 introduced silence threshold
 * pb 2003/05/20 removed bug in global peak
 * pb 2003/05/22 changed 1.2 to 1.25
 * pb 2004/05/11 undefined pitch is NUMundefined rather than 0.0
 * pb 2004/11/01 Pitch_getVoicedIntervalAfter clips to my xmax
 * pb 2004/11/28 repaired memory leak in Pitch_to_PointProcess
 * pb 2004/11/28 truncated tleft in Pitch_getVoicedIntervalAfter to my xmin (otherwise, getValue can crash)
 */

#include "Pitch_to_PointProcess.h"
#include "PitchTier_to_PointProcess.h"
#include "Pitch_to_PitchTier.h"
#include <ctype.h>

PointProcess Pitch_to_PointProcess (Pitch pitch) {
	PitchTier pitchTier = NULL;
	PointProcess point = NULL;
	pitchTier = Pitch_to_PitchTier (pitch); cherror
	point = PitchTier_Pitch_to_PointProcess (pitchTier, pitch); cherror
end:
	forget (pitchTier);
	iferror forget (point);
	return point;
}

static int Pitch_getVoicedIntervalAfter (Pitch me, double after, double *tleft, double *tright) {
	long ileft = Sampled_xToHighIndex (me, after), iright;
	if (ileft > my nx) return 0;   /* Offright. */
	if (ileft < 1) ileft = 1;   /* Offleft. */

	/* Search for first voiced frame. */
	for (; ileft <= my nx; ileft ++)
		if (Pitch_isVoiced_i (me, ileft)) break;
	if (ileft > my nx) return 0;   /* Offright. */

	/* Search for last voiced frame. */
	for (iright = ileft; iright <= my nx; iright ++)
		if (! Pitch_isVoiced_i (me, iright)) break;
	iright --;

	*tleft = Sampled_indexToX (me, ileft) - 0.5 * my dx;   /* The whole frame is considered voiced. */
	*tright = Sampled_indexToX (me, iright) + 0.5 * my dx;
	if (*tleft >= my xmax - 0.5 * my dx) return 0;
	if (*tleft < my xmin) *tleft = my xmin;
	if (*tright > my xmax) *tright = my xmax;
	return 1;
}

static double findExtremum_3 (float *x, long n, int includeMaxima, int includeMinima) {
	int includeAll = includeMaxima == includeMinima;
	long imin = 1, imax = 1, i, iextr;
	double minimum, maximum;
	if (n < 3) {
		if (n <= 0) return 0.0;   /* Outside. */
		else if (n == 1) return 1.0;
		else {   /* n == 2 */
			double xleft = includeAll ? fabs (x [1]) : includeMaxima ? x [1] : - x [1];
			double xright = includeAll ? fabs (x [2]) : includeMaxima ? x [2] : - x [2];
			if (xleft > xright) return 1.0;
			else if (xleft < xright) return 2.0;
			else return 1.5;
		}
	}
	minimum = maximum = x [1];
	for (i = 2; i <= n; i ++) {
		if (x [i] < minimum) { minimum = x [i]; imin = i; }
		if (x [i] > maximum) { maximum = x [i]; imax = i; }
	}
	if (minimum == maximum) {
		return 0.5 * (n + 1.0);   /* All equal. */
	}
	iextr = includeAll ? ( fabs (minimum) > fabs (maximum) ? imin : imax ) : includeMaxima ? imax : imin;
	if (iextr == 1) return 1.0;
	if (iextr == n) return (double) n;
	/* Parabolic interpolation. */
	/* We do NOT need fabs here: we look for a genuine extremum. */
	return iextr + 0.5 * (x [iextr + 1] - x [iextr - 1]) / (2 * x [iextr] - x [iextr - 1] - x [iextr + 1]);
}

static double Sound_findExtremum (Sound me, double tmin, double tmax, int includeMaxima, int includeMinima) {
	long imin = Sampled_xToLowIndex (me, tmin), imax = Sampled_xToHighIndex (me, tmax);
	double iextremum;
	Melder_assert (NUMdefined (tmin));
	Melder_assert (NUMdefined (tmax));
	if (imin < 1) imin = 1;
	if (imax > my nx) imax = my nx;
	iextremum = findExtremum_3 (my z [1] + imin - 1, imax - imin + 1, includeMaxima, includeMinima);
	if (iextremum)
		return my x1 + (imin - 1 + iextremum - 1) * my dx;
	else
		return (tmin + tmax) / 2;
}

static double Sound_findMaximumCorrelation (Sound me, double t1, double windowLength, double tmin2, double tmax2, double *tout, double *peak) {
	double maximumCorrelation = -1.0, r1 = 0.0, r2 = 0.0, r3 = 0.0, r1_best, r3_best, ir;
	double halfWindowLength = 0.5 * windowLength;
	long i1, i2, ileft2;
	long ileft1 = Sampled_xToNearestIndex ((Sampled) me, t1 - halfWindowLength);
	long iright1 = Sampled_xToNearestIndex ((Sampled) me, t1 + halfWindowLength);
	long ileft2min = Sampled_xToLowIndex ((Sampled) me, tmin2 - halfWindowLength);
	long ileft2max = Sampled_xToHighIndex ((Sampled) me, tmax2 - halfWindowLength);
	float *amp = my z [1];
	*peak = 0.0;   /* Default. */
	for (ileft2 = ileft2min; ileft2 <= ileft2max; ileft2 ++) {
		double norm1 = 0.0, norm2 = 0.0, product = 0.0, localPeak = 0.0;
		for (i1 = ileft1, i2 = ileft2; i1 <= iright1; i1 ++, i2 ++) {
			if (i1 < 1 || i1 > my nx || i2 < 1 || i2 > my nx) continue;
			norm1 += amp [i1] * amp [i1];
			norm2 += amp [i2] * amp [i2];
			product += amp [i1] * amp [i2];
			if (fabs (amp [i2]) > localPeak)
				localPeak = fabs (amp [i2]);
		}
		r1 = r2;
		r2 = r3;
		r3 = product ? product / (sqrt (norm1 * norm2)) : 0.0;
		if (r2 > maximumCorrelation && r2 >= r1 && r2 >= r3) {
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
	if (maximumCorrelation > -1.0) {
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

PointProcess Sound_Pitch_to_PointProcess_cc (Sound sound, Pitch pitch) {
	PointProcess point = PointProcess_create (sound -> xmin, sound -> xmax, 10);
	double t = pitch -> xmin;
	double addedRight = -1e300;
	double globalPeak = Vector_getAbsoluteExtremum (sound, sound -> xmin, sound -> xmax, 0), peak;
	
	/*
	 * Cycle over all voiced intervals.
	 */
	for (;;) {
		double tleft, tright, tmiddle, f0middle, tmax, tsave;
		if (! Pitch_getVoicedIntervalAfter (pitch, t, & tleft, & tright)) break;
		/*
		 * Go to the middle of the voice stretch.
		 */
		tmiddle = (tleft + tright) / 2;
		if (! Melder_progress ((tmiddle - sound -> xmin) / (sound -> xmax - sound -> xmin), "Sound & Pitch to PointProcess"))
			goto end;
		f0middle = Pitch_getValueAtTime (pitch, tmiddle, Pitch_HERTZ, Pitch_LINEAR);

		/*
		 * Our first point is near this middle.
		 */
		if (f0middle == NUMundefined) {
			Melder_fatal ("Sound_Pitch_to_PointProcess_cc: tleft %s, tright %s, f0middle %s",
				Melder_double (tleft), Melder_double (tright), Melder_double (f0middle));
		}
		tmax = Sound_findExtremum (sound, tmiddle - 0.5 / f0middle, tmiddle + 0.5 / f0middle, TRUE, TRUE);
		Melder_assert (NUMdefined (tmax));
		if (! PointProcess_addPoint (point, tmax)) goto end;

		tsave = tmax;
		for (;;) {
			double f0 = Pitch_getValueAtTime (pitch, tmax, Pitch_HERTZ, Pitch_LINEAR), correlation;
			if (f0 == NUMundefined) break;
			correlation = Sound_findMaximumCorrelation (sound, tmax, 1.0 / f0, tmax - 1.25 / f0, tmax - 0.8 / f0, & tmax, & peak);
			if (correlation == -1) /*break*/ tmax -= 1.0 / f0;   /* This one period will drop out. */
			if (tmax < tleft) {
				if (correlation > 0.7 && peak > 0.023333 * globalPeak && tmax - addedRight > 0.8 / f0)
					if (! PointProcess_addPoint (point, tmax)) goto end;
				break;
			}
			if (correlation > 0.3 && (peak == 0.0 || peak > 0.01 * globalPeak)) {
				if (tmax - addedRight > 0.8 / f0)   /* Do not fill in a short originally unvoiced interval twice. */
					if (! PointProcess_addPoint (point, tmax)) goto end;
			}
		}
		tmax = tsave;
		for (;;) {
			double f0 = Pitch_getValueAtTime (pitch, tmax, Pitch_HERTZ, Pitch_LINEAR), correlation;
			if (f0 == NUMundefined) break;
			correlation = Sound_findMaximumCorrelation (sound, tmax, 1.0 / f0, tmax + 0.8 / f0, tmax + 1.25 / f0, & tmax, & peak);
			if (correlation == -1) /*break*/ tmax += 1.0 / f0;
			if (tmax > tright) {
				if (correlation > 0.7 && peak > 0.023333 * globalPeak) {
					if (! PointProcess_addPoint (point, tmax)) goto end;
					addedRight = tmax;
				}
				break;
			}
			if (correlation > 0.3 && (peak == 0.0 || peak > 0.01 * globalPeak)) {
				if (! PointProcess_addPoint (point, tmax)) goto end;
				addedRight = tmax;
			}
		}
		t = tright;
	}
end:
	Melder_progress (1.0, NULL);
	iferror forget (point);
	return point;
}
	

PointProcess Sound_Pitch_to_PointProcess_peaks (Sound sound, Pitch pitch, int includeMaxima, int includeMinima) {
	PointProcess point = PointProcess_create (sound -> xmin, sound -> xmax, 10);
	double t = pitch -> xmin;
	double addedRight = -1e300;
	/*
	 * Cycle over all voiced intervals.
	 */

	for (;;) {
		double tleft, tright, tmiddle, f0middle, tmax, tsave;
		if (! Pitch_getVoicedIntervalAfter (pitch, t, & tleft, & tright)) break;
		/*
		 * Go to the middle of the voiced interval.
		 */
		tmiddle = (tleft + tright) / 2;
		if (! Melder_progress ((tmiddle - sound -> xmin) / (sound -> xmax - sound -> xmin), "Sound & Pitch to PointProcess"))
			goto end;
		f0middle = Pitch_getValueAtTime (pitch, tmiddle, Pitch_HERTZ, Pitch_LINEAR);

		/*
		 * Our first point is near this middle.
		 */
		Melder_assert (NUMdefined (f0middle));
		tmax = Sound_findExtremum (sound, tmiddle - 0.5 / f0middle, tmiddle + 0.5 / f0middle, includeMaxima, includeMinima);
		Melder_assert (NUMdefined (tmax));
		if (! PointProcess_addPoint (point, tmax)) goto end;

		tsave = tmax;
		for (;;) {
			double f0 = Pitch_getValueAtTime (pitch, tmax, Pitch_HERTZ, Pitch_LINEAR);
			if (f0 == NUMundefined) break;
			tmax = Sound_findExtremum (sound, tmax - 1.25 / f0, tmax - 0.8 / f0, includeMaxima, includeMinima);
			if (tmax < tleft) {
				if (tmax - addedRight > 0.8 / f0)
					if (! PointProcess_addPoint (point, tmax)) goto end;
				break;
			}
			if (tmax - addedRight > 0.8 / f0)   /* Do not fill in a short originally unvoiced interval twice. */
				if (! PointProcess_addPoint (point, tmax)) goto end;
		}
		tmax = tsave;
		for (;;) {
			double f0 = Pitch_getValueAtTime (pitch, tmax, Pitch_HERTZ, Pitch_LINEAR);
			if (f0 == NUMundefined) break;
			tmax = Sound_findExtremum (sound, tmax + 0.8 / f0, tmax + 1.25 / f0, includeMaxima, includeMinima);
			if (tmax > tright) {
				if (! PointProcess_addPoint (point, tmax)) goto end;
				addedRight = tmax;
				break;
			}
			if (! PointProcess_addPoint (point, tmax)) goto end;
			addedRight = tmax;
		}
		t = tright;
	}
end:
	Melder_progress (1.0, NULL);
	iferror forget (point);
	return point;
}
	
/* End of file Pitch_to_PointProcess.c */
