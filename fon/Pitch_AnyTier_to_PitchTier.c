/* Pitch_AnyTier_to_PitchTier.c
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
 * pb 2005/06/16 units
 */

#include "Pitch_AnyTier_to_PitchTier.h"
#include "Pitch_to_PitchTier.h"

PitchTier PitchTier_AnyTier_to_PitchTier (PitchTier pitch, AnyTier tier) {
	SortedSetOfDouble points = tier -> points;
	long ipoint;
	PitchTier result = NULL;
	if (pitch -> points -> size == 0) return Melder_errorp ("No pitch points.");

	/*
	 * Result's domain is a union of both domains.
	 */
	result = PitchTier_create (
		pitch -> xmin < tier -> xmin ? pitch -> xmin : tier -> xmin,
		pitch -> xmax > tier -> xmax ? pitch -> xmax : tier -> xmax);
	if (! result) goto end;

	/*
	 * Copy pitch's frequencies at tier's points to the resulting PitchTier.
	 */
	for (ipoint = 1; ipoint <= points -> size; ipoint ++) {
		AnyPoint point = points -> item [ipoint];
		double time = point -> time;
		double frequency = RealTier_getValueAtTime (pitch, time);
		if (! RealTier_addPoint (result, time, frequency)) goto end;
	}

end:
	if (Melder_hasError ()) {
		forget (result);
		Melder_error1 (L"(Pitch_AnyTier_to_PitchTier:) Not performed.");
	}
	return result;
}

PitchTier Pitch_AnyTier_to_PitchTier (Pitch pitch, AnyTier tier, int checkMethod) {
	SortedSetOfDouble points = tier -> points;
	long ipoint;
	PitchTier result = NULL;
	if (checkMethod == 2) {
		PitchTier temp = Pitch_to_PitchTier (pitch);
		if (! temp) goto end;
		result = PitchTier_AnyTier_to_PitchTier (temp, tier);
		forget (temp);
		goto end;
	}

	/*
	 * Result's domain is a union of both domains.
	 */
	result = PitchTier_create (
		pitch -> xmin < tier -> xmin ? pitch -> xmin : tier -> xmin,
		pitch -> xmax > tier -> xmax ? pitch -> xmax : tier -> xmax);
	if (! result) goto end;

	/*
	 * Copy pitch's frequencies at tier's points to the resulting PitchTier.
	 */
	for (ipoint = 1; ipoint <= points -> size; ipoint ++) {
		AnyPoint point = points -> item [ipoint];
		double time = point -> time;
		double frequency = Pitch_getValueAtTime (pitch, time, Pitch_UNIT_HERTZ, Pitch_LINEAR);
		if (frequency == NUMundefined && checkMethod)
			error3 (L"No periodicity at time ", Melder_single (time), L" seconds.")
		if (! RealTier_addPoint (result, time, frequency)) goto end;
	}

end:
	iferror {
		forget (result);
		Melder_error1 (L"(Pitch_AnyTier_to_PitchTier:) Not performed.");
	}
	return result;
}

/* End of file Pitch_AnyTier_to_PitchTier.c */
