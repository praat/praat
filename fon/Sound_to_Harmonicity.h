/* Sound_to_Harmonicity.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Sound.h"
#include "Harmonicity.h"

autoHarmonicity Sound_to_Harmonicity_ac (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow);

autoHarmonicity Sound_to_Harmonicity_cc (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow);

autoMatrix Sound_to_Harmonicity_GNE (Sound me,
	double fmin,   /* 500 Hz */
	double fmax,   /* 4500 Hz */
	double bandwidth,  /* 1000 Hz */
	double step);   /* 80 Hz */

/* End of file Sound_to_Harmonicity.h */
