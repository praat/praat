#ifndef _KlattGrid_h_
#define _KlattGrid_h_
/* KlattGrid.h
 *
 * Copyright (C) 2008-2019 David Weenink, 2015 Paul Boersma
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

#include "Collection.h"
#include "PointProcess.h"
#include "Sound.h"
#include "IntensityTier.h"
#include "PitchTier.h"
#include "FormantGrid.h"
#include "KlattTable.h"
Thing_declare (Interpreter);

#include "KlattGrid_enums.h"

#include "KlattGrid_def.h"

/******************** PhonationPoint & Tier ************************************/

autoPhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
	double power1, double power2, double pulseScale);

autoPhonationTier PhonationTier_create (double tmin, double tmax);
autoPointProcess PhonationTier_to_PointProcess_closures (PhonationTier me);

/************************ PhonationGrid *********************************************/

autoPhonationGrid PhonationGrid_create (double tmin, double tmax);
autoPhonationGridPlayOptions PhonationGridPlayOptions_create ();
void PhonationGrid_setNames (PhonationGrid me);

autoSound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency);

void PhonationGrid_draw (PhonationGrid me, Graphics g);

double PhonationGrid_getMaximumPeriod (PhonationGrid me);

autoPhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me);

/************************ VocalTractGrid *********************************************/

autoVocalTractGrid VocalTractGrid_create (double tmin, double tmax, integer numberOfFormants,
	integer numberOfNasalFormants,	integer numberOfNasalAntiFormants);
autoVocalTractGridPlayOptions VocalTractGridPlayOptions_create ();
void VocalTractGrid_setNames (VocalTractGrid me);
void VocalTractGrid_draw (VocalTractGrid me, Graphics g, kKlattGridFilterModel filterModel);

/************************ CouplingGrid *********************************************/

autoCouplingGrid CouplingGrid_create (double tmin, double tmax, integer numberOfTrachealFormants, integer numberOfTrachealAntiFormants, integer numberOfDeltaFormants);
autoCouplingGridPlayOptions CouplingGridPlayOptions_create ();
void CouplingGrid_setNames (CouplingGrid me);
double CouplingGrid_getDeltaFormantAtTime (CouplingGrid me, integer iformant, double t);
double CouplingGrid_getDeltaBandwidthAtTime (CouplingGrid me, integer iformant, double t);

/********************** FormantGrid & CouplingGrid *************************************/

void FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee);

/********************** Sound & FormantGrid (& IntensityTier) *************************************/

void Sound_FormantGrid_filterWithOneFormant_inplace (Sound me, FormantGrid thee, integer iformant);
void Sound_FormantGrid_filterWithOneAntiFormant_inplace (Sound me, FormantGrid thee, integer iformant);
void Sound_FormantGrid_Intensities_filterWithOneFormant_inplace (Sound me, FormantGrid thee, OrderedOf<structIntensityTier>* amplitudes, integer iformant);
autoSound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, OrderedOf<structIntensityTier>* amplitudes, integer iformantb, integer iformante, int alternatingSign);

/************************ FricationGrid *********************************************/

autoFricationGrid FricationGrid_create (double tmin, double tmax, integer numberOfFormants);
autoFricationGridPlayOptions FricationGridPlayOptions_create ();
void FricationGrid_setNames (FricationGrid me);
void FricationGrid_draw (FricationGrid me, Graphics g);

autoSound FricationGrid_to_Sound (FricationGrid me, double samplingFrequency);

autoSound Sound_FricationGrid_filter (Sound me, FricationGrid thee);

/************************ Sound & VocalTractGrid & CouplingGrid *********************************************/

autoSound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling);

/************************ KlattGrid *********************************************/

autoKlattGrid KlattGrid_create (double tmin, double tmax, integer numberOfFormants,
	integer numberOfNasalFormants, integer numberOfNasalAntiFormants,
	integer numberOfTrachealFormants, integer numberOfTrachealAntiFormants,
	integer numberOfFricationFormants, integer numberOfDeltaFormants);

autoKlattGrid KlattGrid_createFromVowel (double duration, double f0start, double f1, double b1, double f2, double b2, double f3, double b3, double f4, double bandWidthFraction, double formantFrequencyInterval);

autoKlattGrid KlattGrid_createExample ();
autoKlattGridPlayOptions KlattGridPlayOptions_create ();
void KlattGrid_setNames (KlattGrid me);
autoKlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration);

void KlattGrid_draw (KlattGrid me, Graphics g, kKlattGridFilterModel filterModel);
void klattGrid_drawPhonation (KlattGrid me, Graphics g);
void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, kKlattGridFilterModel filterModel, int withTrachea);

conststring32 KlattGrid_getFormantName (kKlattGridFormantType formantType);

// Add, Remove, Extract, Replace from PhonationGrid
#define PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE_PROTO(Name,tierType) \
double KlattGrid_get##Name##AtTime (KlattGrid me, double t); \
void KlattGrid_add##Name##Point (KlattGrid me, double t, double value); \
void KlattGrid_remove##Name##Points (KlattGrid me, double t1, double t2); \
auto##tierType KlattGrid_extract##Name##Tier (KlattGrid me); \
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
double KlattGrid_get##Name##AtTime (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t); \
void KlattGrid_add##Name##Point (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t, double value); \
void KlattGrid_remove##Name##Points (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t1, double t2); \
double KlattGrid_getDelta##Name##AtTime (KlattGrid me, integer iformant, double t); \
void KlattGrid_addDelta##Name##Point (KlattGrid me, integer iformant, double t, double value); \
void KlattGrid_removeDelta##Name##Points (KlattGrid me, integer iformant, double t1, double t2);

// 12 prototypes
KlattGrid_QUERY_ADD_REMOVE_PROTO(Formant)
KlattGrid_QUERY_ADD_REMOVE_PROTO(Bandwidth)

void KlattGrid_formula_frequencies (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter);
void KlattGrid_formula_bandwidths (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter);
void KlattGrid_formula_amplitudes (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter);

autoFormantGrid KlattGrid_extractFormantGrid (KlattGrid me, kKlattGridFormantType formantType);
void KlattGrid_replaceFormantGrid (KlattGrid me, kKlattGridFormantType formantType, FormantGrid thee);

autoFormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me);
void KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee);

autoFormantGrid KlattGrid_to_oralFormantGrid_openPhases (KlattGrid me, double fadeFraction);
autoPointProcess KlattGrid_extractPointProcess_glottalClosures (KlattGrid me);

double KlattGrid_getAmplitudeAtTime (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t);
void KlattGrid_addAmplitudePoint (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t, double value);
void KlattGrid_removeAmplitudePoints (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t1, double t2);
autoIntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer iformant);
void KlattGrid_replaceAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer iformant, IntensityTier thee);

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t);
void KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationAmplitudePoints (KlattGrid me, double t1, double t2);
autoIntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me);
void KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee);

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t);
void KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationBypassPoints (KlattGrid me, double t1, double t2);
autoIntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me);
void KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee);

void KlattGrid_setGlottisCoupling (KlattGrid me);

autoFormantGrid * KlattGrid_getAddressOfFormantGrid (KlattGrid me, kKlattGridFormantType formantType);
OrderedOf<structIntensityTier>* KlattGrid_getAddressOfAmplitudes (KlattGrid me, kKlattGridFormantType formantType);

/*
	One can add (or remove) formant frequency tiers, formant bandwidth tiers and formant amplitude tiers.
	The first two types are handled together, the third type is handled separately.
*/
void KlattGrid_addFormantFrequencyAndBandwidthTiers (KlattGrid me, kKlattGridFormantType formantType, integer position);
void KlattGrid_removeFormantFrequencyAndBandwidthTiers (KlattGrid me, kKlattGridFormantType formantType, integer position);
void KlattGrid_addFormantAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer position);
void KlattGrid_removeFormantAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer position);

/*
	The following two functions are deprecated;
	they combine the actions of the above functions,
	i.e. they add (or remove) a formant frequency tier, a formant bandwidth tier,
	and a formant amplitude tier in one go.
	Use instead the above division of these actions into two steps.
*/
void KlattGrid_addFormant (KlattGrid me, kKlattGridFormantType formantType, integer position);
void KlattGrid_removeFormant (KlattGrid me, kKlattGridFormantType formantType, integer position);

/***************** KlattGrid & Sound *************************************/

// reset PlayOptions to defaults
void KlattGrid_play (KlattGrid me);
// use playOptions
void KlattGrid_playSpecial (KlattGrid me);

void KlattGrid_setDefaultPlayOptions (KlattGrid me);

autoSound KlattGrid_to_Sound (KlattGrid me);

autoSound KlattGrid_to_Sound_phonation (KlattGrid me);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);

/*
	glottal: phonation+aspiration, before entering the filter
	frication: noise before entering the parallel frication filter section.
*/

autoSound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, kKlattGridFilterModel filterModel);

autoSound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee);

autoKlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, integer maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double minimumPitchIntensity, int subtractMean);

#endif /* _KlattGrid_h_ */
