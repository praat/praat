/* Pitch_AnyTier_to_PitchTier.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2005/06/16 units
 * pb 2011/06/04 C++
 */

#include "Pitch_AnyTier_to_PitchTier.h"
#include "Pitch_to_PitchTier.h"

PitchTier PitchTier_AnyTier_to_PitchTier (PitchTier pitch, AnyTier tier) {
	try {
		SortedSetOfDouble points = tier -> points;
		if (pitch -> points -> size == 0) Melder_throw ("No pitch points.");

		/*
		 * Result's domain is a union of both domains.
		 */
		autoPitchTier thee = PitchTier_create (
			pitch -> xmin < tier -> xmin ? pitch -> xmin : tier -> xmin,
			pitch -> xmax > tier -> xmax ? pitch -> xmax : tier -> xmax);

		/*
		 * Copy pitch's frequencies at tier's points to the resulting PitchTier.
		 */
		for (long ipoint = 1; ipoint <= points -> size; ipoint ++) {
			AnyPoint point = (AnyPoint) points -> item [ipoint];
			double time = point -> number;
			double frequency = RealTier_getValueAtTime (pitch, time);
			RealTier_addPoint (thee.peek(), time, frequency);
		}

		return thee.peek();
	} catch (MelderError) {
		Melder_throw (pitch, " & ", tier, ": not converted to PitchTier.");
	}
}

PitchTier Pitch_AnyTier_to_PitchTier (Pitch pitch, AnyTier tier, int checkMethod) {
	try {
		SortedSetOfDouble points = tier -> points;
		if (checkMethod == 2) {
			autoPitchTier temp = Pitch_to_PitchTier (pitch);
			autoPitchTier thee = PitchTier_AnyTier_to_PitchTier (temp.peek(), tier);
			return thee.transfer();
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
		for (long ipoint = 1; ipoint <= points -> size; ipoint ++) {
			AnyPoint point = (AnyPoint) points -> item [ipoint];
			double time = point -> number;
			double frequency = Pitch_getValueAtTime (pitch, time, kPitch_unit_HERTZ, Pitch_LINEAR);
			if (frequency == NUMundefined && checkMethod)
				Melder_throw ("No periodicity at time ", time, " seconds.");
			RealTier_addPoint (thee.peek(), time, frequency);
		}

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (pitch, " & ", tier, ": not converted to PitchTier.");
	}
}

/* End of file Pitch_AnyTier_to_PitchTier.cpp */
