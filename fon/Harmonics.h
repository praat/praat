#ifndef _Harmonics_h_
#define _Harmonics_h_
/* Harmonics.h
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "PointProcess.h"
#include "Sound.h"

#include "Harmonics_def.h"
oo_CLASS_CREATE (Harmonics, Data);

Harmonics Harmonics_create (long numberOfHarmonics);

void Harmonics_draw (Harmonics me, Graphics g, double fmin, double fmax,
	double minimum, double maximum, int garnish, const wchar_t *method);

Matrix Harmonics_to_Matrix (Harmonics me);
Harmonics Matrix_to_Harmonics (Matrix me);

/* Direct computation. */

Harmonics PointProcess_Sound_to_Harmonics (PointProcess pulses, Sound sound,
	long maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/* Shortcut. */

Harmonics Sound_to_Harmonics (Sound me, double bandwidth);

/* End of file Harmonics.h */
#endif
