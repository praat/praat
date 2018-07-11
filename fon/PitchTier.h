#ifndef _PitchTier_h_
#define _PitchTier_h_
/* PitchTier.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

#include "RealTier.h"
#include "Graphics.h"
#include "Sound.h"
#include "Pitch.h"

/********** class PitchTier **********/

Thing_define (PitchTier, RealTier) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoPitchTier PitchTier_create (double tmin, double tmax);
/*
	Postconditions:
		result -> xmin == tmin;
		result -> xmax == tmax;
		result -> points.size == 0;
*/

void PitchTier_shiftFrequencies (PitchTier me, double tmin, double tmax, double shift, kPitch_unit units);
void PitchTier_multiplyFrequencies (PitchTier me, double tmin, double tmax, double factor);

void PitchTier_draw (PitchTier me, Graphics g, double tmin, double tmax,
	double fmin, double fmax, bool garnish, conststring32 method);

autoPitchTier PointProcess_upto_PitchTier (PointProcess me, double frequency);
void PitchTier_stylize (PitchTier me, double frequencyResolution, bool useSemitones);

void PitchTier_writeToPitchTierSpreadsheetFile (PitchTier me, MelderFile file);
void PitchTier_writeToHeaderlessSpreadsheetFile (PitchTier me, MelderFile file);

/* End of file PitchTier.h */
#endif
