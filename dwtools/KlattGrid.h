#ifndef _KlattGrid_h_
#define _KlattGrid_h_
/* KlattGrid.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * djmw 20080917 Initial version
 */

#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _IntensityTier_h_
	#include "IntensityTier.h"
#endif
#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif
#ifndef _FormantGrid_h_
	#include "FormantGrid.h"
#endif
#ifndef _KlattTable_h_
	#include "KlattTable.h"
#endif

#include "KlattGrid_def.h"

#define DeltaFormantGrid_methods FormantGrid_methods
oo_CLASS_CREATE (DeltaFormantGrid, FormantGrid);

#define FormantGridP_members FormantGrid_members \
	Ordered amplitudes;

#define FormantGridP_methods FormantGrid_methods
oo_CLASS_CREATE (FormantGridP, FormantGrid);

#define PhonationGrid_methods Function_methods
oo_CLASS_CREATE (PhonationGrid, Function);

#define KlattGrid_methods Function_methods
oo_CLASS_CREATE (KlattGrid, Function);

typedef struct synthesisParams {
	double samplingFrequency;
	double maximumPeriod;
	int voicing, aspiration, spectralTilt;
	int filterModel; // 1: parallel, 0: cascade
	long startFormant, endFormant;
	long startNasalFormant, endNasalFormant;
	long startTrachealFormant, endTrachealFormant;
	long startNasalAntiFormant, endNasalAntiFormant;
	long startTrachealAntiFormant, endTrachealAntiFormant;
	long startFricationFormant, endFricationFormant;
	int fricationBypass;
	int klatt80;
	double openglottis_fadeFraction; // (0-0.5)
	KlattGrid *kg; // because of coupling between source and filter
} *synthesisParams;

FormantGridP FormantGridP_create (double tmin, double tmax, long numberOfFormants);
FormantGrid FormantGridP_downto_FormantGrid (FormantGridP me);
DeltaFormantGrid DeltaFormantGrid_create (double tmin, double tmax, long numberOfFormants);
double DeltaFormantGrid_getDeltaFormantAtTime (DeltaFormantGrid me, long iformant, double t);
double DeltaFormantGrid_getDeltaBandwidthAtTime (DeltaFormantGrid me, long formantNumber, double t);

PhonationGrid PhonationGrid_create (double tmin, double tmax);

double PhonationGrid_getCollisionPhaseAtTime (PhonationGrid me, double t);
int PhonationGrid_addCollisionPhasePoint (PhonationGrid me, double t, double value);
void PhonationGrid_removeCollisionPhasePointsBetween (PhonationGrid me, double t1, double t2);

double PhonationGrid_getOpenPhaseAtTime (PhonationGrid me, double t);
int PhonationGrid_addOpenPhasePoint (PhonationGrid me, double t, double value);
void PhonationGrid_removeOpenPhasePointsBetween (PhonationGrid me, double t1, double t2);

double PhonationGrid_getFlowFunctionAtTime (PhonationGrid me, long powerTerm, double t);
int PhonationGrid_addFlowFunctionPoint (PhonationGrid me, long powerTerm, double t, double value);
void PhonationGrid_removeFlowFunctionPointsBetween (PhonationGrid me, long powerTerm, double t1, double t2);

double PhonationGrid_getPitchAtTime (PhonationGrid me, double t);
int PhonationGrid_addPitchPoint (PhonationGrid me, double t, double value);
void PhonationGrid_removePitchPointsBetween (PhonationGrid me, double t1, double t2);

double PhonationGrid_getFlutterAtTime (PhonationGrid me, double t);
int PhonationGrid_addFlutterPoint (PhonationGrid me, double t, double value);
void PhonationGrid_removeFlutterPointsBetween (PhonationGrid me, double t1, double t2);

double PhonationGrid_getPhonationAmplitudeAtTime (PhonationGrid me, int phonationAmplitudeTier, double t);
int PhonationGrid_addPhonationAmplitudePoint (PhonationGrid me, int phonationAmplitudeTier, double t, double value);
void PhonationGrid_removePhonationAmplitudePointsBetween (PhonationGrid me, int phonationAmplitudeTier, double t1, double t2);

double PhonationGrid_getSpectralTiltAtTime (PhonationGrid me, double t);
int PhonationGrid_addSpectralTiltPoint (PhonationGrid me, double t, double value);
void PhonationGrid_removeSpectralTiltPointsBetween (PhonationGrid me, double t1, double t2);

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants, long numberOfNasalFormants,
	long numberOfTrachealFormants, long numberOfFricationFormants, long numberOfDeltaFormants);

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int parallel);

double KlattGrid_getBandwidthAtTime (KlattGrid me, int gridType, long formantNumber, double t);
int KlattGrid_addBandwidthPoint (KlattGrid me, int gridType, long formantNumber, double t, double value);
void KlattGrid_removeBandwidthPointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2);

double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, long formantNumber, double t);
int KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long formantNumber, double t, double value);
void KlattGrid_removeDeltaBandwidthPointsBetween (KlattGrid me, long iformant, double t1, double t2);

double KlattGrid_getFormantAtTime (KlattGrid me, int gridType, long formantNumber, double t);
int KlattGrid_addFormantPoint (KlattGrid me, int gridType, long formantNumber, double t, double value);
void KlattGrid_removeFormantPointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2);

double KlattGrid_getDeltaFormantAtTime (KlattGrid me, long formantNumber, double t);
int KlattGrid_addDeltaFormantPoint (KlattGrid me, long formantNumber, double t, double value);
void KlattGrid_removeDeltaFormantPointsBetween (KlattGrid me, long iformant, double t1, double t2);

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int gridType, long formantNumber, double t);
int KlattGrid_addAmplitudePoint (KlattGrid me, int gridType, long formantNumber, double t, double value);
void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2);

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t);
int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2);

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t);
int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2);

Sound KlattGrid_to_Sound (KlattGrid me, synthesisParams params);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

KlattGrid KlattGrid_createExample (void);

KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration);

/*
	glottal: phonation+aspiration, before entering the filter
	frication: noise before entering the parallel frication filter section.
*/
Sound Sounds_KlattGrid_filter_allSources (Sound glottal, Sound frication, KlattGrid me, RealTier glottisOpenDurations, synthesisParams params);

Sound Sounds_KlattGrid_filter_oneSource (Sound frication, KlattGrid me, int sourceType, synthesisParams params);

/* type = 0 glottal souce signal, 1 = voiced only, 2 = aspiration only 3: frication */
Sound Sound_KlattGrid_filter (Sound me, KlattGrid thee, int type, synthesisParams params);

/**************** KlattGrid & Tiers ************************/

int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee);

#endif /* _KlattGrid_h_ */
