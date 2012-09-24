#ifndef _FormantGrid_h_
#define _FormantGrid_h_
/* FormantGrid.h
 *
 * Copyright (C) 2008-2011 Paul Boersma & David Weenink
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

#include "RealTier.h"
#include "Sound.h"
#include "Formant.h"

Thing_declare (Interpreter);

#include "FormantGrid_def.h"
oo_CLASS_CREATE (FormantGrid, Function);

void FormantGrid_init (I, double tmin, double tmax, long numberOfFormants);
FormantGrid FormantGrid_createEmpty (double tmin, double tmax, long numberOfFormants);

FormantGrid FormantGrid_create (double tmin, double tmax, long numberOfFormants,
	double initialFirstFormant, double initialFormantSpacing,
	double initialFirstBandwidth, double initialBandwidthSpacing);

double FormantGrid_getFormantAtTime (FormantGrid me, long iformant, double t);
double FormantGrid_getBandwidthAtTime (FormantGrid me, long iformant, double t);

void FormantGrid_addFormantPoint (FormantGrid me, long iformant, double t, double value);
void FormantGrid_addBandwidthPoint (FormantGrid me, long iformant, double t, double value);
void FormantGrid_removeFormantPointsBetween (FormantGrid me, long iformant, double tmin, double tmax);
void FormantGrid_removeBandwidthPointsBetween (FormantGrid me, long iformant, double tmin, double tmax);

void Sound_FormantGrid_filter_inline (Sound me, FormantGrid formantGrid);
Sound Sound_FormantGrid_filter (Sound me, FormantGrid formantGrid);
Sound Sound_FormantGrid_filter_noscale (Sound me, FormantGrid formantGrid);

Sound FormantGrid_to_Sound (FormantGrid me, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2);
void FormantGrid_playPart (FormantGrid me, double tmin, double tmax, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure);

void FormantGrid_formula_frequencies (FormantGrid me, const wchar_t *expression, Interpreter interpreter, FormantGrid thee);
void FormantGrid_formula_bandwidths (FormantGrid me, const wchar_t *expression, Interpreter interpreter, FormantGrid thee);

FormantGrid Formant_downto_FormantGrid (Formant me);
Formant FormantGrid_to_Formant (FormantGrid me, double dt, double intensity);

Sound Sound_Formant_filter (Sound me, Formant formant);
Sound Sound_Formant_filter_noscale (Sound me, Formant formant);

/* End of file FormantGrid.h */
#endif
