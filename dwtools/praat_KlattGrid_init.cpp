/* praat_KlattGrid_init.cpp
 *
 * Copyright (C) 2009-2011 David Weenink
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

static const wchar_t *formant_names[] = { L"", L"oral ", L"nasal ", L"frication ", L"tracheal ", L"nasal anti", L"tracheal anti", L"delta "};

static void KlattGrid_4formants_addCommonField (void *dia) {
	Any radio;
	OPTIONMENU (L"Formant type", 1)
	OPTION (L"Normal formant")
	OPTION (L"Nasal formant")
	OPTION (L"Frication formant")
	OPTION (L"Tracheal formant")
}

static void KlattGrid_6formants_addCommonField (void *dia) {
	Any radio;
	OPTIONMENU (L"Formant type", 1)
	OPTION (L"Normal formant")
	OPTION (L"Nasal formant")
	OPTION (L"Frication formant")
	OPTION (L"Tracheal formant")
	OPTION (L"Nasal antiformant")
	OPTION (L"Tracheal antiformant")
	//	OPTION (L"Delta formant")
}

static void KlattGrid_7formants_addCommonField (void *dia) {
	Any radio;
	OPTIONMENU (L"Formant type", 1)
	OPTION (L"Normal formant")
	OPTION (L"Nasal formant")
	OPTION (L"Frication formant")
	OPTION (L"Tracheal formant")
	OPTION (L"Nasal antiformant")
	OPTION (L"Tracheal antiformant")
	OPTION (L"Delta formant")
}

static void KlattGrid_PhonationGridPlayOptions_addCommonFields (void *dia) {
	Any radio;
	//LABEL (L"", L"Phonation options")
	BOOLEAN (L"Voicing", 1)
	BOOLEAN (L"Flutter", 1)
	BOOLEAN (L"Double pulsing", 1)
	BOOLEAN (L"Collision phase", 1)
	BOOLEAN (L"Spectral tilt", 1)
	OPTIONMENU (L"Flow function", 1)
	OPTION (L"Powers in tiers")
	OPTION (L"t^2-t^3")
	OPTION (L"t^3-t^4")
	BOOLEAN (L"Flow derivative", 1)
	BOOLEAN (L"Aspiration", 1)
	BOOLEAN (L"Breathiness", 1)
}

static void KlattGrid_PhonationGridPlayOptions_getCommonFields (void *dia, KlattGrid thee) {
	PhonationGridPlayOptions pp = thy phonation -> options;
	pp -> voicing = GET_INTEGER (L"Voicing");
	pp -> flutter = GET_INTEGER (L"Flutter");
	pp -> doublePulsing = GET_INTEGER (L"Double pulsing");
	pp -> collisionPhase = GET_INTEGER (L"Collision phase");
	pp -> spectralTilt = GET_INTEGER (L"Spectral tilt");
	pp -> flowFunction = GET_INTEGER (L"Flow function");
	pp -> flowDerivative = GET_INTEGER (L"Flow derivative");
	pp -> aspiration = GET_INTEGER (L"Aspiration");
	pp -> breathiness = GET_INTEGER (L"Breathiness");
}

static void KlattGrid_PlayOptions_addCommonFields (void *dia, int sound) {
	Any radio;
	//LABEL (L"", L"Time domain")
	REAL (L"left Time range (s)", L"0")
	REAL (L"right Time range (s)", L"0")
	if (sound) POSITIVE (L"Sampling frequency (Hz)", L"44100")
		BOOLEAN (L"Scale peak", 1)
		KlattGrid_PhonationGridPlayOptions_addCommonFields (dia);
	OPTIONMENU (L"Filter options", 1)
	OPTION (L"Cascade")
	OPTION (L"Parallel")
	REAL (L"left Oral formant range", L"1")
	REAL (L"right Oral formant range", L"5")
	REAL (L"left Nasal formant range", L"1")
	REAL (L"right Nasal formant range", L"1")
	REAL (L"left Nasal antiformant range", L"1")
	REAL (L"right Nasal antiformant range", L"1")
	//LABEL (L"", L"Coupling options")
	REAL (L"left Tracheal formant range", L"1")
	REAL (L"right Tracheal formant range", L"1")
	REAL (L"left Tracheal antiformant range", L"1")
	REAL (L"right Tracheal antiformant range", L"1")
	REAL (L"left Delta formant range", L"1")
	REAL (L"right Delta formant range", L"1")
	REAL (L"left Delta bandwidth range", L"1")
	REAL (L"right Delta bandwidth range", L"1")
	//LABEL (L"", L"Frication options")
	REAL (L"left Frication formant range", L"1")
	REAL (L"right Frication formant range", L"6")
	BOOLEAN (L"Frication bypass", 1)
}

static void KlattGrid_PlayOptions_getCommonFields (void *dia, int sound, KlattGrid thee) {
	KlattGrid_setDefaultPlayOptions (thee);
	KlattGridPlayOptions pk = thy options;
	pk -> scalePeak = GET_INTEGER (L"Scale peak");
	pk -> xmin = GET_REAL (L"left Time range");
	pk -> xmax = GET_REAL (L"right Time range");
	if (sound) {
		pk -> samplingFrequency = GET_REAL (L"Sampling frequency");
	}
	pk -> scalePeak = GET_INTEGER (L"Scale peak");
	KlattGrid_PhonationGridPlayOptions_getCommonFields (dia, thee);
	VocalTractGridPlayOptions pv = thy vocalTract -> options;
	pv -> filterModel = GET_INTEGER (L"Filter options") == 1 ? KlattGrid_FILTER_CASCADE : KlattGrid_FILTER_PARALLEL;
	pv -> startOralFormant = GET_REAL (L"left Oral formant range");
	pv -> endOralFormant  = GET_REAL (L"right Oral formant range");
	pv -> startNasalFormant = GET_REAL (L"left Nasal formant range");
	pv -> endNasalFormant = GET_REAL (L"right Nasal formant range");
	pv -> startNasalAntiFormant = GET_REAL (L"left Nasal antiformant range");
	pv -> endNasalAntiFormant = GET_REAL (L"right Nasal antiformant range");
	CouplingGridPlayOptions pc = thy coupling -> options;
	pc -> startTrachealFormant = GET_REAL (L"left Tracheal formant range");
	pc -> endTrachealFormant = GET_REAL (L"right Tracheal formant range");
	pc -> startTrachealAntiFormant = GET_REAL (L"left Tracheal antiformant range");
	pc -> endTrachealAntiFormant = GET_REAL (L"right Tracheal antiformant range");
	pc -> startDeltaFormant = GET_REAL (L"left Delta formant range");
	pc -> endDeltaFormant = GET_REAL (L"right Delta formant range");
	pc -> startDeltaBandwidth = GET_REAL (L"left Delta bandwidth range");
	pc -> endDeltaFormant = GET_REAL (L"right Delta bandwidth range");
	FricationGridPlayOptions pf = thy frication -> options;
	pf -> startFricationFormant = GET_REAL (L"left Frication formant range");
	pf -> endFricationFormant = GET_REAL (L"right Frication formant range");
	pf -> bypass = GET_INTEGER (L"Frication bypass");
}

DIRECT (KlattGrid_createExample)
praat_new (KlattGrid_createExample(), L"example");
END

FORM (KlattGrid_create, L"Create KlattGrid", L"Create KlattGrid...")
	WORD (L"Name", L"kg")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	INTEGER (L"Number of oral formants", L"6")
	INTEGER (L"Number of nasal formants", L"1")
	INTEGER (L"Number of nasal antiformants", L"1")
	INTEGER (L"Number of frication formants", L"6")
	LABEL (L"", L"Coupling between source and filter")
	INTEGER (L"Number of tracheal formants", L"1")
	INTEGER (L"Number of tracheal antiformants", L"1")
	INTEGER (L"Number of delta formants", L"1")
	OK
DO
	double tmin = GET_REAL (L"Start time");
	double tmax = GET_REAL (L"End time");
	REQUIRE (tmin < tmax, L"The start time must lie before the end time.")
	long numberOfOralFormants = GET_INTEGER (L"Number of oral formants");
	long numberOfNasalFormants = GET_INTEGER (L"Number of nasal formants");
	long numberOfNasalAntiFormants = GET_INTEGER (L"Number of nasal antiformants");
	long numberOfTrachealFormants = GET_INTEGER (L"Number of tracheal formants");
	long numberOfTrachealAntiFormants = GET_INTEGER (L"Number of tracheal antiformants");
	long numberOfFricationFormants = GET_INTEGER (L"Number of frication formants");
	long numberOfDeltaFormants = GET_INTEGER (L"Number of delta formants");
	REQUIRE (numberOfOralFormants >= 0 && numberOfNasalFormants >= 0 && numberOfNasalAntiFormants >= 0
		&& numberOfTrachealFormants >= 0 && numberOfTrachealAntiFormants >= 0
		&& numberOfFricationFormants >= 0 && numberOfDeltaFormants >= 0,
		L"The number of (..) formants cannot be negative!")
	praat_new (KlattGrid_create (tmin, tmax, numberOfOralFormants,
		numberOfNasalFormants, numberOfNasalAntiFormants,
		numberOfTrachealFormants, numberOfTrachealAntiFormants,
		numberOfFricationFormants, numberOfDeltaFormants),
		GET_STRING (L"Name"));
END


#define KlattGrid_INSTALL_TIER_EDITOR(Name,name) \
DIRECT (KlattGrid_edit##Name##Tier) \
	if (theCurrentPraatApplication -> batch) { Melder_throw ("Cannot edit a KlattGrid from batch."); } \
	LOOP {\
		iam (KlattGrid); \
		auto##KlattGrid_##name##TierEditor editor = KlattGrid_##name##TierEditor_create (ID_AND_FULL_NAME, me); \
		praat_installEditor (editor.transfer(), IOBJECT); \
	}\
END

KlattGrid_INSTALL_TIER_EDITOR (Pitch, pitch)
KlattGrid_INSTALL_TIER_EDITOR (VoicingAmplitude, voicingAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (Flutter, flutter)
KlattGrid_INSTALL_TIER_EDITOR (Power1, power1)
KlattGrid_INSTALL_TIER_EDITOR (Power2, power2)
KlattGrid_INSTALL_TIER_EDITOR (OpenPhase, openPhase)
KlattGrid_INSTALL_TIER_EDITOR (CollisionPhase, collisionPhase)
KlattGrid_INSTALL_TIER_EDITOR (DoublePulsing, doublePulsing)
KlattGrid_INSTALL_TIER_EDITOR (AspirationAmplitude, aspirationAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (BreathinessAmplitude, breathinessAmplitude)
KlattGrid_INSTALL_TIER_EDITOR (SpectralTilt, spectralTilt)

KlattGrid_INSTALL_TIER_EDITOR (FricationBypass, fricationBypass)
KlattGrid_INSTALL_TIER_EDITOR (FricationAmplitude, fricationAmplitude)

#undef KlattGrid_INSTALL_TIER_EDITOR

#define KlattGRID_EDIT_FORMANTGRID(Name,formantType) \
DIRECT (KlattGrid_edit##Name##FormantGrid) \
	if (theCurrentPraatApplication -> batch) { Melder_throw ("Cannot edit a KlattGrid from batch."); } \
	LOOP {\
		iam (KlattGrid); \
		const wchar_t *id_and_name = Melder_wcscat (Melder_integer (ID), L". ", formant_names[formantType], L"formant grid"); \
		autoKlattGrid_formantGridEditor editor = KlattGrid_formantGridEditor_create (id_and_name, me, formantType); \
		praat_installEditor (editor.transfer(), IOBJECT); \
		Melder_free (id_and_name); \
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
FORM (KlattGrid_edit##Name##FormantAmplitudeTier, L"KlattGrid: View & Edit " #name "formant amplitude tier", 0) \
	NATURAL (L"Formant number", L"1") \
	OK \
DO \
	long formantNumber = GET_INTEGER (L"Formant number"); \
	if (theCurrentPraatApplication -> batch) { Melder_throw ("Cannot edit a KlattGrid from batch."); } \
	LOOP { \
		iam (KlattGrid); \
		Ordered *amp = KlattGrid_getAddressOfAmplitudes (me, formantType); \
		if (amp == NULL) Melder_throw ("Unknown formant type"); \
		if (formantNumber > (*amp) -> size) Melder_throw ("Formant number does not exist."); \
		const wchar_t *id_and_name = Melder_wcscat (Melder_integer (ID), L". ", formant_names[formantType], L"formant amplitude tier"); \
		autoKlattGrid_decibelTierEditor editor = KlattGrid_decibelTierEditor_create (id_and_name, me, (RealTier) (*amp)->item[formantNumber]); \
		praat_installEditor (editor.transfer(), IOBJECT); \
		Melder_free (id_and_name); \
	} \
END

KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Oral, oral, KlattGrid_ORAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Nasal, nasal, KlattGrid_NASAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Tracheal, tracheal, KlattGrid_TRACHEAL_FORMANTS)
KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER (Frication, frication, KlattGrid_FRICATION_FORMANTS)

#undef KlattGrid_EDIT_FORMANT_AMPLITUDE_TIER

#define KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE(Name,name,unit,default,require, requiremessage,newname,tiertype) \
FORM(KlattGrid_get##Name##AtTime, L"KlattGrid: Get " #name " at time", 0) \
	REAL (L"Time", L"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
	Melder_informationReal (KlattGrid_get##Name##AtTime (me, GET_REAL (L"Time")), unit); \
	}\
END \
FORM (KlattGrid_add##Name##Point, L"KlattGrid: Add " #name " point", 0) \
	REAL (L"Time (s)", L"0.5") \
	REAL (L"Value" unit, default) \
	OK \
DO \
	double value = GET_REAL (L"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Name##Point (me, GET_REAL (L"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_remove##Name##Points, L"Remove " #name " points", 0) \
	REAL (L"From time (s)", L"0.3")\
	REAL (L"To time (s)", L"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Name##Points (me, GET_REAL (L"From time"), GET_REAL (L"To time")); \
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
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Pitch, pitch, L" (Hz)", (L"100.0"),
        (value >= 0), (L"Pitch must be greater equal zero."), L"f0", PitchTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (VoicingAmplitude, voicing amplitude, L" (dB SPL)", L"90.0",
        (1), L"", L"voicing", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Flutter, flutter, L" (0..1)", (L"0.0"),
        (value >= 0 && value <= 1), (L"Flutter must be in [0,1]."), L"flutter", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power1, power1, L"", L"3",
        (value > 0), L"Power1 needs to be positive.", L"power1", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (Power2, power2, L"", L"4",
        (value > 0), L"Power2 needs to be positive.", L"power2", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (OpenPhase, open phase, L"", L"0.7",
        (value >= 0 && value <= 1), L"Open phase must be greater than zero and smaller equal one.", L"openPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (CollisionPhase, collision phase, L"", L"0.03",
        (value >= 0 && value < 1), L"Collision phase must be greater equal zero and smaller than one.", L"collisionPhase", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (DoublePulsing, double pulsing, L" (0..1)", L"0.0",
        (value >= 0 && value <= 1), L"Double pulsing must be greater equal zero and smaller equal one.", L"doublePulsing", RealTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (SpectralTilt, spectral tilt, L" (dB)", L"0.0",
        (value >= 0), L"Spectral tilt must be greater equal zero.", L"spectralTilt", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (AspirationAmplitude, aspiration amplitude, L" (dB SPL)", L"0.0",
        (1), L"", L"aspiration", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (BreathinessAmplitude, breathiness amplitude, L" (dB SPL)", L"30.0",
        (1), L"", L"breathiness", IntensityTier)

KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationAmplitude, frication amplitude, L" (dB SPL)", L"30.0",
        (1), L"", L"frication", IntensityTier)
KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE (FricationBypass, frication bypass, L" (dB)", L"30.0",
        (1), L"", L"bypass", IntensityTier)

#undef KlattGrid_PHONATION_GET_ADD_REMOVE_EXTRACT_REPLACE

#define KlattGrid_FORMULA_FORMANT_FBA_VALUE(Name,namef,ForBs,forbs,textfield,formantType,label) \
FORM (KlattGrid_formula##Name##Formant##ForBs, L"KlattGrid: Formula (" #namef "ormant " #forbs ")", L"Formant: Formula (" #forbs ")...") \
	LABEL (L"", L"row is formant number, col is point number:\nfor row from 1 to nrow do for col from 1 to ncol do " #ForBs " (row, col) :=") \
	LABEL (L"", label) \
	TEXTFIELD (L"formula", textfield) \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_formula_##forbs (me, formantType, GET_STRING (L"formula"), interpreter); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_ADD_FBA_VALUE(Name,namef,Form,FBA,fba,formantType,default,unit,require,requiremessage) \
FORM (KlattGrid_add##Name##Formant##FBA##Point, L"KlattGrid: Add " #namef "ormant " #fba " point", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	REAL (L"Value " #unit, default) \
	OK \
DO \
	double value = GET_REAL (L"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Form##Point (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), value); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FBA_VALUE(Name,namef,Form,FBA,fba,formantType) \
FORM (KlattGrid_remove##Name##Formant##FBA##Points, L"KlattGrid: Remove " #namef "ormant " #fba " points", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"From time (s)", L"0.3")\
	REAL (L"To time (s)", L"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Form##Points (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time")); \
		praat_dataChanged (me);\
	} \
END

#define KlattGrid_ADD_FORMANT(Name,namef,formantType) \
FORM (KlattGrid_add##Name##Formant, L"KlattGrid: Add " #namef "ormant", 0) \
	INTEGER (L"Position", L"0 (=at end)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_addFormant (me, formantType, GET_INTEGER (L"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_REMOVE_FORMANT(Name,namef,formantType) \
FORM (KlattGrid_remove##Name##Formant, L"KlattGrid: Remove " #namef "ormant", 0) \
	INTEGER (L"Position", L"0 (=do nothing)") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeFormant (me, formantType, GET_INTEGER (L"Position")); \
		praat_dataChanged (me); \
	} \
END

#define KlattGrid_FORMULA_ADD_REMOVE_FBA(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, L"if row = 2 then self + 200 else self fi", formantType, L" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, L"self / 10 ; 10% of frequency", formantType, L"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType, L"500.0", (Hz), (value>0), L"Frequency must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType, L"50.0", (Hz), (value>0), L"Bandwidth must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType, L"0.0", (dB), (NUMdefined(value)), L"Amplitude must be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Amplitude, Amplitude, amplitude, formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT(Name,namef,formantType)

#define KlattGrid_FORMULA_ADD_REMOVE_FB(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, L"if row = 2 then self + 200 else self fi",formantType, L" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, L"self / 10 ; 10% of frequency",formantType,L"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, L"500.0", (Hz), (value>0), L"Frequency must be greater than zero.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  L"50.0", (Hz), (value>0), L"Bandwidth must be greater than zero.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
KlattGrid_ADD_FORMANT(Name,namef,formantType) \
KlattGrid_REMOVE_FORMANT(Name,namef,formantType)

#define KlattGrid_FORMULA_ADD_REMOVE_FB_DELTA(Name,namef,formantType) \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Frequencies, frequencies, L"if row = 2 then self + 200 else self fi",formantType, L" ") \
KlattGrid_FORMULA_FORMANT_FBA_VALUE (Name, namef, Bandwidths, bandwidths, L"self / 10 ; 10% of frequency",formantType,L"Warning: self is formant frequency.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Formant,Frequency, frequency, formantType, L"-100.0", (Hz), (value!=NUMundefined), L"Frequency must be defined.") \
KlattGrid_ADD_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType,  L"-50.0", (Hz), (value!=NUMundefined), L"Bandwidth must be defined.") \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Formant, Frequency, frequency, formantType) \
KlattGrid_REMOVE_FBA_VALUE (Name, namef, Bandwidth, Bandwidth, bandwidth, formantType) \
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

FORM (KlattGrid_formula_frequencies, L"KlattGrid: Formula (frequencies)", L"Formant: Formula (frequencies)...")
	KlattGrid_6formants_addCommonField (dia);
	LABEL (L"", L"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (L"formula", L"if row = 2 then self + 200 else self fi")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_formula_frequencies (me, formantType, GET_STRING (L"formula"), interpreter);
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_formula_bandwidths, L"KlattGrid: Formula (bandwidths)", L"Formant: Formula (bandwidths)...")
	KlattGrid_6formants_addCommonField (dia);
	LABEL (L"", L"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (L"formula", L"if row = 2 then self + 200 else self fi")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_formula_bandwidths (me, formantType, GET_STRING (L"formula"), interpreter);
		praat_dataChanged (me);
	}
END

#define KlattGrid_FORMANT_GET_FB_VALUE(Name,name,ForB,forb,FormB,formantType) \
FORM (KlattGrid_get##Name##Formant##ForB##AtTime, L"KlattGrid: Get " #name " " #forb " at time", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##FormB##AtTime (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time")), L" (Hz)"); \
	} \
END

#define KlattGrid_FORMANT_GET_A_VALUE(Name,name,formantType) \
FORM (KlattGrid_get##Name##FormantAmplitudeAtTime, L"KlattGrid: Get " #name " formant amplitude at time", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
	Melder_informationReal (KlattGrid_getAmplitudeAtTime (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time")), L" (dB)"); \
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
FORM (KlattGrid_extract##Name##FormantAmplitudeTier, L"KlattGrid: Extract " #name " formant amplitude tier", 0) \
	NATURAL (L"Formant number", L"1") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		praat_new (KlattGrid_extractAmplitudeTier (me, formantType, GET_INTEGER (L"Formant number")), formant_names[formantType]); \
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
FORM (KlattGrid_replace##Name##FormantAmplitudeTier, L"KlattGrid: Replace " #name " formant amplitude tier", 0) \
	NATURAL (L"Formant number", L"1") \
	OK \
DO \
	KlattGrid me = FIRST (KlattGrid); \
	IntensityTier thee = FIRST (IntensityTier); \
	KlattGrid_replaceAmplitudeTier (me, formantType, GET_INTEGER (L"Formant number"), thee); \
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
FORM (KlattGrid_get##Name##AtTime, L"KlattGrid: Get " #name " at time", 0) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	OK \
DO \
	int formantType = GET_INTEGER (L"Formant type"); \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_get##Name##AtTime (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time")), L" (Hz)"); \
	} \
END \
FORM (KlattGrid_getDelta##Name##AtTime, L"KlattGrid: Get delta " #name " at time", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		Melder_informationReal (KlattGrid_getDelta##Name##AtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time")), L" (Hz)"); \
	} \
END \
FORM (KlattGrid_add##Name##Point, L"KlattGrid: Add " #name " point", 0) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	REAL (L"Value" unit, default) \
	OK \
DO \
	int formantType = GET_INTEGER (L"Formant type"); \
	double value = GET_REAL (L"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_add##Name##Point (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_addDelta##Name##Point, L"KlattGrid: Add delta " #name " point", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"Time (s)", L"0.5") \
	REAL (L"Value" unit, default) \
	OK \
DO \
	double value = GET_REAL (L"Value"); \
	REQUIRE (require, requiremessage) \
	LOOP { iam (KlattGrid); \
		KlattGrid_addDelta##Name##Point (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), value); \
		praat_dataChanged (me); \
	} \
END \
FORM (KlattGrid_remove##Name##Points, L"Remove " #name " points", 0) \
	KlattGrid_6formants_addCommonField (dia); \
	NATURAL (L"Formant number", L"1") \
	REAL (L"From time (s)", L"0.3")\
	REAL (L"To time (s)", L"0.7") \
	OK \
DO \
	int formantType = GET_INTEGER (L"Formant type"); \
	LOOP { iam (KlattGrid); \
		KlattGrid_remove##Name##Points (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time")); \
		praat_dataChanged (me);\
	} \
END \
FORM (KlattGrid_removeDelta##Name##Points, L"Remove delta " #name " points", 0) \
	NATURAL (L"Formant number", L"1") \
	REAL (L"From time (s)", L"0.3")\
	REAL (L"To time (s)", L"0.7") \
	OK \
DO \
	LOOP { iam (KlattGrid); \
		KlattGrid_removeDelta##Name##Points (me, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time")); \
		praat_dataChanged (me);\
	} \
END

KlattGrid_FORMANT_GET_ADD_REMOVE (Formant, formant, L" (Hz)", L"500.0", (value > 0), L"Frequency must be greater than zero.")
KlattGrid_FORMANT_GET_ADD_REMOVE (Bandwidth, bandwidth, L" (Hz)", L"50.0", (value > 0), L"Bandwidth must be greater than zero.")

#undef KlattGrid_FORMANT_GET_ADD_REMOVE

FORM (KlattGrid_addFormantAndBandwidthTier, L"", 0)
	KlattGrid_7formants_addCommonField (dia);
	INTEGER (L"Position", L"0 (=at end)")
	OK
DO
	long gridType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_addFormantAndBandwidthTier (me, gridType, GET_INTEGER (L"Position"));
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_extractFormantGrid, L"KlattGrid: Extract formant grid", 0)
	KlattGrid_6formants_addCommonField (dia);
	OK
DO
	long gridType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_extractFormantGrid (me, gridType), formant_names[gridType]);
	}
END

FORM (KlattGrid_replaceFormantGrid, L"KlattGrid: Replace formant grid", 0)
	KlattGrid_6formants_addCommonField (dia);
	OK
DO
	KlattGrid me = FIRST (KlattGrid);
	FormantGrid thee = FIRST (FormantGrid);
	KlattGrid_replaceFormantGrid (me, GET_INTEGER (L"Formant type"), thee);
	praat_dataChanged (OBJECT);
END

FORM (KlattGrid_getAmplitudeAtTime, L"KlattGrid: Get amplitude at time", 0) \
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		Melder_informationReal (KlattGrid_getAmplitudeAtTime (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time")), L" (dB)");
	}
END

FORM (KlattGrid_addAmplitudePoint, L"KlattGrid: Add amplitude point", 0)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	REAL (L"Value (Hz)", L"80.0")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	double value = GET_REAL (L"Value");
	LOOP {
		iam (KlattGrid);
		KlattGrid_addAmplitudePoint (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), value);
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_removeAmplitudePoints, L"Remove amplitude points", 0) \
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (L"Formant number", L"1")
	REAL (L"From time (s)", L"0.3")
	REAL (L"To time (s)", L"0.7")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		KlattGrid_removeAmplitudePoints (me, formantType, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time"));
		praat_dataChanged (me);
	}
END

FORM (KlattGrid_extractAmplitudeTier, L"", 0)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (L"Formant number", L"1")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_extractAmplitudeTier (me, formantType, GET_INTEGER (L"Formant number")), formant_names[formantType]);
	}
END

FORM (KlattGrid_replaceAmplitudeTier, L"KlattGrid: Replace amplitude tier", 0)
	KlattGrid_4formants_addCommonField (dia);
	NATURAL (L"Formant number", L"1")
	OK
DO
	int formantType = GET_INTEGER (L"Formant type");
	KlattGrid me = FIRST (KlattGrid);
	IntensityTier thee = FIRST (IntensityTier);
	KlattGrid_replaceAmplitudeTier (me, formantType, GET_INTEGER (L"Formant number"), thee);
	praat_dataChanged (me);
END

FORM (KlattGrid_to_Sound_special, L"KlattGrid: To Sound (special)", L"KlattGrid: To Sound (special)...")
	KlattGrid_PlayOptions_addCommonFields (dia, 1);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_setDefaultPlayOptions (me);
		KlattGrid_PlayOptions_getCommonFields (dia, 1, me);
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

FORM (KlattGrid_playSpecial, L"KlattGrid: Play special", L"KlattGrid: Play special...")
	KlattGrid_PlayOptions_addCommonFields (dia, 0);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_setDefaultPlayOptions (me);
		KlattGrid_PlayOptions_getCommonFields (dia, 0, me);
		KlattGrid_playSpecial (me);
	}
END

FORM (KlattGrid_to_Sound_phonation, L"KlattGrid: To Sound (phonation)", L"KlattGrid: To Sound (phonation)...")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	KlattGrid_PhonationGridPlayOptions_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (KlattGrid);
		KlattGrid_PhonationGridPlayOptions_getCommonFields (dia, me);
		my options -> samplingFrequency = GET_REAL (L"Sampling frequency");
		praat_new (KlattGrid_to_Sound_phonation (me), my name, L"_phonation");
	}
END

DIRECT (KlattGrid_help) Melder_help (L"KlattGrid"); END

DIRECT (KlattGrid_play)
	LOOP {
		iam (KlattGrid);
		KlattGrid_play (me);
	}
END

FORM (KlattGrid_draw, L"KlattGrid: Draw", 0)
	RADIO (L"Synthesis model", 1)
	RADIOBUTTON (L"Cascade")
	RADIOBUTTON (L"Parallel")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		KlattGrid_draw (me, GRAPHICS, GET_INTEGER (L"Synthesis model") - 1);
	}
END

FORM (KlattGrid_drawVocalTract, L"KlattGrid: Draw vocal tract", 0)
	RADIO (L"Synthesis model", 1)
	RADIOBUTTON (L"Cascade")
	RADIOBUTTON (L"Parallel")
	BOOLEAN (L"Include tracheal formants", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		KlattGrid_drawVocalTract (me, GRAPHICS, GET_INTEGER (L"Synthesis model") - 1, GET_INTEGER (L"Include tracheal formants"));
	}
END

DIRECT (KlattGrid_drawPhonation)
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		PhonationGrid_draw (my phonation, GRAPHICS);
	}
END

DIRECT (KlattGrid_drawFrication)
	autoPraatPicture picture;
	LOOP {
		iam (KlattGrid);
		FricationGrid_draw (my frication, GRAPHICS);
	}
END

FORM (KlattGrid_to_oralFormantGrid_openPhases, L"KlattGrid: Extract oral formant grid (open phases)", L"KlattGrid: Extract oral formant grid (open phases)...")
	REAL (L"Fade fraction (0..0.5)", L"0.1")
	OK
DO
	double fadeFraction = GET_REAL (L"Fade fraction");
	REQUIRE (fadeFraction < 0.5, L"Fade fraction has to be smaller than 0.5.")
	LOOP {
		iam (KlattGrid);
		praat_new (KlattGrid_to_oralFormantGrid_openPhases (me, fadeFraction), L"corrected");
	}
END

FORM (Sound_KlattGrid_filterByVocalTract, L"Sound & KlattGrid: Filter by vocal tract", L"Sound & KlattGrid: Filter by vocal tract...")
	RADIO (L"Vocal tract filter model", 1)
	RADIOBUTTON (L"Cascade")
	RADIOBUTTON (L"Parallel")
	OK
DO
	Sound me = FIRST (Sound);
	KlattGrid thee = FIRST (KlattGrid);
	int filterModel = GET_INTEGER (L"Vocal tract filter model") - 1;
	praat_new (Sound_KlattGrid_filterByVocalTract (me, thee, filterModel), my name, L"_", thy name);
END

void praat_KlattGrid_init ();
void praat_KlattGrid_init () {

	Thing_recognizeClassesByName (classKlattGrid, NULL);

	praat_addMenuCommand (L"Objects", L"New", L"Acoustic synthesis (Klatt)", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"KlattGrid help", 0, 1, DO_KlattGrid_help);
	praat_addMenuCommand (L"Objects", L"New", L"-- the synthesizer grid --", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create KlattGrid...", 0, 1, DO_KlattGrid_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create KlattGrid example", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_createExample);

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
	praat_addAction1 (classKlattGrid, 0, L"KlattGrid help", 0, 0, DO_KlattGrid_help);
	praat_addAction1 (classKlattGrid, 0, L"Edit phonation -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Edit pitch tier", 0, 1, DO_KlattGrid_editPitchTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit voicing amplitude tier", 0, 1, DO_KlattGrid_editVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit flutter tier", 0, 1, DO_KlattGrid_editFlutterTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit power1 tier", 0, 1, DO_KlattGrid_editPower1Tier);
	praat_addAction1 (classKlattGrid, 0, L"Edit power2 tier", 0, 1, DO_KlattGrid_editPower2Tier);
	praat_addAction1 (classKlattGrid, 0, L"Edit open phase tier", 0, 1, DO_KlattGrid_editOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit collision phase tier", 0, 1, DO_KlattGrid_editCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit double pulsing tier", 0, 1, DO_KlattGrid_editDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit spectral tilt tier", 0, 1, DO_KlattGrid_editSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit aspiration amplitude tier", 0, 1, DO_KlattGrid_editAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit breathiness amplitude tier", 0, 1, DO_KlattGrid_editBreathinessAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, L"Edit filters -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Edit oral formant grid", 0, 1, DO_KlattGrid_editOralFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit nasal formant grid", 0, 1, DO_KlattGrid_editNasalFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit nasal antiformant grid", 0, 1, DO_KlattGrid_editNasalAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit oral formant amplitude tier...", 0, 1, DO_KlattGrid_editOralFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit nasal formant amplitude tier...", 0, 1, DO_KlattGrid_editNasalFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"-- edit delta formant grid --", 0, 1, 0);
	praat_addAction1 (classKlattGrid, 0, L"Edit delta formant grid", 0, 1, DO_KlattGrid_editDeltaFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit tracheal formant grid", 0, 1, DO_KlattGrid_editTrachealFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit tracheal antiformant grid", 0, 1, DO_KlattGrid_editTrachealAntiFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit tracheal formant amplitude tier...", 0, 1, DO_KlattGrid_editTrachealFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"-- edit frication tiers --", 0, 1, 0);
	praat_addAction1 (classKlattGrid, 1, L"Edit frication amplitude tier", 0, 1, DO_KlattGrid_editFricationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit frication formant grid", 0, 1, DO_KlattGrid_editFricationFormantGrid);
	praat_addAction1 (classKlattGrid, 0, L"Edit frication formant amplitude tier...", 0, 1, DO_KlattGrid_editFricationFormantAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Edit frication bypass tier", 0, 1, DO_KlattGrid_editFricationBypassTier);
	praat_addAction1 (classKlattGrid, 1, L"Edit frication amplitude tier", 0, 1, DO_KlattGrid_editFricationAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, L"Play", 0, 0, DO_KlattGrid_play);
	praat_addAction1 (classKlattGrid, 0, L"Play special...", 0, 0, DO_KlattGrid_playSpecial);
	praat_addAction1 (classKlattGrid, 0, L"To Sound", 0, 0, DO_KlattGrid_to_Sound);
	praat_addAction1 (classKlattGrid, 0, L"To Sound (special)...", 0, 0, DO_KlattGrid_to_Sound_special);
	praat_addAction1 (classKlattGrid, 0, L"To Sound (phonation)...", 0, 0, DO_KlattGrid_to_Sound_phonation);

	praat_addAction1 (classKlattGrid, 0, L"Draw -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Draw synthesizer...", 0, 1, DO_KlattGrid_draw);
	praat_addAction1 (classKlattGrid, 0, L"Draw vocal tract...", 0, 1, DO_KlattGrid_drawVocalTract);
	praat_addAction1 (classKlattGrid, 0, L"Draw phonation", 0, 1, DO_KlattGrid_drawPhonation);
	praat_addAction1 (classKlattGrid, 0, L"Draw frication", 0, 1, DO_KlattGrid_drawFrication);

	praat_addAction1 (classKlattGrid, 0, L"Query phonation -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 1, L"Get pitch at time...", 0, 1, DO_KlattGrid_getPitchAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get voicing amplitude at time...", 0, 1, DO_KlattGrid_getVoicingAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get flutter at time...", 0, 1, DO_KlattGrid_getFlutterAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get power1 at time...", 0, 1, DO_KlattGrid_getPower1AtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get power2 at time...", 0, 1, DO_KlattGrid_getPower2AtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get open phase at time...", 0, 1, DO_KlattGrid_getOpenPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get collision phase at time...", 0, 1, DO_KlattGrid_getCollisionPhaseAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get double pulsing at time...", 0, 1, DO_KlattGrid_getDoublePulsingAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get spectral tilt at time...", 0, 1, DO_KlattGrid_getSpectralTiltAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get aspiration amplitude at time...", 0, 1, DO_KlattGrid_getAspirationAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get breathiness amplitude at time...", 0, 1, DO_KlattGrid_getBreathinessAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, L"Query filters -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 1, L"Get formant at time...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get bandwidth at time...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getBandwidthAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get amplitude at time...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getAmplitudeAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get delta formant at time...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getDeltaFormantAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get delta bandwidth at time...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_getDeltaBandwidthAtTime);

#define KlattGRID_GET_FORMANT_FB_VALUES_ACTION(Name,namef) \
	praat_addAction1 (classKlattGrid, 1, L"Get " #namef "ormant frequency at time...", 0, 1, DO_KlattGrid_get##Name##FormantFrequencyAtTime); \
	praat_addAction1 (classKlattGrid, 1, L"Get " #namef "ormant bandwidth at time...", 0, 1, DO_KlattGrid_get##Name##FormantBandwidthAtTime);

#define KlattGRID_GET_FORMANT_A_VALUES_ACTION(Name,name) \
	praat_addAction1 (classKlattGrid, 1, L"Get " #name " formant amplitude at time...", 0, 1, DO_KlattGrid_get##Name##FormantAmplitudeAtTime); \

	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Oral, oral f)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Oral, oral)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Nasal, nasal f)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Nasal, nasal)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (NasalAnti, nasal antif)

	praat_addAction1 (classKlattGrid, 1, L"-- query delta characteristics", 0, 1, 0);
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Delta, delta f)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Tracheal, tracheal f)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Tracheal, tracheal)
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (TrachealAnti, tracheal antif)
	praat_addAction1 (classKlattGrid, 1, L"-- query frication characteristics", 0, 1, 0);
	KlattGRID_GET_FORMANT_FB_VALUES_ACTION (Frication, frication f)
	KlattGRID_GET_FORMANT_A_VALUES_ACTION (Frication, frication)

#undef KlattGRID_GET_FORMANT_A_VALUES_ACTION
#undef KlattGRID_GET_FORMANT_A_VALUES_ACTION

	praat_addAction1 (classKlattGrid, 1, L"Get frication bypass at time...", 0, 1, DO_KlattGrid_getFricationBypassAtTime);
	praat_addAction1 (classKlattGrid, 1, L"Get frication amplitude at time...", 0, 1, DO_KlattGrid_getFricationAmplitudeAtTime);

	praat_addAction1 (classKlattGrid, 0, L"Modify phonation -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Add pitch point...", 0, 1, DO_KlattGrid_addPitchPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add voicing amplitude point...", 0, 1, DO_KlattGrid_addVoicingAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, L"Add flutter point...", 0, 1, DO_KlattGrid_addFlutterPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add power1 point...", 0, 1, DO_KlattGrid_addPower1Point);
	praat_addAction1 (classKlattGrid, 0, L"Add power2 point...", 0, 1, DO_KlattGrid_addPower2Point);
	praat_addAction1 (classKlattGrid, 0, L"Add open phase point...", 0, 1, DO_KlattGrid_addOpenPhasePoint);
	praat_addAction1 (classKlattGrid, 0, L"Add collision phase point...", 0, 1, DO_KlattGrid_addCollisionPhasePoint);
	praat_addAction1 (classKlattGrid, 0, L"Add double pulsing point...", 0, 1, DO_KlattGrid_addDoublePulsingPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add spectral tilt point...", 0, 1, DO_KlattGrid_addSpectralTiltPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add aspiration amplitude point...", 0, 1, DO_KlattGrid_addAspirationAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, L"Add breathiness amplitude point...", 0, 1, DO_KlattGrid_addBreathinessAmplitudePoint);

#define KlattGrid_REMOVE_POINTS_ACTION(Name,name) \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #name " points between...", 0, praat_DEPTH_1+praat_HIDDEN, DO_KlattGrid_remove##Name##Points); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #name " points...", 0, 1, DO_KlattGrid_remove##Name##Points);

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

	praat_addAction1 (classKlattGrid, 0, L"Modify vocal tract -", 0, 0, 0);

#define KlattGrid_MODIFY_ACTION_FBA(Name,namef) \
	praat_addAction1 (classKlattGrid, 0, L"Formula (" #namef "ormant frequencies)...", 0, 1, DO_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, L"Formula (" #namef "ormant bandwidths)...", 0, 1, DO_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant frequency point...", 0, 1, DO_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant bandwidth point...", 0, 1, DO_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant amplitude point...", 0, 1, DO_KlattGrid_add##Name##FormantAmplitudePoint); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant frequency points...", 0, 1, DO_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant bandwidth points...", 0, 1, DO_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant amplitude points...", 0, 1, DO_KlattGrid_remove##Name##FormantAmplitudePoints); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant...", 0, 1, DO_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant...", 0, 1, DO_KlattGrid_remove##Name##Formant);

#define KlattGrid_MODIFY_ACTION_FB(Name,namef) \
	praat_addAction1 (classKlattGrid, 0, L"Formula (" #namef "ormant frequencies)...", 0, 1, DO_KlattGrid_formula##Name##FormantFrequencies); \
	praat_addAction1 (classKlattGrid, 0, L"Formula (" #namef "ormant bandwidths)...", 0, 1, DO_KlattGrid_formula##Name##FormantBandwidths); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant frequency point...", 0, 1, DO_KlattGrid_add##Name##FormantFrequencyPoint); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant bandwidth point...", 0, 1, DO_KlattGrid_add##Name##FormantBandwidthPoint); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant frequency points...", 0, 1, DO_KlattGrid_remove##Name##FormantFrequencyPoints); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant bandwidth points...", 0, 1, DO_KlattGrid_remove##Name##FormantBandwidthPoints); \
	praat_addAction1 (classKlattGrid, 0, L"Add " #namef "ormant...", 0, 1, DO_KlattGrid_add##Name##Formant); \
	praat_addAction1 (classKlattGrid, 0, L"Remove " #namef "ormant...", 0, 1, DO_KlattGrid_remove##Name##Formant);

	KlattGrid_MODIFY_ACTION_FBA (Oral, oral f)
	praat_addAction1 (classKlattGrid, 0, L"-- oral modify separator --", 0, 1, 0);
	KlattGrid_MODIFY_ACTION_FBA (Nasal, nasal f)
	praat_addAction1 (classKlattGrid, 0, L"-- nasal modify separator --", 0, 1, 0);
	KlattGrid_MODIFY_ACTION_FB (NasalAnti, nasal antif)

	praat_addAction1 (classKlattGrid, 0, L"Formula (frequencies)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_formula_frequencies);
	praat_addAction1 (classKlattGrid, 0, L"Formula (bandwidths)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_formula_bandwidths);
	praat_addAction1 (classKlattGrid, 0, L"Add formant point...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addFormantPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add bandwidth point...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add amplitude point...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addAmplitudePoint);
	praat_addAction1 (classKlattGrid, 0, L"Remove formant points between...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeFormantPoints);
	praat_addAction1 (classKlattGrid, 0, L"Remove bandwidth points between...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeBandwidthPoints);
	praat_addAction1 (classKlattGrid, 0, L"Remove amplitude points between...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeAmplitudePoints);
	praat_addAction1 (classKlattGrid, 0, L"Modify coupling - ", 0, 0, 0);
	KlattGrid_MODIFY_ACTION_FB (Delta, delta f)
	praat_addAction1 (classKlattGrid, 0, L"-- delta modify separator --", 0, 1, 0);
	KlattGrid_MODIFY_ACTION_FBA (Tracheal, tracheal f)
	praat_addAction1 (classKlattGrid, 0, L"-- nasal modify separator --", 0, 1, 0);
	KlattGrid_MODIFY_ACTION_FB (TrachealAnti, tracheal antif)

	praat_addAction1 (classKlattGrid, 0, L"Add delta formant point...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addDeltaFormantPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add delta bandwidth point...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addDeltaBandwidthPoint);
	praat_addAction1 (classKlattGrid, 0, L"Remove delta formant points between...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeDeltaFormantPoints);
	praat_addAction1 (classKlattGrid, 0, L"Remove delta bandwidth points between...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_removeDeltaBandwidthPoints);

	praat_addAction1 (classKlattGrid, 0, L"Modify frication -", 0, 0, 0);
	KlattGrid_MODIFY_ACTION_FBA (Frication, frication f)
	praat_addAction1 (classKlattGrid, 0, L"-- frication modify separator --", 0, 1, 0);

	praat_addAction1 (classKlattGrid, 0, L"Add frication bypass point...", 0, 1, DO_KlattGrid_addFricationBypassPoint);
	praat_addAction1 (classKlattGrid, 0, L"Add frication amplitude point...", 0, 1, DO_KlattGrid_addFricationAmplitudePoint);
	KlattGrid_REMOVE_POINTS_ACTION (FricationBypass, frication bypass)
	KlattGrid_REMOVE_POINTS_ACTION (FricationAmplitude, frication amplitude)
	praat_addAction1 (classKlattGrid, 0, L"Add formant and bandwidth tier...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_addFormantAndBandwidthTier);

#undef KlattGrid_REMOVE_POINTS_ACTION
#undef KlattGrid_MODIFY_ACTION_FB
#undef KlattGrid_MODIFY_ACTION_FBA

	praat_addAction1 (classKlattGrid, 0, L"Extract phonation -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Extract pitch tier", 0, 1, DO_KlattGrid_extractPitchTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract voicing amplitude tier", 0, 1, DO_KlattGrid_extractVoicingAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract flutter tier", 0, 1, DO_KlattGrid_extractFlutterTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract power1 tier", 0, 1, DO_KlattGrid_extractPower1Tier);
	praat_addAction1 (classKlattGrid, 0, L"Extract power2 tier", 0, 1, DO_KlattGrid_extractPower2Tier);
	praat_addAction1 (classKlattGrid, 0, L"Extract open phase tier", 0, 1, DO_KlattGrid_extractOpenPhaseTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract collision phase tier", 0, 1, DO_KlattGrid_extractCollisionPhaseTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract double pulsing tier", 0, 1, DO_KlattGrid_extractDoublePulsingTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract spectral tilt tier", 0, 1, DO_KlattGrid_extractSpectralTiltTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract aspiration amplitude tier", 0, 1, DO_KlattGrid_extractAspirationAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract breathiness amplitude tier", 0, 1, DO_KlattGrid_extractBreathinessAmplitudeTier);
	praat_addAction1 (classKlattGrid, 0, L"-- extract glottal events--", 0, 1, 0);
	praat_addAction1 (classKlattGrid, 0, L"Extract PointProcess (glottal closures)", 0, 1, DO_KlattGrid_extractPointProcess_glottalClosures);

#define KlattGRID_EXTRACT_FORMANT_GRID_ACTION(Name,namef) \
	praat_addAction1 (classKlattGrid, 0, L"Extract " #namef "ormant grid", 0, 1, DO_KlattGrid_extract##Name##FormantGrid);
#define KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION(Name,name) \
	praat_addAction1 (classKlattGrid, 0, L"Extract " #name " formant amplitude tier...", 0, 1, DO_KlattGrid_extract##Name##FormantAmplitudeTier);

	praat_addAction1 (classKlattGrid, 0, L"Extract filters -", 0, 0, 0);
	praat_addAction1 (classKlattGrid, 0, L"Extract formant grid...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_extractFormantGrid); // deprecated
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Oral, oral f)
	praat_addAction1 (classKlattGrid, 0, L"Extract amplitude tier...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_KlattGrid_extractAmplitudeTier); // deprecated
	praat_addAction1 (classKlattGrid, 0, L"Extract formant grid (open phases)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_to_oralFormantGrid_openPhases);
	praat_addAction1 (classKlattGrid, 0, L"Extract oral formant grid (open phases)...", 0, 1, DO_KlattGrid_to_oralFormantGrid_openPhases);
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Oral, oral)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Nasal, nasal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Nasal, nasal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (NasalAnti, nasal antif)

	praat_addAction1 (classKlattGrid, 0, L"-- extract delta characteristics", 0, 1, 0);
	praat_addAction1 (classKlattGrid, 0, L"Extract delta formant grid", 0, 1, DO_KlattGrid_extractDeltaFormantGrid);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Tracheal, tracheal f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal)
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (TrachealAnti, tracheal antif)
	praat_addAction1 (classKlattGrid, 0, L"-- extract frication characteristics", 0, 1, 0);
	KlattGRID_EXTRACT_FORMANT_GRID_ACTION (Frication, frication f)
	KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION (Frication, frication)
	praat_addAction1 (classKlattGrid, 0, L"Extract frication bypass tier", 0, 1, DO_KlattGrid_extractFricationBypassTier);
	praat_addAction1 (classKlattGrid, 0, L"Extract frication amplitude tier", 0, 1, DO_KlattGrid_extractFricationAmplitudeTier);

#undef KlattGRID_EXTRACT_FORMANT_AMPLITUDE_ACTION
#undef KlattGRID_EXTRACT_FORMANT_GRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classPitchTier, 1, L"Replace pitch tier", 0, 1, DO_KlattGrid_replacePitchTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace flutter tier", 0, 1, DO_KlattGrid_replaceFlutterTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace power1 tier", 0, 1, DO_KlattGrid_replacePower1Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace power2 tier", 0, 1, DO_KlattGrid_replacePower2Tier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace open phase tier", 0, 1, DO_KlattGrid_replaceOpenPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace collision phase tier", 0, 1, DO_KlattGrid_replaceCollisionPhaseTier);
	praat_addAction2 (classKlattGrid, 1, classRealTier, 1, L"Replace double pulsing tier", 0, 1, DO_KlattGrid_replaceDoublePulsingTier);

	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"-- replace formant amplitudes --", 0, 1, 0);

#define KlattGrid_REPLACE_FORMANTGRID_ACTION(Name,namef) \
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, L"Replace " #namef "ormant grid", 0, 1, DO_KlattGrid_replace##Name##FormantGrid);
#define KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION(Name,namef) \
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace " #namef "ormant amplitude tier...", 0, 1, DO_KlattGrid_replace##Name##FormantAmplitudeTier);


	KlattGrid_REPLACE_FORMANTGRID_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (NasalAnti, nasal antif)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, L"-- replace coupling --", 0, 1, 0);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (TrachealAnti, tracheal antif)
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Delta, delta f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, L"-- replace frication --", 0, 1, 0);
	KlattGrid_REPLACE_FORMANTGRID_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classFormantGrid, 1, L"Replace formant grid...", 0, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_replaceFormantGrid);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace voicing amplitude tier", 0, 1, DO_KlattGrid_replaceVoicingAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace spectral tilt tier", 0, 1, DO_KlattGrid_replaceSpectralTiltTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace aspiration amplitude tier", 0, 1,
	                  DO_KlattGrid_replaceAspirationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace breathiness amplitude tier", 0, 1,
	                  DO_KlattGrid_replaceBreathinessAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace amplitude tier...", 0, praat_HIDDEN + praat_DEPTH_1, DO_KlattGrid_replaceAmplitudeTier);
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Oral, oral f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Nasal, nasal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Tracheal, tracheal f)
	KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION (Frication, frication f)
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace frication amplitude tier", 0, 1, DO_KlattGrid_replaceFricationAmplitudeTier);
	praat_addAction2 (classKlattGrid, 1, classIntensityTier, 1, L"Replace frication bypass tier", 0, 1, DO_KlattGrid_replaceFricationBypassTier);

#undef KlattGrid_REPLACE_FORMANT_AMPLITUDE_ACTION
#undef KlattGrid_REPLACE_FORMANTGRID_ACTION

	praat_addAction2 (classKlattGrid, 1, classSound, 1, L"Filter by vocal tract...", 0, 1, DO_Sound_KlattGrid_filterByVocalTract);

	INCLUDE_MANPAGES (manual_KlattGrid)
}


/* End of file praat_KlattGrid_init.cpp */