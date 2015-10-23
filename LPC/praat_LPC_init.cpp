/* praat_LPC_init.cpp
 *
 * Copyright (C) 1994-2014 David Weenink
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
#include "LPC_and_Polynomial.h"
#include "LPC_and_Tube.h"
#include "LPC_to_Spectrogram.h"
#include "LPC_to_Spectrum.h"
#include "NUM2.h"
#include "praatP.h"
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "VocalTractTier.h"

#undef iam
#define iam iam_LOOP

static const char32 *DRAW_BUTTON    = U"Draw -";
static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *MODIFY_BUTTON   = U"Modify -";

void praat_CC_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);
void praat_TimeFunction_modify_init (ClassInfo klas);
int praat_Fon_formula (UiForm dia, Interpreter interpreter);

/********************** Cepstrum  ****************************************/

DIRECT (Cepstrum_downto_PowerCepstrum)
	LOOP {
		iam (Cepstrum);
		autoPowerCepstrum thee = Cepstrum_downto_PowerCepstrum (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PowerCepstrum_help)
	Melder_help (U"PowerCepstrum");
END

FORM (Cepstrum_drawLinear, U"Cepstrum: Draw linear", U"Cepstrum: Draw (linear)...")
	REAL (U"left Quefrency range (s)", U"0.0")
	REAL (U"right Quefrency range (s)", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Cepstrum);
		Cepstrum_drawLinear (me, GRAPHICS, GET_REAL (U"left Quefrency range"), GET_REAL (U"right Quefrency range"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END

FORM (PowerCepstrum_draw, U"PowerCepstrum: Draw", U"PowerCepstrum: Draw...")
	REAL (U"left Quefrency range (s)", U"0.0")
	REAL (U"right Quefrency range (s)", U"0.0")
	REAL (U"Minimum (dB)", U"0.0")
	REAL (U"Maximum (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_draw (me, GRAPHICS, GET_REAL (U"left Quefrency range"), GET_REAL (U"right Quefrency range"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END

FORM (PowerCepstrum_drawTiltLine, U"PowerCepstrum: Draw tilt line", U"PowerCepstrum: Draw tilt line...")
	REAL (U"left Quefrency range (s)", U"0.0")
	REAL (U"right Quefrency range (s)", U"0.0")
	REAL (U"left Amplitude range (dB)", U"0.0")
	REAL (U"right Amplitude range (dB)", U"0.0")
	LABEL (U"", U"Parameters for the tilt line fit")
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_drawTiltLine (me, GRAPHICS, GET_REAL (U"left Quefrency range"), GET_REAL (U"right Quefrency range"),
			GET_REAL (U"left Amplitude range"), GET_REAL (U"right Amplitude range"),
			GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"), 
			GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
	}
END

FORM (PowerCepstrum_formula, U"PowerCepstrum: Formula...", U"PowerCepstrum: Formula...")
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

FORM (PowerCepstrum_getPeak, U"PowerCepstrum: Get peak", 0)
	REAL (U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO (U"Interpolation", 2)
	RADIOBUTTON (U"None")
	RADIOBUTTON (U"Parabolic")
	RADIOBUTTON (U"Cubic")
	RADIOBUTTON (U"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, GET_REAL (U"left Search peak in pitch range"), GET_REAL (U"right Search peak in pitch range"), GET_INTEGER (U"Interpolation") - 1, &peakdB, &quefrency);
		Melder_informationReal (peakdB, U" dB");
	}
END

FORM (PowerCepstrum_getQuefrencyOfPeak, U"PowerCepstrum: Get quefrency of peak", 0)
	REAL (U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO (U"Interpolation", 2)
	RADIOBUTTON (U"None")
	RADIOBUTTON (U"Parabolic")
	RADIOBUTTON (U"Cubic")
	RADIOBUTTON (U"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, GET_REAL (U"left Search peak in pitch range"), GET_REAL (U"right Search peak in pitch range"), GET_INTEGER (U"Interpolation") - 1, &peakdB, &quefrency);
		double f = 1.0 / quefrency;
		Melder_information (quefrency, U" s (f =", f, U" Hz)");
	}
END

FORM (PowerCepstrum_getRNR, U"PowerCepstrum: Get rhamonics to noise ration", 0)
	REAL (U"left Pitch range (Hz)", U"60.0")
	REAL (U"right Pitch range (Hz)", U"333.3")
	POSITIVE (U"Fractional width (0-1)", U"0.05")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double rnr = PowerCepstrum_getRNR (me, GET_REAL (U"left Pitch range"), GET_REAL (U"right Pitch range"), GET_REAL (U"Fractional width"));
		Melder_information (rnr, U" (rnr)");
	}
END

FORM (PowerCepstrum_getPeakProminence_hillenbrand, U"PowerCepstrum: Get peak prominence (hillenbrand)", U"PowerCepstrum: Get peak prominence (hillenbrand)...")
	REAL (U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (U"right Search peak in pitch range (Hz)", U"333.3")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence_hillenbrand (me,
			GET_REAL (U"left Search peak in pitch range"), GET_REAL (U"right Search peak in pitch range"), &qpeak);
		Melder_information (cpp, U" dB; quefrency=", qpeak, U" s (f=",
			1.0 / qpeak, U" Hz).");
	}
END

FORM (PowerCepstrum_getTiltLineSlope, U"PowerCepstrum: Get tilt line slope", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		int lineType = GET_INTEGER (U"Line type");
		PowerCepstrum_fitTiltLine (me, GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"), 
			  &a, &intercept, lineType, GET_INTEGER (U"Fit method"));
		Melder_information (a, U" dB / ", lineType == 1 ? U"s" : U"ln (s)");
	}
END


FORM (PowerCepstrum_getTiltLineIntercept, U"PowerCepstrum: Get tilt line intercept", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		int lineType = GET_INTEGER (U"Line type");
		PowerCepstrum_fitTiltLine (me, GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"), 
			  &a, &intercept, lineType, GET_INTEGER (U"Fit method"));
		Melder_information (intercept, U" dB");
	}
END

FORM (PowerCepstrum_getPeakProminence, U"PowerCepstrum: Get peak prominence", U"PowerCepstrum: Get peak prominence...")
	REAL (U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (U"right Search peak in pitch range (Hz)", U"333.3")
	RADIO (U"Interpolation", 2)
	RADIOBUTTON (U"None")
	RADIOBUTTON (U"Parabolic")
	RADIOBUTTON (U"Cubic")
	RADIOBUTTON (U"Sinc70")
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence (me,
			GET_REAL (U"left Search peak in pitch range"), GET_REAL (U"right Search peak in pitch range"),
			GET_INTEGER (U"Interpolation") - 1,
			GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"),
			GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"), &qpeak);
		Melder_information (cpp, U" dB; quefrency=", qpeak, U" s (f=",
			1.0 / qpeak, U" Hz).");
	}
END

FORM (PowerCepstrum_subtractTilt_inline, U"PowerCepstrum: Subtract tilt (in-line)", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_subtractTilt_inline (me, GET_REAL (U"left Tilt line quefrency range"), 
			GET_REAL (U"right Tilt line quefrency range"), GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
	}
END

FORM (PowerCepstrum_smooth_inline, U"PowerCepstrum: Smooth (in-line)", 0)
	REAL (U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (U"Number of iterations", U"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_smooth_inline (me, GET_REAL (U"Quefrency averaging window"), GET_INTEGER (U"Number of iterations"));
	}
END

FORM (PowerCepstrum_smooth, U"PowerCepstrum: Smooth", 0)
	REAL (U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (U"Number of iterations", U"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_smooth (me, GET_REAL (U"Quefrency averaging window"), GET_INTEGER (U"Number of iterations"));
		praat_new (thee.transfer(), my name, U"_smooth");
	}
END

FORM (PowerCepstrum_subtractTilt, U"PowerCepstrum: Subtract tilt", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 1)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_subtractTilt (me, GET_REAL (U"left Tilt line quefrency range"), 
			GET_REAL (U"right Tilt line quefrency range"), GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
		praat_new (thee.transfer(), my name, U"minusTilt");
	}
END

DIRECT (Cepstrum_to_Spectrum)
	LOOP {
		iam (Cepstrum);
		autoSpectrum thee = Cepstrum_to_Spectrum (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PowerCepstrum_to_Matrix)
	LOOP {
		iam (PowerCepstrum);
		autoMatrix thee = PowerCepstrum_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/********************** Cepstrogram  ****************************************/

DIRECT (PowerCepstrogram_help)
	Melder_help (U"PowerCepstrogram");
END

FORM (old_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", 0)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Quefrency range (s)", U"0.0")
	REAL (U"right Quefrency range (s)", U"0.0")
	REAL (U"Minimum (dB)", U"0.0")
	REAL (U"Maximum (dB)", U"0.0")
	BOOLEAN (U"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_paint (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Quefrency range"), GET_REAL (U"right Quefrency range"),
			GET_REAL (U"Maximum"), false, GET_REAL (U"Maximum") - GET_REAL (U"Minimum"),
			0.0, GET_INTEGER (U"Garnish"));
        }
END

FORM (PowerCepstrogram_paint, U"PowerCepstrogram: Paint", U"PowerCepstrogram: Paint...")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	REAL (U"left Quefrency range (s)", U"0.0")
	REAL (U"right Quefrency range (s)", U"0.0")
	REAL (U"Maximum (dB)", U"80.0")
	BOOLEAN (U"Autoscaling", 0);
	REAL (U"Dynamic range (dB)", U"30.0");
	REAL (U"Dynamic compression (0-1)", U"0.0");
	BOOLEAN (U"Garnish", 1);
	OK
DO_ALTERNATIVE (old_PowerCepstrogram_paint)
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_paint (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Quefrency range"), GET_REAL (U"right Quefrency range"),
		GET_REAL (U"Maximum"), GET_INTEGER (U"Autoscaling"), GET_REAL (U"Dynamic range"),
		GET_REAL (U"Dynamic compression"), GET_INTEGER (U"Garnish"));
	}
END

FORM (PowerCepstrogram_smooth, U"PowerCepstrogram: Smooth", U"PowerCepstrogram: Smooth...")
	REAL (U"Time averaging window (s)", U"0.02")
	REAL (U"Quefrency averaging window (s)", U"0.0005")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_smooth (me, GET_REAL (U"Time averaging window"), GET_REAL (U"Quefrency averaging window"));
		praat_new (thee.transfer(), my name, U"_smoothed");
	}
END

DIRECT (PowerCepstrogram_getStartQuefrency)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymin, U" (s)");
	}
END

DIRECT (PowerCepstrogram_getEndQuefrency)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymax, U" (s)");
	}
END

DIRECT (PowerCepstrogram_getNumberOfQuefrencyBins)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ny, U" quefrency bins");
	}
END

DIRECT (PowerCepstrogram_getQuefrencyStep)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my dy, U" quefrency step (s)");
	}
END

FORM (PowerCepstrogram_subtractTilt, U"PowerCepstrogram: Subtract tilt", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 2)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_subtractTilt (me, GET_REAL (U"left Tilt line quefrency range"), 
			GET_REAL (U"right Tilt line quefrency range"), GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
		praat_new (thee.transfer(), my name, U"_minusTilt");
	}
END

FORM (PowerCepstrogram_subtractTilt_inline, U"PowerCepstrogram: Subtract tilt (in-line)", 0)
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 2)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_subtractTilt_inline (me, GET_REAL (U"left Tilt line quefrency range"), 
			GET_REAL (U"right Tilt line quefrency range"), GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
	}
END

FORM (PowerCepstrogram_getCPPS_hillenbrand, U"PowerCepstrogram: Get CPPS", 0)
	LABEL (U"", U"Smoothing:")
	BOOLEAN (U"Subtract tilt before smoothing", 1)
	REAL (U"Time averaging window (s)", U"0.001")
	REAL (U"Quefrency averaging window (s)", U"0.00005")
	LABEL (U"", U"Peak search:")
	REAL (U"left Peak search pitch range (Hz)", U"60.0")
	REAL (U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS_hillenbrand (me, GET_INTEGER (U"Subtract tilt before smoothing"), GET_REAL (U"Time averaging window"), GET_REAL (U"Quefrency averaging window"),
			GET_REAL (U"left Peak search pitch range"), GET_REAL (U"right Peak search pitch range"));
		Melder_informationReal (cpps, U" dB");
	}
END


FORM (PowerCepstrogram_getCPPS, U"PowerCepstrogram: Get CPPS", 0)
	LABEL (U"", U"Smoothing:")
	BOOLEAN (U"Subtract tilt before smoothing", 1)
	REAL (U"Time averaging window (s)", U"0.001")
	REAL (U"Quefrency averaging window (s)", U"0.00005")
	LABEL (U"", U"Peak search:")
	REAL (U"left Peak search pitch range (Hz)", U"60.0")
	REAL (U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (U"Tolerance (0-1)", U"0.05")
	RADIO (U"Interpolation", 2)
	RADIOBUTTON (U"None")
	RADIOBUTTON (U"Parabolic")
	RADIOBUTTON (U"Cubic")
	RADIOBUTTON (U"Sinc70")
	LABEL (U"", U"Tilt line:")
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 2)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS (me, GET_INTEGER (U"Subtract tilt before smoothing"), GET_REAL (U"Time averaging window"), 
			GET_REAL (U"Quefrency averaging window"),
			GET_REAL (U"left Peak search pitch range"), GET_REAL (U"right Peak search pitch range"), GET_REAL (U"Tolerance"),
			GET_INTEGER (U"Interpolation") - 1, GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"),
			GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
		Melder_informationReal (cpps, U" dB");
	}
END

FORM (PowerCepstrogram_formula, U"PowerCepstrogram: Formula", U"")
	LABEL (U"label", U"Do for all times and quefrencies:")
	LABEL (U"label", U"   `x' is the time in seconds")
	LABEL (U"label", U"   `y' is the quefrency in seconds")
	LABEL (U"label", U"   `self' is the current value")
	LABEL (U"label", U"   Replace all values with:")
	TEXTFIELD (U"formula", U"sqrt(self)")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (U"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PowerCepstrogram may have partially changed
			throw;
		}
	}
END

FORM (PowerCepstrogram_to_PowerCepstrum_slice, U"PowerCepstrogram: To PowerCepstrum (slice)", 0)
	REAL (U"Time (s)", U"0.1")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double time = GET_REAL (U"Time");
		autoPowerCepstrum thee = PowerCepstrogram_to_PowerCepstrum_slice (me, time);
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

FORM (PowerCepstrogram_to_Table_cpp, U"PowerCepstrogram: To Table (peak prominence)", U"PowerCepstrogram: To Table (peak prominence)...")
	REAL (U"left Peak search pitch range (Hz)", U"60.0")
	REAL (U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (U"Tolerance (0-1)", U"0.05")
	RADIO (U"Interpolation", 2)
	RADIOBUTTON (U"None")
	RADIOBUTTON (U"Parabolic")
	RADIOBUTTON (U"Cubic")
	RADIOBUTTON (U"Sinc70")
	REAL (U"left Tilt line quefrency range (s)", U"0.001")
	REAL (U"right Tilt line quefrency range (s)", U"0.0 (=end)")
	OPTIONMENU (U"Line type", 2)
	OPTION (U"Straight")
	OPTION (U"Exponential decay")
	OPTIONMENU (U"Fit method", 2)
	OPTION (U"Least squares")
	OPTION (U"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_cpp (me,
			GET_REAL (U"left Peak search pitch range"), GET_REAL (U"right Peak search pitch range"), GET_REAL (U"Tolerance"),
			GET_INTEGER (U"Interpolation") - 1,
			GET_REAL (U"left Tilt line quefrency range"), GET_REAL (U"right Tilt line quefrency range"),
			GET_INTEGER (U"Line type"), GET_INTEGER (U"Fit method"));
		praat_new (thee.transfer(), my name, U"_cpp");
	}
END

FORM (PowerCepstrogram_to_Table_hillenbrand, U"PowerCepstrogram: To Table (hillenbrand)", U"PowerCepstrogram: To Table (peak prominence...")
	REAL (U"left Peak search pitch range (Hz)", U"60.0")
	REAL (U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_hillenbrand (me,
			GET_REAL (U"left Peak search pitch range"), GET_REAL (U"right Peak search pitch range"));
		praat_new (thee.transfer(), my name, U"_cpp");
	}
END

DIRECT (PowerCepstrogram_to_Matrix)
	LOOP {
		iam (PowerCepstrogram);
		autoMatrix thee = PowerCepstrogram_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/********************** Cepstrumc  ****************************************/

DIRECT (Cepstrumc_to_LPC)
	LOOP {
		iam (Cepstrumc);
		autoLPC thee = Cepstrumc_to_LPC (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Cepstrumc_to_DTW, U"Cepstrumc: To DTW", U"Cepstrumc: To DTW...")
	LABEL (U"", U"Distance calculation between Cepstra")
	REAL (U"Cepstral weight", U"1.0")
	REAL (U"Log energy weight", U"0.0")
	REAL (U"Regression weight", U"0.0")
	REAL (U"Regression weight log energy", U"0.0")
	REAL (U"Window for regression coefficients (seconds)", U"0.056")
	LABEL (U"", U"Boundary conditions for time warp")
	BOOLEAN (U"Match begin positions", 0)
	BOOLEAN (U"Match end positions", 0)
	RADIO (U"Slope constraints", 1)
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
	autoDTW thee = Cepstrumc_to_DTW (c1, c2, GET_REAL (U"Cepstral weight"),
		GET_REAL (U"Log energy weight"), GET_REAL (U"Regression weight"),
		GET_REAL (U"Regression weight log energy"), GET_REAL (U"Window for regression coefficients"),
		GET_INTEGER (U"Match begin positions"), GET_INTEGER (U"Match end positions"),
		GET_INTEGER (U"Slope constraints"));
	praat_new (thee.transfer(), c1 -> name, U"_", c2 -> name);
END

DIRECT (Cepstrumc_to_Matrix)
	LOOP {
		iam (Cepstrumc);
		autoMatrix thee = Cepstrumc_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/******************** Formant ********************************************/

FORM (Formant_to_LPC, U"Formant: To LPC", 0)
	POSITIVE (U"Sampling frequency (Hz)", U"16000.0")
	OK
DO
	LOOP {
		iam (Formant);
		autoLPC thee = Formant_to_LPC (me, 1.0 / GET_REAL (U"Sampling frequency"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Formant_formula, U"Formant: Formula", 0)
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0")
	NATURAL (U"left Formant range", U"1")
	NATURAL (U"right Formant range", U"5")
	LABEL (U"", U"Formant frequencies in odd numbered rows")
	LABEL (U"", U"Formant bandwidths in even numbered rows")
	SENTENCE (U"Formula", U"if row mod 2 = 1 and self[row,col]/self[row+1,col] < 5 then 0 else self fi")
	OK
DO
	char32 *expression = GET_STRING (U"Formula");
	LOOP {
		iam (Formant);
		Formant_formula (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"left Formant range"), GET_INTEGER (U"right Formant range"), interpreter, expression);
	}
END

/******************** Formant & Spectrogram ************************************/

FORM (Formant_and_Spectrogram_to_IntensityTier, U"Formant & Spectrogram: To IntensityTier", U"Formant & Spectrogram: To IntensityTier...")
	NATURAL (U"Formant number", U"1")
	OK
DO
	Formant me = FIRST (Formant);
	long iformant = GET_INTEGER (U"Formant number");
	Spectrogram thee = FIRST (Spectrogram);
	autoIntensityTier him = Formant_and_Spectrogram_to_IntensityTier (me, thee, iformant);
	praat_new (him.transfer(), my name, U"_", GET_INTEGER (U"Formant number"));
END

/********************LFCC ********************************************/

DIRECT (LFCC_help)
	Melder_help (U"LFCC");
END

FORM (LFCC_to_LPC, U"LFCC: To LPC", U"LFCC: To LPC...")
	INTEGER (U"Number of coefficients", U"0")
	OK
DO
	long ncof = GET_INTEGER (U"Number of coefficients");
	if (ncof < 0) {
		Melder_throw (U"Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LFCC);
		autoLPC thee = LFCC_to_LPC (me, ncof);
		praat_new (thee.transfer(), my name);
	}
END

/********************LPC ********************************************/

DIRECT (LPC_help) Melder_help (U"LPC"); END

FORM (LPC_drawGain, U"LPC: Draw gain", U"LPC: Draw gain...")
	REAL (U"From time (seconds)", U"0.0")
	REAL (U"To time (seconds)", U"0.0 (=all)")
	REAL (U"Minimum gain", U"0.0")
	REAL (U"Maximum gain", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawGain (me, GRAPHICS, GET_REAL (U"From time"), GET_REAL (U"To time"),
			GET_REAL (U"Minimum gain"), GET_REAL (U"Maximum gain"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (LPC_getSamplingInterval)
	LOOP {
		iam (LPC);
		Melder_information (my samplingPeriod, U" seconds");
	}
END

FORM (LPC_getNumberOfCoefficients, U"LPC: Get number of coefficients", U"LPC: Get number of coefficients...")
	NATURAL (U"Frame number", U"1")
	OK
DO
	long iframe = GET_INTEGER (U"Frame number");
	LOOP {
		iam (LPC);
		if (iframe > my nx) {
			Melder_throw (U"Frame number is too large.\n\nPlease choose a number between 1 and ", my nx);
		}
		Melder_information (my d_frames[iframe].nCoefficients, U" coefficients");
	}
END

FORM (LPC_drawPoles, U"LPC: Draw poles", U"LPC: Draw poles...")
	REAL (U"Time (seconds)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawPoles (me, GRAPHICS, GET_REAL (U"Time"), GET_INTEGER (U"Garnish"));
	}
END

DIRECT (LPC_to_Formant)
	LOOP {
		iam (LPC);
		autoFormant thee = LPC_to_Formant (me, 50);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (LPC_to_Formant_keep_all)
	LOOP {
		iam (LPC);
		autoFormant thee = LPC_to_Formant (me, 0);
		praat_new (thee.transfer(), my name);
	}
END

FORM (LPC_to_LFCC, U"LPC: To LFCC", U"LPC: To LFCC...")
	NATURAL (U"Number of coefficients", U"0")
	OK
DO
	long ncof = GET_INTEGER (U"Number of coefficients");
	if (ncof < 0) {
		Melder_throw (U"Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LPC);
		autoLFCC thee = LPC_to_LFCC (me, ncof);
		praat_new (thee.transfer(), my name);
	}
END

FORM (LPC_to_Polynomial, U"LPC: To Polynomial", U"LPC: To Polynomial (slice)...")
	REAL (U"Time (seconds)", U"0.0")
	OK
DO
	double time = GET_REAL (U"Time");
	LOOP {
		iam (LPC);
		autoPolynomial thee = LPC_to_Polynomial (me, time);
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_Spectrum, U"LPC: To Spectrum", U"LPC: To Spectrum (slice)...")
	REAL (U"Time (seconds)", U"0.0")
	REAL (U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (U"Bandwidth reduction (Hz)", U"0.0")
	REAL (U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		double time = GET_REAL (U"Time");
		autoSpectrum thee = LPC_to_Spectrum (me, time, GET_REAL (U"Minimum frequency resolution"),
		GET_REAL (U"Bandwidth reduction"), GET_REAL (U"De-emphasis frequency"));
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_Spectrogram, U"LPC: To Spectrogram", U"LPC: To Spectrogram...")
	REAL (U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (U"Bandwidth reduction (Hz)", U"0.0")
	REAL (U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		autoSpectrogram thee = LPC_to_Spectrogram (me, GET_REAL (U"Minimum frequency resolution"), GET_REAL (U"Bandwidth reduction"), GET_REAL (U"De-emphasis frequency"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (LPC_to_VocalTract_special, U"LPC: To VocalTract", U"LPC: To VocalTract (slice, special)...")
	REAL (U"Time (s)", U"0.0")
	REAL (U"Glottal damping", U"0.1")
	BOOLEAN (U"Radiation damping", 1)
	BOOLEAN (U"Internal damping", 1)
	OK
DO
	double glottalDamping = GET_REAL (U"Glottal damping");
	bool radiationDamping = GET_INTEGER (U"Radiation damping");
	bool internalDamping = GET_INTEGER (U"Internal damping");
	LOOP {
		iam (LPC);
		double time = GET_REAL (U"Time");
		autoVocalTract thee = LPC_to_VocalTract (me, time, glottalDamping, radiationDamping, internalDamping);
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_VocalTract, U"LPC: To VocalTract", U"LPC: To VocalTract (slice)...")
	REAL (U"Time (s)", U"0.0")
	POSITIVE (U"Length (m)", U"0.17")
	OK
DO
	double time = GET_REAL (U"Time");
	LOOP {
		iam (LPC);
		autoVocalTract thee = LPC_to_VocalTract (me, time, GET_REAL (U"Length"));
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

DIRECT (LPC_downto_Matrix_lpc)
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_lpc (me);
		praat_new (thee.transfer(), my name, U"_lpc");
	}
END

DIRECT (LPC_downto_Matrix_rc)
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_rc (me);
		praat_new (thee.transfer(), my name, U"_rc");
	}
END

DIRECT (LPC_downto_Matrix_area)
	LOOP {
		iam (LPC);
		autoMatrix thee = LPC_downto_Matrix_area (me);
		praat_new (thee.transfer(), my name, U"_area");
	}
END

/********************** Sound *******************************************/

FORM (Sound_to_PowerCepstrogram, U"Sound: To PowerCepstrogram", U"Sound: To PowerCepstrogram...")
	POSITIVE (U"Pitch floor (Hz)", U"60.0")
	POSITIVE (U"Time step (s)", U"0.002")
	POSITIVE (U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram (me, GET_REAL (U"Pitch floor"), GET_REAL (U"Time step"), GET_REAL(U"Maximum frequency"),
			 GET_REAL (U"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END


FORM (Sound_to_PowerCepstrogram_hillenbrand, U"Sound: To PowerCepstrogram (hillenbrand)", U"Sound: To PowerCepstrogram...")
	POSITIVE (U"Pitch floor (Hz)", U"60.0")
	POSITIVE (U"Time step (s)", U"0.002")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram_hillenbrand (me, GET_REAL (U"Pitch floor"), GET_REAL (U"Time step"));
		praat_new (thee.transfer(), my name);
	}
END
	
FORM (Sound_to_Formant_robust, U"Sound: To Formant (robust)", U"Sound: To Formant (robust)...")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Max. number of formants", U"5.0")
	REAL (U"Maximum formant (Hz)", U"5500 (= adult female)")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50")
	POSITIVE (U"Number of std. dev.", U"1.5")
	NATURAL (U"Maximum number of iterations", U"5")
	REAL (U"Tolerance", U"0.000001")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Formant_robust (me, GET_REAL (U"Time step"), GET_REAL (U"Max. number of formants"),
			GET_REAL (U"Maximum formant"), GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis from"), 50.0,
			GET_REAL (U"Number of std. dev."), GET_INTEGER (U"Maximum number of iterations"),
			GET_REAL (U"Tolerance"), 1), my name);
	}
END

static void Sound_to_LPC_addCommonFields (UiForm dia) {
	LABEL (U"", U"Warning 1:  for formant analysis, use \"To Formant\" instead.")
	LABEL (U"", U"Warning 2:  if you do use \"To LPC\", you may want to resample first.")
	LABEL (U"", U"Click Help for more details.")
	LABEL (U"", U"")
	NATURAL (U"Prediction order", U"16")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Time step (s)", U"0.005")
	REAL (U"Pre-emphasis frequency (Hz)", U"50.0")
}

static void Sound_to_LPC_checkCommonFields (UiForm dia, long *predictionOrder, double *analysisWindowDuration,
        double *timeStep, double *preemphasisFrequency) {
	*predictionOrder = GET_INTEGER (U"Prediction order");
	*analysisWindowDuration = GET_REAL (U"Window length");
	*timeStep = GET_REAL (U"Time step");
	*preemphasisFrequency = GET_REAL (U"Pre-emphasis frequency");
	if (*preemphasisFrequency < 0.0) {
		Melder_throw (U"Pre-emphasis frequencies cannot be negative.");
	}
}

FORM (Sound_to_LPC_auto, U"Sound: To LPC (autocorrelation)", U"Sound: To LPC (autocorrelation)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, &preemphasisFrequency);
	LOOP {
		iam (Sound);
		autoLPC thee = Sound_to_LPC_auto (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_LPC_covar, U"Sound: To LPC (covariance)", U"Sound: To LPC (covariance)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		autoLPC thee = Sound_to_LPC_covar (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_LPC_burg, U"Sound: To LPC (burg)", U"Sound: To LPC (burg)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		autoLPC thee = Sound_to_LPC_burg (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_LPC_marple, U"Sound: To LPC (marple)", U"Sound: To LPC (marple)...")
	Sound_to_LPC_addCommonFields (dia);
	POSITIVE (U"Tolerance 1", U"1e-6")
	POSITIVE (U"Tolerance 2", U"1e-6")
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, &timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		autoLPC thee = Sound_to_LPC_marple (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency, GET_REAL (U"Tolerance 1"), GET_REAL (U"Tolerance 2"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_MFCC, U"Sound: To MFCC", U"Sound: To MFCC...")
	NATURAL (U"Number of coefficients", U"12")
	POSITIVE (U"Window length (s)", U"0.015")
	POSITIVE (U"Time step (s)", U"0.005")
	LABEL (U"", U"Filter bank parameters")
	POSITIVE (U"Position of first filter (mel)", U"100.0")
	POSITIVE (U"Distance between filters (mel)", U"100.0")
	REAL (U"Maximum frequency (mel)", U"0.0");
	OK
DO
	long p = GET_INTEGER (U"Number of coefficients");
	if (p > 24) {
		Melder_throw (U"Number of coefficients must be < 25.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_to_MFCC (me, p, GET_REAL (U"Window length"),
			GET_REAL (U"Time step"), GET_REAL (U"Position of first filter"),
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Distance between filters")), my name);
	}
END

FORM (VocalTract_drawSegments, U"VocalTract: Draw segments", 0)
	POSITIVE (U"Maximum length (cm)", U"20.0")
	POSITIVE (U"Maximum area (cm^2)", U"90.0")
	BOOLEAN (U"Closed at glottis", 1)
	OK
DO
	autoPraatPicture picture;
	double maxLength = GET_REAL (U"Maximum length") / 100.0;
	double maxArea = GET_REAL (U"Maximum area") / 10000.0;
	bool closedAtGlottis = GET_INTEGER (U"Closed at glottis");
	LOOP {
		iam (VocalTract);
		VocalTract_drawSegments (me, GRAPHICS, maxLength, maxArea, closedAtGlottis);
	}
END

DIRECT (VocalTract_getLength)
	LOOP {
		iam (VocalTract);
		Melder_information (my xmax - my xmin, U" m");
	}
END

FORM (VocalTract_setLength, U"", 0)
	POSITIVE (U"New length (m)", U"0.17")
	OK
DO
	double newLength = GET_REAL (U"New length");
	LOOP {
		iam (VocalTract);
		VocalTract_setLength (me, newLength);
	}
END

FORM (VocalTract_to_VocalTractTier, U"VocalTract: To VocalTractTier", 0)
	REAL (U"Tier start time (s)", U"0.0")
	REAL (U"Tier end time (s)", U"1.0")
	REAL (U"Insert at time (s)", U"0.5")
	OK
DO
	double xmin = GET_REAL (U"Tier start time");
	double xmax = GET_REAL (U"Tier end time");
	double time = GET_REAL (U"Insert at time");
	REQUIRE (xmin < xmax, U"The start time must be before the end time.")
	REQUIRE (time >= xmin and time <= xmax, U"The insert time must be between start en end time.")
	LOOP {
		iam (VocalTract);
		autoVocalTractTier thee = VocalTract_to_VocalTractTier (me, xmin, xmax, time);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (VocalTractTier_help)
	Melder_help (U"VocalTractTier");
END

FORM (VocalTractTier_to_LPC, U"VocalTractTier: To LPC", 0)
	POSITIVE (U"Time step", U"0.005")
	OK
DO
	LOOP {
		iam (VocalTractTier);
		autoLPC thee = VocalTractTier_to_LPC (me, GET_REAL (U"Time step"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (VocalTractTier_to_VocalTract, U"", 0)
	REAL (U"Time (s)", U"0.1")
	OK
DO
	double time = GET_REAL (U"Time");
	LOOP {
		iam (VocalTractTier);
		autoVocalTract thee = VocalTractTier_to_VocalTract (me, time);
		praat_new (thee.transfer(), my name);
	}
END

FORM (VocalTractTier_addVocalTract, U"VocalTractTier: Add VocalTract", 0)
	REAL (U"Time", U"0.1")
	OK
DO
	VocalTractTier me = FIRST (VocalTractTier);
	VocalTract thee = FIRST (VocalTract);
	VocalTractTier_addVocalTract (me, GET_REAL (U"Time"), thee);
	praat_dataChanged (me);
END

/******************* LPC & Sound *************************************/

FORM (LPC_and_Sound_filter, U"LPC & Sound: Filter", U"LPC & Sound: Filter...")
	BOOLEAN (U"Use LPC gain", 0)
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filter (me , s, GET_INTEGER (U"Use LPC gain"));
	praat_new (thee.transfer(), my name);
END

FORM (LPC_and_Sound_filterWithFilterAtTime, U"LPC & Sound: Filter with one filter at time",
      U"LPC & Sound: Filter with filter at time...")
	OPTIONMENU (U"Channel", 2)
	OPTION (U"Both")
	OPTION (U"Left")
	OPTION (U"Right")
	REAL (U"Use filter at time (s)", U"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	long channel = GET_INTEGER (U"Channel") - 1;
	autoSound thee = LPC_and_Sound_filterWithFilterAtTime (me , s, channel, GET_REAL (U"Use filter at time"));
	praat_new (thee.transfer(), my name);
END

DIRECT (LPC_and_Sound_filterInverse)
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	autoSound thee = LPC_and_Sound_filterInverse (me , s);
	praat_new (thee.transfer(), my name);
END

FORM (LPC_and_Sound_filterInverseWithFilterAtTime, U"LPC & Sound: Filter (inverse) with filter at time",
      U"LPC & Sound: Filter (inverse) with filter at time...")
	OPTIONMENU (U"Channel", 2)
	OPTION (U"Both")
	OPTION (U"Left")
	OPTION (U"Right")
	REAL (U"Use filter at time (s)", U"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	long channel = GET_INTEGER (U"Channel") - 1;
	autoSound thee = LPC_and_Sound_filterInverseWithFilterAtTime (me , s, channel, GET_REAL (U"Use filter at time"));
	praat_new (thee.transfer(), my name);
END

FORM (LPC_and_Sound_to_LPC_robust, U"Robust LPC analysis", U"LPC & Sound: To LPC (robust)...")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (U"Number of std. dev.", U"1.5")
	NATURAL (U"Maximum number of iterations", U"5")
	REAL (U"Tolerance", U"0.000001")
	BOOLEAN (U"Variable location", 0)
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	praat_new (LPC_and_Sound_to_LPC_robust (me, s, GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis frequency"),
		GET_REAL (U"Number of std. dev."), GET_INTEGER (U"Maximum number of iterations"),
		GET_REAL (U"Tolerance"), GET_INTEGER (U"Variable location")), my name, U"_r");
END

extern void praat_TimeTier_query_init (ClassInfo klas);
extern void praat_TimeTier_modify_init (ClassInfo klas);
void praat_uvafon_LPC_init ();
void praat_uvafon_LPC_init () {
	Thing_recognizeClassesByName (classCepstrumc, classPowerCepstrum, classCepstrogram, classPowerCepstrogram, classLPC, classLFCC, classMFCC, classVocalTractTier, NULL);

	praat_addAction1 (classPowerCepstrum, 0, U"PowerCepstrum help", 0, 0, DO_PowerCepstrum_help);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw...", 0, 0, DO_PowerCepstrum_draw);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw tilt line...", 0, 0, DO_PowerCepstrum_drawTiltLine);
	praat_addAction1 (classCepstrum, 0, U"Draw (linear)...", 0, praat_HIDDEN, DO_Cepstrum_drawLinear);
	praat_addAction1 (classCepstrum, 0, U"Down to PowerCepstrum", 0, 0, DO_Cepstrum_downto_PowerCepstrum);
	
	praat_addAction1 (classPowerCepstrum, 1, U"Query -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak...", 0, 1, DO_PowerCepstrum_getPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get quefrency of peak...", 0, 1, DO_PowerCepstrum_getQuefrencyOfPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_PowerCepstrum_getPeakProminence_hillenbrand);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence...", 0, 1, DO_PowerCepstrum_getPeakProminence);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line slope...", 0, 1, DO_PowerCepstrum_getTiltLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, U"Get tilt line intercept...", 0, 1, DO_PowerCepstrum_getTiltLineIntercept);
		praat_addAction1 (classPowerCepstrum, 0, U"Get rhamonics to noise ratio...", 0, 1, DO_PowerCepstrum_getRNR);
	praat_addAction1 (classPowerCepstrum, 1, U"Modify -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, U"Formula...", 0, 1, DO_PowerCepstrum_formula);
		praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt (in-line)...", 0, 1, DO_PowerCepstrum_subtractTilt_inline);
		praat_addAction1 (classPowerCepstrum, 0, U"Smooth (in-line)...", 0, 1, DO_PowerCepstrum_smooth_inline);

	praat_addAction1 (classPowerCepstrum, 0, U"Subtract tilt...", 0, 0, DO_PowerCepstrum_subtractTilt);
	praat_addAction1 (classPowerCepstrum, 0, U"Smooth...", 0, 0, DO_PowerCepstrum_smooth);
	praat_addAction1 (classCepstrum, 0, U"To Spectrum", 0, praat_HIDDEN, DO_Cepstrum_to_Spectrum);
	praat_addAction1 (classPowerCepstrum, 0, U"To Matrix", 0, 0, DO_PowerCepstrum_to_Matrix);

	praat_addAction1 (classPowerCepstrogram, 0, U"PowerCepstrogram help", 0, 0, DO_PowerCepstrogram_help);
	praat_addAction1 (classPowerCepstrogram, 0, U"Paint...", 0, 0, DO_PowerCepstrogram_paint);
	praat_addAction1 (classPowerCepstrogram, 1, U"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency domain", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get start quefrency", 0, 2, DO_PowerCepstrogram_getStartQuefrency);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get end quefrency", 0, 2, DO_PowerCepstrogram_getEndQuefrency);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency sampling", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get number of quefrency bins", 0, 2, DO_PowerCepstrogram_getNumberOfQuefrencyBins);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get quefrency step", 0, 2, DO_PowerCepstrogram_getQuefrencyStep);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_PowerCepstrogram_getCPPS_hillenbrand);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS...", 0, 1, DO_PowerCepstrogram_getCPPS);
	praat_addAction1 (classPowerCepstrogram, 0, U"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 0, U"Formula...", 0, 1, DO_PowerCepstrogram_formula);
		praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt (in-line)...", 0, 1, DO_PowerCepstrogram_subtractTilt_inline);
	praat_addAction1 (classPowerCepstrogram, 0, U"To PowerCepstrum (slice)...", 0, 0, DO_PowerCepstrogram_to_PowerCepstrum_slice);
	praat_addAction1 (classPowerCepstrogram, 0, U"Smooth...", 0, 0, DO_PowerCepstrogram_smooth);
	praat_addAction1 (classPowerCepstrogram, 0, U"Subtract tilt...", 0, 0, DO_PowerCepstrogram_subtractTilt);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (hillenbrand)...", 0, praat_HIDDEN, DO_PowerCepstrogram_to_Table_hillenbrand);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (peak prominence)...", 0, praat_HIDDEN, DO_PowerCepstrogram_to_Table_cpp);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Matrix", 0, 0, DO_PowerCepstrogram_to_Matrix);

	praat_addAction1 (classCepstrumc, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To LPC", 0, 0, DO_Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, U"To DTW...", 0, 0, DO_Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, U"Hack", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, U"To Matrix", 0, 0, DO_Cepstrumc_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classFormant, 0, U"To LPC...", 0, 0, DO_Formant_to_LPC);
	praat_addAction1 (classFormant, 0, U"Formula...", U"Formula (bandwidths)...", 1, DO_Formant_formula);
	praat_addAction2 (classFormant, 1, classSpectrogram, 1, U"To IntensityTier...", 0, 0, DO_Formant_and_Spectrogram_to_IntensityTier);

	
	
	praat_addAction1 (classLFCC, 0, U"LFCC help", 0, 0, DO_LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, U"To LPC...", 0, 0, DO_LFCC_to_LPC);

	praat_addAction1 (classLPC, 0, U"LPC help", 0, 0, DO_LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classLPC, 0, U"Draw gain...", 0, 1, DO_LPC_drawGain);
	praat_addAction1 (classLPC, 0, U"Draw poles...", 0, 1, DO_LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, 0, 0, 0);
		praat_TimeFrameSampled_query_init (classLPC);
		praat_addAction1 (classLPC, 1, U"Get sampling interval", 0, 1, DO_LPC_getSamplingInterval);
		praat_addAction1 (classLPC, 1, U"Get number of coefficients...", 0, 1, DO_LPC_getNumberOfCoefficients);
	praat_addAction1 (classLPC, 0, MODIFY_BUTTON, 0, 0, 0);
		praat_TimeFunction_modify_init (classLPC);
	praat_addAction1 (classLPC, 0, U"Extract", 0, 0, 0);

	praat_addAction1 (classLPC, 0, U"To Spectrum (slice)...", 0, 0, DO_LPC_to_Spectrum);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice)...", 0, 0, DO_LPC_to_VocalTract);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice, special)...", 0, 0, DO_LPC_to_VocalTract_special);
	praat_addAction1 (classLPC, 0, U"To Polynomial (slice)...", 0, 0, DO_LPC_to_Polynomial);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (lpc)", 0, 0, DO_LPC_downto_Matrix_lpc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (rc)", 0, praat_HIDDEN, DO_LPC_downto_Matrix_rc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (area)", 0, praat_HIDDEN, DO_LPC_downto_Matrix_area);
	praat_addAction1 (classLPC, 0, U"Analyse", 0, 0, 0);
	praat_addAction1 (classLPC, 0, U"To Formant", 0, 0, DO_LPC_to_Formant);
	praat_addAction1 (classLPC, 0, U"To Formant (keep all)", 0, 0, DO_LPC_to_Formant_keep_all);
	praat_addAction1 (classLPC, 0, U"To LFCC...", 0, 0, DO_LPC_to_LFCC);
	praat_addAction1 (classLPC, 0, U"To Spectrogram...", 0, 0, DO_LPC_to_Spectrogram);

	praat_addAction2 (classLPC, 1, classSound, 1, U"Analyse", 0, 0, 0);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter...", 0, 0, DO_LPC_and_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse)", 0, 0, DO_LPC_and_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, U"To LPC (robust)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_LPC_and_Sound_to_LPC_robust);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter with filter at time...", 0, 0, DO_LPC_and_Sound_filterWithFilterAtTime);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse) with filter at time...", 0, 0, DO_LPC_and_Sound_filterInverseWithFilterAtTime);


	praat_addAction1 (classSound, 0, U"To LPC (autocorrelation)...", U"To Formant (sl)...", 1, DO_Sound_to_LPC_auto);
	praat_addAction1 (classSound, 0, U"To LPC (covariance)...", U"To LPC (autocorrelation)...", 1, DO_Sound_to_LPC_covar);
	praat_addAction1 (classSound, 0, U"To LPC (burg)...", U"To LPC (covariance)...", 1, DO_Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, U"To LPC (marple)...", U"To LPC (burg)...", 1, DO_Sound_to_LPC_marple);
	praat_addAction1 (classSound, 0, U"To MFCC...", U"To LPC (marple)...", 1, DO_Sound_to_MFCC);
	praat_addAction1 (classSound, 0, U"To Formant (robust)...", U"To Formant (sl)...", 2, DO_Sound_to_Formant_robust);
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram...", U"To Harmonicity (gne)...", 1, DO_Sound_to_PowerCepstrogram);
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram (hillenbrand)...", U"To Harmonicity (gne)...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_PowerCepstrogram_hillenbrand);
	
	praat_addAction1 (classVocalTract, 0, U"Draw segments...", U"Draw", 0, DO_VocalTract_drawSegments);
	praat_addAction1 (classVocalTract, 1, U"Get length", U"Draw segments...", 0, DO_VocalTract_getLength);
	praat_addAction1 (classVocalTract, 1, U"Set length", U"Formula...", 0, DO_VocalTract_getLength);
	praat_addAction1 (classVocalTract, 0, U"To VocalTractTier...", U"To Spectrum...", 0, DO_VocalTract_to_VocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"VocalTractTier help", 0, 0, DO_VocalTractTier_help);
	praat_addAction1 (classVocalTractTier, 0, U"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"To LPC...", 0, 0, DO_VocalTractTier_to_LPC);
	praat_addAction1 (classVocalTractTier, 0, U"To VocalTract...", 0, 0, DO_VocalTractTier_to_VocalTract);
	praat_addAction2 (classVocalTractTier, 1, classVocalTract, 1, U"Add VocalTract...", 0, 0, DO_VocalTractTier_addVocalTract);

	INCLUDE_MANPAGES (manual_LPC)
	INCLUDE_MANPAGES (manual_DataModeler)

	INCLUDE_LIBRARY (praat_DataModeler_init)
}

/* End of file praat_LPC_init.c */
