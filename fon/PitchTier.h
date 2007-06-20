#ifndef _PitchTier_h_
#define _PitchTier_h_
/* PitchTier.h
 *
 * Copyright (C) 1992-2003 Paul Boersma
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
 * pb 2003/11/20
 */

#ifndef _RealTier_h_
	#include "RealTier.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

/********** class PitchTier **********/

#define PitchTier_members RealTier_members
#define PitchTier_methods RealTier_methods
class_create (PitchTier, RealTier);

PitchTier PitchTier_create (double tmin, double tmax);
/*
	Postconditions:
		result -> xmin == tmin;
		result -> xmax == tmax;
		result -> points -> size == 0;
*/

int PitchTier_shiftFrequencies (PitchTier me, double tmin, double tmax, double shift, int units);
void PitchTier_multiplyFrequencies (PitchTier me, double tmin, double tmax, double factor);

void PitchTier_draw (PitchTier me, Graphics g, double tmin, double tmax,
	double fmin, double fmax, int garnish);

PitchTier PointProcess_upto_PitchTier (PointProcess me, double frequency);
void PitchTier_stylize (PitchTier me, double frequencyResolution, int useSemitones);

int PitchTier_writeToPitchTierSpreadsheetFile (PitchTier me, MelderFile fs);
int PitchTier_writeToHeaderlessSpreadsheetFile (PitchTier me, MelderFile fs);

/* End of file PitchTier.h */
#endif
