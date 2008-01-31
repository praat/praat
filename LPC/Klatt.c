/* Klatt.h
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
 djmw 20080124 First
*/

#include "Klatt.h"
#include "PitchTier_to_Sound.h"
#include "PointProcess_and_Sound.h"

#include "oo_DESTROY.h"
#include "Klatt_def.h"
#include "oo_COPY.h"
#include "Klatt_def.h"
#include "oo_EQUAL.h"
#include "Klatt_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Klatt_def.h"
#include "oo_WRITE_TEXT.h"
#include "Klatt_def.h"
#include "oo_READ_TEXT.h"
#include "Klatt_def.h"
#include "oo_WRITE_BINARY.h"
#include "Klatt_def.h"
#include "oo_READ_BINARY.h"
#include "Klatt_def.h"
#include "oo_DESCRIPTION.h"
#include "Klatt_def.h"

class_methods (GlottisControlPoint, Data) {
	class_method_local (GlottisControlPoint, destroy)
	class_method_local (GlottisControlPoint, copy)
	class_method_local (GlottisControlPoint, equal)
	class_method_local (GlottisControlPoint, canWriteAsEncoding)
	class_method_local (GlottisControlPoint, writeText)
	class_method_local (GlottisControlPoint, readText)
	class_method_local (GlottisControlPoint, writeBinary)
	class_method_local (GlottisControlPoint, readBinary)
	class_method_local (GlottisControlPoint, description)
	class_methods_end
}

static void info (I)
{
	iam (Klatt);
	classFunction -> info (me);
	MelderInfo_writeLine5 (L"Time domain: ", Melder_double (my xmin), L" to ", Melder_double (my xmax), L" (s).");
}

class_methods (Klatt, Function)
{
	class_method_local (Klatt, destroy)
	class_method_local (Klatt, equal)
	class_method_local (Klatt, copy)
	class_method_local (Klatt, canWriteAsEncoding)
	class_method_local (Klatt, readText)
	class_method_local (Klatt, readBinary)
	class_method_local (Klatt, writeText)
	class_method_local (Klatt, writeBinary)
	class_method_local (Klatt, description)
	class_method (info)
	class_methods_end
}

GlottisControlPoint GlottisControlPoint_create (double time, double amplitude, double openPhase, double power1, double power2)
{
	GlottisControlPoint me = new (GlottisControlPoint);
	if (me == NULL) return NULL;
	my time = time;
	my amplitude = amplitude;
	my openPhase = openPhase;
	my power1 = power1;
	my power2 = power2;
	return me;
}

GlottisControlTier GlottisControlTier_create (double tmin, double tmax)
{
	GlottisControlTier me = new (GlottisControlTier);
	if (me == NULL || ((my points = SortedSetOfDouble_create ()) == NULL))
	{ 
		forget (me); return NULL;
	}
	my xmin = tmin;
	my xmax = tmax;
	return me;
}

PointProcess GlottisControlTier_to_PointProcess (GlottisControlTier me)
{
	PointProcess thee = PointProcess_create (my xmin, my xmax, my points -> size);
	if (thee == NULL) return NULL;
	for (long i = 1; i <= my points -> size; i++)
	{
		GlottisControlPoint him = my points -> item[i];
		// PointProcess_addPoint (thee, my points -> item[i] -> time))
		thy t[i] = his time;
	}
	thy nt = my points -> size;
	return thee;
}

Klatt Klatt_create (double tmin, double tmax)
{
	Klatt me = new (Klatt);
	if (me == NULL || ! Function_init (me, tmin, tmax)) return NULL;
	my gct = GlottisControlTier_create (tmin, tmax);
	if (my gct == NULL) return NULL;
	my ft = FormantTier_create (tmin, tmax);
	if (my ft == NULL) forget (me);
	return me;
}

Sound Klatt_to_Sound_pulses (Klatt me, double from, double to, double samplingFrequency, double adaptFactor, double adaptTime, long interpolationDepth)
{
	Sound thee = NULL;
	// to be done; select (from, to)
	(void) from; (void) to;
	
	
	PointProcess point = GlottisControlTier_to_PointProcess (my gct);
	if (point == NULL)  return NULL;
	
	thee = PointProcess_to_Sound_pulseTrain (point, samplingFrequency, adaptFactor,
		adaptTime, interpolationDepth);
	if (thee != NULL) Sound_Klatt_filter_inline (thee, me);
	forget (point);
	return thee;
}

Sound Klatt_to_Sound_phonation (Klatt me, double from, double to, double samplingFrequency, double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2)
{
	Sound thee = NULL;
	(void) from; (void) to;
	// to be done: select (from, to)
	PointProcess point = GlottisControlTier_to_PointProcess (my gct);
	if (point == NULL)  return NULL;
	
	thee = PointProcess_to_Sound_phonation (point, samplingFrequency, adaptFactor,
		maximumPeriod, openPhase, collisionPhase, power1, power2);
	if (thee != NULL) Sound_Klatt_filter_inline (thee, me);
	forget (point);
	return thee;
}

Sound Sound_Klatt_filter (Sound me, Klatt thee)
{
	// thee is the source and contains the timing xmin, xmax!	
	Sound him = Sound_FormantTier_filter (me, thy ft);
	if (thy it != NULL)
	{
		// modify intensities
	}
	return him;
}
 
void Sound_Klatt_filter_inline (Sound me, Klatt thee)
{
	// thee is the source and contains the timing xmin, xmax!	
	Sound_FormantTier_filter_inline (me, thy ft);
	if (thy it != NULL)
	{
		// modify intensities
	}
}

Klatt Sound_to_Klatt (Sound me, double from, double to)
{
	Klatt thee = NULL;
	(void) me;
	(void) from;
	(void) to;
	return thee;
}

/* End of file Klatt.c */
