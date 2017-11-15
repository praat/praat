/* Pitch_AnyTier_to_PitchTier.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2017 Paul Boersma
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

#include "Pitch_AnyTier_to_PitchTier.h"
#include "Pitch_to_PitchTier.h"

autoPitchTier PitchTier_AnyTier_to_PitchTier (PitchTier pitch, AnyTier tier) {
	try {
		if (pitch -> points.size == 0) Melder_throw (U"No pitch points.");

		/*
		 * Result's domain is a union of both domains.
		 */
		autoPitchTier thee = PitchTier_create (
			pitch -> xmin < tier -> xmin ? pitch -> xmin : tier -> xmin,
			pitch -> xmax > tier -> xmax ? pitch -> xmax : tier -> xmax);

		/*
		 * Copy pitch's frequencies at tier's points to the resulting PitchTier.
		 */
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			AnyPoint point = tier -> points.at [ipoint];
			double time = point -> number;
			double frequency = RealTier_getValueAtTime (pitch, time);
			RealTier_addPoint (thee.get(), time, frequency);
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (pitch, U" & ", tier, U": not converted to PitchTier.");
	}
}

autoPitchTier Pitch_AnyTier_to_PitchTier (Pitch pitch, AnyTier tier, int checkMethod) {
	try {
		if (checkMethod == 2) {
			autoPitchTier temp = Pitch_to_PitchTier (pitch);
			return PitchTier_AnyTier_to_PitchTier (temp.get(), tier);
		}

		/*
		 * Result's domain is a union of both domains.
		 */
		autoPitchTier thee = PitchTier_create (
			pitch -> xmin < tier -> xmin ? pitch -> xmin : tier -> xmin,
			pitch -> xmax > tier -> xmax ? pitch -> xmax : tier -> xmax);

		/*
		 * Copy pitch's frequencies at tier's points to the resulting PitchTier.
		 */
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			AnyPoint point = tier -> points.at [ipoint];
			double time = point -> number;
			double frequency = Pitch_getValueAtTime (pitch, time, kPitch_unit::HERTZ, Pitch_LINEAR);
			if (isundef (frequency) && checkMethod != 0)
				Melder_throw (U"No periodicity at time ", time, U" seconds.");
			RealTier_addPoint (thee.get(), time, frequency);
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (pitch, U" & ", tier, U": not converted to PitchTier.");
	}
}

/* End of file Pitch_AnyTier_to_PitchTier.cpp */
