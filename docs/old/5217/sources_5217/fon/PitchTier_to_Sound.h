/* PitchTier_to_Sound.h
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
 * pb 2005/07/08
 */

#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

Sound PitchTier_to_Sound_pulseTrain (PitchTier me, double samplingFrequency,
	 double adaptFactor, double adaptTime, long interpolationDepth,
	 int hum);
Sound PitchTier_to_Sound_phonation (PitchTier me, double samplingFrequency,
	 double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2,
	 int hum);
int PitchTier_playPart (PitchTier me, double tmin, double tmax, int hum);
int PitchTier_play (PitchTier me);
int PitchTier_hum (PitchTier me);

Sound PitchTier_to_Sound_sine (I, double tmin, double tmax, double samplingFrequency);
int PitchTier_playPart_sine (I, double tmin, double tmax);

/* End of file PitchTier_to_Sound.h */
