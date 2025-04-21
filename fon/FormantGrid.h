#ifndef _FormantGrid_h_
#define _FormantGrid_h_
/* FormantGrid.h
 *
 * Copyright (C) 2008-2012,2014-2018,2025 Paul Boersma & David Weenink
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

double FormantGrid_getFormantAtTime (constFormantGrid me, integer formantNumber, double time);
double FormantGrid_getBandwidthAtTime (constFormantGrid me, integer formantNumber, double time);

void FormantGrid_addFormantPoint (mutableFormantGrid me, integer formantNumber, double time, double value);
void FormantGrid_addBandwidthPoint (mutableFormantGrid me, integer formantNumber, double time, double value);
void FormantGrid_removeFormantPointsBetween (mutableFormantGrid me, integer formantNumber, double tmin, double tmax);
void FormantGrid_removeBandwidthPointsBetween (mutableFormantGrid me, integer formantNumber, double tmin, double tmax);

void Sound_FormantGrid_filter_inplace (mutableSound me, constFormantGrid formantGrid);
autoSound Sound_FormantGrid_filter (constSound me, constFormantGrid formantGrid);
autoSound Sound_FormantGrid_filter_noscale (constSound me, constFormantGrid formantGrid);

autoSound FormantGrid_to_Sound (
	constFormantGrid me,
	double samplingFrequency,        // the sample rate of the resulting Sound
	double tStart, double f0Start,   // time and F0 of the first pitch point
	double tMid, double f0Mid,       // time and F0 of the second pitch point
	double tEnd, double f0End,       // time and F0 of the third pitch point
	double adaptFactor, double maximumPeriod,                               // pulse generation settings
	double openPhase, double collisionPhase, double power1, double power2   // phonation shape settings
);
void FormantGrid_playPart (
	constFormantGrid me,
	double tmin, double tmax,        // the time range of the part to play
	double samplingFrequency,        // the sample rate of the resulting Sound
	double tStart, double f0Start,   // time and F0 of the first pitch point
	double tMid, double f0Mid,       // time and F0 of the second pitch point
	double tEnd, double f0End,       // time and F0 of the third pitch point
	double adaptFactor, double maximumPeriod,                                // pulse generation settings
	double openPhase, double collisionPhase, double power1, double power2,   // phonation shape settings
	Sound_PlayCallback playCallback, mutableThing playBoss   // callback to boss: cancel playing, show running cursor...
);

void FormantGrid_formula_frequencies (
	mutableFormantGrid me,      // the source (and often also the target, namely if `thee` is null)
	conststring32 expression,
	Interpreter interpreter,
	mutableFormantGrid thee     // the target (if not null)
);
void FormantGrid_formula_bandwidths (
	mutableFormantGrid me,      // the source (and often also the target, namely if `thee` is null)
	conststring32 expression,
	Interpreter interpreter,
	mutableFormantGrid thee     // the target (if not null)
);

autoFormantGrid Formant_downto_FormantGrid (constFormant me);
autoFormant FormantGrid_to_Formant (constFormantGrid me, double dt, double intensity);

autoSound Sound_Formant_filter (constSound me, constFormant formant);
autoSound Sound_Formant_filter_noscale (constSound me, constFormant formant);

/* End of file FormantGrid.h */
#endif
