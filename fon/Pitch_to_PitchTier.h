/* Pitch_to_PitchTier.h
 *
 * Copyright (C) 1992-2005,2010-2012,2015,2016,2018,2020 Paul Boersma
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

#include "Pitch.h"
#include "PitchTier.h"

autoPitchTier Pitch_to_PitchTier (Pitch me);
/*
	Creates a PitchTier from an existing Pitch contour.
	The best candidate in each frame becomes a target in the result.
	Unvoiced frames are not converted.
*/

void PitchTier_Pitch_draw (PitchTier me, Pitch uv, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	int nonPeriodicLineType, bool garnish, conststring32 method);

autoPitch Pitch_PitchTier_to_Pitch (Pitch me, PitchTier tier);

/* End of file Pitch_to_PitchTier.h */
