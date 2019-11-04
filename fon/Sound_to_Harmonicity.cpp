/* Sound_to_Harmonicity.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2017,2019 Paul Boersma
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

#include "Sound_to_Pitch.h"
#include "Sound_to_Harmonicity.h"

autoHarmonicity Sound_to_Harmonicity_ac (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	try {
		autoPitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 1,
				silenceThreshold, 0.0, 0.0, 0.0, 0.0, 0.5 / my dx);
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
				pitch -> dx, pitch -> x1);
		for (integer i = 1; i <= thy nx; i ++) {
			if (pitch -> frames [i]. candidates [1]. frequency == 0.0) {
				thy z [1] [i] = -200.0;
			} else {
				const double r = pitch -> frames [i]. candidates [1]. strength;
				thy z [1] [i] = ( r <= 1e-15 ? -150.0 : r > 1.0 - 1e-15 ? 150.0 : 10.0 * log10 (r / (1.0 - r)) );
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": harmonicity analysis (ac) not performed.");
	}
}

autoHarmonicity Sound_to_Harmonicity_cc (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	try {
		autoPitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 3,
				silenceThreshold, 0.0, 0.0, 0.0, 0.0, 0.5 / my dx);
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
				pitch -> dx, pitch -> x1);
		for (integer i = 1; i <= thy nx; i ++) {
			if (pitch -> frames [i]. candidates [1]. frequency == 0.0) {
				thy z [1] [i] = -200.0;
			} else {
				double r = pitch -> frames [i]. candidates [1]. strength;
				thy z [1] [i] = ( r <= 1e-15 ? -150.0 : r > 1.0 - 1e-15 ? 150.0 : 10.0 * log10 (r / (1.0 - r)) );
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": harmonicity analysis (cc) not performed.");
	}
}

/* End of file Sound_to_Harmonicity.cpp */
