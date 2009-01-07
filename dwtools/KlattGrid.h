#ifndef _KlattGrid_h_
#define _KlattGrid_h_
/* KlattGrid.h
 *
 * Copyright (C) 2008-2009 David Weenink
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
 * djmw 20090109 Latest modification
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
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#include "KlattGrid_def.h"

#define PhonationPoint_methods Data_methods
oo_CLASS_CREATE (PhonationPoint, Data);

#define PhonationTier_methods Function_methods
oo_CLASS_CREATE (PhonationTier, Function);

#define PhonationGrid_methods Function_methods
oo_CLASS_CREATE (PhonationGrid, Function);

#define VocalTractGrid_methods Function_methods
oo_CLASS_CREATE (VocalTractGrid, Function);

#define CouplingGrid_methods Function_methods
oo_CLASS_CREATE (CouplingGrid, Function);

#define FricationGrid_methods Function_methods
oo_CLASS_CREATE (FricationGrid, Function);

#define KlattGrid_methods Function_methods
oo_CLASS_CREATE (KlattGrid, Function);

/****************** synthesis parameters **************************/

typedef struct KlattGrid_synthesisParams {
	double samplingFrequency;
	double maximumPeriod;
	int voicing, aspiration, breathiness, spectralTilt;
	int filterModel; // 1: parallel, 0: cascade
	int sourceIsFlowDerivative;
	long startFormant, endFormant;
	long startNasalFormant, endNasalFormant;
	long startTrachealFormant, endTrachealFormant;
	long startNasalAntiFormant, endNasalAntiFormant;
	long startTrachealAntiFormant, endTrachealAntiFormant;
	long startFricationFormant, endFricationFormant;
	double openglottis_fadeFraction; // (0-0.5)
	int openglottis, fricationBypass;
	int klatt80;
	KlattGrid *kg; // because of coupling between source and filter
} *KlattGrid_synthesisParams;

struct KlattGrid_synthesisParams KlattGrid_synthesisParams_createDefault (KlattGrid me);
void KlattGrid_synthesisParams_setDefault (KlattGrid_synthesisParams p, KlattGrid thee);

/******************** PhonationPoint & Tier ************************************/

PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
	double power1, double power2, double pulseScale);

PhonationTier PhonationTier_create (double tmin, double tmax);

/************************ PhonationGrid *********************************************/

PhonationGrid PhonationGrid_create (double tmin, double tmax);

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency);

void PhonationGrid_draw (PhonationGrid me, Graphics g);

double PhonationGrid_getMaximumPeriod (PhonationGrid me);

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me, double maximumPeriod, int klatt80);

/************************ VocalTractGrid *********************************************/

VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants,	long numberOfNasalAntiFormants);

void VocalTractGrid_draw (VocalTractGrid me, Graphics g, int filterModel);

/************************ CouplingGrid *********************************************/
	
CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants);

double CouplingGrid_getDeltaFormantAtTime (CouplingGrid me, long iformant, double t);
double CouplingGrid_getDeltaBandwidthAtTime (CouplingGrid me, long iformant, double t);

/********************** FormantGrid & CouplingGrid *************************************/

int FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee, double fadeFraction);

/********************** Sound & FormantGrid (& IntensityTier) *************************************/

int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant);
Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign);

/************************ FricationGrid *********************************************/

FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants);

void FricationGrid_draw (FricationGrid me, Graphics g);

Sound FricationGrid_to_Sound (FricationGrid me, KlattGrid_synthesisParams p);

Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee, KlattGrid_synthesisParams params);

/************************ Sound & VocalTractGrid & CouplingGrid *********************************************/

Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling, KlattGrid_synthesisParams p);

/************************ KlattGrid *********************************************/

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants, long numberOfNasalAntiFormants,
	long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
	long numberOfFricationFormants, long numberOfDeltaFormants);

KlattGrid KlattGrid_createExample (void);

KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration);

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel);
void klattGrid_drawPhonation (KlattGrid me, Graphics g);
void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea);

#define KlattGrid_NORMAL_FORMANTS 1
#define KlattGrid_NASAL_FORMANTS 2
#define KlattGrid_FRICATION_FORMANTS 3
#define KlattGrid_TRACHEAL_FORMANTS 4
#define KlattGrid_NASAL_ANTIFORMANTS 5
#define KlattGrid_TRACHEAL_ANTIFORMANTS 6
#define KlattGrid_DELTA_FORMANTS 7

// Add, Remove, Extract, Replace from PhonationGrid
#define PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO(Name,tierType) \
double KlattGrid_get##Name##AtTime (KlattGrid me, double t); \
int KlattGrid_add##Name##Point (KlattGrid me, double t, double value); \
void KlattGrid_remove##Name##PointsBetween (KlattGrid me, double t1, double t2); \
tierType KlattGrid_extract##Name##Tier (KlattGrid me); \
int KlattGrid_replace##Name##Tier (KlattGrid me, tierType thee);

// Generate 55 prototypes	
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (Pitch, PitchTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (Flutter, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (DoublePulsing, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (OpenPhase, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (CollisionPhase, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (SpectralTilt, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (Power1, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (Power2, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (VoicingAmplitude, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (AspirationAmplitude, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO (BreathinessAmplitude, IntensityTier)

#define KlattGrid_QUERY_ADD_REMOVE_PROTO(Name) \
double KlattGrid_get##Name##AtTime (KlattGrid me, int formantType, long iformant, double t); \
int KlattGrid_add##Name##Point (KlattGrid me, int formantType, long iformant, double t, double value); \
void KlattGrid_remove##Name##PointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2); \
double KlattGrid_getDelta##Name##AtTime (KlattGrid me, long iformant, double t); \
int KlattGrid_addDelta##Name##Point (KlattGrid me, long iformant, double t, double value); \
void KlattGrid_removeDelta##Name##PointsBetween (KlattGrid me, long iformant, double t1, double t2);

// 12 prototypes
KlattGrid_QUERY_ADD_REMOVE_PROTO(Formant)
KlattGrid_QUERY_ADD_REMOVE_PROTO(Bandwidth)

int KlattGrid_formula_frequencies (KlattGrid me, int formantType, const wchar_t *expression);
int KlattGrid_formula_bandwidths (KlattGrid me, int formantType, const wchar_t *expression);

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType);
int KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee);

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me);
int KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee);

FormantGrid KlattGrid_to_FormantGrid_openPhases (KlattGrid me, double fadeFraction);

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t);
int KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value);
void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2);
IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant);
int KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee);


double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t);
int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me);
int KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee);

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t);
int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me);
int KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee);

int KlattGrid_setGlottisCoupling (KlattGrid me, double maximumPeriod, int klatt80);

Any KlattGrid_getAddressOfFormantGrid (KlattGrid me, int formantType);
Any KlattGrid_getAddressOfAmplitudes (KlattGrid me, int formantType);

/***************** KlattGrid & Sound *************************************/

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int filterType);

int KlattGrid_play (KlattGrid me);

Sound KlattGrid_to_Sound (KlattGrid me, KlattGrid_synthesisParams params);

Sound KlattGrid_to_Sound_phonation (KlattGrid me, double samplingFrequency, int voicing, int spectralTilt, int sourceIsFlowDerivative, int breathiness, int aspiration);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

/*
	glottal: phonation+aspiration, before entering the filter
	frication: noise before entering the parallel frication filter section.
*/

Sound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, int filterModel);

Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, KlattGrid_synthesisParams params);

Sound Sounds_KlattGrid_filter_allSources (Sound glottal, Sound frication, KlattGrid me, KlattGrid_synthesisParams params);

Sound Sounds_KlattGrid_filter_oneSource (Sound frication, KlattGrid me, int sourceType, KlattGrid_synthesisParams params);

/* type = 0 glottal souce signal, 1 = voiced only, 2 = aspiration only 3: frication */
Sound Sound_KlattGrid_filter (Sound me, KlattGrid thee, int type, KlattGrid_synthesisParams params);

KlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, long maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double minimumPitchIntensity, int subtractMean);

/**************** KlattGrid & Tiers ************************/

int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee);

#endif /* _KlattGrid_h_ */
