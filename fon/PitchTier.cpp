/* PitchTier.cpp
 *
 * Copyright (C) 1992-2008,2010-2013,2015-2018,2021,2022,2024 Paul Boersma
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

#include "PitchTier.h"

Thing_implement (PitchTier, RealTier, 0);

void structPitchTier :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Number of points: ", points.size);
	MelderInfo_writeLine (U"Minimum pitch value: ", RealTier_getMinimumValue (this), U" Hz");
	MelderInfo_writeLine (U"Maximum pitch value: ", RealTier_getMaximumValue (this), U" Hz");
}

autoPitchTier PitchTier_create (double tmin, double tmax) {
	try {
		autoPitchTier me = Thing_new (PitchTier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PitchTier not created.");
	}
}

void PitchTier_draw (PitchTier me, Graphics g, double tmin, double tmax,
	double fmin, double fmax, bool garnish, conststring32 method)
{
	RealTier_draw (me, g, tmin, tmax, fmin, fmax, garnish, method, U"Frequency (Hz)");
}

autoPitchTier PointProcess_upto_PitchTier (PointProcess me, double frequency) {
	try {
		autoPitchTier thee = PointProcess_upto_RealTier (me, frequency, classPitchTier).static_cast_move <structPitchTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PitchTier.");
	}
}

void PitchTier_stylize (PitchTier me, double frequencyResolution, bool useSemitones) {
	for (;;) {
		integer imin = 0;
		double dfmin = 1e308;
		for (integer i = 2; i <= my points.size - 1; i ++) {
			RealPoint pm = my points.at [i];
			RealPoint pl = my points.at [i - 1];
			RealPoint pr = my points.at [i + 1];
			double expectedFrequency = pl -> value + (pr -> value - pl -> value) /
				 (pr -> number - pl -> number) * (pm -> number - pl -> number);
			double df = useSemitones ?
				12 * fabs (log (pm -> value / expectedFrequency)) / NUMln2:
				fabs (pm -> value - expectedFrequency);
			if (df < dfmin) {
				imin = i;
				dfmin = df;
			}
		}
		if (imin == 0 || dfmin > frequencyResolution) break;
		my points. removeItem (imin);
	}
}

static void PitchTier_writeToSpreadsheetFile (PitchTier me, MelderFile file, bool hasHeader) {
	autofile f = Melder_fopen (file, "w");
	if (hasHeader)
		fprintf (f, "\"ooTextFile\"\n\"PitchTier\"\n%s %s %s\n",
			Melder8_double (my xmin), Melder8_double (my xmax), Melder8_integer (my points.size));
	for (integer i = 1; i <= my points.size; i ++) {
		RealPoint point = my points.at [i];
		fprintf (f, "%.17g\t%.17g\n", point -> number, point -> value);
	}
	f.close (file);
}

void PitchTier_writeToPitchTierSpreadsheetFile (PitchTier me, MelderFile file) {
	try {
		PitchTier_writeToSpreadsheetFile (me, file, true);
	} catch (MelderError) {
		Melder_throw (me, U" not written to tab-separated PitchTier file.");
	}
}

void PitchTier_writeToHeaderlessSpreadsheetFile (PitchTier me, MelderFile file) {
	try {
		PitchTier_writeToSpreadsheetFile (me, file, false);
	} catch (MelderError) {
		Melder_throw (me, U" not written to tab-separated table file.");
	}
}

void PitchTier_shiftFrequencies (PitchTier me, double tmin, double tmax, double shift, kPitch_unit unit) {
	try {
		for (integer i = 1; i <= my points.size; i ++) {
			RealPoint point = my points.at [i];
			double frequency = point -> value;
			if (point -> number < tmin || point -> number > tmax) continue;
			switch (unit) {
				case kPitch_unit::HERTZ: {
					frequency += shift;
					if (frequency <= 0.0)
						Melder_throw (U"The resulting frequency has to be greater than 0 Hz.");
				} break; case kPitch_unit::MEL: {
					frequency = NUMhertzToMel (frequency) + shift;
					if (frequency <= 0.0)
						Melder_throw (U"The resulting frequency has to be greater than 0 mel.");
					frequency = NUMmelToHertz (frequency);
				} break; case kPitch_unit::LOG_HERTZ: {
					frequency = pow (10.0, log10 (frequency) + shift);
				} break; case kPitch_unit::SEMITONES_1: {
					frequency = NUMsemitonesToHertz (NUMhertzToSemitones (frequency) + shift);
				} break; case kPitch_unit::ERB: {
					frequency = NUMhertzToErb (frequency) + shift;
					if (frequency <= 0.0)
						Melder_throw (U"The resulting frequency has to be greater than 0 ERB.");
					frequency = NUMerbToHertz (frequency);
				}
			}
			point -> value = frequency;
		}
	} catch (MelderError) {
		Melder_throw (me, U": not all frequencies were shifted.");
	}
}

void PitchTier_multiplyFrequencies (PitchTier me, double tmin, double tmax, double factor) {
	Melder_assert (factor > 0.0);
	for (integer i = 1; i <= my points.size; i ++) {
		RealPoint point = my points.at [i];
		if (point -> number < tmin || point -> number > tmax) continue;
		point -> value *= factor;
	}
}

autoPitchTier RealTier_to_PitchTier (RealTier me) {
	try {
		autoPitchTier thee = Thing_new (PitchTier);
		my structRealTier :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PitchTier.");
	}
}

/* End of file PitchTier.cpp */
