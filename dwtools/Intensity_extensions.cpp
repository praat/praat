/* Intensity_extensions.cpp
 *
 * Copyright (C) 2007-2019 David Weenink, 2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20061204 Initial version
 djmw 20070129 Extra argument for Vector_get<Maximum/Minimum>AndX
 djmw 20071201 Melder_warning<n>
 djmw 20090818 Intensity_to_TextGrid_detectSilences: mark last interval also if sounding
*/

#include "Intensity_extensions.h"
#include "TextGrid_extensions.h"

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, integer iinterval, double time, conststring32 leftLabel) {
	Melder_require (time > my xmin && time < my xmax,
		U"Time is outside interval.");
	/*
		Find interval to split.
	*/
	if (iinterval <= 0)
		iinterval = IntervalTier_timeToLowIndex (me, time);

	/*
		Modify end time of left label.
	*/
	const TextInterval ti = my intervals.at [iinterval];
	ti -> xmax = time;
	TextInterval_setText (ti, leftLabel);

	autoTextInterval ti_new = TextInterval_create (time, my xmax, U"");
	my intervals. addItem_unsorted_move (ti_new.move());
}

autoTextGrid Intensity_to_TextGrid_detectSilences (Intensity me,
	double silenceThreshold_dB, double minSilenceDuration, double minSoundingDuration,
	conststring32 silenceLabel, conststring32 soundingLabel)
{
	try {
		const double duration = my xmax - my xmin;

		Melder_require (silenceThreshold_dB < 0.0,
			U"The silence threshold w.r.t. the maximum intensity should be a negative number.");

		autoTextGrid thee = TextGrid_create (my xmin, my xmax, U"silences", U"");
		const IntervalTier it = (IntervalTier) thy tiers->at [1];
		TextInterval_setText (it -> intervals.at [1], soundingLabel);
		if (minSilenceDuration > duration)
			return thee;

		double intensity_max_db, intensity_min_db, xOfMaximum, xOfMinimum;
		Vector_getMaximumAndX (me, 0.0, 0.0, 1, kVector_peakInterpolation :: PARABOLIC, & intensity_max_db, & xOfMaximum);
		Vector_getMinimumAndX (me, 0.0, 0.0, 1, kVector_peakInterpolation :: PARABOLIC, & intensity_min_db, & xOfMinimum);
		double intensity_dbRange = intensity_max_db - intensity_min_db;

		if (intensity_dbRange < 10.0)
			Melder_warning (U"The loudest and softest part in your sound differ by only ", intensity_dbRange, U" dB.");

		const double intensityThreshold = intensity_max_db - fabs (silenceThreshold_dB);

		if (minSilenceDuration > duration || intensityThreshold < intensity_min_db)
			return thee;

		bool inSilenceInterval = my z [1] [1] < intensityThreshold;
		integer iinterval = 1;
		conststring32 label;
		for (integer i = 2; i <= my nx; i ++) {
			bool addBoundary = false;
			if (my z [1] [i] < intensityThreshold) {
				if (! inSilenceInterval) {   // start of silence
					addBoundary = true;
					inSilenceInterval = true;
					label = soundingLabel;
				}
			} else {
				if (inSilenceInterval) {   // end of silence
					addBoundary = true;
					inSilenceInterval = false;
					label = silenceLabel;
				}
			}

			if (addBoundary) {
				const double time = my x1 + (i - 1) * my dx;
				IntervalTier_addBoundaryUnsorted (it, iinterval, time, label);
				iinterval ++;
			}
		}
		/*
			(re)label last interval.
		*/
		label = inSilenceInterval ? silenceLabel : soundingLabel;
		TextInterval_setText (it -> intervals.at [iinterval], label);
		it -> intervals. sort ();

		/*
			First remove short non-silence intervals in-between silence intervals and
			then remove the remaining short silence intervals.
			This works much better than first removing short silence intervals and
			then short non-silence intervals.
		*/
		IntervalTier_cutIntervals_minimumDuration (it, soundingLabel, minSoundingDuration);
		IntervalTier_cutIntervalsOnLabelMatch (it, silenceLabel);
		IntervalTier_cutIntervals_minimumDuration (it, silenceLabel, minSilenceDuration);
		IntervalTier_cutIntervalsOnLabelMatch (it, soundingLabel);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": TextGrid not created.");
	}
}

autoIntensity IntensityTier_to_Intensity (IntensityTier me, double dt) {
	try {
		const integer nt = Melder_ifloor ((my xmax - my xmin) / dt);
		const double t1 = 0.5 * dt;
		autoIntensity thee = Intensity_create (my xmin, my xmax, nt, dt, t1);
		for (integer i = 1; i <= nt; i ++) {
			const double time = t1 + (i - 1) * dt;
			thy z [1] [i] = RealTier_getValueAtTime (me, time);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Intensity not created.");
	}
}

autoTextGrid IntensityTier_to_TextGrid_detectSilences (IntensityTier me, double dt, double silenceThreshold_dB, double minSilenceDuration,
	double minSoundingDuration, conststring32 silenceLabel, conststring32 soundingLabel) {
	try {
		autoIntensity intensity = IntensityTier_to_Intensity (me, dt);
		autoTextGrid thee = Intensity_to_TextGrid_detectSilences (intensity.get(), silenceThreshold_dB, minSilenceDuration, minSoundingDuration, silenceLabel, soundingLabel);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": TextGrid not created.");
	}
}

/* End of file Intensity_extensions.cpp */
