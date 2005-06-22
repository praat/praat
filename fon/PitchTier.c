/* PitchTier.c
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
 * pb 2003/05/31 PointProcess_upto_RealTier
 * pb 2004/10/03 moved audio to PitchTier_to_Sound.c
 * pb 2005/06/16 units
 */

#include "PitchTier.h"
#include "Pitch.h"

class_methods (PitchTier, RealTier)
class_methods_end

PitchTier PitchTier_create (double tmin, double tmax) {
	PitchTier me = new (PitchTier);
	if (! me || ! RealTier_init (me, tmin, tmax)) { forget (me); return NULL; }
	return me;
}

void PitchTier_draw (PitchTier me, Graphics g, double tmin, double tmax,
	double fmin, double fmax, int garnish)
{
	RealTier_draw (me, g, tmin, tmax, fmin, fmax, garnish, "Frequency (Hz)");
}

PitchTier PointProcess_upto_PitchTier (PointProcess me, double frequency) {
	PitchTier thee = (PitchTier) PointProcess_upto_RealTier (me, frequency);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classPitchTier);
	return thee;
}

void PitchTier_stylize (PitchTier me, double frequencyResolution, int useSemitones) {
	double dfmin;
	for (;;) {
		long i, imin = 0;
		dfmin = 1e300;
		for (i = 2; i <= my points -> size - 1; i ++) {
			RealPoint pm = my points -> item [i];
			RealPoint pl = my points -> item [i - 1];
			RealPoint pr = my points -> item [i + 1];
			double expectedFrequency = pl -> value + (pr -> value - pl -> value) /
				 (pr -> time - pl -> time) * (pm -> time - pl -> time);
			double df = useSemitones ?
				12 * fabs (log (pm -> value / expectedFrequency)) / NUMln2:
				fabs (pm -> value - expectedFrequency);
			if (df < dfmin) {
				imin = i;
				dfmin = df;
			}
		}
		if (imin == 0 || dfmin > frequencyResolution) break;
		Collection_removeItem (my points, imin);
	}
}

static int PitchTier_writeToSpreadsheetFile (PitchTier me, MelderFile fs, int hasHeader) {
	FILE *f = Melder_fopen (fs, "w");
	long i;
	if (! f) return 0;
	if (hasHeader)
		fprintf (f, "\"ooTextFile\"\n\"PitchTier\"\n%.17g %.17g %ld\n", my xmin, my xmax, my points -> size);
	for (i = 1; i <= my points -> size; i ++) {
		RealPoint point = my points -> item [i];
		fprintf (f, "%.17g\t%.17g\n", point -> time, point -> value);
	}
	if (! Melder_fclose (fs, f)) return 0;
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

int PitchTier_writeToPitchTierSpreadsheetFile (PitchTier me, MelderFile fs) {
	return PitchTier_writeToSpreadsheetFile (me, fs, TRUE);
}

int PitchTier_writeToHeaderlessSpreadsheetFile (PitchTier me, MelderFile fs) {
	return PitchTier_writeToSpreadsheetFile (me, fs, FALSE);
}

int PitchTier_shiftFrequencies (PitchTier me, double tmin, double tmax, double shift, int unit) {
	long i;
	for (i = 1; i <= my points -> size; i ++) {
		RealPoint point = my points -> item [i];
		double frequency = point -> value;
		if (point -> time < tmin || point -> time > tmax) continue;
		switch (unit) {
			case Pitch_UNIT_HERTZ: {	
				frequency += shift;
				if (frequency <= 0.0) {
					Melder_error ("Resulting frequency has to be greater than 0 Hz.");
					goto end;
				}
			} break; case Pitch_UNIT_MEL: {
				frequency = NUMhertzToMel (frequency) + shift;
				if (frequency <= 0.0) {
					Melder_error ("Resulting frequency has to be greater than 0 mel.");
					goto end;
				}
				frequency = NUMmelToHertz (frequency);
			} break; case Pitch_UNIT_LOG_HERTZ: {
				frequency = pow (10.0, log10 (frequency) + shift);
			} break; case Pitch_UNIT_SEMITONES_1: {
				frequency = NUMsemitonesToHertz (NUMhertzToSemitones (frequency) + shift);
			} break; case Pitch_UNIT_ERB: {
				frequency = NUMhertzToErb (frequency) + shift;
				if (frequency <= 0.0) {
					Melder_error ("Resulting frequency has to be greater than 0 ERB.");
					goto end;
				}
				frequency = NUMerbToHertz (frequency);
			}
		}
		point -> value = frequency;
	}
end:
	iferror return Melder_error ("(PitchTier_shiftFrequency:) Not completed.");
	return 1;
}

void PitchTier_multiplyFrequencies (PitchTier me, double tmin, double tmax, double factor) {
	long i;
	Melder_assert (factor > 0.0);
	for (i = 1; i <= my points -> size; i ++) {
		RealPoint point = my points -> item [i];
		if (point -> time < tmin || point -> time > tmax) continue;
		point -> value *= factor;
	}
}

/* End of file PitchTier.c */
