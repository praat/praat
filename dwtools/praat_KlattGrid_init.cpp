/* praat_KlattGrid_init.cpp
 *
 * Copyright (C) 2009-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	djmw 20090420
*/

#include "praat.h"

#include "IntensityTierEditor.h"
#include "KlattGridEditors.h"
#include "KlattTable.h"
#include "praat_TimeFunction.h"

#undef iam
#define iam iam_LOOP

/******************* KlattGrid  *********************************/

#define KlattGrid_4formants_addCommonField(formantType) \
	OPTIONMENU_ENUM (kKlattGridFormantType, formantType, U"Formant type", kKlattGridFormantType::DEFAULT)

#define KlattGrid_6formants_addCommonField(formantType) \
	OPTIONMENU_ENUM (kKlattGridFormantType, formantType, U"Formant type", kKlattGridFormantType::DEFAULT)


#define KlattGrid_PhonationGridPlayOptions_addCommonFields(useVoicing,useFlutter,useDoublePulsing,useCollisionPhase,useSpectralTilt,flowFunctionType,useFlowDerivative,useAspiration,useBreathiness) \
	BOOLEAN (useVoicing, U"Voicing", true) \
	BOOLEAN (useFlutter, U"Flutter", true) \
	BOOLEAN (useDoublePulsing, U"Double pulsing", true) \
	BOOLEAN (useCollisionPhase, U"Collision phase", true) \
	BOOLEAN (useSpectralTilt, U"Spectral tilt", true) \
	OPTIONMENU (flowFunctionType, U"Flow function", 1) \
		OPTION (U"Powers in tiers") \
		OPTION (U"t^2-t^3") \
		OPTION (U"t^3-t^4") \
	BOOLEAN (useFlowDerivative, U"Flow derivative", true) \
	BOOLEAN (useAspiration, U"Aspiration", true) \
	BOOLEAN (useBreathiness, U"Breathiness", true)

static void KlattGrid_PhonationGridPlayOptions (KlattGrid me, int useVoicing, int useFlutter, int useDoublePulsing, int useCollisionPhase, int useSpectralTilt, int flowFunctionType, int useFlowDerivative, int useAspiration, int useBreathiness) {
	PhonationGridPlayOptions pp = my phonation -> options.get();
	pp -> voicing = useVoicing;
	pp -> flutter = useFlutter;
	pp -> doublePulsing = useDoublePulsing;
	pp -> collisionPhase = useCollisionPhase;
	pp -> spectralTilt = useSpectralTilt;
	pp -> flowFunction = flowFunctionType;
	pp -> flowDerivative = useFlowDerivative;
	pp -> aspiration = useAspiration;
	pp -> breathiness = useBreathiness;
}

#define KlattGrid_formantSelection_vocalTract_commonFields(filterModel,fromOralFormant,toOralFormant,fromNasalFormant,toNasalFormant,fromNasalAntiFormant,toNasalAntiFormant) \
	OPTIONMENU_ENUM (kKlattGridFilterModel, filterModel, U"Filter model", kKlattGridFilterModel::DEFAULT) \
	INTEGER (fromOralFormant, U"left Oral formant range", U"1") \
	INTEGER (toOralFormant, U"right Oral formant range", U"5") \
	INTEGER (fromNasalFormant, U"left Nasal formant range", U"1") \
	INTEGER (toNasalFormant, U"right Nasal formant range", U"1") \
	INTEGER (fromNasalAntiFormant, U"left Nasal antiformant range", U"1") \
	INTEGER (toNasalAntiFormant, U"right Nasal antiformant range", U"1")
	
static void KlattGrid_formantSelection_vocalTract (KlattGrid me, kKlattGridFilterModel filterModel, integer fromOralFormant, integer toOralFormant, integer fromNasalFormant, integer toNasalFormant, integer fromNasalAntiFormant, integer toNasalAntiFormant) {
	VocalTractGridPlayOptions pv = my vocalTract -> options.get();
	pv -> filterModel = filterModel;
	pv -> startOralFormant = fromOralFormant;
	pv -> endOralFormant  = toOralFormant;
	pv -> startNasalFormant = fromNasalFormant;
	pv -> endNasalFormant = toNasalFormant;
	pv -> startNasalAntiFormant = fromNasalAntiFormant;
	pv -> endNasalAntiFormant = toNasalAntiFormant;
}

#define KlattGrid_formantSelection_coupling_commonFields(fromTrachealFormant,toTrachealFormant,fromTrachealAntiFormant,toTrachealAntiFormant,fromDeltaFormant,toDeltaFormant,fromDeltaBandwidth,toDeltaBandwidth) \
	INTEGER (fromTrachealFormant, U"left Tracheal formant range", U"1") \
	INTEGER (toTrachealFormant, U"right Tracheal formant range", U"1") \
	INTEGER (fromTrachealAntiFormant, U"left Tracheal antiformant range", U"1") \
	INTEGER (toTrachealAntiFormant, U"right Tracheal antiformant range", U"1") \
	INTEGER (fromDeltaFormant, U"left Delta formant range", U"1") \
	INTEGER (toDeltaFormant, U"right Delta formant range", U"1") \
	INTEGER (fromDeltaBandwidth, U"left Delta bandwidth range", U"1") \
	INTEGER (toDeltaBandwidth, U"right Delta bandwidth range", U"1")

static void KlattGrid_formantSelection_coupling (KlattGrid me, integer fromTrachealFormant, integer toTrachealFormant, integer fromTrachealAntiFormant, integer toTrachealAntiFormant, integer fromDeltaFormant, integer toDeltaFormant, integer fromDeltaBandwidth, integer toDeltaBandwidth) {
	CouplingGridPlayOptions pc = my coupling -> options.get();
	pc -> startTrachealFormant = fromTrachealFormant;
	pc -> endTrachealFormant = toTrachealFormant;
	pc -> startTrachealAntiFormant = fromTrachealAntiFormant;
	pc -> endTrachealAntiFormant = toTrachealAntiFormant;
	pc -> startDeltaFormant = fromDeltaFormant;
	pc -> endDeltaFormant = toDeltaFormant;
	pc -> startDeltaBandwidth = fromDeltaBandwidth;
	pc -> endDeltaFormant = toDeltaBandwidth;
}

#define KlattGrid_formantSelection_frication_commonFields(fromFricationFormant,toFricationFormant,useFricationBypass) \
	INTEGER (fromFricationFormant, U"left Frication formant range", U"1") \
	INTEGER (toFricationFormant, U"right Frication formant range", U"6") \
	BOOLEAN (useFricationBypass, U"Frication bypass", true)

static void KlattGrid_formantSelection_frication (KlattGrid me, integer fromFricationFormant, integer toFricationFormant, int useFricationBypass) {
	FricationGridPlayOptions pf = my frication -> options.get();
	pf -> startFricationFormant = fromFricationFormant;
	pf -> endFricationFormant = toFricationFormant;
	pf -> bypass = useFricationBypass;
}

DIRECT (NEW1_KlattGrid_createExample) {
	CREATE_ONE
		autoKlattGrid result = KlattGrid_createExample();
	CREATE_ONE_END (U"example")
}

FORM (NEW1_KlattGrid_createFromVowel, U"Create KlattGrid from vowel", U"Create KlattGrid from vowel...") {
	WORD (name, U"Name", U"a")
	POSITIVE (duration, U"Duration (s)", U"0.4")
	POSITIVE (f0start, U"Pitch (Hz)", U"125.0")
	REAL (f1, U"F1 (Hz)", U"800.0")
	POSITIVE (b1, U"B1 (Hz)", U"50.0")
	REAL (f2, U"F2 (Hz)", U"1200.0")
	POSITIVE (b2, U"B2 (Hz)", U"50.0")
	REAL (f3, U"F3 (Hz)", U"2300.0")
	POSITIVE (b3, U"B3 (Hz)", U"100.0")
	REAL (f4, U"F4 (Hz)", U"2800.0")
	POSITIVE (bandWidthFraction, U"Bandwidth fraction", U"0.05")
	REAL (formantFrequencyInterval, U"Formant frequency interval (Hz)", U"1000.0")
	OK
DO
	CREATE_ONE
		autoKlattGrid result = KlattGrid_createFromVowel (duration, f0start, f1, b1, f2, b2, f3, b3, f4, bandWidthFraction, formantFrequencyInterval);
	CREATE_ONE_END (name)
}

FORM (NEW1_KlattGrid_create, U"Create KlattGrid", U"Create KlattGrid...") {
	WORD (name, U"Name", U"kg")
	REAL (fromTime, U"Start time (s)", U"0.0")
	REAL (toTime, U"End time (s)", U"1.0")
	INTEGER (numberOfOralFormants, U"Number of oral formants", U"6")
	INTEGER (numberOfNasalFormants, U"Number of nasal formants", U"1")
	INTEGER (numberOfNasalAntiFormants, U"Number of nasal antiformants", U"1")
	INTEGER (numberOfFricationFormants, U"Number of frication formants", U"6")
	LABEL (U"Coupling between source and filter")
	INTEGER (numberOfTrachealFormants, U"Number of tracheal formants", U"1")
	INTEGER (numberOfTrachealAntiFormants, U"Number of tracheal antiformants", U"1")
	INTEGER (numberOfDeltaFormants, U"Number of delta formants", U"1")
	OK
DO
	Melder_require (fromTime < toTime, U"The start time must lie before the end time.");
	Melder_require (numberOfOralFormants >= 0 && numberOfNasalFormants >= 0 && numberOfNasalAntiFormants >= 0
		&& numberOfTrachealFormants >= 0 && numberOfTrachealAntiFormants >= 0
		&& numberOfFricationFormants >= 0 && numberOfDeltaFormants >= 0,
		U"No number of formants should be negative.");
	CREATE_ONE
		autoKlattGrid result = KlattGrid_create (fromTime, toTime, numberOfOralFormants, numberOfNasalFormants, numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfFricationFormants, numberOfDeltaFormants);
	CREATE_ONE_END (name)
}


#define KlattGrid_INSTALL_TIER_EDITOR(Name) \
DIRECT (WINDOW_KlattGrid_edit##Name##Tier)  {\
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	FIND_ONE_WITH_IOBJECT (KlattGrid) \
		auto##KlattGrid_##Name##TierEditor editor = KlattGrid_##Name##TierEditor_create (ID_AND_FULL_NAME, me); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	END }

KlattGrid_INSTALL_TIER_EDITOR (Pitch)
KlattGrid_INSTALL_TIER_EDITOR (VoicingAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (Flutter)
KlattGrid_INSTALL_TIER_EDITOR (Power1)
KlattGrid_INSTALL_TIER_EDITOR (Power2)
KlattGrid_INSTALL_TIER_EDITOR (OpenPhase)
KlattGrid_INSTALL_TIER_EDITOR (CollisionPhase)
KlattGrid_INSTALL_TIER_EDITOR (DoublePulsing)
KlattGrid_INSTALL_TIER_EDITOR (AspirationAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (BreathinessAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (SpectralTilt)

KlattGrid_INSTALL_TIER_EDITOR (FricationBypass)
KlattGrid_INSTALL_TIER_EDITOR (FricationAmplitude)

#undef KlattGrid_INSTALL_TIER_EDITOR

#define KlattGRID_EDIT_FORMANTGRID(Name,formantType)  \
DIRECT (WINDOW_KlattGrid_edit##Name##FormantGrid) { \
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	LOOP { \
		iam (KlattGrid); \
		conststring32 id_and_name = Melder_cat (ID, U". ", KlattGrid_getFormantName (formantType), U" grid"); \
		autoKlattGrid_FormantGridEditor editor = KlattGrid_FormantGridEditor_create (id_and_name, me, formantType); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	} \
END }

KlattGRID_EDIT_FORMANTGRID (Oral, kKlattGridFormantType::ORAL)
KlattGRID_EDIT_FORMANTGRID (Nasal, kKlattGridFormantType::NASAL)
KlattGRID_EDIT_FORMANTGRID (Tracheal, kKlattGridFormantType::TRACHEAL)
KlattGRID_EDIT_FORMANTGRID (NasalAnti, kKlattGridFormantType::NASAL_ANTI)
KlattGRID_EDIT_FORMANTGRID (TrachealAnti, kKlattGridFormantType::TRACHEALANTI)
KlattGRID_EDIT_FORMANTGRID (Delta, kKlattGridFormantType::DELTA)
KlattGRID_EDIT_FORMANTGRID (Frication, kKlattGridFormantType::FRICATION)

#undef KlattGRID_EDIT_FORMANTGRID

#define KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER(Name,name,formantType)  \
FORM (WINDOW_KlattGrid_edit##Name##FormantAmplitudeTier, U"KlattGrid: View & Edit " #name " formant amplitude tier", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	OK \
DO \
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	LOOP { \
		iam (KlattGrid); \
		OrderedOf<structIntensityTier>* amp = KlattGrid_getAddressOfAmplitudes (me, formantType); \
		if (! amp) Melder_throw (U"Unknown formant type"); \
		if (formantNumber > amp->size) Melder_throw (U"Formant number does not exist."); \
		conststring32 id_and_name = Melder_cat (ID, U". ", KlattGrid_getFormantName (formantType), U" amplitude tier"); \
		autoKlattGrid_DecibelTierEditor editor = KlattGrid_DecibelTierEditor_create (id_and_name, me, amp->at [formantNumber]); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	} \
END }

KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Oral, oral, kKlattGridFormantType::ORAL)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Nasal, nasal, kKlattGridFormantType::NASAL)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Tracheal, tracheal, kKlattGridFormantType::TRACHEAL)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Frication, frication, kKlattGridFormantType::FRICATION)

#undef KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER

#define KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE(Name,name,unit,default,requireCondition,requireMessage,newname,tiertype)  \
FORM (REAL_KlattGrid_get##Name##AtTime, U"KlattGrid: Get " #name " at time", nullptr) { \
	REAL (time, U"Time", U"0.5") \
	OK \
DO \
	NUMBER_ONE (KlattGrid) \
		double result = KlattGrid_get##Name##AtTime (me, time); \
	NUMBER_ONE_END (unit) \
} \
FORM (MODIFY_KlattGrid_add##Name##Point, U"KlattGrid: Add " #name " point", nullptr) { \
	REAL (time, U"Time (s)", U"0.5") \
	REAL (value, U"Value" unit, default) \
	OK \
DO \
	Melder_require (requireCondition, requireMessage); \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_add##Name##Point (me, time, value); \
	MODIFY_EACH_END \
} \
FORM (MODIFY_KlattGrid_remove##Name##Points, U"Remove " #name " points", nullptr) { \
	REAL (fromTime, U"From time (s)", U"0.3") \
	REAL (toTime, U"To time (s)", U"0.7") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_remove##Name##Points (me, fromTime, toTime); \
	MODIFY_EACH_END \
} \
DIRECT (NEW_KlattGrid_extract##Name##Tier) { \
	CONVERT_EACH (KlattGrid) \
		autoDaata result = KlattGrid_extract##Name##Tier (me); \
	CONVERT_EACH_END (newname) \
} \
DIRECT (MODIFY_KlattGrid_replace##Name##Tier) { \
	MODIFY_FIRST_OF_TWO (KlattGrid, tiertype); \
		KlattGrid_replace##Name##Tier (me, you); \
	MODIFY_FIRST_OF_TWO_END \
}

// 55 DO_KlattGrid... functions
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Pitch, pitch, U" (Hz)", (U"100.0"),
        (value >= 0.0), (U"Pitch should be greater equal zero."), U"f0", PitchTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (VoicingAmplitude, voicing amplitude, U" (dB SPL)", U"90.0",
        (1), U"", U"voicing", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Flutter, flutter, U" (0..1)", (U"0.0"),
        (value >= 0.0 && value <= 1.0), (U"Flutter should be in [0,1]."), U"flutter", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power1, power1, U"", U"3",
        (value > 0.0), U"Power1 needs to be positive.", U"power1", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power2, power2, U"", U"4",
        (value > 0.0), U"Power2 needs to be positive.", U"power2", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (OpenPhase, open phase, U"", U"0.7",
        (value >= 0.0 && value <= 1.0), U"Open phase should be greater than zero and smaller equal one.", U"openPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (CollisionPhase, collision phase, U"", U"0.03",
        (value >= 0.0 && value < 1.0), U"Collision phase should be greater equal zero and smaller than one.", U"collisionPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (DoublePulsing, double pulsing, U" (0..1)", U"0.0",
        (value >= 0.0 && value <= 1.0), U"Double pulsing should be greater equal zero and smaller equal one.", U"doublePulsing", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (SpectralTilt, spectral tilt, U" (dB)", U"0.0",
        (value >= 0.0), U"Spectral tilt should be greater equal zero.", U"spectralTilt", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (AspirationAmplitude, aspiration amplitude, U" (dB SPL)", U"0.0",
        (true), U"", U"aspiration", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (BreathinessAmplitude, breathiness amplitude, U" (dB SPL)", U"30.0",
        (true), U"", U"breathiness", IntensityTier)

KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationAmplitude, frication amplitude, U" (dB SPL)", U"30.0",
        (true), U"", U"frication", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationBypass, frication bypass, U" (dB)", U"30.0",
        (true), U"", U"bypass", IntensityTier)

#undef KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE

#define KlattGrid_FORMULA_FORMANT_FBA_VALUE(Name,namef,ForBs,forbs,textfield,formantType,label)  \
FORM (MODIFY_KlattGrid_formula##Name##Formant##ForBs, U"KlattGrid: Formula (" #namef "ormant " #forbs ")", U"Formant: Formula (" #forbs ")...") { \
	LABEL (U"row is formant number, col is point number:\nfor row from 1 to nrow do for col from 1 to ncol do " #ForBs " (row, col) :=") \
	TEXTFIELD (formula, U"Formula:", textfield) \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_formula_##forbs (me, formantType, formula, interpreter); \
	MODIFY_EACH_END \
}

#define KlattGrid_ADD_FBA_VALUE(Name,namef,Form,FBA,fba,formantType,default,unit,requireCondition,requireMessage)  \
FORM (MODIFY_KlattGrid_add##Name##Formant##FBA##Point, U"KlattGrid: Add " #namef "ormant " #fba " point", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	REAL (value, U"Value " #unit, default) \
	OK \
DO \
	Melder_require (requireCondition, requireMessage); \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_add##Form##Point (me, formantType, formantNumber, time, value); \
	MODIFY_EACH_END \
}

#define KlattGrid_REMOVE_FBA_VALUE(Name,namef,Form,FBA,fba,formantType)  \
FORM (MODIFY_KlattGrid_remove##Name##Formant##FBA##Points, U"KlattGrid: Remove " #namef "ormant " #fba " points", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (fromTime, U"From time (s)", U"0.3")\
	REAL (toTime, U"To time (s)", U"0.7") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_remove##Form##Points (me, formantType, formantNumber, fromTime, toTime); \
	MODIFY_EACH_END \
}

#define KlattGrid_ADD_FORMANT(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_add##Name##Formant, U"KlattGrid: Add " #namef "ormant", nullptr) { \
	INTEGER (position, U"Position", U"0 (= at end)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_addFormant (me, formantType, position); \
	MODIFY_EACH_END \
}

#define KlattGrid_REMOVE_FORMANT(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_remove##Name##Formant, U"KlattGrid: Remove " #namef "ormant", nullptr) { \
	INTEGER (position, U"Position", U"0 (= do nothing)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_removeFormant (me, formantType, position); \
	MODIFY_EACH_END \
}

#define KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers, U"KlattGrid: Add " #namef "ormant", nullptr) { \
	INTEGER (position, U"Position", U"0 (= at end)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_addFormantFrequencyAndBandwidthTiers (me, formantType, position); \
	MODIFY_EACH_END \
}

#define KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers, U"KlattGrid: Remove " #namef "ormant", nullptr) { \
	INTEGER (position, U"Position", U"0 (= at end)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_removeFormantFrequencyAndBandwidthTiers (me, formantType, position); \
	MODIFY_EACH_END \
}

#define KlattGrid_ADD_FORMANT_AMPLITUDETIER(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_add##Name##FormantAmplitudeTier, U"KlattGrid: Add " #namef "ormant amplitude tier", nullptr) { \
	INTEGER (position, U"Position", U"0 (= at end)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_addFormantAmplitudeTier (me, formantType, position); \
	MODIFY_EACH_END \
}

#define KlattGrid_REMOVE_FORMANT_AMPLITUDETIER(Name,namef,formantType)  \
FORM (MODIFY_KlattGrid_remove##Name##FormantAmplitudeTier, U"KlattGrid: Remove " #namef "ormant amplitude tier", nullptr) { \
	INTEGER (position, U"Position", U"0 (= at end)") \
	OK \
DO \
	MODIFY_EACH (KlattGrid); \
		KlattGrid_removeFormant (me, formantType, position); \
	MODIFY_EACH_END \
}


#define KlattGrid_FORMULA_ADD_REMOVE_FBA(Name,namef,formantType)  \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi", formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency", formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType, U"500.0", (Hz), (value>0), U"Frequency should be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType, U"50.0", (Hz), (value>0), U"Bandwidth should be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType, U"0.0", (dB), (isdefined(value)), U"Amplitude should be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType) \
KlattGrid_ADD_FORMANT (Name, namef, formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT_AMPLITUDETIER (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT (Name, namef, formantType) \
KlattGrid_ADD_FORMANT_AMPLITUDETIER (Name, namef, formantType)


#define KlattGrid_FORMULA_ADD_REMOVE_FB(Name,namef,formantType)  \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi",formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency",formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, U"500.0", (Hz), (value>0), U"Frequency should be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  U"50.0", (Hz), (value>0), U"Bandwidth should be greater than zero.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT (Name, namef, formantType)

#define KlattGrid_FORMULA_ADD_REMOVE_FB_DELTA(Name,namef,formantType)  \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi",formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency",formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, U"-100.0", (Hz), (isdefined(value)), U"Frequency should be defined.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  U"-50.0", (Hz), (isdefined(value)), U"Bandwidth should be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS (Name, namef, formantType) \
KlattGrid_ADD_FORMANT (Name, namef, formantType) \
KlattGrid_REMOVE_FORMANT (Name, namef, formantType)

KlattGrid_FORMULA_ADD_REMOVE_FBA (Oral, oral f, kKlattGridFormantType::ORAL)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Nasal, nasal f, kKlattGridFormantType::NASAL)
KlattGrid_FORMULA_ADD_REMOVE_FB (NasalAnti, nasal antif, kKlattGridFormantType::NASAL_ANTI)
KlattGrid_FORMULA_ADD_REMOVE_FB_DELTA (Delta, delta f, kKlattGridFormantType::DELTA)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Tracheal, tracheal f, kKlattGridFormantType::TRACHEAL)
KlattGrid_FORMULA_ADD_REMOVE_FB (TrachealAnti, tracheal antif, kKlattGridFormantType::TRACHEALANTI)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Frication, frication f, kKlattGridFormantType::FRICATION)

#undef KlattGrid_FORMULA_ADD_REMOVE_FB
#undef KlattGrid_FORMULA_ADD_REMOVE
#undef KlattGrid_ADD_FORMANT_AND_BANDWDTH_TIER
#undef KlattGrid_REMOVE_FBA_VALUE
#undef KlattGrid_ADD_FBA_VALUE
#undef KlattGrid_FORMULA_FORMANT_FB_VALUE

DIRECT (NEW_KlattGrid_extractPointProcess_glottalClosures) {
	CONVERT_EACH (KlattGrid)
		autoPointProcess result = KlattGrid_extractPointProcess_glottalClosures (me);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_KlattGrid_formula_frequencies, U"KlattGrid: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	KlattGrid_6formants_addCommonField (formantType)
	LABEL (U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (formula, U"Formula:", U"if row = 2 then self + 200 else self fi")
	OK
DO
	MODIFY_EACH (KlattGrid)
		KlattGrid_formula_frequencies (me, formantType, formula, interpreter);
	MODIFY_EACH_END
}

FORM (MODIFY_KlattGrid_formula_bandwidths, U"KlattGrid: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	KlattGrid_6formants_addCommonField (formantType)
	LABEL (U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (formula, U"Formula:", U"if row = 2 then self + 200 else self fi")
	OK
DO
	MODIFY_EACH (KlattGrid)
		KlattGrid_formula_bandwidths (me, formantType, formula, interpreter);
	MODIFY_EACH_END
}

#define KlattGrid_FORMANT_GET_FB_VALUE(Name,name,ForB,forb,FormB,formantType)  \
FORM (REAL_KlattGrid_get##Name##Formant##ForB##AtTime, U"KlattGrid: Get " #name " " #forb " at time", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##FormB##AtTime (me, formantType, formantNumber, time), U" (Hz)"); \
	} \
END }

#define KlattGrid_FORMANT_GET_A_VALUE(Name,name,formantType)  \
FORM (REAL_KlattGrid_get##Name##FormantAmplitudeAtTime, U"KlattGrid: Get " #name " formant amplitude at time", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
	Melder_informationReal (KlattGrid_getAmplitudeAtTime (me, formantType, formantNumber, time), U" (dB)"); \
	} \
END }

#define KlattGrid_FORMANT_GET_FB_VALUES(Name,name,formantType) \
KlattGrid_FORMANT_GET_FB_VALUE (Name, name, Frequency, frequency, Formant, formantType) \
KlattGrid_FORMANT_GET_FB_VALUE (Name, name, Bandwidth, bandwidth, Bandwidth, formantType)

KlattGrid_FORMANT_GET_FB_VALUES (Oral, oral, kKlattGridFormantType::ORAL)
KlattGrid_FORMANT_GET_A_VALUE (Oral, oral, kKlattGridFormantType::ORAL)
KlattGrid_FORMANT_GET_FB_VALUES (Nasal, nasal, kKlattGridFormantType::NASAL)
KlattGrid_FORMANT_GET_A_VALUE (Nasal, nasal, kKlattGridFormantType::NASAL)
KlattGrid_FORMANT_GET_FB_VALUES (NasalAnti, nasal anti, kKlattGridFormantType::NASAL_ANTI)
KlattGrid_FORMANT_GET_FB_VALUES (Tracheal, tracheal f, kKlattGridFormantType::TRACHEAL)
KlattGrid_FORMANT_GET_A_VALUE (Tracheal, tracheal f, kKlattGridFormantType::TRACHEAL)
KlattGrid_FORMANT_GET_FB_VALUES (Delta, delta f, kKlattGridFormantType::DELTA)
KlattGrid_FORMANT_GET_FB_VALUES (TrachealAnti, tracheal antif, kKlattGridFormantType::TRACHEALANTI)
KlattGrid_FORMANT_GET_FB_VALUES (Frication, frication, kKlattGridFormantType::FRICATION)
KlattGrid_FORMANT_GET_A_VALUE (Frication, frication, kKlattGridFormantType::FRICATION)

#undef KlattGrid_FORMANT_GET_FB_VALUES
#undef KlattGrid_FORMANT_GET_A_VALUE

#define KlattGrid_EXTRACT_FORMANT_GRID(Name,gridType)  \
DIRECT (NEW_KlattGrid_extract##Name##FormantGrid) { \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extractFormantGrid (me, gridType), KlattGrid_getFormantName (gridType)); \
	} \
END }

#define KlattGrid_EXTRACT_FORMANT_AMPLITUDE(Name,name,formantType)  \
FORM (NEW_KlattGrid_extract##Name##FormantAmplitudeTier, U"KlattGrid: Extract " #name " formant amplitude tier", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extractAmplitudeTier (me, formantType, formantNumber), KlattGrid_getFormantName (formantType)); \
	} \
END }

KlattGrid_EXTRACT_FORMANT_GRID (Oral, kKlattGridFormantType::ORAL)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Oral, oral, kKlattGridFormantType::ORAL)
KlattGrid_EXTRACT_FORMANT_GRID (Nasal, kKlattGridFormantType::NASAL)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Nasal, nasal, kKlattGridFormantType::NASAL)
KlattGrid_EXTRACT_FORMANT_GRID (Frication, kKlattGridFormantType::FRICATION)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Frication, frication, kKlattGridFormantType::FRICATION)
KlattGrid_EXTRACT_FORMANT_GRID (Tracheal, kKlattGridFormantType::TRACHEAL)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Tracheal, tracheal, kKlattGridFormantType::TRACHEAL)
KlattGrid_EXTRACT_FORMANT_GRID (NasalAnti, kKlattGridFormantType::NASAL_ANTI)
KlattGrid_EXTRACT_FORMANT_GRID (TrachealAnti, kKlattGridFormantType::TRACHEALANTI)
KlattGrid_EXTRACT_FORMANT_GRID (Delta, kKlattGridFormantType::DELTA)

#undef KlattGrid_EXTRACT_FORMANTGRID

#define KlattGrid_REPLACE_FORMANT_GRID(Name,formantType)  \
DIRECT (MODIFY_KlattGrid_replace##Name##FormantGrid) { \
	MODIFY_FIRST_OF_TWO (KlattGrid, FormantGrid); \
		KlattGrid_replaceFormantGrid (me, formantType, you); \
	MODIFY_FIRST_OF_TWO_END \
}

#define KlattGrid_REPLACE_FORMANT_AMPLITUDE(Name,name,formantType)  \
FORM (MODIFY_KlattGrid_replace##Name##FormantAmplitudeTier, U"KlattGrid: Replace " #name " formant amplitude tier", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	OK \
DO \
	MODIFY_FIRST_OF_TWO (KlattGrid, IntensityTier); \
		KlattGrid_replaceAmplitudeTier (me, formantType, formantNumber, you); \
	MODIFY_FIRST_OF_TWO_END \
}

KlattGrid_REPLACE_FORMANT_GRID (Oral, kKlattGridFormantType::ORAL)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Oral, oral, kKlattGridFormantType::ORAL)
KlattGrid_REPLACE_FORMANT_GRID (Nasal, kKlattGridFormantType::NASAL)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Nasal, nasal, kKlattGridFormantType::NASAL)
KlattGrid_REPLACE_FORMANT_GRID (NasalAnti, kKlattGridFormantType::NASAL_ANTI)
KlattGrid_REPLACE_FORMANT_GRID (Tracheal, kKlattGridFormantType::TRACHEAL)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Tracheal, tracheal, kKlattGridFormantType::TRACHEAL)
KlattGrid_REPLACE_FORMANT_GRID (TrachealAnti, kKlattGridFormantType::TRACHEALANTI)
KlattGrid_REPLACE_FORMANT_GRID (Delta, kKlattGridFormantType::DELTA)
KlattGrid_REPLACE_FORMANT_GRID (Frication, kKlattGridFormantType::FRICATION)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Frication, frication, kKlattGridFormantType::FRICATION)

#undef KlattGrid_REPLACE_FORMANT_AMPLITUDE
#undef KlattGrid_REPLACE_FORMANTGRID

#define KlattGrid_FORMANT_GET_ADD_REMOVE(Name,name,unit,default,requireCondition,requireMessage)  \
FORM (REAL_KlattGrid_get##Name##AtTime, U"KlattGrid: Get " #name " at time", nullptr) { \
	KlattGrid_6formants_addCommonField (formantType) \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##Name##AtTime (me, formantType, formantNumber, time), U" (Hz)"); \
	} \
END } \
FORM (REAL_KlattGrid_getDelta##Name##AtTime, U"KlattGrid: Get delta " #name " at time", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_getDelta##Name##AtTime (me, formantNumber, time), U" (Hz)"); \
	} \
END } \
FORM (MODIFY_KlattGrid_add##Name##Point, U"KlattGrid: Add " #name " point", nullptr) { \
	KlattGrid_6formants_addCommonField (formantType) \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	REAL (value, U"Value" unit, default) \
	OK \
DO \
	Melder_require (requireCondition, requireMessage); \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Name##Point (me, formantType, formantNumber, time, value); \
		praat_dataChanged (me); \
	} \
END } \
FORM (MODIFY_KlattGrid_addDelta##Name##Point, U"KlattGrid: Add delta " #name " point", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (time, U"Time (s)", U"0.5") \
	REAL (value, U"Value" unit, default) \
	OK \
DO \
	Melder_require (requireCondition, requireMessage); \
	LOOP { iam (KlattGrid); \
		KlattGrid_addDelta##Name##Point (me, formantNumber, time, value); \
		praat_dataChanged (me); \
	} \
END } \
FORM (MODIFY_KlattGrid_remove##Name##Points, U"Remove " #name " points", nullptr) { \
	KlattGrid_6formants_addCommonField (formantType) \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (fromTime, U"From time (s)", U"0.3")\
	REAL (toTime, U"To time (s)", U"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Name##Points (me, formantType, formantNumber, fromTime, toTime); \
		praat_dataChanged (me);\
	} \
END } \
FORM (MODIFY_KlattGrid_removeDelta##Name##Points, U"Remove delta " #name " points", nullptr) { \
	NATURAL (formantNumber, U"Formant number", U"1") \
	REAL (fromTime, U"From time (s)", U"0.3")\
	REAL (toTime, U"To time (s)", U"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeDelta##Name##Points (me, formantNumber, fromTime, toTime); \
		praat_dataChanged (me);\
	} \
END }

KlattGrid_FORMANT_GET_ADD_REMOVE (Formant, formant, U" (Hz)", U"500.0", (value > 0), U"Frequency should be greater than zero.")
KlattGrid_FORMANT_GET_ADD_REMOVE (Bandwidth, bandwidth, U" (Hz)", U"50.0", (value > 0), U"Bandwidth should be greater than zero.")

#undef KlattGrid_FORMANT_GET_ADD_REMOVE

FORM (MODIFY_KlattGrid_addFormantAndBandwidthTier, U"", nullptr) {
	OPTIONMENU_ENUM (kKlattGridFormantType, formantType, U"Formant type", kKlattGridFormantType::DEFAULT)
	INTEGER (position, U"Position", U"0 (= at end)")
	OK
DO
	MODIFY_EACH (KlattGrid)
		KlattGrid_addFormantFrequencyAndBandwidthTiers (me, formantType, position);
	MODIFY_EACH_END
}

FORM (NEW_KlattGrid_extractFormantGrid, U"KlattGrid: Extract formant grid", nullptr) {
	KlattGrid_6formants_addCommonField (formantType)
	OK
DO
	CONVERT_EACH (KlattGrid)
		autoFormantGrid result = KlattGrid_extractFormantGrid (me, formantType);
	CONVERT_EACH_END (KlattGrid_getFormantName (formantType))
}

FORM (MODIFY_KlattGrid_replaceFormantGrid, U"KlattGrid: Replace formant grid", nullptr) {
	KlattGrid_6formants_addCommonField (formantType)
	OK
DO
	MODIFY_FIRST_OF_TWO (KlattGrid, FormantGrid)
		KlattGrid_replaceFormantGrid (me, formantType, you);
	MODIFY_FIRST_OF_TWO_END
}

FORM (REAL_KlattGrid_getAmplitudeAtTime, U"KlattGrid: Get amplitude at time", nullptr) {
	KlattGrid_4formants_addCommonField (formantType)
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (KlattGrid)
		double result = KlattGrid_getAmplitudeAtTime (me, formantType, formantNumber, time);
	NUMBER_ONE_END (U" dB")
}

FORM (MODIFY_KlattGrid_addAmplitudePoint, U"KlattGrid: Add amplitude point", nullptr) {
	KlattGrid_4formants_addCommonField (formantType)
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.5")
	REAL (value_hz, U"Value (Hz)", U"80.0")
	OK
DO
	MODIFY_EACH (KlattGrid)
		KlattGrid_addAmplitudePoint (me, formantType, formantNumber, time, value_hz);
	MODIFY_EACH_END
}

FORM (MODIFY_KlattGrid_removeAmplitudePoints, U"Remove amplitude points", nullptr) {
	KlattGrid_4formants_addCommonField (formantType)
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (fromTime, U"From time (s)", U"0.3")
	REAL (toTime, U"To time (s)", U"0.7")
	OK
DO
	MODIFY_EACH (KlattGrid);
		KlattGrid_removeAmplitudePoints (me, formantType, formantNumber, fromTime, toTime);
	MODIFY_EACH_END
}

FORM (NEW_KlattGrid_extractAmplitudeTier, U"", nullptr) {
	KlattGrid_4formants_addCommonField (formantType)
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH (KlattGrid)
		autoIntensityTier result = KlattGrid_extractAmplitudeTier (me, formantType, formantNumber);
	CONVERT_EACH_END (KlattGrid_getFormantName (formantType))
}

FORM (MODIFY_KlattGrid_replaceAmplitudeTier, U"KlattGrid: Replace amplitude tier", nullptr) {
	KlattGrid_4formants_addCommonField (formantType)
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	MODIFY_FIRST_OF_TWO (KlattGrid, IntensityTier)
		KlattGrid_replaceAmplitudeTier (me, formantType, formantNumber, you);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW_KlattGrid_to_Sound_special, U"KlattGrid: To Sound (special)", U"KlattGrid: To Sound (special)...") {
	REAL (fromTime, U"left Time range (s)", U"0")
	REAL (toTime, U"right Time range (s)", U"0")
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	BOOLEAN (scalePeak, U"Scale peak", true)
	KlattGrid_PhonationGridPlayOptions_addCommonFields (useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness)
	KlattGrid_formantSelection_vocalTract_commonFields (filtersStructure, fromOralFormant, toOralFormant, fromNasalFormant, toNasalFormant, fromNasalAntiFormant, toNasalAntiFormant)
	KlattGrid_formantSelection_coupling_commonFields (fromTrachealFormant, toTrachealFormant, fromTrachealAntiFormant, toTrachealAntiFormant, fromDeltaFormant, toDeltaFormant, fromDeltaBandwidth, toDeltaBandwidth)
	KlattGrid_formantSelection_frication_commonFields(fromFricationFormant,toFricationFormant,useFricationBypass)
	OK
DO
	CONVERT_EACH (KlattGrid)
		KlattGrid_setDefaultPlayOptions (me);
		KlattGridPlayOptions pk = my options.get();
		pk -> scalePeak = scalePeak;
		pk -> xmin = fromTime;
		pk -> xmax = toTime;
		pk -> samplingFrequency = samplingFrequency;
		pk -> scalePeak = scalePeak;
		KlattGrid_PhonationGridPlayOptions (me, useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness);
		KlattGrid_formantSelection_vocalTract (me, filtersStructure, fromOralFormant, toOralFormant, fromNasalFormant, toNasalFormant, fromNasalAntiFormant, toNasalAntiFormant);
		KlattGrid_formantSelection_coupling (me, fromTrachealFormant, toTrachealFormant, fromTrachealAntiFormant, toTrachealAntiFormant, fromDeltaFormant, toDeltaFormant, fromDeltaBandwidth, toDeltaBandwidth);
		KlattGrid_formantSelection_frication (me, fromFricationFormant, toFricationFormant, useFricationBypass);
		autoSound result = KlattGrid_to_Sound (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_KlattGrid_to_Sound) {
	CONVERT_EACH (KlattGrid)
		KlattGrid_setDefaultPlayOptions (me);
		autoSound result = KlattGrid_to_Sound (me);
	CONVERT_EACH_END (my name.get())
}

FORM (PLAY_KlattGrid_playSpecial, U"KlattGrid: Play special", U"KlattGrid: Play special...") {
	REAL (fromTime, U"left Time range (s)", U"0")
	REAL (toTime, U"right Time range (s)", U"0")
	BOOLEAN (scalePeak, U"Scale peak", true)
	KlattGrid_PhonationGridPlayOptions_addCommonFields (useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness)
	KlattGrid_formantSelection_vocalTract_commonFields (filtersStructure, fromOralFormant, toOralFormant, fromNasalFormant, toNasalFormant, fromNasalAntiFormant, toNasalAntiFormant);
	KlattGrid_formantSelection_coupling_commonFields (fromTrachealFormant, toTrachealFormant, fromTrachealAntiFormant, toTrachealAntiFormant, fromDeltaFormant, toDeltaFormant, fromDeltaBandwidth, toDeltaBandwidth)
	KlattGrid_formantSelection_frication_commonFields(fromFricationFormant,toFricationFormant,useFricationBypass)
	OK
DO
	PLAY_EACH (KlattGrid)
		KlattGrid_setDefaultPlayOptions (me);
		KlattGridPlayOptions pk = my options.get();
		pk -> scalePeak = scalePeak;
		pk -> xmin = fromTime;
		pk -> xmax = toTime;
		pk -> scalePeak = scalePeak;
		KlattGrid_PhonationGridPlayOptions (me, useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness);
		KlattGrid_formantSelection_vocalTract (me, filtersStructure, fromOralFormant, toOralFormant, fromNasalFormant, toNasalFormant, fromNasalAntiFormant, toNasalAntiFormant);
		KlattGrid_formantSelection_coupling (me, fromTrachealFormant, toTrachealFormant, fromTrachealAntiFormant, toTrachealAntiFormant, fromDeltaFormant, toDeltaFormant, fromDeltaBandwidth, toDeltaBandwidth);
		KlattGrid_formantSelection_frication (me, fromFricationFormant, toFricationFormant, useFricationBypass);
		KlattGrid_playSpecial (me);
	PLAY_EACH_END
}

FORM (NEW_KlattGrid_to_Sound_phonation, U"KlattGrid: To Sound (phonation)", U"KlattGrid: To Sound (phonation)...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	KlattGrid_PhonationGridPlayOptions_addCommonFields (useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness)
	OK
DO
	CONVERT_EACH (KlattGrid)
		KlattGrid_PhonationGridPlayOptions (me, useVoicing, useFlutter, useDoublePulsing, useCollisionPhase, useSpectralTilt, flowFunctionType, useFlowDerivative, useAspiration, useBreathiness);
		my options -> samplingFrequency = samplingFrequency;
		autoSound result = KlattGrid_to_Sound_phonation (me);
	CONVERT_EACH_END (my name.get(), U"_phonation")
}

DIRECT (HELP_KlattGrid_help)  {
	HELP (U"KlattGrid")
}

DIRECT (PLAY_KlattGrid_play) {
	PLAY_EACH (KlattGrid)
		KlattGrid_play (me);
	PLAY_EACH_END
}

FORM (GRAPHICS_KlattGrid_draw, U"KlattGrid: Draw", nullptr) {
	RADIO_ENUM (kKlattGridFilterModel, filterModel, U"Synthesis filter model", kKlattGridFilterModel::DEFAULT)
	OK
DO
	GRAPHICS_EACH (KlattGrid)
		KlattGrid_draw (me, GRAPHICS, filterModel);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_KlattGrid_drawVocalTract, U"KlattGrid: Draw vocal tract", nullptr) {
	RADIO_ENUM (kKlattGridFilterModel, filterModel, U"Synthesis filter model", kKlattGridFilterModel::DEFAULT)
	BOOLEAN (includeTrachealFormants, U"Include tracheal formants", true);
	OK
DO
	GRAPHICS_EACH (KlattGrid)
		KlattGrid_drawVocalTract (me, GRAPHICS, filterModel, includeTrachealFormants);
	GRAPHICS_EACH_END
}

DIRECT (GRAPHICS_KlattGrid_drawPhonation) {
	GRAPHICS_EACH (KlattGrid)
		PhonationGrid_draw (my phonation.get(), GRAPHICS);
	GRAPHICS_EACH_END
}

DIRECT (GRAPHICS_KlattGrid_drawFrication) {
	GRAPHICS_EACH (KlattGrid)
		FricationGrid_draw (my frication.get(), GRAPHICS);
	GRAPHICS_EACH_END
}

FORM (NEW_KlattGrid_to_oralFormantGrid_openPhases, U"KlattGrid: Extract oral formant grid (open phases)", U"KlattGrid: Extract oral formant grid (open phases)...") {
	REAL (fadeFraction, U"Fade fraction (0..0.5)", U"0.1")
	OK
DO
	Melder_require (fadeFraction < 0.5, U"The fade fraction should be less than 0.5.");
	CONVERT_EACH (KlattGrid)
		autoFormantGrid result = KlattGrid_to_oralFormantGrid_openPhases (me, fadeFraction);
	CONVERT_EACH_END (U"corrected")
}

FORM (NEW_Sound_KlattGrid_filterByVocalTract, U"Sound & KlattGrid: Filter by vocal tract", U"Sound & KlattGrid: Filter by vocal tract...") {
	RADIO_ENUM (kKlattGridFilterModel, filterModel, U"Vocal tract filter model", kKlattGridFilterModel::DEFAULT)
	OK
DO
	CONVERT_TWO (Sound, KlattGrid)
		autoSound result = Sound_KlattGrid_filterByVocalTract (me, you, filterModel);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

void praat_KlattGrid_init ();
void praat_KlattGrid_init () {

	Thing_recognizeClassesByName (classKlattGrid, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Acoustic synthesis (Klatt)", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"KlattGrid help", nullptr, praat_DEPTH_1 | praat_NO_API, HELP_KlattGrid_help);
	praat_addMenuCommand (U"Objects", U"New", U"-- the synthesizer grid --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattGrid...", nullptr, 1, NEW1_KlattGrid_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattGrid from vowel...", nullptr, 1, NEW1_KlattGrid_createFromVowel);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattGrid example", nullptr, praat_DEPTH_1 + praat_HIDDEN, NEW1_KlattGrid_createExample);

	/*
	Edit oral/nasal/tracheal/frication/delta formant grid
	Edit nasal/tracheal antiformant grid
	Get oral/nasal/tracheal/frication/delta formant at time...
	Get nasal/tracheal antiformant at time...
	Get oral/nasal/tracheal/frication/delta formant bandwidth at time...
	Get nasal/tracheal antiformant bandwidth at time...
	Get oral/nasal/tracheal/frication formant amplitude at time...
	Formula (oral/nasal/tracheal/frication/delta formant frequencies)...
	Formula (nasal/tracheal antiformant frequencies)...
	Formula (oral/nasal/tracheal/frication/delta formant bandwidths)...
	Formula (nasal/tracheal antiformant bandwidths)...
	Add oral/nasal/tracheal/frication/delta formant point...
	Add nasal/tracheal antiformant point...
	Add oral/nasal/tracheal/frication/delta formant bandwidth point...
	Add nasal/tracheal antiformant bandwidth point...
	Add oral/nasal/tracheal/frication formant amplitude point...
	Remove oral/nasal/tracheal/frication/delta formant points...
	Remove nasal/tracheal antiformant points...
	Remove oral/nasal/tracheal/frication/delta bandwidth points...
	Remove nasal/tracheal antiformant bandwidth points...
	Remove oral/nasal/tracheal/frication formant amplitude points...
	Extract oral/nasal/tracheal/frication/delta formant grid
	Extract nasal/tracheal antiformant grid
	Replace oral/nasal/tracheal/frication/delta formant grid
	Replace nasal/tracheal antiformant grid
	Add oral/nasal/tracheal/frication/delta formant and bandwidth tier
	Add nasal/tracheal antiformant and bandwidth tier
	*/
	praat_addAction1 (classKlattGrid, 0, U"KlattGrid help", nullptr, 0, HELP_KlattGrid_help);
	praat_addAction1 (classKlattGrid, 0, U"Edit phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit pitch tier", nullptr, 1, WINDOW_KlattGrid_editPitchTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit voicing amplitude tier", nullptr, 1, WINDOW_KlattGrid_editVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit flutter tier", nullptr, 1, WINDOW_KlattGrid_editFlutterTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit power1 tier", nullptr, 1, WINDOW_KlattGrid_editPower1Tier);
	praat_addAction1 (classKlattGrid, 0, U"Edit power2 tier", nullptr, 1, WINDOW_KlattGrid_editPower2Tier);
	praat_addAction1 (classKlattGrid, 0, U"Edit open phase tier", nullptr, 1, WINDOW_KlattGrid_editOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit collision phase tier", nullptr, 1, WINDOW_KlattGrid_editCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit double pulsing tier", nullptr, 1, WINDOW_KlattGrid_editDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit spectral tilt tier", nullptr, 1, WINDOW_KlattGrid_editSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit aspiration amplitude tier", nullptr, 1, WINDOW_KlattGrid_editAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit breathiness amplitude tier", nullptr, 1, WINDOW_KlattGrid_editBreathinessAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Edit filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit oral formant grid", nullptr, 1, WINDOW_KlattGrid_editOralFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal formant grid", nullptr, 1, WINDOW_KlattGrid_editNasalFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal antiformant grid", nullptr, 1, WINDOW_KlattGrid_editNasalAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit oral formant amplitude tier...", nullptr, 1, WINDOW_KlattGrid_editOralFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal formant amplitude tier...", nullptr, 1, WINDOW_KlattGrid_editNasalFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- edit delta formant grid --", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit delta formant grid", nullptr, 1, WINDOW_KlattGrid_editDeltaFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal formant grid", nullptr, 1, WINDOW_KlattGrid_editTrachealFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal antiformant grid", nullptr, 1, WINDOW_KlattGrid_editTrachealAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal formant amplitude tier...", nullptr, 1, WINDOW_KlattGrid_editTrachealFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- edit frication tiers --", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Edit frication amplitude tier", nullptr, 1, WINDOW_KlattGrid_editFricationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication formant grid", nullptr, 1, WINDOW_KlattGrid_editFricationFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication formant amplitude tier...", nullptr, 1, WINDOW_KlattGrid_editFricationFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication bypass tier", nullptr, 1, WINDOW_KlattGrid_editFricationBypassTier);
	praat_addAction1 (classKlattGrid, 1, U"Edit frication amplitude tier", nullptr, 1, WINDOW_KlattGrid_editFricationAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Play", nullptr, 0, PLAY_KlattGrid_play);
	praat_addAction1 (classKlattGrid, 0, U"Play special...", nullptr, 0, PLAY_KlattGrid_playSpecial);
	praat_addAction1 (classKlattGrid, 0, U"To Sound", nullptr, 0, NEW_KlattGrid_to_Sound);
	praat_addAction1 (classKlattGrid, 0, U"To Sound (special)...", nullptr, 0, NEW_KlattGrid_to_Sound_special);
	praat_addAction1 (classKlattGrid, 0, U"To Sound (phonation)...", nullptr, 0, NEW_KlattGrid_to_Sound_phonation);

	praat_addAction1 (classKlattGrid, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Draw synthesizer...", nullptr, 1, GRAPHICS_KlattGrid_draw);
	praat_addAction1 (classKlattGrid, 0, U"Draw vocal tract...", nullptr, 1, GRAPHICS_KlattGrid_drawVocalTract);
	praat_addAction1 (classKlattGrid, 0, U"Draw phonation", nullptr, 1, GRAPHICS_KlattGrid_drawPhonation);
	praat_addAction1 (classKlattGrid, 0, U"Draw frication", nullptr, 1, GRAPHICS_KlattGrid_drawFrication);

	praat_addAction1 (classKlattGrid, 0, U"Query phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Get pitch at time...", nullptr, 1, REAL_KlattGrid_getPitchAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get voicing amplitude at time...", nullptr, 1, REAL_KlattGrid_getVoicingAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get flutter at time...", nullptr, 1, REAL_KlattGrid_getFlutterAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get power1 at time...", nullptr, 1, REAL_KlattGrid_getPower1AtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get power2 at time...", nullptr, 1, REAL_KlattGrid_getPower2AtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get open phase at time...", nullptr, 1, REAL_KlattGrid_getOpenPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get collision phase at time...", nullptr, 1, REAL_KlattGrid_getCollisionPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get double pulsing at time...", nullptr, 1, REAL_KlattGrid_getDoublePulsingAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get spectral tilt at time...", nullptr, 1, REAL_KlattGrid_getSpectralTiltAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get aspiration amplitude at time...", nullptr, 1, REAL_KlattGrid_getAspirationAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get breathiness amplitude at time...", nullptr, 1, REAL_KlattGrid_getBreathinessAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, U"Query filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Get formant at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, REAL_KlattGrid_getFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get bandwidth at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, REAL_KlattGrid_getBandwidthAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get amplitude at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, REAL_KlattGrid_getAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get delta formant at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, REAL_KlattGrid_getDeltaFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get delta bandwidth at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, REAL_KlattGrid_getDeltaBandwidthAtTime);

#define KlattGRID_GET_FORMANT_FB_VALUES_ACTION(Name, formantname) \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " frequency at time...", nullptr, 1, REAL_KlattGrid_get##Name##FormantFrequencyAtTime); \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " bandwidth at time...", nullptr, 1, REAL_KlattGrid_get##Name##FormantBandwidthAtTime);

#define KlattGRID_GET_FORMANT_A_VALUES_ACTION(Name,formantname) \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " amplitude at time...", nullptr, 1, REAL_KlattGrid_get##Name##FormantAmplitudeAtTime); \

	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Oral, oral formant)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Oral, oral formant)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Nasal, nasal formant)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Nasal, nasal formant)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (NasalAnti, nasal antiformant)

	praat_addAction1 (classKlattGrid, 1, U"-- query delta characteristics", nullptr, 1, nullptr);
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Delta, delta formant)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Tracheal, tracheal formant)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Tracheal, tracheal formant)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (TrachealAnti, tracheal antiformant)
	praat_addAction1 (classKlattGrid, 1, U"-- query frication characteristics", nullptr, 1, nullptr);
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Frication, frication formant)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Frication, frication formant)

#undef KlattGRID_GET_FORMANT_A_VALUES_ACTION
#undef KlattGRID_GET_FORMANT_A_VALUES_ACTION

	praat_addAction1 (classKlattGrid, 1, U"Get frication bypass at time...", nullptr, 1, REAL_KlattGrid_getFricationBypassAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get frication amplitude at time...", nullptr, 1, REAL_KlattGrid_getFricationAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, U"Modify phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Add pitch point...", nullptr, 1, MODIFY_KlattGrid_addPitchPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add voicing amplitude point...", nullptr, 1, MODIFY_KlattGrid_addVoicingAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add flutter point...", nullptr, 1, MODIFY_KlattGrid_addFlutterPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add power1 point...", nullptr, 1, MODIFY_KlattGrid_addPower1Point);
	praat_addAction1 (classKlattGrid, 0, U"Add power2 point...", nullptr, 1, MODIFY_KlattGrid_addPower2Point);
	praat_addAction1 (classKlattGrid, 0, U"Add open phase point...", nullptr, 1, MODIFY_KlattGrid_addOpenPhasePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add collision phase point...", nullptr, 1, MODIFY_KlattGrid_addCollisionPhasePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add double pulsing point...", nullptr, 1, MODIFY_KlattGrid_addDoublePulsingPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add spectral tilt point...", nullptr, 1, MODIFY_KlattGrid_addSpectralTiltPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add aspiration amplitude point...", nullptr, 1, MODIFY_KlattGrid_addAspirationAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add breathiness amplitude point...", nullptr, 1, MODIFY_KlattGrid_addBreathinessAmplitudePoint);

#define KlattGrid_REMOVE_POINTS_ACTION(Name,name)  \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #name " points between...", nullptr, praat_DEPTH_1+praat_HIDDEN, MODIFY_KlattGrid_remove##Name##Points); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #name " points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##Points);

	KlattGrid_REMOVE_POINTS_ACTION (Pitch, pitch)
	KlattGrid_REMOVE_POINTS_ACTION (VoicingAmplitude, voicing amplitude)
	KlattGrid_REMOVE_POINTS_ACTION (Flutter, flutter)
	KlattGrid_REMOVE_POINTS_ACTION (Power1, power1)
	KlattGrid_REMOVE_POINTS_ACTION (Power2, power2)
	KlattGrid_REMOVE_POINTS_ACTION (OpenPhase, open phase)
	KlattGrid_REMOVE_POINTS_ACTION (CollisionPhase, collision phase)
	KlattGrid_REMOVE_POINTS_ACTION (DoublePulsing, double pulsing)
	KlattGrid_REMOVE_POINTS_ACTION (SpectralTilt, spectral tilt)
	KlattGrid_REMOVE_POINTS_ACTION (AspirationAmplitude, aspiration amplitude)
	KlattGrid_REMOVE_POINTS_ACTION (BreathinessAmplitude, breathiness amplitude)

	praat_addAction1 (classKlattGrid, 0, U"Modify vocal tract -", nullptr, 0, nullptr);

#define KlattGrid_MODIFY_ACTIONS_FBA(Name,formantname)  \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " frequencies)...", nullptr, 1, MODIFY_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " bandwidths)...", nullptr, 1, MODIFY_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency point...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " bandwidth point...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " amplitude point...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantAmplitudePoint); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " bandwidth points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " amplitude points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantAmplitudePoints); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency and bandwidth tiers...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency and bandwidth tiers...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " amplitude tier...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantAmplitudeTier); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " amplitude tier...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantAmplitudeTier); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_remove##Name##Formant);

#define KlattGrid_MODIFY_ACTIONS_FB(Name,formantname)  \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " frequencies)...", nullptr, 1, MODIFY_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " bandwidths)...", nullptr, 1, MODIFY_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency point...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " bandwidth point...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " bandwidth points...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency and bandwidth tiers...", nullptr, 1, MODIFY_KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency and bandwidth tiers...", nullptr, 1, MODIFY_KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_remove##Name##Formant);

	KlattGrid_MODIFY_ACTIONS_FBA (Oral, oral formant)
	praat_addAction1 (classKlattGrid, 0, U"-- oral modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Nasal, nasal formant)
	praat_addAction1 (classKlattGrid, 0, U"-- nasal modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (NasalAnti, nasal antiformant)

	praat_addAction1 (classKlattGrid, 0, U"Formula (frequencies)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_formula_frequencies);
	praat_addAction1 (classKlattGrid, 0, U"Formula (bandwidths)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_formula_bandwidths);
	praat_addAction1 (classKlattGrid, 0, U"Add formant point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addFormantPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add bandwidth point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add amplitude point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Remove formant points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_removeFormantPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove bandwidth points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_removeBandwidthPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove amplitude points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_removeAmplitudePoints);
	praat_addAction1 (classKlattGrid, 0, U"Modify coupling - ", nullptr, 0, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (Delta, delta formant)
	praat_addAction1 (classKlattGrid, 0, U"-- delta modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Tracheal, tracheal formant)
	praat_addAction1 (classKlattGrid, 0, U"-- nasal modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (TrachealAnti, tracheal antiformant)

	praat_addAction1 (classKlattGrid, 0, U"Add delta formant point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addDeltaFormantPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add delta bandwidth point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addDeltaBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, U"Remove delta formant points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_removeDeltaFormantPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove delta bandwidth points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_removeDeltaBandwidthPoints);

	praat_addAction1 (classKlattGrid, 0, U"Modify frication -", nullptr, 0, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Frication, frication formant)
	praat_addAction1 (classKlattGrid, 0, U"-- frication modify separator --", nullptr, 1, nullptr);

	praat_addAction1 (classKlattGrid, 0, U"Add frication bypass point...", nullptr, 1, MODIFY_KlattGrid_addFricationBypassPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add frication amplitude point...", nullptr, 1, MODIFY_KlattGrid_addFricationAmplitudePoint);
	KlattGrid_REMOVE_POINTS_ACTION (FricationBypass, frication bypass)
	KlattGrid_REMOVE_POINTS_ACTION (FricationAmplitude, frication amplitude)
	praat_addAction1 (classKlattGrid, 0, U"Add formant and bandwidth tier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, MODIFY_KlattGrid_addFormantAndBandwidthTier);

#undef KlattGrid_REMOVE_POINTS_ACTION
#undef KlattGrid_MODIFY_ACTION_FB
#undef KlattGrid_MODIFY_ACTION_FBA

	praat_addAction1 (classKlattGrid, 0, U"Extract phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract pitch tier", nullptr, 1, NEW_KlattGrid_extractPitchTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract voicing amplitude tier", nullptr, 1, NEW_KlattGrid_extractVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract flutter tier", nullptr, 1, NEW_KlattGrid_extractFlutterTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract power1 tier", nullptr, 1, NEW_KlattGrid_extractPower1Tier);
	praat_addAction1 (classKlattGrid, 0, U"Extract power2 tier", nullptr, 1, NEW_KlattGrid_extractPower2Tier);
	praat_addAction1 (classKlattGrid, 0, U"Extract open phase tier", nullptr, 1, NEW_KlattGrid_extractOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract collision phase tier", nullptr, 1, NEW_KlattGrid_extractCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract double pulsing tier", nullptr, 1, NEW_KlattGrid_extractDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract spectral tilt tier", nullptr, 1, NEW_KlattGrid_extractSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract aspiration amplitude tier", nullptr, 1, NEW_KlattGrid_extractAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract breathiness amplitude tier", nullptr, 1, NEW_KlattGrid_extractBreathinessAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- extract glottal events--", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract PointProcess (glottal closures)", nullptr, 1, NEW_KlattGrid_extractPointProcess_glottalClosures);

#define KlattGRID_EXTRACT_FORMANT_GRID_ACTION(Name,namef)  \
	praat_addAction1 (classKlattGrid, 0, U"Extract " #namef "ormant grid", nullptr, 1, NEW_KlattGrid_extract##Name##FormantGrid);
#define KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION(Name,name)  \
	praat_addAction1 (classKlattGrid, 0, U"Extract " #name " formant amplitude tier...", nullptr, 1, NEW_KlattGrid_extract##Name##FormantAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Extract filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract formant grid...", nullptr, praat_DEPTH_1 + praat_HIDDEN + praat_NO_API, NEW_KlattGrid_extractFormantGrid); // deprecated
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Oral, oral f)
	praat_addAction1 (classKlattGrid, 0, U"Extract amplitude tier...", nullptr, praat_DEPTH_1 + praat_HIDDEN + praat_NO_API, NEW_KlattGrid_extractAmplitudeTier); // deprecated
	praat_addAction1 (classKlattGrid, 0, U"Extract formant grid (open phases)...", nullptr, praat_HIDDEN + praat_DEPTH_1, NEW_KlattGrid_to_oralFormantGrid_openPhases);
	praat_addAction1 (classKlattGrid, 0, U"Extract oral formant grid (open phases)...", nullptr, 1, NEW_KlattGrid_to_oralFormantGrid_openPhases);
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Oral, oral)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Nasal, nasal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Nasal, nasal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (NasalAnti, nasal antif)

	praat_addAction1 (classKlattGrid, 0, U"-- extract delta characteristics", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract delta formant grid", nullptr, 1, NEW_KlattGrid_extractDeltaFormantGrid);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Tracheal, tracheal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (TrachealAnti, tracheal antif)
	praat_addAction1 (classKlattGrid, 0, U"-- extract frication characteristics", nullptr, 1, nullptr);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Frication, frication f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Frication, frication)
	praat_addAction1 (classKlattGrid, 0, U"Extract frication bypass tier", nullptr, 1, NEW_KlattGrid_extractFricationBypassTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract frication amplitude tier", nullptr, 1, NEW_KlattGrid_extractFricationAmplitudeTier);

#undef KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION
#undef KlattGRID_EXTRACT_FORMANT_GRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classPitchTier, 1, U"Replace pitch tier", nullptr, 1, MODIFY_KlattGrid_replacePitchTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace flutter tier", nullptr, 1, MODIFY_KlattGrid_replaceFlutterTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace power1 tier", nullptr, 1, MODIFY_KlattGrid_replacePower1Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace power2 tier", nullptr, 1, MODIFY_KlattGrid_replacePower2Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace open phase tier", nullptr, 1, MODIFY_KlattGrid_replaceOpenPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace collision phase tier", nullptr, 1, MODIFY_KlattGrid_replaceCollisionPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace double pulsing tier", nullptr, 1, MODIFY_KlattGrid_replaceDoublePulsingTier);

	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"-- replace formant amplitudes --", nullptr, 1, nullptr);

#define KlattGrid_REPLACE_FORMANTGRID_ACTION(Name,namef)  \
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"Replace " #namef "ormant grid", nullptr, 1, MODIFY_KlattGrid_replace##Name##FormantGrid);
#define KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION(Name,namef)  \
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace " #namef "ormant amplitude tier...", nullptr, 1, MODIFY_KlattGrid_replace##Name##FormantAmplitudeTier);

	KlattGrid_REPLACE_FORMANTGRID_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (NasalAnti, nasal antif)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"-- replace coupling --", nullptr, 1, nullptr);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (TrachealAnti, tracheal antif)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Delta, delta f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"-- replace frication --", nullptr, 1, nullptr);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"Replace formant grid...", nullptr, praat_HIDDEN + praat_DEPTH_1, MODIFY_KlattGrid_replaceFormantGrid);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace voicing amplitude tier", nullptr, 1, MODIFY_KlattGrid_replaceVoicingAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace spectral tilt tier", nullptr, 1, MODIFY_KlattGrid_replaceSpectralTiltTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace aspiration amplitude tier", nullptr, 1, MODIFY_KlattGrid_replaceAspirationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace breathiness amplitude tier", nullptr, 1, MODIFY_KlattGrid_replaceBreathinessAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace amplitude tier...", nullptr, praat_HIDDEN + praat_DEPTH_1, MODIFY_KlattGrid_replaceAmplitudeTier);
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace frication amplitude tier", nullptr, 1, MODIFY_KlattGrid_replaceFricationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace frication bypass tier", nullptr, 1, MODIFY_KlattGrid_replaceFricationBypassTier);

#undef KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION
#undef KlattGrid_REPLACE_FORMANTGRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classSound, 1, U"Filter by vocal tract...", nullptr, 1, NEW_Sound_KlattGrid_filterByVocalTract);

	INCLUDE_MANPAGES (manual_KlattGrid)
}

/* End of file praat_KlattGrid_init.cpp 1290*/
