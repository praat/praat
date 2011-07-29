/* Pitch_to_Sound.h
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
#include "Sound.h"

/* These routines convert a Pitch into a PointProcess, */
/* then this PointProcess into a Sound (pulse train), */
/* and then optionally filter this with 6 formants. */

Sound Pitch_to_Sound (Pitch me, double tmin, double tmax, int hum);
void Pitch_play (Pitch me, double tmin, double tmax);
void Pitch_hum (Pitch me, double tmin, double tmax);

/* This one converts a Pitch into a PitchTier, */
/* then this PitchTier into a Sound (sine wave), */
/* and then cuts away the unvoiced stretches. */

Sound Pitch_to_Sound_sine (Pitch me, double tmin, double tmax, double samplingFrequency,
	int roundToNearestZeroCrossings);

/* End of file Pitch_to_Sound.h */
