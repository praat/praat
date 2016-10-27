/* praat_Tiers.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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

#include "Ltas.h"
#include "PitchTier_to_PointProcess.h"
#include "PitchTier_to_Sound.h"
#include "Matrix_and_PointProcess.h"
#include "PointProcess_and_Sound.h"
#include "Sound_PointProcess.h"
#include "SpectrumTier.h"
#include "TextGrid.h"
#include "VoiceAnalysis.h"

#include "praat_Tiers.h"

#undef iam
#define iam iam_LOOP

// MARK: - AMPLITUDETIER

// MARK: New

FORM (NEW1_AmplitudeTier_create, U"Create empty AmplitudeTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoAmplitudeTier thee = AmplitudeTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END }

// MARK: Help

DIRECT3 (HELP_AmplitudeTier_help) {
	Melder_help (U"AmplitudeTier");
END }

// MARK: View & Edit

DIRECT3 (WINDOW_AmplitudeTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an AmplitudeTier from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
	}
	LOOP if (CLASS == classAmplitudeTier) {
		iam (AmplitudeTier);
		autoAmplitudeTierEditor editor = AmplitudeTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

DIRECT3 (INFO_AmplitudeTier_Sound_viewAndEdit) {
	Melder_information (U"To include a copy of a Sound in your AmplitudeTier window:\n"
		"   select an AmplitudeTier and a Sound, and click \"View & Edit\".");
END }

// MARK: Query

static void dia_AmplitudeTier_getRangeProperty (UiForm dia) {
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum amplitude factor", U"1.6")
}

FORM (REAL_AmplitudeTier_getShimmer_local, U"AmplitudeTier: Get shimmer (local)", U"AmplitudeTier: Get shimmer (local)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

FORM (REAL_AmplitudeTier_getShimmer_local_dB, U"AmplitudeTier: Get shimmer (local, dB)", U"AmplitudeTier: Get shimmer (local, dB)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local_dB (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

FORM (REAL_AmplitudeTier_getShimmer_apq3, U"AmplitudeTier: Get shimmer (apq3)", U"AmplitudeTier: Get shimmer (apq3)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq3 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

FORM (REAL_AmplitudeTier_getShimmer_apq5, U"AmplitudeTier: Get shimmer (apq5)", U"AmplitudeTier: Get shimmer (apq5)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq5 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

FORM (REAL_AmplitudeTier_getShimmer_apq11, U"AmplitudeTier: Get shimmer (apq11)", U"AmplitudeTier: Get shimmer (apq11)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq11 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

FORM (REAL_AmplitudeTier_getShimmer_dda, U"AmplitudeTier: Get shimmer (dda)", U"AmplitudeTier: Get shimmer (dda)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_dda (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END }

/*FORM (REAL_AmplitudeTier_getValueAtTime, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL (U"Time")), U"Pa");
END }
	
FORM (REAL_AmplitudeTier_getValueAtIndex, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER (U"Point number")), U"Pa");
END }*/

// MARK: Modify

FORM (MODIFY_AmplitudeTier_addPoint, U"Add one point", U"AmplitudeTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Sound pressure (Pa)", U"0.8")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Sound pressure"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_AmplitudeTier_formula, U"AmplitudeTier: Formula", U"AmplitudeTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in Pascal")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"- self ; upside down")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		try {
			RealTier_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END }

// MARK: Synthesize

FORM (NEW_AmplitudeTier_to_Sound, U"AmplitudeTier: To Sound (pulse train)", U"AmplitudeTier: To Sound (pulse train)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		autoSound thee = AmplitudeTier_to_Sound (me, GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Interpolation depth"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: Convert

DIRECT3 (NEW_AmplitudeTier_downto_PointProcess) {
	LOOP {
		iam (AmplitudeTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END }

DIRECT3 (NEW_AmplitudeTier_downto_TableOfReal) {
	LOOP {
		iam (AmplitudeTier);
		autoTableOfReal thee = AmplitudeTier_downto_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_AmplitudeTier_to_IntensityTier, U"AmplitudeTier: To IntensityTier", U"AmplitudeTier: To IntensityTier...") {
	REAL (U"Threshold (dB)", U"-10000.0")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		autoIntensityTier thee = AmplitudeTier_to_IntensityTier (me, GET_REAL (U"Threshold"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: - AMPLITUDETIER & SOUND

DIRECT3 (NEW1_Sound_AmplitudeTier_multiply) {
	Sound sound = nullptr;
	AmplitudeTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classAmplitudeTier) tier = (AmplitudeTier) OBJECT;
	}
	autoSound thee = Sound_AmplitudeTier_multiply (sound, tier);
	praat_new (thee.move(), sound -> name, U"_amp");
END }

// MARK: - DURATIONTIER

// MARK: New

FORM (NEW1_DurationTier_create, U"Create empty DurationTier", U"Create DurationTier...") {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoDurationTier thee = DurationTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END }

// MARK: Help

DIRECT3 (HELP_DurationTier_help) {
	Melder_help (U"DurationTier");
END }

// MARK: View & Edit

DIRECT3 (WINDOW_DurationTier_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a DurationTier from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
	}
	LOOP if (CLASS == classDurationTier) {
		iam (DurationTier);
		autoDurationTierEditor editor = DurationTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

DIRECT3 (INFO_DurationTier_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your DurationTier editor:\n"
		"   select a DurationTier and a Sound, and click \"View & Edit\".");
END }

DIRECT3 (INFO_DurationTier_Manipulation_replace) {
	Melder_information (U"To replace the DurationTier in a Manipulation object,\n"
		"select a DurationTier object and a Manipulation object\nand choose \"Replace duration\".");
END }

// MARK: Query

FORM (REAL_DurationTier_getTargetDuration, U"Get target duration", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	OK
DO
	LOOP {
		iam (DurationTier);
		double area = RealTier_getArea (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (area, U"seconds");
	}
END }

FORM (REAL_DurationTier_getValueAtTime, U"Get DurationTier value", U"DurationTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (U"Time"));
		Melder_informationReal (value, nullptr);
	}
END }
	
FORM (REAL_DurationTier_getValueAtIndex, U"Get DurationTier value", U"Duration: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (U"Point number"));
		Melder_informationReal (value, nullptr);
	}
END }

// MARK: Modify

FORM (MODIFY_DurationTier_addPoint, U"Add one point to DurationTier", U"DurationTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Relative duration", U"1.5")
	OK
DO
	LOOP {
		iam (DurationTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Relative duration"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_DurationTier_formula, U"DurationTier: Formula", U"DurationTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in relative units")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self * 1.5 ; slow down")
	OK
DO
	LOOP {
		iam (DurationTier);
		try {
			RealTier_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END }

// MARK: Convert

DIRECT3 (NEW_DurationTier_downto_PointProcess) {
	LOOP {
		iam (DurationTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END }

// MARK: - FORMANTGRID

// MARK: New

FORM (NEW1_FormantGrid_create, U"Create FormantGrid", nullptr) {
	WORD (U"Name", U"schwa")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	NATURAL (U"Number of formants", U"10")
	POSITIVE (U"Initial first formant (Hz)", U"550.0")
	POSITIVE (U"Initial formant spacing (Hz)", U"1100.0")
	REAL (U"Initial first bandwidth (Hz)", U"60.0")
	REAL (U"Initial bandwidth spacing (Hz)", U"50.0")
	OK
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoFormantGrid thee = FormantGrid_create (startTime, endTime, GET_INTEGER (U"Number of formants"),
		GET_REAL (U"Initial first formant"), GET_REAL (U"Initial formant spacing"),
		GET_REAL (U"Initial first bandwidth"), GET_REAL (U"Initial bandwidth spacing"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END }

// MARK: Help

DIRECT3 (HELP_FormantGrid_help) {
	Melder_help (U"FormantGrid");
END }

// MARK: View & Edit

static void cb_FormantGridEditor_publish (Editor /* me */, autoDaata publish) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publish.move(), U"fromFormantGridEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT3 (WINDOW_FormantGrid_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a FormantGrid from batch.");
	LOOP {
		iam (FormantGrid);
		autoFormantGridEditor editor = FormantGridEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_FormantGridEditor_publish);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: Modify

FORM (MODIFY_FormantGrid_formula_frequencies, U"FormantGrid: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	LOOP {
		iam (FormantGrid);
		try {
			FormantGrid_formula_frequencies (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the FormantGrid may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_FormantGrid_formula_bandwidths, U"FormantGrid: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	LABEL (U"", U"self [] is the FormantGrid itself, so it returns frequencies, not bandwidths!")
	TEXTFIELD (U"formula", U"self / 10 ; one tenth of the formant frequency")
	OK
DO
	LOOP {
		iam (FormantGrid);
		try {
			FormantGrid_formula_bandwidths (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the FormantGrid may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_FormantGrid_addFormantPoint, U"FormantGrid: Add formant point", U"FormantGrid: Add formant point...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	POSITIVE (U"Frequency (Hz)", U"550.0")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addFormantPoint (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_REAL (U"Frequency"));
		praat_dataChanged (OBJECT);
	}
END }

FORM (MODIFY_FormantGrid_addBandwidthPoint, U"FormantGrid: Add bandwidth point", U"FormantGrid: Add bandwidth point...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addBandwidthPoint (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_REAL (U"Bandwidth"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_FormantGrid_removeBandwidthPointsBetween, U"Remove bandwidth points between", U"FormantGrid: Remove bandwidth points between...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"From time (s)", U"0.3")
	REAL (U"To time (s)", U"0.7")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeBandwidthPointsBetween (me, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_FormantGrid_removeFormantPointsBetween, U"Remove formant points between", U"FormantGrid: Remove formant points between...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"From time (s)", U"0.3")
	REAL (U"To time (s)", U"0.7")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeFormantPointsBetween (me, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END }

// MARK: Convert

FORM (NEW_FormantGrid_to_Formant, U"FormantGrid: To Formant", nullptr) {
	POSITIVE (U"Time step (s)", U"0.01")
	REAL (U"Intensity (Pa\u00B2)", U"0.1")
	OK
DO
	double intensity = GET_REAL (U"Intensity");
	if (intensity < 0.0) Melder_throw (U"Intensity cannot be negative.");
	LOOP {
		iam (FormantGrid);
		autoFormant thee = FormantGrid_to_Formant (me, GET_REAL (U"Time step"), intensity);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - FORMANTGRID & SOUND

DIRECT3 (NEW1_Sound_FormantGrid_filter) {
	Sound me = nullptr;
	FormantGrid grid = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter (me, grid);
	praat_new (thee.move(), my name, U"_filt");
END }

DIRECT3 (NEW1_Sound_FormantGrid_filter_noscale) {
	Sound me = nullptr;
	FormantGrid grid = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter_noscale (me, grid);
	praat_new (thee.move(), my name, U"_filt");
END }

// MARK: - FORMANTTIER

// MARK: New

FORM (NEW1_FormantTier_create, U"Create empty FormantTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoFormantTier thee = FormantTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END }

// MARK: Draw

FORM (GRAPHICS_FormantTier_speckle, U"Draw FormantTier", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (U"Maximum frequency (Hz)", U"5500.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (FormantTier);
		autoPraatPicture picture;
		FormantTier_speckle (me, GRAPHICS, fromTime, toTime, GET_REAL (U"Maximum frequency"), GET_INTEGER (U"Garnish"));
	}
END }

// MARK: Query

FORM (REAL_FormantTier_getValueAtTime, U"FormantTier: Get value", U"FormantTier: Get value at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (FormantTier);
		double value = FormantTier_getValueAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"));
		Melder_informationReal (value, U"hertz");
	}
END }
	
FORM (REAL_FormantTier_getBandwidthAtTime, U"FormantTier: Get bandwidth", U"FormantTier: Get bandwidth at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (FormantTier);
		double bandwidth = FormantTier_getBandwidthAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"));
		Melder_informationReal (bandwidth, U"hertz");
	}
END }
	
// MARK: Modify

FORM (MODIFY_FormantTier_addPoint, U"Add one point", U"FormantTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	LABEL (U"", U"Frequencies and bandwidths (Hz):")
	TEXTFIELD (U"fb pairs", U"500 50 1500 100 2500 150 3500 200 4500 300")
	OK
DO
	autoFormantPoint point = FormantPoint_create (GET_REAL (U"Time"));
	double *f = point -> formant, *b = point -> bandwidth;
	char *fbpairs = Melder_peek32to8 (GET_STRING (U"fb pairs"));
	int numberOfFormants = sscanf (fbpairs, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
		f, b, f+1, b+1, f+2, b+2, f+3, b+3, f+4, b+4, f+5, b+5, f+6, b+6, f+7, b+7, f+8, b+8, f+9, b+9) / 2;
	if (numberOfFormants < 1)
		Melder_throw (U"Number of formant-bandwidth pairs must be at least 1.");
	point -> numberOfFormants = numberOfFormants;
	LOOP {
		iam (FormantTier);
		autoFormantPoint point2 = Data_copy (point.get());
		AnyTier_addPoint_move (me->asAnyTier(), point2.move());
		praat_dataChanged (me);
	}
END }

// MARK: Convert

FORM (NEW_FormantTier_downto_TableOfReal, U"Down to TableOfReal", nullptr) {
	BOOLEAN (U"Include formants", true)
	BOOLEAN (U"Include bandwidths", false)
	OK
DO
	LOOP {
		iam (FormantTier);
		autoTableOfReal thee = FormantTier_downto_TableOfReal (me, GET_INTEGER (U"Include formants"), GET_INTEGER (U"Include bandwidths"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: - FORMANTTIER & SOUND

DIRECT3 (NEW1_Sound_FormantTier_filter) {
	Sound me = nullptr;
	FormantTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter (me, tier);
	praat_new (thee.move(), my name, U"_filt");
END }

DIRECT3 (NEW1_Sound_FormantTier_filter_noscale) {
	Sound me = nullptr;
	FormantTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter_noscale (me, tier);
	praat_new (thee.move(), my name, U"_filt");
END }

// MARK: - INTENSITYTIER

// MARK: New

FORM (NEW1_IntensityTier_create, U"Create empty IntensityTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoIntensityTier thee = IntensityTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END }

// MARK: Help

DIRECT (HELP_IntensityTier_help) {
	Melder_help (U"IntensityTier");
END }

// MARK: View & Edit

DIRECT (WINDOW_IntensityTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an IntensityTier from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
		if (sound) break;   // OPTIMIZE
	}
	LOOP if (CLASS == classIntensityTier) {
		iam (IntensityTier);
		autoIntensityTierEditor editor = IntensityTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

DIRECT (HINT_IntensityTier_Sound_viewAndEdit) {
	Melder_information (U"To include a copy of a Sound in your IntensityTier window:\n"
		"   select an IntensityTier and a Sound, and click \"View & Edit\".");
END }

// MARK: Query

FORM (REAL_IntensityTier_getValueAtTime, U"Get IntensityTier value", U"IntensityTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (U"Time"));
		Melder_informationReal (value, U"dB");
	}
END }
	
FORM (REAL_IntensityTier_getValueAtIndex, U"Get IntensityTier value", U"IntensityTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (U"Point number"));
		Melder_informationReal (value, U"dB");
	}
END }

// MARK: Modify

FORM (MODIFY_IntensityTier_addPoint, U"Add one point", U"IntensityTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Intensity (dB)", U"75")
	OK
DO
	LOOP {
		iam (IntensityTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Intensity"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_IntensityTier_formula, U"IntensityTier: Formula", U"IntensityTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in dB")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self + 3.0")
	OK
DO
	LOOP {
		iam (IntensityTier);
		try {
			RealTier_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the IntensityTier may have partially changed
			throw;
		}
	}
END }

// MARK: Convert

DIRECT (NEW_IntensityTier_downto_PointProcess) {
	LOOP {
		iam (IntensityTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_IntensityTier_downto_TableOfReal) {
	LOOP {
		iam (IntensityTier);
		autoTableOfReal thee = IntensityTier_downto_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_IntensityTier_to_AmplitudeTier) {
	LOOP {
		iam (IntensityTier);
		autoAmplitudeTier thee = IntensityTier_to_AmplitudeTier (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - INTENSITYTIER & POINTPROCESS

DIRECT (NEW1_IntensityTier_PointProcess_to_IntensityTier) {
	IntensityTier intensity = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = IntensityTier_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.move(), intensity -> name);
END }

// MARK: - INTENSITYTIER & SOUND

DIRECT (NEW1_Sound_IntensityTier_multiply_old) {
	Sound sound = nullptr;
	IntensityTier intensity = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (sound && intensity) break;   // OPTIMIZE
	}
	autoSound thee = Sound_IntensityTier_multiply (sound, intensity, true);
	praat_new (thee.move(), sound -> name, U"_int");
END }

FORM (NEW1_Sound_IntensityTier_multiply, U"Sound & IntervalTier: Multiply", nullptr) {
	BOOLEAN (U"Scale to 0.9", true)
	OK
DO
	Sound sound = nullptr;
	IntensityTier intensity = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (sound && intensity) break;   // OPTIMIZE
	}
	autoSound thee = Sound_IntensityTier_multiply (sound, intensity, GET_INTEGER (U"Scale to 0.9"));
	praat_new (thee.move(), sound -> name, U"_int");
END }

// MARK: - PITCHTIER

FORM (MODIFY_PitchTier_addPoint, U"PitchTier: Add point", U"PitchTier: Add point...") {
	REALVAR (time, U"Time (s)", U"0.5")
	REALVAR (pitch, U"Pitch (Hz)", U"200.0")
	OK
DO
	LOOP {
		iam (PitchTier);
		RealTier_addPoint (me, time, pitch);
		praat_dataChanged (me);
	}
END }

FORM (NEW1_PitchTier_create, U"Create empty PitchTier", nullptr) {
	WORDVAR (name, U"Name", U"empty")
	REALVAR (startTime, U"Start time (s)", U"0.0")
	REALVAR (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoPitchTier me = PitchTier_create (startTime, endTime);
	praat_new (me.move(), name);
END }

DIRECT (NEW_PitchTier_downto_PointProcess) {
	LOOP {
		iam (PitchTier);
		autoPointProcess you = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (you.move(), my name);
	}
END }

FORM (NEW_PitchTier_downto_TableOfReal, U"PitchTier: Down to TableOfReal", nullptr) {
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		autoTableOfReal thee = PitchTier_downto_TableOfReal (me, GET_INTEGER (U"Unit") - 1);
		praat_new (thee.move(), my name);
	}
END }

FORM (GRAPHICS_old_PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"500.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), U"lines and speckles");
	}
END }

FORM (GRAPHICS_PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"500.0")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_PitchTier_draw)
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END }

DIRECT (WINDOW_PitchTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PitchTier from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPitchTier) {
		iam (PitchTier);
		autoPitchTierEditor editor = PitchTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

FORM (MODIFY_PitchTier_formula, U"PitchTier: Formula", U"PitchTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in hertz")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self * 2 ; one octave up")
	OK
DO
	LOOP {
		iam (PitchTier);
		try {
			RealTier_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PitchTier may have partially changed
			throw;
		}
	}
END }

FORM (REAL_PitchTier_getMean_curve, U"PitchTier: Get mean (curve)", U"PitchTier: Get mean (curve)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	Melder_informationReal (RealTier_getMean_curve (FIRST_ANY (PitchTier), fromTime, toTime), U"Hz");
END }
	
FORM (REAL_PitchTier_getMean_points, U"PitchTier: Get mean (points)", U"PitchTier: Get mean (points)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	Melder_informationReal (RealTier_getMean_points (FIRST_ANY (PitchTier), fromTime, toTime), U"Hz");
END }
	
FORM (REAL_PitchTier_getStandardDeviation_curve, U"PitchTier: Get standard deviation (curve)", U"PitchTier: Get standard deviation (curve)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_curve (FIRST_ANY (PitchTier), fromTime, toTime), U"Hz");
END }
	
FORM (REAL_PitchTier_getStandardDeviation_points, U"PitchTier: Get standard deviation (points)", U"PitchTier: Get standard deviation (points)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_points (FIRST_ANY (PitchTier), fromTime, toTime), U"Hz");
END }
	
FORM (REAL_PitchTier_getValueAtTime, U"PitchTier: Get value at time", U"PitchTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (FIRST_ANY (PitchTier), GET_REAL (U"Time")), U"Hz");
END }
	
FORM (REAL_PitchTier_getValueAtIndex, U"PitchTier: Get value at index", U"PitchTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (FIRST_ANY (PitchTier), GET_INTEGER (U"Point number")), U"Hz");
END }

DIRECT (HELP_PitchTier_help) {
	Melder_help (U"PitchTier");
END }

DIRECT (PLAY_PitchTier_hum) {
	LOOP {
		iam (PitchTier);
		PitchTier_hum (me);
	}
END }

FORM (MODIFY_PitchTier_interpolateQuadratically, U"PitchTier: Interpolate quadratically", nullptr) {
	NATURAL (U"Number of points per parabola", U"4")
	RADIO (U"Unit", 2)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		RealTier_interpolateQuadratically (me, GET_INTEGER (U"Number of points per parabola"), GET_INTEGER (U"Unit") - 1);
		praat_dataChanged (me);
	}
END }

DIRECT (PLAY_PitchTier_play) {
	LOOP {
		iam (PitchTier);
		PitchTier_play (me);
	}
END }

DIRECT (PLAY_PitchTier_playSine) {
	LOOP {
		iam (PitchTier);
		PitchTier_playPart_sine (me, 0.0, 0.0);
	}
END }

FORM (MODIFY_PitchTier_shiftFrequencies, U"PitchTier: Shift frequencies", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1000.0")
	REALVAR (frequencyShift, U"Frequency shift", U"-20.0")
	OPTIONMENUVAR (unit, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK
DO
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	LOOP {
		iam (PitchTier);
		try {
			PitchTier_shiftFrequencies (me, fromTime, toTime, frequencyShift, unit);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PitchTier may have partially changed
			throw;
		}
	}
END }

#define MODIFY_EACH(klas)  LOOP { iam (klas);
#define MODIFY_EACH_END  praat_dataChanged (me); } END

FORM (MODIFY_PitchTier_multiplyFrequencies, U"PitchTier: Multiply frequencies", nullptr){
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1000.0")
	POSITIVEVAR (factor, U"Factor", U"1.2")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_multiplyFrequencies (me, fromTime, toTime, factor);
	MODIFY_EACH_END
}

FORM (MODIFY_PitchTier_stylize, U"PitchTier: Stylize", U"PitchTier: Stylize...") {
	REAL (U"Frequency resolution", U"4.0")
	RADIO (U"Unit", 2)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		PitchTier_stylize (me, GET_REAL (U"Frequency resolution"), GET_INTEGER (U"Unit") - 1);
		praat_dataChanged (me);
	}
END }

#define CONVERT_EACH(klas)  LOOP { iam (klas);
#define CONVERT_EACH_END(name)  praat_new (you.move(), name); } END

DIRECT (NEW_PitchTier_to_PointProcess) {
	CONVERT_EACH (PitchTier)
		autoPointProcess you = PitchTier_to_PointProcess (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PitchTier_to_Sound_phonation, U"PitchTier: To Sound (phonation)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Maximum period (s)", U"0.05")
	POSITIVE (U"Open phase", U"0.7")
	REAL (U"Collision phase", U"0.03")
	POSITIVE (U"Power 1", U"3.0")
	POSITIVE (U"Power 2", U"4.0")
	BOOLEAN (U"Hum", false)
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_phonation (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Maximum period"),
			GET_REAL (U"Open phase"), GET_REAL (U"Collision phase"), GET_REAL (U"Power 1"), GET_REAL (U"Power 2"), GET_INTEGER (U"Hum"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PitchTier_to_Sound_pulseTrain, U"PitchTier: To Sound (pulse train)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Adaptation time", U"0.05")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	BOOLEAN (U"Hum", false)
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_pulseTrain (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Adaptation time"),
			GET_INTEGER (U"Interpolation depth"), GET_INTEGER (U"Hum"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PitchTier_to_Sound_sine, U"PitchTier: To Sound (sine)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_sine (me, 0.0, 0.0, GET_REAL (U"Sampling frequency"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (HINT_PitchTier_Sound_viewAndEdit) {
	Melder_information (U"To include a copy of a Sound in your PitchTier window:\n"
		"   select a PitchTier and a Sound, and click \"View & Edit\".");
END }

FORM_SAVE (SAVE_PitchTier_writeToPitchTierSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"PitchTier") {
	LOOP {
		iam (PitchTier);
		PitchTier_writeToPitchTierSpreadsheetFile (me, file);
	}
END }

FORM_SAVE (SAVE_PitchTier_writeToHeaderlessSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"txt") {
	LOOP {
		iam (PitchTier);
		PitchTier_writeToHeaderlessSpreadsheetFile (me, file);
	}
END }

DIRECT3 (INFO_PitchTier_Manipulation_replace) {
	Melder_information (U"To replace the PitchTier in a Manipulation object,\n"
		"select a PitchTier object and a Manipulation object\nand choose \"Replace pitch\".");
END }

// MARK: - PITCHTIER & POINTPROCESS

DIRECT (NEW1_PitchTier_PointProcess_to_PitchTier) {
	PitchTier pitch = FIRST (PitchTier);
	PointProcess point = FIRST (PointProcess);
	autoPitchTier thee = PitchTier_PointProcess_to_PitchTier (pitch, point);
	praat_new (thee.move(), pitch -> name);
END }

// MARK: - POINTPROCESS

FORM (MODIFY_PointProcess_addPoint, U"PointProcess: Add point", U"PointProcess: Add point...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_addPoint (me, GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END }

FORM (NEW1_PointProcess_createEmpty, U"Create an empty PointProcess", U"Create empty PointProcess...") {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax < tmin) Melder_throw (U"End time (", tmax, U") should not be less than start time (", tmin, U").");
	autoPointProcess me = PointProcess_create (tmin, tmax, 0);
	praat_new (me.move(), GET_STRING (U"Name"));
END }

FORM (NEW1_PointProcess_createPoissonProcess, U"Create Poisson process", U"Create Poisson process...") {
	WORD (U"Name", U"poisson")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	POSITIVE (U"Density (/s)", U"100.0")
	OK
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax < tmin)
		Melder_throw (U"End time (", tmax, U") should not be less than start time (", tmin, U").");
	autoPointProcess me = PointProcess_createPoissonProcess (tmin, tmax, GET_REAL (U"Density"));
	praat_new (me.move(), GET_STRING (U"Name"));
END }

DIRECT (NEW1_PointProcess_difference) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_difference (point1, point2);
	praat_new (thee.move(), U"difference");
END }

FORM (GRAPHICS_PointProcess_draw, U"PointProcess: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPraatPicture picture;
		PointProcess_draw (me, GRAPHICS, fromTime, toTime, GET_INTEGER (U"Garnish"));
	}
END }

DIRECT (WINDOW_PointProcess_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PointProcess from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPointProcess) {
		iam (PointProcess);
		autoPointEditor editor = PointEditor_create (ID_AND_FULL_NAME, me, sound);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

FORM (MODIFY_PointProcess_fill, U"PointProcess: Fill", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (U"Period (s)", U"0.01")
	OK
DO
	LOOP {
		iam (PointProcess);
		try {
			PointProcess_fill (me, fromTime, toTime, GET_REAL (U"Period"));
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PointProcess may have partially changed
			throw;
		}
	}
END }

FORM (REAL_PointProcess_getInterval, U"PointProcess: Get interval", U"PointProcess: Get interval...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_informationReal (PointProcess_getInterval (FIRST_ANY (PointProcess), GET_REAL (U"Time")), U"seconds");
END }

#define dia_PointProcess_getRangeProperty(fromTime,toTime,shortestPeriod,longestPeriod,maximumPeriodfactor) \
	praat_TimeFunction_RANGE (fromTime, toTime) \
	REALVAR (shortestPeriod, U"Shortest period (s)", U"0.0001") \
	REALVAR (longestPeriod, U"Longest period (s)", U"0.02") \
	POSITIVEVAR (maximumPeriodFactor, U"Maximum period factor", U"1.3")

FORM (REAL_PointProcess_getJitter_local, U"PointProcess: Get jitter (local)", U"PointProcess: Get jitter (local)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), nullptr);
END }

FORM (REAL_PointProcess_getJitter_local_absolute, U"PointProcess: Get jitter (local, absolute)", U"PointProcess: Get jitter (local, absolute)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local_absolute (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), U"seconds");
END }

FORM (REAL_PointProcess_getJitter_rap, U"PointProcess: Get jitter (rap)", U"PointProcess: Get jitter (rap)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getJitter_rap (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), nullptr);
END }

FORM (REAL_PointProcess_getJitter_ppq5, U"PointProcess: Get jitter (ppq5)", U"PointProcess: Get jitter (ppq5)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ppq5 (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), nullptr);
END }

FORM (REAL_PointProcess_getJitter_ddp, U"PointProcess: Get jitter (ddp)", U"PointProcess: Get jitter (ddp)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ddp (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), nullptr);
END }

FORM (REAL_PointProcess_getMeanPeriod, U"PointProcess: Get mean period", U"PointProcess: Get mean period...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getMeanPeriod (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), U"seconds");
END }

FORM (REAL_PointProcess_getStdevPeriod, U"PointProcess: Get stdev period", U"PointProcess: Get stdev period...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_informationReal (PointProcess_getStdevPeriod (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor), U"seconds");
END }

FORM (INTEGER_PointProcess_getLowIndex, U"PointProcess: Get low index", U"PointProcess: Get low index...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_information (PointProcess_getLowIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END }

FORM (INTEGER_PointProcess_getHighIndex, U"PointProcess: Get high index", U"PointProcess: Get high index...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_information (PointProcess_getHighIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END }

FORM (INTEGER_PointProcess_getNearestIndex, U"PointProcess: Get nearest index", U"PointProcess: Get nearest index...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	Melder_information (PointProcess_getNearestIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END }

DIRECT (INTEGER_PointProcess_getNumberOfPoints) {
	PointProcess me = FIRST_ANY (PointProcess);
	Melder_information (my nt);
END }

FORM (INTEGER_PointProcess_getNumberOfPeriods, U"PointProcess: Get number of periods", U"PointProcess: Get number of periods...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	Melder_information (PointProcess_getNumberOfPeriods (FIRST_ANY (PointProcess), fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor));
END }

FORM (REAL_PointProcess_getTimeFromIndex, U"Get time", 0 /*"PointProcess: Get time from index..."*/) {
	NATURAL (U"Point number", U"10")
	OK
DO
	PointProcess me = FIRST_ANY (PointProcess);
	long i = GET_INTEGER (U"Point number");
	if (i > my nt) Melder_information (U"--undefined--");
	else Melder_informationReal (my t [i], U"seconds");
END }

DIRECT (HELP_PointProcess_help) {
	Melder_help (U"PointProcess");
END }

DIRECT (PLAY_PointProcess_hum) {
	LOOP {
		iam (PointProcess);
		PointProcess_hum (me, my xmin, my xmax);
	}
END }

DIRECT (NEW1_PointProcess_intersection) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_intersection (point1, point2);
	praat_new (thee.move(), U"intersection");
END }

DIRECT (PLAY_PointProcess_play) {
	LOOP {
		iam (PointProcess);
		PointProcess_play (me);
	}
END }

FORM (MODIFY_PointProcess_removePoint, U"PointProcess: Remove point", U"PointProcess: Remove point...") {
	NATURAL (U"Index", U"1")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoint (me, GET_INTEGER (U"Index"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_PointProcess_removePointNear, U"PointProcess: Remove point near", U"PointProcess: Remove point near...") {
	REAL (U"Time (s)", U"0.5")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointNear (me, GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_PointProcess_removePoints, U"PointProcess: Remove points", U"PointProcess: Remove points...") {
	NATURAL (U"From index", U"1")
	NATURAL (U"To index", U"10")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoints (me, GET_INTEGER (U"From index"), GET_INTEGER (U"To index"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_PointProcess_removePointsBetween, U"PointProcess: Remove points between", U"PointProcess: Remove points between...") {
	REAL (U"left Time range (s)", U"0.3")
	REAL (U"right Time range (s)", U"0.7")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointsBetween (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		praat_dataChanged (me);
	}
END }

DIRECT (NEW_PointProcess_to_IntervalTier) {
	LOOP {
		iam (PointProcess);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_PointProcess_to_Matrix) {
	LOOP {
		iam (PointProcess);
		autoMatrix thee = PointProcess_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_to_PitchTier, U"PointProcess: To PitchTier", U"PointProcess: To PitchTier...") {
	POSITIVE (U"Maximum interval (s)", U"0.02")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_to_PitchTier (me, GET_REAL (U"Maximum interval"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_to_TextGrid, U"PointProcess: To TextGrid...", U"PointProcess: To TextGrid...") {
	SENTENCE (U"Tier names", U"Mary John bell")
	SENTENCE (U"Point tiers", U"bell")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (U"Tier names"), GET_STRING (U"Point tiers"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_to_TextGrid_vuv, U"PointProcess: To TextGrid (vuv)...", U"PointProcess: To TextGrid (vuv)...") {
	POSITIVE (U"Maximum period (s)", U"0.02")
	REAL (U"Mean period (s)", U"0.01")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = PointProcess_to_TextGrid_vuv (me, GET_REAL (U"Maximum period"), GET_REAL (U"Mean period"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_PointProcess_to_TextTier) {
	LOOP {
		iam (PointProcess);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_to_Sound_phonation, U"PointProcess: To Sound (phonation)", U"PointProcess: To Sound (phonation)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Maximum period (s)", U"0.05")
	POSITIVE (U"Open phase", U"0.7")
	REAL (U"Collision phase", U"0.03")
	POSITIVE (U"Power 1", U"3.0")
	POSITIVE (U"Power 2", U"4.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_phonation (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Maximum period"),
			GET_REAL (U"Open phase"), GET_REAL (U"Collision phase"), GET_REAL (U"Power 1"), GET_REAL (U"Power 2"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_to_Sound_pulseTrain, U"PointProcess: To Sound (pulse train)", U"PointProcess: To Sound (pulse train)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Adaptation time (s)", U"0.05")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_pulseTrain (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Adaptation time"),
			GET_INTEGER (U"Interpolation depth"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_PointProcess_to_Sound_hum) {
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_hum (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW1_PointProcess_union) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_union (point1, point2);
	praat_new (thee.move(), U"union");
END }

FORM (NEW_PointProcess_upto_IntensityTier, U"PointProcess: Up to IntensityTier", U"PointProcess: Up to IntensityTier...") {
	POSITIVE (U"Intensity (dB)", U"70.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoIntensityTier thee = PointProcess_upto_IntensityTier (me, GET_REAL (U"Intensity"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_upto_PitchTier, U"PointProcess: Up to PitchTier", U"PointProcess: Up to PitchTier...") {
	POSITIVE (U"Frequency (Hz)", U"190.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_upto_PitchTier (me, GET_REAL (U"Frequency"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_PointProcess_upto_TextTier, U"PointProcess: Up to TextTier", U"PointProcess: Up to TextTier...") {
	SENTENCE (U"Text", U"")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextTier thee = PointProcess_upto_TextTier (me, GET_STRING (U"Text"));
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_PointProcess_voice, U"PointProcess: Fill unvoiced parts", nullptr) {
	POSITIVE (U"Period (s)", U"0.01")
	POSITIVE (U"Maximum voiced period (s)", U"0.02000000001")
	OK
DO
	LOOP {
		iam (PointProcess);
		try {
			PointProcess_voice (me, GET_REAL (U"Period"), GET_REAL (U"Maximum voiced period"));
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PointProcess may have partially changed
			throw;
		}
	}
END }

DIRECT (HINT_PointProcess_Sound_viewAndEdit) {
	Melder_information (U"To include a copy of a Sound in your PointProcess window:\n"
		"   select a PointProcess and a Sound, and click \"View & Edit\".");
END }

// MARK: - POINTPROCESS & SOUND

DIRECT (MODIFY_Point_Sound_transplantDomain) {
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	point -> xmin = sound -> xmin;
	point -> xmax = sound -> xmax;
	praat_dataChanged (point);
END }

FORM (REAL_Point_Sound_getShimmer_local, U"PointProcess & Sound: Get shimmer (local)", U"PointProcess & Sound: Get shimmer (local)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (REAL_Point_Sound_getShimmer_local_dB, U"PointProcess & Sound: Get shimmer (local, dB)", U"PointProcess & Sound: Get shimmer (local, dB)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local_dB (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (REAL_Point_Sound_getShimmer_apq3, U"PointProcess & Sound: Get shimmer (apq3)", U"PointProcess & Sound: Get shimmer (apq3)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq3 (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (REAL_Point_Sound_getShimmer_apq5, U"PointProcess & Sound: Get shimmer (apq)", U"PointProcess & Sound: Get shimmer (apq5)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq5 (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (REAL_Point_Sound_getShimmer_apq11, U"PointProcess & Sound: Get shimmer (apq11)", U"PointProcess & Sound: Get shimmer (apq11)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq11 (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (REAL_Point_Sound_getShimmer_dda, U"PointProcess & Sound: Get shimmer (dda)", U"PointProcess & Sound: Get shimmer (dda)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_dda (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor, GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END }

FORM (NEW1_PointProcess_Sound_to_AmplitudeTier_period, U"PointProcess & Sound: To AmplitudeTier (period)", nullptr) {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_period (point, sound, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor);
	praat_new (thee.move(), sound -> name, U"_", point -> name);
END }

DIRECT (NEW1_PointProcess_Sound_to_AmplitudeTier_point) {
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_point (point, sound);
	praat_new (thee.move(), sound -> name, U"_", point -> name);
END }

FORM (NEW1_PointProcess_Sound_to_Ltas, U"PointProcess & Sound: To Ltas", nullptr) {
	POSITIVE (U"Maximum frequency (Hz)", U"5000")
	POSITIVE (U"Band width (Hz)", U"100")
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas (point, sound,
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Band width"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
	praat_new (thee.move(), sound -> name);
END }

FORM (NEW1_PointProcess_Sound_to_Ltas_harmonics, U"PointProcess & Sound: To Ltas (harmonics", nullptr) {
	NATURAL (U"Maximum harmonic", U"20")
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas_harmonics (point, sound,
		GET_INTEGER (U"Maximum harmonic"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
	praat_new (thee.move(), sound -> name);
END }

FORM (NEW1_Sound_PointProcess_to_SoundEnsemble_correlate, U"Sound & PointProcess: To SoundEnsemble (correlate)", nullptr) {
	REAL (U"From time (s)", U"-0.1")
	REAL (U"To time (s)", U"1.0")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoSound thee = Sound_PointProcess_to_SoundEnsemble_correlate (sound, point, GET_REAL (U"From time"), GET_REAL (U"To time"));
	praat_new (thee.move(), point -> name);
END }

// MARK: - SPECTRUMTIER

DIRECT (NEW_SpectrumTier_downto_Table) {
	LOOP {
		iam (SpectrumTier);
		autoTable thee = SpectrumTier_downto_Table (me, true, true, true);
		praat_new (thee.move(), my name);
	}
END }

FORM (GRAPHICS_old_SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {   // 2010-10-19
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"10000.0")
	REAL (U"left Power range (dB)", U"20.0")
	REAL (U"right Power range (dB)", U"80.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"),
			GET_INTEGER (U"Garnish"), U"lines and speckles");
	}
END }

FORM (GRAPHICS_SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"10000.0")
	REAL (U"left Power range (dB)", U"20.0")
	REAL (U"right Power range (dB)", U"80.0")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_SpectrumTier_draw)
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"),
			GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END }

FORM (LIST_SpectrumTier_list, U"SpectrumTier: List", nullptr) {
	BOOLEAN (U"Include indexes", true)
	BOOLEAN (U"Include frequency", true)
	BOOLEAN (U"Include power density", true)
	OK
DO
	LOOP {
		iam (SpectrumTier);
		SpectrumTier_list (me, GET_INTEGER (U"Include indexes"), GET_INTEGER (U"Include frequency"), GET_INTEGER (U"Include power density"));
	}
END }

FORM (MODIFY_SpectrumTier_removePointsBelow, U"SpectrumTier: Remove points below", nullptr) {
	REAL (U"Remove all points below (dB)", U"40.0")
	OK
DO
	LOOP {
		iam (SpectrumTier);
		RealTier_removePointsBelow ((RealTier) me, GET_REAL (U"Remove all points below"));
		praat_dataChanged (me);
	}
END }

// MARK: - buttons

void praat_Tiers_init () {
	Thing_recognizeClassesByName (classPointProcess,
		classRealPoint, classRealTier,
		classPitchTier, classIntensityTier, classDurationTier, classAmplitudeTier,
		classSpectrumTier,
		classFormantPoint, classFormantTier, classFormantGrid,
		nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Tiers", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty PointProcess...", nullptr, 1, NEW1_PointProcess_createEmpty);
		praat_addMenuCommand (U"Objects", U"New", U"Create Poisson process...", nullptr, 1, NEW1_PointProcess_createPoissonProcess);
		praat_addMenuCommand (U"Objects", U"New", U"-- new tiers ---", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create PitchTier...", nullptr, 1, NEW1_PitchTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantGrid...", nullptr, 1, NEW1_FormantGrid_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantTier...", nullptr, praat_HIDDEN | praat_DEPTH_1, NEW1_FormantTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create IntensityTier...", nullptr, 1, NEW1_IntensityTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create DurationTier...", nullptr, 1, NEW1_DurationTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create AmplitudeTier...", nullptr, 1, NEW1_AmplitudeTier_create);

	praat_addAction1 (classAmplitudeTier, 0, U"AmplitudeTier help", nullptr, 0, HELP_AmplitudeTier_help);
	praat_addAction1 (classAmplitudeTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 0, U"View & Edit with Sound?", nullptr, 0, INFO_AmplitudeTier_Sound_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_local);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local_dB)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_local_dB);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq3)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq3);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq5)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq5);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq11)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq11);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (dda)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_dda);
	praat_addAction1 (classAmplitudeTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 0, U"Add point...", nullptr, 1, MODIFY_AmplitudeTier_addPoint);
		praat_addAction1 (classAmplitudeTier, 0, U"Formula...", nullptr, 1, MODIFY_AmplitudeTier_formula);
	praat_addAction1 (classAmplitudeTier, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classAmplitudeTier, 0, U"To Sound (pulse train)...", nullptr, 0, NEW_AmplitudeTier_to_Sound);
	praat_addAction1 (classAmplitudeTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classAmplitudeTier, 0, U"To IntensityTier...", nullptr, 0, NEW_AmplitudeTier_to_IntensityTier);
		praat_addAction1 (classAmplitudeTier, 0, U"Down to PointProcess", nullptr, 0, NEW_AmplitudeTier_downto_PointProcess);
		praat_addAction1 (classAmplitudeTier, 0, U"Down to TableOfReal", nullptr, 0, NEW_AmplitudeTier_downto_TableOfReal);

	praat_addAction1 (classDurationTier, 0, U"DurationTier help", nullptr, 0, HELP_DurationTier_help);
	praat_addAction1 (classDurationTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_DurationTier_edit);
	praat_addAction1 (classDurationTier, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_DurationTier_edit);
	praat_addAction1 (classDurationTier, 0, U"View & Edit with Sound?", nullptr, 0, INFO_DurationTier_Sound_edit);
	praat_addAction1 (classDurationTier, 0, U"& Manipulation: Replace?", nullptr, 0, INFO_DurationTier_Manipulation_replace);
	praat_addAction1 (classDurationTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classDurationTier);
		praat_addAction1 (classDurationTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classDurationTier, 1, U"Get value at time...", nullptr, 1, REAL_DurationTier_getValueAtTime);
		praat_addAction1 (classDurationTier, 1, U"Get value at index...", nullptr, 1, REAL_DurationTier_getValueAtIndex);
		praat_addAction1 (classDurationTier, 1, U"Get target duration...", nullptr, 1, REAL_DurationTier_getTargetDuration);
	praat_addAction1 (classDurationTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classDurationTier);
		praat_addAction1 (classDurationTier, 0, U"Add point...", nullptr, 1, MODIFY_DurationTier_addPoint);
		praat_addAction1 (classDurationTier, 0, U"Formula...", nullptr, 1, MODIFY_DurationTier_formula);
	praat_addAction1 (classDurationTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classDurationTier, 0, U"Down to PointProcess", nullptr, 0, NEW_DurationTier_downto_PointProcess);

	praat_addAction1 (classFormantGrid, 0, U"FormantGrid help", nullptr, 0, HELP_FormantGrid_help);
	praat_addAction1 (classFormantGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormantGrid);
		praat_addAction1 (classFormantGrid, 0, U"Formula (frequencies)...", nullptr, 1, MODIFY_FormantGrid_formula_frequencies);
		//praat_addAction1 (classFormantGrid, 0, U"Formula (bandwidths)...", nullptr, 1, MODIFY_FormantGrid_formula_bandwidths);
		praat_addAction1 (classFormantGrid, 0, U"Add formant point...", nullptr, 1, MODIFY_FormantGrid_addFormantPoint);
		praat_addAction1 (classFormantGrid, 0, U"Add bandwidth point...", nullptr, 1, MODIFY_FormantGrid_addBandwidthPoint);
		praat_addAction1 (classFormantGrid, 0, U"Remove formant points between...", nullptr, 1, MODIFY_FormantGrid_removeFormantPointsBetween);
		praat_addAction1 (classFormantGrid, 0, U"Remove bandwidth points between...", nullptr, 1, MODIFY_FormantGrid_removeBandwidthPointsBetween);
	praat_addAction1 (classFormantGrid, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantGrid, 0, U"To Formant...", nullptr, 1, NEW_FormantGrid_to_Formant);

	praat_addAction1 (classFormantTier, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantTier, 0, U"Speckle...", nullptr, 1, GRAPHICS_FormantTier_speckle);
	praat_addAction1 (classFormantTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classFormantTier);
		praat_addAction1 (classFormantTier, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classFormantTier, 1, U"Get value at time...", nullptr, 1, REAL_FormantTier_getValueAtTime);
		praat_addAction1 (classFormantTier, 1, U"Get bandwidth at time...", nullptr, 1, REAL_FormantTier_getBandwidthAtTime);
	praat_addAction1 (classFormantTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classFormantTier);
		praat_addAction1 (classFormantTier, 0, U"Add point...", nullptr, 1, MODIFY_FormantTier_addPoint);
	praat_addAction1 (classFormantTier, 0, U"Down", nullptr, 0, nullptr);
		praat_addAction1 (classFormantTier, 0, U"Down to TableOfReal...", nullptr, 0, NEW_FormantTier_downto_TableOfReal);

	praat_addAction1 (classIntensityTier, 0, U"IntensityTier help", nullptr, 0, HELP_IntensityTier_help);
	praat_addAction1 (classIntensityTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction1 (classIntensityTier, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction1 (classIntensityTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_IntensityTier_Sound_viewAndEdit);
	praat_addAction1 (classIntensityTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensityTier, 1, U"Get value at time...", nullptr, 1, REAL_IntensityTier_getValueAtTime);
		praat_addAction1 (classIntensityTier, 1, U"Get value at index...", nullptr, 1, REAL_IntensityTier_getValueAtIndex);
	praat_addAction1 (classIntensityTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 0, U"Add point...", nullptr, 1, MODIFY_IntensityTier_addPoint);
		praat_addAction1 (classIntensityTier, 0, U"Formula...", nullptr, 1, MODIFY_IntensityTier_formula);
	praat_addAction1 (classIntensityTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classIntensityTier, 0, U"To AmplitudeTier", nullptr, 0, NEW_IntensityTier_to_AmplitudeTier);
		praat_addAction1 (classIntensityTier, 0, U"Down to PointProcess", nullptr, 0, NEW_IntensityTier_downto_PointProcess);
		praat_addAction1 (classIntensityTier, 0, U"Down to TableOfReal", nullptr, 0, NEW_IntensityTier_downto_TableOfReal);

	praat_addAction1 (classPitchTier, 1, U"Save as PitchTier spreadsheet file...", nullptr, 0, SAVE_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1,   U"Write to PitchTier spreadsheet file...", U"*Save as PitchTier spreadsheet file...", praat_DEPRECATED_2011, SAVE_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, U"Save as headerless spreadsheet file...", nullptr, 0, SAVE_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1,   U"Write to headerless spreadsheet file...", U"*Save as headerless spreadsheet file...", praat_DEPRECATED_2011, SAVE_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 0, U"PitchTier help", nullptr, 0, HELP_PitchTier_help);
	praat_addAction1 (classPitchTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PitchTier_viewAndEdit);
	praat_addAction1 (classPitchTier, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PitchTier_viewAndEdit);
	praat_addAction1 (classPitchTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_PitchTier_Sound_viewAndEdit);
	praat_addAction1 (classPitchTier, 0, U"Play pulses", nullptr, 0, PLAY_PitchTier_play);
	praat_addAction1 (classPitchTier, 0, U"Hum", nullptr, 0, PLAY_PitchTier_hum);
	praat_addAction1 (classPitchTier, 0, U"Play sine", nullptr, 0, PLAY_PitchTier_playSine);
	praat_addAction1 (classPitchTier, 0, U"Draw...", nullptr, 0, GRAPHICS_PitchTier_draw);
	praat_addAction1 (classPitchTier, 0, U"& Manipulation: Replace?", nullptr, 0, INFO_PitchTier_Manipulation_replace);
	praat_addAction1 (classPitchTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classPitchTier);
		praat_addAction1 (classPitchTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get value at time...", nullptr, 1, REAL_PitchTier_getValueAtTime);
		praat_addAction1 (classPitchTier, 1, U"Get value at index...", nullptr, 1, REAL_PitchTier_getValueAtIndex);
		praat_addAction1 (classPitchTier, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get mean (curve)...", nullptr, 1, REAL_PitchTier_getMean_curve);
		praat_addAction1 (classPitchTier, 1, U"Get mean (points)...", nullptr, 1, REAL_PitchTier_getMean_points);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (curve)...", nullptr, 1, REAL_PitchTier_getStandardDeviation_curve);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (points)...", nullptr, 1, REAL_PitchTier_getStandardDeviation_points);
	praat_addAction1 (classPitchTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classPitchTier);
		praat_addAction1 (classPitchTier, 0, U"Add point...", nullptr, 1, MODIFY_PitchTier_addPoint);
		praat_addAction1 (classPitchTier, 0, U"Formula...", nullptr, 1, MODIFY_PitchTier_formula);
		praat_addAction1 (classPitchTier, 0, U"-- stylize --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Stylize...", nullptr, 1, MODIFY_PitchTier_stylize);
		praat_addAction1 (classPitchTier, 0, U"Interpolate quadratically...", nullptr, 1, MODIFY_PitchTier_interpolateQuadratically);
		praat_addAction1 (classPitchTier, 0, U"-- modify frequencies --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Shift frequencies...", nullptr, 1, MODIFY_PitchTier_shiftFrequencies);
		praat_addAction1 (classPitchTier, 0, U"Multiply frequencies...", nullptr, 1, MODIFY_PitchTier_multiplyFrequencies);
	praat_addAction1 (classPitchTier, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"To PointProcess", nullptr, 1, NEW_PitchTier_to_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"To Sound (pulse train)...", nullptr, 1, NEW_PitchTier_to_Sound_pulseTrain);
		praat_addAction1 (classPitchTier, 0, U"To Sound (phonation)...", nullptr, 1, NEW_PitchTier_to_Sound_phonation);
		praat_addAction1 (classPitchTier, 0, U"To Sound (sine)...", nullptr, 1, NEW_PitchTier_to_Sound_sine);
	praat_addAction1 (classPitchTier, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Down to PointProcess", nullptr, 1, NEW_PitchTier_downto_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"Down to TableOfReal...", nullptr, 1, NEW_PitchTier_downto_TableOfReal);

	praat_addAction1 (classPointProcess, 0, U"PointProcess help", nullptr, 0, HELP_PointProcess_help);
	praat_addAction1 (classPointProcess, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 1,   U"View & Edit alone", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 1,   U"Edit alone", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 0, U"View & Edit with Sound?", nullptr, praat_NO_API, HINT_PointProcess_Sound_viewAndEdit);
	praat_addAction1 (classPointProcess, 0, U"Play -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Play as pulse train", nullptr, 1, PLAY_PointProcess_play);
		praat_addAction1 (classPointProcess, 0, U"Hum", nullptr, 1, PLAY_PointProcess_hum);
		praat_addAction1 (classPointProcess, 0, U"-- to sound --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To Sound (pulse train)...", nullptr, 1, NEW_PointProcess_to_Sound_pulseTrain);
		praat_addAction1 (classPointProcess, 0, U"To Sound (phonation)...", nullptr, 1, NEW_PointProcess_to_Sound_phonation);
		praat_addAction1 (classPointProcess, 0, U"To Sound (hum)", nullptr, 1, NEW_PointProcess_to_Sound_hum);
	praat_addAction1 (classPointProcess, 0, U"Draw...", nullptr, 0, GRAPHICS_PointProcess_draw);
	praat_addAction1 (classPointProcess, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classPointProcess);
		praat_addAction1 (classPointProcess, 1, U"-- script get --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of points", nullptr, 1, INTEGER_PointProcess_getNumberOfPoints);
		praat_addAction1 (classPointProcess, 1, U"Get low index...", nullptr, 1, INTEGER_PointProcess_getLowIndex);
		praat_addAction1 (classPointProcess, 1, U"Get high index...", nullptr, 1, INTEGER_PointProcess_getHighIndex);
		praat_addAction1 (classPointProcess, 1, U"Get nearest index...", nullptr, 1, INTEGER_PointProcess_getNearestIndex);
		praat_addAction1 (classPointProcess, 1, U"Get time from index...", nullptr, 1, REAL_PointProcess_getTimeFromIndex);
		praat_addAction1 (classPointProcess, 1, U"Get interval...", nullptr, 1, REAL_PointProcess_getInterval);
		praat_addAction1 (classPointProcess, 1, U"-- periods --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of periods...", nullptr, 1, INTEGER_PointProcess_getNumberOfPeriods);
		praat_addAction1 (classPointProcess, 1, U"Get mean period...", nullptr, 1, REAL_PointProcess_getMeanPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get stdev period...", nullptr, 1, REAL_PointProcess_getStdevPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local)...", nullptr, 1, REAL_PointProcess_getJitter_local);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local, absolute)...", nullptr, 1, REAL_PointProcess_getJitter_local_absolute);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (rap)...", nullptr, 1, REAL_PointProcess_getJitter_rap);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ppq5)...", nullptr, 1, REAL_PointProcess_getJitter_ppq5);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ddp)...", nullptr, 1, REAL_PointProcess_getJitter_ddp);
	praat_addAction1 (classPointProcess, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPointProcess);
		praat_addAction1 (classPointProcess, 0, U"Add point...", nullptr, 1, MODIFY_PointProcess_addPoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point...", nullptr, 1, MODIFY_PointProcess_removePoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point near...", nullptr, 1, MODIFY_PointProcess_removePointNear);
		praat_addAction1 (classPointProcess, 0, U"Remove points...", nullptr, 1, MODIFY_PointProcess_removePoints);
		praat_addAction1 (classPointProcess, 0, U"Remove points between...", nullptr, 1, MODIFY_PointProcess_removePointsBetween);
		praat_addAction1 (classPointProcess, 0, U"-- voice --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Fill...", nullptr, 1, MODIFY_PointProcess_fill);
		praat_addAction1 (classPointProcess, 0, U"Voice...", nullptr, 1, MODIFY_PointProcess_voice);
	praat_addAction1 (classPointProcess, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid...", nullptr, 1, NEW_PointProcess_to_TextGrid);
		praat_addAction1 (classPointProcess, 0, U"-- to single tier --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextTier", nullptr, 1, NEW_PointProcess_to_TextTier);
		praat_addAction1 (classPointProcess, 0, U"To IntervalTier", nullptr, 1, NEW_PointProcess_to_IntervalTier);
	praat_addAction1 (classPointProcess, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To PitchTier...", nullptr, 1, NEW_PointProcess_to_PitchTier);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid (vuv)...", nullptr, 1, NEW_PointProcess_to_TextGrid_vuv);
	praat_addAction1 (classPointProcess, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classPointProcess, 0, U"To Matrix", nullptr, 2, NEW_PointProcess_to_Matrix);
			praat_addAction1 (classPointProcess, 0, U"Up to TextTier...", nullptr, 2, NEW_PointProcess_upto_TextTier);
			praat_addAction1 (classPointProcess, 0, U"Up to PitchTier...", nullptr, 2, NEW_PointProcess_upto_PitchTier);
			praat_addAction1 (classPointProcess, 0, U"Up to IntensityTier...", nullptr, 2, NEW_PointProcess_upto_IntensityTier);
	praat_addAction1 (classPointProcess, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 2, U"Union", nullptr, 1, NEW1_PointProcess_union);
		praat_addAction1 (classPointProcess, 2, U"Intersection", nullptr, 1, NEW1_PointProcess_intersection);
		praat_addAction1 (classPointProcess, 2, U"Difference", nullptr, 1, NEW1_PointProcess_difference);

	praat_addAction1 (classSpectrumTier, 0, U"Draw...", nullptr, 0, GRAPHICS_SpectrumTier_draw);
	praat_addAction1 (classSpectrumTier, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrumTier, 1, U"List...", nullptr, 1, LIST_SpectrumTier_list);
		praat_addAction1 (classSpectrumTier, 0, U"Down to Table", nullptr, 1, NEW_SpectrumTier_downto_Table);
	praat_addAction1 (classSpectrumTier, 0, U"Remove points below...", nullptr, 0, MODIFY_SpectrumTier_removePointsBelow);

	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"Multiply", nullptr, 0, NEW1_Sound_AmplitudeTier_multiply);
	praat_addAction2 (classDurationTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_DurationTier_edit);
	praat_addAction2 (classDurationTier, 1, classSound, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_DurationTier_edit);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter", nullptr, 0, NEW1_Sound_FormantGrid_filter);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, NEW1_Sound_FormantGrid_filter_noscale);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter", nullptr, 0, NEW1_Sound_FormantTier_filter);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, NEW1_Sound_FormantTier_filter_noscale);
	praat_addAction2 (classIntensityTier, 1, classPointProcess, 1, U"To IntensityTier", nullptr, 0, NEW1_IntensityTier_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"Multiply...", nullptr, 0, NEW1_Sound_IntensityTier_multiply);
	praat_addAction2 (classIntensityTier, 1, classSound, 1,   U"Multiply", U"*Multiply...", praat_DEPRECATED_2005, NEW1_Sound_IntensityTier_multiply_old);
	praat_addAction2 (classPitchTier, 1, classPointProcess, 1, U"To PitchTier", nullptr, 0, NEW1_PitchTier_PointProcess_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PitchTier_viewAndEdit);
	praat_addAction2 (classPitchTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PitchTier_viewAndEdit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PointProcess_viewAndEdit);
	praat_addAction2 (classPointProcess, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Query", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local)...", nullptr, 0, REAL_Point_Sound_getShimmer_local);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local_dB)...", nullptr, 0, REAL_Point_Sound_getShimmer_local_dB);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq3)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq3);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq5)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq5);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq11)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq11);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (dda)...", nullptr, 0, REAL_Point_Sound_getShimmer_dda);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Modify", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Transplant time domain", nullptr, 0, MODIFY_Point_Sound_transplantDomain);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Analyse", nullptr, 0, nullptr);
	/*praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Manipulation", nullptr, 0, NEW1_Sound_PointProcess_to_Manipulation);*/
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (point)", nullptr, 0, NEW1_PointProcess_Sound_to_AmplitudeTier_point);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (period)...", nullptr, 0, NEW1_PointProcess_Sound_to_AmplitudeTier_period);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas...", nullptr, 0, NEW1_PointProcess_Sound_to_Ltas);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas (only harmonics)...", nullptr, 0, NEW1_PointProcess_Sound_to_Ltas_harmonics);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Sound ensemble...", nullptr, 0, NEW1_Sound_PointProcess_to_SoundEnsemble_correlate);
}

/* End of file praat_Tiers.cpp */
