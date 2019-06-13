#ifndef _Pitch_extensions_h_
#define _Pitch_extensions_h_
/* Pitch_extensions.h
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19970408
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "Pitch.h"
#include "PitchTier.h"

void Pitch_Frame_addPitch (Pitch_Frame me, double f, double strength, integer maxnCandidates);

void Pitch_Frame_getPitch (Pitch_Frame me, double *f, double *strength);

void Pitch_Frame_resizeStrengths (Pitch_Frame me, double maxStrength, double unvoicedCriterium);
	
autoPitch Pitch_scaleTime (Pitch me, double scaleFactor);
/*  Scale time domain and pitches:
	xmin' = xmin; dx' = dx * scaleFactor; x1' = xmin + 0.5 * dx'; 
	xmax' = xmin + nx * dx';
	pitch[i]' = pitch[i]/scaleFactor;
*/

autoPitch PitchTier_to_Pitch (PitchTier me, double dt, double pitchFloor, double pitchCeiling);

#endif /* _Pitch_extensions_h_ */
