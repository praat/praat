/* Sound_to_Intensity.h
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

#include "Sound.h"
#include "Intensity.h"
#include "IntensityTier.h"

Intensity Sound_to_Intensity (Sound me, double minimumPitch, double timeStep, int subtractMean);
/*
	Function:
		smooth away the periodic part of a signal,
		by convolving the square of the signal with a Kaiser(20.24) window;
		and resample on original sample points.
	Arguments:
		'minimumPitch':
			the minimum periodicity frequency that will be smoothed away
			to at most 0.00001 %.
			The Hanning/Hamming-equivalent window length will be 3.2 / 'minimumPitch'.
			The actual window length will be twice that.
		'timeStep':
			if <= 0.0, then 0.8 / minimumPitch.
	Performance:
		every periodicity frequency greater than 'minimumPitch'
		will be smoothed away to at most 0.00001 %;
		if 'timeStep' is 0 or less than 3.2 / 'minimumPitch',
		aliased frequencies will be at least 140 dB down.
	Example:
		minimumPitch = 100 Hz;
		Hanning/Hanning-equivalent window duration = 32 ms;
		actual window duration = 64 ms;
*/

IntensityTier Sound_to_IntensityTier (Sound me, double minimumPitch, double timeStep, int subtractMean);

/* End of file Sound_to_Intensity.h */
