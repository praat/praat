#ifndef _Klatt_h_
#define _Klatt_h_
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
 djmw 20080124 
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

#include "Klatt_def.h"
#define GlottisControlPoint_methods Data_methods
oo_CLASS_CREATE (GlottisControlPoint, Data);

#define GlottisControlTier_methods Function_methods
oo_CLASS_CREATE (GlottisControlTier, Function);

#define Klatt_methods Function_methods
oo_CLASS_CREATE (Klatt, Function);

GlottisControlPoint GlottisControlPoint_create (double time, double amplitude, double openPhase, double power1, double power2);

GlottisControlTier GlottisControlTier_create (double tmin, double tmax);
PointProcess GlottisControlTier_to_PointProcess (GlottisControlTier me);

Klatt Klatt_create (double tmin, double tmax);

Sound Klatt_to_Sound_pulses (Klatt me, double from, double to, double samplingFrequency, double adaptationFactor, double adaptationTime, long interpolationDepth);

Sound Klatt_to_Sound_phonation (Klatt me, double from, double to, double samplingFrequency, double adaptationFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2);

Sound Sound_Klatt_filter (Sound me, Klatt thee);
void Sound_Klatt_filter_inline (Sound me, Klatt thee);

Klatt Sound_to_Klatt (Sound me, double from, double to);

#endif  /* _Klatt_h_ */
