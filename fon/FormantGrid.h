#ifndef _FormantGrid_h_
#define _FormantGrid_h_
/* FormantGrid.h
 *
 * Copyright (C) 2008-2011,2014,2015,2017 Paul Boersma & David Weenink
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

#include "RealTier.h"
#include "Sound.h"
#include "Formant.h"

Thing_declare (Interpreter);

#include "FormantGrid_def.h"

void FormantGrid_init (FormantGrid me, double tmin, double tmax, integer numberOfFormants);
autoFormantGrid FormantGrid_createEmpty (double tmin, double tmax, integer numberOfFormants);

autoFormantGrid FormantGrid_create (double tmin, double tmax, integer numberOfFormants,
	double initialFirstFormant, double initialFormantSpacing,
	double initialFirstBandwidth, double initialBandwidthSpacing);

double FormantGrid_getFormantAtTime (FormantGrid me, integer formantNumber, double time);
double FormantGrid_getBandwidthAtTime (FormantGrid me, integer formantNumber, double time);

void FormantGrid_addFormantPoint (FormantGrid me, integer formantNumber, double time, double value);
void FormantGrid_addBandwidthPoint (FormantGrid me, integer formantNumber, double time, double value);
void FormantGrid_removeFormantPointsBetween (FormantGrid me, integer formantNumber, double tmin, double tmax);
void FormantGrid_removeBandwidthPointsBetween (FormantGrid me, integer formantNumber, double tmin, double tmax);

void Sound_FormantGrid_filter_inplace (Sound me, FormantGrid formantGrid);
autoSound Sound_FormantGrid_filter (Sound me, FormantGrid formantGrid);
autoSound Sound_FormantGrid_filter_noscale (Sound me, FormantGrid formantGrid);

autoSound FormantGrid_to_Sound (FormantGrid me, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2);
void FormantGrid_playPart (FormantGrid me, double tmin, double tmax, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2,
	Sound_PlayCallback playCallback, Thing playBoss);

void FormantGrid_formula_frequencies (FormantGrid me, conststring32 expression, Interpreter interpreter, FormantGrid thee);
void FormantGrid_formula_bandwidths (FormantGrid me, conststring32 expression, Interpreter interpreter, FormantGrid thee);

autoFormantGrid Formant_downto_FormantGrid (Formant me);
autoFormant FormantGrid_to_Formant (FormantGrid me, double dt, double intensity);

autoSound Sound_Formant_filter (Sound me, Formant formant);
autoSound Sound_Formant_filter_noscale (Sound me, Formant formant);

/* End of file FormantGrid.h */
#endif
