#ifndef _TextGrid_and_PitchTier_h_
#define _TextGrid_and_PitchTier_h_
/* TextGrid_and_PitchTier.h
 *
 * Copyright (C) 2017 David Weenink
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

#include "TextGrid.h"
#include "PitchTier.h"

void PitchTier_modifyInterval (PitchTier me, double tmin, double tmax,
	conststring32 times_string, int time_offset, conststring32 pitches_string, int pitch_unit, int pitch_as, int pitch_is);

void PitchTier_modifyInterval_toneLevels (PitchTier me, double tmin, double tmax, double fmin, double fmax,
	integer numberOfToneLevels, conststring32 times_string, int time_offset, conststring32 pitches_string);

autoPitchTier IntervalTier_PitchTier_to_PitchTier (IntervalTier me, PitchTier thee,
	conststring32 times_string, int time_offset, conststring32 pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status,
	kMelder_string which, conststring32 criterion);

autoPitchTier TextGrid_PitchTier_to_PitchTier (TextGrid me, integer tierNumber,
	conststring32 times_string, int time_offset, conststring32 pitches_string, int pitch_unit, int pitch_as, int pitchAnchor_status,
	kMelder_string which, conststring32 criterion);

#endif
