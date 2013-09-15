/* praat_LPC_init.cpp
 *
 * Copyright (C) 1994-2013 David Weenink
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
#include "praat.h"
#include "Cepstrumc.h"
#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "DTW.h"
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
#include "MelFilter_and_MFCC.h"
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "VocalTractTier.h"

#undef iam
#define iam iam_LOOP

static const wchar_t *DRAW_BUTTON    = L"Draw -";
static const wchar_t *QUERY_BUTTON   = L"Query -";

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
	Melder_help (L"PowerCepstrum");
END

FORM (Cepstrum_drawLinear, L"Cepstrum: Draw linear", L"Cepstrum: Draw (linear)...")
	REAL (L"left Quefrency range (s)", L"0.0")
	REAL (L"right Quefrency range (s)", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Cepstrum);
		Cepstrum_drawLinear (me, GRAPHICS, GET_REAL (L"left Quefrency range"), GET_REAL (L"right Quefrency range"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (PowerCepstrum_draw, L"PowerCepstrum: Draw", L"PowerCepstrum: Draw...")
	REAL (L"left Quefrency range (s)", L"0.0")
	REAL (L"right Quefrency range (s)", L"0.0")
	REAL (L"Minimum (dB)", L"0.0")
	REAL (L"Maximum (dB)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_draw (me, GRAPHICS, GET_REAL (L"left Quefrency range"), GET_REAL (L"right Quefrency range"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (PowerCepstrum_drawTiltLine, L"PowerCepstrum: Draw tilt line", L"PowerCepstrum: Draw tilt line...")
	REAL (L"left Quefrency range (s)", L"0.0")
	REAL (L"right Quefrency range (s)", L"0.0")
	REAL (L"left Amplitude range (dB)", L"0.0")
	REAL (L"right Amplitude range (dB)", L"0.0")
	LABEL (L"", L"Parameters for the tilt line fit")
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_drawTiltLine (me, GRAPHICS, GET_REAL (L"left Quefrency range"), GET_REAL (L"right Quefrency range"),
			GET_REAL (L"left Amplitude range"), GET_REAL (L"right Amplitude range"),
			GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"), 
			GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
	}
END

FORM (PowerCepstrum_formula, L"PowerCepstrum: Formula...", L"PowerCepstrum: Formula...")
	LABEL (L"label", L"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	praat_Fon_formula (dia, interpreter);
END

FORM (PowerCepstrum_getPeak, L"PowerCepstrum: Get peak", 0)
	REAL (L"left Search peak in pitch range (Hz)", L"60.0")
	REAL (L"right Search peak in pitch range (Hz)", L"333.3")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, GET_REAL (L"left Search peak in pitch range"), GET_REAL (L"right Search peak in pitch range"), GET_INTEGER (L"Interpolation") - 1, &peakdB, &quefrency);
		Melder_informationReal (peakdB, L" dB");
	}
END

FORM (PowerCepstrum_getQuefrencyOfPeak, L"PowerCepstrum: Get quefrency of peak", 0)
	REAL (L"left Search peak in pitch range (Hz)", L"60.0")
	REAL (L"right Search peak in pitch range (Hz)", L"333.3")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double peakdB, quefrency;
		PowerCepstrum_getMaximumAndQuefrency (me, GET_REAL (L"left Search peak in pitch range"), GET_REAL (L"right Search peak in pitch range"), GET_INTEGER (L"Interpolation") - 1, &peakdB, &quefrency);
		double f = 1 / quefrency;
		Melder_information (Melder_double (quefrency), L" s (f =", Melder_double (f), L" Hz)");
	}
END

FORM (PowerCepstrum_getRNR, L"PowerCepstrum: Get rhamonics to noise ration", 0)
	REAL (L"left Pitch range (Hz)", L"60.0")
	REAL (L"right Pitch range (Hz)", L"333.3")
	POSITIVE (L"Fractional width (0-1)", L"0.05")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double rnr = PowerCepstrum_getRNR (me, GET_REAL (L"left Pitch range"), GET_REAL (L"right Pitch range"), GET_REAL (L"Fractional width"));
		Melder_information (Melder_double (rnr), L" (rnr)");
	}
END

FORM (PowerCepstrum_getPeakProminence_hillenbrand, L"PowerCepstrum: Get peak prominence (hillenbrand)", L"PowerCepstrum: Get peak prominence (hillenbrand)...")
	REAL (L"left Search peak in pitch range (Hz)", L"60.0")
	REAL (L"right Search peak in pitch range (Hz)", L"333.3")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence_hillenbrand (me,
			GET_REAL (L"left Search peak in pitch range"), GET_REAL (L"right Search peak in pitch range"), &qpeak);
		Melder_information (Melder_double (cpp), L" dB; quefrency=", Melder_double (qpeak), L" s (f=",
			Melder_double (1 / qpeak), L" Hz).");
	}
END

FORM (PowerCepstrum_getTiltLineSlope, L"PowerCepstrum: Get tilt line slope", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		int lineType = GET_INTEGER (L"Line type");
		PowerCepstrum_fitTiltLine (me, GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"), 
			  &a, &intercept, lineType, GET_INTEGER (L"Fit method"));
		Melder_information (Melder_double (a), L" dB / ", lineType == 1 ? L"s" : L"ln (s)");
	}
END


FORM (PowerCepstrum_getTiltLineIntercept, L"PowerCepstrum: Get tilt line intercept", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double a, intercept;
		int lineType = GET_INTEGER (L"Line type");
		PowerCepstrum_fitTiltLine (me, GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"), 
			  &a, &intercept, lineType, GET_INTEGER (L"Fit method"));
		Melder_information (Melder_double (intercept), L" dB");
	}
END

FORM (PowerCepstrum_getPeakProminence, L"PowerCepstrum: Get peak prominence", L"PowerCepstrum: Get peak prominence...")
	REAL (L"left Search peak in pitch range (Hz)", L"60.0")
	REAL (L"right Search peak in pitch range (Hz)", L"333.3")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		double qpeak, cpp = PowerCepstrum_getPeakProminence (me,
			GET_REAL (L"left Search peak in pitch range"), GET_REAL (L"right Search peak in pitch range"),
			GET_INTEGER (L"Interpolation") - 1,
			GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"),
			GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"), &qpeak);
		Melder_information (Melder_double (cpp), L" dB; quefrency=", Melder_double (qpeak), L" s (f=",
			Melder_double (1 / qpeak), L" Hz).");
	}
END

FORM (PowerCepstrum_subtractTilt_inline, L"PowerCepstrum: Subtract tilt (in-line)", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_subtractTilt_inline (me, GET_REAL (L"left Tilt line quefrency range"), 
			GET_REAL (L"right Tilt line quefrency range"), GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
	}
END

FORM (PowerCepstrum_smooth_inline, L"PowerCepstrum: Smooth (in-line)", 0)
	REAL (L"Quefrency averaging window (s)", L"0.0005")
	NATURAL (L"Number of iterations", L"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		PowerCepstrum_smooth_inline (me, GET_REAL (L"Quefrency averaging window"), GET_INTEGER (L"Number of iterations"));
	}
END

FORM (PowerCepstrum_smooth, L"PowerCepstrum: Smooth", 0)
	REAL (L"Quefrency averaging window (s)", L"0.0005")
	NATURAL (L"Number of iterations", L"1");
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_smooth (me, GET_REAL (L"Quefrency averaging window"), GET_INTEGER (L"Number of iterations"));
		praat_new (thee.transfer(), my name, L"_smooth");
	}
END

FORM (PowerCepstrum_subtractTilt, L"PowerCepstrum: Subtract tilt", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 1)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrum);
		autoPowerCepstrum thee = PowerCepstrum_subtractTilt (me, GET_REAL (L"left Tilt line quefrency range"), 
			GET_REAL (L"right Tilt line quefrency range"), GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
		praat_new (thee.transfer(), my name, L"minusTilt");
	}
END

DIRECT (Cepstrum_to_Spectrum)
	LOOP {
		iam (Cepstrum);
		praat_new (Cepstrum_to_Spectrum (me), my name);
	}
END

DIRECT (PowerCepstrum_to_Matrix)
	LOOP {
		iam (PowerCepstrum);
		praat_new (PowerCepstrum_to_Matrix (me), my name);
	}
END

/********************** Cepstrogram  ****************************************/

DIRECT (PowerCepstrogram_help)
	Melder_help (L"PowerCepstrogram");
END


FORM (PowerCepstrogram_paint, L"PowerCepstrogram: Paint", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	REAL (L"left Quefrency range (s)", L"0.0")
	REAL (L"right Quefrency range (s)", L"0.0")
	REAL (L"Minimum (dB)", L"0.0")
	REAL (L"Maximum (dB)", L"0.0")
	BOOLEAN (L"Garnish", 1);
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_paint (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Quefrency range"), GET_REAL (L"right Quefrency range"),
  			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (PowerCepstrogram_smooth, L"PowerCepstrogram: Smooth", L"PowerCepstrogram: Smooth...")
	REAL (L"Time averaging window (s)", L"0.02")
	REAL (L"Quefrency averaging window (s)", L"0.0005")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_smooth (me, GET_REAL (L"Time averaging window"), GET_REAL (L"Quefrency averaging window"));
		praat_new (thee.transfer(), my name, L"_smoothed");
	}
END

DIRECT (PowerCepstrogram_getStartQuefrency)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymin, L" (s)");
	}
END

DIRECT (PowerCepstrogram_getEndQuefrency)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ymax, L" (s)");
	}
END

DIRECT (PowerCepstrogram_getNumberOfQuefrencyBins)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my ny, L" quefrency bins");
	}
END

DIRECT (PowerCepstrogram_getQuefrencyStep)
	LOOP {
		iam (PowerCepstrogram);
		Melder_informationReal (my dy, L" quefrency step (s)");
	}
END

FORM (PowerCepstrogram_subtractTilt, L"PowerCepstrogram: Subtract tilt", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 2)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoPowerCepstrogram thee = PowerCepstrogram_subtractTilt (me, GET_REAL (L"left Tilt line quefrency range"), 
			GET_REAL (L"right Tilt line quefrency range"), GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
		praat_new (thee.transfer(), my name, L"_minusTilt");
	}
END

FORM (PowerCepstrogram_subtractTilt_inline, L"PowerCepstrogram: Subtract tilt (in-line)", 0)
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 2)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		PowerCepstrogram_subtractTilt_inline (me, GET_REAL (L"left Tilt line quefrency range"), 
			GET_REAL (L"right Tilt line quefrency range"), GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
	}
END

FORM (PowerCepstrogram_getCPPS_hillenbrand, L"PowerCepstrogram: Get CPPS", 0)
	LABEL (L"", L"Smoothing:")
	BOOLEAN (L"Subtract tilt before smoothing", 1)
	REAL (L"Time averaging window (s)", L"0.001")
	REAL (L"Quefrency averaging window (s)", L"0.00005")
	LABEL (L"", L"Peak search:")
	REAL (L"left Peak search pitch range (Hz)", L"60.0")
	REAL (L"right Peak search pitch range (Hz)", L"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS_hillenbrand (me, GET_INTEGER (L"Subtract tilt before smoothing"), GET_REAL (L"Time averaging window"), GET_REAL (L"Quefrency averaging window"),
			GET_REAL (L"left Peak search pitch range"), GET_REAL (L"right Peak search pitch range"));
		Melder_informationReal (cpps, L" dB");
	}
END


FORM (PowerCepstrogram_getCPPS, L"PowerCepstrogram: Get CPPS", 0)
	LABEL (L"", L"Smoothing:")
	BOOLEAN (L"Subtract tilt before smoothing", 1)
	REAL (L"Time averaging window (s)", L"0.001")
	REAL (L"Quefrency averaging window (s)", L"0.00005")
	LABEL (L"", L"Peak search:")
	REAL (L"left Peak search pitch range (Hz)", L"60.0")
	REAL (L"right Peak search pitch range (Hz)", L"330.0")
	POSITIVE (L"Tolerance (0-1)", L"0.05")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	LABEL (L"", L"Tilt line:")
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 2)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double cpps = PowerCepstrogram_getCPPS (me, GET_INTEGER (L"Subtract tilt before smoothing"), GET_REAL (L"Time averaging window"), 
			GET_REAL (L"Quefrency averaging window"),
			GET_REAL (L"left Peak search pitch range"), GET_REAL (L"right Peak search pitch range"), GET_REAL (L"Tolerance"),
			GET_INTEGER (L"Interpolation") - 1, GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"),
			GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
		Melder_informationReal (cpps, L" dB");
	}
END

FORM (PowerCepstrogram_formula, L"PowerCepstrogram: Formula", L"")
	LABEL (L"label", L"Do for all times and quefrencies:")
	LABEL (L"label", L"   `x' is the time in seconds")
	LABEL (L"label", L"   `y' is the quefrency in seconds")
	LABEL (L"label", L"   `self' is the current value")
	LABEL (L"label", L"   Replace all values with:")
	TEXTFIELD (L"formula", L"sqrt(self)")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PowerCepstrogram may have partially changed
			throw;
		}
	}
END

FORM (PowerCepstrogram_to_PowerCepstrum_slice, L"PowerCepstrogram: To PowerCepstrum (slice)", 0)
	REAL (L"Time (s)", L"0.1")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		double time = GET_REAL (L"Time");
		autoPowerCepstrum thee = PowerCepstrogram_to_PowerCepstrum_slice (me, time);
		praat_new (thee.transfer(), my name, NUMstring_timeNoDot (time));
	}
END

FORM (PowerCepstrogram_to_Table_cpp, L"PowerCepstrogram: To Table (peak prominence)", L"PowerCepstrogram: To Table (peak prominence...")
	REAL (L"left Peak search pitch range (Hz)", L"60.0")
	REAL (L"right Peak search pitch range (Hz)", L"330.0")
	POSITIVE (L"Tolerance (0-1)", L"0.05")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	REAL (L"left Tilt line quefrency range (s)", L"0.001")
	REAL (L"right Tilt line quefrency range (s)", L"0.0 (=end)")
	OPTIONMENU (L"Line type", 2)
	OPTION (L"Straight")
	OPTION (L"Exponential decay")
	OPTIONMENU (L"Fit method", 2)
	OPTION (L"Least squares")
	OPTION (L"Robust")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_cpp (me,
			GET_REAL (L"left Peak search pitch range"), GET_REAL (L"right Peak search pitch range"), GET_REAL (L"Tolerance"),
			GET_INTEGER (L"Interpolation") - 1,
			GET_REAL (L"left Tilt line quefrency range"), GET_REAL (L"right Tilt line quefrency range"),
			GET_INTEGER (L"Line type"), GET_INTEGER (L"Fit method"));
		praat_new (thee.transfer(), my name, L"_cpp");
	}
END

FORM (PowerCepstrogram_to_Table_hillenbrand, L"PowerCepstrogram: To Table (hillenbrand)", L"PowerCepstrogram: To Table (peak prominence...")
	REAL (L"left Peak search pitch range (Hz)", L"60.0")
	REAL (L"right Peak search pitch range (Hz)", L"330.0")
	OK
DO
	LOOP {
		iam (PowerCepstrogram);
		autoTable thee = PowerCepstrogram_to_Table_hillenbrand (me,
			GET_REAL (L"left Peak search pitch range"), GET_REAL (L"right Peak search pitch range"));
		praat_new (thee.transfer(), my name, L"_cpp");
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
		praat_new (Cepstrumc_to_LPC (me), my name);
	}
END

FORM (Cepstrumc_to_DTW, L"Cepstrumc: To DTW", L"Cepstrumc: To DTW...")
	LABEL (L"", L"Distance calculation between Cepstra")
	REAL (L"Cepstral weight", L"1.0")
	REAL (L"Log energy weight", L"0.0")
	REAL (L"Regression weight", L"0.0")
	REAL (L"Regression weight log energy", L"0.0")
	REAL (L"Window for regression coefficients (seconds)", L"0.056")
	LABEL (L"", L"Boundary conditions for time warp")
	BOOLEAN (L"Match begin positions", 0)
	BOOLEAN (L"Match end positions", 0)
	RADIO (L"Slope constraints", 1)
	RADIOBUTTON (L"no restriction")
	RADIOBUTTON (L"1/3 < slope < 3")
	RADIOBUTTON (L"1/2 < slope < 2")
	RADIOBUTTON (L"2/3 < slope < 3/2")
	OK
DO
	Cepstrumc c1 = 0, c2 = 0;
	LOOP {
		iam (Cepstrumc);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (Cepstrumc_to_DTW (c1, c2, GET_REAL (L"Cepstral weight"),
		GET_REAL (L"Log energy weight"), GET_REAL (L"Regression weight"),
		GET_REAL (L"Regression weight log energy"), GET_REAL (L"Window for regression coefficients"),
		GET_INTEGER (L"Match begin positions"), GET_INTEGER (L"Match end positions"),
		GET_INTEGER (L"Slope constraints")), c1->name, L"_", c2->name);
END

DIRECT (Cepstrumc_to_Matrix)
	LOOP {
		iam (Cepstrumc);
		praat_new (Cepstrumc_to_Matrix (me), my name);
	}
END

/******************** Formant ********************************************/

FORM (Formant_to_LPC, L"Formant: To LPC", 0)
	POSITIVE (L"Sampling frequency (Hz)", L"16000.0")
	OK
DO
	LOOP {
		iam (Formant);
		praat_new (Formant_to_LPC (me, 1.0 / GET_REAL (L"Sampling frequency")), my name);
	}
END

FORM (Formant_formula, L"Formant: Formula", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0")
	NATURAL (L"left Formant range", L"1")
	NATURAL (L"right Formant range", L"5")
	LABEL (L"", L"Formant frequencies in odd numbered rows")
	LABEL (L"", L"Formant bandwidths in even numbered rows")
	SENTENCE (L"Formula", L"if row mod 2 = 1 and self[row,col]/self[row+1,col] < 5 then 0 else self fi")
	OK
DO
	wchar_t *expression = GET_STRING (L"Formula");
	LOOP {
		iam (Formant);
		Formant_formula (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"left Formant range"), GET_INTEGER (L"right Formant range"), interpreter, expression);
	}
END

/******************** Formant & Spectrogram ********************************************/

FORM (Formant_and_Spectrogram_to_IntensityTier, L"Formant & Spectrogram: To IntensityTier", L"Formant & Spectrogram: To IntensityTier...")
	NATURAL (L"Formant number", L"1")
	OK
DO
	Formant me = FIRST (Formant);
	long iformant = GET_INTEGER (L"Formant number");
	Spectrogram thee = FIRST (Spectrogram);
	autoIntensityTier him = Formant_and_Spectrogram_to_IntensityTier (me, thee, iformant);
	praat_new (him.transfer(), my name, L"_", Melder_integer (GET_INTEGER (L"Formant number")));
END


/********************LFCC ********************************************/

DIRECT (LFCC_help)
	Melder_help (L"LFCC");
END

FORM (LFCC_to_LPC, L"LFCC: To LPC", L"LFCC: To LPC...")
	INTEGER (L"Number of coefficients", L"0")
	OK
DO
	long ncof = GET_INTEGER (L"Number of coefficients");
	if (ncof < 0) {
		Melder_throw ("Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LFCC);
		praat_new (LFCC_to_LPC (me, ncof), my name);
	}
END

/********************LPC ********************************************/

DIRECT (LPC_help) Melder_help (L"LPC"); END

FORM (LPC_drawGain, L"LPC: Draw gain", L"LPC: Draw gain...")
	REAL (L"From time (seconds)", L"0.0")
	REAL (L"To time (seconds)", L"0.0 (=all)")
	REAL (L"Minimum gain", L"0.0")
	REAL (L"Maximum gain", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawGain (me, GRAPHICS, GET_REAL (L"From time"), GET_REAL (L"To time"),
			GET_REAL (L"Minimum gain"), GET_REAL (L"Maximum gain"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (LPC_getSamplingInterval)
	LOOP {
		iam (LPC);
		Melder_information (Melder_double (my samplingPeriod), L" seconds");
	}
END

FORM (LPC_getNumberOfCoefficients, L"LPC: Get number of coefficients", L"LPC: Get number of coefficients...")
	NATURAL (L"Frame number", L"1")
	OK
DO
	long iframe = GET_INTEGER (L"Frame number");
	LOOP {
		iam (LPC);
		if (iframe > my nx) {
			Melder_throw ("Frame number is too large.\n\nPlease choose a number between 1 and ", my nx);
		}
		Melder_information (Melder_integer ( (my d_frames[iframe]).nCoefficients), L" coefficients");
	}
END

FORM (LPC_drawPoles, L"LPC: Draw poles", L"LPC: Draw poles...")
	REAL (L"Time (seconds)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LPC);
		LPC_drawPoles (me, GRAPHICS, GET_REAL (L"Time"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (LPC_to_Formant)
	LOOP {
		iam (LPC);
		praat_new (LPC_to_Formant (me, 50), my name);
	}
END

DIRECT (LPC_to_Formant_keep_all)
	LOOP {
		iam (LPC);
		praat_new (LPC_to_Formant (me, 0), my name);
	}
END

FORM (LPC_to_LFCC, L"LPC: To LFCC", L"LPC: To LFCC...")
	INTEGER (L"Number of coefficients", L"0")
	OK
DO
	long ncof = GET_INTEGER (L"Number of coefficients");
	if (ncof < 0) {
		Melder_throw ("Number of coefficients must be greater or equal zero.");
	}
	LOOP {
		iam (LPC);
		praat_new (LPC_to_LFCC (me, ncof), my name);
	}
END

FORM (LPC_to_Polynomial, L"LPC: To Polynomial", L"LPC: To Polynomial (slice)...")
	REAL (L"Time (seconds)", L"0.0")
	OK
	DO
	LOOP {
		iam (LPC);
		double time = GET_REAL (L"Time");
		praat_new (LPC_to_Polynomial (me, time), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_Spectrum, L"LPC: To Spectrum", L"LPC: To Spectrum (slice)...")
	REAL (L"Time (seconds)", L"0.0")
	REAL (L"Minimum frequency resolution (Hz)", L"20.0")
	REAL (L"Bandwidth reduction (Hz)", L"0.0")
	REAL (L"De-emphasis frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		double time = GET_REAL (L"Time");
		praat_new (LPC_to_Spectrum (me, time, GET_REAL (L"Minimum frequency resolution"),
		GET_REAL (L"Bandwidth reduction"), GET_REAL (L"De-emphasis frequency")), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_Spectrogram, L"LPC: To Spectrogram", L"LPC: To Spectrogram...")
	REAL (L"Minimum frequency resolution (Hz)", L"20.0")
	REAL (L"Bandwidth reduction (Hz)", L"0.0")
	REAL (L"De-emphasis frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (LPC);
		praat_new (LPC_to_Spectrogram (me, GET_REAL (L"Minimum frequency resolution"),
			GET_REAL (L"Bandwidth reduction"), GET_REAL (L"De-emphasis frequency")), my name);
	}
END

FORM (LPC_to_VocalTract_special, L"LPC: To VocalTract", L"LPC: To VocalTract (slice, special)...")
	REAL (L"Time (s)", L"0.0")
	REAL (L"Glottal damping", L"0.1")
	BOOLEAN (L"Radiation damping", 1)
	BOOLEAN (L"Internal damping", 1)
	OK
DO
	double glottalDamping = GET_REAL (L"Glottal damping");
	bool radiationDamping = GET_INTEGER (L"Radiation damping");
	bool internalDamping = GET_INTEGER (L"Internal damping");
	LOOP {
		iam (LPC);
		double time = GET_REAL (L"Time");
		praat_new (LPC_to_VocalTract (me, time, glottalDamping, radiationDamping, internalDamping), my name, NUMstring_timeNoDot (time));
	}
END

FORM (LPC_to_VocalTract, L"LPC: To VocalTract", L"LPC: To VocalTract (slice)...")
	REAL (L"Time (s)", L"0.0")
	POSITIVE (L"Length (m)", L"0.17")
	OK
DO
	LOOP {
		iam (LPC);
		double time = GET_REAL (L"Time");
		praat_new (LPC_to_VocalTract (me, time, GET_REAL (L"Length")), my name, NUMstring_timeNoDot (time));
	}
END

DIRECT (LPC_downto_Matrix_lpc)
	LOOP {
		iam (LPC);
		praat_new (LPC_downto_Matrix_lpc (me), my name, L"_lpc");
	}
END

DIRECT (LPC_downto_Matrix_rc)
	LOOP {
		iam (LPC);
		praat_new (LPC_downto_Matrix_rc (me), my name, L"_rc");
	}
END

DIRECT (LPC_downto_Matrix_area)
	LOOP {
		iam (LPC);
		praat_new (LPC_downto_Matrix_area (me), my name, L"_area");
	}
END

/********************** Sound *******************************************/

FORM (Sound_to_PowerCepstrogram, L"Sound: To PowerCepstrogram", L"Sound: To PowerCepstrogram...")
	POSITIVE (L"Pitch floor (Hz)", L"60.0")
	POSITIVE (L"Time step (s)", L"0.002")
	POSITIVE (L"Maximum frequency (Hz)", L"5000.0")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram (me, GET_REAL (L"Pitch floor"), GET_REAL (L"Time step"), GET_REAL(L"Maximum frequency"),
			 GET_REAL (L"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END


FORM (Sound_to_PowerCepstrogram_hillenbrand, L"Sound: To PowerCepstrogram (hillenbrand)", L"Sound: To PowerCepstrogram...")
	POSITIVE (L"Pitch floor (Hz)", L"60.0")
	POSITIVE (L"Time step (s)", L"0.002")
	OK
DO
	LOOP {
		iam (Sound);
		autoPowerCepstrogram thee = Sound_to_PowerCepstrogram_hillenbrand (me, GET_REAL (L"Pitch floor"), GET_REAL (L"Time step"));
		praat_new (thee.transfer(), my name);
	}
END
	
FORM (Sound_to_Formant_robust, L"Sound: To Formant (robust)", L"Sound: To Formant (robust)...")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Max. number of formants", L"5")
	REAL (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50")
	POSITIVE (L"Number of std. dev.", L"1.5")
	NATURAL (L"Maximum number of iterations", L"5")
	REAL (L"Tolerance", L"0.000001")
	OK
DO
	LOOP {
		iam (Sound);
		praat_new (Sound_to_Formant_robust (me, GET_REAL (L"Time step"), GET_REAL (L"Max. number of formants"),
			GET_REAL (L"Maximum formant"), GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"), 50.0,
			GET_REAL (L"Number of std. dev."), GET_INTEGER (L"Maximum number of iterations"),
			GET_REAL (L"Tolerance"), 1), my name);
	}
END

static void Sound_to_LPC_addCommonFields (void *dia) {
	LABEL (L"", L"Warning 1:  for formant analysis, use \"To Formant\" instead.")
	LABEL (L"", L"Warning 2:  if you do use \"To LPC\", you may want to resample first.")
	LABEL (L"", L"Click Help for more details.")
	LABEL (L"", L"")
	NATURAL (L"Prediction order", L"16")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Time step (s)", L"0.005")
	REAL (L"Pre-emphasis frequency (Hz)", L"50.0")
}

static void Sound_to_LPC_checkCommonFields (void *dia, long *predictionOrder, double *analysisWindowDuration,
        double *timeStep, double *preemphasisFrequency) {
	*predictionOrder = GET_INTEGER (L"Prediction order");
	*analysisWindowDuration = GET_REAL (L"Window length");
	*timeStep = GET_REAL (L"Time step");
	*preemphasisFrequency = GET_REAL (L"Pre-emphasis frequency");
	if (*preemphasisFrequency < 0.0) {
		Melder_throw ("Pre-emphasis frequencies cannot be negative.");
	}
}

FORM (Sound_to_LPC_auto, L"Sound: To LPC (autocorrelation)", L"Sound: To LPC (autocorrelation)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, &preemphasisFrequency);
	LOOP {
		iam (Sound);
		praat_new (Sound_to_LPC_auto (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency), my name);
	}
END

FORM (Sound_to_LPC_covar, L"Sound: To LPC (covariance)", L"Sound: To LPC (covariance)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		praat_new (Sound_to_LPC_covar (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency), my name);
	}
END

FORM (Sound_to_LPC_burg, L"Sound: To LPC (burg)", L"Sound: To LPC (burg)...")
	Sound_to_LPC_addCommonFields (dia);
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, & timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		praat_new (Sound_to_LPC_burg (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency), my name);
	}
END

FORM (Sound_to_LPC_marple, L"Sound: To LPC (marple)", L"Sound: To LPC (marple)...")
	Sound_to_LPC_addCommonFields (dia);
	POSITIVE (L"Tolerance 1", L"1e-6")
	POSITIVE (L"Tolerance 2", L"1e-6")
	OK
DO
	long numberOfPoles;
	double analysisWindowDuration, timeStep, preemphasisFrequency;
	Sound_to_LPC_checkCommonFields (dia, & numberOfPoles, & analysisWindowDuration, &timeStep, & preemphasisFrequency);
	LOOP {
		iam (Sound);
		praat_new (Sound_to_LPC_marple (me, numberOfPoles, analysisWindowDuration, timeStep, preemphasisFrequency,
		GET_REAL (L"Tolerance 1"), GET_REAL (L"Tolerance 2")), my name);
	}
END

FORM (Sound_to_MFCC, L"Sound: To MFCC", L"Sound: To MFCC...")
	NATURAL (L"Number of coefficients", L"12")
	POSITIVE (L"Window length (s)", L"0.015")
	POSITIVE (L"Time step (s)", L"0.005")
	LABEL (L"", L"Filter bank parameters")
	POSITIVE (L"Position of first filter (mel)", L"100.0")
	POSITIVE (L"Distance between filters (mel)", L"100.0")
	REAL (L"Maximum frequency (mel)", L"0.0");
	OK
DO
	long p = GET_INTEGER (L"Number of coefficients");
	if (p > 24) {
		Melder_throw ("Number of coefficients must be < 25.");
	}
	LOOP {
		iam (Sound);
		praat_new (Sound_to_MFCC (me, p, GET_REAL (L"Window length"),
			GET_REAL (L"Time step"), GET_REAL (L"Position of first filter"),
			GET_REAL (L"Maximum frequency"), GET_REAL (L"Distance between filters")), my name);
	}
END

FORM (VocalTract_drawSegments, L"VocalTract: Draw segments", 0)
	POSITIVE (L"Maximum length (cm)", L"20.0")
	POSITIVE (L"Maximum area (cm^2)", L"90.0")
	BOOLEAN (L"Closed at glottis", 1)
	OK
DO
	autoPraatPicture picture;
	double maxLength = GET_REAL (L"Maximum length") / 100;
	double maxArea = GET_REAL (L"Maximum area") / 10000;
	bool closedAtGlottis = GET_INTEGER (L"Closed at glottis");
	LOOP {
		iam (VocalTract);
		VocalTract_drawSegments (me, GRAPHICS, maxLength, maxArea, closedAtGlottis);
	}
END

DIRECT (VocalTract_getLength)
	LOOP {
		iam (VocalTract);
		double length = my xmax - my xmin;
		if (length <= 0.02) {
			length = NUMundefined;
		}
		Melder_information (Melder_double (length), L" m");
	}
END

FORM (VocalTract_setLength, L"", 0)
	POSITIVE (L"New length (m)", L"0.17")
	OK
DO
	double newLength = GET_REAL (L"New length");
	LOOP {
		iam (VocalTract);
		VocalTract_setLength (me, newLength);
	}
END

FORM (VocalTract_to_VocalTractTier, L"VocalTract: To VocalTractTier", 0)
	REAL (L"Tier start time (s)", L"0.0")
	REAL (L"Tier end time (s)", L"1.0")
	REAL (L"Insert at time (s)", L"0.5")
	OK
DO
	double xmin = GET_REAL (L"Tier start time");
	double xmax = GET_REAL (L"Tier end time");
	double time = GET_REAL (L"Insert at time");
	REQUIRE (xmin < xmax, L"The start time must be before the end time.")
	REQUIRE (time >= xmin and time <= xmax, L"The insert time must be between start en end time.")
	LOOP {
		iam (VocalTract);
		autoVocalTractTier thee = VocalTract_to_VocalTractTier (me, xmin, xmax, time);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (VocalTractTier_help)
	Melder_help (L"VocalTractTier");
END

FORM (VocalTractTier_to_LPC, L"VocalTractTier: To LPC", 0)
	POSITIVE (L"Time step", L"0.005")
	OK
DO
	LOOP {
		iam (VocalTractTier);
		praat_new (VocalTractTier_to_LPC (me, GET_REAL (L"Time step")), my name);
	}
END

FORM (VocalTractTier_to_VocalTract, L"", 0)
	REAL (L"Time (s)", L"0.1")
	OK
DO
	double time = GET_REAL (L"Time");
	LOOP {
		iam (VocalTractTier);
		praat_new (VocalTractTier_to_VocalTract (me, time), my name);
	}
END

FORM (VocalTractTier_addVocalTract, L"VocalTractTier: Add VocalTract", 0)
	REAL (L"Time", L"0.1")
	OK
DO
	VocalTractTier me = FIRST (VocalTractTier);
	VocalTract thee = FIRST (VocalTract);
	VocalTractTier_addVocalTract (me, GET_REAL (L"Time"), thee);
	praat_dataChanged (me);
END

/******************* LPC & Sound *************************************/

FORM (LPC_and_Sound_filter, L"LPC & Sound: Filter", L"LPC & Sound: Filter...")
	BOOLEAN (L"Use LPC gain", 0)
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	praat_new (LPC_and_Sound_filter (me , s, GET_INTEGER (L"Use LPC gain")), my name);
END

FORM (LPC_and_Sound_filterWithFilterAtTime, L"LPC & Sound: Filter with one filter at time",
      L"LPC & Sound: Filter with filter at time...")
	OPTIONMENU (L"Channel", 2)
	OPTION (L"Both")
	OPTION (L"Left")
	OPTION (L"Right")
	REAL (L"Use filter at time (s)", L"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	long channel = GET_INTEGER (L"Channel") - 1;
	praat_new (LPC_and_Sound_filterWithFilterAtTime (me , s, channel, GET_REAL (L"Use filter at time")), my name);
END

DIRECT (LPC_and_Sound_filterInverse)
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	praat_new (LPC_and_Sound_filterInverse (me , s), my name);
END

FORM (LPC_and_Sound_filterInverseWithFilterAtTime, L"LPC & Sound: Filter (inverse) with filter at time",
      L"LPC & Sound: Filter (inverse) with filter at time...")
	OPTIONMENU (L"Channel", 2)
	OPTION (L"Both")
	OPTION (L"Left")
	OPTION (L"Right")
	REAL (L"Use filter at time (s)", L"0.0")
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	long channel = GET_INTEGER (L"Channel") - 1;
	praat_new (LPC_and_Sound_filterInverseWithFilterAtTime (me , s, channel, GET_REAL (L"Use filter at time")), my name);
END

FORM (LPC_and_Sound_to_LPC_robust, L"Robust LPC analysis", L"LPC & Sound: To LPC (robust)...")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis frequency (Hz)", L"50.0")
	POSITIVE (L"Number of std. dev.", L"1.5")
	NATURAL (L"Maximum number of iterations", L"5")
	REAL (L"Tolerance", L"0.000001")
	BOOLEAN (L"Variable location", 0)
	OK
DO
	LPC me = FIRST (LPC);
	Sound s = FIRST (Sound);
	praat_new (LPC_and_Sound_to_LPC_robust (me, s, GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis frequency"),
		GET_REAL (L"Number of std. dev."), GET_INTEGER (L"Maximum number of iterations"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Variable location")), my name, L"_r");
END

extern void praat_TimeTier_query_init (ClassInfo klas);
extern void praat_TimeTier_modify_init (ClassInfo klas);
void praat_uvafon_LPC_init (void);
void praat_uvafon_LPC_init (void) {
	Thing_recognizeClassesByName (classCepstrumc, classPowerCepstrum, classCepstrogram, classPowerCepstrogram, classLPC, classLFCC, classMFCC, classVocalTractTier, NULL);

	praat_addAction1 (classPowerCepstrum, 0, L"PowerCepstrum help", 0, 0, DO_PowerCepstrum_help);
	praat_addAction1 (classPowerCepstrum, 0, L"Draw...", 0, 0, DO_PowerCepstrum_draw);
	praat_addAction1 (classPowerCepstrum, 0, L"Draw tilt line...", 0, 0, DO_PowerCepstrum_drawTiltLine);
	praat_addAction1 (classCepstrum, 0, L"Draw (linear)...", 0, praat_HIDDEN, DO_Cepstrum_drawLinear);
	praat_addAction1 (classCepstrum, 0, L"Down to PowerCepstrum", 0, 0, DO_Cepstrum_downto_PowerCepstrum);
	
	praat_addAction1 (classPowerCepstrum, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, L"Get peak...", 0, 1, DO_PowerCepstrum_getPeak);
		praat_addAction1 (classPowerCepstrum, 0, L"Get quefrency of peak...", 0, 1, DO_PowerCepstrum_getQuefrencyOfPeak);
		praat_addAction1 (classPowerCepstrum, 0, L"Get peak prominence (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_PowerCepstrum_getPeakProminence_hillenbrand);
		praat_addAction1 (classPowerCepstrum, 0, L"Get peak prominence...", 0, 1, DO_PowerCepstrum_getPeakProminence);
		praat_addAction1 (classPowerCepstrum, 0, L"Get tilt line slope...", 0, 1, DO_PowerCepstrum_getTiltLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, L"Get tilt line intercept...", 0, 1, DO_PowerCepstrum_getTiltLineIntercept);
		praat_addAction1 (classPowerCepstrum, 0, L"Get rhamonics to noise ratio...", 0, 1, DO_PowerCepstrum_getRNR);
	praat_addAction1 (classPowerCepstrum, 1, L"Modify -", 0, 0, 0);
		praat_addAction1 (classPowerCepstrum, 0, L"Formula...", 0, 1, DO_PowerCepstrum_formula);
		praat_addAction1 (classPowerCepstrum, 0, L"Subtract tilt (in-line)...", 0, 1, DO_PowerCepstrum_subtractTilt_inline);
		praat_addAction1 (classPowerCepstrum, 0, L"Smooth (in-line)...", 0, 1, DO_PowerCepstrum_smooth_inline);

	praat_addAction1 (classPowerCepstrum, 0, L"Subtract tilt...", 0, 0, DO_PowerCepstrum_subtractTilt);
	praat_addAction1 (classPowerCepstrum, 0, L"Smooth...", 0, 0, DO_PowerCepstrum_smooth);
	praat_addAction1 (classCepstrum, 0, L"To Spectrum", 0, praat_HIDDEN, DO_Cepstrum_to_Spectrum);
	praat_addAction1 (classPowerCepstrum, 0, L"To Matrix", 0, 0, DO_PowerCepstrum_to_Matrix);

	praat_addAction1 (classPowerCepstrogram, 0, L"PowerCepstrogram help", 0, 0, DO_PowerCepstrogram_help);
	praat_addAction1 (classPowerCepstrogram, 0, L"Paint...", 0, 0, DO_PowerCepstrogram_paint);
	praat_addAction1 (classPowerCepstrogram, 1, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 1, L"Query quefrency domain", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, L"Get start quefrency", 0, 2, DO_PowerCepstrogram_getStartQuefrency);
			praat_addAction1 (classPowerCepstrogram, 1, L"Get end quefrency", 0, 2, DO_PowerCepstrogram_getEndQuefrency);
		praat_addAction1 (classPowerCepstrogram, 1, L"Query quefrency sampling", 0, 1, 0);
			praat_addAction1 (classPowerCepstrogram, 1, L"Get number of quefrency bins", 0, 2, DO_PowerCepstrogram_getNumberOfQuefrencyBins);
			praat_addAction1 (classPowerCepstrogram, 1, L"Get quefrency step", 0, 2, DO_PowerCepstrogram_getQuefrencyStep);
		praat_addAction1 (classPowerCepstrogram, 0, L"Get CPPS (hillenbrand)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_PowerCepstrogram_getCPPS_hillenbrand);
		praat_addAction1 (classPowerCepstrogram, 0, L"Get CPPS...", 0, 1, DO_PowerCepstrogram_getCPPS);
	praat_addAction1 (classPowerCepstrogram, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 0, L"Formula...", 0, 1, DO_PowerCepstrogram_formula);
		praat_addAction1 (classPowerCepstrogram, 0, L"Subtract tilt (in-line)...", 0, 1, DO_PowerCepstrogram_subtractTilt_inline);
	praat_addAction1 (classPowerCepstrogram, 0, L"To PowerCepstrum (slice)...", 0, 0, DO_PowerCepstrogram_to_PowerCepstrum_slice);
	praat_addAction1 (classPowerCepstrogram, 0, L"Smooth...", 0, 0, DO_PowerCepstrogram_smooth);
	praat_addAction1 (classPowerCepstrogram, 0, L"Subtract tilt...", 0, 0, DO_PowerCepstrogram_subtractTilt);
	praat_addAction1 (classPowerCepstrogram, 0, L"To Table (hillenbrand)...", 0, praat_HIDDEN, DO_PowerCepstrogram_to_Table_hillenbrand);
	praat_addAction1 (classPowerCepstrogram, 0, L"To Table (peak prominence)...", 0, praat_HIDDEN, DO_PowerCepstrogram_to_Table_cpp);
	praat_addAction1 (classPowerCepstrogram, 0, L"To Matrix", 0, 0, DO_PowerCepstrogram_to_Matrix);

	praat_addAction1 (classCepstrumc, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, L"To LPC", 0, 0, DO_Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, L"To DTW...", 0, 0, DO_Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classCepstrumc, 0, L"To Matrix", 0, 0, DO_Cepstrumc_to_Matrix);

	praat_addAction1 (classFormant, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classFormant, 0, L"To LPC...", 0, 0, DO_Formant_to_LPC);
	praat_addAction1 (classFormant, 0, L"Formula...", L"Formula (bandwidths)...", 1, DO_Formant_formula);
	praat_addAction2 (classFormant, 1, classSpectrogram, 1, L"To IntensityTier...", 0, 0, DO_Formant_and_Spectrogram_to_IntensityTier);

	praat_addAction1 (classLFCC, 0, L"LFCC help", 0, 0, DO_LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, L"To LPC...", 0, 0, DO_LFCC_to_LPC);

	praat_addAction1 (classLPC, 0, L"LPC help", 0, 0, DO_LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classLPC, 0, L"Draw gain...", 0, 1, DO_LPC_drawGain);
	praat_addAction1 (classLPC, 0, L"Draw poles...", 0, 1, DO_LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, 0, 0, 0);
	praat_TimeFrameSampled_query_init (classLPC);
	praat_addAction1 (classLPC, 1, L"Get sampling interval", 0, 1, DO_LPC_getSamplingInterval);
	praat_addAction1 (classLPC, 1, L"Get number of coefficients...", 0, 1, DO_LPC_getNumberOfCoefficients);
	praat_addAction1 (classLPC, 0, L"Extract", 0, 0, 0);

	praat_addAction1 (classLPC, 0, L"To Spectrum (slice)...", 0, 0, DO_LPC_to_Spectrum);
	praat_addAction1 (classLPC, 0, L"To VocalTract (slice)...", 0, 0, DO_LPC_to_VocalTract);
	praat_addAction1 (classLPC, 0, L"To VocalTract (slice, special)...", 0, 0, DO_LPC_to_VocalTract_special);
	praat_addAction1 (classLPC, 0, L"To Polynomial (slice)...", 0, 0, DO_LPC_to_Polynomial);
	praat_addAction1 (classLPC, 0, L"Down to Matrix (lpc)", 0, 0, DO_LPC_downto_Matrix_lpc);
	praat_addAction1 (classLPC, 0, L"Down to Matrix (rc)", 0, praat_HIDDEN, DO_LPC_downto_Matrix_rc);
	praat_addAction1 (classLPC, 0, L"Down to Matrix (area)", 0, praat_HIDDEN, DO_LPC_downto_Matrix_area);
	praat_addAction1 (classLPC, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classLPC, 0, L"To Formant", 0, 0, DO_LPC_to_Formant);
	praat_addAction1 (classLPC, 0, L"To Formant (keep all)", 0, 0, DO_LPC_to_Formant_keep_all);
	praat_addAction1 (classLPC, 0, L"To LFCC...", 0, 0, DO_LPC_to_LFCC);
	praat_addAction1 (classLPC, 0, L"To Spectrogram...", 0, 0, DO_LPC_to_Spectrogram);

	praat_addAction2 (classLPC, 1, classSound, 1, L"Analyse", 0, 0, 0);
	praat_addAction2 (classLPC, 1, classSound, 1, L"Filter...", 0, 0, DO_LPC_and_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, L"Filter (inverse)", 0, 0, DO_LPC_and_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, L"To LPC (robust)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_LPC_and_Sound_to_LPC_robust);
	praat_addAction2 (classLPC, 1, classSound, 1, L"Filter with filter at time...", 0, 0, DO_LPC_and_Sound_filterWithFilterAtTime);
	praat_addAction2 (classLPC, 1, classSound, 1, L"Filter (inverse) with filter at time...", 0, 0, DO_LPC_and_Sound_filterInverseWithFilterAtTime);


	praat_addAction1 (classSound, 0, L"To LPC (autocorrelation)...", L"To Formant (sl)...", 1, DO_Sound_to_LPC_auto);
	praat_addAction1 (classSound, 0, L"To LPC (covariance)...", L"To LPC (autocorrelation)...", 1, DO_Sound_to_LPC_covar);
	praat_addAction1 (classSound, 0, L"To LPC (burg)...", L"To LPC (covariance)...", 1, DO_Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, L"To LPC (marple)...", L"To LPC (burg)...", 1, DO_Sound_to_LPC_marple);
	praat_addAction1 (classSound, 0, L"To MFCC...", L"To LPC (marple)...", 1, DO_Sound_to_MFCC);
	praat_addAction1 (classSound, 0, L"To Formant (robust)...", L"To Formant (sl)...", 2, DO_Sound_to_Formant_robust);
	praat_addAction1 (classSound, 0, L"To PowerCepstrogram...", L"To Harmonicity (gne)...", 1, DO_Sound_to_PowerCepstrogram);
	praat_addAction1 (classSound, 0, L"To PowerCepstrogram (hillenbrand)...", L"To Harmonicity (gne)...", praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_PowerCepstrogram_hillenbrand);
	
	praat_addAction1 (classVocalTract, 0, L"Draw segments...", L"Draw", 0, DO_VocalTract_drawSegments);
	praat_addAction1 (classVocalTract, 1, L"Get length", L"Draw segments...", 0, DO_VocalTract_getLength);
	praat_addAction1 (classVocalTract, 1, L"Set length", L"Formula...", 0, DO_VocalTract_getLength);
	praat_addAction1 (classVocalTract, 0, L"To VocalTractTier...", L"To Spectrum...", 0, DO_VocalTract_to_VocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, L"VocalTractTier help", 0, 0, DO_VocalTractTier_help);
	praat_addAction1 (classVocalTractTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, L"To LPC...", 0, 0, DO_VocalTractTier_to_LPC);
	praat_addAction1 (classVocalTractTier, 0, L"To VocalTract...", 0, 0, DO_VocalTractTier_to_VocalTract);
	praat_addAction2 (classVocalTractTier, 1, classVocalTract, 1, L"Add VocalTract...", 0, 0, DO_VocalTractTier_addVocalTract);
	INCLUDE_MANPAGES (manual_LPC_init)

}

/* End of file praat_LPC_init.c */
