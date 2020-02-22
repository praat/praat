/* Pitch_extensions.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, 2017,2019 Paul Boersma
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
 djmw 1997
 djmw 20030217 Latest modification
 djmw 20061201 Interface change: removed minimumPitch parameter from PitchTier_modifyRange.
 djmw 20061207 Removed PitchTier_modifyRange.
*/

#include "Pitch_extensions.h"

void Pitch_Frame_addPitch (Pitch_Frame me, double f, double strength, integer maxnCandidates) {
	integer pos = 0;
	double weakest = 1e308;
	if (my nCandidates < maxnCandidates) {
		my candidates. resize (++ my nCandidates);
		pos = my nCandidates;
	} else {
		// Find weakest candidate so far (skip the unvoiced one)
		for (integer i = 1; i <= maxnCandidates; i ++)
			if (my candidates [i]. strength < weakest && my candidates [i]. frequency > 0.0)
				weakest = my candidates [pos = i]. strength;
		if (strength < weakest)
			pos = 0;
	}
	if (pos > 0) {
		my candidates [pos]. frequency = f;
		my candidates [pos]. strength = strength;
	}
}

void Pitch_Frame_getPitch (Pitch_Frame me, double *out_f, double *out_strength) {
	integer pos = 1;
	double strength = -1.0;
	for (integer i = 1; i <= my nCandidates; i ++)
		if (my candidates [i]. strength > strength && my candidates [i]. frequency > 0.0)
			strength = my candidates [pos = i]. strength;
	if (out_f)
		*out_f = my candidates [pos]. frequency;
	if (out_strength)
		*out_strength = strength;
}

void Pitch_Frame_resizeStrengths (Pitch_Frame me, double maxStrength, double unvoicedCriterium) {
	integer pos = 1;
	double strongest = my candidates [1]. strength;
	for (integer i = 2; i <= my nCandidates; i ++)
		if (my candidates [i]. strength > strongest)
			strongest = my candidates [pos = i]. strength;
	if (strongest != 0)
		for (integer i = 1; i <= my nCandidates; i ++)
			my candidates [i]. strength *= maxStrength / strongest;

	if (maxStrength < unvoicedCriterium)
		for (integer i = 1; i <= my nCandidates; i ++)
			if (my candidates [i]. frequency == 0) {
				pos = i;
				break;
			}
	if (pos != 1) {
		std::swap (my candidates [1]. frequency, my candidates [pos]. frequency);
		std::swap (my candidates [1]. strength, my candidates [pos]. strength);
	}
}

autoPitch Pitch_scaleTime (Pitch me, double scaleFactor) {
	try {
		double dx = my dx, x1 = my x1, xmax = my xmax;
		if (scaleFactor != 1.0) {
			dx = my dx * scaleFactor;
			x1 = my xmin + 0.5 * dx;
			xmax = my xmin + my nx * dx;
		}
		autoPitch thee = Pitch_create (my xmin, xmax, my nx, dx, x1, my ceiling, 2);
		for (integer i = 1; i <= my nx; i ++) {
			double f = my frames [i]. candidates [1]. frequency;
			thy frames [i]. candidates [1]. strength = my frames [i]. candidates [1]. strength;
			f /= scaleFactor;
			if (f < my ceiling)
				thy frames [i]. candidates [1]. frequency = f;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not scaled.");
	}
}

static double HertzToSpecial (double value, kPitch_unit pitchUnit) {
	return	( pitchUnit == kPitch_unit::HERTZ ? value :
		( pitchUnit == kPitch_unit::HERTZ_LOGARITHMIC ? ( value <= 0.0 ? undefined : log10 (value)) :
		( pitchUnit == kPitch_unit::MEL ? NUMhertzToMel (value) :
		( pitchUnit == kPitch_unit::LOG_HERTZ ? ( value <= 0.0 ? undefined : log10 (value) ) :
		( pitchUnit == kPitch_unit::SEMITONES_1 ? ( value <= 0.0 ? undefined : 12.0 * log (value / 1.0) / NUMln2 ) :
		( pitchUnit == kPitch_unit::SEMITONES_100 ? ( value <= 0.0 ? undefined : 12.0 * log (value / 100.0) / NUMln2 ) :
		( pitchUnit == kPitch_unit::SEMITONES_200 ? (value <= 0.0 ? undefined : 12.0 * log (value / 200.0) / NUMln2 ) :
		( pitchUnit == kPitch_unit::SEMITONES_440 ? ( value <= 0.0 ? undefined : 12.0 * log (value / 440.0) / NUMln2 ) :
		( pitchUnit == kPitch_unit::ERB ? NUMhertzToErb (value) : undefined ) ) ) ) ) ) ) ) );
}

static double SpecialToHertz (double value, kPitch_unit pitchUnit) {
	return ( pitchUnit == kPitch_unit::HERTZ ? value :
		( pitchUnit == kPitch_unit::HERTZ_LOGARITHMIC ? pow (10.0, value) :
		( pitchUnit == kPitch_unit::MEL ? NUMmelToHertz (value) :
		( pitchUnit == kPitch_unit::LOG_HERTZ ? pow (10.0, value) :
		( pitchUnit == kPitch_unit::SEMITONES_1 ? 1.0 * exp (value * (NUMln2 / 12.0)) :
		( pitchUnit == kPitch_unit::SEMITONES_100 ? 100.0 * exp (value * (NUMln2 / 12.0)) :
		( pitchUnit == kPitch_unit::SEMITONES_200 ? 200.0 * exp (value * (NUMln2 / 12.0)) :
		( pitchUnit == kPitch_unit::SEMITONES_440 ? 440.0 * exp (value * (NUMln2 / 12.0)) :
		( pitchUnit == kPitch_unit::ERB ? NUMerbToHertz (value) : undefined ) ) ) ) ) ) ) ) );
}

autoPitchTier PitchTier_normalizePitchRange (PitchTier me, double pitchMin_ref_Hz, double pitchMax_ref_Hz, double pitchMin_Hz, double pitchMax_Hz, kPitch_unit pitchUnit);
autoPitchTier PitchTier_normalizePitchRange (PitchTier me, double pitchMin_ref_Hz, double pitchMax_ref_Hz, double pitchMin_Hz, double pitchMax_Hz, kPitch_unit pitchUnit) {
	try {
		const double fminr = HertzToSpecial (pitchMin_ref_Hz, pitchUnit);
		const double fmaxr = HertzToSpecial (pitchMax_ref_Hz, pitchUnit);
		const double fmin = HertzToSpecial (pitchMin_Hz, pitchUnit);
		const double fmax = HertzToSpecial (pitchMax_Hz, pitchUnit);

		Melder_require (! (isundef (fminr) || isundef (fmaxr) || isundef (fmin) || isundef (fmax)), 
			U"The conversion of a pitch value is not defined.");
		const double ranger = fmaxr - fminr, range = fmax - fmin;
		Melder_require (ranger >= 0.01 && range >= 0.01,
			U"Pitch range too small.");
		
		const double fmidr = fminr + ranger / 2.0;
		const double factor = ranger / range;
		autoPitchTier thee = Data_copy (me);
		for (integer i = 1; i <= my points.size; i ++) {
			const RealPoint point = thy points.at [i];
			double f = HertzToSpecial (point -> value, pitchUnit);
			f = factor * (f - fmidr);
			f = SpecialToHertz (f, pitchUnit);
			point -> value = f;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PitchTier created.");
	}
}

autoPitch PitchTier_to_Pitch (PitchTier me, double dt, double pitchFloor, double pitchCeiling) {
	try {
		Melder_require (my points.size > 0,
			U"The PitchTier is empty.");
		Melder_require (dt > 0.0,
			U"The time step should be a positive number.");
		Melder_require (pitchFloor < pitchCeiling,
			U"The pitch floor should be lower than the pitch ceiling.");
		
		const double tmin = my xmin, tmax = my xmax, t1 = my xmin + dt / 2.0;
		integer nt = Melder_ifloor ((tmax - tmin - t1) / dt);
		if (t1 + nt * dt < tmax)
			nt ++;
		Melder_require (nt > 0,
			U"Duration is too short.");
		
		autoPitch thee = Pitch_create (tmin, tmax, nt, dt, t1, pitchCeiling, 1);
		for (integer i = 1; i <= nt; i ++) {
			const Pitch_Frame frame = & thy frames [i];
			const Pitch_Candidate candidate = & frame -> candidates [1];
			const double t = t1 + (i - 1) * dt;
			double f = RealTier_getValueAtTime (me, t);
			if (f < pitchFloor || f > pitchCeiling)
				f = 0.0;
			candidate -> frequency = f;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Pitch created.");
	}
}

/* End of file Pitch_extensions.cpp */
