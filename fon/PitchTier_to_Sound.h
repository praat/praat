/* PitchTier_to_Sound.h
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

#include "PitchTier.h"
#include "Sound.h"

autoSound PitchTier_to_Sound_pulseTrain (PitchTier me, double samplingFrequency,
	 double adaptFactor, double adaptTime, integer interpolationDepth,
	 bool hum);
autoSound PitchTier_to_Sound_phonation (PitchTier me, double samplingFrequency,
	 double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2,
	 bool hum);
void PitchTier_playPart (PitchTier me, double tmin, double tmax, bool hum);
void PitchTier_play (PitchTier me);
void PitchTier_hum (PitchTier me);

autoSound PitchTier_to_Sound_sine (PitchTier me, double tmin, double tmax, double samplingFrequency);
void PitchTier_playPart_sine (PitchTier me, double tmin, double tmax);

/* End of file PitchTier_to_Sound.h */
