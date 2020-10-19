#ifndef _Harmonics_h_
#define _Harmonics_h_
/* Harmonics.h
 *
 * Copyright (C) 2011,2012,2015-2018,2020 Paul Boersma
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

#include "PointProcess.h"
#include "Sound.h"

#include "Harmonics_def.h"

autoHarmonics Harmonics_create (integer numberOfHarmonics);

void Harmonics_draw (Harmonics me, Graphics g, double fmin, double fmax,
	double minimum, double maximum, bool garnish, conststring32 method);

autoMatrix Harmonics_to_Matrix (Harmonics me);
autoHarmonics Matrix_to_Harmonics (Matrix me);

/* Direct computation. */

autoHarmonics PointProcess_Sound_to_Harmonics (PointProcess pulses, Sound sound,
	integer maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/* Shortcut. */

autoHarmonics Sound_to_Harmonics (Sound me, double bandwidth);

/* End of file Harmonics.h */
#endif
