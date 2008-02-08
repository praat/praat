#ifndef _Vowel_h_
#define _Vowel_h_
/* Vowel.h
 *
 * Copyright (C) 2008 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20080130 
*/

#ifndef _FormantTier_h_
	#include "FormantTier.h"
#endif
#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif
#ifndef _IntensityTier_h_
	#include "IntensityTier.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#include "Vowel_def.h"
#define Vowel_methods Function_methods
oo_CLASS_CREATE (Vowel, Function);

Vowel Vowel_create (double duration);
Vowel Vowel_create_twoFormantSchwa (double duration);

int Vowel_fixateDuration (Vowel me, double newDuration);

Sound Vowel_to_Sound_pulses (Vowel me, double samplingFrequency,
	double adaptFactor, double adaptTime, long interpolationDepth);

Sound Vowel_to_Sound_phonation (Vowel me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2);

void Vowel_play (Vowel me);

#endif /* _Vowel_h_ */
