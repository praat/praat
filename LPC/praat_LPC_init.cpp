/* praat_LPC_init.cpp
 *
 * Copyright (C) 1994-2016 David Weenink
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

#include <math.h>
#include "Cepstrumc.h"
#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "DTW.h"
#include "FilterBank.h"
#include "Formant_extensions.h"
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
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "VocalTractTier.h"

#include "praat_TimeFunction.h"
#include "praat_Matrix.h"

#undef iam
#define iam iam_LOOP

#define praat_Quefrency_RANGE(fromQuefrency,toQuefrency) \
	REALVAR (fromQuefrency, U"left Quefrency range (s)", U"0.0") \
	REALVAR (toQuefrency, U"right Quefrency range (s)", U"0.0 (= all)")

static const char32 *DRAW_BUTTON    = U"Draw -";
static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *MODIFY_BUTTON   = U"Modify -";

void praat_CC_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);

/********************** Cepstrum  ****************************************/

DIRECT (NEW_Cepstrum_downto_PowerCepstrum) {
	LOOP {
		iam (Cepstrum);
		autoPowerCepstrum thee = Cepstrum_downto_PowerCepstrum (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (HELP_PowerCepstrum_help) {
	Melder_help (U"PowerCepstrum");
END }

FORM (GRAPHICS_Cepstrum_drawLinear, U"Cepstrum: Draw linear", U"Cepstrum: Draw (linear)...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REALVAR (ymin, U"Minimum", U"0.0")
	REALVAR (ymax, U"Maximum", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Cepstrum);
		Cepstrum_drawLinear (me, GRAPHICS, fromQuefrency, toQuefrency, ymin, ymax, garnish);
	}
END }

FORM (GRAPHICS_PowerCepstrum_draw, U"PowerCepstrum: Draw", U"PowerCepstrum: Draw...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REALVAR (ymin, U"Minimum (dB)", U"0.0")
	REALVAR (ymax, U"Maximum (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_draw (me, GRAPHICS, fromQuefrency, toQuefrency, ymin, ymax, garnish);
	}
END }

FORM (GRAPHICS_PowerCepstrum_drawTiltLine, U"PowerCepstrum: Draw tilt line", U"PowerCepstrum: Draw tilt line...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REALVAR (fromAmplitude_dB, U"left Amplitude range (dB)", U"0.0")
	REALVAR (toAmplitude_dB, U"right Amplitude range (dB)", U"0.0")
	LABEL (U"", U"Parameters for the tilt line fit")
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_drawTiltLine (me, GRAPHICS, fromQuefrency, toQuefrency,fromAmplitude_dB,toAmplitude_dB, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
	}
END }

FORM (GRAPHICS_PowerCepstrum_formula, U"PowerCepstrum: Formula...", U"PowerCepstrum: Formula...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	praat_Matrix_formula (dia, interpreter);
END }

FORM (REAL_PowerCepstrum_getPeak, U"PowerCepstrum: Get peak", nullptr) {
	REALVAR (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIOVAR (interpolationMethod, U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, fromPitch, toPitch, interpolationMethod - 1, &peakdB, &quefrency);
		Melder_informationReal (peakdB, U" dB");
	}
END }

FORM (REAL_PowerCepstrum_getQuefrencyOfPeak, U"PowerCepstrum: Get quefrency of peak", nullptr) {
	REALVAR (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIOVAR (interpolationMethod, U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, fromPitch, toPitch, interpolationMethod - 1, &peakdB, &quefrency);
		double f = 1.0 / quefrency;
		Melder_information (quefrency, U" s (f =", f, U" Hz)");
	}
END }

FORM (REAL_PowerCepstrum_getRNR, U"PowerCepstrum: Get rhamonics to noise ration", nullptr) {
	REALVAR (fromPitch, U"left Pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Pitch range (Hz)", U"333.3")
	POSITIVEVAR (fractionalWIdth, U"Fractional width (0-1)", U"0.05")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double rnr = PowerCepstrum_getRNR (me, fromPitch, toPitch, fractionalWIdth);
		Melder_information (rnr, U" (rnr)");
	}
END }

FORM (REAL_PowerCepstrum_getPeakProminence_hillenbrand, U"PowerCepstrum: Get peak prominence (hillenbrand)", U"PowerCepstrum: Get peak prominence (hillenbrand)...") {
	REALVAR (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence_hillenbrand (me, fromPitch, toPitch, &qpeak);
		Melder_information (cpp, U" dB; quefrency=", qpeak, U" s (f=", 1.0 / qpeak, U" Hz).");
	}
END }

FORM (REAL_PowerCepstrum_getTiltLineSlope, U"PowerCepstrum: Get tilt line slope", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		PowerCepstrum_fitTiltLine (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, &a, &intercept, lineType, fitMethod);
		Melder_information (a, U" dB / ", lineType == 1 ? U"s" : U"ln (s)");
	}
END }


FORM (REAL_PowerCepstrum_getTiltLineIntercept, U"PowerCepstrum: Get tilt line intercept", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		PowerCepstrum_fitTiltLine (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, &a, &intercept, lineType, fitMethod);
		Melder_information (intercept, U" dB");
	}
END }

FORM (REAL_PowerCepstrum_getPeakProminence, U"PowerCepstrum: Get peak prominence", U"PowerCepstrum: Get peak prominence...") {
	REALVAR (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	RADIOVAR (interpolationMethod, U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence (me, fromPitch, toPitch, interpolationMethod - 1, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod, &qpeak);
		Melder_information (cpp, U" dB; quefrency=", qpeak, U" s (f=",
			1.0 / qpeak, U" Hz).");
	}
END }

FORM (MODIFY_PowerCepstrum_subtractTilt_inline, U"PowerCepstrum: Subtract tilt (in-line)", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_subtractTilt_inline (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
	}
END }

FORM (MODIFY_PowerCepstrum_smooth_inline, U"PowerCepstrum: Smooth (in-line)", nullptr) {
	REALVAR (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURALVAR (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_smooth_inline (me, quefrencySmoothingWindowDuration, numberOfIterations);
	}
END }

FORM (NEW_PowerCepstrum_smooth, U"PowerCepstrum: Smooth", nullptr) {
	REALVAR (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURALVAR (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_smooth (me, quefrencySmoothingWindowDuration, numberOfIterations);
		praat_new (thee.move(), my name, U"_smooth");
	}
END }

FORM (NEW_PowerCepstrum_subtractTilt, U"PowerCepstrum: Subtract tilt", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 1)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_subtractTilt (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
		praat_new (thee.move(), my name, U"minusTilt");
	}
END }

DIRECT (NEW_Cepstrum_to_Spectrum) {
	LOOP {
		iam (Cepstrum);
		autoSpectrum thee = Cepstrum_to_Spectrum (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_PowerCepstrum_to_Matrix) {
	LOOP {
		iam (PowerCepstrum);
		autoMatrix thee = PowerCepstrum_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

/********************** Cepstrogram  ****************************************/

DIRECT (HELP_PowerCepstrogram_help) {
	Melder_help (U"PowerCepstrogram");
END }

FORM (GRAPHICS_old_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REALVAR (minimum_dB, U"Minimum (dB)", U"0.0")
	REALVAR (maximum_dB, U"Maximum (dB)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, false, maximum_dB - minimum_dB, 0.0, garnish);
        }
END }

FORM (GRAPHICS_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", U"PowerCepstrogram: Paint...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REALVAR (maximum_dB, U"Maximum (dB)", U"80.0")
	BOOLEANVAR (autoscaling, U"Autoscaling", false);
	REALVAR (dynamicRange_dB, U"Dynamic range (dB)", U"30.0");
	REALVAR (compression, U"Dynamic compression (0-1)", U"0.0");
	BOOLEANVAR (garnish, U"Garnish", true);
	OK
DO_ALTERNATIVE (GRAPHICS_old_PowerCepstrogram_paint)
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, autoscaling, dynamicRange_dB, compression, garnish);
	}
END }

FORM (NEW_PowerCepstrogram_smooth, U"PowerCepstrogram: Smooth", U"PowerCepstrogram: Smooth...") {
	REALVAR (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REALVAR (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_smooth (me, smoothingWindowDuration, quefrencySmoothingWindowDuration);
		praat_new (thee.move(), my name, U"_smoothed");
	}
END }

DIRECT (REAL_PowerCepstrogram_getStartQuefrency) {
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymin, U" (s)");
	}
END }

DIRECT (REAL_PowerCepstrogram_getEndQuefrency) {
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymax, U" (s)");
	}
END }

DIRECT (INTEGER_PowerCepstrogram_getNumberOfQuefrencyBins) {
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ny, U" quefrency bins");
	}
END }

DIRECT (REAL_PowerCepstrogram_getQuefrencyStep) {
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my dy, U" quefrency step (s)");
	}
END }

FORM (NEW_PowerCepstrogram_subtractTilt, U"PowerCepstrogram: Subtract tilt", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 2)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_subtractTilt (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
		praat_new (thee.move(), my name, U"_minusTilt");
	}
END }

FORM (MODIFY_PowerCepstrogram_subtractTilt_inline, U"PowerCepstrogram: Subtract tilt (in-line)", nullptr) {
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 2)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_subtractTilt_inline (me, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
	}
END }

FORM (REAL_PowerCepstrogram_getCPPS_hillenbrand, U"PowerCepstrogram: Get CPPS", nullptr) {
	LABEL (U"", U"Smoothing:")
	BOOLEANVAR (subtractTiltBeforeSmoothing, U"Subtract tilt before smoothing", true)
	REALVAR (smoothinWindowDuration, U"Time averaging window (s)", U"0.001")
	REALVAR (quefrencySmoothinWindowDuration, U"Quefrency averaging window (s)", U"0.00005")
	LABEL (U"", U"Peak search:")
	REALVAR (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS_hillenbrand (me, subtractTiltBeforeSmoothing, smoothinWindowDuration, quefrencySmoothinWindowDuration, fromPitch, toPitch);
		Melder_informationReal (cpps, U" dB");
	}
END }


FORM (REAL_PowerCepstrogram_getCPPS, U"PowerCepstrogram: Get CPPS", nullptr) {
	LABEL (U"", U"Smoothing:")
	BOOLEANVAR (subtractTiltBeforeSmoothing, U"Subtract tilt before smoothing", true)
	REALVAR (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REALVAR (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	LABEL (U"", U"Peak search:")
	REALVAR (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVEVAR (tolerance, U"Tolerance (0-1)", U"0.05")
	RADIOVAR (interpolationMethod, U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
	LABEL (U"", U"Tilt line:")
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 2)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS (me, subtractTiltBeforeSmoothing, smoothingWindowDuration, quefrencySmoothingWindowDuration, fromPitch, toPitch, tolerance, interpolationMethod - 1, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
		Melder_informationReal (cpps, U" dB");
	}
END }

FORM (MODIFY_PowerCepstrogram_formula, U"PowerCepstrogram: Formula", nullptr) {
	LABEL (U"label", U"Do for all times and quefrencies:")
	LABEL (U"label", U"   `x' is the time in seconds")
	LABEL (U"label", U"   `y' is the quefrency in seconds")
	LABEL (U"label", U"   `self' is the current value")
	LABEL (U"label", U"   Replace all values with:")
	TEXTVAR (formula, U"formula", U"sqrt(self)")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		try {
			Matrix_formula ((Matrix) me, formula, interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PowerCepstrogram may have partially changed
			throw;
		}
	}
END }

FORM (NEW_PowerCepstrogram_to_PowerCepstrum_slice, U"PowerCepstrogram: To PowerCepstrum (slice)", nullptr) {
	REALVAR (time, U"Time (s)", U"0.1")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrum thee = PowerCepstrogram_to_PowerCepstrum_slice (me, time);
		praat_new (thee.move(), my name, NUMstring_timeNoDot (time));
	}
END }

FORM (NEW_PowerCepstrogram_to_Table_cpp, U"PowerCepstrogram: To Table (peak prominence)", U"PowerCepstrogram: To Table (peak prominence)...") {
	REALVAR (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVEVAR (tolerance, U"Tolerance (0-1)", U"0.05")
	RADIOVAR (interpolationMethod, U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
	REALVAR (fromQuefrency_tiltLine, U"left Tilt line quefrency range (s)", U"0.001")
	REALVAR (toQuefrency_tiltLine, U"right Tilt line quefrency range (s)", U"0.0 (= end)")
	OPTIONMENUVAR (lineType, U"Line type", 2)
		OPTION (U"Straight")
		OPTION (U"Exponential decay")
	OPTIONMENUVAR (fitMethod, U"Fit method", 2)
		OPTION (U"Least squares")
		OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_cpp (me, fromPitch, toPitch, tolerance, interpolationMethod - 1, fromQuefrency_tiltLine, toQuefrency_tiltLine, lineType, fitMethod);
		praat_new (thee.move(), my name, U"_cpp");
	}
END }

FORM (NEW_PowerCepstrogram_to_Table_hillenbrand, U"PowerCepstrogram: To Table (hillenbrand)", U"PowerCepstrogram: To Table (peak prominence...") {
	REALVAR (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REALVAR (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_hillenbrand (me,fromPitch, toPitch);
		praat_new (thee.move(), my name, U"_cpp");
	}
END }

DIRECT (NEW_PowerCepstrogram_to_Matrix) {
	LOOP {
		iam (PowerCepstrogram);
		autoMatrix thee = PowerCepstrogram_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

/********************** Cepstrumc  ****************************************/

DIRECT (NEW_Cepstrumc_to_LPC) {
	LOOP {
		iam (Cepstrumc);
		autoLPC thee = Cepstrumc_to_LPC (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Cepstrumc_to_DTW, U"Cepstrumc: To DTW", U"Cepstrumc: To DTW...") {
	LABEL (U"", U"Distance calculation between Cepstra")
	REALVAR (cepstralWeight, U"Cepstral weight", U"1.0")
	REALVAR (logEnergyWeight, U"Log energy weight", U"0.0")
	REALVAR (regressionWeight, U"Regression weight", U"0.0")
	REALVAR (regressionLogEnergyWeight, U"Regression weight log energy", U"0.0")
	REALVAR (windowDuration, U"Window for regression coefficients (seconds)", U"0.056")
	LABEL (U"", U"Boundary conditions for time warp")
	BOOLEANVAR (matchBeginPositions, U"Match begin positions", false)
	BOOLEANVAR (matchEndPositions, U"Match end positions", false)
	RADIOVAR (slopeConstraintType, U"Slope constraints", 1)
		RADIOBUTTON (U"no restriction")
		RADIOBUTTON (U"1/3 < slope < 3")
		RADIOBUTTON (U"1/2 < slope < 2")
		RADIOBUTTON (U"2/3 < slope < 3/2")
	OK
DO
	Cepstrumc c1 = nullptr, c2 = nullptr;
	LOOP {
		iam (Cepstrumc);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	autoDTW thee = Cepstrumc_to_DTW (c1, c2, cepstralWeight, logEnergyWeight, regressionWeight, regressionLogEnergyWeight, windowDuration, matchBeginPositions, matchEndPositions, slopeConstraintType);
	praat_new (thee.move(), c1 -> name, U"_", c2 -> name);
END }

DIRECT (NEW_Cepstrumc_to_Matrix) {
	LOOP {
		iam (Cepstrumc);
		autoMatrix thee = Cepstrumc_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END }

/******************** Formant ********************************************/

FORM (NEW_Formant_to_LPC, U"Formant: To LPC", nullptr) {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"16000.0")
	OK
DO
	LOOP {
		iam (Formant);
		autoLPC thee = Formant_to_LPC (me, 1.0 / samplingFrequency);
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_Formant_formula, U"Formant: Formula", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	NATURALVAR (fromFormant, U"left Formant range", U"1")
	NATURALVAR (toFormant, U"right Formant range", U"5")
	LABEL (U"", U"Formant frequencies in odd numbered rows")
	LABEL (U"", U"Formant bandwidths in even numbered rows")
	SENTENCEVAR (formula, U"Formula", U"if row mod 2 = 1 and self[row,col]/self[row+1,col] < 5 then 0 else self fi")
	OK
DO
	LOOP {
		iam (Formant);
		Formant_formula (me, fromTime, toTime, fromFormant, toFormant, interpreter, formula);
	}
END }

/******************** Formant & Spectrogram ************************************/

FORM (NEW1_Formant_and_Spectrogram_to_IntensityTier, U"Formant & Spectrogram: To IntensityTier", U"Formant & Spectrogram: To IntensityTier...") {
	NATURALVAR (formantNumber, U"Formant number", U"1")
	OK
DO
	Formant me = FIRST (Formant);
	Spectrogram thee = FIRST (Spectrogram);
	autoIntensityTier him = Formant_and_Spectrogram_to_IntensityTier (me, thee, formantNumber);
	praat_new (him.move(), my name, U"_", formantNumber);
END }

/********************LFCC ********************************************/

DIRECT (HELP_LFCC_help) {
	Melder_help (U"LFCC");
END }

FORM (NEW_LFCC_to_LPC, U"LFCC: To LPC", U"LFCC: To LPC...") {
	INTEGERVAR (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	if (numberOfCoefficients < 0) {
		Melder_throw (U"Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LFCC);
		autoLPC thee = LFCC_to_LPC (me, numberOfCoefficients);
		praat_new (thee.move(), my name);
	}
END }

/********************LineSpectralFrequencies ********************************************/

DIRECT (HELP_LineSpectralFrequencies_help) {
	Melder_help (U"LineSpectralFrequencies"); 
END }

FORM (GRAPHICS_LineSpectralFrequencies_drawFrequencies, U"LineSpectralFrequencies: Draw frequencies", nullptr) {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REALVAR (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LineSpectralFrequencies);
		LineSpectralFrequencies_drawFrequencies (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish);
	}
END }

DIRECT (NEW_LineSpectralFrequencies_to_LPC) {
	LOOP {
		iam (LineSpectralFrequencies);
		autoLPC thee = LineSpectralFrequencies_to_LPC (me);
		praat_new (thee.move(), my name);
	}
END }

/********************LPC ********************************************/

DIRECT (HELP_LPC_help) {
	Melder_help (U"LPC");
END }

FORM (GRAPHICS_LPC_drawGain, U"LPC: Draw gain", U"LPC: Draw gain...") {
	praat_TimeFunction_RANGE(fromTime,toTime)
	REALVAR (minimumGain, U"Minimum gain", U"0.0")
	REALVAR (maximumGain, U"Maximum gain", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawGain (me, GRAPHICS, fromTime, toTime, minimumGain, maximumGain, garnish);
	}
END }

DIRECT (REAL_LPC_getSamplingInterval) {
	LOOP {
		iam (LPC);
		Melder_information (my samplingPeriod, U" seconds");
	}
END }

FORM (INTEGER_LPC_getNumberOfCoefficients, U"LPC: Get number of coefficients", U"LPC: Get number of coefficients...") {
	NATURALVAR (frameNumber, U"Frame number", U"1")
	OK
DO
	LOOP {
		iam (LPC);
		if (frameNumber > my nx) {
			Melder_throw (U"Frame number is too large.\n\nPlease choose a number between 1 and ", my nx);
		}
		Melder_information (my d_frames[frameNumber].nCoefficients, U" coefficients");
	}
END }

FORM (GRAPHICS_LPC_drawPoles, U"LPC: Draw poles", U"LPC: Draw poles...") {
	REALVAR (time, U"Time (seconds)", U"0.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawPoles (me, GRAPHICS, time, garnish);
	}
END }

DIRECT (NEW_LPC_to_Formant) {
	LOOP {
		iam (LPC);
		autoFormant thee = LPC_to_Formant (me, 50.0);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_LPC_to_Formant_keep_all) {
	LOOP {
		iam (LPC);
		autoFormant thee = LPC_to_Formant (me, 0.0);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_LPC_to_LFCC, U"LPC: To LFCC", U"LPC: To LFCC...") {
	INTEGERVAR (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	if (numberOfCoefficients < 0) {
		Melder_throw (U"Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LPC);
		autoLFCC thee = LPC_to_LFCC (me, numberOfCoefficients);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_LPC_to_LineSpectralFrequencies, U"LPC: To LineSpectralFrequencies", nullptr) {
	REALVAR (gridSize, U"Grid size", U"0.0")
	OK
DO
	LOOP {
		iam (LPC);
		autoLineSpectralFrequencies thee = LPC_to_LineSpectralFrequencies (me, gridSize);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_LPC_to_Polynomial, U"LPC: To Polynomial", U"LPC: To Polynomial (slice)...") {
	REALVAR (time, U"Time (s)", U"0.0")
	OK
DO
	LOOP {
		iam (LPC);
		autoPolynomial thee = LPC_to_Polynomial (me, time);
		praat_new (thee.move(), my name, NUMstring_timeNoDot (time));
	}
END }

FORM (NEW_LPC_to_Spectrum, U"LPC: To Spectrum", U"LPC: To Spectrum (slice)...") {
	REALVAR (time, U"Time (seconds)", U"0.0")
	REALVAR (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REALVAR (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REALVAR (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		autoSpectrum thee = LPC_to_Spectrum (me, time, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
		praat_new (thee.move(), my name, NUMstring_timeNoDot (time));
	}
END }

FORM (NEW_LPC_to_Spectrogram, U"LPC: To Spectrogram", U"LPC: To Spectrogram...") {
	REALVAR (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REALVAR (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REALVAR (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		autoSpectrogram thee = LPC_to_Spectrogram (me, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_LPC_to_VocalTract_special, U"LPC: To VocalTract", U"LPC: To VocalTract (slice, special)...") {
	REALVAR (time, U"Time (s)", U"0.0")
	REALVAR (glottalDamping, U"Glottal damping", U"0.1")
	BOOLEANVAR (radiationDamping, U"Radiation damping", true)
	BOOLEANVAR (internalDamping, U"Internal damping", true)
	OK
DO
	LOOP {
		iam (LPC);
		autoVocalTract thee = LPC_to_VocalTract (me, time, glottalDamping, radiationDamping, internalDamping);
		praat_new (thee.move(), my name, NUMstring_timeNoDot (time));
	}
END }

FORM (NEW_LPC_to_VocalTract, U"LPC: To VocalTract", U"LPC: To VocalTract (slice)...") {
	REALVAR (time, U"Time (s)", U"0.0")
	POSITIVEVAR (lenght, U"Length (m)", U"0.17")
	OK
DO
	LOOP {
		iam (LPC);
		autoVocalTract thee = LPC_to_VocalTract (me, time, lenght);
		praat_new (thee.move(), my name, NUMstring_timeNoDot (time));
	}
END }

DIRECT (NEW_LPC_downto_Matrix_lpc) {
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_lpc (me);
		praat_new (thee.move(), my name, U"_lpc");
	}
END }

DIRECT (NEW_LPC_downto_Matrix_rc) {
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_rc (me);
		praat_new (thee.move(), my name, U"_rc");
	}
END }

DIRECT (NEW_LPC_downto_Matrix_area) {
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_area (me);
		praat_new (thee.move(), my name, U"_area");
	}
END }

/********************** Sound *******************************************/

FORM (NEW_Sound_to_PowerCepstrogram, U"Sound: To PowerCepstrogram", U"Sound: To PowerCepstrogram...") {
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVEVAR (timeStep,U"Time step (s)", U"0.002")
	POSITIVEVAR (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram (me, pitchFloor, timeStep, maximumFrequency, preEmphasisFrequency);
		praat_new (thee.move(), my name);
	}
END }


FORM (NEW_Sound_to_PowerCepstrogram_hillenbrand, U"Sound: To PowerCepstrogram (hillenbrand)", U"Sound: To PowerCepstrogram...") {
	POSITIVEVAR (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.002")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram_hillenbrand (me, pitchFloor, timeStep);
		praat_new (thee.move(), my name);
	}
END }
	
FORM (NEW_Sound_to_Formant_robust, U"Sound: To Formant (robust)", U"Sound: To Formant (robust)...") {
	REALVAR (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVEVAR (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REALVAR (maximumFormantFrequency, U"Maximum formant (Hz)", U"5500 (= adult female)")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50")
	POSITIVEVAR (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REALVAR (tolerance, U"Tolerance", U"0.000001")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Formant_robust (me, timeStep, maximumNumberOfFormants, maximumFormantFrequency, windowLength, preEmphasisFrequency, 50.0, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, 1), my name);
	}
END }

static void Sound_to_LPC_addWarning (UiForm dia) {
	LABEL (U"", U"Warning 1:  for formant analysis, use \"To Formant\" instead.")
	LABEL (U"", U"Warning 2:  if you do use \"To LPC\", you may want to resample first.")
	LABEL (U"", U"Click Help for more details.")
	LABEL (U"", U"")
}

FORM (NEW_Sound_to_LPC_auto, U"Sound: To LPC (autocorrelation)", U"Sound: To LPC (autocorrelation)...") {
	Sound_to_LPC_addWarning (dia);
	NATURALVAR (predictionOrder, U"Prediction order", U"16")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	REALVAR (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
		autoLPC thee = Sound_to_LPC_auto (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_LPC_covar, U"Sound: To LPC (covariance)", U"Sound: To LPC (covariance)...") {
	Sound_to_LPC_addWarning (dia);
	NATURALVAR (predictionOrder, U"Prediction order", U"16")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	REALVAR (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
		autoLPC thee = Sound_to_LPC_covar (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_LPC_burg, U"Sound: To LPC (burg)", U"Sound: To LPC (burg)...") {
	Sound_to_LPC_addWarning (dia);
	NATURALVAR (predictionOrder, U"Prediction order", U"16")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	REALVAR (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
		autoLPC thee = Sound_to_LPC_burg (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_LPC_marple, U"Sound: To LPC (marple)", U"Sound: To LPC (marple)...") {
	Sound_to_LPC_addWarning (dia);
	NATURALVAR (predictionOrder, U"Prediction order", U"16")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	REALVAR (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVEVAR (tolerance1, U"Tolerance 1", U"1e-6")
	POSITIVEVAR (tolerance2, U"Tolerance 2", U"1e-6")
	OK
DO
	LOOP {
		iam (Sound);
		preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
		autoLPC thee = Sound_to_LPC_marple (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency, tolerance1, tolerance2);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Sound_to_MFCC, U"Sound: To MFCC", U"Sound: To MFCC...") {
	NATURALVAR (numberOfCoefficients, U"Number of coefficients", U"12")
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.015")
	POSITIVEVAR (timeStep, U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVEVAR (firstFilterFrequency, U"First filter frequency (mel)", U"100.0")
	POSITIVEVAR (distancBetweenFilters, U"Distance between filters (mel)", U"100.0")
	REALVAR (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	if (numberOfCoefficients > 24) {
		Melder_throw (U"Number of coefficients must be < 25.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_to_MFCC (me, numberOfCoefficients, windowLength, timeStep, firstFilterFrequency, maximumFrequency, distancBetweenFilters), my name);
	}
END }

FORM (GRAPHICS_VocalTract_drawSegments, U"VocalTract: Draw segments", nullptr) {
	POSITIVEVAR (maximumLength, U"Maximum length (cm)", U"20.0")
	POSITIVEVAR (maximumArea, U"Maximum area (cm^2)", U"90.0")
	BOOLEANVAR (glottisClosed, U"Closed at glottis", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (VocalTract);
		VocalTract_drawSegments (me, GRAPHICS, maximumLength, maximumArea, glottisClosed);
	}
END }

DIRECT (REAL_VocalTract_getLength) {
	LOOP {
		iam (VocalTract);
		Melder_information (my xmax - my xmin, U" m");
	}
END }

FORM (MODIFY_VocalTract_setLength, U"", nullptr) {
	POSITIVEVAR (length, U"New length (m)", U"0.17")
	OK
DO
	LOOP {
		iam (VocalTract);
		VocalTract_setLength (me, length);
	}
END }

FORM (NEW_VocalTract_to_VocalTractTier, U"VocalTract: To VocalTractTier", nullptr) {
	REALVAR (fromTime, U"Tier start time (s)", U"0.0")
	REALVAR (toTime, U"Tier end time (s)", U"1.0")
	REALVAR (time, U"Insert at time (s)", U"0.5")
	OK
DO
	REQUIRE (fromTime < toTime, U"The start time must be before the end time.")
	REQUIRE (time >= fromTime and time <= toTime, U"The insert time must be between start and end time.")
	LOOP {
		iam (VocalTract);
		autoVocalTractTier thee = VocalTract_to_VocalTractTier (me, fromTime, toTime, time);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (HELP_VocalTractTier_help) {
	Melder_help (U"VocalTractTier");
END }

FORM (NEW_VocalTractTier_to_LPC, U"VocalTractTier: To LPC", nullptr) {
	POSITIVEVAR (timeStep, U"Time step", U"0.005")
	OK
DO
	LOOP {
		iam (VocalTractTier);
		autoLPC thee = VocalTractTier_to_LPC (me, timeStep);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_VocalTractTier_to_VocalTract, U"", nullptr) {
	REALVAR (time, U"Time (s)", U"0.1")
	OK
DO
	LOOP {
		iam (VocalTractTier);
		autoVocalTract thee = VocalTractTier_to_VocalTract (me, time);
		praat_new (thee.move(), my name);
	}
END }

FORM (MODIFY_VocalTractTier_addVocalTract, U"VocalTractTier: Add VocalTract", nullptr) {
	REALVAR (time, U"Time (s)", U"0.1")
	OK
DO
	VocalTractTier me = FIRST (VocalTractTier);
	VocalTract thee = FIRST (VocalTract);
	VocalTractTier_addVocalTract_copy (me, time, thee);
	praat_dataChanged (me);
END }

/******************* LPC & Sound *************************************/

FORM (NEW1_LPC_and_Sound_filter, U"LPC & Sound: Filter", U"LPC & Sound: Filter...") {
	BOOLEANVAR (useGain, U"Use LPC gain", false)
	OK
DO
	LPC me = FIRST (LPC);
	Sound sound = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filter (me , sound, useGain);
	praat_new (thee.move(), my name);
END }

FORM (NEW1_LPC_and_Sound_filterWithFilterAtTime, U"LPC & Sound: Filter with one filter at time", U"LPC & Sound: Filter with filter at time...") {
	OPTIONMENUVAR (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REALVAR (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filterWithFilterAtTime (me , s, channel - 1, time);
	praat_new (thee.move(), my name);
END }

DIRECT (NEW1_LPC_and_Sound_filterInverse) {
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filterInverse (me , s);
	praat_new (thee.move(), my name);
END }

FORM (NEW1_LPC_and_Sound_filterInverseWithFilterAtTime, U"LPC & Sound: Filter (inverse) with filter at time",
      U"LPC & Sound: Filter (inverse) with filter at time...") {
	OPTIONMENUVAR (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REALVAR (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filterInverseWithFilterAtTime (me , s, channel - 1, time);
	praat_new (thee.move(), my name);
END }

FORM (NEW1_LPC_and_Sound_to_LPC_robust, U"Robust LPC analysis", U"LPC & Sound: To LPC (robust)...") {
	POSITIVEVAR (windowLength, U"Window length (s)", U"0.025")
	POSITIVEVAR (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVEVAR (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REALVAR (tolerance, U"Tolerance", U"0.000001")
	BOOLEANVAR (locationVariable, U"Variable location", false)
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	praat_new (LPC_and_Sound_to_LPC_robust (me, s, windowLength, preEmphasisFrequency, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, locationVariable), my name, U"_r");
END }

extern void praat_TimeTier_query_init (ClassInfo klas);
extern void praat_TimeTier_modify_init (ClassInfo klas);
void praat_uvafon_LPC_init ();
void praat_uvafon_LPC_init () {
	Thing_recognizeClassesByName (classCepstrumc, classPowerCepstrum, classCepstrogram, classPowerCepstrogram, classLPC, classLFCC, classLineSpectralFrequencies, classMFCC, classVocalTractTier, nullptr);

	praat_addAction1 (classPowerCepstrum, 0, U"PowerCepstrum help", 0, 0, HELP_PowerCepstrum_help);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw...", 0, 0, GRAPHICS_PowerCepstrum_draw);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw tilt line...", 0, 0, GRAPHICS_PowerCepstrum_drawTiltLine);
	praat_addAction1 (classCepstrum, 0, U"Draw (linear)...", 0, praat_HIDDEN, GRAPHICS_Cepstrum_drawLinear);
	praat_addAction1 (classCepstrum, 0, U"Down to PowerCepstrum", 0, 0, NEW_Cepstrum_downto_PowerCepstrum);
	
	praat_addAction1 (classPowerCepstrum, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak...", 0, 1, REAL_PowerCepstrum_getPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get quefrency of peak...", 0, 1, REAL_PowerCepstrum_getQuefrencyOfPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, REAL_PowerCepstrum_getPeakProminence_hillenbrand);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence...", 0, 1, REAL_PowerCepstrum_getPeakProminence);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line slope...", 0, 1, REAL_PowerCepstrum_getTiltLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line intercept...", 0, 1, REAL_PowerCepstrum_getTiltLineIntercept);
		praat_addAction1 (classPowerCepstrum, 0, U"Get rhamonics to noise ratio...", 0, 1, REAL_PowerCepstrum_getRNR);
	praat_addAction1 (classPowerCepstrum, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Formula...", 0, 1, GRAPHICS_PowerCepstrum_formula);
		praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt (in-line)...", 0, 1, MODIFY_PowerCepstrum_subtractTilt_inline);
		praat_addAction1 (classPowerCepstrum, 0, U"Smooth (in-line)...", 0, 1, MODIFY_PowerCepstrum_smooth_inline);

	praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt...", 0, 0, NEW_PowerCepstrum_subtractTilt);
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
		praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt (in-line)...", 0, 1, MODIFY_PowerCepstrogram_subtractTilt_inline);
	praat_addAction1 (classPowerCepstrogram, 0, U"To PowerCepstrum (slice)...", 0, 0, NEW_PowerCepstrogram_to_PowerCepstrum_slice);
	praat_addAction1 (classPowerCepstrogram, 0, U"Smooth...", 0, 0, NEW_PowerCepstrogram_smooth);
	praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt...", 0, 0, NEW_PowerCepstrogram_subtractTilt);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (hillenbrand)...", 0, praat_HIDDEN, NEW_PowerCepstrogram_to_Table_hillenbrand);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (peak prominence)...", 0, praat_HIDDEN, NEW_PowerCepstrogram_to_Table_cpp);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Matrix", 0, 0, NEW_PowerCepstrogram_to_Matrix);

	praat_addAction1 (classCepstrumc, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To LPC", 0, 0, NEW_Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, U"To DTW...", 0, 0, NEW_Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, U"Hack", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To Matrix", 0, 0, NEW_Cepstrumc_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classFormant, 0, U"To LPC...", 0, 0, NEW_Formant_to_LPC);
	praat_addAction1 (classFormant, 0, U"Formula...", U"Formula (bandwidths)...", 1, MODIFY_Formant_formula);
	praat_addAction2 (classFormant, 1, classSpectrogram, 1, U"To IntensityTier...", 0, 0, NEW1_Formant_and_Spectrogram_to_IntensityTier);

	
	
	praat_addAction1 (classLFCC, 0, U"LFCC help", 0, 0, HELP_LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, U"To LPC...", 0, 0, NEW_LFCC_to_LPC);

	praat_addAction1 (classLineSpectralFrequencies, 0, U"LineSpectralFrequencies help", 0, 0, HELP_LineSpectralFrequencies_help);
	praat_addAction1 (classLineSpectralFrequencies, 0, U"Draw frequencies...", 0, 0, GRAPHICS_LineSpectralFrequencies_drawFrequencies);
	praat_addAction1 (classLineSpectralFrequencies, 0, U"To LPC", 0, 0, NEW_LineSpectralFrequencies_to_LPC);
	
	praat_addAction1 (classLPC, 0, U"LPC help", 0, 0, HELP_LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classLPC, 0, U"Draw gain...", 0, 1, GRAPHICS_LPC_drawGain);
	praat_addAction1 (classLPC, 0, U"Draw poles...", 0, 1, GRAPHICS_LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, 0, 0, 0);
		praat_TimeFrameSampled_query_init (classLPC);
		praat_addAction1 (classLPC, 1, U"Get sampling interval", 0, 1, REAL_LPC_getSamplingInterval);
		praat_addAction1 (classLPC, 1, U"Get number of coefficients...", 0, 1, INTEGER_LPC_getNumberOfCoefficients);
	praat_addAction1 (classLPC, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_TimeFunction_modify_init (classLPC);
	praat_addAction1 (classLPC, 0, U"Extract", 0, 0, 0);

	praat_addAction1 (classLPC, 0, U"To Spectrum (slice)...", 0, 0, NEW_LPC_to_Spectrum);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice)...", 0, 0, NEW_LPC_to_VocalTract);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice, special)...", 0, 0, NEW_LPC_to_VocalTract_special);
	praat_addAction1 (classLPC, 0, U"To Polynomial (slice)...", 0, 0, NEW_LPC_to_Polynomial);
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
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter...", 0, 0, NEW1_LPC_and_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse)", 0, 0, NEW1_LPC_and_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, U"To LPC (robust)...", 0, praat_HIDDEN + praat_DEPTH_1, NEW1_LPC_and_Sound_to_LPC_robust);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter with filter at time...", 0, 0, NEW1_LPC_and_Sound_filterWithFilterAtTime);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse) with filter at time...", 0, 0, NEW1_LPC_and_Sound_filterInverseWithFilterAtTime);


	praat_addAction1 (classSound, 0, U"To LPC (autocorrelation)...", U"To Formant (sl)...", 1, NEW_Sound_to_LPC_auto);
	praat_addAction1 (classSound, 0, U"To LPC (covariance)...", U"To LPC (autocorrelation)...", 1, NEW_Sound_to_LPC_covar);
	praat_addAction1 (classSound, 0, U"To LPC (burg)...", U"To LPC (covariance)...", 1, NEW_Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, U"To LPC (marple)...", U"To LPC (burg)...", 1, NEW_Sound_to_LPC_marple);
	praat_addAction1 (classSound, 0, U"To MFCC...", U"To LPC (marple)...", 1, NEW_Sound_to_MFCC);
	praat_addAction1 (classSound, 0, U"To Formant (robust)...", U"To Formant (sl)...", 2, NEW_Sound_to_Formant_robust);
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram...", U"To Harmonicity (gne)...", 1, NEW_Sound_to_PowerCepstrogram);
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram (hillenbrand)...", U"To Harmonicity (gne)...", praat_HIDDEN + praat_DEPTH_1, NEW_Sound_to_PowerCepstrogram_hillenbrand);
	
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
