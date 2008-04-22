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
 djmw 20080130 First
*/

#include "Vowel.h"
#include "PitchTier_to_Sound.h"
#include "PitchTier_to_PointProcess.h"
#include "PointProcess_and_Sound.h"


#include "oo_DESTROY.h"
#include "Vowel_def.h"
#include "oo_COPY.h"
#include "Vowel_def.h"
#include "oo_EQUAL.h"
#include "Vowel_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Vowel_def.h"
#include "oo_WRITE_TEXT.h"
#include "Vowel_def.h"
#include "oo_READ_TEXT.h"
#include "Vowel_def.h"
#include "oo_WRITE_BINARY.h"
#include "Vowel_def.h"
#include "oo_READ_BINARY.h"
#include "Vowel_def.h"
#include "oo_DESCRIPTION.h"
#include "Vowel_def.h"

class_methods (Vowel, Function)
{
	class_method_local (Vowel, destroy)
	class_method_local (Vowel, equal)
	class_method_local (Vowel, copy)
	class_method_local (Vowel, canWriteAsEncoding)
	class_method_local (Vowel, readText)
	class_method_local (Vowel, readBinary)
	class_method_local (Vowel, writeText)
	class_method_local (Vowel, writeBinary)
	class_method_local (Vowel, description)
	class_methods_end
}

Vowel Vowel_create (double duration)
{
	Vowel me = new (Vowel);
	
	if (me == NULL || ! Function_init (me, 0, duration)) return NULL;
	my ft = FormantTier_create (0, duration);
	my pt = PitchTier_create (0, duration);
	if (Melder_hasError ()) forget (me);
	return me;
}

Vowel Vowel_create_twoFormantSchwa (double duration)
{
	FormantPoint fp = NULL;
	Vowel me = Vowel_create (duration);
	if (me == NULL) return NULL;
	
	fp =  FormantPoint_create (0);
	fp -> formant [0] = 500;
	fp -> bandwidth[0] = 50;
	fp -> formant [1] = 1500;
	fp -> bandwidth[1] = 150;
	fp -> numberOfFormants = 2;
	if (! Collection_addItem (my ft -> points, fp) || ! RealTier_addPoint (my pt, 0, 140)) goto end;
	
	fp =  FormantPoint_create (duration);
	if (fp == NULL) goto end;
	fp -> formant [0] = 500;
	fp -> bandwidth[0] = 50;
	fp -> formant [1] = 1500;
	fp -> bandwidth[1] = 150;
	fp -> numberOfFormants = 2;
	if (Collection_addItem (my ft -> points, fp)) RealTier_addPoint (my pt, duration, 140);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

void Vowel_play (Vowel me)
{
	Sound thee = Vowel_to_Sound_pulses (me, 44100, 0.7, 0.05, 30);
	if (thee != NULL) Sound_play (thee, NULL, NULL);
	forget (thee);
}

Sound Vowel_to_Sound_pulses (Vowel me, double samplingFrequency, double adaptFactor, double adaptTime, long interpolationDepth)
{
	Sound thee = NULL;
	PointProcess pp = PitchTier_to_PointProcess (my pt);
	if (pp != NULL)
	{
		thee = PointProcess_to_Sound_pulseTrain (pp, samplingFrequency, adaptFactor, adaptTime, interpolationDepth);
		Sound_FormantTier_filter_inline (thee, my ft);
		forget (pp);
	}
	return thee;
}

Sound Vowel_to_Sound_phonation (Vowel me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2)
{
	Sound thee = NULL;
	PointProcess pp = PitchTier_to_PointProcess (my pt);
	if (pp != NULL)
	{
		thee = PointProcess_to_Sound_phonation (pp, samplingFrequency, adaptFactor, maximumPeriod,
			openPhase, collisionPhase, power1, power2);
		Sound_FormantTier_filter_inline (thee, my ft);
		forget (pp);
	}
	return thee;
}

/* End of file Vowel.c */
