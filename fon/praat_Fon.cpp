/* praat_Fon.cpp
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

#include "AmplitudeTier.h"
#include "AmplitudeTierEditor.h"
#include "Cochleagram_and_Excitation.h"
#include "Corpus.h"
#include "Distributions_and_Strings.h"
#include "Distributions_and_Transition.h"
#include "DurationTierEditor.h"
#include "Excitation_to_Formant.h"
#include "FormantGrid.h"
#include "FormantGridEditor.h"
#include "FormantTier.h"
#include "Harmonicity.h"
#include "IntensityTier.h"
#include "IntensityTierEditor.h"
#include "LongSound.h"
#include "Ltas_to_SpectrumTier.h"
#include "ManipulationEditor.h"
#include "Matrix_and_Pitch.h"
#include "Matrix_and_PointProcess.h"
#include "Matrix_and_Polygon.h"
#include "Matrix_extensions.h"
#include "Movie.h"
#include "ParamCurve.h"
#include "Pitch_Intensity.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "Pitch_to_Sound.h"
#include "PitchEditor.h"
#include "PitchTier_to_PointProcess.h"
#include "PitchTier_to_Sound.h"
#include "PitchTierEditor.h"
#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "Praat_tests.h"
#include "Sound_and_Spectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_PointProcess.h"
#include "SpectrogramEditor.h"
#include "Spectrum_and_Spectrogram.h"
#include "Spectrum_to_Excitation.h"
#include "Spectrum_to_Formant.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "Strings_extensions.h"
#include "StringsEditor.h"
#include "TableEditor.h"
#include "TextGrid.h"
#include "VocalTract.h"
#include "VoiceAnalysis.h"
#include "WordList.h"

#include "praat_TimeTier.h"
#include "praat_TimeFrameSampled.h"
#include "praat_TimeVector.h"
#include "praat_Sound.h"
#include "praat_Matrix.h"
#include "praat_Tiers.h"
#include "praat_ExperimentMFC.h"
#include "praat_uvafon.h"

#undef iam
#define iam iam_LOOP

static const char32 *STRING_FROM_FREQUENCY_HZ = U"left Frequency range (Hz)";
static const char32 *STRING_TO_FREQUENCY_HZ = U"right Frequency range (Hz)";

// MARK: - COCHLEAGRAM

// MARK: Help

DIRECT (HELP_Cochleagram_help) {
	Melder_help (U"Cochleagram");
END }

// MARK: Movie

DIRECT (MOVIE_Cochleagram_movie) {
	MOVIE_ONE (Cochleagram, U"Cochleagram movie", 300, 300)
		Matrix_movie (me, graphics);
	MOVIE_ONE_END
}

// MARK: Info

FORM (REAL_Cochleagram_difference, U"Cochleagram difference", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	Cochleagram coch1 = nullptr, coch2 = nullptr;
	LOOP (coch1 ? coch2 : coch1) = (Cochleagram) OBJECT;
	Melder_informationReal (Cochleagram_difference (coch1, coch2, fromTime, toTime), U"Hertz (root-mean-square)");
END }

// MARK: Draw

FORM (GRAPHICS_Cochleagram_paint, U"Paint Cochleagram", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (Cochleagram);
		autoPraatPicture picture;
		Cochleagram_paint (me, GRAPHICS, fromTime, toTime, GET_INTEGER (U"Garnish"));
	}
END }

// MARK: Modify

FORM (MODIFY_Cochleagram_formula, U"Cochleagram Formula", U"Cochleagram: Formula...") {
	LABEL (U"label", U"`x' is time in seconds, `y' is place in Bark")
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Cochleagram);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (U"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Cochleagram may have partially changed
			throw;
		}
	}
END }

// MARK: Analyse

FORM (NEW_Cochleagram_to_Excitation, U"From Cochleagram to Excitation", nullptr) {
	REAL (U"Time (s)", U"0.0")
	OK
DO
	LOOP {
		iam (Cochleagram);
		autoExcitation thee = Cochleagram_to_Excitation (me, GET_REAL (U"Time"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: Hack

DIRECT (NEW_Cochleagram_to_Matrix) {
	LOOP {
		iam (Cochleagram);
		autoMatrix thee = Cochleagram_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - CORPUS

// MARK: New

FORM (NEW1_Corpus_create, U"Create Corpus", U"Create Corpus...") {
	WORD (U"Name", U"myCorpus")
	LABEL (U"", U"Folder with sound files:")
	TEXTFIELD (U"folderWithSoundFiles", U"")
	LABEL (U"", U"Folder with annotation files:")
	TEXTFIELD (U"folderWithAnnotationFiles", U"")
	OK
DO
END }

// MARK: View & Edit

DIRECT (WINDOW_Corpus_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a Corpus from batch.");
	LOOP {
		iam (Corpus);
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: - DISTRIBUTIONS

FORM (NEW_Distributions_to_Transition, U"To Transition", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK
DO
	LOOP {
		iam (Distributions);
		autoTransition thee = Distributions_to_Transition (me, nullptr, GET_INTEGER (U"Environment"), nullptr, GET_INTEGER (U"Greedy"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW1_Distributions_to_Transition_adj, U"To Transition", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK
DO
	Distributions dist = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (dist, nullptr, GET_INTEGER (U"Environment"), trans, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END }

FORM (NEW1_Distributions_to_Transition_noise, U"To Transition (noise)", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK
DO
	Distributions underlying = nullptr, surface = nullptr;
	LOOP (underlying ? surface : underlying) = (Distributions) OBJECT;
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (U"Environment"), nullptr, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END }

FORM (NEW1_Distributions_to_Transition_noise_adj, U"To Transition (noise)", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK
DO
	Distributions underlying = nullptr, surface = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) (underlying ? surface : underlying) = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (U"Environment"), trans, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END }

// MARK: - DISTRIBUTIONS & TRANSITION

DIRECT (NEW1_Distributions_Transition_map) {
	Distributions dist = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoDistributions thee = Distributions_Transition_map (dist, trans);
	praat_new (thee.move(), U"surface");
END }

// MARK: - EXCITATION

// MARK: Help

DIRECT (HELP_Excitation_help) {
	Melder_help (U"Excitation");
END }

// MARK: Draw

FORM (GRAPHICS_Excitation_draw, U"Draw Excitation", nullptr) {
	REAL (U"From frequency (Bark)", U"0")
	REAL (U"To frequency (Bark)", U"25.6")
	REAL (U"Minimum (phon)", U"0")
	REAL (U"Maximum (phon)", U"100")
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (Excitation);
		autoPraatPicture picture;
		Excitation_draw (me, GRAPHICS,
			GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END }

// MARK: Query

DIRECT (REAL_Excitation_getLoudness) {
	LOOP {
		iam (Excitation);
		double loudness = Excitation_getLoudness (me);
		Melder_informationReal (loudness, U"sones");
	}
END }

// MARK: Modify

FORM (MODIFY_Excitation_formula, U"Excitation Formula", U"Excitation: Formula...") {
	LABEL (U"label", U"`x' is the place in Bark, `col' is the bin number")
	LABEL (U"label", U"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Excitation);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END }

// MARK: Convert

FORM (NEW_Excitation_to_Formant, U"From Excitation to Formant", 0) {
	NATURAL (U"Maximum number of formants", U"20")
	OK
DO
	LOOP {
		iam (Excitation);
		autoFormant thee = Excitation_to_Formant (me, GET_INTEGER (U"Maximum number of formants"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Excitation_to_Matrix) {
	LOOP {
		iam (Excitation);
		autoMatrix thee = Excitation_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - FORMANT

// MARK: Help

DIRECT (HELP_Formant_help) {
	HELP (U"Formant")
}

// MARK: Draw

FORM (GRAPHICS_Formant_drawSpeckles, U"Draw Formant", U"Formant: Draw speckles...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	REALVAR (dynamicRange, U"Dynamic range (dB)", U"30.0")
	BOOLEANVAR (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_drawSpeckles (me, GRAPHICS, fromTime, toTime, maximumFrequency, dynamicRange, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Formant_drawTracks, U"Draw formant tracks", U"Formant: Draw tracks...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	BOOLEANVAR (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Formant_scatterPlot, U"Formant: Scatter plot", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (U"Horizontal formant number", U"2")
	REAL (U"left Horizontal range (Hz)", U"3000.0")
	REAL (U"right Horizontal range (Hz)", U"400.0")
	NATURAL (U"Vertical formant number", U"1")
	REAL (U"left Vertical range (Hz)", U"1500.0")
	REAL (U"right Vertical range (Hz)", U"100.0")
	POSITIVE (U"Mark size (mm)", U"1.0")
	BOOLEAN (U"Garnish", true)
	SENTENCE (U"Mark string (+xo.)", U"+")
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_scatterPlot (me, GRAPHICS, fromTime, toTime,
			GET_INTEGER (U"Horizontal formant number"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_INTEGER (U"Vertical formant number"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Mark size"), GET_STRING (U"Mark string"), GET_INTEGER (U"Garnish"));
	GRAPHICS_EACH_END
}

// MARK: Tabulate

FORM (LIST_Formant_list, U"Formant: List", 0) {
	BOOLEAN (U"Include frame number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include intensity", false)
	NATURAL (U"Intensity decimals", U"3")
	BOOLEAN (U"Include number of formants", true)
	NATURAL (U"Frequency decimals", U"3")
	BOOLEAN (U"Include bandwidths", true)
	OK
DO
	LOOP {
		iam (Formant);
		Formant_list (me, GET_INTEGER (U"Include frame number"),
			GET_INTEGER (U"Include time"), GET_INTEGER (U"Time decimals"),
			GET_INTEGER (U"Include intensity"), GET_INTEGER (U"Intensity decimals"),
			GET_INTEGER (U"Include number of formants"), GET_INTEGER (U"Frequency decimals"),
			GET_INTEGER (U"Include bandwidths"));
		break;
	}
END }

FORM (NEW_Formant_downto_Table, U"Formant: Down to Table", nullptr) {
	BOOLEAN (U"Include frame number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include intensity", false)
	NATURAL (U"Intensity decimals", U"3")
	BOOLEAN (U"Include number of formants", true)
	NATURAL (U"Frequency decimals", U"3")
	BOOLEAN (U"Include bandwidths", true)
	OK
DO
	LOOP {
		iam (Formant);
		autoTable thee = Formant_downto_Table (me, GET_INTEGER (U"Include frame number"),
			GET_INTEGER (U"Include time"), GET_INTEGER (U"Time decimals"),
			GET_INTEGER (U"Include intensity"), GET_INTEGER (U"Intensity decimals"),
			GET_INTEGER (U"Include number of formants"), GET_INTEGER (U"Frequency decimals"),
			GET_INTEGER (U"Include bandwidths"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: Query

FORM (REAL_Formant_getBandwidthAtTime, U"Formant: Get bandwidth", U"Formant: Get bandwidth at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Linear")
	OK
DO
	LOOP {
		iam (Formant);
		double bandwidth = Formant_getBandwidthAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (bandwidth, GET_STRING (U"Unit"));
		break;
	}
END }
	
FORM (REAL_Formant_getMaximum, U"Formant: Get maximum", U"Formant: Get maximum...") {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double maximum = Formant_getMaximum (me, GET_INTEGER (U"Formant number"),
			fromTime, toTime, GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, GET_STRING (U"Unit"));
		break;
	}
END }

DIRECT (INTEGER_Formant_getMaximumNumberOfFormants) {
	LOOP {
		iam (Formant);
		long maximumNumberOfFormants = Formant_getMaxNumFormants (me);
		Melder_information (maximumNumberOfFormants, U" (there are at most this many formants in every frame)");
		break;
	}
END }

FORM (REAL_Formant_getMean, U"Formant: Get mean", U"Formant: Get mean...") {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	OK
DO
	LOOP {
		iam (Formant);
		double mean = Formant_getMean (me, GET_INTEGER (U"Formant number"),
			fromTime, toTime, GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (mean, GET_STRING (U"Unit"));
		break;
	}
END }

FORM (REAL_Formant_getMinimum, U"Formant: Get minimum", U"Formant: Get minimum...") {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double minimum = Formant_getMinimum (me, GET_INTEGER (U"Formant number"),
			fromTime, toTime, GET_INTEGER (U"Unit") - 1,
			GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, GET_STRING (U"Unit"));
		break;
	}
END }

DIRECT (INTEGER_Formant_getMinimumNumberOfFormants) {
	LOOP {
		iam (Formant);
		long minimumNumberOfFormants = Formant_getMinNumFormants (me);
		Melder_information (minimumNumberOfFormants, U" (there are at least this many formants in every frame)");
		break;
	}
END }

FORM (INTEGER_Formant_getNumberOfFormants, U"Formant: Get number of formants", U"Formant: Get number of formants...") {
	NATURAL (U"Frame number", U"1")
	OK
DO
	LOOP {
		iam (Formant);
		long frame = GET_INTEGER (U"Frame number");
		if (frame > my nx) Melder_throw (U"There is no frame ", frame, U" in a Formant with only ", my nx, U" frames.");
		Melder_information (my d_frames [frame]. nFormants, U" formants");
		break;
	}
END }

FORM (REAL_Formant_getQuantile, U"Formant: Get quantile", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (U"Quantile", U"0.50 (= median)")
	OK
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantile (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"Quantile"), fromTime, toTime, GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (U"Unit"));
		break;
	}
END }

FORM (REAL_Formant_getQuantileOfBandwidth, U"Formant: Get quantile of bandwidth", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (U"Quantile", U"0.50 (= median)")
	OK
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantileOfBandwidth (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"Quantile"), fromTime, toTime, GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (U"Unit"));
		break;
	}
END }

FORM (REAL_Formant_getStandardDeviation, U"Formant: Get standard deviation", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	OK
DO
	LOOP {
		iam (Formant);
		double stdev = Formant_getStandardDeviation (me, GET_INTEGER (U"Formant number"),
			fromTime, toTime, GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (stdev, GET_STRING (U"Unit"));
		break;
	}
END }

FORM (REAL_Formant_getTimeOfMaximum, U"Formant: Get time of maximum", U"Formant: Get time of maximum...") {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMaximum (me, GET_INTEGER (U"Formant number"), fromTime, toTime,
			GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
		break;
	}
END }

FORM (REAL_Formant_getTimeOfMinimum, U"Formant: Get time of minimum", U"Formant: Get time of minimum...") {
	NATURAL (U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMinimum (me, GET_INTEGER (U"Formant number"), fromTime, toTime,
			GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
		break;
	}
END }

FORM (REAL_Formant_getValueAtTime, U"Formant: Get value", U"Formant: Get value at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Linear")
	OK
DO
	LOOP {
		iam (Formant);
		double value = Formant_getValueAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (value, GET_STRING (U"Unit"));
		break;
	}
END }
	
// MARK: Modify

DIRECT (MODIFY_Formant_sort) {
	LOOP {
		iam (Formant);
		Formant_sort (me);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Formant_formula_frequencies, U"Formant: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	LABEL (U"", U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	LOOP {
		iam (Formant);
		try {
			Formant_formula_frequencies (me, GET_STRING (U"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Formant may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_Formant_formula_bandwidths, U"Formant: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	LABEL (U"", U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	TEXTFIELD (U"formula", U"self / 2 ; sharpen all peaks")
	OK
DO
	LOOP {
		iam (Formant);
		try {
			Formant_formula_bandwidths (me, GET_STRING (U"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Formant may have partially changed
			throw;
		}
	}
END }

// MARK: Convert

FORM (NEW_Formant_tracker, U"Formant tracker", U"Formant: Track...") {
	NATURAL (U"Number of tracks (1-5)", U"3")
	REAL (U"Reference F1 (Hz)", U"550")
	REAL (U"Reference F2 (Hz)", U"1650")
	REAL (U"Reference F3 (Hz)", U"2750")
	REAL (U"Reference F4 (Hz)", U"3850")
	REAL (U"Reference F5 (Hz)", U"4950")
	REAL (U"Frequency cost (/kHz)", U"1.0")
	REAL (U"Bandwidth cost", U"1.0")
	REAL (U"Transition cost (/octave)", U"1.0")
	OK
DO
	long numberOfTracks = GET_INTEGER (U"Number of tracks");
	if (numberOfTracks > 5) Melder_throw (U"Number of tracks cannot be more than 5.");
	LOOP {
		iam (Formant);
		autoFormant thee = Formant_tracker (me, GET_INTEGER (U"Number of tracks"),
			GET_REAL (U"Reference F1"), GET_REAL (U"Reference F2"),
			GET_REAL (U"Reference F3"), GET_REAL (U"Reference F4"),
			GET_REAL (U"Reference F5"), GET_REAL (U"Frequency cost"),
			GET_REAL (U"Bandwidth cost"), GET_REAL (U"Transition cost"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Formant_downto_FormantGrid) {
	LOOP {
		iam (Formant);
		autoFormantGrid thee = Formant_downto_FormantGrid (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Formant_downto_FormantTier) {
	LOOP {
		iam (Formant);
		autoFormantTier thee = Formant_downto_FormantTier (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Formant_to_Matrix, U"From Formant to Matrix", nullptr) {
	INTEGER (U"Formant", U"1")
	OK
DO
	LOOP {
		iam (Formant);
		autoMatrix thee = Formant_to_Matrix (me, GET_INTEGER (U"Formant"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: - FORMANT & POINTPROCESS

DIRECT (NEW1_Formant_PointProcess_to_FormantTier) {
	Formant formant = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (formant && point) break;
	}
	autoFormantTier thee = Formant_PointProcess_to_FormantTier (formant, point);
	praat_new (thee.move(), formant -> name, U"_", point -> name);
END }

// MARK: - FORMANT & SOUND

DIRECT (NEW1_Sound_Formant_filter) {
	Sound sound = nullptr;
	Formant formant = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter (sound, formant);
	praat_new (thee.move(), sound -> name, U"_filt");
END }

DIRECT (NEW1_Sound_Formant_filter_noscale) {
	Sound sound = nullptr;
	Formant formant = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter_noscale (sound, formant);
	praat_new (thee.move(), sound -> name, U"_filt");
END }

// MARK: - HARMONICITY

// MARK: Help

DIRECT (HELP_Harmonicity_help) {
	Melder_help (U"Harmonicity");
END }

// MARK: Draw

FORM (GRAPHICS_Harmonicity_draw, U"Draw harmonicity", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (minimum, U"Minimum", U"0.0")
	REALVAR (maximum, U"Maximum", U"0.0 (= auto)")
	OK
DO
	LOOP {
		iam (Harmonicity);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS, fromTime, toTime, 0.0, 0.0, minimum, maximum);
	}
END }

// MARK: Query

FORM (REAL_Harmonicity_getMaximum, U"Harmonicity: Get maximum", U"Harmonicity: Get maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getMaximum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (maximum, U"dB");
	}
END }

FORM (REAL_Harmonicity_getMean, U"Harmonicity: Get mean", U"Harmonicity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double mean = Harmonicity_getMean (me, fromTime, toTime);
		Melder_informationReal (mean, U"dB");
	}
END }

FORM (REAL_Harmonicity_getMinimum, U"Harmonicity: Get minimum", U"Harmonicity: Get minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getMinimum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (minimum, U"dB");
	}
END }

FORM (REAL_Harmonicity_getStandardDeviation, U"Harmonicity: Get standard deviation", U"Harmonicity: Get standard deviation...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double stdev = Harmonicity_getStandardDeviation (me, fromTime, toTime);
		Melder_informationReal (stdev, U"dB");
	}
END }

FORM (REAL_Harmonicity_getTimeOfMaximum, U"Harmonicity: Get time of maximum", U"Harmonicity: Get time of maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getXOfMaximum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (maximum, U"seconds");
	}
END }

FORM (REAL_Harmonicity_getTimeOfMinimum, U"Harmonicity: Get time of minimum", U"Harmonicity: Get time of minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getXOfMinimum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (minimum, U"seconds");
	}
END }

FORM (REAL_Harmonicity_getValueAtTime, U"Harmonicity: Get value", U"Harmonicity: Get value at time...") {
	praat_TimeVector_INTERPOLATED_VALUE (time, interpolation)
	OK
DO
	LOOP {
		iam (Harmonicity);
		double value = Vector_getValueAtX (me, time, 1, interpolation - 1);
		Melder_informationReal (value, U"dB");
	}
END }
	
FORM (REAL_Harmonicity_getValueInFrame, U"Get value in frame", U"Harmonicity: Get value in frame...") {
	INTEGER (U"Frame number", U"10")
	OK
DO
	LOOP {
		iam (Harmonicity);
		long frameNumber = GET_INTEGER (U"Frame number");
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], U"dB");
	}
END }

// MARK: Modify

FORM (MODIFY_Harmonicity_formula, U"Harmonicity Formula", U"Harmonicity: Formula...") {
	LABEL (U"label", U"x is time")
	LABEL (U"label", U"for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Harmonicity);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Harmonicity may have partically changed
			throw;
		}
	}
END }

// MARK: Convert

DIRECT (NEW_Harmonicity_to_Matrix) {
	LOOP {
		iam (Harmonicity);
		autoMatrix thee = Harmonicity_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - INTENSITY

// MARK: Draw

FORM (GRAPHICS_Intensity_draw, U"Draw Intensity", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (U"Minimum (dB)", U"0.0")
	REAL (U"Maximum (dB)", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", true)
	OK
DO
	LOOP {
		iam (Intensity);
		autoPraatPicture picture;
		Intensity_draw (me, GRAPHICS, fromTime, toTime,
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END }

DIRECT (NEW_Intensity_downto_IntensityTier) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_downto_IntensityTier (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Intensity_downto_Matrix) {
	LOOP {
		iam (Intensity);
		autoMatrix thee = Intensity_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_Intensity_formula, U"Intensity Formula", 0) {
	LABEL (U"label", U"`x' is the time in seconds, `col' is the frame number, `self' is in dB")
	LABEL (U"label", U"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK
DO
	LOOP {
		iam (Intensity);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Intensity may have partially changed
			throw;
		}
	}
END }

FORM (REAL_Intensity_getMaximum, U"Intensity: Get maximum", U"Intensity: Get maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Intensity);
		double maximum = Vector_getMaximum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (maximum, U"dB");
		break;   // OPTIMIZE
	}
END }

FORM (REAL_old_Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, fromTime, toTime, 0, 0, true);
		Melder_informationReal (mean, U"dB");
		break;   // OPTIMIZE
	}
END }

FORM (REAL_Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK
DO_ALTERNATIVE (REAL_old_Intensity_getMean)
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, fromTime, toTime,
			0, GET_INTEGER (U"Averaging method"), true);
		Melder_informationReal (mean, U"dB");
		break;   // OPTIMIZE
	}
END }

FORM (REAL_Intensity_getMinimum, U"Intensity: Get minimum", U"Intensity: Get minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Intensity);
		double minimum = Vector_getMinimum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (minimum, U"dB");
		break;   // OPTIMIZE
	}
END }

FORM (REAL_Intensity_getQuantile, U"Intensity: Get quantile", 0) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (U"Quantile (0-1)", U"0.50")
	OK
DO
	LOOP {
		iam (Intensity);
		double quantile = Intensity_getQuantile (me, fromTime, toTime, GET_REAL (U"Quantile"));
		Melder_informationReal (quantile, U"dB");
	}
END }

FORM (REAL_Intensity_getStandardDeviation, U"Intensity: Get standard deviation", U"Intensity: Get standard deviation...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	LOOP {
		iam (Intensity);
		double stdev = Vector_getStandardDeviation (me, fromTime, toTime, 1);
		Melder_informationReal (stdev, U"dB");
	}
END }

FORM (REAL_Intensity_getTimeOfMaximum, U"Intensity: Get time of maximum", U"Intensity: Get time of maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMaximum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (time, U"seconds");
	}
END }

FORM (REAL_Intensity_getTimeOfMinimum, U"Intensity: Get time of minimum", U"Intensity: Get time of minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMinimum (me, fromTime, toTime, interpolation - 1);
		Melder_informationReal (time, U"seconds");
	}
END }

FORM (REAL_Intensity_getValueAtTime, U"Intensity: Get value", U"Intensity: Get value at time...") {
	praat_TimeVector_INTERPOLATED_VALUE (time, interpolation)
	OK
DO
	LOOP {
		iam (Intensity);
		double value = Vector_getValueAtX (me, time, 1, interpolation - 1);
		Melder_informationReal (value, U"dB");
	}
END }
	
FORM (REAL_Intensity_getValueInFrame, U"Get value in frame", U"Intensity: Get value in frame...") {
	INTEGERVAR (frameNumber, U"Frame number", U"10")
	OK
DO
	LOOP {
		iam (Intensity);
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], U"dB");
	}
END }

DIRECT (HELP_Intensity_help) {
	Melder_help (U"Intensity");
END }

DIRECT (NEW_Intensity_to_IntensityTier_peaks) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Intensity_to_IntensityTier_valleys) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_valleys (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - INTENSITY & PITCH

FORM (GRAPHICS_Pitch_Intensity_draw, U"Plot intensity by pitch", nullptr) {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= auto)")
	REAL (U"From intensity (dB)", U"0.0")
	REAL (U"To intensity (dB)", U"100.0")
	BOOLEAN (U"Garnish", true)
	RADIO (U"Drawing method", 1)
		RADIOBUTTON (U"Speckles")
		RADIOBUTTON (U"Curve")
		RADIOBUTTON (U"Speckles and curve")
	OK
DO
	Pitch pitch = nullptr;
	Intensity intensity = nullptr;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	autoPraatPicture picture;
	Pitch_Intensity_draw (pitch, intensity, GRAPHICS,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		GET_REAL (U"From intensity"), GET_REAL (U"To intensity"), GET_INTEGER (U"Garnish"), GET_INTEGER (U"Drawing method"));
END }

FORM (GRAPHICS_Pitch_Intensity_speckle, U"Plot intensity by pitch", nullptr) {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= auto)")
	REAL (U"From intensity (dB)", U"0.0")
	REAL (U"To intensity (dB)", U"100.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	Pitch pitch = nullptr;
	Intensity intensity = nullptr;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	autoPraatPicture picture;
	Pitch_Intensity_draw (pitch, intensity, GRAPHICS,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		GET_REAL (U"From intensity"), GET_REAL (U"To intensity"), GET_INTEGER (U"Garnish"), 1);
END }

DIRECT (REAL_Pitch_Intensity_getMean) {
	Pitch pitch = nullptr;
	Intensity intensity = nullptr;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	double value = Pitch_Intensity_getMean (pitch, intensity);
	Melder_informationReal (value, U"dB");
END }

DIRECT (REAL_Pitch_Intensity_getMeanAbsoluteSlope) {
	Pitch pitch = nullptr;
	Intensity intensity = nullptr;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	double slope = Pitch_Intensity_getMeanAbsoluteSlope (pitch, intensity);
	Melder_informationReal (slope, U"dB/second");
END }

// MARK: - INTENSITY & POINTPROCESS

DIRECT (NEW1_Intensity_PointProcess_to_IntensityTier) {
	Intensity intensity = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = Intensity_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.move(), intensity -> name);
END }

// MARK: - INTERVALTIER, the remainder is in praat_TextGrid_init.cpp

FORM_READ (READ1_IntervalTier_readFromXwaves, U"Read IntervalTier from Xwaves", 0, true) {
	autoIntervalTier me = IntervalTier_readFromXwaves (file);
	praat_newWithFile (me.move(), file, MelderFile_name (file));
END }

// MARK: - LTAS

DIRECT (NEW1_Ltases_average) {
	autoLtasBag ltases = LtasBag_create ();
	LOOP {
		iam (Ltas);
		ltases -> addItem_ref (me);
	}
	autoLtas result = Ltases_average (ltases.get());
	praat_new (result.move(), U"averaged");
END }

FORM (NEW_Ltas_computeTrendLine, U"Ltas: Compute trend line", U"Ltas: Compute trend line...") {
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"600.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"4000.0")
	OK
DO
	CONVERT_EACH (Ltas)
		autoLtas result = Ltas_computeTrendLine (me, fromFrequency, toFrequency);
	CONVERT_EACH_END (my name, U"_trend")
}

FORM (GRAPHICS_old_Ltas_draw, U"Ltas: Draw", nullptr) {
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REALVAR (fromPower, U"left Power range (dB/Hz)", U"-20.0")
	REALVAR (toPower, U"right Power range (dB/Hz)", U"80.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Ltas)
		Ltas_draw (me, GRAPHICS, fromFrequency, toFrequency, fromPower, toPower, garnish, U"Bars");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Ltas_draw, U"Ltas: Draw", nullptr) {
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REALVAR (fromPower, U"left Power range (dB/Hz)", U"-20.0")
	REALVAR (toPower, U"right Power range (dB/Hz)", U"80.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENUSTRVAR (drawingMethod, U"Drawing method", 2)
		OPTION (U"Curve")
		OPTION (U"Bars")
		OPTION (U"Poles")
		OPTION (U"Speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_Ltas_draw)
	GRAPHICS_EACH (Ltas)
		Ltas_draw (me, GRAPHICS, fromFrequency, toFrequency, fromPower, toPower, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

FORM (MODIFY_Ltas_formula, U"Ltas Formula", nullptr) {
	LABEL (U"label", U"`x' is the frequency in hertz, `col' is the bin number")
	LABEL (U"label", U"x := x1;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK
DO
	LOOP {
		iam (Ltas);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Ltas may have partially changed
			throw;
		}
	}
END }

FORM (REAL_Ltas_getBinNumberFromFrequency, U"Ltas: Get band from frequency", U"Ltas: Get band from frequency...") {
	REAL (U"Frequency (Hz)", U"2000.0")
	OK
DO
	Ltas me = FIRST (Ltas);
	double binNumber = Sampled_xToIndex (me, GET_REAL (U"Frequency"));
	Melder_informationReal (binNumber, nullptr);
END }

DIRECT (REAL_Ltas_getBinWidth) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my dx, U"hertz");
END }

FORM (REAL_Ltas_getFrequencyFromBinNumber, U"Ltas: Get frequency from bin number", U"Ltas: Get frequency from bin number...") {
	NATURAL (U"Bin number", U"1")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Sampled_indexToX (me, GET_INTEGER (U"Bin number"));
	Melder_informationReal (frequency, U"hertz");
END }

FORM (REAL_Ltas_getFrequencyOfMaximum, U"Ltas: Get frequency of maximum", U"Ltas: Get frequency of maximum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMaximum (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (frequency, U"hertz");
END }

FORM (REAL_Ltas_getFrequencyOfMinimum, U"Ltas: Get frequency of minimum", U"Ltas: Get frequency of minimum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMinimum (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (frequency, U"hertz");
END }

DIRECT (REAL_Ltas_getHighestFrequency) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmax, U"Hz");
END }

FORM (REAL_Ltas_getLocalPeakHeight, U"Ltas: Get local peak height", nullptr) {
	REALVAR (environmentMin, U"left Environment (Hz)", U"1700.0")
	REALVAR (environmentMax, U"right Environment (Hz)", U"4200.0")
	REALVAR (peakMin, U"left Peak (Hz)", U"2400.0")
	REALVAR (peakMax, U"right Peak (Hz)", U"3200.0")
	RADIOVAR (averagingMethod, U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	if (environmentMin >= peakMin) Melder_throw (U"The beginning of the environment must lie before the peak.");
	if (peakMin >= peakMax) Melder_throw (U"The end of the peak must lie after its beginning.");
	if (environmentMax <= peakMax) Melder_throw (U"The end of the environment must lie after the peak.");
	double localPeakHeight = Ltas_getLocalPeakHeight (me, environmentMin, environmentMax, peakMin, peakMax, averagingMethod);
	Melder_informationReal (localPeakHeight, U"dB");
END }

DIRECT (REAL_Ltas_getLowestFrequency) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmin, U"hertz");
END }

FORM (REAL_Ltas_getMaximum, U"Ltas: Get maximum", U"Ltas: Get maximum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double maximum = Vector_getMaximum (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (maximum, U"dB");
END }

FORM (REAL_Ltas_getMean, U"Ltas: Get mean", U"Ltas: Get mean...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double mean = Sampled_getMean_standardUnit (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		0,
		GET_INTEGER (U"Averaging method"),
		false);
	Melder_informationReal (mean, U"dB");
END }

FORM (REAL_Ltas_getMinimum, U"Ltas: Get minimum", U"Ltas: Get minimum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double minimum = Vector_getMinimum (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (minimum, U"dB");
END }

DIRECT (INTEGER_Ltas_getNumberOfBins) {
	Ltas me = FIRST (Ltas);
	long numberOfBins = my nx;
	Melder_information (numberOfBins, U" bins");
END }

FORM (REAL_Ltas_getSlope, U"Ltas: Get slope", 0) {
	REAL (U"left Low band (Hz)", U"0.0")
	REAL (U"right Low band (Hz)", U"1000.0")
	REAL (U"left High band (Hz)", U"1000.0")
	REAL (U"right High band (Hz)", U"4000.0")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double slope = Ltas_getSlope (me,
		GET_REAL (U"left Low band"),
		GET_REAL (U"right Low band"),
		GET_REAL (U"left High band"),
		GET_REAL (U"right High band"),
		GET_INTEGER (U"Averaging method"));
	Melder_informationReal (slope, U"dB");
END }

FORM (REAL_Ltas_getStandardDeviation, U"Ltas: Get standard deviation", U"Ltas: Get standard deviation...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double stdev = Sampled_getStandardDeviation_standardUnit (me,
		GET_REAL (U"From frequency"),
		GET_REAL (U"To frequency"),
		0,   // level (irrelevant)
		GET_INTEGER (U"Averaging method"),
		false);   // interpolate (don't)
	Melder_informationReal (stdev, U"dB");
END }

FORM (REAL_Ltas_getValueAtFrequency, U"Ltas: Get value", U"Ltas: Get value at frequency...") {
	REAL (U"Frequency (Hz)", U"1500")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double value = Vector_getValueAtX (me,
		GET_REAL (U"Frequency"),
		1,   // level
		GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (value, U"dB");
END }
	
FORM (REAL_Ltas_getValueInBin, U"Get value in bin", U"Ltas: Get value in bin...") {
	INTEGER (U"Bin number", U"100")
	OK
DO
	Ltas me = FIRST (Ltas);
	long binNumber = GET_INTEGER (U"Bin number");
	double value = binNumber < 1 || binNumber > my nx ? NUMundefined : my z [1] [binNumber];
	Melder_informationReal (value, U"dB");
END }

DIRECT (HELP_Ltas_help) {
	Melder_help (U"Ltas");
END }

DIRECT (NEW1_Ltases_merge) {
	autoLtasBag ltases = LtasBag_create ();
	LOOP {
		iam (Ltas);
		ltases -> addItem_ref (me);
	}
	autoLtas thee = Ltases_merge (ltases.get());
	praat_new (thee.move(), U"merged");
END }

FORM (NEW_Ltas_subtractTrendLine, U"Ltas: Subtract trend line", U"Ltas: Subtract trend line...") {
	REAL (U"left Frequency range (Hz)", U"600.0")
	POSITIVE (U"right Frequency range (Hz)", U"4000.0")
	OK
DO
	LOOP {
		iam (Ltas);
		autoLtas result = Ltas_subtractTrendLine (me,
			GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"));
		praat_new (result.move(), my name, U"_fit");
	}
END }

DIRECT (NEW_Ltas_to_Matrix) {
	LOOP {
		iam (Ltas);
		autoMatrix result = Ltas_to_Matrix (me);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW_Ltas_to_SpectrumTier_peaks) {
	LOOP {
		iam (Ltas);
		autoSpectrumTier thee = Ltas_to_SpectrumTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - MANIPULATION

static void cb_ManipulationEditor_publication (Editor /* editor */, autoDaata publication) {
	try {
		praat_new (publication.move(), U"fromManipulationEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_Manipulation_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Manipulation from batch.");
	LOOP {
		iam (Manipulation);
		autoManipulationEditor editor = ManipulationEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_ManipulationEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

DIRECT (NEW_Manipulation_extractDurationTier) {
	LOOP {
		iam (Manipulation);
		if (! my duration) Melder_throw (me, U": I don't contain a DurationTier.");
		autoDurationTier thee = Data_copy (my duration.get());
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Manipulation_extractOriginalSound) {
	LOOP {
		iam (Manipulation);
		if (! my sound) Melder_throw (me, U": I don't contain a Sound.");
		autoSound thee = Data_copy (my sound.get());
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Manipulation_extractPitchTier) {
	LOOP {
		iam (Manipulation);
		if (! my pitch) Melder_throw (me, U": I don't contain a PitchTier.");
		autoPitchTier thee = Data_copy (my pitch.get());
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Manipulation_extractPulses) {
	LOOP {
		iam (Manipulation);
		if (! my pulses) Melder_throw (me, U": I don't contain a PointProcess.");
		autoPointProcess thee = Data_copy (my pulses.get());
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Manipulation_getResynthesis_lpc) {
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_PITCH_LPC);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Manipulation_getResynthesis_overlapAdd) {
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_OVERLAPADD);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (HELP_Manipulation_help) {
	Melder_help (U"Manipulation");
END }

DIRECT (PLAY_Manipulation_play_lpc) {
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_PITCH_LPC);
	}
END }

DIRECT (PLAY_Manipulation_play_overlapAdd) {
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_OVERLAPADD);
	}
END }

DIRECT (MODIFY_Manipulation_removeDuration) {
	LOOP {
		iam (Manipulation);
		my duration = autoDurationTier();
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Manipulation_removeOriginalSound) {
	LOOP {
		iam (Manipulation);
		my sound = autoSound();
		praat_dataChanged (me);
	}
END }

FORM_SAVE (SAVE_Manipulation_writeToBinaryFileWithoutSound, U"Binary file without Sound", nullptr, nullptr) {
	Manipulation_writeToBinaryFileWithoutSound (FIRST_ANY (Manipulation), file);
END }

FORM_SAVE (SAVE_Manipulation_writeToTextFileWithoutSound, U"Text file without Sound", nullptr, nullptr) {
	Manipulation_writeToTextFileWithoutSound (FIRST_ANY (Manipulation), file);
END }

// MARK: - MANIPULATION & DURATIONTIER

DIRECT (MODIFY_Manipulation_replaceDurationTier) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceDurationTier (me, FIRST (DurationTier));
	praat_dataChanged (me);
END }

DIRECT (HELP_Manipulation_replaceDurationTier_help) {
	Melder_help (U"Manipulation: Replace duration tier");
END }

// MARK: - MANIPULATION & PITCHTIER

DIRECT (MODIFY_Manipulation_replacePitchTier) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePitchTier (me, FIRST (PitchTier));
	praat_dataChanged (me);
END }

DIRECT (HELP_Manipulation_replacePitchTier_help) {
	Melder_help (U"Manipulation: Replace pitch tier");
END }

// MARK: - MANIPULATION & POINTPROCESS

DIRECT (MODIFY_Manipulation_replacePulses) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePulses (me, FIRST (PointProcess));
	praat_dataChanged (me);
END }

// MARK: - MANIPULATION & SOUND

DIRECT (MODIFY_Manipulation_replaceOriginalSound) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceOriginalSound (me, FIRST (Sound));
	praat_dataChanged (me);
END }

// MARK: - MANIPULATION & TEXTTIER

DIRECT (NEW1_Manipulation_TextTier_to_Manipulation) {
	Manipulation me = FIRST (Manipulation);
	TextTier thee = FIRST (TextTier);
	autoManipulation him = Manipulation_AnyTier_to_Manipulation (me, reinterpret_cast <AnyTier> (thee));
	praat_new (him.move(), my name);
END }

// MARK: - PARAMCURVE

FORM (GRAPHICS_ParamCurve_draw, U"Draw parametrized curve", nullptr) {
	REAL (U"Tmin", U"0.0")
	REAL (U"Tmax", U"0.0")
	REAL (U"Step", U"0.0")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ParamCurve)
		ParamCurve_draw (me, GRAPHICS,
			GET_REAL (U"Tmin"), GET_REAL (U"Tmax"), GET_REAL (U"Step"),
			GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"),
			GET_INTEGER (U"Garnish"));
	GRAPHICS_EACH_END
}

DIRECT (HELP_ParamCurve_help) {
	Melder_help (U"ParamCurve");
END }

// MARK: - PITCH

DIRECT (INTEGER_Pitch_getNumberOfVoicedFrames) {
	Pitch me = FIRST (Pitch);
	Melder_information (Pitch_countVoicedFrames (me), U" voiced frames");
END }

DIRECT (INFO_Pitch_difference) {
	Pitch pit1 = nullptr, pit2 = nullptr;
	LOOP (pit1 ? pit2 : pit1) = (Pitch) OBJECT;
	Pitch_difference (pit1, pit2);
END }

FORM (GRAPHICS_Pitch_draw, U"Pitch: Draw", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVEVAR (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawErb, U"Pitch: Draw erb", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (ERB)", U"0")
	REALVAR (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_ERB);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawLogarithmic, U"Pitch: Draw logarithmic", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVEVAR (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVEVAR (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawMel, U"Pitch: Draw mel", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_MEL);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones100, U"Pitch: Draw semitones (re 100 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 100 Hz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones200, U"Pitch: Draw semitones (re 200 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 200 Hz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-24.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"18.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_200);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones440, U"Pitch: Draw semitones (re 440 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 440 Hz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-36.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"6.0")
	BOOLEAN (U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_440);
	GRAPHICS_EACH_END
}

DIRECT (WINDOW_Pitch_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Pitch from batch.");
	LOOP {
		iam (Pitch);
		autoPitchEditor editor = PitchEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

FORM (MODIFY_Pitch_formula, U"Pitch: Formula", U"Formula...") {
	LABEL (U"", U"x = time; col = frame; row = candidate (1 = current path); frequency (time, candidate) :=")
	TEXTFIELD (U"formula", U"self*2; Example: octave jump up")
	OK
DO
	LOOP {
		iam (Pitch);
		try {
			Pitch_formula (me, GET_STRING (U"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Pitch may have partially changed
			throw;
		}
	}
END }

FORM (REAL_Pitch_getMinimum, U"Pitch: Get minimum", 0) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	RADIOVARx (interpolation, U"Interpolation", 2, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	REAL_ONE (Pitch)
		double result = Pitch_getMinimum (me, fromTime, toTime, unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0))
}

FORM (REAL_Pitch_getMaximum, U"Pitch: Get maximum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	RADIOVARx (interpolation, U"Interpolation", 2, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	REAL_ONE (Pitch)
		double result = Pitch_getMaximum (me, fromTime, toTime, unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0))
}

FORM (REAL_Pitch_getMean, U"Pitch: Get mean", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	OK
DO
	REAL_ONE (Pitch)
		double result = Pitch_getMean (me, fromTime, toTime, unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
}

FORM (REAL_Pitch_getMeanAbsoluteSlope, U"Pitch: Get mean absolute slope", 0) {
	RADIOVAR (unit, U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Mel")
		RADIOBUTTON (U"Semitones")
		RADIOBUTTON (U"ERB")
	OK
DO
	FIND_ONE (Pitch)
		double slope;
		long nVoiced = (unit == 1 ? Pitch_getMeanAbsSlope_hertz : unit == 2 ? Pitch_getMeanAbsSlope_mel : unit == 3 ? Pitch_getMeanAbsSlope_semitones : Pitch_getMeanAbsSlope_erb)
			(me, & slope);
		if (nVoiced < 2) {
			Melder_information (U"--undefined--");
		} else {
			Melder_information (slope, U" ", GET_STRING (U"Unit"), U"/s");
		}
	END
}

DIRECT (REAL_Pitch_getMeanAbsSlope_noOctave) {
	REAL_ONE (Pitch)
		double result;
		(void) Pitch_getMeanAbsSlope_noOctave (me, & result);
	REAL_ONE_END (U"Semitones/s")
}

FORM (REAL_Pitch_getQuantile, U"Pitch: Get quantile", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (quantile, U"Quantile", U"0.50 (= median)")
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	OK
DO
	REAL_ONE (Pitch)
		double result = Sampled_getQuantile (me, fromTime, toTime, quantile, Pitch_LEVEL_FREQUENCY, unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0))
}

FORM (REAL_Pitch_getStandardDeviation, U"Pitch: Get standard deviation", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
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
	REAL_ONE (Pitch)
		double result = Pitch_getStandardDeviation (me, fromTime, toTime, unit);
		const char32 *unitText =
			unit == kPitch_unit_HERTZ ? U"Hz" :
			unit == kPitch_unit_MEL ? U"mel" :
			unit == kPitch_unit_LOG_HERTZ ? U"logHz" :
			unit == kPitch_unit_SEMITONES_1 ? U"semitones" :
			U"ERB";
	REAL_ONE_END (unitText)
}

FORM (REAL_Pitch_getTimeOfMaximum, U"Pitch: Get time of maximum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	RADIOVARx (interpolation, U"Interpolation", 2, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	REAL_ONE (Pitch)
		double result = Pitch_getTimeOfMaximum (me, fromTime, toTime, unit, interpolation);
	REAL_ONE_END (U"seconds")
}

FORM (REAL_Pitch_getTimeOfMinimum, U"Pitch: Get time of minimum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	RADIOVARx (interpolation, U"Interpolation", 2, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK
DO
	REAL_ONE (Pitch)
		double result = Pitch_getTimeOfMinimum (me, fromTime, toTime, unit, interpolation);
	REAL_ONE_END (U"seconds")
}

FORM (REAL_Pitch_getValueAtTime, U"Pitch: Get value at time", U"Pitch: Get value at time...") {
	REALVAR (time, U"Time (s)", U"0.5")
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	RADIOVARx (interpolation, U"Interpolation", 2, 0)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
	OK
DO
	REAL_ONE (Pitch)
		double result = Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0))
}
	
FORM (REAL_Pitch_getValueInFrame, U"Pitch: Get value in frame", U"Pitch: Get value in frame...") {
	INTEGERVAR (frameNumber, U"Frame number", U"10")
	OPTIONMENU_ENUMVAR (unit, U"Unit", kPitch_unit, DEFAULT)
	OK
DO
	REAL_ONE (Pitch)
		double result = Sampled_getValueAtSample (me, frameNumber, Pitch_LEVEL_FREQUENCY, unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, unit);
	REAL_ONE_END (Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
}

DIRECT (HELP_Pitch_help) {
	HELP (U"Pitch")
}

DIRECT (PLAY_Pitch_hum) {
	LOOP {
		iam (Pitch);
		Pitch_hum (me, 0.0, 0.0);
	}
END }

DIRECT (NEW_Pitch_interpolate) {
	CONVERT_EACH (Pitch)
		autoPitch result = Pitch_interpolate (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_killOctaveJumps) {
	CONVERT_EACH (Pitch)
		autoPitch result = Pitch_killOctaveJumps (me);
	CONVERT_EACH_END (my name)
}

DIRECT (PLAY_Pitch_play) {
	LOOP {
		iam (Pitch);
		Pitch_play (me, 0.0, 0.0);
	}
END }

FORM (NEW_Pitch_smooth, U"Pitch: Smooth", U"Pitch: Smooth...") {
	REALVAR (bandwidth, U"Bandwidth (Hz)", U"10.0")
	OK
DO
	CONVERT_EACH (Pitch)
		autoPitch result = Pitch_smooth (me, bandwidth);
	CONVERT_EACH_END (my name)
}

FORM (GRAPHICS_Pitch_speckle, U"Pitch: Speckle", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVEVAR (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency should be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_HERTZ);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleErb, U"Pitch: Speckle erb", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_ERB);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleLogarithmic, U"Pitch: Speckle logarithmic", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVEVAR (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVEVAR (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleMel, U"Pitch: Speckle mel", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_MEL);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones100, U"Pitch: Speckle semitones (re 100 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones200, U"Pitch: Speckle semitones (re 200 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 200 hertz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-24.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"18.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_SEMITONES_200);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones440, U"Pitch: Speckle semitones (re 440 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"", U"Range in semitones re 440 hertz:")
	REALVAR (fromFrequency, U"left Frequency range (st)", U"-36.0")
	REALVAR (toFrequency, U"right Frequency range (st)", U"6.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit_SEMITONES_440);
	GRAPHICS_EACH_END
}

FORM (NEW_Pitch_subtractLinearFit, U"Pitch: subtract linear fit", nullptr) {
	RADIOVARx (unit, U"Unit", 1, 0)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Hertz (logarithmic)")
		RADIOBUTTON (U"Mel")
		RADIOBUTTON (U"Semitones")
		RADIOBUTTON (U"ERB")
	OK
DO
	CONVERT_EACH (Pitch)
		autoPitch result = Pitch_subtractLinearFit (me, unit);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_IntervalTier) {
	CONVERT_EACH (Pitch)
		autoIntervalTier result = IntervalTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_Matrix) {
	CONVERT_EACH (Pitch)
		autoMatrix result = Pitch_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_PitchTier) {
	CONVERT_EACH (Pitch)
		autoPitchTier result = Pitch_to_PitchTier (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_PointProcess) {
	CONVERT_EACH (Pitch)
		autoPointProcess result = Pitch_to_PointProcess (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_Sound_pulses) {
	CONVERT_EACH (Pitch)
		autoSound result = Pitch_to_Sound (me, 0.0, 0.0, false);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_Sound_hum) {
	CONVERT_EACH (Pitch)
		autoSound result = Pitch_to_Sound (me, 0.0, 0.0, true);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Pitch_to_Sound_sine, U"Pitch: To Sound (sine)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	RADIO (U"Cut voiceless stretches", 2)
		OPTION (U"exactly")
		OPTION (U"at nearest zero crossings")
	OK
DO
	CONVERT_EACH (Pitch)
		autoSound result = Pitch_to_Sound_sine (me, 0.0, 0.0, GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Cut voiceless stretches") - 1);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Pitch_to_TextGrid, U"To TextGrid...", U"Pitch: To TextGrid...") {
	SENTENCEVAR (tierNames, U"Tier names", U"Mary John bell")
	SENTENCEVAR (pointTiers, U"Point tiers", U"bell")
	OK
DO
	CONVERT_EACH (Pitch)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, tierNames, pointTiers);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_Pitch_to_TextTier) {
	CONVERT_EACH (Pitch)
		autoTextTier result = TextTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name)
}

// MARK: - PITCH & PITCHTIER

FORM (GRAPHICS_old_PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"From frequency (Hz)", U"0.0")
	REALVAR (toFrequency, U"To frequency (Hz)", U"500.0")
	RADIOVARx (lineTypeForNonperiodicIntervals, U"Line type for non-periodic intervals", 2, 0)
		RADIOBUTTON (U"Normal")
		RADIOBUTTON (U"Dotted")
		RADIOBUTTON (U"Blank")
	BOOLEANVAR (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_TWO (PitchTier, Pitch)
		PitchTier_Pitch_draw (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			lineTypeForNonperiodicIntervals, garnish, U"lines and speckles");
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"From frequency (Hz)", U"0.0")
	REALVAR (toFrequency, U"To frequency (Hz)", U"500.0")
	RADIOVARx (lineTypeForNonperiodicIntervals, U"Line type for non-periodic intervals", 2, 0)
		RADIOBUTTON (U"Normal")
		RADIOBUTTON (U"Dotted")
		RADIOBUTTON (U"Blank")
	BOOLEANVAR (garnish, U"Garnish", 1)
	LABEL (U"", U"")
	OPTIONMENUSTRVAR (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_PitchTier_Pitch_draw)
	GRAPHICS_TWO (PitchTier, Pitch)
		PitchTier_Pitch_draw (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			lineTypeForNonperiodicIntervals, garnish, drawingMethod);
	GRAPHICS_TWO_END
}

DIRECT (NEW1_Pitch_PitchTier_to_Pitch) {
	CONVERT_TWO (Pitch, PitchTier)
		autoPitch result = Pitch_PitchTier_to_Pitch (me, you);
	CONVERT_TWO_END (my name, U"_stylized");
}

// MARK: - PITCH & POINTPROCESS

DIRECT (NEW1_Pitch_PointProcess_to_PitchTier) {
	CONVERT_TWO (Pitch, PointProcess)
		autoPitchTier result = Pitch_PointProcess_to_PitchTier (me, you);
	CONVERT_TWO_END (my name);
}

// MARK: - PITCH & SOUND

DIRECT (NEW1_Sound_Pitch_to_Manipulation) {
	CONVERT_TWO (Sound, Pitch)
		autoManipulation result = Sound_Pitch_to_Manipulation (me, you);
	CONVERT_TWO_END (your name);
}

DIRECT (NEW1_Sound_Pitch_to_PointProcess_cc) {
	CONVERT_TWO (Sound, Pitch)
		autoPointProcess result = Sound_Pitch_to_PointProcess_cc (me, you);
	CONVERT_TWO_END (my name, U"_", your name);
}

FORM (NEW1_Sound_Pitch_to_PointProcess_peaks, U"Sound & Pitch: To PointProcess (peaks)", 0) {
	BOOLEANVAR (includeMaxima, U"Include maxima", 1)
	BOOLEANVAR (includeMinima, U"Include minima", 0)
	OK
DO
	CONVERT_TWO (Sound, Pitch)
		autoPointProcess result = Sound_Pitch_to_PointProcess_peaks (me, you, includeMaxima, includeMinima);
	CONVERT_TWO_END (my name, U"_", your name)
}

// MARK: - POLYGON

FORM (GRAPHICS_Polygon_draw, U"Polygon: Draw", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_draw (me, GRAPHICS, xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_drawCircles, U"Polygon: Draw circles", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	POSITIVEVAR (diameter, U"Diameter (mm)", U"3.0")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_drawCircles (me, GRAPHICS, xmin, xmax, ymin, ymax, diameter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_drawClosed, U"Polygon: Draw", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_drawClosed (me, GRAPHICS, xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygons_drawConnection, U"Polygons: Draw connection", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	BOOLEANVAR (arrow, U"Arrow", false)
	POSITIVEVAR (relativeLength, U"Relative length", U"0.9")
	OK
DO
	GRAPHICS_COUPLE (Polygon)
		Polygons_drawConnection (me, you, GRAPHICS, xmin, xmax, ymin, ymax, arrow, relativeLength);
	GRAPHICS_COUPLE_END
}

DIRECT (HELP_Polygon_help) {
	HELP (U"Polygon")
}

FORM (GRAPHICS_Polygon_paint, U"Polygon: Paint", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_paint (me, GRAPHICS, GET_COLOUR (U"Colour"), xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_paintCircles, U"Polygon: Paint circles", nullptr) {
	REALVAR (xmin, U"Xmin", U"0.0")
	REALVAR (xmax, U"Xmax", U"0.0 (= all)")
	REALVAR (ymin, U"Ymin", U"0.0")
	REALVAR (ymax, U"Ymax", U"0.0 (= all)")
	POSITIVEVAR (diameter, U"Diameter (mm)", U"3.0")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_paintCircles (me, GRAPHICS, xmin, xmax, ymin, ymax, diameter);
	GRAPHICS_EACH_END
}

DIRECT (MODIFY_Polygon_randomize) {
	MODIFY_EACH (Polygon)
		Polygon_randomize (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_salesperson, U"Polygon: Find shortest path", nullptr) {
	NATURALVAR (numberOfIterations, U"Number of iterations", U"1")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_salesperson (me, numberOfIterations);
	MODIFY_EACH_END
}

DIRECT (NEW_Polygon_to_Matrix) {
	CONVERT_EACH (Polygon)
		autoMatrix result = Polygon_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

// MARK: - SOUND & PITCH & POINTPROCESS

FORM (INFO_Sound_Pitch_PointProcess_voiceReport, U"Voice report", U"Voice") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE4 (fromPitch, U"left Pitch range (Hz)", U"75.0")
	POSITIVE4 (toPitch, U"right Pitch range (Hz)", U"600.0")
	POSITIVE4 (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	REAL4 (silenceThreshold, U"Silence threshold", U"0.03")
	REAL4 (voicingThreshold, U"Voicing threshold", U"0.45")
	OK
DO
	INFO_THREE (Sound, Pitch, PointProcess)
		Sound_Pitch_PointProcess_voiceReport (me, you, him, fromTime, toTime, fromPitch, toPitch,
			maximumPeriodFactor, maximumAmplitudeFactor, silenceThreshold, voicingThreshold);
	INFO_THREE_END
}

// MARK: - SOUND & POINTPROCESS & PITCHTIER & DURATIONTIER

FORM (NEW1_Sound_Point_Pitch_Duration_to_Sound, U"To Sound", nullptr) {
	POSITIVE4 (longestPeriod, U"Longest period (s)", U"0.02")
	OK
DO
	CONVERT_FOUR (Sound, PointProcess, PitchTier, DurationTier)
		autoSound result = Sound_Point_Pitch_Duration_to_Sound (me, you, him, she, longestPeriod);
	CONVERT_FOUR_END (U"manip");
}

// MARK: - SPECTROGRAM

FORM (GRAPHICS_Spectrogram_paint, U"Spectrogram: Paint", U"Spectrogram: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (U"Maximum (dB/Hz)", U"100.0")
	BOOLEAN (U"Autoscaling", 1)
	POSITIVE (U"Dynamic range (dB)", U"50.0")
	REAL (U"Pre-emphasis (dB/oct)", U"6.0")
	REAL (U"Dynamic compression (0-1)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoPraatPicture picture;
		Spectrogram_paint (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			GET_REAL (U"Maximum"), GET_INTEGER (U"Autoscaling"),
			GET_REAL (U"Dynamic range"), GET_REAL (U"Pre-emphasis"),
			GET_REAL (U"Dynamic compression"), GET_INTEGER (U"Garnish"));
	}
END }

FORM (MODIFY_Spectrogram_formula, U"Spectrogram: Formula", U"Spectrogram: Formula...") {
	LABEL (U"label", U"Do for all times and frequencies:")
	LABEL (U"label", U"   `x' is the time in seconds")
	LABEL (U"label", U"   `y' is the frequency in hertz")
	LABEL (U"label", U"   `self' is the current value in Pa\u00B2/Hz")
	LABEL (U"label", U"   Replace all values with:")
	TEXTFIELD (U"formula", U"self * exp (- x / 0.1)")
	OK
DO
	LOOP {
		iam (Spectrogram);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Spectrogram may have partially changed
			throw;
		}
	}
END }

FORM (REAL_Spectrogram_getPowerAt, U"Spectrogram: Get power at (time, frequency)", nullptr) {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Frequency (Hz)", U"1000")
	OK
DO
	Spectrogram me = FIRST_ANY (Spectrogram);
	double time = GET_REAL (U"Time"), frequency = GET_REAL (U"Frequency");
	MelderInfo_open ();
	MelderInfo_write (Matrix_getValueAtXY (me, time, frequency));
	MelderInfo_write (U" Pa2/Hz (at time = ", time, U" seconds and frequency = ", frequency, U" Hz)");
	MelderInfo_close ();
END }

DIRECT (HELP_Spectrogram_help) {
	HELP (U"Spectrogram")
}

DIRECT (MOVIE_Spectrogram_movie) {
	MOVIE_ONE (Spectrogram, U"Spectrogram movie", 300, 300)
		Matrix_movie (me, graphics);
	MOVIE_ONE_END
}

DIRECT (NEW_Spectrogram_to_Matrix) {
	LOOP {
		iam (Spectrogram);
		autoMatrix thee = Spectrogram_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Spectrogram_to_Sound, U"Spectrogram: To Sound", nullptr) {
	REAL (U"Sampling frequency (Hz)", U"44100")
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoSound thee = Spectrogram_to_Sound (me, GET_REAL (U"Sampling frequency"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Spectrogram_to_Spectrum, U"Spectrogram: To Spectrum (slice)", nullptr) {
	REALVAR (time, U"Time (seconds)", U"0.0")
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoSpectrum you = Spectrogram_to_Spectrum (me, time);
		praat_new (you.move(), my name);
	}
END }

DIRECT (WINDOW_Spectrogram_view) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Spectrogram from batch.");
	LOOP {
		iam (Spectrogram);
		autoSpectrogramEditor editor = SpectrogramEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: - SPECTRUM

FORM (NEW_Spectrum_cepstralSmoothing, U"Spectrum: Cepstral smoothing", nullptr) {
	POSITIVE4 (bandwidth, U"Bandwidth (Hz)", U"500.0")
	OK
DO
	CONVERT_EACH (Spectrum)
		autoSpectrum result = Spectrum_cepstralSmoothing (me, bandwidth);
	CONVERT_EACH_END (my name)
}

FORM (GRAPHICS_Spectrum_draw, U"Spectrum: Draw", nullptr) {
	REAL4 (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL4 (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL4 (minimumPower, U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL4 (maximumPower, U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_draw (me, GRAPHICS, fromFrequency, toFrequency, minimumPower, maximumPower, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Spectrum_drawLogFreq, U"Spectrum: Draw (log freq)", nullptr) {
	POSITIVE4 (fromFrequency, U"left Frequency range (Hz)", U"10.0")
	POSITIVE4 (toFrequency, U"right Frequency range (Hz)", U"10000.0")
	REAL4 (minimumPower, U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL4 (maximumPower, U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_drawLogFreq (me, GRAPHICS, fromFrequency, toFrequency, minimumPower, maximumPower, garnish);
	GRAPHICS_EACH_END
}

DIRECT (WINDOW_Spectrum_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Spectrum from batch.");
	LOOP {
		iam (Spectrum);
		autoSpectrumEditor editor = SpectrumEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

FORM (MODIFY_Spectrum_formula, U"Spectrum: Formula", U"Spectrum: Formula...") {
	LABEL (U"label", U"`x' is the frequency in hertz, `col' is the bin number;   "
		"`y' = `row' is 1 (real part) or 2 (imaginary part)")
	LABEL (U"label", U"y := 1;   row := 1;   "
		"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	LABEL (U"label", U"y := 2;   row := 2;   "
		"x := 0;   for col := 1 to ncol do { self [2, col] := `formula' ; x := x + dx }")
	TEXTFIELD4 (formula, U"formula", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Spectrum)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (REAL_Spectrum_getBandDensity, U"Spectrum: Get band density", nullptr) {
	REAL (U"Band floor (Hz)", U"200.0")
	REAL (U"Band ceiling (Hz)", U"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double density = Spectrum_getBandDensity (me, GET_REAL (U"Band floor"), GET_REAL (U"Band ceiling"));
		Melder_informationReal (density, U"Pa2 / Hz2");
	}
END }

FORM (REAL_Spectrum_getBandDensityDifference, U"Spectrum: Get band density difference", nullptr) {
	REAL (U"Low band floor (Hz)", U"0.0")
	REAL (U"Low band ceiling (Hz)", U"500.0")
	REAL (U"High band floor (Hz)", U"500.0")
	REAL (U"High band ceiling (Hz)", U"4000.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandDensityDifference (me,
			GET_REAL (U"Low band floor"), GET_REAL (U"Low band ceiling"), GET_REAL (U"High band floor"), GET_REAL (U"High band ceiling"));
		Melder_informationReal (difference, U"dB");
	}
END }

FORM (REAL_Spectrum_getBandEnergy, U"Spectrum: Get band energy", nullptr) {
	REAL (U"Band floor (Hz)", U"200.0")
	REAL (U"Band ceiling (Hz)", U"1000.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double energy = Spectrum_getBandEnergy (me, GET_REAL (U"Band floor"), GET_REAL (U"Band ceiling"));
		Melder_informationReal (energy, U"Pa2 sec");
	}
END }

FORM (REAL_Spectrum_getBandEnergyDifference, U"Spectrum: Get band energy difference", nullptr) {
	REAL (U"Low band floor (Hz)", U"0.0")
	REAL (U"Low band ceiling (Hz)", U"500.0")
	REAL (U"High band floor (Hz)", U"500.0")
	REAL (U"High band ceiling (Hz)", U"4000.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandEnergyDifference (me,
			GET_REAL (U"Low band floor"), GET_REAL (U"Low band ceiling"), GET_REAL (U"High band floor"), GET_REAL (U"High band ceiling"));
		Melder_informationReal (difference, U"dB");
	}
END }

FORM (REAL_Spectrum_getBinNumberFromFrequency, U"Spectrum: Get bin number from frequency", nullptr) {
	REAL (U"Frequency (Hz)", U"2000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double bin = Sampled_xToIndex (me, GET_REAL (U"Frequency"));
		Melder_informationReal (bin, nullptr);
	}
END }

DIRECT (REAL_Spectrum_getBinWidth) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my dx, U"hertz");
	}
END }

FORM (REAL_Spectrum_getCentralMoment, U"Spectrum: Get central moment", U"Spectrum: Get central moment...") {
	POSITIVE (U"Moment", U"3.0")
	POSITIVE (U"Power", U"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double moment = Spectrum_getCentralMoment (me, GET_REAL (U"Moment"), GET_REAL (U"Power"));
		Melder_informationReal (moment, U"hertz to the power 'moment'");
	}
END }

FORM (REAL_Spectrum_getCentreOfGravity, U"Spectrum: Get centre of gravity", U"Spectrum: Get centre of gravity...") {
	POSITIVE (U"Power", U"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double centreOfGravity = Spectrum_getCentreOfGravity (me, GET_REAL (U"Power"));
		Melder_informationReal (centreOfGravity, U"hertz");
	}
END }

FORM (REAL_Spectrum_getFrequencyFromBin, U"Spectrum: Get frequency from bin", nullptr) {
	NATURAL (U"Band number", U"1")
	OK
DO
	LOOP {
		iam (Spectrum);
		double frequency = Sampled_indexToX (me, GET_INTEGER (U"Band number"));
		Melder_informationReal (frequency, U"hertz");
	}
END }

DIRECT (REAL_Spectrum_getHighestFrequency) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmax, U"hertz");
	}
END }

FORM (REAL_Spectrum_getImaginaryValueInBin, U"Spectrum: Get imaginary value in bin", nullptr) {
	NATURAL (U"Bin number", U"100")
	OK
DO
	long binNumber = GET_INTEGER (U"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
		Melder_informationReal (my z [2] [binNumber], nullptr);
	}
END }

FORM (REAL_Spectrum_getKurtosis, U"Spectrum: Get kurtosis", U"Spectrum: Get kurtosis...") {
	POSITIVE (U"Power", U"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double kurtosis = Spectrum_getKurtosis (me, GET_REAL (U"Power"));
		Melder_informationReal (kurtosis, nullptr);
	}
END }

DIRECT (REAL_Spectrum_getLowestFrequency) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmin, U"hertz");
	}
END }

DIRECT (INTEGER_Spectrum_getNumberOfBins) {
	LOOP {
		iam (Spectrum);
		Melder_information (my nx, U" bins");
	}
END }

#define REAL_ONE(klas)  LOOP { iam (klas);
#define REAL_ONE_END(value,unit)  Melder_informationReal (value, unit); } END

FORM (REAL_Spectrum_getRealValueInBin, U"Spectrum: Get real value in bin", nullptr) {
	NATURALVAR (binNumber, U"Bin number", U"100")
	OK
DO
	REAL_ONE (Spectrum)
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
	REAL_ONE_END (my z [1] [binNumber], nullptr)
}

FORM (REAL_Spectrum_getSkewness, U"Spectrum: Get skewness", U"Spectrum: Get skewness...") {
	POSITIVEVAR (power, U"Power", U"2.0")
	OK
DO
	REAL_ONE (Spectrum)
		double skewness = Spectrum_getSkewness (me, power);
	REAL_ONE_END (skewness, nullptr)
}

FORM (REAL_Spectrum_getStandardDeviation, U"Spectrum: Get standard deviation", U"Spectrum: Get standard deviation...") {
	POSITIVEVAR (power, U"Power", U"2.0")
	OK
DO
	REAL_ONE (Spectrum)
		double stdev = Spectrum_getStandardDeviation (me, power);
	REAL_ONE_END (stdev, U"hertz")
}

DIRECT (HELP_Spectrum_help) {
	Melder_help (U"Spectrum");
END }

FORM (LIST_Spectrum_list, U"Spectrum: List", 0) {
	BOOLEAN (U"Include bin number", false)
	BOOLEAN (U"Include frequency", true)
	BOOLEAN (U"Include real part", false)
	BOOLEAN (U"Include imaginary part", false)
	BOOLEAN (U"Include energy density", false)
	BOOLEAN (U"Include power density", true)
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_list (me, GET_INTEGER (U"Include bin number"), GET_INTEGER (U"Include frequency"),
			GET_INTEGER (U"Include real part"), GET_INTEGER (U"Include imaginary part"),
			GET_INTEGER (U"Include energy density"), GET_INTEGER (U"Include power density"));
	}
END }

FORM (NEW_Spectrum_lpcSmoothing, U"Spectrum: LPC smoothing", 0) {
	NATURAL (U"Number of peaks", U"5")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoSpectrum you = Spectrum_lpcSmoothing (me, GET_INTEGER (U"Number of peaks"), GET_REAL (U"Pre-emphasis from"));
		praat_new (you.move(), my name);
	}
END }

FORM (MODIFY_Spectrum_passHannBand, U"Spectrum: Filter (pass Hann band)", U"Spectrum: Filter (pass Hann band)...") {
	REAL (U"From frequency (Hz)", U"500")
	REAL (U"To frequency (Hz)", U"1000")
	POSITIVE (U"Smoothing (Hz)", U"100")
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_passHannBand (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Spectrum_stopHannBand, U"Spectrum: Filter (stop Hann band)", U"Spectrum: Filter (stop Hann band)...") {
	REAL (U"From frequency (Hz)", U"500")
	REAL (U"To frequency (Hz)", U"1000")
	POSITIVE (U"Smoothing (Hz)", U"100")
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_stopHannBand (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_dataChanged (me);
	}
END }

FORM (NEW_Spectrum_to_Excitation, U"Spectrum: To Excitation", 0) {
	POSITIVE (U"Frequency resolution (Bark)", U"0.1")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoExcitation thee = Spectrum_to_Excitation (me, GET_REAL (U"Frequency resolution"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Spectrum_to_Formant_peaks, U"Spectrum: To Formant (peaks)", 0) {
	LABEL (U"", U"Warning: this simply picks peaks from 0 Hz up!")
	NATURAL (U"Maximum number of formants", U"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoFormant thee = Spectrum_to_Formant (me, GET_INTEGER (U"Maximum number of formants"));
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Spectrum_to_Ltas, U"Spectrum: To Long-term average spectrum", nullptr) {
	POSITIVE (U"Bandwidth (Hz)", U"1000.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas (me, GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Spectrum_to_Ltas_1to1) {
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas_1to1 (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Spectrum_to_Matrix) {
	LOOP {
		iam (Spectrum);
		autoMatrix thee = Spectrum_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Spectrum_to_Sound) {
	LOOP {
		iam (Spectrum);
		autoSound thee = Spectrum_to_Sound (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Spectrum_to_Spectrogram) {
	LOOP {
		iam (Spectrum);
		autoSpectrogram thee = Spectrum_to_Spectrogram (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Spectrum_to_SpectrumTier_peaks) {
	LOOP {
		iam (Spectrum);
		autoSpectrumTier thee = Spectrum_to_SpectrumTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - STRINGS

// MARK: New

FORM (NEW1_Strings_createAsFileList, U"Create Strings as file list", U"Create Strings as file list...") {
	SENTENCEVAR (name, U"Name", U"fileList")
	LABEL (U"", U"File path:")
	static structMelderDir defaultDir { { 0 } };
	Melder_getHomeDir (& defaultDir);
	static const char32 *homeDirectory = Melder_dirToPath (& defaultDir);
	static char32 defaultPath [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, U"/*.wav");
	#elif defined (_WIN32)
	{
		static int len = str32len (homeDirectory);
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, len == 0 || homeDirectory [len - 1] != U'\\' ? U"\\" : U"", U"*.wav");
	}
	#else
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, U"/*.wav");
	#endif
	TEXTVAR (path, U"path", defaultPath)
	OK
DO
	autoStrings me = Strings_createAsFileList (path);
	praat_new (me.move(), name);
END }

FORM (NEW1_Strings_createAsDirectoryList, U"Create Strings as directory list", U"Create Strings as directory list...") {
	SENTENCEVAR (name, U"Name", U"directoryList")
	LABEL (U"", U"Path:")
	static structMelderDir defaultDir = { { 0 } };
	Melder_getHomeDir (& defaultDir);
	static const char32 *homeDirectory = Melder_dirToPath (& defaultDir);
	static char32 defaultPath [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, U"/*");
	#elif defined (_WIN32)
	{
		int len = str32len (workingDirectory);
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, len == 0 || homeDirectory [len - 1] != U'\\' ? U"\\" : U"");
	}
	#else
		Melder_sprint (defaultPath, kMelder_MAXPATH+1, homeDirectory, U"/*");
	#endif
	TEXTVAR (path, U"path", defaultPath)
	OK
DO
	autoStrings me = Strings_createAsDirectoryList (path);
	praat_new (me.move(), name);
END }

// MARK: Open

FORM_READ (READ1_Strings_readFromRawTextFile, U"Read Strings from raw text file", 0, true) {
	autoStrings me = Strings_readFromRawTextFile (file);
	praat_new (me.move(), MelderFile_name (file));
END }

// MARK: Save

FORM_SAVE (SAVE_Strings_writeToRawTextFile, U"Save Strings as text file", nullptr, U"txt") {
	LOOP {
		iam (Strings);
		Strings_writeToRawTextFile (me, file);
	}
END }

// MARK: Help

DIRECT (HELP_Strings_help) {
	Melder_help (U"Strings");
END }

// MARK: View & Edit

DIRECT (WINDOW_Strings_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Strings from batch.");
	LOOP {
		iam (Strings);
		autoStringsEditor editor = StringsEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: Query

DIRECT (BOOLEAN_Strings_equal) {
	Strings s1 = nullptr, s2 = nullptr;
	LOOP (s1 ? s2 : s1) = (Strings) OBJECT;
	bool equal = Data_equal (s1, s2);
	Melder_information ((int) equal);   // we need a 0 or 1
END }

DIRECT (INTEGER_Strings_getNumberOfStrings) {
	LOOP {
		iam (Strings);
		Melder_information (my numberOfStrings);
	}
END }

FORM (STRING_Strings_getString, U"Get string", nullptr) {
	NATURAL (U"Position", U"1")
	OK
DO
	LOOP {
		iam (Strings);
		long index = GET_INTEGER (U"Position");
		Melder_information (index > my numberOfStrings ? U"" : my strings [index]);   // TODO
	}
END }

// MARK: Modify

FORM (MODIFY_Strings_insertString, U"Strings: Insert string", nullptr) {
	INTEGERVAR (atPosition, U"At position", U"0 (= at end)")
	LABEL (U"", U"String:")
	TEXTVAR (string, U"string", U"")
	OK
DO
	LOOP {
		iam (Strings);
		Strings_insert (me, atPosition, string);
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Strings_nativize) {
	LOOP {
		iam (Strings);
		try {
			Strings_nativize (me);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // BUG: in case of error, the Strings may have partially changed
			throw;
		}
	}
END }

DIRECT (MODIFY_Strings_genericize) {
	LOOP {
		iam (Strings);
		try {
			Strings_genericize (me);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // BUG: in case of error, the Strings may have partially changed
			throw;
		}
	}
END }

DIRECT (MODIFY_Strings_randomize) {
	LOOP {
		iam (Strings);
		Strings_randomize (me);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Strings_removeString, U"Strings: Remove string", nullptr) {
	NATURAL (U"Position", U"1")
	OK
DO
	LOOP {
		iam (Strings);
		Strings_remove (me, GET_INTEGER (U"Position"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Strings_replaceAll, U"Strings: Replace all", nullptr) {
	SENTENCE (U"Find", U"a")
	SENTENCE (U"Replace with", U"b")
	INTEGER (U"Replace limit per string", U"0 (= unlimited)")
	RADIO (U"Find and replace strings are", 1)
		RADIOBUTTON (U"literals")
		RADIOBUTTON (U"regular expressions")
	OK
DO
	long numberOfMatches, numberOfStringMatches;
	LOOP {
		iam (Strings);
		autoStrings thee = Strings_change (me, GET_STRING (U"Find"), GET_STRING (U"Replace with"),
			GET_INTEGER (U"Replace limit per string"), & numberOfMatches, & numberOfStringMatches, GET_INTEGER (U"Find and replace strings are") - 1);
		praat_new (thee.move());
	}
END }

FORM (MODIFY_Strings_setString, U"Strings: Set string", nullptr) {
	NATURAL (U"Position", U"1")
	LABEL (U"", U"New string:")
	TEXTFIELD (U"newString", U"")
	OK
DO
	LOOP {
		iam (Strings);
		Strings_replace (me, GET_INTEGER (U"Position"), GET_STRING (U"newString"));
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Strings_sort) {
	LOOP {
		iam (Strings);
		Strings_sort (me);
		praat_dataChanged (me);
	}
END }

// MARK: Convert

DIRECT (NEW_Strings_to_Distributions) {
	LOOP {
		iam (Strings);
		autoDistributions thee = Strings_to_Distributions (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Strings_to_WordList) {
	LOOP {
		iam (Strings);
		autoWordList thee = Strings_to_WordList (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - TABLE; the remainder is in praat_Stat.cpp *****/

DIRECT (NEW_Table_to_Matrix) {
	LOOP {
		iam (Table);
		autoMatrix thee = Table_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - TEXTGRID; the remainder is in praat_TextGrid_init.cpp

FORM (NEW1_TextGrid_create, U"Create TextGrid", U"Create TextGrid...") {
	LABEL (U"", U"Hint: to label or segment an existing Sound,")
	LABEL (U"", U"select that Sound and choose \"To TextGrid...\".")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	SENTENCE (U"All tier names", U"Mary John bell")
	SENTENCE (U"Which of these are point tiers?", U"bell")
	OK
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax <= tmin) Melder_throw (U"End time should be greater than start time");
	autoTextGrid thee = TextGrid_create (tmin, tmax, GET_STRING (U"All tier names"), GET_STRING (U"Which of these are point tiers?"));
	praat_new (thee.move(), GET_STRING (U"All tier names"));
END }

// MARK: - TEXTTIER; the remainder is in praat_TextGrid_init.cpp *****/

FORM_READ (READ1_TextTier_readFromXwaves, U"Read TextTier from Xwaves", nullptr, true) {
	autoTextTier me = TextTier_readFromXwaves (file);
	praat_new (me.move(), MelderFile_name (file));
END }

/***** TRANSITION *****/

DIRECT (NEW_Transition_conflate) {
	LOOP {
		iam (Transition);
		autoDistributions thee = Transition_to_Distributions_conflate (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (GRAPHICS_Transition_drawAsNumbers, U"Draw as numbers", nullptr) {
	RADIO (U"Format", 1)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"2")
	OK
DO
	LOOP {
		iam (Transition);
		autoPraatPicture picture;
		Transition_drawAsNumbers (me, GRAPHICS, GET_INTEGER (U"Format"), GET_INTEGER (U"Precision"));
	}
END }

DIRECT (NEWTIMES2_Transition_eigen) {
	LOOP {
		iam (Transition);
		autoMatrix vectors, values;
		Transition_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	}
END }

DIRECT (HELP_Transition_help) {
	Melder_help (U"Transition");
END }

FORM (NEW_Transition_power, U"Transition: Power...", nullptr) {
	NATURAL (U"Power", U"2")
	OK
DO
	LOOP {
		iam (Transition);
		autoTransition thee = Transition_power (me, GET_INTEGER (U"Power"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Transition_to_Matrix) {
	LOOP {
		iam (Transition);
		autoMatrix thee = Transition_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

/***** Praat menu *****/

FORM (INFO_Praat_test, U"Praat test", 0) {
	OPTIONMENU_ENUM (U"Test", kPraatTests, DEFAULT)
	SENTENCE (U"arg1", U"1000000")
	SENTENCE (U"arg2", U"")
	SENTENCE (U"arg3", U"")
	SENTENCE (U"arg4", U"")
	OK
DO
	Praat_tests (GET_ENUM (kPraatTests, U"Test"), GET_STRING (U"arg1"),
		GET_STRING (U"arg2"), GET_STRING (U"arg3"), GET_STRING (U"arg4"));
END }

/***** Help menu *****/

DIRECT (HELP_ObjectWindow) { Melder_help (U"Object window"); END }
DIRECT (HELP_Intro) { Melder_help (U"Intro"); END }
DIRECT (HELP_WhatsNew) { Melder_help (U"What's new?"); END }
DIRECT (HELP_TypesOfObjects) { Melder_help (U"Types of objects"); END }
DIRECT (HELP_Editors) { Melder_help (U"Editors"); END }
DIRECT (HELP_FrequentlyAskedQuestions) { Melder_help (U"FAQ (Frequently Asked Questions)"); END }
DIRECT (HELP_Acknowledgments) { Melder_help (U"Acknowledgments"); END }
DIRECT (HELP_FormulasTutorial) { Melder_help (U"Formulas"); END }
DIRECT (HELP_ScriptingTutorial) { Melder_help (U"Scripting"); END }
DIRECT (HELP_DemoWindow) { Melder_help (U"Demo window"); END }
DIRECT (HELP_Interoperability) { Melder_help (U"Interoperability"); END }
DIRECT (HELP_Programming) { Melder_help (U"Programming with Praat"); END }
DIRECT (HELP_SearchManual_Fon) { Melder_search (); END }

/***** file recognizers *****/

static autoDaata cgnSyntaxFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 57) return autoDaata ();
	if (! strnequ (& header [0], "<?xml version=\"1.0\"?>", 21) ||
	    (! strnequ (& header [22], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35) &&
	     ! strnequ (& header [23], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35))) return autoDaata ();
	return TextGrid_readFromCgnSyntaxFile (file);
}

static autoDaata chronologicalTextGridTextFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 100) return autoDaata ();
	if (strnequ (& header [0], "\"Praat chronological TextGrid text file\"", 40))
		return TextGrid_readFromChronologicalTextFile (file);
	char headerCopy [101];
	memcpy (headerCopy, header, 100);
	headerCopy [100] = '\0';
	for (int i = 0; i < 100; i ++)
		if (headerCopy [i] == '\0') headerCopy [i] = '\001';
	//if (strstr (headerCopy, "\"\001P\001r\001a\001a\001t\001 \001c\001h\001r\001o\001n\001o\001l\001o\001g\001i\001c\001a\001l\001"
	//	" \001T\001e\001x\001t\001G\001r\001i\001d\001 t\001"))
	if (strstr (headerCopy, "\"\001P\001r\001a\001a\001t\001 \001c\001h\001r\001o\001n\001o\001l\001o\001g\001i\001c\001a\001l\001"
		" \001T\001e\001x\001t\001G\001r\001i\001d\001 \001t\001e\001x\001t\001 \001f\001i\001l\001e\001\""))
	{
		return TextGrid_readFromChronologicalTextFile (file);
	}
	return autoDaata ();
}

/***** buttons *****/

void praat_uvafon_init () {
	Thing_recognizeClassesByName (classPolygon, classParamCurve,
		classSpectrum, classLtas, classSpectrogram, classFormant,
		classExcitation, classCochleagram, classVocalTract,
		classLabel, classTier, classAutosegment,   // three obsolete classes
		classIntensity, classPitch, classHarmonicity,
		classTransition,
		classManipulation, classTextPoint, classTextInterval, classTextTier,
		classIntervalTier, classTextGrid, classWordList, classSpellingChecker,
		classCorpus,
		nullptr);
	Thing_recognizeClassByOtherName (classManipulation, U"Psola");
	Thing_recognizeClassByOtherName (classManipulation, U"Analysis");
	Thing_recognizeClassByOtherName (classPitchTier, U"StylPitch");

	Data_recognizeFileType (cgnSyntaxFileRecognizer);
	Data_recognizeFileType (chronologicalTextGridTextFileRecognizer);
	Data_recognizeFileType (IDXFormattedMatrixFileRecognizer);
	
	structManipulationEditor :: f_preferences ();
	structSpectrumEditor     :: f_preferences ();
	structFormantGridEditor  :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"Technical", U"Praat test...", nullptr, 0, INFO_Praat_test);

	/*
		The user interfaces for the classes are included in the order
		in which they have to appear in the New menu:
	*/
	praat_Sound_init ();
	praat_addMenuCommand (U"Objects", U"New", U"-- new numerics --", nullptr, 0, nullptr);
	praat_Matrix_init ();
	INCLUDE_LIBRARY (praat_uvafon_stat_init)
	praat_Tiers_init ();
	praat_uvafon_TextGrid_init ();

	praat_addMenuCommand (U"Objects", U"Open", U"Read Strings from raw text file...", nullptr, 0, READ1_Strings_readFromRawTextFile);

	praat_addMenuCommand (U"Objects", U"New", U"-- new textgrid --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create TextGrid...", nullptr, 0, NEW1_TextGrid_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Corpus...", nullptr, praat_HIDDEN, NEW1_Corpus_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as file list...", nullptr, 0, NEW1_Strings_createAsFileList);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as directory list...", nullptr, 0, NEW1_Strings_createAsDirectoryList);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read tier --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special tier file...", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read TextTier from Xwaves...", nullptr, 1, READ1_TextTier_readFromXwaves);
		praat_addMenuCommand (U"Objects", U"Open", U"Read IntervalTier from Xwaves...", nullptr, 1, READ1_IntervalTier_readFromXwaves);

	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Praat Intro", nullptr, '?', HELP_Intro);
	#ifndef macintosh
		praat_addMenuCommand (U"Objects", U"Help", U"Object window", nullptr, 0, HELP_ObjectWindow);
	#endif
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Frequently asked questions", nullptr, 0, HELP_FrequentlyAskedQuestions);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"What's new?", nullptr, 0, HELP_WhatsNew);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Types of objects", nullptr, 0, HELP_TypesOfObjects);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Editors", nullptr, 0, HELP_Editors);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Acknowledgments", nullptr, 0, HELP_Acknowledgments);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- shell help --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Formulas tutorial", nullptr, 0, HELP_FormulasTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Scripting tutorial", nullptr, 0, HELP_ScriptingTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Demo window", nullptr, 0, HELP_DemoWindow);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Interoperability", nullptr, 0, HELP_Interoperability);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Programming", nullptr, 0, HELP_Programming);
	#ifdef macintosh
		praat_addMenuCommand (U"Objects", U"Help", U"Praat Intro", nullptr, '?', HELP_Intro);
		praat_addMenuCommand (U"Objects", U"Help", U"Object window help", nullptr, 0, HELP_ObjectWindow);
		praat_addMenuCommand (U"Objects", U"Help", U"-- manual --", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Help", U"Search Praat manual...", nullptr, 'M', HELP_SearchManual_Fon);
	#endif

	praat_addAction1 (classCochleagram, 0, U"Cochleagram help", nullptr, 0, HELP_Cochleagram_help);
	praat_addAction1 (classCochleagram, 1, U"Movie", nullptr, 0, MOVIE_Cochleagram_movie);
praat_addAction1 (classCochleagram, 0, U"Info", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 2, U"Difference...", nullptr, 0, REAL_Cochleagram_difference);
praat_addAction1 (classCochleagram, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Paint...", nullptr, 0, GRAPHICS_Cochleagram_paint);
praat_addAction1 (classCochleagram, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Formula...", nullptr, 0, MODIFY_Cochleagram_formula);
praat_addAction1 (classCochleagram, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Excitation (slice)...", nullptr, 0, NEW_Cochleagram_to_Excitation);
praat_addAction1 (classCochleagram, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Matrix", nullptr, 0, NEW_Cochleagram_to_Matrix);

	praat_addAction1 (classCorpus, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Corpus_edit);

praat_addAction1 (classDistributions, 0, U"Learn", nullptr, 0, nullptr);
	praat_addAction1 (classDistributions, 1, U"To Transition...", nullptr, 0, NEW_Distributions_to_Transition);
	praat_addAction1 (classDistributions, 2, U"To Transition (noise)...", nullptr, 0, NEW1_Distributions_to_Transition_noise);

	praat_addAction1 (classExcitation, 0, U"Excitation help", nullptr, 0, HELP_Excitation_help);
praat_addAction1 (classExcitation, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Draw...", nullptr, 0, GRAPHICS_Excitation_draw);
praat_addAction1 (classExcitation, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Formant...", nullptr, 0, NEW_Excitation_to_Formant);
praat_addAction1 (classExcitation, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 1, U"Get loudness", nullptr, 0, REAL_Excitation_getLoudness);
praat_addAction1 (classExcitation, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Formula...", nullptr, 0, MODIFY_Excitation_formula);
praat_addAction1 (classExcitation, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Matrix", nullptr, 0, NEW_Excitation_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Formant help", nullptr, 0, HELP_Formant_help);
	praat_addAction1 (classFormant, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 0, U"Speckle...", nullptr, 1, GRAPHICS_Formant_drawSpeckles);
		praat_addAction1 (classFormant, 0, U"Draw tracks...", nullptr, 1, GRAPHICS_Formant_drawTracks);
		praat_addAction1 (classFormant, 0, U"Scatter plot...", nullptr, 1, GRAPHICS_Formant_scatterPlot);
	praat_addAction1 (classFormant, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 1, U"List...", nullptr, 1, LIST_Formant_list);
		praat_addAction1 (classFormant, 0, U"Down to Table...", nullptr, 1, NEW_Formant_downto_Table);
	praat_addAction1 (classFormant, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classFormant);
		praat_addAction1 (classFormant, 1, U"Get number of formants...", nullptr, 1, INTEGER_Formant_getNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get minimum number of formants", nullptr, 1, INTEGER_Formant_getMinimumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get maximum number of formants", nullptr, 1, INTEGER_Formant_getMaximumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get value at time...", nullptr, 1, REAL_Formant_getValueAtTime);
		praat_addAction1 (classFormant, 1, U"Get bandwidth at time...", nullptr, 1, REAL_Formant_getBandwidthAtTime);
		praat_addAction1 (classFormant, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get minimum...", nullptr, 1, REAL_Formant_getMinimum);
		praat_addAction1 (classFormant, 1, U"Get time of minimum...", nullptr, 1, REAL_Formant_getTimeOfMinimum);
		praat_addAction1 (classFormant, 1, U"Get maximum...", nullptr, 1, REAL_Formant_getMaximum);
		praat_addAction1 (classFormant, 1, U"Get time of maximum...", nullptr, 1, REAL_Formant_getTimeOfMaximum);
		praat_addAction1 (classFormant, 1, U"-- get distribution --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get quantile...", nullptr, 1, REAL_Formant_getQuantile);
		praat_addAction1 (classFormant, 1, U"Get quantile of bandwidth...", nullptr, 1, REAL_Formant_getQuantileOfBandwidth);
		praat_addAction1 (classFormant, 1, U"Get mean...", nullptr, 1, REAL_Formant_getMean);
		praat_addAction1 (classFormant, 1, U"Get standard deviation...", nullptr, 1, REAL_Formant_getStandardDeviation);
	praat_addAction1 (classFormant, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormant);
		praat_addAction1 (classFormant, 0, U"Sort", nullptr, 1, MODIFY_Formant_sort);
		praat_addAction1 (classFormant, 0, U"Formula (frequencies)...", nullptr, 1, MODIFY_Formant_formula_frequencies);
		praat_addAction1 (classFormant, 0, U"Formula (bandwidths)...", nullptr, 1, MODIFY_Formant_formula_bandwidths);
praat_addAction1 (classFormant, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"Track...", nullptr, 0, NEW_Formant_tracker);
	praat_addAction1 (classFormant, 0, U"Down to FormantTier", nullptr, praat_HIDDEN, NEW_Formant_downto_FormantTier);
	praat_addAction1 (classFormant, 0, U"Down to FormantGrid", nullptr, 0, NEW_Formant_downto_FormantGrid);
praat_addAction1 (classFormant, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"To Matrix...", nullptr, 0, NEW_Formant_to_Matrix);

	praat_addAction1 (classHarmonicity, 0, U"Harmonicity help", nullptr, 0, HELP_Harmonicity_help);
	praat_addAction1 (classHarmonicity, 0, U"Draw", nullptr, 0, nullptr);
		praat_addAction1 (classHarmonicity, 0, U"Draw...", nullptr, 0, GRAPHICS_Harmonicity_draw);
	praat_addAction1 (classHarmonicity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get value at time...", nullptr, 1, REAL_Harmonicity_getValueAtTime);
		praat_addAction1 (classHarmonicity, 1, U"Get value in frame...", nullptr, 1, REAL_Harmonicity_getValueInFrame);
		praat_addAction1 (classHarmonicity, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get minimum...", nullptr, 1, REAL_Harmonicity_getMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of minimum...", nullptr, 1, REAL_Harmonicity_getTimeOfMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get maximum...", nullptr, 1, REAL_Harmonicity_getMaximum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of maximum...", nullptr, 1, REAL_Harmonicity_getTimeOfMaximum);
		praat_addAction1 (classHarmonicity, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get mean...", nullptr, 1, REAL_Harmonicity_getMean);
		praat_addAction1 (classHarmonicity, 1, U"Get standard deviation...", nullptr, 1, REAL_Harmonicity_getStandardDeviation);
	praat_addAction1 (classHarmonicity, 0, U"Modify", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 0, U"Formula...", nullptr, 0, MODIFY_Harmonicity_formula);
	praat_addAction1 (classHarmonicity, 0, U"Hack", nullptr, 0, nullptr);
		praat_addAction1 (classHarmonicity, 0, U"To Matrix", nullptr, 0, NEW_Harmonicity_to_Matrix);

	praat_addAction1 (classIntensity, 0, U"Intensity help", nullptr, 0, HELP_Intensity_help);
	praat_addAction1 (classIntensity, 0, U"Draw...", nullptr, 0, GRAPHICS_Intensity_draw);
	praat_addAction1 (classIntensity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classIntensity);
		praat_addAction1 (classIntensity, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get value at time...", nullptr, 1, REAL_Intensity_getValueAtTime);
		praat_addAction1 (classIntensity, 1, U"Get value in frame...", nullptr, 1, REAL_Intensity_getValueInFrame);
		praat_addAction1 (classIntensity, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get minimum...", nullptr, 1, REAL_Intensity_getMinimum);
		praat_addAction1 (classIntensity, 1, U"Get time of minimum...", nullptr, 1, REAL_Intensity_getTimeOfMinimum);
		praat_addAction1 (classIntensity, 1, U"Get maximum...", nullptr, 1, REAL_Intensity_getMaximum);
		praat_addAction1 (classIntensity, 1, U"Get time of maximum...", nullptr, 1, REAL_Intensity_getTimeOfMaximum);
		praat_addAction1 (classIntensity, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get quantile...", nullptr, 1, REAL_Intensity_getQuantile);
		praat_addAction1 (classIntensity, 1, U"Get mean...", nullptr, 1, REAL_Intensity_getMean);
		praat_addAction1 (classIntensity, 1, U"Get standard deviation...", nullptr, 1, REAL_Intensity_getStandardDeviation);
	praat_addAction1 (classIntensity, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classIntensity);
		praat_addAction1 (classIntensity, 0, U"Formula...", nullptr, 1, MODIFY_Intensity_formula);
	praat_addAction1 (classIntensity, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classIntensity, 0, U"To IntensityTier (peaks)", nullptr, 0, NEW_Intensity_to_IntensityTier_peaks);
		praat_addAction1 (classIntensity, 0, U"To IntensityTier (valleys)", nullptr, 0, NEW_Intensity_to_IntensityTier_valleys);
	praat_addAction1 (classIntensity, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classIntensity, 0, U"Down to IntensityTier", nullptr, 0, NEW_Intensity_downto_IntensityTier);
		praat_addAction1 (classIntensity, 0, U"Down to Matrix", nullptr, 0, NEW_Intensity_downto_Matrix);

	praat_addAction1 (classLtas, 0, U"Ltas help", nullptr, 0, HELP_Ltas_help);
	praat_addAction1 (classLtas, 0, U"Draw...", nullptr, 0, GRAPHICS_Ltas_draw);
	praat_addAction1 (classLtas, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classLtas, 1, U"Frequency domain", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get lowest frequency", nullptr, 2, REAL_Ltas_getLowestFrequency);
		praat_addAction1 (classLtas, 1, U"Get highest frequency", nullptr, 2, REAL_Ltas_getHighestFrequency);
		praat_addAction1 (classLtas, 1, U"Frequency sampling", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get number of bins", nullptr, 2, INTEGER_Ltas_getNumberOfBins);
			praat_addAction1 (classLtas, 1, U"Get number of bands", U"*Get number of bins", praat_DEPTH_2 | praat_DEPRECATED_2004, INTEGER_Ltas_getNumberOfBins);
		praat_addAction1 (classLtas, 1, U"Get bin width", nullptr, 2, REAL_Ltas_getBinWidth);
			praat_addAction1 (classLtas, 1, U"Get band width", U"*Get bin width", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Ltas_getBinWidth);
		praat_addAction1 (classLtas, 1, U"Get frequency from bin number...", nullptr, 2, REAL_Ltas_getFrequencyFromBinNumber);
			praat_addAction1 (classLtas, 1, U"Get frequency from band...", U"*Get frequency from bin number...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Ltas_getFrequencyFromBinNumber);
		praat_addAction1 (classLtas, 1, U"Get bin number from frequency...", nullptr, 2, REAL_Ltas_getBinNumberFromFrequency);
			praat_addAction1 (classLtas, 1, U"Get band from frequency...", U"*Get bin number from frequency...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Ltas_getBinNumberFromFrequency);
		praat_addAction1 (classLtas, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get value at frequency...", nullptr, 1, REAL_Ltas_getValueAtFrequency);
		praat_addAction1 (classLtas, 1, U"Get value in bin...", nullptr, 1, REAL_Ltas_getValueInBin);
			praat_addAction1 (classLtas, 1, U"Get value in band...", U"*Get value in bin...", praat_DEPTH_1 | praat_DEPRECATED_2004, REAL_Ltas_getValueInBin);
		praat_addAction1 (classLtas, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get minimum...", nullptr, 1, REAL_Ltas_getMinimum);
		praat_addAction1 (classLtas, 1, U"Get frequency of minimum...", nullptr, 1, REAL_Ltas_getFrequencyOfMinimum);
		praat_addAction1 (classLtas, 1, U"Get maximum...", nullptr, 1, REAL_Ltas_getMaximum);
		praat_addAction1 (classLtas, 1, U"Get frequency of maximum...", nullptr, 1, REAL_Ltas_getFrequencyOfMaximum);
		praat_addAction1 (classLtas, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get mean...", nullptr, 1, REAL_Ltas_getMean);
		praat_addAction1 (classLtas, 1, U"Get slope...", nullptr, 1, REAL_Ltas_getSlope);
		praat_addAction1 (classLtas, 1, U"Get local peak height...", nullptr, 1, REAL_Ltas_getLocalPeakHeight);
		praat_addAction1 (classLtas, 1, U"Get standard deviation...", nullptr, 1, REAL_Ltas_getStandardDeviation);
	praat_addAction1 (classLtas, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Formula...", nullptr, 0, MODIFY_Ltas_formula);
	praat_addAction1 (classLtas, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To SpectrumTier (peaks)", nullptr, 0, NEW_Ltas_to_SpectrumTier_peaks);
	praat_addAction1 (classLtas, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Compute trend line...", nullptr, 0, NEW_Ltas_computeTrendLine);
	praat_addAction1 (classLtas, 0, U"Subtract trend line...", nullptr, 0, NEW_Ltas_subtractTrendLine);
	praat_addAction1 (classLtas, 0, U"Combine", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Merge", nullptr, praat_DEPRECATED_2005, NEW1_Ltases_merge);
	praat_addAction1 (classLtas, 0, U"Average", nullptr, 0, NEW1_Ltases_average);
	praat_addAction1 (classLtas, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To Matrix", nullptr, 0, NEW_Ltas_to_Matrix);

	praat_addAction1 (classManipulation, 0, U"Manipulation help", nullptr, 0, HELP_Manipulation_help);
	praat_addAction1 (classManipulation, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Manipulation_viewAndEdit);
	praat_addAction1 (classManipulation, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_Manipulation_viewAndEdit);
	praat_addAction1 (classManipulation, 0, U"Play (overlap-add)", nullptr, 0, PLAY_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0,   U"Play (PSOLA)", nullptr, praat_DEPRECATED_2007, PLAY_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Play (LPC)", nullptr, 0, PLAY_Manipulation_play_lpc);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (overlap-add)", nullptr, 0, NEW_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0,   U"Get resynthesis (PSOLA)", U"*Get resynthesis (overlap-add)", praat_DEPRECATED_2007, NEW_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (LPC)", nullptr, 0, NEW_Manipulation_getResynthesis_lpc);
	praat_addAction1 (classManipulation, 0, U"Extract original sound", nullptr, 0, NEW_Manipulation_extractOriginalSound);
	praat_addAction1 (classManipulation, 0, U"Extract pulses", nullptr, 0, NEW_Manipulation_extractPulses);
	praat_addAction1 (classManipulation, 0, U"Extract pitch tier", nullptr, 0, NEW_Manipulation_extractPitchTier);
	praat_addAction1 (classManipulation, 0, U"Extract duration tier", nullptr, 0, NEW_Manipulation_extractDurationTier);
	praat_addAction1 (classManipulation, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classManipulation);
		praat_addAction1 (classManipulation, 0, U"Replace pitch tier?", nullptr, 1, HELP_Manipulation_replacePitchTier_help);
		praat_addAction1 (classManipulation, 0, U"Replace duration tier?", nullptr, 1, HELP_Manipulation_replaceDurationTier_help);
	praat_addAction1 (classManipulation, 1, U"Save as text file without Sound...", nullptr, 0, SAVE_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1,   U"Write to text file without Sound...", U"*Save as text file without Sound...", praat_DEPRECATED_2011, SAVE_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, U"Save as binary file without Sound...", nullptr, 0, SAVE_Manipulation_writeToBinaryFileWithoutSound);
	praat_addAction1 (classManipulation, 1,   U"Write to binary file without Sound...", U"*Save as binary file without Sound...", praat_DEPRECATED_2011, SAVE_Manipulation_writeToBinaryFileWithoutSound);

	praat_addAction1 (classParamCurve, 0, U"ParamCurve help", nullptr, 0, HELP_ParamCurve_help);
	praat_addAction1 (classParamCurve, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classParamCurve, 0, U"Draw...", nullptr, 0, GRAPHICS_ParamCurve_draw);

	praat_addAction1 (classPitch, 0, U"Pitch help", nullptr, 0, HELP_Pitch_help);
	praat_addAction1 (classPitch, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Pitch_viewAndEdit);
	praat_addAction1 (classPitch, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_Pitch_viewAndEdit);
	praat_addAction1 (classPitch, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Play pulses", nullptr, 1, PLAY_Pitch_play);
		praat_addAction1 (classPitch, 0, U"Hum", nullptr, 1, PLAY_Pitch_hum);
		praat_addAction1 (classPitch, 0, U"-- to sound --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"To Sound (pulses)", nullptr, 1, NEW_Pitch_to_Sound_pulses);
		praat_addAction1 (classPitch, 0, U"To Sound (hum)", nullptr, 1, NEW_Pitch_to_Sound_hum);
		praat_addAction1 (classPitch, 0, U"To Sound (sine)...", nullptr, 1, NEW_Pitch_to_Sound_sine);
	praat_addAction1 (classPitch, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Draw...", nullptr, 1, GRAPHICS_Pitch_draw);
		praat_addAction1 (classPitch, 0, U"Draw logarithmic...", nullptr, 1, GRAPHICS_Pitch_drawLogarithmic);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 100 Hz)...", nullptr, 1, GRAPHICS_Pitch_drawSemitones100);
		praat_addAction1 (classPitch, 0,   U"Draw semitones...", U"*Draw semitones (re 100 Hz)...", praat_DEPTH_1 | praat_DEPRECATED_2012, GRAPHICS_Pitch_drawSemitones100);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 200 Hz)...", nullptr, 1, GRAPHICS_Pitch_drawSemitones200);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 440 Hz)...", nullptr, 1, GRAPHICS_Pitch_drawSemitones440);
		praat_addAction1 (classPitch, 0, U"Draw mel...", nullptr, 1, GRAPHICS_Pitch_drawMel);
		praat_addAction1 (classPitch, 0, U"Draw erb...", nullptr, 1, GRAPHICS_Pitch_drawErb);
		praat_addAction1 (classPitch, 0, U"Speckle...", nullptr, 1, GRAPHICS_Pitch_speckle);
		praat_addAction1 (classPitch, 0, U"Speckle logarithmic...", nullptr, 1, GRAPHICS_Pitch_speckleLogarithmic);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 100 Hz)...", nullptr, 1, GRAPHICS_Pitch_speckleSemitones100);
		praat_addAction1 (classPitch, 0,   U"Speckle semitones...", U"*Speckle semitones (re 100 Hz)...", praat_DEPTH_1 | praat_DEPRECATED_2012, GRAPHICS_Pitch_speckleSemitones100);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 200 Hz)...", nullptr, 1, GRAPHICS_Pitch_speckleSemitones200);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 440 Hz)...", nullptr, 1, GRAPHICS_Pitch_speckleSemitones440);
		praat_addAction1 (classPitch, 0, U"Speckle mel...", nullptr, 1, GRAPHICS_Pitch_speckleMel);
		praat_addAction1 (classPitch, 0, U"Speckle erb...", nullptr, 1, GRAPHICS_Pitch_speckleErb);
	praat_addAction1 (classPitch, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classPitch);
		praat_addAction1 (classPitch, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Count voiced frames", nullptr, 1, INTEGER_Pitch_getNumberOfVoicedFrames);
		praat_addAction1 (classPitch, 1, U"Get value at time...", nullptr, 1, REAL_Pitch_getValueAtTime);
		praat_addAction1 (classPitch, 1, U"Get value in frame...", nullptr, 1, REAL_Pitch_getValueInFrame);
		praat_addAction1 (classPitch, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get minimum...", nullptr, 1, REAL_Pitch_getMinimum);
		praat_addAction1 (classPitch, 1, U"Get time of minimum...", nullptr, 1, REAL_Pitch_getTimeOfMinimum);
		praat_addAction1 (classPitch, 1, U"Get maximum...", nullptr, 1, REAL_Pitch_getMaximum);
		praat_addAction1 (classPitch, 1, U"Get time of maximum...", nullptr, 1, REAL_Pitch_getTimeOfMaximum);
		praat_addAction1 (classPitch, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get quantile...", nullptr, 1, REAL_Pitch_getQuantile);
		/*praat_addAction1 (classPitch, 1, U"Get spreading...", nullptr, 1, REAL_Pitch_getSpreading);*/
		praat_addAction1 (classPitch, 1, U"Get mean...", nullptr, 1, REAL_Pitch_getMean);
		praat_addAction1 (classPitch, 1, U"Get standard deviation...", nullptr, 1, REAL_Pitch_getStandardDeviation);
		praat_addAction1 (classPitch, 1, U"-- get slope --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get mean absolute slope...", nullptr, 1, REAL_Pitch_getMeanAbsoluteSlope);
		praat_addAction1 (classPitch, 1, U"Get slope without octave jumps", nullptr, 1, REAL_Pitch_getMeanAbsSlope_noOctave);
		praat_addAction1 (classPitch, 2, U"-- query two --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 2, U"Count differences", nullptr, 1, INFO_Pitch_difference);
	praat_addAction1 (classPitch, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPitch);
		praat_addAction1 (classPitch, 0, U"Formula...", nullptr, 1, MODIFY_Pitch_formula);
	praat_addAction1 (classPitch, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextGrid...", nullptr, 1, NEW_Pitch_to_TextGrid);
		praat_addAction1 (classPitch, 0, U"-- to single tier --", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextTier", nullptr, praat_HIDDEN | praat_DEPTH_1, NEW_Pitch_to_TextTier);
		praat_addAction1 (classPitch, 0, U"To IntervalTier", nullptr, praat_HIDDEN | praat_DEPTH_1, NEW_Pitch_to_IntervalTier);
	praat_addAction1 (classPitch, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To PointProcess", nullptr, 1, NEW_Pitch_to_PointProcess);
	praat_addAction1 (classPitch, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Interpolate", nullptr, 1, NEW_Pitch_interpolate);
		praat_addAction1 (classPitch, 0, U"Smooth...", nullptr, 1, NEW_Pitch_smooth);
		praat_addAction1 (classPitch, 0, U"Subtract linear fit...", nullptr, 1, NEW_Pitch_subtractLinearFit);
		praat_addAction1 (classPitch, 0, U"Hack", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"Kill octave jumps", nullptr, 2, NEW_Pitch_killOctaveJumps);
		praat_addAction1 (classPitch, 0, U"-- to other types --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"Down to PitchTier", nullptr, 1, NEW_Pitch_to_PitchTier);
		praat_addAction1 (classPitch, 0, U"To Matrix", nullptr, 1, NEW_Pitch_to_Matrix);

	praat_addAction1 (classPolygon, 0, U"Polygon help", nullptr, 0, HELP_Polygon_help);
praat_addAction1 (classPolygon, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"Draw...", nullptr, 1, GRAPHICS_Polygon_draw);
	praat_addAction1 (classPolygon, 0, U"Draw closed...", nullptr, 1, GRAPHICS_Polygon_drawClosed);
	praat_addAction1 (classPolygon, 0, U"Paint...", nullptr, 1, GRAPHICS_Polygon_paint);
	praat_addAction1 (classPolygon, 0, U"Draw circles...", nullptr, 1, GRAPHICS_Polygon_drawCircles);
	praat_addAction1 (classPolygon, 0, U"Paint circles...", nullptr, 1, GRAPHICS_Polygon_paintCircles);
	praat_addAction1 (classPolygon, 2, U"Draw connection...", nullptr, 1, GRAPHICS_Polygons_drawConnection);
praat_addAction1 (classPolygon, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"Randomize", nullptr, 1, MODIFY_Polygon_randomize);
	praat_addAction1 (classPolygon, 0, U"Salesperson...", nullptr, 1, MODIFY_Polygon_salesperson);
praat_addAction1 (classPolygon, 0, U"Hack -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"To Matrix", nullptr, 1, NEW_Polygon_to_Matrix);

	praat_addAction1 (classSpectrogram, 0, U"Spectrogram help", nullptr, 0, HELP_Spectrogram_help);
	praat_addAction1 (classSpectrogram, 1, U"View", nullptr, 0, WINDOW_Spectrogram_view);
	praat_addAction1 (classSpectrogram, 1, U"Movie", nullptr, 0, MOVIE_Spectrogram_movie);
	praat_addAction1 (classSpectrogram, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 1, U"Get power at...", nullptr, 1, REAL_Spectrogram_getPowerAt);
	praat_addAction1 (classSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"Paint...", nullptr, 1, GRAPHICS_Spectrogram_paint);
	praat_addAction1 (classSpectrogram, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 1, NEW_Spectrogram_to_Spectrum);
	praat_addAction1 (classSpectrogram, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Sound...", nullptr, 1, NEW_Spectrogram_to_Sound);
	praat_addAction1 (classSpectrogram, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 0, U"Formula...", nullptr, 1, MODIFY_Spectrogram_formula);
	praat_addAction1 (classSpectrogram, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Matrix", nullptr, 1, NEW_Spectrogram_to_Matrix);

	praat_addAction1 (classSpectrum, 0, U"Spectrum help", nullptr, 0, HELP_Spectrum_help);
	praat_addAction1 (classSpectrum, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Spectrum_viewAndEdit);
	praat_addAction1 (classSpectrum, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_Spectrum_viewAndEdit);
	praat_addAction1 (classSpectrum, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Sound", nullptr, 1, NEW_Spectrum_to_Sound);
		praat_addAction1 (classSpectrum, 0,   U"To Sound (fft)", U"*To Sound", praat_DEPTH_1 | praat_DEPRECATED_2004, NEW_Spectrum_to_Sound);
	praat_addAction1 (classSpectrum, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Draw...", nullptr, 1, GRAPHICS_Spectrum_draw);
		praat_addAction1 (classSpectrum, 0, U"Draw (log freq)...", nullptr, 1, GRAPHICS_Spectrum_drawLogFreq);
	praat_addAction1 (classSpectrum, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"List...", nullptr, 1, LIST_Spectrum_list);
	praat_addAction1 (classSpectrum, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Frequency domain", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get lowest frequency", nullptr, 2, REAL_Spectrum_getLowestFrequency);
			praat_addAction1 (classSpectrum, 1, U"Get highest frequency", nullptr, 2, REAL_Spectrum_getHighestFrequency);
		praat_addAction1 (classSpectrum, 1, U"Frequency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get number of bins", nullptr, 2, INTEGER_Spectrum_getNumberOfBins);
			praat_addAction1 (classSpectrum, 1, U"Get bin width", nullptr, 2, REAL_Spectrum_getBinWidth);
			praat_addAction1 (classSpectrum, 1, U"Get frequency from bin number...", nullptr, 2, REAL_Spectrum_getFrequencyFromBin);
			praat_addAction1 (classSpectrum, 1,   U"Get frequency from bin...", U"*Get frequency from bin number...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Spectrum_getFrequencyFromBin);
			praat_addAction1 (classSpectrum, 1, U"Get bin number from frequency...", nullptr, 2, REAL_Spectrum_getBinNumberFromFrequency);
			praat_addAction1 (classSpectrum, 1,   U"Get bin from frequency...", nullptr, praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Spectrum_getBinNumberFromFrequency);
		praat_addAction1 (classSpectrum, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get real value in bin...", nullptr, 1, REAL_Spectrum_getRealValueInBin);
		praat_addAction1 (classSpectrum, 1, U"Get imaginary value in bin...", nullptr, 1, REAL_Spectrum_getImaginaryValueInBin);
		praat_addAction1 (classSpectrum, 1, U"-- get energy --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get band energy...", nullptr, 1, REAL_Spectrum_getBandEnergy);
		praat_addAction1 (classSpectrum, 1, U"Get band density...", nullptr, 1, REAL_Spectrum_getBandDensity);
		praat_addAction1 (classSpectrum, 1, U"Get band energy difference...", nullptr, 1, REAL_Spectrum_getBandEnergyDifference);
		praat_addAction1 (classSpectrum, 1, U"Get band density difference...", nullptr, 1, REAL_Spectrum_getBandDensityDifference);
		praat_addAction1 (classSpectrum, 1, U"-- get moments --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get centre of gravity...", nullptr, 1, REAL_Spectrum_getCentreOfGravity);
		praat_addAction1 (classSpectrum, 1, U"Get standard deviation...", nullptr, 1, REAL_Spectrum_getStandardDeviation);
		praat_addAction1 (classSpectrum, 1, U"Get skewness...", nullptr, 1, REAL_Spectrum_getSkewness);
		praat_addAction1 (classSpectrum, 1, U"Get kurtosis...", nullptr, 1, REAL_Spectrum_getKurtosis);
		praat_addAction1 (classSpectrum, 1, U"Get central moment...", nullptr, 1, REAL_Spectrum_getCentralMoment);
	praat_addAction1 (classSpectrum, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Formula...", nullptr, 1, MODIFY_Spectrum_formula);
		praat_addAction1 (classSpectrum, 0, U"Filter (pass Hann band)...", nullptr, 1, MODIFY_Spectrum_passHannBand);
		praat_addAction1 (classSpectrum, 0, U"Filter (stop Hann band)...", nullptr, 1, MODIFY_Spectrum_stopHannBand);
	praat_addAction1 (classSpectrum, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Excitation...", nullptr, 1, NEW_Spectrum_to_Excitation);
		praat_addAction1 (classSpectrum, 0, U"To SpectrumTier (peaks)", nullptr, 1, NEW_Spectrum_to_SpectrumTier_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Formant (peaks)...", nullptr, 1, NEW_Spectrum_to_Formant_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Ltas...", nullptr, 1, NEW_Spectrum_to_Ltas);
		praat_addAction1 (classSpectrum, 0, U"To Ltas (1-to-1)", nullptr, 1, NEW_Spectrum_to_Ltas_1to1);
		praat_addAction1 (classSpectrum, 0, U"To Spectrogram", nullptr, 1, NEW_Spectrum_to_Spectrogram);
	praat_addAction1 (classSpectrum, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Cepstral smoothing...", nullptr, 1, NEW_Spectrum_cepstralSmoothing);
		praat_addAction1 (classSpectrum, 0, U"LPC smoothing...", nullptr, 1, NEW_Spectrum_lpcSmoothing);
		praat_addAction1 (classSpectrum, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 0, U"To Matrix", nullptr, 2, NEW_Spectrum_to_Matrix);

	praat_addAction1 (classStrings, 0, U"Strings help", nullptr, 0, HELP_Strings_help);
	praat_addAction1 (classStrings, 1, U"Save as raw text file...", nullptr, 0, SAVE_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1,  U"Write to raw text file...", U"*Save as raw text file...", praat_DEPRECATED_2011, SAVE_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Strings_viewAndEdit);
	praat_addAction1 (classStrings, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_Strings_viewAndEdit);
	praat_addAction1 (classStrings, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 2, U"Equal?", nullptr, 1, BOOLEAN_Strings_equal);
		praat_addAction1 (classStrings, 1, U"Get number of strings", nullptr, 1, INTEGER_Strings_getNumberOfStrings);
		praat_addAction1 (classStrings, 1, U"Get string...", nullptr, 1, STRING_Strings_getString);
	praat_addAction1 (classStrings, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"Set string...", nullptr, 1, MODIFY_Strings_setString);
		praat_addAction1 (classStrings, 0, U"Insert string...", nullptr, 1, MODIFY_Strings_insertString);
		praat_addAction1 (classStrings, 0, U"Remove string...", nullptr, 1, MODIFY_Strings_removeString);
		praat_addAction1 (classStrings, 0, U"-- modify order --", nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Randomize", nullptr, 1, MODIFY_Strings_randomize);
		praat_addAction1 (classStrings, 0, U"Sort", nullptr, 1, MODIFY_Strings_sort);
		praat_addAction1 (classStrings, 0, U"-- convert --", nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Replace all...", nullptr, 1, MODIFY_Strings_replaceAll);
		praat_addAction1 (classStrings, 0, U"Genericize", nullptr, 1, MODIFY_Strings_genericize);
		praat_addAction1 (classStrings, 0, U"Nativize", nullptr, 1, MODIFY_Strings_nativize);
	praat_addAction1 (classStrings, 0, U"Analyze", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"To Distributions", nullptr, 0, NEW_Strings_to_Distributions);
	praat_addAction1 (classStrings, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"To WordList", nullptr, 0, NEW_Strings_to_WordList);

	praat_addAction1 (classTable, 0, U"Down to Matrix", nullptr, 0, NEW_Table_to_Matrix);

	praat_addAction1 (classTransition, 0, U"Transition help", nullptr, 0, HELP_Transition_help);
praat_addAction1 (classTransition, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Draw as numbers...", nullptr, 0, GRAPHICS_Transition_drawAsNumbers);
praat_addAction1 (classTransition, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Eigen", nullptr, 0, NEWTIMES2_Transition_eigen);
	praat_addAction1 (classTransition, 0, U"Conflate", nullptr, 0, NEW_Transition_conflate);
praat_addAction1 (classTransition, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Power...", nullptr, 0, NEW_Transition_power);
praat_addAction1 (classTransition, 0, U"Cast", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"To Matrix", nullptr, 0, NEW_Transition_to_Matrix);

	praat_addAction2 (classDistributions, 1, classTransition, 1, U"Map", nullptr, 0, NEW1_Distributions_Transition_map);
	praat_addAction2 (classDistributions, 1, classTransition, 1, U"To Transition...", nullptr, 0, NEW1_Distributions_to_Transition_adj);
	praat_addAction2 (classDistributions, 2, classTransition, 1, U"To Transition (noise)...", nullptr, 0, NEW1_Distributions_to_Transition_noise_adj);

	praat_addAction2 (classFormant, 1, classPointProcess, 1, U"To FormantTier", nullptr, 0, NEW1_Formant_PointProcess_to_FormantTier);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter", nullptr, 0, NEW1_Sound_Formant_filter);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, NEW1_Sound_Formant_filter_noscale);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw (phonetogram)...", nullptr, 0, GRAPHICS_Pitch_Intensity_draw);
	praat_addAction2 (classIntensity, 1, classPitch, 1,   U"Speckle (phonetogram)...", nullptr, praat_DEPRECATED_2005, GRAPHICS_Pitch_Intensity_speckle);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Get mean", nullptr, 1, REAL_Pitch_Intensity_getMean);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Get mean absolute slope", nullptr, 1, REAL_Pitch_Intensity_getMeanAbsoluteSlope);
	praat_addAction2 (classIntensity, 1, classPointProcess, 1, U"To IntensityTier", nullptr, 0, NEW1_Intensity_PointProcess_to_IntensityTier);

	praat_addAction2 (classManipulation, 1, classSound, 1, U"Replace original sound", nullptr, 0, MODIFY_Manipulation_replaceOriginalSound);
	praat_addAction2 (classManipulation, 1, classPointProcess, 1, U"Replace pulses", nullptr, 0, MODIFY_Manipulation_replacePulses);
	praat_addAction2 (classManipulation, 1, classPitchTier, 1, U"Replace pitch tier", nullptr, 0, MODIFY_Manipulation_replacePitchTier);
	praat_addAction2 (classManipulation, 1, classDurationTier, 1, U"Replace duration tier", nullptr, 0, MODIFY_Manipulation_replaceDurationTier);
	praat_addAction2 (classManipulation, 1, classTextTier, 1, U"To Manipulation", nullptr, 0, NEW1_Manipulation_TextTier_to_Manipulation);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"Draw...", nullptr, 0, GRAPHICS_PitchTier_Pitch_draw);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"To Pitch", nullptr, 0, NEW1_Pitch_PitchTier_to_Pitch);
	praat_addAction2 (classPitch, 1, classPointProcess, 1, U"To PitchTier", nullptr, 0, NEW1_Pitch_PointProcess_to_PitchTier);
	praat_addAction3 (classPitch, 1, classPointProcess, 1, classSound, 1, U"Voice report...", nullptr, 0, INFO_Sound_Pitch_PointProcess_voiceReport);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (cc)", nullptr, 0, NEW1_Sound_Pitch_to_PointProcess_cc);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (peaks)...", nullptr, 0, NEW1_Sound_Pitch_to_PointProcess_peaks);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To Manipulation", nullptr, 0, NEW1_Sound_Pitch_to_Manipulation);

	praat_addAction4 (classDurationTier, 1, classPitchTier, 1, classPointProcess, 1, classSound, 1, U"To Sound...", nullptr, 0, NEW1_Sound_Point_Pitch_Duration_to_Sound);

	INCLUDE_MANPAGES (manual_Manual_init)
	INCLUDE_MANPAGES (manual_Script_init)
	INCLUDE_MANPAGES (manual_Picture_init)

	INCLUDE_MANPAGES (manual_Fon_init)
	INCLUDE_MANPAGES (manual_tutorials_init)
	INCLUDE_MANPAGES (manual_glossary_init)
	INCLUDE_MANPAGES (manual_references_init)
	INCLUDE_MANPAGES (manual_soundFiles_init)
	INCLUDE_MANPAGES (manual_statistics_init)
	INCLUDE_MANPAGES (manual_voice_init)
	INCLUDE_MANPAGES (manual_programming_init)
	INCLUDE_MANPAGES (manual_gram_init)
	INCLUDE_MANPAGES (manual_ExperimentMFC_init)
	INCLUDE_MANPAGES (manual_EEG_init)

	INCLUDE_LIBRARY (praat_EEG_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new synthesis --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_Artsynth_init)
	INCLUDE_LIBRARY (praat_uvafon_David_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new grammars --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_gram_init)
	INCLUDE_LIBRARY (praat_uvafon_FFNet_init)
	INCLUDE_LIBRARY (praat_uvafon_LPC_init)
	praat_ExperimentMFC_init ();
}

/* End of file praat_Fon.cpp */
