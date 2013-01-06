/* Intensity_extensions.cpp
 *
 * Copyright (C) 2007-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20061204 Initial version
 djmw 20070129 Extra argument for Vector_get<Maximum/Minimum>AndX
 djmw 20071201 Melder_warning<n>
 djmw 20090818 Intensity_to_TextGrid_detectSilences: mark last interval also if sounding
*/

#include "Intensity_extensions.h"
#include "TextGrid_extensions.h"

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, long iinterval, double time, const wchar_t *leftLabel) {
	if (time <= my xmin || time >= my xmax) {
		Melder_throw ("Time is outside interval.");
	}

	// Find interval to split
	if (iinterval <= 0) {
		iinterval = IntervalTier_timeToLowIndex (me, time);
	}

	// Modify end time of left label
	TextInterval ti = (TextInterval) my intervals -> item[iinterval];
	ti -> xmax = time;
	TextInterval_setText (ti, leftLabel);

	autoTextInterval ti_new = TextInterval_create (time, my xmax, L"");
	Sorted_addItem_unsorted (my intervals, ti_new.transfer());
}

TextGrid Intensity_to_TextGrid_detectSilences (Intensity me, double silenceThreshold_dB,
		double minSilenceDuration, double minSoundingDuration, const wchar_t *silenceLabel, const wchar_t *soundingLabel) {
	try {
		double duration = my xmax - my xmin, time;

		if (silenceThreshold_dB >= 0) {
			Melder_throw ("The silence threshold w.r.t. the maximum intensity should be a negative number.");
		}

		autoTextGrid thee = TextGrid_create (my xmin, my xmax, L"silences", L"");
		IntervalTier it = (IntervalTier) thy tiers -> item[1];
		TextInterval_setText ( (TextInterval) it -> intervals -> item[1], soundingLabel);
		if (minSilenceDuration > duration) {
			return thee.transfer();
		}

		double intensity_max_db, intensity_min_db, xOfMaximum, xOfMinimum;
		Vector_getMaximumAndX (me, 0, 0, 1, NUM_PEAK_INTERPOLATE_PARABOLIC, &intensity_max_db, &xOfMaximum);
		Vector_getMinimumAndX (me, 0, 0, 1, NUM_PEAK_INTERPOLATE_PARABOLIC, &intensity_min_db, &xOfMinimum);
		double intensity_dbRange = intensity_max_db - intensity_min_db;

		if (intensity_dbRange < 10) Melder_warning (L"The loudest and softest part in your sound only differ by ",
			        Melder_double (intensity_dbRange), L" dB.");

		double intensityThreshold = intensity_max_db - fabs (silenceThreshold_dB);

		if (minSilenceDuration > duration || intensityThreshold < intensity_min_db) {
			return thee.transfer();
		}

		int inSilenceInterval = my z[1][1] < intensityThreshold;
		long iinterval = 1;
		const wchar_t *label;
		for (long i = 2; i <= my nx; i++) {
			int addBoundary = 0;
			if (my z[1][i] < intensityThreshold) {
				if (!inSilenceInterval) { // Start of silence
					addBoundary = 1;
					inSilenceInterval = 1;
					label = soundingLabel;
				}
			} else {
				if (inSilenceInterval) { // End of silence
					addBoundary = 1;
					inSilenceInterval = 0;
					label = silenceLabel;
				}
			}

			if (addBoundary) {
				time = my x1 + (i - 1) * my dx;
				IntervalTier_addBoundaryUnsorted (it, iinterval, time, label);
				iinterval++;
			}
		}

		// (re)label last interval */

		label = inSilenceInterval ? silenceLabel : soundingLabel;
		TextInterval_setText ( (TextInterval) it -> intervals -> item[iinterval], label);
		Sorted_sort (it -> intervals);

		// First remove short non-silence intervals in-between silence intervals and
		// then remove the remaining short silence intervals.
		// This works much better than first removing short silence intervals and
		// then short non-silence intervals.

		IntervalTier_cutIntervals_minimumDuration (it, soundingLabel, minSoundingDuration);
		IntervalTier_cutIntervalsOnLabelMatch (it, silenceLabel);
		IntervalTier_cutIntervals_minimumDuration (it, silenceLabel, minSilenceDuration);
		IntervalTier_cutIntervalsOnLabelMatch (it, soundingLabel);

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": TextGrid not created.");
	}
}

Intensity IntensityTier_to_Intensity (IntensityTier me, double dt) {
	try {
		long nt = (my xmax - my xmin) / dt;
		double t1 = 0.5 * dt;
		autoIntensity thee = Intensity_create (my xmin, my xmax, nt, dt, t1);
		for (long i = 1; i <= nt; i++) {
			double time = t1 + (i - 1) * dt;
			thy z[1][i] = RealTier_getValueAtTime (me, time);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, " no Intensity created.");
	}
}

TextGrid IntensityTier_to_TextGrid_detectSilences (IntensityTier me, double dt, double silenceThreshold_dB, double minSilenceDuration,
	double minSoundingDuration, const wchar_t *silenceLabel, const wchar_t *soundingLabel) {
	try {
		autoIntensity intensity = IntensityTier_to_Intensity (me, dt);
		autoTextGrid thee = Intensity_to_TextGrid_detectSilences (intensity.peek(), silenceThreshold_dB, minSilenceDuration, minSoundingDuration, silenceLabel, soundingLabel);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, " no TextGrid created.");
	}
}

/* End of file Intensity_extensions.cpp */
