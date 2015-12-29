/* praat_KlattGrid_init.cpp
 *
 * Copyright (C) 2009-2015 David Weenink
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
	djmw 20090420
*/

#include "praat.h"

#include "IntensityTierEditor.h"
#include "KlattGridEditors.h"
#include "KlattTable.h"

#undef iam
#define iam iam_LOOP


/******************* KlattGrid  *********************************/

static const char32 *formant_names[] = { U"", U"oral ", U"nasal ", U"frication ", U"tracheal ", U"nasal anti", U"tracheal anti", U"delta "};

static void KlattGrid_4formants_addCommonField (UiForm dia) {
	UiField radio;
	OPTIONMENU (U"Formant type", 1)
		OPTION (U"Normal formant")
		OPTION (U"Nasal formant")
		OPTION (U"Frication formant")
		OPTION (U"Tracheal formant")
}

static void KlattGrid_6formants_addCommonField (UiForm dia) {
	UiField radio;
	OPTIONMENU (U"Formant type", 1)
		OPTION (U"Normal formant")
		OPTION (U"Nasal formant")
		OPTION (U"Frication formant")
		OPTION (U"Tracheal formant")
		OPTION (U"Nasal antiformant")
		OPTION (U"Tracheal antiformant")
		//	OPTION (U"Delta formant")
}

static void KlattGrid_7formants_addCommonField (UiForm dia) {
	UiField radio;
	OPTIONMENU (U"Formant type", 1)
		OPTION (U"Normal formant")
		OPTION (U"Nasal formant")
		OPTION (U"Frication formant")
		OPTION (U"Tracheal formant")
		OPTION (U"Nasal antiformant")
		OPTION (U"Tracheal antiformant")
		OPTION (U"Delta formant")
}

static void KlattGrid_PhonationGridPlayOptions_addCommonFields (UiForm dia) {
	UiField radio;
	//LABEL (U"", U"Phonation options")
	BOOLEAN (U"Voicing", true)
	BOOLEAN (U"Flutter", true)
	BOOLEAN (U"Double pulsing", true)
	BOOLEAN (U"Collision phase", true)
	BOOLEAN (U"Spectral tilt", true)
	OPTIONMENU (U"Flow function", 1)
		OPTION (U"Powers in tiers")
		OPTION (U"t^2-t^3")
		OPTION (U"t^3-t^4")
	BOOLEAN (U"Flow derivative", true)
	BOOLEAN (U"Aspiration", true)
	BOOLEAN (U"Breathiness", true)
}

static void KlattGrid_PhonationGridPlayOptions_getCommonFields (UiForm dia, KlattGrid thee) {
	PhonationGridPlayOptions pp = thy phonation -> options.get();
	pp -> voicing = GET_INTEGER (U"Voicing");
	pp -> flutter = GET_INTEGER (U"Flutter");
	pp -> doublePulsing = GET_INTEGER (U"Double pulsing");
	pp -> collisionPhase = GET_INTEGER (U"Collision phase");
	pp -> spectralTilt = GET_INTEGER (U"Spectral tilt");
	pp -> flowFunction = GET_INTEGER (U"Flow function");
	pp -> flowDerivative = GET_INTEGER (U"Flow derivative");
	pp -> aspiration = GET_INTEGER (U"Aspiration");
	pp -> breathiness = GET_INTEGER (U"Breathiness");
}

static void KlattGrid_PlayOptions_addCommonFields (UiForm dia, bool hasSound) {
	UiField radio;
	//LABEL (U"", U"Time domain")
	REAL (U"left Time range (s)", U"0")
	REAL (U"right Time range (s)", U"0")
	if (hasSound) {
		POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	}
	BOOLEAN (U"Scale peak", true)
	KlattGrid_PhonationGridPlayOptions_addCommonFields (dia);
	OPTIONMENU (U"Filter options", 1)
		OPTION (U"Cascade")
		OPTION (U"Parallel")
	INTEGER (U"left Oral formant range", U"1")
	INTEGER (U"right Oral formant range", U"5")
	INTEGER (U"left Nasal formant range", U"1")
	INTEGER (U"right Nasal formant range", U"1")
	INTEGER (U"left Nasal antiformant range", U"1")
	INTEGER (U"right Nasal antiformant range", U"1")
	//LABEL (U"", U"Coupling options")
	INTEGER (U"left Tracheal formant range", U"1")
	INTEGER (U"right Tracheal formant range", U"1")
	INTEGER (U"left Tracheal antiformant range", U"1")
	INTEGER (U"right Tracheal antiformant range", U"1")
	INTEGER (U"left Delta formant range", U"1")
	INTEGER (U"right Delta formant range", U"1")
	INTEGER (U"left Delta bandwidth range", U"1")
	INTEGER (U"right Delta bandwidth range", U"1")
	//LABEL (U"", U"Frication options")
	INTEGER (U"left Frication formant range", U"1")
	INTEGER (U"right Frication formant range", U"6")
	BOOLEAN (U"Frication bypass", true)
}

static void KlattGrid_PlayOptions_getCommonFields (UiForm dia, bool hasSound, KlattGrid thee) {
	KlattGrid_setDefaultPlayOptions (thee);
	KlattGridPlayOptions pk = thy options.get();
	pk -> scalePeak = GET_INTEGER (U"Scale peak");
	pk -> xmin = GET_REAL (U"left Time range");
	pk -> xmax = GET_REAL (U"right Time range");
	if (hasSound) {
		pk -> samplingFrequency = GET_REAL (U"Sampling frequency");
	}
	pk -> scalePeak = GET_INTEGER (U"Scale peak");
	KlattGrid_PhonationGridPlayOptions_getCommonFields (dia, thee);
	VocalTractGridPlayOptions pv = thy vocalTract -> options.get();
	pv -> filterModel = ( GET_INTEGER (U"Filter options") == 1 ? KlattGrid_FILTER_CASCADE : KlattGrid_FILTER_PARALLEL );
	pv -> startOralFormant = GET_INTEGER (U"left Oral formant range");
	pv -> endOralFormant  = GET_INTEGER (U"right Oral formant range");
	pv -> startNasalFormant = GET_INTEGER (U"left Nasal formant range");
	pv -> endNasalFormant = GET_INTEGER (U"right Nasal formant range");
	pv -> startNasalAntiFormant = GET_INTEGER (U"left Nasal antiformant range");
	pv -> endNasalAntiFormant = GET_INTEGER (U"right Nasal antiformant range");
	CouplingGridPlayOptions pc = thy coupling -> options.get();
	pc -> startTrachealFormant = GET_INTEGER (U"left Tracheal formant range");
	pc -> endTrachealFormant = GET_INTEGER (U"right Tracheal formant range");
	pc -> startTrachealAntiFormant = GET_INTEGER (U"left Tracheal antiformant range");
	pc -> endTrachealAntiFormant = GET_INTEGER (U"right Tracheal antiformant range");
	pc -> startDeltaFormant = GET_INTEGER (U"left Delta formant range");
	pc -> endDeltaFormant = GET_INTEGER (U"right Delta formant range");
	pc -> startDeltaBandwidth = GET_INTEGER (U"left Delta bandwidth range");
	pc -> endDeltaFormant = GET_INTEGER (U"right Delta bandwidth range");
	FricationGridPlayOptions pf = thy frication -> options.get();
	pf -> startFricationFormant = GET_INTEGER (U"left Frication formant range");
	pf -> endFricationFormant = GET_INTEGER (U"right Frication formant range");
	pf -> bypass = GET_INTEGER (U"Frication bypass");
}

DIRECT (KlattGrid_createExample)
	praat_new (KlattGrid_createExample(), U"example");
END

FORM (KlattGrid_create, U"Create KlattGrid", U"Create KlattGrid...")
	WORD (U"Name", U"kg")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	INTEGER (U"Number of oral formants", U"6")
	INTEGER (U"Number of nasal formants", U"1")
	INTEGER (U"Number of nasal antiformants", U"1")
	INTEGER (U"Number of frication formants", U"6")
	LABEL (U"", U"Coupling between source and filter")
	INTEGER (U"Number of tracheal formants", U"1")
	INTEGER (U"Number of tracheal antiformants", U"1")
	INTEGER (U"Number of delta formants", U"1")
	OK
DO
	double tmin = GET_REAL (U"Start time");
	double tmax = GET_REAL (U"End time");
	REQUIRE (tmin < tmax, U"The start time must lie before the end time.")
	long numberOfOralFormants = GET_INTEGER (U"Number of oral formants");
	long numberOfNasalFormants = GET_INTEGER (U"Number of nasal formants");
	long numberOfNasalAntiFormants = GET_INTEGER (U"Number of nasal antiformants");
	long numberOfTrachealFormants = GET_INTEGER (U"Number of tracheal formants");
	long numberOfTrachealAntiFormants = GET_INTEGER (U"Number of tracheal antiformants");
	long numberOfFricationFormants = GET_INTEGER (U"Number of frication formants");
	long numberOfDeltaFormants = GET_INTEGER (U"Number of delta formants");
	REQUIRE (numberOfOralFormants >= 0 && numberOfNasalFormants >= 0 && numberOfNasalAntiFormants >= 0
		&& numberOfTrachealFormants >= 0 && numberOfTrachealAntiFormants >= 0
		&& numberOfFricationFormants >= 0 && numberOfDeltaFormants >= 0,
		U"The number of (..) formants cannot be negative!")
	praat_new (KlattGrid_create (tmin, tmax, numberOfOralFormants,
		numberOfNasalFormants, numberOfNasalAntiFormants,
		numberOfTrachealFormants, numberOfTrachealAntiFormants,
		numberOfFricationFormants, numberOfDeltaFormants),
		GET_STRING (U"Name"));
END


#define KlattGrid_INSTALL_TIER_EDITOR(Name) \
DIRECT (KlattGrid_edit##Name##Tier) \
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	LOOP {\
		iam (KlattGrid); \
		auto##KlattGrid_##Name##TierEditor editor = KlattGrid_##Name##TierEditor_create (ID_AND_FULL_NAME, me); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	}\
END

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

#define KlattGRID_EDIT_FORMANTGRID(Name,formantType) \
DIRECT (KlattGrid_edit##Name##FormantGrid) \
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	LOOP {\
		iam (KlattGrid); \
		const char32 *id_and_name = Melder_cat (ID, U". ", formant_names[formantType], U"formant grid"); \
		autoKlattGrid_FormantGridEditor editor = KlattGrid_FormantGridEditor_create (id_and_name, me, formantType); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	} \
END

KlattGRID_EDIT_FORMANTGRID (Oral, KlattGrid_ORAL_FORMANTS)
KlattGRID_EDIT_FORMANTGRID (Nasal, KlattGrid_NASAL_FORMANTS)
KlattGRID_EDIT_FORMANTGRID (Tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGRID_EDIT_FORMANTGRID (NasalAnti, KlattGrid_NASAL_ANTIFORMANTS)
KlattGRID_EDIT_FORMANTGRID (TrachealAnti, KlattGrid_TRACHEAL_ANTIFORMANTS)
KlattGRID_EDIT_FORMANTGRID (Delta, KlattGrid_DELTA_FORMANTS)
KlattGRID_EDIT_FORMANTGRID (Frication, KlattGrid_FRICATION_FORMANTS)

#undef KlattGRID_EDIT_FORMANTGRID

#define KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER(Name,name,formantType) \
FORM (KlattGrid_edit##Name##FormantAmplitudeTier, U"KlattGrid: View & Edit " #name "formant amplitude tier", nullptr) \
	NATURAL (U"Formant number", U"1") \
	OK \
DO \
	long formantNumber = GET_INTEGER (U"Formant number"); \
	if (theCurrentPraatApplication -> batch) { Melder_throw (U"Cannot edit a KlattGrid from batch."); } \
	LOOP { \
		iam (KlattGrid); \
		OrderedOf<structIntensityTier>* amp = KlattGrid_getAddressOfAmplitudes (me, formantType); \
		if (! amp) Melder_throw (U"Unknown formant type"); \
		if (formantNumber > amp->size()) Melder_throw (U"Formant number does not exist."); \
		const char32 *id_and_name = Melder_cat (ID, U". ", formant_names[formantType], U"formant amplitude tier"); \
		autoKlattGrid_DecibelTierEditor editor = KlattGrid_DecibelTierEditor_create (id_and_name, me, (*amp) [formantNumber]); \
		praat_installEditor (editor.get(), IOBJECT); \
		editor.releaseToUser(); \
	} \
END

KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Tracheal, tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Frication, frication, KlattGrid_FRICATION_FORMANTS)

#undef KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER

#define KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE(Name,name,unit,default,require,requiremessage,newname,tiertype) \
FORM (KlattGrid_get##Name##AtTime, U"KlattGrid: Get " #name " at time", nullptr) \
	REAL (U"Time", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
	Melder_informationReal (KlattGrid_get##Name##AtTime (me, GET_REAL (U"Time")), unit); \
	}\
END \
FORM (KlattGrid_add##Name##Point, U"KlattGrid: Add " #name " point", nullptr) \
	REAL (U"Time (s)", U"0.5") \
	REAL (U"Value" unit, default) \
	OK \
DO \
	double value = GET_REAL (U"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Name##Point (me, GET_REAL (U"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_remove##Name##Points, U"Remove " #name " points", nullptr) \
	REAL (U"From time (s)", U"0.3")\
	REAL (U"To time (s)", U"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Name##Points (me, GET_REAL (U"From time"), GET_REAL (U"To time")); \
		praat_dataChanged (me);\
	} \
END \
DIRECT (KlattGrid_extract##Name##Tier) \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extract##Name##Tier (me), newname); \
	} \
END \
DIRECT (KlattGrid_replace##Name##Tier) \
	KlattGrid me = FIRST(KlattGrid); \
	tiertype thee = FIRST(tiertype); \
	KlattGrid_replace##Name##Tier (me, thee); \
	praat_dataChanged (me);\
END

// 55 DO_KlattGrid... functions
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Pitch, pitch, U" (Hz)", (U"100.0"),
        (value >= 0.0), (U"Pitch must be greater equal zero."), U"f0", PitchTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (VoicingAmplitude, voicing amplitude, U" (dB SPL)", U"90.0",
        (1), U"", U"voicing", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Flutter, flutter, U" (0..1)", (U"0.0"),
        (value >= 0.0 && value <= 1.0), (U"Flutter must be in [0,1]."), U"flutter", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power1, power1, U"", U"3",
        (value > 0.0), U"Power1 needs to be positive.", U"power1", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power2, power2, U"", U"4",
        (value > 0.0), U"Power2 needs to be positive.", U"power2", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (OpenPhase, open phase, U"", U"0.7",
        (value >= 0.0 && value <= 1.0), U"Open phase must be greater than zero and smaller equal one.", U"openPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (CollisionPhase, collision phase, U"", U"0.03",
        (value >= 0.0 && value < 1.0), U"Collision phase must be greater equal zero and smaller than one.", U"collisionPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (DoublePulsing, double pulsing, U" (0..1)", U"0.0",
        (value >= 0.0 && value <= 1.0), U"Double pulsing must be greater equal zero and smaller equal one.", U"doublePulsing", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (SpectralTilt, spectral tilt, U" (dB)", U"0.0",
        (value >= 0.0), U"Spectral tilt must be greater equal zero.", U"spectralTilt", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (AspirationAmplitude, aspiration amplitude, U" (dB SPL)", U"0.0",
        (true), U"", U"aspiration", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (BreathinessAmplitude, breathiness amplitude, U" (dB SPL)", U"30.0",
        (true), U"", U"breathiness", IntensityTier)

KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationAmplitude, frication amplitude, U" (dB SPL)", U"30.0",
        (true), U"", U"frication", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationBypass, frication bypass, U" (dB)", U"30.0",
        (true), U"", U"bypass", IntensityTier)

#undef KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE

#define KlattGrid_FORMULA_FORMANT_FBA_VALUE(Name,namef,ForBs,forbs,textfield,formantType,label) \
FORM (KlattGrid_formula##Name##Formant##ForBs, U"KlattGrid: Formula (" #namef "ormant " #forbs ")", U"Formant: Formula (" #forbs ")...") \
	LABEL (U"", U"row is formant number, col is point number:\nfor row from 1 to nrow do for col from 1 to ncol do " #ForBs " (row, col) :=") \
	LABEL (U"", label) \
	TEXTFIELD (U"formula", textfield) \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_formula_##forbs (me, formantType, GET_STRING (U"formula"), interpreter); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_ADD_FBA_VALUE(Name,namef,Form,FBA,fba,formantType,default,unit,require,requiremessage) \
FORM (KlattGrid_add##Name##Formant##FBA##Point, U"KlattGrid: Add " #namef "ormant " #fba " point", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	REAL (U"Value " #unit, default) \
	OK \
DO \
	double value = GET_REAL (U"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Form##Point (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), value); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FBA_VALUE(Name,namef,Form,FBA,fba,formantType) \
FORM (KlattGrid_remove##Name##Formant##FBA##Points, U"KlattGrid: Remove " #namef "ormant " #fba " points", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"From time (s)", U"0.3")\
	REAL (U"To time (s)", U"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Form##Points (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time")); \
		praat_dataChanged (me);\
	} \
END

#define KlattGrid_ADD_FORMANT(Name,namef,formantType) \
FORM (KlattGrid_add##Name##Formant, U"KlattGrid: Add " #namef "ormant", nullptr) \
	INTEGER (U"Position", U"0 (=at end)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_addFormant (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FORMANT(Name,namef,formantType) \
FORM (KlattGrid_remove##Name##Formant, U"KlattGrid: Remove " #namef "ormant", nullptr) \
	INTEGER (U"Position", U"0 (=do nothing)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeFormant (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
FORM (KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers, U"KlattGrid: Add " #namef "ormant", nullptr) \
	INTEGER (U"Position", U"0 (=at end)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_addFormantFrequencyAndBandwidthTiers (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
FORM (KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers, U"KlattGrid: Remove " #namef "ormant", nullptr) \
	INTEGER (U"Position", U"0 (=do nothing)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeFormantFrequencyAndBandwidthTiers (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_ADD_FORMANT_AMPLITUDETIER(Name,namef,formantType) \
FORM (KlattGrid_add##Name##FormantAmplitudeTier, U"KlattGrid: Add " #namef "ormant amplitude tier", nullptr) \
	INTEGER (U"Position", U"0 (=at end)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_addFormantAmplitudeTier (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FORMANT_AMPLITUDETIER(Name,namef,formantType) \
FORM (KlattGrid_remove##Name##FormantAmplitudeTier, U"KlattGrid: Remove " #namef "ormant amplitude tier", nullptr) \
	INTEGER (U"Position", U"0 (=do nothing)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeFormant (me, formantType, GET_INTEGER (U"Position")); \
		praat_dataChanged (me); \
	} \
END


#define KlattGrid_FORMULA_ADD_REMOVE_FBA(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi", formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency", formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType, U"500.0", (Hz), (value>0), U"Frequency must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType, U"50.0", (Hz), (value>0), U"Bandwidth must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType, U"0.0", (dB), (NUMdefined(value)), U"Amplitude must be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT_AMPLITUDETIER(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT(Name,namef,formantType) \
KlattGrid_ADD_FORMANT_AMPLITUDETIER(Name,namef,formantType)


#define KlattGrid_FORMULA_ADD_REMOVE_FB(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi",formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency",formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, U"500.0", (Hz), (value>0), U"Frequency must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  U"50.0", (Hz), (value>0), U"Bandwidth must be greater than zero.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT(Name,namef,formantType)

#define KlattGrid_FORMULA_ADD_REMOVE_FB_DELTA(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, U"if row = 2 then self + 200 else self fi",formantType, U" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, U"self / 10 ; 10% of frequency",formantType, U"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, U"-100.0", (Hz), (value!=NUMundefined), U"Frequency must be defined.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  U"-50.0", (Hz), (value!=NUMundefined), U"Bandwidth must be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_ADD_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT_FREQUENCYANDBANDWIDTHTIERS(Name,namef,formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT(Name,namef,formantType)

KlattGrid_FORMULA_ADD_REMOVE_FBA (Oral, oral f, KlattGrid_ORAL_FORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Nasal, nasal f, KlattGrid_NASAL_FORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FB (NasalAnti, nasal antif, KlattGrid_NASAL_ANTIFORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FB_DELTA (Delta, delta f, KlattGrid_DELTA_FORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Tracheal, tracheal f, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FB (TrachealAnti, tracheal antif, KlattGrid_TRACHEAL_ANTIFORMANTS)
KlattGrid_FORMULA_ADD_REMOVE_FBA (Frication, frication f, KlattGrid_FRICATION_FORMANTS)

#undef KlattGrid_FORMULA_ADD_REMOVE_FB
#undef KlattGrid_FORMULA_ADD_REMOVE
#undef KlattGrid_ADD_FORMANT_AND_BANDWDTH_TIER
#undef KlattGrid_REMOVE_FBA_VALUE
#undef KlattGrid_ADD_FBA_VALUE
#undef KlattGrid_FORMULA_FORMANT_FB_VALUE

DIRECT (KlattGrid_extractPointProcess_glottalClosures)
LOOP {
	iam (KlattGrid);
	praat_new (KlattGrid_extractPointProcess_glottalClosures (me), my name);
}
END

FORM (KlattGrid_formula_frequencies, U"KlattGrid: Formula (frequencies)", U"Formant: Formula (frequencies)...")
	KlattGrid_6formants_addCommonField (dia);
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_formula_frequencies (me, formantType, GET_STRING (U"formula"), interpreter);
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_formula_bandwidths, U"KlattGrid: Formula (bandwidths)", U"Formant: Formula (bandwidths)...")
	KlattGrid_6formants_addCommonField (dia);
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_formula_bandwidths (me, formantType, GET_STRING (U"formula"), interpreter);
		praat_dataChanged (me);
	}
END

#define KlattGrid_FORMANT_GET_FB_VALUE(Name,name,ForB,forb,FormB,formantType) \
FORM (KlattGrid_get##Name##Formant##ForB##AtTime, U"KlattGrid: Get " #name " " #forb " at time", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##FormB##AtTime (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time")), U" (Hz)"); \
	} \
END

#define KlattGrid_FORMANT_GET_A_VALUE(Name,name,formantType) \
FORM (KlattGrid_get##Name##FormantAmplitudeAtTime, U"KlattGrid: Get " #name " formant amplitude at time", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
	Melder_informationReal (KlattGrid_getAmplitudeAtTime (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time")), U" (dB)"); \
	} \
END

#define KlattGrid_FORMANT_GET_FB_VALUES(Name,name,formantType) \
KlattGrid_FORMANT_GET_FB_VALUE(Name,name,Frequency,frequency,Formant,formantType) \
KlattGrid_FORMANT_GET_FB_VALUE(Name,name,Bandwidth,bandwidth,Bandwidth,formantType)

KlattGrid_FORMANT_GET_FB_VALUES (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_FORMANT_GET_A_VALUE (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_FORMANT_GET_A_VALUE (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (NasalAnti, nasal anti, KlattGrid_NASAL_ANTIFORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (Tracheal, tracheal f, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_FORMANT_GET_A_VALUE (Tracheal, tracheal f, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (Delta, delta f, KlattGrid_DELTA_FORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (TrachealAnti, tracheal antif, KlattGrid_TRACHEAL_ANTIFORMANTS)
KlattGrid_FORMANT_GET_FB_VALUES (Frication, frication, KlattGrid_FRICATION_FORMANTS)
KlattGrid_FORMANT_GET_A_VALUE (Frication, frication, KlattGrid_FRICATION_FORMANTS)

#undef KlattGrid_FORMANT_GET_FB_VALUES
#undef KlattGrid_FORMANT_GET_A_VALUE

#define KlattGrid_EXTRACT_FORMANT_GRID(Name,gridType) \
DIRECT (KlattGrid_extract##Name##FormantGrid) \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extractFormantGrid (me, gridType), formant_names[gridType]); \
	} \
END

#define KlattGrid_EXTRACT_FORMANT_AMPLITUDE(Name,name,formantType) \
FORM (KlattGrid_extract##Name##FormantAmplitudeTier, U"KlattGrid: Extract " #name " formant amplitude tier", nullptr) \
	NATURAL (U"Formant number", U"1") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extractAmplitudeTier (me, formantType, GET_INTEGER (U"Formant number")), formant_names[formantType]); \
	} \
END

KlattGrid_EXTRACT_FORMANT_GRID (Oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (Nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (Frication, KlattGrid_FRICATION_FORMANTS)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Frication, frication, KlattGrid_FRICATION_FORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (Tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_AMPLITUDE (Tracheal, tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (NasalAnti, KlattGrid_NASAL_ANTIFORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (TrachealAnti, KlattGrid_TRACHEAL_ANTIFORMANTS)
KlattGrid_EXTRACT_FORMANT_GRID (Delta, KlattGrid_DELTA_FORMANTS)

#undef KlattGrid_EXTRACT_FORMANTGRID

#define KlattGrid_REPLACE_FORMANT_GRID(Name,formantType) \
DIRECT (KlattGrid_replace##Name##FormantGrid) \
	KlattGrid me = FIRST (KlattGrid); \
	FormantGrid thee = FIRST (FormantGrid); \
	KlattGrid_replaceFormantGrid (me, formantType, thee); \
	praat_dataChanged (me); \
END

#define KlattGrid_REPLACE_FORMANT_AMPLITUDE(Name,name,formantType) \
FORM (KlattGrid_replace##Name##FormantAmplitudeTier, U"KlattGrid: Replace " #name " formant amplitude tier", nullptr) \
	NATURAL (U"Formant number", U"1") \
	OK \
DO \
	KlattGrid me = FIRST (KlattGrid); \
	IntensityTier thee = FIRST (IntensityTier); \
	KlattGrid_replaceAmplitudeTier (me, formantType, GET_INTEGER (U"Formant number"), thee); \
	praat_dataChanged (me); \
END

KlattGrid_REPLACE_FORMANT_GRID (Oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (Nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (NasalAnti, KlattGrid_NASAL_ANTIFORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (Tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Tracheal, tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (TrachealAnti, KlattGrid_TRACHEAL_ANTIFORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (Delta, KlattGrid_DELTA_FORMANTS)
KlattGrid_REPLACE_FORMANT_GRID (Frication, KlattGrid_FRICATION_FORMANTS)
KlattGrid_REPLACE_FORMANT_AMPLITUDE (Frication, frication, KlattGrid_FRICATION_FORMANTS)

#undef KlattGrid_REPLACE_FORMANT_AMPLITUDE
#undef KlattGrid_REPLACE_FORMANTGRID

#define KlattGrid_FORMANT_GET_ADD_REMOVE(Name,name,unit,default,require,requiremessage) \
FORM (KlattGrid_get##Name##AtTime, U"KlattGrid: Get " #name " at time", nullptr) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	OK \
DO \
	int formantType = GET_INTEGER (U"Formant type"); \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##Name##AtTime (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time")), U" (Hz)"); \
	} \
END \
FORM (KlattGrid_getDelta##Name##AtTime, U"KlattGrid: Get delta " #name " at time", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_getDelta##Name##AtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time")), U" (Hz)"); \
	} \
END \
FORM (KlattGrid_add##Name##Point, U"KlattGrid: Add " #name " point", nullptr) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	REAL (U"Value" unit, default) \
	OK \
DO \
	int formantType = GET_INTEGER (U"Formant type"); \
	double value = GET_REAL (U"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Name##Point (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_addDelta##Name##Point, U"KlattGrid: Add delta " #name " point", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"Time (s)", U"0.5") \
	REAL (U"Value" unit, default) \
	OK \
DO \
	double value = GET_REAL (U"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_addDelta##Name##Point (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_remove##Name##Points, U"Remove " #name " points", nullptr) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (U"Formant number", U"1") \
	REAL (U"From time (s)", U"0.3")\
	REAL (U"To time (s)", U"0.7") \
	OK \
DO \
	int formantType = GET_INTEGER (U"Formant type"); \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Name##Points (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time")); \
		praat_dataChanged (me);\
	} \
END \
FORM (KlattGrid_removeDelta##Name##Points, U"Remove delta " #name " points", nullptr) \
	NATURAL (U"Formant number", U"1") \
	REAL (U"From time (s)", U"0.3")\
	REAL (U"To time (s)", U"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeDelta##Name##Points (me, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time")); \
		praat_dataChanged (me);\
	} \
END

KlattGrid_FORMANT_GET_ADD_REMOVE (Formant, formant, U" (Hz)", U"500.0", (value > 0), U"Frequency must be greater than zero.")
KlattGrid_FORMANT_GET_ADD_REMOVE (Bandwidth, bandwidth, U" (Hz)", U"50.0", (value > 0), U"Bandwidth must be greater than zero.")

#undef KlattGrid_FORMANT_GET_ADD_REMOVE

FORM (KlattGrid_addFormantAndBandwidthTier, U"", nullptr)
	KlattGrid_7formants_addCommonField (dia);
	INTEGER (U"Position", U"0 (=at end)")
	OK
DO
	long gridType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_addFormantFrequencyAndBandwidthTiers (me, gridType, GET_INTEGER (U"Position"));
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_extractFormantGrid, U"KlattGrid: Extract formant grid", nullptr)
	KlattGrid_6formants_addCommonField (dia);
	OK
DO
	long gridType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_extractFormantGrid (me, gridType), formant_names[gridType]);
	}
END

FORM (KlattGrid_replaceFormantGrid, U"KlattGrid: Replace formant grid", nullptr)
	KlattGrid_6formants_addCommonField (dia);
	OK
DO
	KlattGrid me = FIRST (KlattGrid);
	FormantGrid thee = FIRST (FormantGrid);
	KlattGrid_replaceFormantGrid (me, GET_INTEGER (U"Formant type"), thee);
	praat_dataChanged (OBJECT);
END

FORM (KlattGrid_getAmplitudeAtTime, U"KlattGrid: Get amplitude at time", nullptr)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		Melder_informationReal (KlattGrid_getAmplitudeAtTime (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time")), U" (dB)");
	}
END

FORM (KlattGrid_addAmplitudePoint, U"KlattGrid: Add amplitude point", nullptr)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	REAL (U"Value (Hz)", U"80.0")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	double value = GET_REAL (U"Value");
	LOOP {
		iam (KlattGrid);
		KlattGrid_addAmplitudePoint (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), value);
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_removeAmplitudePoints, U"Remove amplitude points", nullptr)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (U"Formant number", U"1")
	REAL (U"From time (s)", U"0.3")
	REAL (U"To time (s)", U"0.7")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_removeAmplitudePoints (me, formantType, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_extractAmplitudeTier, U"", nullptr)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (U"Formant number", U"1")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_extractAmplitudeTier (me, formantType, GET_INTEGER (U"Formant number")), formant_names[formantType]);
	}
END

FORM (KlattGrid_replaceAmplitudeTier, U"KlattGrid: Replace amplitude tier", nullptr)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (U"Formant number", U"1")
	OK
DO
	int formantType = GET_INTEGER (U"Formant type");
	KlattGrid me = FIRST (KlattGrid);
	IntensityTier thee = FIRST (IntensityTier);
	KlattGrid_replaceAmplitudeTier (me, formantType, GET_INTEGER (U"Formant number"), thee);
	praat_dataChanged (me);
END

FORM (KlattGrid_to_Sound_special, U"KlattGrid: To Sound (special)", U"KlattGrid: To Sound (special)...")
	KlattGrid_PlayOptions_addCommonFields (dia, true);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_setDefaultPlayOptions (me);
		KlattGrid_PlayOptions_getCommonFields (dia, true, me);
		praat_new (KlattGrid_to_Sound (me), my name);
	}
END

DIRECT (KlattGrid_to_Sound)
	LOOP {
		iam (KlattGrid);
		KlattGrid_setDefaultPlayOptions (me);
		praat_new (KlattGrid_to_Sound (me), my name);
	}
END

FORM (KlattGrid_playSpecial, U"KlattGrid: Play special", U"KlattGrid: Play special...")
	KlattGrid_PlayOptions_addCommonFields (dia, false);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_setDefaultPlayOptions (me);
		KlattGrid_PlayOptions_getCommonFields (dia, false, me);
		KlattGrid_playSpecial (me);
	}
END

FORM (KlattGrid_to_Sound_phonation, U"KlattGrid: To Sound (phonation)", U"KlattGrid: To Sound (phonation)...")
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	KlattGrid_PhonationGridPlayOptions_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_PhonationGridPlayOptions_getCommonFields (dia, me);
		my options -> samplingFrequency = GET_REAL (U"Sampling frequency");
		praat_new (KlattGrid_to_Sound_phonation (me), my name, U"_phonation");
	}
END

DIRECT (KlattGrid_help) Melder_help (U"KlattGrid"); END

DIRECT (KlattGrid_play)
	LOOP {
		iam (KlattGrid);
		KlattGrid_play (me);
	}
END

FORM (KlattGrid_draw, U"KlattGrid: Draw", nullptr)
	RADIO (U"Synthesis model", 1)
		RADIOBUTTON (U"Cascade")
		RADIOBUTTON (U"Parallel")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		KlattGrid_draw (me, GRAPHICS, GET_INTEGER (U"Synthesis model") - 1);
	}
END

FORM (KlattGrid_drawVocalTract, U"KlattGrid: Draw vocal tract", nullptr)
	RADIO (U"Synthesis model", 1)
		RADIOBUTTON (U"Cascade")
		RADIOBUTTON (U"Parallel")
	BOOLEAN (U"Include tracheal formants", true);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		KlattGrid_drawVocalTract (me, GRAPHICS, GET_INTEGER (U"Synthesis model") - 1, GET_INTEGER (U"Include tracheal formants"));
	}
END

DIRECT (KlattGrid_drawPhonation)
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		PhonationGrid_draw (my phonation.get(), GRAPHICS);
	}
END

DIRECT (KlattGrid_drawFrication)
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		FricationGrid_draw (my frication.get(), GRAPHICS);
	}
END

FORM (KlattGrid_to_oralFormantGrid_openPhases, U"KlattGrid: Extract oral formant grid (open phases)", U"KlattGrid: Extract oral formant grid (open phases)...")
	REAL (U"Fade fraction (0..0.5)", U"0.1")
	OK
DO
	double fadeFraction = GET_REAL (U"Fade fraction");
	REQUIRE (fadeFraction < 0.5, U"Fade fraction has to be smaller than 0.5.")
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_to_oralFormantGrid_openPhases (me, fadeFraction), U"corrected");
	}
END

FORM (Sound_KlattGrid_filterByVocalTract, U"Sound & KlattGrid: Filter by vocal tract", U"Sound & KlattGrid: Filter by vocal tract...")
	RADIO (U"Vocal tract filter model", 1)
		RADIOBUTTON (U"Cascade")
		RADIOBUTTON (U"Parallel")
	OK
DO
	Sound me = FIRST (Sound);
	KlattGrid thee = FIRST (KlattGrid);
	int filterModel = GET_INTEGER (U"Vocal tract filter model") - 1;
	praat_new (Sound_KlattGrid_filterByVocalTract (me, thee, filterModel), my name, U"_", thy name);
END

void praat_KlattGrid_init ();
void praat_KlattGrid_init () {

	Thing_recognizeClassesByName (classKlattGrid, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Acoustic synthesis (Klatt)", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"KlattGrid help", nullptr, 1, DO_KlattGrid_help);
	praat_addMenuCommand (U"Objects", U"New", U"-- the synthesizer grid --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattGrid...", nullptr, 1, DO_KlattGrid_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattGrid example", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_createExample);

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
	#define KlattGrid_ORAL_FORMANTS 1
	#define KlattGrid_NASAL_FORMANTS 2
	#define KlattGrid_FRICATION_FORMANTS 3
	#define KlattGrid_TRACHEAL_FORMANTS 4
	#define KlattGrid_NASAL_ANTIFORMANTS 5
	#define KlattGrid_TRACHEAL_ANTIFORMANTS 6
	#define KlattGrid_DELTA_FORMANTS 7
	*/
	praat_addAction1 (classKlattGrid, 0, U"KlattGrid help", nullptr, 0, DO_KlattGrid_help);
	praat_addAction1 (classKlattGrid, 0, U"Edit phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit pitch tier", nullptr, 1, DO_KlattGrid_editPitchTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit voicing amplitude tier", nullptr, 1, DO_KlattGrid_editVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit flutter tier", nullptr, 1, DO_KlattGrid_editFlutterTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit power1 tier", nullptr, 1, DO_KlattGrid_editPower1Tier);
	praat_addAction1 (classKlattGrid, 0, U"Edit power2 tier", nullptr, 1, DO_KlattGrid_editPower2Tier);
	praat_addAction1 (classKlattGrid, 0, U"Edit open phase tier", nullptr, 1, DO_KlattGrid_editOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit collision phase tier", nullptr, 1, DO_KlattGrid_editCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit double pulsing tier", nullptr, 1, DO_KlattGrid_editDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit spectral tilt tier", nullptr, 1, DO_KlattGrid_editSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit aspiration amplitude tier", nullptr, 1, DO_KlattGrid_editAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit breathiness amplitude tier", nullptr, 1, DO_KlattGrid_editBreathinessAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Edit filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit oral formant grid", nullptr, 1, DO_KlattGrid_editOralFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal formant grid", nullptr, 1, DO_KlattGrid_editNasalFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal antiformant grid", nullptr, 1, DO_KlattGrid_editNasalAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit oral formant amplitude tier...", nullptr, 1, DO_KlattGrid_editOralFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit nasal formant amplitude tier...", nullptr, 1, DO_KlattGrid_editNasalFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- edit delta formant grid --", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Edit delta formant grid", nullptr, 1, DO_KlattGrid_editDeltaFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal formant grid", nullptr, 1, DO_KlattGrid_editTrachealFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal antiformant grid", nullptr, 1, DO_KlattGrid_editTrachealAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit tracheal formant amplitude tier...", nullptr, 1, DO_KlattGrid_editTrachealFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- edit frication tiers --", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Edit frication amplitude tier", nullptr, 1, DO_KlattGrid_editFricationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication formant grid", nullptr, 1, DO_KlattGrid_editFricationFormantGrid);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication formant amplitude tier...", nullptr, 1, DO_KlattGrid_editFricationFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Edit frication bypass tier", nullptr, 1, DO_KlattGrid_editFricationBypassTier);
	praat_addAction1 (classKlattGrid, 1, U"Edit frication amplitude tier", nullptr, 1, DO_KlattGrid_editFricationAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Play", nullptr, 0, DO_KlattGrid_play);
	praat_addAction1 (classKlattGrid, 0, U"Play special...", nullptr, 0, DO_KlattGrid_playSpecial);
	praat_addAction1 (classKlattGrid, 0, U"To Sound", nullptr, 0, DO_KlattGrid_to_Sound);
	praat_addAction1 (classKlattGrid, 0, U"To Sound (special)...", nullptr, 0, DO_KlattGrid_to_Sound_special);
	praat_addAction1 (classKlattGrid, 0, U"To Sound (phonation)...", nullptr, 0, DO_KlattGrid_to_Sound_phonation);

	praat_addAction1 (classKlattGrid, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Draw synthesizer...", nullptr, 1, DO_KlattGrid_draw);
	praat_addAction1 (classKlattGrid, 0, U"Draw vocal tract...", nullptr, 1, DO_KlattGrid_drawVocalTract);
	praat_addAction1 (classKlattGrid, 0, U"Draw phonation", nullptr, 1, DO_KlattGrid_drawPhonation);
	praat_addAction1 (classKlattGrid, 0, U"Draw frication", nullptr, 1, DO_KlattGrid_drawFrication);

	praat_addAction1 (classKlattGrid, 0, U"Query phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Get pitch at time...", nullptr, 1, DO_KlattGrid_getPitchAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get voicing amplitude at time...", nullptr, 1, DO_KlattGrid_getVoicingAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get flutter at time...", nullptr, 1, DO_KlattGrid_getFlutterAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get power1 at time...", nullptr, 1, DO_KlattGrid_getPower1AtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get power2 at time...", nullptr, 1, DO_KlattGrid_getPower2AtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get open phase at time...", nullptr, 1, DO_KlattGrid_getOpenPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get collision phase at time...", nullptr, 1, DO_KlattGrid_getCollisionPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get double pulsing at time...", nullptr, 1, DO_KlattGrid_getDoublePulsingAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get spectral tilt at time...", nullptr, 1, DO_KlattGrid_getSpectralTiltAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get aspiration amplitude at time...", nullptr, 1, DO_KlattGrid_getAspirationAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get breathiness amplitude at time...", nullptr, 1, DO_KlattGrid_getBreathinessAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, U"Query filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 1, U"Get formant at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get bandwidth at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getBandwidthAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get amplitude at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get delta formant at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getDeltaFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get delta bandwidth at time...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getDeltaBandwidthAtTime);

#define KlattGRID_GET_FORMANT_FB_VALUES_ACTION(Name, formantname) \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " frequency at time...", nullptr, 1, DO_KlattGrid_get##Name##FormantFrequencyAtTime); \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " bandwidth at time...", nullptr, 1, DO_KlattGrid_get##Name##FormantBandwidthAtTime);

#define KlattGRID_GET_FORMANT_A_VALUES_ACTION(Name,formantname) \
	praat_addAction1 (classKlattGrid, 1, U"Get " #formantname " amplitude at time...", nullptr, 1, DO_KlattGrid_get##Name##FormantAmplitudeAtTime); \

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

	praat_addAction1 (classKlattGrid, 1, U"Get frication bypass at time...", nullptr, 1, DO_KlattGrid_getFricationBypassAtTime);
	praat_addAction1 (classKlattGrid, 1, U"Get frication amplitude at time...", nullptr, 1, DO_KlattGrid_getFricationAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, U"Modify phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Add pitch point...", nullptr, 1, DO_KlattGrid_addPitchPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add voicing amplitude point...", nullptr, 1, DO_KlattGrid_addVoicingAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add flutter point...", nullptr, 1, DO_KlattGrid_addFlutterPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add power1 point...", nullptr, 1, DO_KlattGrid_addPower1Point);
	praat_addAction1 (classKlattGrid, 0, U"Add power2 point...", nullptr, 1, DO_KlattGrid_addPower2Point);
	praat_addAction1 (classKlattGrid, 0, U"Add open phase point...", nullptr, 1, DO_KlattGrid_addOpenPhasePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add collision phase point...", nullptr, 1, DO_KlattGrid_addCollisionPhasePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add double pulsing point...", nullptr, 1, DO_KlattGrid_addDoublePulsingPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add spectral tilt point...", nullptr, 1, DO_KlattGrid_addSpectralTiltPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add aspiration amplitude point...", nullptr, 1, DO_KlattGrid_addAspirationAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Add breathiness amplitude point...", nullptr, 1, DO_KlattGrid_addBreathinessAmplitudePoint);

#define KlattGrid_REMOVE_POINTS_ACTION(Name,name) \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #name " points between...", nullptr, praat_DEPTH_1+praat_HIDDEN, DO_KlattGrid_remove##Name##Points); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #name " points...", nullptr, 1, DO_KlattGrid_remove##Name##Points);

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

#define KlattGrid_MODIFY_ACTIONS_FBA(Name,formantname) \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " frequencies)...", nullptr, 1, DO_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " bandwidths)...", nullptr, 1, DO_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency point...", nullptr, 1, DO_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " bandwidth point...", nullptr, 1, DO_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " amplitude point...", nullptr, 1, DO_KlattGrid_add##Name##FormantAmplitudePoint); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency points...", nullptr, 1, DO_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " bandwidth points...", nullptr, 1, DO_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " amplitude points...", nullptr, 1, DO_KlattGrid_remove##Name##FormantAmplitudePoints); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency and bandwidth tiers...", nullptr, 1, DO_KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency and bandwidth tiers...", nullptr, 1, DO_KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " amplitude tier...", nullptr, 1, DO_KlattGrid_add##Name##FormantAmplitudeTier); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " amplitude tier...", nullptr, 1, DO_KlattGrid_remove##Name##FormantAmplitudeTier); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_remove##Name##Formant);

#define KlattGrid_MODIFY_ACTIONS_FB(Name,formantname) \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " frequencies)...", nullptr, 1, DO_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, U"Formula (" #formantname " bandwidths)...", nullptr, 1, DO_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency point...", nullptr, 1, DO_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " bandwidth point...", nullptr, 1, DO_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency points...", nullptr, 1, DO_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " bandwidth points...", nullptr, 1, DO_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname " frequency and bandwidth tiers...", nullptr, 1, DO_KlattGrid_add##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname " frequency and bandwidth tiers...", nullptr, 1, DO_KlattGrid_remove##Name##FormantFrequencyAndBandwidthTiers); \
	praat_addAction1 (classKlattGrid, 0, U"Add " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, U"Remove " #formantname "...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_remove##Name##Formant);

	KlattGrid_MODIFY_ACTIONS_FBA (Oral, oral formant)
	praat_addAction1 (classKlattGrid, 0, U"-- oral modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Nasal, nasal formant)
	praat_addAction1 (classKlattGrid, 0, U"-- nasal modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (NasalAnti, nasal antiformant)

	praat_addAction1 (classKlattGrid, 0, U"Formula (frequencies)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_formula_frequencies);
	praat_addAction1 (classKlattGrid, 0, U"Formula (bandwidths)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_formula_bandwidths);
	praat_addAction1 (classKlattGrid, 0, U"Add formant point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addFormantPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add bandwidth point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add amplitude point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, U"Remove formant points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeFormantPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove bandwidth points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeBandwidthPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove amplitude points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeAmplitudePoints);
	praat_addAction1 (classKlattGrid, 0, U"Modify coupling - ", nullptr, 0, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (Delta, delta formant)
	praat_addAction1 (classKlattGrid, 0, U"-- delta modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Tracheal, tracheal formant)
	praat_addAction1 (classKlattGrid, 0, U"-- nasal modify separator --", nullptr, 1, nullptr);
	KlattGrid_MODIFY_ACTIONS_FB (TrachealAnti, tracheal antiformant)

	praat_addAction1 (classKlattGrid, 0, U"Add delta formant point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addDeltaFormantPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add delta bandwidth point...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addDeltaBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, U"Remove delta formant points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeDeltaFormantPoints);
	praat_addAction1 (classKlattGrid, 0, U"Remove delta bandwidth points between...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeDeltaBandwidthPoints);

	praat_addAction1 (classKlattGrid, 0, U"Modify frication -", nullptr, 0, nullptr);
	KlattGrid_MODIFY_ACTIONS_FBA (Frication, frication formant)
	praat_addAction1 (classKlattGrid, 0, U"-- frication modify separator --", nullptr, 1, nullptr);

	praat_addAction1 (classKlattGrid, 0, U"Add frication bypass point...", nullptr, 1, DO_KlattGrid_addFricationBypassPoint);
	praat_addAction1 (classKlattGrid, 0, U"Add frication amplitude point...", nullptr, 1, DO_KlattGrid_addFricationAmplitudePoint);
	KlattGrid_REMOVE_POINTS_ACTION (FricationBypass, frication bypass)
	KlattGrid_REMOVE_POINTS_ACTION (FricationAmplitude, frication amplitude)
	praat_addAction1 (classKlattGrid, 0, U"Add formant and bandwidth tier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addFormantAndBandwidthTier);

#undef KlattGrid_REMOVE_POINTS_ACTION
#undef KlattGrid_MODIFY_ACTION_FB
#undef KlattGrid_MODIFY_ACTION_FBA

	praat_addAction1 (classKlattGrid, 0, U"Extract phonation -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract pitch tier", nullptr, 1, DO_KlattGrid_extractPitchTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract voicing amplitude tier", nullptr, 1, DO_KlattGrid_extractVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract flutter tier", nullptr, 1, DO_KlattGrid_extractFlutterTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract power1 tier", nullptr, 1, DO_KlattGrid_extractPower1Tier);
	praat_addAction1 (classKlattGrid, 0, U"Extract power2 tier", nullptr, 1, DO_KlattGrid_extractPower2Tier);
	praat_addAction1 (classKlattGrid, 0, U"Extract open phase tier", nullptr, 1, DO_KlattGrid_extractOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract collision phase tier", nullptr, 1, DO_KlattGrid_extractCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract double pulsing tier", nullptr, 1, DO_KlattGrid_extractDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract spectral tilt tier", nullptr, 1, DO_KlattGrid_extractSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract aspiration amplitude tier", nullptr, 1, DO_KlattGrid_extractAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract breathiness amplitude tier", nullptr, 1, DO_KlattGrid_extractBreathinessAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, U"-- extract glottal events--", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract PointProcess (glottal closures)", nullptr, 1, DO_KlattGrid_extractPointProcess_glottalClosures);

#define KlattGRID_EXTRACT_FORMANT_GRID_ACTION(Name,namef) \
	praat_addAction1 (classKlattGrid, 0, U"Extract " #namef "ormant grid", nullptr, 1, DO_KlattGrid_extract##Name##FormantGrid);
#define KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION(Name,name) \
	praat_addAction1 (classKlattGrid, 0, U"Extract " #name " formant amplitude tier...", nullptr, 1, DO_KlattGrid_extract##Name##FormantAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, U"Extract filters -", nullptr, 0, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract formant grid...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_extractFormantGrid); // deprecated
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Oral, oral f)
	praat_addAction1 (classKlattGrid, 0, U"Extract amplitude tier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_extractAmplitudeTier); // deprecated
	praat_addAction1 (classKlattGrid, 0, U"Extract formant grid (open phases)...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_to_oralFormantGrid_openPhases);
	praat_addAction1 (classKlattGrid, 0, U"Extract oral formant grid (open phases)...", nullptr, 1, DO_KlattGrid_to_oralFormantGrid_openPhases);
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Oral, oral)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Nasal, nasal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Nasal, nasal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (NasalAnti, nasal antif)

	praat_addAction1 (classKlattGrid, 0, U"-- extract delta characteristics", nullptr, 1, nullptr);
	praat_addAction1 (classKlattGrid, 0, U"Extract delta formant grid", nullptr, 1, DO_KlattGrid_extractDeltaFormantGrid);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Tracheal, tracheal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (TrachealAnti, tracheal antif)
	praat_addAction1 (classKlattGrid, 0, U"-- extract frication characteristics", nullptr, 1, nullptr);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Frication, frication f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Frication, frication)
	praat_addAction1 (classKlattGrid, 0, U"Extract frication bypass tier", nullptr, 1, DO_KlattGrid_extractFricationBypassTier);
	praat_addAction1 (classKlattGrid, 0, U"Extract frication amplitude tier", nullptr, 1, DO_KlattGrid_extractFricationAmplitudeTier);

#undef KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION
#undef KlattGRID_EXTRACT_FORMANT_GRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classPitchTier, 1, U"Replace pitch tier", nullptr, 1, DO_KlattGrid_replacePitchTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace flutter tier", nullptr, 1, DO_KlattGrid_replaceFlutterTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace power1 tier", nullptr, 1, DO_KlattGrid_replacePower1Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace power2 tier", nullptr, 1, DO_KlattGrid_replacePower2Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace open phase tier", nullptr, 1, DO_KlattGrid_replaceOpenPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace collision phase tier", nullptr, 1, DO_KlattGrid_replaceCollisionPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, U"Replace double pulsing tier", nullptr, 1, DO_KlattGrid_replaceDoublePulsingTier);

	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"-- replace formant amplitudes --", nullptr, 1, nullptr);

#define KlattGrid_REPLACE_FORMANTGRID_ACTION(Name,namef) \
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"Replace " #namef "ormant grid", nullptr, 1, DO_KlattGrid_replace##Name##FormantGrid);
#define KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION(Name,namef) \
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace " #namef "ormant amplitude tier...", nullptr, 1, DO_KlattGrid_replace##Name##FormantAmplitudeTier);

	KlattGrid_REPLACE_FORMANTGRID_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (NasalAnti, nasal antif)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"-- replace coupling --", nullptr, 1, nullptr);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (TrachealAnti, tracheal antif)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Delta, delta f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"-- replace frication --", nullptr, 1, nullptr);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, U"Replace formant grid...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_replaceFormantGrid);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace voicing amplitude tier", nullptr, 1, DO_KlattGrid_replaceVoicingAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace spectral tilt tier", nullptr, 1, DO_KlattGrid_replaceSpectralTiltTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace aspiration amplitude tier", nullptr, 1,
	                  DO_KlattGrid_replaceAspirationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace breathiness amplitude tier", nullptr, 1,
	                  DO_KlattGrid_replaceBreathinessAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace amplitude tier...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_replaceAmplitudeTier);
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace frication amplitude tier", nullptr, 1, DO_KlattGrid_replaceFricationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, U"Replace frication bypass tier", nullptr, 1, DO_KlattGrid_replaceFricationBypassTier);

#undef KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION
#undef KlattGrid_REPLACE_FORMANTGRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classSound, 1, U"Filter by vocal tract...", nullptr, 1, DO_Sound_KlattGrid_filterByVocalTract);

	INCLUDE_MANPAGES (manual_KlattGrid)
}

/* End of file praat_KlattGrid_init.cpp */
