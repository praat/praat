/* Pitch_extensions.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 1997
 djmw 20030217 Latest modification
 djmw 20061201 Interface change: removed minimumPitch parameter from PitchTier_modifyRange.
 djmw 20061207 Removed PitchTier_modifyRange.
*/

#include "Pitch_extensions.h"

void Pitch_Frame_addPitch (Pitch_Frame me, double f, double strength, int maxnCandidates) {
	long pos = 0;
	double weakest = 1e38;
	if (my nCandidates < maxnCandidates) {
		pos = ++ my nCandidates;
	} else {
		/* Find weakest candidate so far (skip the unvoiced one) */
		for (long i = 1; i <= maxnCandidates; i++) if (my candidate[i].strength < weakest &&
			        my candidate[i].frequency > 0) {
				weakest = my candidate[i].strength;
				pos = i;
			}
		if (strength < weakest) {
			pos = 0;
		}
	}
	if (pos) {
		my candidate[pos].frequency = f;
		my candidate[pos].strength = strength;
	}
}

void Pitch_Frame_getPitch (Pitch_Frame me, double *f, double *strength) {
	long pos = 1;
	*strength = -1;
	for (long i = 1; i <= my nCandidates; i++) if (my candidate[i].strength > *strength &&
		        my candidate[i].frequency > 0) {
			*strength = my candidate[i].strength;
			pos = i;
		}
	*f = my candidate[pos].frequency;
}

void Pitch_Frame_resizeStrengths (Pitch_Frame me, double maxStrength,
                                  double unvoicedCriterium) {
	int pos = 1;
	double strongest = my candidate[1].strength;
	for (long i = 2; i <= my nCandidates; i++) {
		if (my candidate[i].strength > strongest) {
			strongest = my candidate[i].strength;
			pos = i;
		}
	}
	if (strongest != 0) {
		for (long i = 1; i <= my nCandidates; i++) {
			my candidate[i].strength *= maxStrength / strongest;
		}
	}
	if (maxStrength < unvoicedCriterium) {
		for (long i = 1; i <= my nCandidates; i++)
			if (my candidate[i].frequency == 0) {
				pos = i;
				break;
			}
	}
	if (pos != 1) {
		double tmp = my candidate[1].frequency;
		my candidate[1].frequency = my candidate[pos].frequency;
		my candidate[pos].frequency = tmp;
		tmp = my candidate[1].strength;
		my candidate[1].strength = my candidate[pos].strength;
		my candidate[pos].strength = tmp;
	}
}

Pitch Pitch_scaleTime (Pitch me, double scaleFactor) {
	try {
		double dx = my dx, x1 = my x1, xmax = my xmax;
		if (scaleFactor != 1) {
			dx = my dx * scaleFactor;
			x1 = my xmin + 0.5 * dx;
			xmax = my xmin + my nx * dx;
		}
		autoPitch thee = Pitch_create (my xmin, xmax, my nx, dx, x1, my ceiling, 2);
		for (long i = 1; i <= my nx; i++) {
			double f = my frame[i].candidate[1].frequency;
			thy frame[i].candidate[1].strength = my frame[i].candidate[1].strength;
			f /= scaleFactor;
			if (f < my ceiling) {
				thy frame[i].candidate[1].frequency = f;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not scaled.");
	}
}

static double HertzToSpecial (double value, int pitchUnit) {
	return	pitchUnit == kPitch_unit_HERTZ ? value :
	        pitchUnit == kPitch_unit_HERTZ_LOGARITHMIC ? value <= 0.0 ? NUMundefined : log10 (value) :
		        pitchUnit == kPitch_unit_MEL ? NUMhertzToMel (value) :
		        pitchUnit == kPitch_unit_LOG_HERTZ ? value <= 0.0 ? NUMundefined : log10 (value) :
		        pitchUnit == kPitch_unit_SEMITONES_1 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 1.0) / NUMln2 :
		        pitchUnit == kPitch_unit_SEMITONES_100 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 100.0) / NUMln2 :
		        pitchUnit == kPitch_unit_SEMITONES_200 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 200.0) / NUMln2 :
		        pitchUnit == kPitch_unit_SEMITONES_440 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 440.0) / NUMln2 :
		        pitchUnit == kPitch_unit_ERB ? NUMhertzToErb (value) :
		        NUMundefined;
}

static double SpecialToHertz (double value, int pitchUnit) {
	return	pitchUnit == kPitch_unit_HERTZ ? value :
	        pitchUnit == kPitch_unit_HERTZ_LOGARITHMIC ? pow (10.0, value) :
	        pitchUnit == kPitch_unit_MEL ? NUMmelToHertz (value) :
	        pitchUnit == kPitch_unit_LOG_HERTZ ? pow (10.0, value) :
	        pitchUnit == kPitch_unit_SEMITONES_1 ? 1.0 * exp (value * (NUMln2 / 12.0)) :
	        pitchUnit == kPitch_unit_SEMITONES_100 ? 100.0 * exp (value * (NUMln2 / 12.0)) :
	        pitchUnit == kPitch_unit_SEMITONES_200 ? 200.0 * exp (value * (NUMln2 / 12.0)) :
	        pitchUnit == kPitch_unit_SEMITONES_440 ? 440.0 * exp (value * (NUMln2 / 12.0)) :
	        pitchUnit == kPitch_unit_ERB ? NUMerbToHertz (value) : NUMundefined;
}

PitchTier PitchTier_normalizePitchRange (PitchTier me, double pitchMin_ref_Hz, double pitchMax_ref_Hz,
        double pitchMin_Hz, double pitchMax_Hz, int pitchUnit);
PitchTier PitchTier_normalizePitchRange (PitchTier me, double pitchMin_ref_Hz, double pitchMax_ref_Hz,
        double pitchMin_Hz, double pitchMax_Hz, int pitchUnit) {
	try {
		double fminr = HertzToSpecial (pitchMin_ref_Hz, pitchUnit);
		double fmaxr = HertzToSpecial (pitchMax_ref_Hz, pitchUnit);
		double fmin = HertzToSpecial (pitchMin_Hz, pitchUnit);
		double fmax = HertzToSpecial (pitchMax_Hz, pitchUnit);

		if (fminr == NUMundefined || fmaxr == NUMundefined || fmin == NUMundefined || fmax == NUMundefined) {
			Melder_throw ("The conversion of a pitch value is not defined. ");
		}
		double ranger = fmaxr - fminr, range = fmax - fmin;
		if (ranger < 0.01 || range < 0.01) {
			Melder_throw ("Pitch range too small.");
		}
		double fmidr = fminr + ranger / 2;
		double factor = ranger / range;
		autoPitchTier thee = Data_copy (me);
		for (long i = 1; i <= my points -> size; i ++) {
			RealPoint point = (RealPoint) thy points -> item [i];
			double f = HertzToSpecial (point -> value, pitchUnit);
			f = factor * (f - fmidr);
			f = SpecialToHertz (f, pitchUnit);
			point -> value = f;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PitchTier created.");
	}
}

Pitch PitchTier_to_Pitch (PitchTier me, double dt, double pitchFloor, double pitchCeiling) {
	try {
		if (my points -> size < 1) {
			Melder_throw ("The PitchTier is empty.");
		}
		if (dt <= 0) {
			Melder_throw ("The time step should be a positive number.");
		}
		if (pitchFloor >= pitchCeiling) {
			Melder_throw ("The pitch ceiling must be a higher number than the pitch floor.");
		}
		double tmin = my xmin, tmax = my xmax, t1 = my xmin + dt / 2;
		long nt = (tmax - tmin - t1) / dt;
		if (t1 + nt * dt < tmax) {
			nt++;
		}
		if (nt < 1) {
			Melder_throw ("Duration is too short.");
		}
		autoPitch thee = Pitch_create (tmin, tmax, nt, dt, t1, pitchCeiling, 1);
		for (long i = 1; i <= nt; i++) {
			Pitch_Frame frame = (Pitch_Frame) & thy frame [i];
			Pitch_Candidate candidate = (Pitch_Candidate) & frame -> candidate [1];
			double t = t1 + (i - 1) * dt;
			double f = RealTier_getValueAtTime (me, t);
			if (f < pitchFloor || f > pitchCeiling) {
				f = 0;
			}
			candidate -> frequency = f;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Pitch created.");
	}
}

/* End of file Pitch_extensions.cpp */
