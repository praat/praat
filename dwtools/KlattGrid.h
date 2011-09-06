#ifndef _KlattGrid_h_
#define _KlattGrid_h_
/* KlattGrid.h
 *
 * Copyright (C) 2008-2011 David Weenink
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
 * djmw 20110306 Latest modification
 */

#include "Collection.h"
#include "PointProcess.h"
#include "Sound.h"
#include "IntensityTier.h"
#include "PitchTier.h"
#include "FormantGrid.h"
#include "KlattTable.h"
#include "Interpreter_decl.h"

#include "KlattGrid_def.h"
oo_CLASS_CREATE (PhonationPoint, Data);
oo_CLASS_CREATE (PhonationTier, Function);
oo_CLASS_CREATE (PhonationGridPlayOptions, Data);
oo_CLASS_CREATE (PhonationGrid, Function);
oo_CLASS_CREATE (VocalTractGridPlayOptions, Data);
oo_CLASS_CREATE (VocalTractGrid, Function);
oo_CLASS_CREATE (CouplingGridPlayOptions, Data);
oo_CLASS_CREATE (CouplingGrid, Function);
oo_CLASS_CREATE (FricationGridPlayOptions, Data);
oo_CLASS_CREATE (FricationGrid, Function);
oo_CLASS_CREATE (KlattGridPlayOptions, Data);
oo_CLASS_CREATE (KlattGrid, Function);

/******************** PhonationPoint & Tier ************************************/

PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
	double power1, double power2, double pulseScale);

PhonationTier PhonationTier_create (double tmin, double tmax);
PointProcess PhonationTier_to_PointProcess_closures (PhonationTier me);

/************************ PhonationGrid *********************************************/

PhonationGrid PhonationGrid_create (double tmin, double tmax);
PhonationGridPlayOptions PhonationGridPlayOptions_create ();
void PhonationGrid_setNames (PhonationGrid me);

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency);

void PhonationGrid_draw (PhonationGrid me, Graphics g);

double PhonationGrid_getMaximumPeriod (PhonationGrid me);

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me);

/************************ VocalTractGrid *********************************************/

VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants,	long numberOfNasalAntiFormants);
VocalTractGridPlayOptions VocalTractGridPlayOptions_create ();
void VocalTractGrid_setNames (VocalTractGrid me);
void VocalTractGrid_draw (VocalTractGrid me, Graphics g, int filterModel);

/************************ CouplingGrid *********************************************/

CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants);
CouplingGridPlayOptions CouplingGridPlayOptions_create ();
void CouplingGrid_setNames (CouplingGrid me);
double CouplingGrid_getDeltaFormantAtTime (CouplingGrid me, long iformant, double t);
double CouplingGrid_getDeltaBandwidthAtTime (CouplingGrid me, long iformant, double t);

/********************** FormantGrid & CouplingGrid *************************************/

void FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee);

/********************** Sound & FormantGrid (& IntensityTier) *************************************/

void Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant);
void Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant);
void Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant);
Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign);

/************************ FricationGrid *********************************************/

FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants);
FricationGridPlayOptions FricationGridPlayOptions_create ();
void FricationGrid_setNames (FricationGrid me);
void FricationGrid_draw (FricationGrid me, Graphics g);

Sound FricationGrid_to_Sound (FricationGrid me, double samplingFrequency);

Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee);

/************************ Sound & VocalTractGrid & CouplingGrid *********************************************/

Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling);

/************************ KlattGrid *********************************************/

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants, long numberOfNasalAntiFormants,
	long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
	long numberOfFricationFormants, long numberOfDeltaFormants);

KlattGrid KlattGrid_createExample ();
KlattGridPlayOptions KlattGridPlayOptions_create ();
void KlattGrid_setNames (KlattGrid me);
KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration);

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel);
void klattGrid_drawPhonation (KlattGrid me, Graphics g);
void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea);

#define KlattGrid_FILTER_CASCADE 0
#define KlattGrid_FILTER_PARALLEL 1

#define KlattGrid_ORAL_FORMANTS 1
#define KlattGrid_NASAL_FORMANTS 2
#define KlattGrid_FRICATION_FORMANTS 3
#define KlattGrid_TRACHEAL_FORMANTS 4
#define KlattGrid_NASAL_ANTIFORMANTS 5
#define KlattGrid_TRACHEAL_ANTIFORMANTS 6
#define KlattGrid_DELTA_FORMANTS 7

// Add, Remove, Extract, Replace from PhonationGrid
#define PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO(Name,tierType) \
double KlattGrid_get##Name##AtTime (KlattGrid me, double t); \
void KlattGrid_add##Name##Point (KlattGrid me, double t, double value); \
void KlattGrid_remove##Name##Points (KlattGrid me, double t1, double t2); \
tierType KlattGrid_extract##Name##Tier (KlattGrid me); \
void KlattGrid_replace##Name##Tier (KlattGrid me, tierType thee);

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
void KlattGrid_add##Name##Point (KlattGrid me, int formantType, long iformant, double t, double value); \
void KlattGrid_remove##Name##Points (KlattGrid me, int formantType, long iformant, double t1, double t2); \
double KlattGrid_getDelta##Name##AtTime (KlattGrid me, long iformant, double t); \
void KlattGrid_addDelta##Name##Point (KlattGrid me, long iformant, double t, double value); \
void KlattGrid_removeDelta##Name##Points (KlattGrid me, long iformant, double t1, double t2);

// 12 prototypes
KlattGrid_QUERY_ADD_REMOVE_PROTO(Formant)
KlattGrid_QUERY_ADD_REMOVE_PROTO(Bandwidth)

void KlattGrid_formula_frequencies (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter);
void KlattGrid_formula_bandwidths (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter);
void KlattGrid_formula_amplitudes (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter);

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType);
void KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee);

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me);
void KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee);

FormantGrid KlattGrid_to_oralFormantGrid_openPhases (KlattGrid me, double fadeFraction);
PointProcess KlattGrid_extractPointProcess_glottalClosures (KlattGrid me);

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t);
void KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value);
void KlattGrid_removeAmplitudePoints (KlattGrid me, int formantType, long iformant, double t1, double t2);
IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant);
void KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee);

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t);
void KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationAmplitudePoints (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me);
void KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee);

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t);
void KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationBypassPoints (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me);
void KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee);

void KlattGrid_setGlottisCoupling (KlattGrid me);

FormantGrid * KlattGrid_getAddressOfFormantGrid (KlattGrid me, int formantType);
Ordered * KlattGrid_getAddressOfAmplitudes (KlattGrid me, int formantType);

// add/remove frequency + bandwidth (+amplitude) tiers
void KlattGrid_addFormant (KlattGrid me,int formantType, long position);
void KlattGrid_removeFormant (KlattGrid me,int formantType, long position);

// add/remove frequency + bandwidth tiers
void KlattGrid_addFormantAndBandwidthTier (KlattGrid me, int formantType, long position);
void KlattGrid_removeFormantAndBandwidthTier (KlattGrid me, int formantType, long position);

/***************** KlattGrid & Sound *************************************/

// reset PlayOptions to defaults
void KlattGrid_play (KlattGrid me);
// use playOptions
void KlattGrid_playSpecial (KlattGrid me);

void KlattGrid_setDefaultPlayOptions (KlattGrid me);

Sound KlattGrid_to_Sound (KlattGrid me);

Sound KlattGrid_to_Sound_phonation (KlattGrid me);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

/*
	glottal: phonation+aspiration, before entering the filter
	frication: noise before entering the parallel frication filter section.
*/

Sound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, int filterModel);

Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee);

KlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, long maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double minimumPitchIntensity, int subtractMean);

#endif /* _KlattGrid_h_ */
