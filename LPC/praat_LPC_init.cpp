/* praat_LPC_init.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
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
 djmw 20030613 Latest modification
 djmw 20040414 Forms texts.
 djmw 20060428 Latest modification
 djmw 20061218 Changed to Melder_information<x> format.
 djmw 20070902 Melder_error<1...>
 djmw 20071011 REQUIRE requires L"".
 djmw 20080313 Cepstrum_formula
 djmw 20100212 Analysis window length is now "Window length"
*/

#include "Cepstrumc.h"
#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "DTW.h"
#include "FilterBank.h"
#include "Formant_extensions.h"
#include "FormantPath.h"
#include "FormantPathEditor.h"
#include "IntervalTierNavigator.h"
#include "LPC.h"
#include "MFCC.h"
#include "LFCC.h"
#include "LPC_and_Cepstrumc.h"
#include "LPC_and_Formant.h"
#include "LPC_and_LFCC.h"
#include "LPC_and_LineSpectralFrequencies.h"
#include "LPC_and_Polynomial.h"
#include "LPC_and_Tube.h"
#include "LPC_to_Spectrogram.h"
#include "LPC_to_Spectrum.h"
#include "NUM2.h"
#include "PowerCepstrum.h"
#include "PowerCepstrogram.h"
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "VocalTractTier.h"

#include "praat_TimeFunction.h"
#include "praat_Matrix.h"

#define praat_Quefrency_RANGE(fromQuefrency,toQuefrency) \
	REAL (fromQuefrency, U"left Quefrency range (s)", U"0.0") \
	REAL (toQuefrency, U"right Quefrency range (s)", U"0.0 (= all)")

static const conststring32 DRAW_BUTTON    = U"Draw -";
static const conststring32 QUERY_BUTTON   = U"Query -";
static const conststring32 MODIFY_BUTTON   = U"Modify -";

void praat_CC_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);

static void cb_FormantPathEditor_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication.move());
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}

DIRECT (WINDOW_FormantPath_viewAndEditAlone) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot view or edit a Formant from batch.");
	FIND_ONE_WITH_IOBJECT (FormantPath)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, nullptr, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (HINT_FormantPath_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a Sound in your FormantPath window:\n"
			"select a FormantPath and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

FORM (GRAPHICS_FormantPath_drawAsGrid, U"FormantPath: Draw as grid", nullptr) {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.1")
	POSITIVE (fmax, U"Maximum frequency", U"6200.0")
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"5")
	BOOLEAN (showBandwidths, U"Show bandwidths", true)
	COLOUR (odd, U"Colour of F1, F3, F5", U"red")
	COLOUR (even, U"Colour of F2, F4", U"purple")
	INTEGER (numberOfRows, U"Number of rows", U"0")
	INTEGER (numberOfColumns, U"Number of columns", U"0")
	POSITIVE (xSpaceFraction, U"X space fraction", U"0.1")
	POSITIVE (ySpaceFraction, U"Y space fraction", U"0.1")
	POSITIVE (lineEvery_Hz, U"Horizontal line every (Hz)", U"1000.0")
	REAL (xCursor, U"X cursor line at (s)", U"-0.1 (=no line)")
	REAL (yCursor, U"Y cursor at (Hz)", U"-100.0 (=no line)")
	SENTENCE (parameters_string, U"Coefficients by track", U"7 7 7 7")
	BOOLEAN (markCandidatesWithinPath, U"Mark candidates within path", false)
	COLOUR (markColour, U"Mark colour", U"{0.984,0.984, 0.7}")
	BOOLEAN (showStress, U"Show stress", true)
	POSITIVE (powerf, U"Power", U"1.25")
	BOOLEAN (showEstimatedModels, U"Show estimated models", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantPath)
		autoINTVEC parameters = newINTVECfromString (parameters_string);
		FormantPath_drawAsGrid (me, GRAPHICS, tmin, tmax, fmax, fromFormant, toFormant, showBandwidths, odd, even, numberOfRows, numberOfColumns, xSpaceFraction, ySpaceFraction, lineEvery_Hz, xCursor, yCursor, markColour, parameters.get(), markCandidatesWithinPath, showStress, powerf, showEstimatedModels, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_FormantPath_to_Matrix_stress, U"FormantPath: To Matrix (stress)", nullptr) {
	POSITIVE (windowLength, U"Window length", U"0.025")
	SENTENCE (parameters_string, U"Coefficients by track", U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	CONVERT_EACH (FormantPath)
		autoINTVEC parameters = newINTVECfromString (parameters_string);
		autoMatrix result = FormantPath_to_Matrix_stress (me, windowLength, parameters.get (), powerf);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_FormantPath_to_Matrix_qsums, U"FormantPath: To Matrix (qsums)", nullptr) {
	INTEGER (numberOfTracks, U"Number of tracks", U"4")
	OK
DO
	CONVERT_EACH (FormantPath)
		autoMatrix result = FormantPath_to_Matrix_qSums (me, numberOfTracks);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_FormantPath_to_Matrix_transition,  U"FormantPath: To Matrix (transition)", nullptr) {
	INTEGER (numberOfTracks, U"Number of tracks", U"4")
	BOOLEAN (maximumCosts, U"Maximum costs", false)
	OK
DO
	CONVERT_EACH (FormantPath)
		autoMatrix result = FormantPath_to_Matrix_transition (me, numberOfTracks, maximumCosts);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_FormantPath_to_Matrix_deltas,  U"FormantPath: To Matrix (deltas)", nullptr) {
	LABEL (U"Within frame:")
	REAL (qWeight, U"F/B weight (0-1)", U"1.0")
	LABEL (U"Between frames:")
	REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
	REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
	REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
	POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
	LABEL (U"Global stress parameters:")
	POSITIVE (windowLength, U"Window length", U"0.035")
	SENTENCE (parameters_string, U"Coefficients by track", U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	CONVERT_EACH (FormantPath)
		autoMatrix result;
		Melder_require (qWeight >= 0 && qWeight <= 1.0 &&
			frequencyChangeWeight >= 0 && frequencyChangeWeight <= 1.0 &&
			stressWeight >= 0 && stressWeight <= 1.0 &&
			ceilingChangeWeight >= 0 && ceilingChangeWeight <= 1.0,
			U"A weight should greater or equal 0.0 and smaller or equal 1.0.");
		autoINTVEC parameters = newINTVECfromString (parameters_string);
		autoINTVEC path = FormantPath_getOptimumPath (me, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight, windowLength, intensityModulationStepSize, parameters.get(), powerf, & result);	
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_FormantPath_pathFinder,  U"FormantPath: Path finder", nullptr) {
	LABEL (U"Within frame:")
	REAL (qWeight, U"F/B weight (0-1)", U"1.0")
	LABEL (U"Between frames:")
	REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
	REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
	REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
	POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
	LABEL (U"Global stress parameters:")
	POSITIVE (windowLength, U"Window length", U"0.035")
	SENTENCE (parameters_string, U"Coefficients by track", U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	MODIFY_EACH (FormantPath)
		Melder_require (qWeight >= 0 && qWeight <= 1.0 &&
			frequencyChangeWeight >= 0 && frequencyChangeWeight <= 1.0 &&
			stressWeight >= 0 && stressWeight <= 1.0 &&
			ceilingChangeWeight >= 0 && ceilingChangeWeight <= 1.0,
			U"A weight should be greater than or equal to 0.0 and smaller than or equal to 1.0.");
		autoINTVEC parameters = newINTVECfromString (parameters_string);
		FormantPath_pathFinder (me, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight, intensityModulationStepSize, windowLength, parameters.get(), powerf);
	MODIFY_EACH_END
}

DIRECT (NEW_FormantPath_extractFormant) {
	CONVERT_EACH (FormantPath)
		autoFormant result = FormantPath_extractFormant (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (WINDOW_Sound_TextGrid_FormantPath_createFormantPathEditor) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot view or edit a Formant from batch.");
	FIND_THREE_WITH_IOBJECT (FormantPath, Sound, TextGrid)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, you, him);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (WINDOW_Sound_FormantPath_createFormantPathEditor) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot view or edit a Formant from batch.");
	FIND_TWO_WITH_IOBJECT (FormantPath, Sound)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, you, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

/********************** Cepstrum  ****************************************/

DIRECT (NEW_Cepstrum_downto_PowerCepstrum) {
	CONVERT_EACH (Cepstrum)
		autoPowerCepstrum result = Cepstrum_downto_PowerCepstrum (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (HELP_PowerCepstrum_help) {
	HELP (U"PowerCepstrum")
}

FORM (GRAPHICS_Cepstrum_drawLinear, U"Cepstrum: Draw linear", U"Cepstrum: Draw (linear)...") {
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (ymin, U"Minimum", U"0.0")
	REAL (ymax, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Cepstrum)
		Cepstrum_drawLinear (me, GRAPHICS, fromQuefrency, toQuefrency, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_PowerCepstrum_draw, U"PowerCepstrum: Draw", U"PowerCepstrum: Draw...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REAL (fromAmplitude_dB, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude_dB, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PowerCepstrum)
		PowerCepstrum_draw (me, GRAPHICS, fromQuefrency, toQuefrency, fromAmplitude_dB, toAmplitude_dB, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_PowerCepstrum_drawTrendLine, U"PowerCepstrum: Draw trend line", U"PowerCepstrum: Draw trend line...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REAL (fromAmplitude_dB, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude_dB, U"right Amplitude range (dB)", U"0.0")
	LABEL (U"Parameters for the trend line fit")
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	GRAPHICS_EACH (PowerCepstrum)
		PowerCepstrum_drawTrendLine (me, GRAPHICS, fromQuefrency, toQuefrency,fromAmplitude_dB,toAmplitude_dB, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	GRAPHICS_EACH_END
}

FORM (MODIFY_PowerCepstrum_formula, U"PowerCepstrum: Formula...", U"PowerCepstrum: Formula...") {
	LABEL (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (formula, U"Formula:", U"self")
	OK
DO
	MODIFY_EACH (PowerCepstrum)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_END
}

FORM (REAL_PowerCepstrum_getPeak, U"PowerCepstrum: Get peak", U"PowerCepstrum: Get peak...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double result;
		PowerCepstrum_getMaximumAndQuefrency (me, fromPitch, toPitch, peakInterpolationType, & result, nullptr);
	NUMBER_ONE_END (U" dB")
}

FORM (REAL_PowerCepstrum_getQuefrencyOfPeak, U"PowerCepstrum: Get quefrency of peak", U"PowerCepstrum: Get quefrency of peak...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double result;
		PowerCepstrum_getMaximumAndQuefrency (me, fromPitch, toPitch, peakInterpolationType, nullptr, & result);
		double f = 1.0 / result;
	NUMBER_ONE_END (U" s (f =", f, U" Hz)")
}

FORM (REAL_PowerCepstrum_getRNR, U"PowerCepstrum: Get rhamonics to noise ration", nullptr) {
	REAL (fromPitch, U"left Pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Pitch range (Hz)", U"333.3")
	POSITIVE (fractionalWIdth, U"Fractional width (0-1)", U"0.05")
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		const double result = PowerCepstrum_getRNR (me, fromPitch, toPitch, fractionalWIdth);
	NUMBER_ONE_END (U" (rnr)")
}

FORM (REAL_PowerCepstrum_getPeakProminence_hillenbrand, U"PowerCepstrum: Get peak prominence (hillenbrand)", U"PowerCepstrum: Get peak prominence (hillenbrand)...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double qpeak;
		const double result = PowerCepstrum_getPeakProminence_hillenbrand (me, fromPitch, toPitch, & qpeak);
	NUMBER_ONE_END (U" dB; quefrency=", qpeak, U" s (f=", 1.0 / qpeak, U" Hz).")
}

FORM (REAL_PowerCepstrum_getTrendLineSlope, U"PowerCepstrum: Get trend line slope", U"PowerCepstrum: Get trend line slope...") {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double result;
		PowerCepstrum_fitTrendLine (me, fromQuefrency_trendLine, toQuefrency_trendLine, & result, nullptr, lineType, fitMethod);
	NUMBER_ONE_END (U" dB / ", lineType == kCepstrumTrendType::LINEAR ? U"s" : U"ln (s)");
}

FORM (REAL_PowerCepstrum_getTrendLineIntercept, U"PowerCepstrum: Get trend line intercept", U"PowerCepstrum: Get trend line intercept...") {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double result;
		PowerCepstrum_fitTrendLine (me, fromQuefrency_trendLine, toQuefrency_trendLine, nullptr, & result, lineType, fitMethod);
	NUMBER_ONE_END (U" dB")
}

FORM (REAL_PowerCepstrum_getPeakProminence, U"PowerCepstrum: Get peak prominence", U"PowerCepstrum: Get peak prominence...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	NUMBER_ONE (PowerCepstrum)
		double qpeak;
		const double result = PowerCepstrum_getPeakProminence (me, fromPitch, toPitch, peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod, & qpeak);
	NUMBER_ONE_END (U" dB; quefrency=", qpeak, U" s (f=", 1.0 / qpeak, U" Hz).");
}

FORM (MODIFY_PowerCepstrum_subtractTrend_inplace, U"PowerCepstrum: Subtract trend (in-place)", U"PowerCepstrum: Subtract trend...") {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05)")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	MODIFY_EACH (PowerCepstrum)
		PowerCepstrum_subtractTrend_inplace (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	MODIFY_EACH_END
}

FORM (MODIFY_PowerCepstrum_smooth_inplace, U"PowerCepstrum: Smooth (in-place)", U"PowerCepstrum: Smooth...") {
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	MODIFY_EACH (PowerCepstrum)
		PowerCepstrum_smooth_inplace (me, quefrencySmoothingWindowDuration, numberOfIterations);
	MODIFY_EACH_END
}

FORM (NEW_PowerCepstrum_smooth, U"PowerCepstrum: Smooth", U"PowerCepstrum: Smooth...") {
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	CONVERT_EACH (PowerCepstrum)
		autoPowerCepstrum result = PowerCepstrum_smooth (me, quefrencySmoothingWindowDuration, numberOfIterations);
	CONVERT_EACH_END (my name.get(), U"_smooth")
}

FORM (NEW_PowerCepstrum_subtractTrend, U"PowerCepstrum: Subtract trend", U"PowerCepstrum: Subtract trend...") {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	CONVERT_EACH (PowerCepstrum)
		autoPowerCepstrum result = PowerCepstrum_subtractTrend (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	CONVERT_EACH_END (my name.get(), U"_minusTrend")
}

DIRECT (NEW_Cepstrum_to_Spectrum) {
	CONVERT_EACH (Cepstrum)
		autoSpectrum result = Cepstrum_to_Spectrum (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_PowerCepstrum_to_Matrix) {
	CONVERT_EACH (PowerCepstrum)
		autoMatrix result = PowerCepstrum_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

/********************** Cepstrogram  ****************************************/

DIRECT (HELP_PowerCepstrogram_help) {
	HELP (U"PowerCepstrogram")
}

FORM (GRAPHICS_old_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (minimum_dB, U"Minimum (dB)", U"0.0")
	REAL (maximum_dB, U"Maximum (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (PowerCepstrogram)
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, false, maximum_dB - minimum_dB, 0.0, garnish);
    GRAPHICS_EACH_END
}

FORM (GRAPHICS_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", U"PowerCepstrogram: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (maximum_dB, U"Maximum (dB)", U"80.0")
	BOOLEAN (autoscaling, U"Autoscaling", false);
	REAL (dynamicRange_dB, U"Dynamic range (dB)", U"30.0");
	REAL (compression, U"Dynamic compression (0-1)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO_ALTERNATIVE (GRAPHICS_old_PowerCepstrogram_paint)
	GRAPHICS_EACH (PowerCepstrogram)
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, autoscaling, dynamicRange_dB, compression, garnish);
	GRAPHICS_EACH_END
}

FORM (NEW_PowerCepstrogram_smooth, U"PowerCepstrogram: Smooth", U"PowerCepstrogram: Smooth...") {
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	OK
DO
	CONVERT_EACH (PowerCepstrogram)
		autoPowerCepstrogram result = PowerCepstrogram_smooth (me, smoothingWindowDuration, quefrencySmoothingWindowDuration);
	CONVERT_EACH_END (my name.get(), U"_smoothed")
}

DIRECT (REAL_PowerCepstrogram_getStartQuefrency) {
	NUMBER_ONE (PowerCepstrogram)
		const double result = my ymin;
	NUMBER_ONE_END (U" (s)")
}

DIRECT (REAL_PowerCepstrogram_getEndQuefrency) {
	NUMBER_ONE (PowerCepstrogram)
		const double result = my ymax;
	NUMBER_ONE_END (U" (s)")
}

DIRECT (INTEGER_PowerCepstrogram_getNumberOfQuefrencyBins) {
	INTEGER_ONE (PowerCepstrogram)
		const integer result = my ny;
	INTEGER_ONE_END (U" quefrency bins")
}

DIRECT (REAL_PowerCepstrogram_getQuefrencyStep) {
	NUMBER_ONE (PowerCepstrogram)
		const double result = my dy;
	NUMBER_ONE_END (U" s (quefrency step)")
}

FORM (NEW_PowerCepstrogram_subtractTrend, U"PowerCepstrogram: Subtract trend", nullptr) {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05)")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	CONVERT_EACH (PowerCepstrogram)
		autoPowerCepstrogram result = PowerCepstrogram_subtractTrend (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	CONVERT_EACH_END (my name.get(), U"_minusTrend")
}

FORM (MODIFY_PowerCepstrogram_subtractTrend_inplace, U"PowerCepstrogram: Subtract trend (in-place)", nullptr) {
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05)")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	MODIFY_EACH (PowerCepstrogram)
		PowerCepstrogram_subtractTrend_inplace (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	MODIFY_EACH_END
}

FORM (REAL_PowerCepstrogram_getCPPS_hillenbrand, U"PowerCepstrogram: Get CPPS", nullptr) {
	LABEL (U"Smoothing:")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", true)
	REAL (smoothinWindowDuration, U"Time averaging window (s)", U"0.001")
	REAL (quefrencySmoothinWindowDuration, U"Quefrency averaging window (s)", U"0.00005")
	LABEL (U"Peak search:")
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	NUMBER_ONE (PowerCepstrogram)
		const double result = PowerCepstrogram_getCPPS_hillenbrand (me, subtractTrendBeforeSmoothing, smoothinWindowDuration, quefrencySmoothinWindowDuration, fromPitch, toPitch);
	NUMBER_ONE_END (U" dB")
}

FORM (REAL_PowerCepstrogram_getCPPS, U"PowerCepstrogram: Get CPPS", U"PowerCepstrogram: Get CPPS...") {
	LABEL (U"Smoothing of the Cepstrogram")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", true)
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	LABEL (U"Peak search:")
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	LABEL (U"Trend line:")
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	NUMBER_ONE (PowerCepstrogram)
		const double result = PowerCepstrogram_getCPPS (me, subtractTrendBeforeSmoothing, smoothingWindowDuration, quefrencySmoothingWindowDuration, fromPitch, toPitch, tolerance, peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	NUMBER_ONE_END (U" dB");
}

FORM (MODIFY_PowerCepstrogram_formula, U"PowerCepstrogram: Formula", nullptr) {
	LABEL (U"Do for all times and quefrencies:")
	LABEL (U"   `x` is the time in seconds")
	LABEL (U"   `y` is the quefrency in seconds")
	LABEL (U"   `self` is the current value")
	TEXTFIELD (formula, U"   Replace all values with:", U"sqrt(self)")
	OK
DO
	MODIFY_EACH_WEAK (PowerCepstrogram)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (NEW_PowerCepstrogram_to_PowerCepstrum_slice, U"PowerCepstrogram: To PowerCepstrum (slice)", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	CONVERT_EACH (PowerCepstrogram)
		autoPowerCepstrum result = PowerCepstrogram_to_PowerCepstrum_slice (me, time);
	CONVERT_EACH_END (my name.get(), NUMstring_timeNoDot (time));
}

FORM (NEW_PowerCepstrogram_to_Table_cpp, U"PowerCepstrogram: To Table (peak prominence)", U"PowerCepstrogram: To Table (peak prominence)...") {
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Trend line quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Trend line quefrency range (s)", U"0.05)")
	OPTIONMENU_ENUM (kCepstrumTrendType, lineType, U"Trend type", kCepstrumTrendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrumTrendFit, fitMethod, U"Fit method", kCepstrumTrendFit::DEFAULT)
	OK
DO
	CONVERT_EACH (PowerCepstrogram)
		autoTable result = PowerCepstrogram_to_Table_cpp (me, fromPitch, toPitch, tolerance, peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	CONVERT_EACH_END (my name.get(), U"_cpp");
}

FORM (NEW_PowerCepstrogram_to_Table_hillenbrand, U"PowerCepstrogram: To Table (hillenbrand)", U"PowerCepstrogram: To Table (peak prominence...") {
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	CONVERT_EACH (PowerCepstrogram)
		autoTable result = PowerCepstrogram_to_Table_hillenbrand (me,fromPitch, toPitch);
	CONVERT_EACH_END (my name.get(), U"_cpp")
}

DIRECT (NEW_PowerCepstrogram_to_Matrix) {
	CONVERT_EACH (PowerCepstrogram)
		autoMatrix result = PowerCepstrogram_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

/********************** Cepstrumc  ****************************************/

DIRECT (NEW_Cepstrumc_to_LPC) {
	CONVERT_EACH (Cepstrumc)
		autoLPC result = Cepstrumc_to_LPC (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW1_Cepstrumc_to_DTW, U"Cepstrumc: To DTW", U"Cepstrumc: To DTW...") {
	LABEL (U"Distance calculation between Cepstra")
	REAL (cepstralWeight, U"Cepstral weight", U"1.0")
	REAL (logEnergyWeight, U"Log energy weight", U"0.0")
	REAL (regressionWeight, U"Regression weight", U"0.0")
	REAL (regressionLogEnergyWeight, U"Regression weight log energy", U"0.0")
	REAL (windowDuration, U"Window for regression coefficients (seconds)", U"0.056")
	LABEL (U"Boundary conditions for time warp")
	BOOLEAN (matchBeginPositions, U"Match begin positions", false)
	BOOLEAN (matchEndPositions, U"Match end positions", false)
	RADIO (slopeConstraintType, U"Slope constraints", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
	OK
DO
	CONVERT_COUPLE (Cepstrumc)
		autoDTW result = Cepstrumc_to_DTW (me, you, cepstralWeight, logEnergyWeight, regressionWeight, regressionLogEnergyWeight, windowDuration, matchBeginPositions, matchEndPositions, slopeConstraintType);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

DIRECT (NEW_Cepstrumc_to_Matrix) {
	CONVERT_EACH (Cepstrumc)
		autoMatrix result = Cepstrumc_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

/******************** Formant ********************************************/

FORM (NEW_Formant_to_LPC, U"Formant: To LPC", nullptr) {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"16000.0")
	OK
DO
	CONVERT_EACH (Formant)
		autoLPC result = Formant_to_LPC (me, 1.0 / samplingFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_Formant_formula, U"Formant: Formula", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"5")
	LABEL (U"Formant frequencies in odd numbered rows")
	LABEL (U"Formant bandwidths in even numbered rows")
	SENTENCE (formula, U"Formula", U"if row mod 2 = 1 and self[row,col]/self[row+1,col] < 5 then 0 else self fi")
	OK
DO
	MODIFY_EACH_WEAK (Formant)
		Formant_formula (me, fromTime, toTime, fromFormant, toFormant, interpreter, formula);
	MODIFY_EACH_WEAK_END
}

/******************** Formant & Spectrogram ************************************/

FORM (NEW1_Formant_Spectrogram_to_IntensityTier, U"Formant & Spectrogram: To IntensityTier", U"Formant & Spectrogram: To IntensityTier...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_TWO (Formant, Spectrogram)
		autoIntensityTier result = Formant_Spectrogram_to_IntensityTier (me, you, formantNumber);
	CONVERT_TWO_END (my name.get(), U"_", formantNumber)
}

/********************LFCC ********************************************/

DIRECT (HELP_LFCC_help) {
	HELP (U"LFCC")
}

FORM (NEW_LFCC_to_LPC, U"LFCC: To LPC", U"LFCC: To LPC...") {
	INTEGER (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	Melder_require (numberOfCoefficients >= 0, U"Number of coefficients should not be less than zero.");
	CONVERT_EACH (LFCC)
		autoLPC result = LFCC_to_LPC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name.get());
}

/********************LineSpectralFrequencies ********************************************/

DIRECT (HELP_LineSpectralFrequencies_help) {
	HELP (U"LineSpectralFrequencies")
}

FORM (GRAPHICS_LineSpectralFrequencies_drawFrequencies, U"LineSpectralFrequencies: Draw frequencies", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LineSpectralFrequencies)
		LineSpectralFrequencies_drawFrequencies (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (INTEGER_LineSpectralFrequencies_getNumberOfFrequencies, U"LineSpectralFrequencies: Get number of frequencies", U"LineSpectralFrequencies: Get number of frequencies...") {
	NATURAL (frameNumber, U"Frame number", U"2")
	OK
DO
	INTEGER_ONE (LineSpectralFrequencies)
		my checkIndex (frameNumber);
		const integer result = my d_frames [frameNumber]. numberOfFrequencies;
	INTEGER_ONE_END (U" frequencies")
}

FORM (NUMVEC_LineSpectralFrequencies_listFrequenciesInFrame, U"LineSpectralFrequencies: List all frequencies in frame", U"") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	NUMVEC_ONE (LineSpectralFrequencies)
		autoVEC result = LineSpectralFrequencies_listFrequenciesInFrame (me, frameNumber);
	NUMVEC_ONE_END
}

DIRECT (NUMMAT_LineSpectralFrequencies_listAllFrequencies) {
	NUMMAT_ONE (LineSpectralFrequencies)
		autoMAT result = LineSpectralFrequencies_listAllFrequencies (me);
	NUMMAT_ONE_END	
}

DIRECT (NEW_LineSpectralFrequencies_to_LPC) {
	CONVERT_EACH (LineSpectralFrequencies)
		autoLPC result = LineSpectralFrequencies_to_LPC (me);
	CONVERT_EACH_END (my name.get());
}

/********************LPC ********************************************/

DIRECT (HELP_LPC_help) {
	HELP (U"LPC")
}

FORM (GRAPHICS_LPC_drawGain, U"LPC: Draw gain", U"LPC: Draw gain...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (minimumGain, U"Minimum gain", U"0.0")
	REAL (maximumGain, U"Maximum gain", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LPC)
		LPC_drawGain (me, GRAPHICS, fromTime, toTime, minimumGain, maximumGain, garnish);
	GRAPHICS_EACH_END
}

DIRECT (REAL_LPC_getSamplingInterval) {
	NUMBER_ONE (LPC)
		const double result = my samplingPeriod;
	NUMBER_ONE_END (U" s");
}

FORM (INTEGER_LPC_getNumberOfCoefficients, U"LPC: Get number of coefficients", U"LPC: Get number of coefficients...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	INTEGER_ONE (LPC)
		my checkIndex (frameNumber);
		const integer result = my d_frames [frameNumber]. nCoefficients;
	INTEGER_ONE_END (U" coefficients")
}

FORM (NUMVEC_LPC_listAllCoefficientsInFrame, U"", U"") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	NUMVEC_ONE (LPC)
		autoVEC result = LPC_listCoefficientsInFrame (me, frameNumber);
	NUMVEC_ONE_END
}

DIRECT (NUMMAT_LPC_listAllCoefficients) {
	NUMMAT_ONE (LPC)
		autoMAT result = LPC_listAllCoefficients (me);
	NUMMAT_ONE_END	
}

FORM (REAL_LPC_getGainInFrame, U"LPC: Get gain in frame", U"LPC: Get gain in frame...") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	NUMBER_ONE (LPC)
		double result = undefined;
		if (frameNumber > 0 && frameNumber <= my nx)
			result = my d_frames [frameNumber] .gain;
	NUMBER_ONE_END (U" gain in frame ", frameNumber)
}

DIRECT (NUMVEC_LPC_listAllGains) {
	NUMVEC_ONE (LPC)
		autoVEC result = LPC_listAllGains (me);
	NUMVEC_ONE_END
}

FORM (GRAPHICS_LPC_drawPoles, U"LPC: Draw poles", U"LPC: Draw poles...") {
	REAL (time, U"Time (seconds)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LPC)
		LPC_drawPoles (me, GRAPHICS, time, garnish);
	GRAPHICS_EACH_END
}

DIRECT (NEW_LPC_to_Formant) {
	CONVERT_EACH (LPC)
		autoFormant result = LPC_to_Formant (me, 50.0);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_LPC_to_Formant_keep_all) {
	CONVERT_EACH (LPC)
		autoFormant result = LPC_to_Formant (me, 0.0);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_LPC_to_LFCC, U"LPC: To LFCC", U"LPC: To LFCC...") {
	INTEGER (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	Melder_require (numberOfCoefficients >= 0, U"The number of coefficients should not be less than zero.");
	CONVERT_EACH (LPC)
		autoLFCC result = LPC_to_LFCC (me, numberOfCoefficients);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_LPC_to_LineSpectralFrequencies, U"LPC: To LineSpectralFrequencies", nullptr) {
	REAL (gridSize, U"Grid size", U"0.0")
	OK
DO
	CONVERT_EACH (LPC)
		autoLineSpectralFrequencies result = LPC_to_LineSpectralFrequencies (me, gridSize);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_LPC_to_Polynomial_slice, U"LPC: To Polynomial", U"LPC: To Polynomial (slice)...") {
	REAL (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH (LPC)
		autoPolynomial result = LPC_to_Polynomial (me, time);
	CONVERT_EACH_END (my name.get(), NUMstring_timeNoDot (time))
}

FORM (NEW_LPC_to_Spectrum_slice, U"LPC: To Spectrum", U"LPC: To Spectrum (slice)...") {
	REAL (time, U"Time (seconds)", U"0.0")
	REAL (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REAL (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (LPC)
		autoSpectrum result = LPC_to_Spectrum (me, time, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
	CONVERT_EACH_END (my name.get(), NUMstring_timeNoDot (time))
}

FORM (NEW_LPC_to_Spectrogram, U"LPC: To Spectrogram", U"LPC: To Spectrogram...") {
	REAL (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REAL (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (LPC)
		autoSpectrogram result = LPC_to_Spectrogram (me, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_LPC_to_VocalTract_slice_special, U"LPC: To VocalTract", U"LPC: To VocalTract (slice, special)...") {
	REAL (time, U"Time (s)", U"0.0")
	REAL (glottalDamping, U"Glottal damping", U"0.1")
	BOOLEAN (radiationDamping, U"Radiation damping", true)
	BOOLEAN (internalDamping, U"Internal damping", true)
	OK
DO
	CONVERT_EACH (LPC)
		autoVocalTract result = LPC_to_VocalTract_slice_special (me, time, glottalDamping, radiationDamping, internalDamping);
	CONVERT_EACH_END (my name.get(), NUMstring_timeNoDot (time))
}

FORM (NEW_LPC_to_VocalTract_slice, U"LPC: To VocalTract", U"LPC: To VocalTract (slice)...") {
	REAL (time, U"Time (s)", U"0.0")
	POSITIVE (length, U"Length (m)", U"0.17")
	OK
DO
	CONVERT_EACH (LPC)
		autoVocalTract result = LPC_to_VocalTract_slice (me, time, length);
	CONVERT_EACH_END (my name.get(), NUMstring_timeNoDot (time))
}

DIRECT (NEW_LPC_downto_Matrix_lpc) {
	CONVERT_EACH (LPC)
		autoMatrix result = LPC_downto_Matrix_lpc (me);
	CONVERT_EACH_END (my name.get(), U"_lpc")
}

DIRECT (NEW_LPC_downto_Matrix_rc) {
	CONVERT_EACH (LPC)
		autoMatrix result = LPC_downto_Matrix_rc (me);
	CONVERT_EACH_END (my name.get(), U"_rc");
}

DIRECT (NEW_LPC_downto_Matrix_area) {
	CONVERT_EACH (LPC)
		autoMatrix result = LPC_downto_Matrix_area (me);
	CONVERT_EACH_END (my name.get(), U"_area");
}

/********************** Sound *******************************************/

FORM (NEW_Sound_to_PowerCepstrogram, U"Sound: To PowerCepstrogram", U"Sound: To PowerCepstrogram...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (timeStep,U"Time step (s)", U"0.002")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50")
	OK
DO
	CONVERT_EACH (Sound)
		autoPowerCepstrogram result = Sound_to_PowerCepstrogram (me, pitchFloor, timeStep, maximumFrequency, preEmphasisFrequency);
	CONVERT_EACH_END (my name.get())
}


FORM (NEW_Sound_to_PowerCepstrogram_hillenbrand, U"Sound: To PowerCepstrogram (hillenbrand)", U"Sound: To PowerCepstrogram...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	OK
DO
	CONVERT_EACH (Sound)
		autoPowerCepstrogram result = Sound_to_PowerCepstrogram_hillenbrand (me, pitchFloor, timeStep);
	CONVERT_EACH_END (my name.get())
}
	
FORM (NEW_Sound_to_Formant_robust, U"Sound: To Formant (robust)", U"Sound: To Formant (robust)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormant result = Sound_to_Formant_robust (me, timeStep, maximumNumberOfFormants, middleCeiling, windowLength, preEmphasisFrequency, 50.0, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, 1);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_FormantPath, U"Sound: To FormantPath", nullptr) {
	REAL (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleFormantCeiling, U"Middle formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	OPTIONMENU_ENUM (kLPC_Analysis, lpcModel, U"LPC model", kLPC_Analysis::DEFAULT)
	LABEL (U"The maximum and minimum ceilings are determined as:")
	LABEL (U" middleFormantCeiling * exp(+/- ceilingStepSize * numberOfStepsToACeiling).")
	POSITIVE (ceilingStepSize, U"Ceiling step size", U"0.05")
	NATURAL (numberOfStepsToACeiling, U"Number of steps up / down", U"4")
	LABEL (U"For Marple analysis:")
	POSITIVE (marple_tol1, U"Tolerance 1", U"1e-6")
	POSITIVE (marple_tol2, U"Tolerance 2", U"1e-6")
	LABEL (U"For Robust analysis:")
	POSITIVE (huber_numberOfStdDev, U"Number of std. dev.", U"1.5")
	NATURAL (huber_maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (huber_tolerance, U"Tolerance", U"0.000001")
	BOOLEAN (sourcesAsMultichannel, U"Get sources as multi channel sound", false)
	OK
DO
	CONVERT_EACH (Sound)
		autoSound multichannel;
		autoFormantPath result = Sound_to_FormantPath_any (me, lpcModel, timeStep, maximumNumberOfFormants, middleFormantCeiling, windowLength, preEmphasisFrequency, ceilingStepSize, numberOfStepsToACeiling, marple_tol1, marple_tol2, huber_numberOfStdDev, huber_tolerance, huber_maximumNumberOfIterations,
			( sourcesAsMultichannel ? & multichannel : nullptr ));
		if (sourcesAsMultichannel)
			praat_new (multichannel.move(), my name.get(), U"_sources");
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_FormantPath_burg, U"Sound: To FormantPath (Burg method)", U"Sound: To FormantPath (burg)...") {
	REAL (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleFormantCeiling, U"Middle formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	LABEL (U"The maximum and minimum ceilings are determined as:")
	LABEL (U" middleCeiling * exp(+/- ceilingStepSize * numberOfStepsToACeiling).")
	POSITIVE (ceilingStepSize, U"Ceiling step size", U"0.05")
	NATURAL (numberOfStepsToACeiling, U"Number of steps up / down", U"4")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormantPath result = Sound_to_FormantPath_burg (me, timeStep, maximumNumberOfFormants, middleFormantCeiling, windowLength, preEmphasisFrequency, ceilingStepSize, numberOfStepsToACeiling);
	CONVERT_EACH_END (my name.get())	
}

#define Sound_to_LPC_addWarning \
	LABEL (U"Warning 1:  for formant analysis, use \"To Formant\" instead.") \
	LABEL (U"Warning 2:  if you do use \"To LPC\", you may want to resample first.") \
	LABEL (U"Click Help for more details.") \
	LABEL (U"")

FORM (NEW_Sound_to_LPC_autocorrelation, U"Sound: To LPC (autocorrelation)", U"Sound: To LPC (autocorrelation)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH (Sound)
		autoLPC result = Sound_to_LPC_autocorrelation (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_LPC_covariance, U"Sound: To LPC (covariance)", U"Sound: To LPC (covariance)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH (Sound)
		autoLPC result = Sound_to_LPC_covariance (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_LPC_burg, U"Sound: To LPC (burg)", U"Sound: To LPC (burg)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH (Sound)
		autoLPC result = Sound_to_LPC_burg (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_LPC_marple, U"Sound: To LPC (marple)", U"Sound: To LPC (marple)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (tolerance1, U"Tolerance 1", U"1e-6")
	POSITIVE (tolerance2, U"Tolerance 2", U"1e-6")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH (Sound)
		autoLPC result = Sound_to_LPC_marple (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency, tolerance1, tolerance2);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_MFCC, U"Sound: To MFCC", U"Sound: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"Filter bank parameters")
	POSITIVE (firstFilterFrequency, U"First filter frequency (mel)", U"100.0")
	POSITIVE (distancBetweenFilters, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	Melder_require (numberOfCoefficients < 25, U"The number of coefficients should be less than 25.");
	CONVERT_EACH (Sound)
		autoMFCC result = Sound_to_MFCC (me, numberOfCoefficients, windowLength, timeStep, firstFilterFrequency, maximumFrequency, distancBetweenFilters);
	CONVERT_EACH_END (my name.get())
}

FORM (GRAPHICS_VocalTract_drawSegments, U"VocalTract: Draw segments", nullptr) {
	POSITIVE (maximumLength, U"Maximum length (cm)", U"20.0")
	POSITIVE (maximumArea, U"Maximum area (cm^2)", U"90.0")
	BOOLEAN (glottisClosed, U"Closed at glottis", true)
	OK
DO
	GRAPHICS_EACH (VocalTract)
		VocalTract_drawSegments (me, GRAPHICS, maximumLength, maximumArea, glottisClosed);
	GRAPHICS_EACH_END
}

DIRECT (REAL_VocalTract_getLength) {
	NUMBER_ONE (VocalTract)
		double result = my xmax - my xmin;
	NUMBER_ONE_END (U" metres")
}

FORM (MODIFY_VocalTract_setLength, U"", nullptr) {
	POSITIVE (length, U"New length (m)", U"0.17")
	OK
DO
	MODIFY_EACH (VocalTract)
		VocalTract_setLength (me, length);
	MODIFY_EACH_END
}

FORM (NEW_VocalTract_to_VocalTractTier, U"VocalTract: To VocalTractTier", nullptr) {
	REAL (fromTime, U"Tier start time (s)", U"0.0")
	REAL (toTime, U"Tier end time (s)", U"1.0")
	REAL (time, U"Insert at time (s)", U"0.5")
	OK
DO
	Melder_require (fromTime < toTime, U"Your start time should be before your end time.");
	Melder_require (time >= fromTime && time <= toTime, U"Your insert time should be between your start and end times.");
	CONVERT_EACH (VocalTract)
		autoVocalTractTier result = VocalTract_to_VocalTractTier (me, fromTime, toTime, time);
	CONVERT_EACH_END (my name.get())
}

DIRECT (HELP_VocalTractTier_help) {
	HELP (U"VocalTractTier")
}

FORM (NEW_VocalTractTier_to_LPC, U"VocalTractTier: To LPC", nullptr) {
	POSITIVE (timeStep, U"Time step", U"0.005")
	OK
DO
	CONVERT_EACH (VocalTractTier)
		autoLPC result = VocalTractTier_to_LPC (me, timeStep);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_VocalTractTier_to_VocalTract, U"", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	CONVERT_EACH (VocalTractTier)
		autoVocalTract result = VocalTractTier_to_VocalTract (me, time);
	CONVERT_EACH_END (my name.get())
}

FORM (MODIFY_VocalTractTier_addVocalTract, U"VocalTractTier: Add VocalTract", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	MODIFY_FIRST_OF_TWO (VocalTractTier, VocalTract)
		VocalTractTier_addVocalTract (me, time, you);
	MODIFY_FIRST_OF_TWO_END
}

/******************* LPC & Sound *************************************/

FORM (NEW1_LPC_Sound_filter, U"LPC & Sound: Filter", U"LPC & Sound: Filter...") {
	BOOLEAN (useGain, U"Use LPC gain", false)
	OK
DO
	CONVERT_TWO (LPC, Sound)
		autoSound result = LPC_Sound_filter (me, you, useGain);
	CONVERT_TWO_END (my name.get())
}

FORM (NEW1_LPC_Sound_filterWithFilterAtTime, U"LPC & Sound: Filter with one filter at time", U"LPC & Sound: Filter with filter at time...") {
	OPTIONMENU (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REAL (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	CONVERT_TWO (LPC, Sound)
		autoSound result = LPC_Sound_filterWithFilterAtTime (me, you, channel - 1, time);
	CONVERT_TWO_END (my name.get())
}

DIRECT (NEW1_LPC_Sound_filterInverse) {
	CONVERT_TWO (LPC, Sound)
		autoSound result = LPC_Sound_filterInverse (me, you);
	CONVERT_TWO_END (my name.get())
}

FORM (NEW1_LPC_Sound_filterInverseWithFilterAtTime, U"LPC & Sound: Filter (inverse) with filter at time",
      U"LPC & Sound: Filter (inverse) with filter at time...") {
	OPTIONMENU (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REAL (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	CONVERT_TWO (LPC, Sound)
		autoSound result = LPC_Sound_filterInverseWithFilterAtTime (me, you, channel - 1, time);
	CONVERT_TWO_END (my name.get())
}

FORM (NEW1_LPC_Sound_to_LPC_robust, U"Robust LPC analysis", U"LPC & Sound: To LPC (robust)...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	BOOLEAN (locationVariable, U"Variable location", false)
	OK
DO
	CONVERT_TWO (LPC, Sound)
		autoLPC result = LPC_Sound_to_LPC_robust (me, you, windowLength, preEmphasisFrequency, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, locationVariable);
	CONVERT_TWO_END (my name.get(), U"_r");
}

extern void praat_TimeTier_query_init (ClassInfo klas);
extern void praat_TimeTier_modify_init (ClassInfo klas);
void praat_uvafon_LPC_init ();
void praat_uvafon_LPC_init () {
	Thing_recognizeClassesByName (classCepstrumc, classPowerCepstrum, classCepstrogram, classFormantPath, classFormantPathEditor, classPowerCepstrogram, classLPC, classLFCC, classLineSpectralFrequencies, classMFCC, classVocalTractTier, nullptr);
	
	structFormantPathEditor  :: f_preferences ();
	
	praat_addAction1 (classPowerCepstrum, 0, U"PowerCepstrum help", 0, 0, HELP_PowerCepstrum_help);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw...", 0, 0, GRAPHICS_PowerCepstrum_draw);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw trend line...", 0, 0, GRAPHICS_PowerCepstrum_drawTrendLine);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw tilt line...", 0, praat_DEPRECATED_2019, GRAPHICS_PowerCepstrum_drawTrendLine);
	praat_addAction1 (classCepstrum, 0, U"Draw (linear)...", 0, praat_HIDDEN, GRAPHICS_Cepstrum_drawLinear);
	praat_addAction1 (classCepstrum, 0, U"Down to PowerCepstrum", 0, 0, NEW_Cepstrum_downto_PowerCepstrum);
	
	praat_addAction1 (classPowerCepstrum, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak...", 0, 1, REAL_PowerCepstrum_getPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get quefrency of peak...", 0, 1, REAL_PowerCepstrum_getQuefrencyOfPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, REAL_PowerCepstrum_getPeakProminence_hillenbrand);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence...", 0, 1, REAL_PowerCepstrum_getPeakProminence);
		praat_addAction1 (classPowerCepstrum, 0, U"Get trend line slope...", 0, 1, REAL_PowerCepstrum_getTrendLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, U"Get trend line intercept...", 0, 1, REAL_PowerCepstrum_getTrendLineIntercept);
		praat_addAction1 (classPowerCepstrum, 0, U"Get rhamonics to noise ratio...", 0, 1, REAL_PowerCepstrum_getRNR);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line slope...", 0, praat_DEPRECATED_2019, REAL_PowerCepstrum_getTrendLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line intercept...", 0, praat_DEPRECATED_2019, REAL_PowerCepstrum_getTrendLineIntercept);
	praat_addAction1 (classPowerCepstrum, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Formula...", 0, 1, MODIFY_PowerCepstrum_formula);
		praat_addAction1 (classPowerCepstrum, 0, U"Subtract trend (in-place)...", 0, 1, MODIFY_PowerCepstrum_subtractTrend_inplace);
		praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt (in-line)...", 0, praat_DEPTH_1 + praat_DEPRECATED_2017, MODIFY_PowerCepstrum_subtractTrend_inplace);
		praat_addAction1 (classPowerCepstrum, 0, U"Smooth (in-place)...", 0, 1, MODIFY_PowerCepstrum_smooth_inplace);
		praat_addAction1 (classPowerCepstrum, 0, U"Smooth (in-line)...", 0, praat_DEPTH_1 + praat_DEPRECATED_2017, MODIFY_PowerCepstrum_smooth_inplace);

	praat_addAction1 (classPowerCepstrum, 0, U"Subtract trend...", 0, 0, NEW_PowerCepstrum_subtractTrend);
	praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt...", 0, praat_DEPRECATED_2019, NEW_PowerCepstrum_subtractTrend);
	praat_addAction1 (classPowerCepstrum, 0, U"Smooth...", 0, 0, NEW_PowerCepstrum_smooth);
	praat_addAction1 (classCepstrum, 0, U"To Spectrum", 0, praat_HIDDEN, NEW_Cepstrum_to_Spectrum);
	praat_addAction1 (classPowerCepstrum, 0, U"To Matrix", 0, 0, NEW_PowerCepstrum_to_Matrix);

	praat_addAction1 (classPowerCepstrogram, 0, U"PowerCepstrogram help", 0, 0, HELP_PowerCepstrogram_help);
	praat_addAction1 (classPowerCepstrogram, 0, U"Paint...", 0, 0, GRAPHICS_PowerCepstrogram_paint);
	praat_addAction1 (classPowerCepstrogram, 1, U"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency domain", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get start quefrency", 0, 2, REAL_PowerCepstrogram_getStartQuefrency);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get end quefrency", 0, 2, REAL_PowerCepstrogram_getEndQuefrency);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency sampling", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get number of quefrency bins", 0, 2, INTEGER_PowerCepstrogram_getNumberOfQuefrencyBins);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get quefrency step", 0, 2, REAL_PowerCepstrogram_getQuefrencyStep);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, REAL_PowerCepstrogram_getCPPS_hillenbrand);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS...", 0, 1, REAL_PowerCepstrogram_getCPPS);
	praat_addAction1 (classPowerCepstrogram, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 0, U"Formula...", 0, 1, MODIFY_PowerCepstrogram_formula);
		praat_addAction1 (classPowerCepstrogram, 0, U"Subtract trend (in-place)...", 0, 1, MODIFY_PowerCepstrogram_subtractTrend_inplace);
		praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt (in-place)...", 0, praat_DEPRECATED_2019, MODIFY_PowerCepstrogram_subtractTrend_inplace);
		praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt (in-line)...", 0, praat_DEPTH_1 + praat_DEPRECATED_2017, MODIFY_PowerCepstrogram_subtractTrend_inplace);
	praat_addAction1 (classPowerCepstrogram, 0, U"To PowerCepstrum (slice)...", 0, 0, NEW_PowerCepstrogram_to_PowerCepstrum_slice);
	praat_addAction1 (classPowerCepstrogram, 0, U"Smooth...", 0, 0, NEW_PowerCepstrogram_smooth);
	praat_addAction1 (classPowerCepstrogram, 0, U"Subtract trend...", 0, 0, NEW_PowerCepstrogram_subtractTrend);
	praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt...", 0, praat_DEPRECATED_2019, NEW_PowerCepstrogram_subtractTrend);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (hillenbrand)...", 0, praat_HIDDEN, NEW_PowerCepstrogram_to_Table_hillenbrand);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (peak prominence)...", 0, praat_HIDDEN, NEW_PowerCepstrogram_to_Table_cpp);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Matrix", 0, 0, NEW_PowerCepstrogram_to_Matrix);

	praat_addAction1 (classCepstrumc, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To LPC", 0, 0, NEW_Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, U"To DTW...", 0, 0, NEW1_Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, U"Hack", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To Matrix", 0, 0, NEW_Cepstrumc_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classFormant, 0, U"To LPC...", 0, 0, NEW_Formant_to_LPC);
	praat_addAction1 (classFormant, 0, U"Formula...", U"Formula (bandwidths)...", 1, MODIFY_Formant_formula);
	praat_addAction2 (classFormant, 1, classSpectrogram, 1, U"To IntensityTier...", 0, 0, NEW1_Formant_Spectrogram_to_IntensityTier);
	
	praat_addAction1 (classFormantPath, 1, U"View & Edit alone", 0, 0, WINDOW_FormantPath_viewAndEditAlone);
	praat_addAction1 (classFormantPath, 1, U"View & Edit with Sound?", 0, 0, HINT_FormantPath_Sound_viewAndEdit);
	praat_addAction1 (classFormantPath, 1, U"Draw as grid...", 0, 0, GRAPHICS_FormantPath_drawAsGrid);	
	praat_addAction1 (classFormantPath, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classFormantPath, 0, U"Extract Formant", 0, 0, NEW_FormantPath_extractFormant);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (stress)...", 0, 0, NEW_FormantPath_to_Matrix_stress);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (qsums)...", 0, 0, NEW_FormantPath_to_Matrix_qsums);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (transition)...", 0, 0, NEW_FormantPath_to_Matrix_transition);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (deltas)...", 0, 0, NEW_FormantPath_to_Matrix_deltas);
	praat_addAction1 (classFormantPath, 0, U"Path finder...", 0, 0, MODIFY_FormantPath_pathFinder);

	praat_addAction1 (classLFCC, 0, U"LFCC help", 0, 0, HELP_LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, U"To LPC...", 0, 0, NEW_LFCC_to_LPC);

	praat_addAction1 (classLineSpectralFrequencies, 0, U"LineSpectralFrequencies help", 0, 0, HELP_LineSpectralFrequencies_help);
	praat_addAction1 (classLineSpectralFrequencies, 0, U"Draw frequencies...", 0, 0, GRAPHICS_LineSpectralFrequencies_drawFrequencies);
	praat_addAction1 (classLineSpectralFrequencies, 0, QUERY_BUTTON, 0, 0, 0);
		praat_TimeFrameSampled_query_init (classLineSpectralFrequencies);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"Get number of frequencies...", 0, 1, INTEGER_LineSpectralFrequencies_getNumberOfFrequencies);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"List frequencies in frame...", 0, 1, NUMVEC_LineSpectralFrequencies_listFrequenciesInFrame);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"List all frequencies", 0, 1, NUMMAT_LineSpectralFrequencies_listAllFrequencies);

	praat_addAction1 (classLineSpectralFrequencies, 0, U"To LPC", 0, 0, NEW_LineSpectralFrequencies_to_LPC);
	
	praat_addAction1 (classLPC, 0, U"LPC help", 0, 0, HELP_LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classLPC, 0, U"Draw gain...", 0, 1, GRAPHICS_LPC_drawGain);
	praat_addAction1 (classLPC, 0, U"Draw poles...", 0, 1, GRAPHICS_LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, 0, 0, 0);
		praat_TimeFrameSampled_query_init (classLPC);
		praat_addAction1 (classLPC, 1, U"Get sampling interval", 0, 1, REAL_LPC_getSamplingInterval);
		praat_addAction1 (classLPC, 1, U"-- get coefficients --", nullptr, 1, nullptr);
		praat_addAction1 (classLPC, 1, U"Get number of coefficients...", 0, 1, INTEGER_LPC_getNumberOfCoefficients);
		praat_addAction1 (classLPC, 1, U"Get coefficients in frame...", 0, 1, NUMVEC_LPC_listAllCoefficientsInFrame);
		praat_addAction1 (classLPC, 1, U"List all coefficients", 0, 1, NUMMAT_LPC_listAllCoefficients);
		praat_addAction1 (classLPC, 1, U"-- get gain --", nullptr, 1, nullptr);
		praat_addAction1 (classLPC, 1, U"Get gain in frame...", 0, 1, REAL_LPC_getGainInFrame);
		praat_addAction1 (classLPC, 1, U"List all gains", 0, 1, NUMVEC_LPC_listAllGains);
	praat_addAction1 (classLPC, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_TimeFunction_modify_init (classLPC);
	praat_addAction1 (classLPC, 0, U"Extract", 0, 0, 0);

	praat_addAction1 (classLPC, 0, U"To Spectrum (slice)...", 0, 0, NEW_LPC_to_Spectrum_slice);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice)...", 0, 0, NEW_LPC_to_VocalTract_slice);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice, special)...", 0, 0, NEW_LPC_to_VocalTract_slice_special);
	praat_addAction1 (classLPC, 0, U"To Polynomial (slice)...", 0, 0, NEW_LPC_to_Polynomial_slice);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (lpc)", 0, 0, NEW_LPC_downto_Matrix_lpc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (rc)", 0, praat_HIDDEN, NEW_LPC_downto_Matrix_rc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (area)", 0, praat_HIDDEN, NEW_LPC_downto_Matrix_area);
	praat_addAction1 (classLPC, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classLPC, 0, U"To Formant", 0, 0, NEW_LPC_to_Formant);
	praat_addAction1 (classLPC, 0, U"To Formant (keep all)", 0, 0, NEW_LPC_to_Formant_keep_all);
	praat_addAction1 (classLPC, 0, U"To LFCC...", 0, 0, NEW_LPC_to_LFCC);
	praat_addAction1 (classLPC, 0, U"To Spectrogram...", 0, 0, NEW_LPC_to_Spectrogram);
	praat_addAction1 (classLPC, 0, U"To LineSpectralFrequencies...", 0, 0, NEW_LPC_to_LineSpectralFrequencies);
	
	praat_addAction2 (classLPC, 1, classSound, 1, U"Analyse", 0, 0, 0);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter...", 0, 0, NEW1_LPC_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse)", 0, 0, NEW1_LPC_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, U"To LPC (robust)...", 0, praat_HIDDEN + praat_DEPTH_1, NEW1_LPC_Sound_to_LPC_robust);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter with filter at time...", 0, 0, NEW1_LPC_Sound_filterWithFilterAtTime);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse) with filter at time...", 0, 0, NEW1_LPC_Sound_filterInverseWithFilterAtTime);

	praat_addAction1 (classSound, 0, U"To PowerCepstrogram...", U"To Harmonicity (gne)...", 1, NEW_Sound_to_PowerCepstrogram);
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram (hillenbrand)...", U"To Harmonicity (gne)...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_PowerCepstrogram_hillenbrand);
	praat_addAction1 (classSound, 0, U"To Formant (robust)...", U"To Formant (sl)...", 2, NEW_Sound_to_Formant_robust);
	praat_addAction1 (classSound, 0, U"To FormantPath...", U"To Formant (robust)...", 2, NEW_Sound_to_FormantPath);
	praat_addAction1 (classSound, 0, U"To FormantPath (burg)...", U"To FormantPath...", 1, NEW_Sound_to_FormantPath_burg);
	praat_addAction1 (classSound, 0, U"To LPC", U"To FormantPath...", 1, nullptr);
	praat_addAction1 (classSound, 0, U"To LPC (autocorrelation)...", U"To LPC", 2, NEW_Sound_to_LPC_autocorrelation);
	praat_addAction1 (classSound, 0, U"To LPC (covariance)...", U"To LPC (autocorrelation)...", 2, NEW_Sound_to_LPC_covariance);
	praat_addAction1 (classSound, 0, U"To LPC (burg)...", U"To LPC (covariance)...", 2, NEW_Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, U"To LPC (marple)...", U"To LPC (burg)...", 2, NEW_Sound_to_LPC_marple);
	praat_addAction1 (classSound, 0, U"To MFCC...", U"To LPC (marple)...", 1, NEW_Sound_to_MFCC);
	praat_addAction2 (classSound, 1, classFormantPath, 1, U"View & Edit", 0, 0, WINDOW_Sound_FormantPath_createFormantPathEditor);
	praat_addAction3 (classSound, 1, classTextGrid, 1, classFormantPath, 1, U"View & Edit", 0, 0, WINDOW_Sound_TextGrid_FormantPath_createFormantPathEditor);
	
	praat_addAction1 (classVocalTract, 0, U"Draw segments...", U"Draw", 0, GRAPHICS_VocalTract_drawSegments);
	praat_addAction1 (classVocalTract, 1, U"Get length", U"Draw segments...", 0, REAL_VocalTract_getLength);
	praat_addAction1 (classVocalTract, 1, U"Set length", U"Formula...", 0, MODIFY_VocalTract_setLength);
	praat_addAction1 (classVocalTract, 0, U"To VocalTractTier...", U"To Spectrum...", 0, NEW_VocalTract_to_VocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"VocalTractTier help", 0, 0, HELP_VocalTractTier_help);
	praat_addAction1 (classVocalTractTier, 0, U"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"To LPC...", 0, 0, NEW_VocalTractTier_to_LPC);
	praat_addAction1 (classVocalTractTier, 0, U"To VocalTract...", 0, 0, NEW_VocalTractTier_to_VocalTract);
	praat_addAction2 (classVocalTractTier, 1, classVocalTract, 1, U"Add VocalTract...", 0, 0, MODIFY_VocalTractTier_addVocalTract);

	INCLUDE_MANPAGES (manual_LPC)
	INCLUDE_MANPAGES (manual_DataModeler)

	INCLUDE_LIBRARY (praat_DataModeler_init)
}

/* End of file praat_LPC_init.c */
