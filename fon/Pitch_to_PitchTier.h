/* Pitch_to_PitchTier.h
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

#include "Pitch.h"
#include "PitchTier.h"

PitchTier Pitch_to_PitchTier (Pitch me);
/*
	Creates a PitchTier from an existing Pitch contour.
	The best candidate in each frame becomes a target in the result.
	Unvoiced frames are not converted.
*/

void PitchTier_Pitch_draw (PitchTier me, Pitch uv, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	int nonPeriodicLineType, int garnish, const wchar_t *method);

Pitch Pitch_PitchTier_to_Pitch (Pitch me, PitchTier tier);

/* End of file Pitch_to_PitchTier.h */
