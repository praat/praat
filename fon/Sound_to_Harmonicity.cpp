/* Sound_to_Harmonicity.cpp
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
 * pb 2004/10/29 corrected
 * pb 2011/03/09 C++
 */

#include "Sound_to_Pitch.h"
#include "Sound_to_Harmonicity.h"

Harmonicity Sound_to_Harmonicity_ac (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	try {
		autoPitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 1,
			silenceThreshold, 0, 0, 0, 0, 0.5 / my dx);
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
			pitch -> dx, pitch -> x1);
		for (long i = 1; i <= thy nx; i ++) {
			if (pitch -> frame [i]. candidate [1]. frequency == 0) {
				thy z [1] [i] = -200;
			} else {
				double r = pitch -> frame [i]. candidate [1]. strength;
				thy z [1] [i] = r <= 1e-15 ? -150 : r > 1 - 1e-15 ? 150 : 10 * log10 (r / (1 - r));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": harmonicity analysis (ac) not performed.");
	}
}

Harmonicity Sound_to_Harmonicity_cc (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	try {
		autoPitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 3,
			silenceThreshold, 0, 0, 0, 0, 0.5 / my dx);
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
			pitch -> dx, pitch -> x1);
		for (long i = 1; i <= thy nx; i ++) {
			if (pitch -> frame [i]. candidate [1]. frequency == 0) {
				thy z [1] [i] = -200;
			} else {
				double r = pitch -> frame [i]. candidate [1]. strength;
				thy z [1] [i] = r <= 1e-15 ? -150 : r > 1 - 1e-15 ? 150 : 10 * log10 (r / (1 - r));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": harmonicity analysis (cc) not performed.");
	}
}

/* End of file Sound_to_Harmonicity.cpp */
