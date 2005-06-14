/* praat_Fon.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2005/05/26
 */

#include "praat.h"

#include "AmplitudeTier.h"
#include "AmplitudeTierEditor.h"
#include "Cochleagram_and_Excitation.h"
#include "Distributions_and_Strings.h"
#include "Distributions_and_Transition.h"
#include "DurationTierEditor.h"
#include "Excitation_to_Formant.h"
#include "FormantTier.h"
#include "Harmonicity.h"
#include "IntensityTier.h"
#include "IntensityTierEditor.h"
#include "LongSound.h"
#include "Ltas.h"
#include "ManipulationEditor.h"
#include "Matrix_and_Pitch.h"
#include "Matrix_and_PointProcess.h"
#include "Matrix_and_Polygon.h"
#include "PairDistribution.h"
#include "ParamCurve.h"
#include "Pitch_AnyTier_to_PitchTier.h"
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
#include "SpectrogramEditor.h"
#include "Spectrum_and_Spectrogram.h"
#include "Spectrum_to_Excitation.h"
#include "Spectrum_to_Formant.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "Table.h"
#include "TextGridEditor.h"
#include "TextGrid_extensions.h"
#include "TextTier_Pitch.h"
#include "VocalTract.h"
#include "VoiceAnalysis.h"
#include "WordList.h"

static const char *STRING_FROM_TIME_SECONDS = "left Time range (s)";
static const char *STRING_TO_TIME_SECONDS = "right Time range (s)";
static const char *STRING_FROM_TIME = "left Time range";
static const char *STRING_TO_TIME = "right Time range";
static const char *STRING_FROM_FREQUENCY_HZ = "left Frequency range (Hz)";
static const char *STRING_TO_FREQUENCY_HZ = "right Frequency range (Hz)";
static const char *STRING_FROM_FREQUENCY = "left Frequency range";
static const char *STRING_TO_FREQUENCY = "right Frequency range";

/***** Common dialog contents. *****/

static void dia_timeRange (Any dia) {
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
}
static void dia_Vector_getExtremum (Any dia) {
	Any radio;
	dia_timeRange (dia);
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
}
static void dia_Vector_getValue (Any dia) {
	Any radio;
	REAL ("Time (s)", "0.5")
	RADIO ("Interpolation", 3)
	RADIOBUTTON ("Nearest")
	RADIOBUTTON ("Linear")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
}

static int getTminTmaxFminFmax (Any dia, double *tmin, double *tmax, double *fmin, double *fmax) {
	*tmin = GET_REAL (STRING_FROM_TIME);
	*tmax = GET_REAL (STRING_TO_TIME);
	*fmin = GET_REAL (STRING_FROM_FREQUENCY);
	*fmax = GET_REAL (STRING_TO_FREQUENCY);
	REQUIRE (*fmax > *fmin, "Maximum frequency must be greater than minimum frequency.")
	return 1;
}
#define GET_TMIN_TMAX_FMIN_FMAX \
	double tmin, tmax, fmin, fmax; \
	if (! getTminTmaxFminFmax (dia, & tmin, & tmax, & fmin, & fmax)) return 0;

/***** Two auxiliary routines, exported. *****/

int praat_Fon_formula (Any dia);
int praat_Fon_formula (Any dia) {
	int IOBJECT;
	WHERE_DOWN (SELECTED) {
		Matrix_formula (OBJECT, GET_STRING ("formula"), NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
	return 1;
}

Graphics Movie_create (const char *title, int width, int height);
Graphics Movie_create (const char *title, int width, int height) {
	static Graphics graphics;
	static Widget shell, dialog, drawingArea;
	if (! graphics) {
		dialog = XmCreateFormDialog (praat.topShell, "Movie", NULL, 0);
		shell = XtParent (dialog);
		drawingArea = XmCreateDrawingArea (dialog, "movingArea", NULL, 0);
		XtVaSetValues (shell, XmNx, 100, XmNy, 100, XmNtitle, title, XmNwidth, width + 2, XmNheight, height + 2,
			XmNdeleteResponse, XmUNMAP, NULL);
		XtVaSetValues (drawingArea, XmNwidth, width, XmNheight, height, NULL);
		XtManageChild (drawingArea);
		XtManageChild (dialog);
		graphics = Graphics_create_xmdrawingarea (drawingArea);
	}
	XtVaSetValues (XtParent (dialog), XmNtitle, title, XmNwidth, width + 2, XmNheight, height + 2, NULL);
	XtVaSetValues (drawingArea, XmNwidth, width, XmNheight, height, NULL);
	XtManageChild (dialog);
	XMapRaised (XtDisplay (shell), XtWindow (shell));
	return graphics;
}

/***** AMPLITUDETIER *****/

FORM (AmplitudeTier_addPoint, "Add one point", "AmplitudeTier: Add point...")
	REAL ("Time (s)", "0.5")
	REAL ("Sound pressure (Pa)", "0.8")
	OK
DO
	WHERE (SELECTED) {
		if (! RealTier_addPoint (OBJECT, GET_REAL ("Time"), GET_REAL ("Sound pressure"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (AmplitudeTier_create, "Create empty AmplitudeTier", NULL)
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double startingTime = GET_REAL ("Starting time"), finishingTime = GET_REAL ("Finishing time");
	REQUIRE (finishingTime > startingTime, "Finishing time must be greater than starting time.")
	if (! praat_new (AmplitudeTier_create (startingTime, finishingTime), GET_STRING ("Name"))) return 0;
END

DIRECT (AmplitudeTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

DIRECT (AmplitudeTier_downto_TableOfReal)
	EVERY_TO (AmplitudeTier_downto_TableOfReal (OBJECT))
END

DIRECT (AmplitudeTier_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit an AmplitudeTier from batch.");
	} else {
		Sound sound = NULL;
		WHERE (SELECTED)
			if (CLASS == classSound) sound = OBJECT;
		WHERE (SELECTED && CLASS == classAmplitudeTier)
			if (! praat_installEditor (AmplitudeTierEditor_create (praat.topShell, FULL_NAME,
				OBJECT, sound, TRUE), IOBJECT)) return 0;
	}
END

FORM (AmplitudeTier_formula, "AmplitudeTier: Formula", "AmplitudeTier: Formula...")
	LABEL ("", "# ncol = the number of points")
	LABEL ("", "for col from 1 to ncol")
	LABEL ("", "   # x = the time of the colth point, in seconds")
	LABEL ("", "   # self = the value of the colth point, in Pascal")
	LABEL ("", "   self = `formula'")
	LABEL ("", "endfor")
	TEXTFIELD ("formula", "- self ; upside down")
	OK
DO
	WHERE_DOWN (SELECTED) {
		RealTier_formula (OBJECT, GET_STRING ("formula"), NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

static void dia_AmplitudeTier_getRangeProperty (Any dia) {
	REAL ("Shortest period (s)", "0.0001")
	REAL ("Longest period (s)", "0.02")
	POSITIVE ("Maximum amplitude factor", "1.6")
}

FORM (AmplitudeTier_getShimmer_local, "AmplitudeTier: Get shimmer (local)", "AmplitudeTier: Get shimmer (local)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_local (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (AmplitudeTier_getShimmer_local_dB, "AmplitudeTier: Get shimmer (local, dB)", "AmplitudeTier: Get shimmer (local, dB)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_local_dB (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (AmplitudeTier_getShimmer_apq3, "AmplitudeTier: Get shimmer (apq3)", "AmplitudeTier: Get shimmer (apq3)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_apq3 (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (AmplitudeTier_getShimmer_apq5, "AmplitudeTier: Get shimmer (apq5)", "AmplitudeTier: Get shimmer (apq5)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_apq5 (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (AmplitudeTier_getShimmer_apq11, "AmplitudeTier: Get shimmer (apq11)", "AmplitudeTier: Get shimmer (apq11)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_apq11 (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (AmplitudeTier_getShimmer_dda, "AmplitudeTier: Get shimmer (dda)", "AmplitudeTier: Get shimmer (dda)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (AmplitudeTier_getShimmer_dda (ONLY (classAmplitudeTier),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum amplitude factor")), NULL);
END

/*FORM (AmplitudeTier_getValueAtTime, "Get AmplitudeTier value", "AmplitudeTier: Get value at time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL ("Time")), "Pa");
END
	
FORM (AmplitudeTier_getValueAtIndex, "Get AmplitudeTier value", "AmplitudeTier: Get value at index...")
	INTEGER ("Point number", "10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER ("Point number")), "Pa");
END*/

DIRECT (AmplitudeTier_help) Melder_help ("AmplitudeTier"); END

FORM (AmplitudeTier_to_IntensityTier, "AmplitudeTier: To IntensityTier", "AmplitudeTier: To IntensityTier...")
	REAL ("Threshold (dB)", "-10000.0")
	OK
DO
	EVERY_TO (AmplitudeTier_to_IntensityTier (OBJECT, GET_REAL ("Threshold")))
END

FORM (AmplitudeTier_to_Sound, "AmplitudeTier: To Sound (pulse train)", "AmplitudeTier: To Sound (pulse train)...")
	POSITIVE ("Sampling frequency (Hz)", "22050")
	NATURAL ("Interpolation depth (samples)", "2000")
	OK
DO
	EVERY_TO (AmplitudeTier_to_Sound (OBJECT, GET_REAL ("Sampling frequency"), GET_INTEGER ("Interpolation depth")))
END

DIRECT (info_AmplitudeTier_Sound_edit)
	Melder_information ("To include a copy of a Sound in your AmplitudeTier editor:\n"
		"   select an AmplitudeTier and a Sound, and click \"Edit\".");
END

/***** AMPLITUDETIER & SOUND *****/

DIRECT (Sound_AmplitudeTier_multiply)
	Sound sound = ONLY (classSound);
	if (! praat_new (Sound_AmplitudeTier_multiply (sound, ONLY (classAmplitudeTier)), "%s_amp", sound -> name)) return 0;
END

/***** ANYTIER (generic) *****/

DIRECT (AnyTier_into_TextGrid)
	TextGrid grid = TextGrid_create (1e30, -1e30, NULL, NULL);
	WHERE (SELECTED) if (! TextGrid_add (grid, OBJECT)) { forget (grid); return 0; }
	if (! praat_new (grid, "grid")) return 0;
END

/***** COCHLEAGRAM *****/

FORM (Cochleagram_difference, "Cochleagram difference", 0)
	dia_timeRange (dia);
	OK
DO
	Data coch1 = NULL, coch2 = NULL;
	WHERE (SELECTED && CLASS == classCochleagram)
		if (coch1) coch2 = OBJECT; else coch1 = OBJECT;
	Melder_informationReal (Cochleagram_difference ((Cochleagram) coch1, (Cochleagram) coch2,
			GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Hertz (root-mean-square)");
END

FORM (Cochleagram_formula, "Cochleagram Formula", "Cochleagram: Formula...")
	LABEL ("label", "`x' is time in seconds, `y' is place in Bark")
	LABEL ("label", "y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

DIRECT (Cochleagram_help) Melder_help ("Cochleagram"); END

DIRECT (Cochleagram_movie)
	Graphics g = Movie_create ("Cochleagram movie", 300, 300);
	WHERE (SELECTED) Matrix_movie (OBJECT, g);
END

FORM (Cochleagram_paint, "Paint Cochleagram", 0)
	dia_timeRange (dia);
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Cochleagram_paint (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Garnish")))
END

FORM (Cochleagram_to_Excitation, "From Cochleagram to Excitation", 0)
	REAL ("Time (s)", "0.0")
	OK
DO
	EVERY_TO (Cochleagram_to_Excitation (OBJECT, GET_REAL ("Time")))
END

DIRECT (Cochleagram_to_Matrix)
	EVERY_TO (Cochleagram_to_Matrix (OBJECT))
END

/***** DISTRIBUTIONS *****/

DIRECT (Distributionses_add)
	Collection me = Collection_create (classDistributions, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new (Distributions_addMany (me), "added")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (Distributionses_getMeanAbsoluteDifference, "Get mean difference", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Distributions me = NULL, thee = NULL;
	WHERE (SELECTED) if (me) thee = OBJECT; else me = OBJECT;
	Melder_informationReal (Distributionses_getMeanAbsoluteDifference (me, thee, GET_INTEGER ("Column number")), NULL);
END

FORM (Distributions_getProbability, "Get probability", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("String", "")
	OK
DO
	Melder_informationReal (Distributions_getProbability (ONLY_OBJECT,
		GET_STRING ("String"), GET_INTEGER ("Column number")), NULL);
END

DIRECT (Distributions_help) Melder_help ("Distributions"); END

FORM (Distributions_to_Strings, "To Strings", 0)
	NATURAL ("Column number", "1")
	NATURAL ("Number of strings", "1000")
	OK
DO
	EVERY_TO (Distributions_to_Strings (OBJECT, GET_INTEGER ("Column number"), GET_INTEGER ("Number of strings")))
END

FORM (Distributions_to_Strings_exact, "To Strings (exact)", 0)
	NATURAL ("Column number", "1")
	OK
DO
	EVERY_TO (Distributions_to_Strings_exact (OBJECT, GET_INTEGER ("Column number")))
END

FORM (Distributions_to_Transition, "To Transition", 0)
	NATURAL ("Environment", "1")
	BOOLEAN ("Greedy", 1)
	OK
DO
	if (! praat_new (Distributions_to_Transition (ONLY_OBJECT, NULL, GET_INTEGER ("Environment"),
		NULL, GET_INTEGER ("Greedy")), NULL)) return 0;
END

FORM (Distributions_to_Transition_adj, "To Transition", 0)
	NATURAL ("Environment", "1")
	BOOLEAN ("Greedy", 1)
	OK
DO
	if (! praat_new (Distributions_to_Transition (ONLY (classDistributions), NULL,
		GET_INTEGER ("Environment"), ONLY (classTransition), GET_INTEGER ("Greedy")), NULL)) return 0;
END

FORM (Distributions_to_Transition_noise, "To Transition (noise)", 0)
	NATURAL ("Environment", "1")
	BOOLEAN ("Greedy", 1)
	OK
DO
	Distributions underlying = NULL, surface = NULL;
	WHERE (SELECTED) if (underlying) surface = OBJECT; else underlying = OBJECT;
	if (! praat_new (Distributions_to_Transition (underlying, surface, GET_INTEGER ("Environment"),
		NULL, GET_INTEGER ("Greedy")), NULL)) return 0;
END

FORM (Distributions_to_Transition_noise_adj, "To Transition (noise)", 0)
	NATURAL ("Environment", "1")
	BOOLEAN ("Greedy", 1)
	OK
DO
	Distributions underlying = NULL, surface = NULL;
	WHERE (SELECTED && CLASS == classDistributions)
		if (underlying) surface = OBJECT; else underlying = OBJECT;
	if (! praat_new (Distributions_to_Transition (underlying, surface, GET_INTEGER ("Environment"),
		ONLY (classTransition), GET_INTEGER ("Greedy")), NULL)) return 0;
END

/***** DISTRIBUTIONS & TRANSITION *****/

DIRECT (Distributions_Transition_map)
	if (! praat_new (Distributions_Transition_map (ONLY (classDistributions), ONLY (classTransition)),
		"surface")) return 0;
END

/***** DURATIONTIER *****/

FORM (DurationTier_addPoint, "Add one point to DurationTier", "DurationTier: Add point...")
	REAL ("Time (s)", "0.5")
	REAL ("Relative duration", "1.5")
	OK
DO
	WHERE (SELECTED) {
		if (! RealTier_addPoint (OBJECT, GET_REAL ("Time"), GET_REAL ("Relative duration"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (DurationTier_create, "Create empty DurationTier", "Create DurationTier...")
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double startingTime = GET_REAL ("Starting time"), finishingTime = GET_REAL ("Finishing time");
	REQUIRE (finishingTime > startingTime, "Finishing time must be greater than starting time.")
	if (! praat_new (DurationTier_create (startingTime, finishingTime), GET_STRING ("Name"))) return 0;
END

DIRECT (DurationTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

DIRECT (DurationTier_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a DurationTier from batch.");
	} else {
		Sound sound = NULL;
		WHERE (SELECTED)
			if (CLASS == classSound) sound = OBJECT;
		WHERE (SELECTED && CLASS == classDurationTier)
			if (! praat_installEditor (DurationTierEditor_create (praat.topShell, FULL_NAME,
				OBJECT, sound, TRUE), IOBJECT)) return 0;
	}
END

FORM (DurationTier_formula, "DurationTier: Formula", "DurationTier: Formula...")
	LABEL ("", "# ncol = the number of points")
	LABEL ("", "for col from 1 to ncol")
	LABEL ("", "   # x = the time of the colth point, in seconds")
	LABEL ("", "   # self = the value of the colth point, in relative units")
	LABEL ("", "   self = `formula'")
	LABEL ("", "endfor")
	TEXTFIELD ("formula", "self * 1.5 ; slow down")
	OK
DO
	WHERE_DOWN (SELECTED) {
		RealTier_formula (OBJECT, GET_STRING ("formula"), NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (DurationTier_getTargetDuration, "Get target duration", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "1.0")
	OK
DO
	Melder_informationReal (RealTier_getArea (ONLY_OBJECT,
		GET_REAL ("left Time range"), GET_REAL ("right Time range")), "seconds");
END

FORM (DurationTier_getValueAtTime, "Get DurationTier value", "DurationTier: Get value at time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL ("Time")), NULL);
END
	
FORM (DurationTier_getValueAtIndex, "Get DurationTier value", "Duration: Get value at index...")
	INTEGER ("Point number", "10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER ("Point number")), NULL);
END

DIRECT (DurationTier_help) Melder_help ("DurationTier"); END

DIRECT (info_DurationTier_Sound_edit)
	Melder_information ("To include a copy of a Sound in your DurationTier editor:\n"
		"   select a DurationTier and a Sound, and click \"Edit\".");
END

/***** EXCITATION *****/

FORM (Excitation_draw, "Draw Excitation", 0)
	REAL ("From frequency (Bark)", "0")
	REAL ("To frequency (Bark)", "25.6")
	REAL ("Minimum (phon)", "0")
	REAL ("Maximum (phon)", "100")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Excitation_draw (OBJECT, GRAPHICS,
		GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		GET_REAL ("Minimum"), GET_REAL ("Maximum"), GET_INTEGER ("Garnish")))
END

FORM (Excitation_formula, "Excitation Formula", "Excitation: Formula...")
	LABEL ("label", "`x' is the place in Bark, `col' is the bin number")
	LABEL ("label", "x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

DIRECT (Excitation_getLoudness)
	Melder_informationReal (Excitation_getLoudness (ONLY (classExcitation)), "sones");
END

DIRECT (Excitation_help) Melder_help ("Excitation"); END

FORM (Excitation_to_Formant, "From Excitation to Formant", 0)
	NATURAL ("Maximum number of formants", "20")
	OK
DO
	EVERY_TO (Excitation_to_Formant (OBJECT, GET_INTEGER ("Maximum number of formants")))
END

DIRECT (Excitation_to_Matrix)
	EVERY_TO (Excitation_to_Matrix (OBJECT))
END

/***** FORMANT *****/

DIRECT (Formant_downto_FormantTier)
	EVERY_TO (Formant_downto_FormantTier (OBJECT))
END

FORM (Formant_drawSpeckles, "Draw Formant", "Formant: Draw speckles...")
	dia_timeRange (dia);
	POSITIVE ("Maximum frequency (Hz)", "5500.0")
	REAL ("Dynamic range (dB)", "30.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Formant_drawSpeckles (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_REAL ("Maximum frequency"),
		GET_REAL ("Dynamic range"), GET_INTEGER ("Garnish")))
END

FORM (Formant_drawTracks, "Draw formant tracks", "Formant: Draw tracks...")
	dia_timeRange (dia);
	POSITIVE ("Maximum frequency (Hz)", "5500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Formant_drawTracks (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_REAL ("Maximum frequency"),
		GET_INTEGER ("Garnish")))
END

FORM (Formant_formula_bandwidths, "Formant: Formula (bandwidths)", "Formant: Formula (bandwidths)...")
	LABEL ("", "row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	TEXTFIELD ("formula", "self / 2 ; sharpen all peaks")
	OK
DO
	WHERE (SELECTED) {
		if (! Formant_formula_bandwidths (OBJECT, GET_STRING ("formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Formant_formula_frequencies, "Formant: Formula (frequencies)", "Formant: Formula (frequencies)...")
	LABEL ("", "row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD ("formula", "if row = 2 then self + 200 else self fi")
	OK
DO
	WHERE (SELECTED) {
		if (! Formant_formula_frequencies (OBJECT, GET_STRING ("formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Formant_getBandwidthAtTime, "Formant: Get bandwidth", "Formant: Get bandwidth at time...")
	NATURAL ("Formant number", "1")
	REAL ("Time (s)", "0.5")
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("Linear")
	OK
DO
	Melder_informationReal (Formant_getBandwidthAtTime (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("Time"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END
	
FORM (Formant_getMaximum, "Formant: Get maximum", "Formant: Get maximum...")
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Formant_getMaximum (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1,
		GET_INTEGER ("Interpolation") - 1), GET_STRING ("Units"));
END

DIRECT (Formant_getMaximumNumberOfFormants)
	Melder_information ("%s (there are at most this many formants in every frame)",
		Melder_integer (Formant_getMaxNumFormants (ONLY_OBJECT)));
END

FORM (Formant_getMean, "Formant: Get mean", "Formant: Get mean...")
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	OK
DO
	Melder_informationReal (Formant_getMean (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

FORM (Formant_getMinimum, "Formant: Get minimum", "Formant: Get minimum...")
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Formant_getMinimum (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1,
		GET_INTEGER ("Interpolation") - 1), GET_STRING ("Units"));
END

DIRECT (Formant_getMinimumNumberOfFormants)
	Melder_information ("%s (at least this many formants in every frame)",
		Melder_integer (Formant_getMinNumFormants (ONLY_OBJECT)));
END

FORM (Formant_getNumberOfFormants, "Formant: Get number of formants", "Formant: Get number of formants...")
	NATURAL ("Frame number", "1")
	OK
DO
	long frame = GET_INTEGER ("Frame number");
	Formant me = ONLY_OBJECT;
	if (frame > my nx) return Melder_error ("There is no frame %ld in a Formant with only %ld frames.", frame, my nx);
	Melder_information ("%d formants", (int) my frame [frame]. nFormants);
END

FORM (Formant_getQuantile, "Formant: Get quantile", 0)
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	REAL ("Quantile", "0.50 (= median)")
	OK
DO
	Melder_informationReal (Formant_getQuantile (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("Quantile"), GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

FORM (Formant_getStandardDeviation, "Formant: Get standard deviation", 0)
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	OK
DO
	Melder_informationReal (Formant_getStandardDeviation (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

FORM (Formant_getTimeOfMaximum, "Formant: Get time of maximum", "Formant: Get time of maximum...")
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Formant_getTimeOfMaximum (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Formant_getTimeOfMinimum, "Formant: Get time of minimum", "Formant: Get time of minimum...")
	NATURAL ("Formant number", "1")
	dia_timeRange (dia);
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Formant_getTimeOfMinimum (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Formant_getValueAtTime, "Formant: Get value", "Formant: Get value at time...")
	NATURAL ("Formant number", "1")
	REAL ("Time (s)", "0.5")
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Bark")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("Linear")
	OK
DO
	Melder_informationReal (Formant_getValueAtTime (ONLY (classFormant), GET_INTEGER ("Formant number"),
		GET_REAL ("Time"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END
	
DIRECT (Formant_help) Melder_help ("Formant"); END

FORM (Formant_scatterPlot, "Formant: Scatter plot", 0)
	dia_timeRange (dia);
	NATURAL ("Horizontal formant number", "2")
	REAL ("left Horizontal range (Hz)", "3000")
	REAL ("right Horizontal range (Hz)", "400")
	NATURAL ("Vertical formant number", "1")
	REAL ("left Vertical range (Hz)", "1500")
	REAL ("right Vertical range (Hz)", "100")
	POSITIVE ("Mark size (mm)", "1.0")
	BOOLEAN ("Garnish", 1)
	SENTENCE ("Mark string (+xo.)", "+")
	OK
DO
	EVERY_DRAW (Formant_scatterPlot (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Horizontal formant number"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_INTEGER ("Vertical formant number"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), GET_INTEGER ("Garnish")))
END

DIRECT (Formant_sort)
	WHERE (SELECTED) {
		Formant_sort (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM (Formant_to_Matrix, "From Formant to Matrix", 0)
	INTEGER ("Formant", "1")
	OK
DO
	EVERY_TO (Formant_to_Matrix (OBJECT, GET_INTEGER ("Formant")))
END

FORM (Formant_tracker, "Formant tracker", "Formant: Track...")
	NATURAL ("Number of tracks", "3")
	REAL ("Reference F1 (Hz)", "550")
	REAL ("Reference F2 (Hz)", "1650")
	REAL ("Reference F3 (Hz)", "2750")
	REAL ("Reference F4 (Hz)", "3850")
	REAL ("Reference F5 (Hz)", "4950")
	REAL ("Frequency cost (/kHz)", "1.0")
	REAL ("Bandwidth cost", "1.0")
	REAL ("Transition cost (/octave)", "1.0")
	OK
DO
	EVERY_TO (Formant_tracker (OBJECT, GET_INTEGER ("Number of tracks"),
		GET_REAL ("Reference F1"), GET_REAL ("Reference F2"),
		GET_REAL ("Reference F3"), GET_REAL ("Reference F4"),
		GET_REAL ("Reference F5"), GET_REAL ("Frequency cost"),
		GET_REAL ("Bandwidth cost"), GET_REAL ("Transition cost")))
END

/***** FORMANT & POINTPROCESS *****/

DIRECT (Formant_PointProcess_to_FormantTier)
	Formant formant = ONLY (classFormant);
	PointProcess point = ONLY (classPointProcess);
	if (! praat_new (Formant_PointProcess_to_FormantTier (formant, point),
		"%s %s", formant -> name, point -> name)) return 0;
END

/***** FORMANT & SOUND *****/

DIRECT (Sound_Formant_filter)
	Sound me = ONLY (classSound);
	if (! praat_new (Sound_Formant_filter (me, ONLY (classFormant)), "%s_filt", my name)) return 0;
END

DIRECT (Sound_Formant_filter_noscale)
	Sound me = ONLY (classSound);
	if (! praat_new (Sound_Formant_filter_noscale (me, ONLY (classFormant)), "%s_filt", my name)) return 0;
END

/***** FORMANTTIER *****/

FORM (FormantTier_addPoint, "Add one point", "FormantTier: Add point...")
	REAL ("Time (s)", "0.5")
	LABEL ("", "Frequencies and bandwidths (Hz):")
	TEXTFIELD ("fb pairs", "500 50 1500 100 2500 150 3500 200 4500 300")
	OK
DO
	char *fbpairs = GET_STRING ("fb pairs");
	FormantPoint point = FormantPoint_create (GET_REAL ("Time"));
	double *f = point -> formant, *b = point -> bandwidth;
	int numberOfFormants = sscanf (fbpairs, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
		f, b, f+1, b+1, f+2, b+2, f+3, b+3, f+4, b+4, f+5, b+5, f+6, b+6, f+7, b+7, f+8, b+8, f+9, b+9) / 2;
	if (numberOfFormants < 1) {
		forget (point);
		return Melder_error ("Number of formant-bandwidth pairs must be at least 1.");
	}
	point -> numberOfFormants = numberOfFormants;
	WHERE (SELECTED) {
		if (! AnyTier_addPoint (OBJECT, Data_copy (point))) { forget (point); return 0; }
		praat_dataChanged (OBJECT);
	}
	forget (point);
END

FORM (FormantTier_create, "Create empty FormantTier", NULL)
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double startingTime = GET_REAL ("Starting time"), finishingTime = GET_REAL ("Finishing time");
	REQUIRE (finishingTime > startingTime, "Finishing time must be greater than starting time.")
	if (! praat_new (FormantTier_create (startingTime, finishingTime), GET_STRING ("Name"))) return 0;
END

FORM (FormantTier_downto_TableOfReal, "Down to TableOfReal", 0)
	BOOLEAN ("Include formants", 1)
	BOOLEAN ("Include bandwidths", 0)
	OK
DO
	EVERY_TO (FormantTier_downto_TableOfReal (OBJECT, GET_INTEGER ("Include formants"), GET_INTEGER ("Include bandwidths")))
END

FORM (FormantTier_getBandwidthAtTime, "FormantTier: Get bandwidth", "FormantTier: Get bandwidth at time...")
	NATURAL ("Formant number", "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (FormantTier_getBandwidthAtTime (ONLY (classFormantTier), GET_INTEGER ("Formant number"),
		GET_REAL ("Time")), "Hertz");
END
	
FORM (FormantTier_getValueAtTime, "FormantTier: Get value", "FormantTier: Get value at time...")
	NATURAL ("Formant number", "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (FormantTier_getValueAtTime (ONLY (classFormantTier), GET_INTEGER ("Formant number"),
		GET_REAL ("Time")), "Hertz");
END
	
DIRECT (FormantTier_help) Melder_help ("FormantTier"); END

FORM (FormantTier_speckle, "Draw FormantTier", 0)
	dia_timeRange (dia);
	POSITIVE ("Maximum frequency (Hz)", "5500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (FormantTier_speckle (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_REAL ("Maximum frequency"), GET_INTEGER ("Garnish")))
END

/***** FORMANTTIER & SOUND *****/

DIRECT (Sound_FormantTier_filter)
	Sound me = ONLY (classSound);
	if (! praat_new (Sound_FormantTier_filter (me, ONLY (classFormantTier)), "%s_filt", my name)) return 0;
END

DIRECT (Sound_FormantTier_filter_noscale)
	Sound me = ONLY (classSound);
	if (! praat_new (Sound_FormantTier_filter_noscale (me, ONLY (classFormantTier)), "%s_filt", my name)) return 0;
END

/***** HARMONICITY *****/

FORM (Harmonicity_draw, "Draw harmonicity", 0)
	dia_timeRange (dia);
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0 (= auto)")
	OK
DO
	EVERY_DRAW (Matrix_drawRows (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), 0.0, 0.0,
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

FORM (Harmonicity_formula, "Harmonicity Formula", "Harmonicity: Formula...")
	LABEL ("label", "x is time")
	LABEL ("label", "for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Harmonicity_getMaximum, "Harmonicity: Get maximum", "Harmonicity: Get maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getMaximum (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "dB");
END

FORM (Harmonicity_getMean, "Harmonicity: Get mean", "Harmonicity: Get mean...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (Harmonicity_getMean (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range")), "dB");
END

FORM (Harmonicity_getMinimum, "Harmonicity: Get minimum", "Harmonicity: Get minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getMinimum (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "dB");
END

FORM (Harmonicity_getStandardDeviation, "Harmonicity: Get standard deviation", "Harmonicity: Get standard deviation...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (Harmonicity_getStandardDeviation (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range")), "dB");
END

FORM (Harmonicity_getTimeOfMaximum, "Harmonicity: Get time of maximum", "Harmonicity: Get time of maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getXOfMaximum (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Harmonicity_getTimeOfMinimum, "Harmonicity: Get time of minimum", "Harmonicity: Get time of minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getXOfMinimum (ONLY (classHarmonicity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Harmonicity_getValueAtTime, "Harmonicity: Get value", "Harmonicity: Get value at time...")
	dia_Vector_getValue (dia);
	OK
DO
	Melder_informationReal (Vector_getValueAtX (ONLY (classHarmonicity), GET_REAL ("Time"), GET_INTEGER ("Interpolation") - 1), "dB");
END
	
FORM (Harmonicity_getValueInFrame, "Get value in frame", "Harmonicity: Get value in frame...")
	INTEGER ("Frame number", "10")
	OK
DO
	Harmonicity me = ONLY (classHarmonicity);
	long frameNumber = GET_INTEGER ("Frame number");
	Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], "dB");
END

DIRECT (Harmonicity_help) Melder_help ("Harmonicity"); END

DIRECT (Harmonicity_to_Matrix)
	EVERY_TO (Harmonicity_to_Matrix (OBJECT))
END

/***** INTENSITY *****/

FORM (Intensity_draw, "Draw Intensity", 0)
	dia_timeRange (dia);
	REAL ("Minimum (dB)", "0.0")
	REAL ("Maximum (dB)", "0.0 (= auto)")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Intensity_draw (OBJECT, GRAPHICS, GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Minimum"), GET_REAL ("Maximum"), GET_INTEGER ("Garnish")))
END

DIRECT (Intensity_downto_IntensityTier)
	EVERY_TO (Intensity_downto_IntensityTier (OBJECT))
END

DIRECT (Intensity_downto_Matrix)
	EVERY_TO (Intensity_to_Matrix (OBJECT))
END

FORM (Intensity_formula, "Intensity Formula", 0)
	LABEL ("label", "`x' is the time in seconds, `col' is the frame number, `self' is in dB")
	LABEL ("label", "x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "0")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Intensity_getMaximum, "Intensity: Get maximum", "Intensity: Get maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getMaximum (ONLY (classIntensity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "dB");
END

FORM (old_Intensity_getMean, "Intensity: Get mean", "Intensity: Get mean...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (Sampled_getMean_standardUnits (ONLY (classIntensity), GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		0, 0, TRUE), "dB");
END

FORM (Intensity_getMean, "Intensity: Get mean", "Intensity: Get mean...")
	dia_timeRange (dia);
	RADIO ("Averaging method", 1)
		RADIOBUTTON ("energy")
		RADIOBUTTON ("sones")
		RADIOBUTTON ("dB")
	OK
DO_ALTERNATIVE (old_Intensity_getMean)
	Melder_informationReal (Sampled_getMean_standardUnits (ONLY (classIntensity), GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		0, GET_INTEGER ("Averaging method"), TRUE), "dB");
END

FORM (Intensity_getMinimum, "Intensity: Get minimum", "Intensity: Get minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getMinimum (ONLY (classIntensity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "dB");
END

FORM (Intensity_getQuantile, "Intensity: Get quantile", 0)
	dia_timeRange (dia);
	REAL ("Quantile (0-1)", "0.50")
	OK
DO
	Melder_informationReal (Intensity_getQuantile (ONLY (classIntensity), GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Quantile")), "dB");
END

FORM (Intensity_getStandardDeviation, "Intensity: Get standard deviation", "Intensity: Get standard deviation...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (Vector_getStandardDeviation (ONLY (classIntensity), GET_REAL ("left Time range"), GET_REAL ("right Time range")), "dB");
END

FORM (Intensity_getTimeOfMaximum, "Intensity: Get time of maximum", "Intensity: Get time of maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getXOfMaximum (ONLY (classIntensity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Intensity_getTimeOfMinimum, "Intensity: Get time of minimum", "Intensity: Get time of minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	Melder_informationReal (Vector_getXOfMinimum (ONLY (classIntensity),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Intensity_getValueAtTime, "Intensity: Get value", "Intensity: Get value at time...")
	dia_Vector_getValue (dia);
	OK
DO
	Melder_informationReal (Vector_getValueAtX (ONLY (classIntensity), GET_REAL ("Time"), GET_INTEGER ("Interpolation") - 1), "dB");
END
	
FORM (Intensity_getValueInFrame, "Get value in frame", "Intensity: Get value in frame...")
	INTEGER ("Frame number", "10")
	OK
DO
	Intensity me = ONLY (classIntensity);
	long frameNumber = GET_INTEGER ("Frame number");
	Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], "dB");
END

DIRECT (Intensity_help) Melder_help ("Intensity"); END

DIRECT (Intensity_to_IntensityTier_peaks)
	EVERY_TO (Intensity_to_IntensityTier_peaks (OBJECT))
END

DIRECT (Intensity_to_IntensityTier_valleys)
	EVERY_TO (Intensity_to_IntensityTier_valleys (OBJECT))
END

/***** INTENSITY & PITCH *****/

FORM (Pitch_Intensity_draw, "Plot intensity by pitch", 0)
	REAL ("From frequency (Hertz)", "0.0")
	REAL ("To frequency (Hertz)", "0.0 (= auto)")
	REAL ("From intensity (dB)", "0.0")
	REAL ("To intensity (dB)", "100.0")
	BOOLEAN ("Garnish", 1)
	RADIO ("Drawing method", 1)
	RADIOBUTTON ("Speckles")
	RADIOBUTTON ("Curve")
	RADIOBUTTON ("Speckles and curve")
	OK
DO
	EVERY_DRAW (Pitch_Intensity_draw (ONLY (classPitch), ONLY (classIntensity), GRAPHICS,
		GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		GET_REAL ("From intensity"), GET_REAL ("To intensity"), GET_INTEGER ("Garnish"), GET_INTEGER ("Drawing method")))
END

FORM (Pitch_Intensity_speckle, "Plot intensity by pitch", 0)
	REAL ("From frequency (Hertz)", "0.0")
	REAL ("To frequency (Hertz)", "0.0 (= auto)")
	REAL ("From intensity (dB)", "0.0")
	REAL ("To intensity (dB)", "100.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Pitch_Intensity_draw (ONLY (classPitch), ONLY (classIntensity), GRAPHICS,
		GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		GET_REAL ("From intensity"), GET_REAL ("To intensity"), GET_INTEGER ("Garnish"), 1))
END

/***** INTENSITY & POINTPROCESS *****/

DIRECT (Intensity_PointProcess_to_IntensityTier)
	Intensity intensity = ONLY (classIntensity);
	if (! praat_new (Intensity_PointProcess_to_IntensityTier (intensity, ONLY (classPointProcess)),
		intensity -> name)) return 0;
END

/***** INTENSITYTIER *****/

FORM (IntensityTier_addPoint, "Add one point", "IntensityTier: Add point...")
	REAL ("Time (s)", "0.5")
	REAL ("Intensity (dB)", "75")
	OK
DO
	WHERE (SELECTED) {
		if (! RealTier_addPoint (OBJECT, GET_REAL ("Time"), GET_REAL ("Intensity"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (IntensityTier_create, "Create empty IntensityTier", NULL)
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double startingTime = GET_REAL ("Starting time"), finishingTime = GET_REAL ("Finishing time");
	REQUIRE (finishingTime > startingTime, "Finishing time must be greater than starting time.")
	if (! praat_new (IntensityTier_create (startingTime, finishingTime), GET_STRING ("Name"))) return 0;
END

DIRECT (IntensityTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

DIRECT (IntensityTier_downto_TableOfReal)
	EVERY_TO (IntensityTier_downto_TableOfReal (OBJECT))
END

DIRECT (IntensityTier_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit an IntensityTier from batch.");
	} else {
		Sound sound = NULL;
		WHERE (SELECTED)
			if (CLASS == classSound) sound = OBJECT;
		WHERE (SELECTED && CLASS == classIntensityTier)
			if (! praat_installEditor (IntensityTierEditor_create (praat.topShell, FULL_NAME,
				OBJECT, sound, TRUE), IOBJECT)) return 0;
	}
END

FORM (IntensityTier_formula, "IntensityTier: Formula", "IntensityTier: Formula...")
	LABEL ("", "# ncol = the number of points")
	LABEL ("", "for col from 1 to ncol")
	LABEL ("", "   # x = the time of the colth point, in seconds")
	LABEL ("", "   # self = the value of the colth point, in dB")
	LABEL ("", "   self = `formula'")
	LABEL ("", "endfor")
	TEXTFIELD ("formula", "self + 3.0")
	OK
DO
	WHERE_DOWN (SELECTED) {
		RealTier_formula (OBJECT, GET_STRING ("formula"), NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (IntensityTier_getValueAtTime, "Get IntensityTier value", "IntensityTier: Get value at time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL ("Time")), "dB");
END
	
FORM (IntensityTier_getValueAtIndex, "Get IntensityTier value", "IntensityTier: Get value at index...")
	INTEGER ("Point number", "10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER ("Point number")), "dB");
END

DIRECT (IntensityTier_help) Melder_help ("IntensityTier"); END

DIRECT (IntensityTier_to_AmplitudeTier)
	EVERY_TO (IntensityTier_to_AmplitudeTier (OBJECT))
END

DIRECT (info_IntensityTier_Sound_edit)
	Melder_information ("To include a copy of a Sound in your IntensityTier editor:\n"
		"   select an IntensityTier and a Sound, and click \"Edit\".");
END

/***** INTENSITYTIER & POINTPROCESS *****/

DIRECT (IntensityTier_PointProcess_to_IntensityTier)
	IntensityTier intensity = ONLY (classIntensityTier);
	if (! praat_new (IntensityTier_PointProcess_to_IntensityTier (intensity, ONLY (classPointProcess)), intensity -> name)) return 0;
END

/***** INTENSITYTIER & SOUND *****/

DIRECT (Sound_IntensityTier_multiply_old)
	Sound sound = ONLY (classSound);
	if (! praat_new (Sound_IntensityTier_multiply (sound, ONLY (classIntensityTier), TRUE), "%s_int", sound -> name)) return 0;
END

FORM (Sound_IntensityTier_multiply, "Sound & IntervalTier: Multiply", 0)
	BOOLEAN ("Scale to 0.9", 1)
	OK
DO
	Sound sound = ONLY (classSound);
	if (! praat_new (Sound_IntensityTier_multiply (sound, ONLY (classIntensityTier), GET_INTEGER ("Scale to 0.9")), "%s_int", sound -> name)) return 0;
END

/***** INTERVALTIER *****/

FORM (IntervalTier_downto_TableOfReal, "IntervalTier: Down to TableOfReal", 0)
	SENTENCE ("Label", "")
	OK
DO
	EVERY_TO (IntervalTier_downto_TableOfReal (OBJECT, GET_STRING ("Label")))
END

DIRECT (IntervalTier_downto_TableOfReal_any)
	EVERY_TO (IntervalTier_downto_TableOfReal_any (OBJECT))
END

FORM (IntervalTier_getCentrePoints, "IntervalTier: Get centre points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getCentrePoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

FORM (IntervalTier_getEndPoints, "IntervalTier: Get end points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getEndPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

FORM (IntervalTier_getStartingPoints, "IntervalTier: Get starting points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getStartingPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

DIRECT (IntervalTier_help) Melder_help ("IntervalTier"); END

/***** INTERVALTIER & POINTPROCESS *****/

FORM (IntervalTier_PointProcess_endToCentre, "From end to centre", "IntervalTier & PointProcess: End to centre...")
	REAL ("Phase (0-1)", "0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL ("Phase");
	if (! praat_new (IntervalTier_PointProcess_endToCentre (tier, point, phase),
		"%s %s %ld", tier -> name, point -> name, (long) (100 * phase))) return 0;
END

FORM (IntervalTier_PointProcess_startToCentre, "From start to centre", "IntervalTier & PointProcess: Start to centre...")
	REAL ("Phase (0-1)", "0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL ("Phase");
	if (! praat_new (IntervalTier_PointProcess_startToCentre (tier, point, phase),
		"%s %s %ld", tier -> name, point -> name, (long) (100 * phase))) return 0;
END

FORM_READ (IntervalTier_readFromXwaves, "Read IntervalTier from Xwaves", 0)
	if (! praat_new (IntervalTier_readFromXwaves (file), MelderFile_name (file))) return 0;
END

/***** LABEL (obsolete) *****/

DIRECT (Label_Sound_to_TextGrid)
	Label label = ONLY (classLabel);
	Sound sound = ONLY (classSound);
	if (! praat_new (Label_Function_to_TextGrid (label, sound), sound -> name)) return 0;
END

DIRECT (info_Label_Sound_to_TextGrid)
	Melder_information ("This is an old-style Label object. To turn it into a TextGrid, "
		"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
END

/***** LTAS *****/

FORM (Ltas_computeTrendLine, "Ltas: Compute trend line", "Ltas: Compute trend line...")
	REAL ("left Frequency range (Hz)", "600.0")
	POSITIVE ("right Frequency range (Hz)", "4000.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Ltas_computeTrendLine (OBJECT, GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range")),
			"%s_trend", NAME)) return 0;
	}
END

FORM (old_Ltas_draw, "Ltas: Draw", 0)
	REAL ("left Frequency range (Hz)", "0.0")
	REAL ("right Frequency range (Hz)", "0.0 (= all)")
	REAL ("left Power range (dB/Hz)", "-20.0")
	REAL ("right Power range (dB/Hz)", "80.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Ltas_draw (OBJECT, GRAPHICS, GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Power range"), GET_REAL ("right Power range"), GET_INTEGER ("Garnish"), "bars"))
END

FORM (Ltas_draw, "Ltas: Draw", 0)
	REAL ("left Frequency range (Hz)", "0.0")
	REAL ("right Frequency range (Hz)", "0.0 (= all)")
	REAL ("left Power range (dB/Hz)", "-20.0")
	REAL ("right Power range (dB/Hz)", "80.0")
	BOOLEAN ("Garnish", 1)
	LABEL ("", "")
	OPTIONMENU ("Drawing method", 2)
		OPTION ("curve")
		OPTION ("bars")
		OPTION ("poles")
		OPTION ("speckles")
	OK
DO_ALTERNATIVE (old_Ltas_draw)
	EVERY_DRAW (Ltas_draw (OBJECT, GRAPHICS, GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("left Power range"), GET_REAL ("right Power range"), GET_INTEGER ("Garnish"), GET_STRING ("Drawing method")))
END

FORM (Ltas_formula, "Ltas Formula", 0)
	LABEL ("label", "`x' is the frequency in Hertz, `col' is the bin number")
	LABEL ("label", "x := x1;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "0")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Ltas_getBinNumberFromFrequency, "Ltas: Get band from frequency", "Ltas: Get band from frequency...")
	REAL ("Frequency (Hz)", "2000")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY (classLtas), GET_REAL ("Frequency")), NULL);
END

DIRECT (Ltas_getBinWidth)
	Ltas me = ONLY (classLtas);
	Melder_informationReal (my dx, "Hertz");
END

FORM (Ltas_getFrequencyFromBinNumber, "Ltas: Get frequency from bin number", "Ltas: Get frequency from bin number...")
	NATURAL ("Bin number", "1")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY (classLtas), GET_INTEGER ("Bin number")), "Hertz");
END

FORM (Ltas_getFrequencyOfMaximum, "Ltas: Get frequency of maximum", "Ltas: Get frequency of maximum...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0 (= all)")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMaximum (ONLY (classLtas),
		GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_INTEGER ("Interpolation") - 1), "Hertz");
END

FORM (Ltas_getFrequencyOfMinimum, "Ltas: Get frequency of minimum", "Ltas: Get frequency of minimum...")
	REAL ("From frequency (s)", "0.0")
	REAL ("To frequency (s)", "0.0 (= all)")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMinimum (ONLY (classLtas),
		GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_INTEGER ("Interpolation") - 1), "Hertz");
END

DIRECT (Ltas_getFrequencyRange)
	Ltas me = ONLY (classLtas);
	Melder_informationReal (my xmax - my xmin, "Hertz");
END

DIRECT (Ltas_getHighestFrequency)
	Ltas me = ONLY (classLtas);
	Melder_informationReal (my xmax, "Hertz");
END

DIRECT (Ltas_getLowestFrequency)
	Ltas me = ONLY (classLtas);
	Melder_informationReal (my xmin, "Hertz");
END

FORM (Ltas_getMaximum, "Ltas: Get maximum", "Ltas: Get maximum...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0 (= all)")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMaximum (ONLY (classLtas),
		GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_INTEGER ("Interpolation") - 1), "dB");
END

FORM (Ltas_getMean, "Ltas: Get mean", "Ltas: Get mean...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0 (= all)")
	RADIO ("Averaging method", 1)
		RADIOBUTTON ("energy")
		RADIOBUTTON ("sones")
		RADIOBUTTON ("dB")
	OK
DO
	Melder_informationReal (Sampled_getMean_standardUnits (ONLY (classLtas), GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		0, GET_INTEGER ("Averaging method"), FALSE), "dB");
END

FORM (Ltas_getMinimum, "Ltas: Get minimum", "Ltas: Get minimum...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0 (= all)")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMinimum (ONLY (classLtas),
		GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_INTEGER ("Interpolation") - 1), "dB");
END

DIRECT (Ltas_getNumberOfBins)
	Ltas me = ONLY (classLtas);
	Melder_information ("%ld bins", my nx);
END

FORM (Ltas_getLocalPeakHeight, "Ltas: Get local peak height", 0)
	REAL ("left Environment (Hz)", "1700.0")
	REAL ("right Environment (Hz)", "4200.0")
	REAL ("left Peak (Hz)", "2400.0")
	REAL ("right Peak (Hz)", "3200.0")
	RADIO ("Averaging method", 1)
		RADIOBUTTON ("energy")
		RADIOBUTTON ("sones")
		RADIOBUTTON ("dB")
	OK
DO
	Melder_informationReal (Ltas_getLocalPeakHeight (ONLY (classLtas), GET_REAL ("left Environment"), GET_REAL ("right Environment"),
		GET_REAL ("left Peak"), GET_REAL ("right Peak"), GET_INTEGER ("Averaging method")), "dB");
END

FORM (Ltas_getSlope, "Ltas: Get slope", 0)
	REAL ("left Low band (Hz)", "0.0")
	REAL ("right Low band (Hz)", "1000.0")
	REAL ("left High band (Hz)", "1000.0")
	REAL ("right High band (Hz)", "4000.0")
	RADIO ("Averaging method", 1)
		RADIOBUTTON ("energy")
		RADIOBUTTON ("sones")
		RADIOBUTTON ("dB")
	OK
DO
	Melder_informationReal (Ltas_getSlope (ONLY (classLtas), GET_REAL ("left Low band"), GET_REAL ("right Low band"),
		GET_REAL ("left High band"), GET_REAL ("right High band"), GET_INTEGER ("Averaging method")), "dB");
END

FORM (Ltas_getStandardDeviation, "Ltas: Get standard deviation", "Ltas: Get standard deviation...")
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "0.0 (= all)")
	RADIO ("Averaging method", 1)
		RADIOBUTTON ("energy")
		RADIOBUTTON ("sones")
		RADIOBUTTON ("dB")
	OK
DO
	Melder_informationReal (Sampled_getStandardDeviation_standardUnits (ONLY (classLtas), GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		0, GET_INTEGER ("Averaging method"), FALSE), "dB");
END

FORM (Ltas_getValueAtFrequency, "Ltas: Get value", "Ltas: Get value at frequency...")
	REAL ("Frequency (Hertz)", "1500")
	RADIO ("Interpolation", 1)
	RADIOBUTTON ("Nearest")
	RADIOBUTTON ("Linear")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getValueAtX (ONLY (classLtas), GET_REAL ("Frequency"), GET_INTEGER ("Interpolation") - 1), "dB");
END
	
FORM (Ltas_getValueInBin, "Get value in bin", "Ltas: Get value in bin...")
	INTEGER ("Bin number", "100")
	OK
DO
	Ltas me = ONLY (classLtas);
	long binNumber = GET_INTEGER ("Bin number");
	Melder_informationReal (binNumber < 1 || binNumber > my nx ? NUMundefined : my z [1] [binNumber], "dB");
END

DIRECT (Ltas_help) Melder_help ("Ltas"); END

DIRECT (Ltases_merge)
	Collection ltases;
	int n = 0;
	WHERE (SELECTED) n ++;
	ltases = Collection_create (classLtas, n);
	WHERE (SELECTED) Collection_addItem (ltases, OBJECT);
	if (! praat_new (Ltases_merge (ltases), "merged")) {
		ltases -> size = 0;   /* Undangle. */
		forget (ltases);
		return 0;
	}
	ltases -> size = 0;   /* Undangle. */
	forget (ltases);
END

FORM (Ltas_subtractTrendLine, "Ltas: Subtract trend line", "Ltas: Subtract trend line...")
	REAL ("left Frequency range (Hertz)", "600.0")
	POSITIVE ("right Frequency range (Hertz)", "4000.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Ltas_subtractTrendLine (OBJECT, GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range")),
			"%s_fit", NAME)) return 0;
	}
END

DIRECT (Ltas_to_Matrix)
	EVERY_TO (Ltas_to_Matrix (OBJECT))
END

/***** MANIPULATION *****/

static void cb_ManipulationEditor_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new (publish, "fromManipulationEditor")) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
}
DIRECT (Manipulation_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a Manipulation from batch.");
	} else {
		WHERE (SELECTED) {
			ManipulationEditor editor = ManipulationEditor_create (praat.topShell, FULL_NAME, OBJECT);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_ManipulationEditor_publish, NULL);
		}
	}
END

DIRECT (Manipulation_extractDurationTier)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		if (ana -> duration) {
			if (! praat_new (Data_copy (ana -> duration), NULL)) return 0;
		} else {
			return Melder_error ("Manipulation does not contain a DurationTier.");
		}
	}
END

DIRECT (Manipulation_extractOriginalSound)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		if (ana -> sound) {
			if (! praat_new (Data_copy (ana -> sound), NULL)) return 0;
		} else {
			return Melder_error ("Manipulation does not contain a Sound.");
		}
	}
END

DIRECT (Manipulation_extractPitchTier)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		if (ana -> pitch) {
			if (! praat_new (Data_copy (ana -> pitch), NULL)) return 0;
		} else {
			return Melder_error ("Manipulation does not contain a PitchTier.");
		}
	}
END

DIRECT (Manipulation_extractPulses)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		if (ana -> pulses) {
			if (! praat_new (Data_copy (ana -> pulses), NULL)) return 0;
		} else {
			return Melder_error ("Manipulation does not contain a PointProcess.");
		}
	}
END

DIRECT (Manipulation_getResynthesis_psola)
	EVERY_TO (Manipulation_to_Sound (OBJECT, Manipulation_PSOLA))
END

DIRECT (Manipulation_getResynthesis_lpc)
	EVERY_TO (Manipulation_to_Sound (OBJECT, Manipulation_PITCH_LPC))
END

DIRECT (Manipulation_help) Melder_help ("Manipulation"); END

DIRECT (Manipulation_play_psola)
	EVERY_CHECK (Manipulation_play (OBJECT, Manipulation_PSOLA))
END

DIRECT (Manipulation_play_lpc)
	EVERY_CHECK (Manipulation_play (OBJECT, Manipulation_PITCH_LPC))
END

DIRECT (Manipulation_removeDuration)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		forget (ana -> duration);
		praat_dataChanged (ana);
	}
END

DIRECT (Manipulation_removeOriginalSound)
	WHERE (SELECTED) {
		Manipulation ana = OBJECT;
		forget (ana -> sound);
		praat_dataChanged (ana);
	}
END

FORM_WRITE (Manipulation_writeToBinaryFileWithoutSound, "Binary file without Sound", 0, 0)
	if (! Manipulation_writeToBinaryFileWithoutSound (ONLY_OBJECT, file)) return 0;
END

FORM_WRITE (Manipulation_writeToTextFileWithoutSound, "Text file without Sound", 0, 0)
	if (! Manipulation_writeToTextFileWithoutSound (ONLY_OBJECT, file)) return 0;
END

DIRECT (info_DurationTier_Manipulation_replace)
	Melder_information ("To replace the DurationTier in a Manipulation object,\n"
		"select a DurationTier object and a Manipulation object\nand choose \"Replace duration\".");
END

DIRECT (info_PitchTier_Manipulation_replace)
	Melder_information ("To replace the PitchTier in a Manipulation object,\n"
		"select a PitchTier object and a Manipulation object\nand choose \"Replace pitch\".");
END

/***** MANIPULATION & DURATIONTIER *****/

DIRECT (Manipulation_replaceDurationTier)
	Manipulation ana = ONLY (classManipulation);
	if (! Manipulation_replaceDurationTier (ana, ONLY (classDurationTier))) return 0;
	praat_dataChanged (ana);
END

DIRECT (Manipulation_replaceDurationTier_help) Melder_help ("Manipulation: Replace duration tier"); END

/***** MANIPULATION & PITCHTIER *****/

DIRECT (Manipulation_replacePitchTier)
	Manipulation ana = ONLY (classManipulation);
	if (! Manipulation_replacePitchTier (ana, ONLY (classPitchTier))) return 0;
	praat_dataChanged (ana);
END

DIRECT (Manipulation_replacePitchTier_help) Melder_help ("Manipulation: Replace pitch tier"); END

/***** MANIPULATION & POINTPROCESS *****/

DIRECT (Manipulation_replacePulses)
	Manipulation ana = ONLY (classManipulation);
	if (! Manipulation_replacePulses (ana, ONLY (classPointProcess))) return 0;
	praat_dataChanged (ana);
END

/***** MANIPULATION & SOUND *****/

DIRECT (Manipulation_replaceOriginalSound)
	Manipulation ana = ONLY (classManipulation);
	if (! Manipulation_replaceOriginalSound (ana, ONLY (classSound))) return 0;
	praat_dataChanged (ana);
END

/***** MANIPULATION & TEXTTIER *****/

DIRECT (Manipulation_TextTier_to_Manipulation)
	if (! praat_new (Manipulation_AnyTier_to_Manipulation (ONLY (classManipulation), ONLY (classTextTier)),
		((Manipulation) (ONLY (classManipulation))) -> name)) return 0;	
END

/***** MATRIX *****/

DIRECT (Matrix_appendRows)
	Matrix m1 = NULL, m2 = NULL;
	WHERE (SELECTED) if (m1) m2 = OBJECT; else m1 = OBJECT;
	if (! praat_new (Matrix_appendRows (m1, m2), "%s_%s", m1 -> name, m2 -> name)) return 0;
END

FORM (Matrix_create, "Create Matrix", "Create Matrix...")
	WORD ("Name", "xy")
	REAL ("xmin", "1.0")
	REAL ("xmax", "1.0")
	NATURAL ("Number of columns", "1")
	POSITIVE ("dx", "1.0")
	REAL ("x1", "1.0")
	REAL ("ymin", "1.0")
	REAL ("ymax", "1.0")
	NATURAL ("Number of rows", "1")
	POSITIVE ("dy", "1.0")
	REAL ("y1", "1.0")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "x*y")
	OK
DO
	double xmin = GET_REAL ("xmin"), xmax = GET_REAL ("xmax");
	double ymin = GET_REAL ("ymin"), ymax = GET_REAL ("ymax");
	if (xmax < xmin)
		return Melder_error ("xmax (%.8g) should not be less than xmin (%.8g).", xmax, xmin);
	if (ymax < ymin)
		return Melder_error ("ymax (%.8g) should not be less than ymin (%.8g).", ymax, ymin);
	if (! praat_new (Matrix_create (
		xmin, xmax, GET_INTEGER ("Number of columns"), GET_REAL ("dx"), GET_REAL ("x1"),
		ymin, ymax, GET_INTEGER ("Number of rows"), GET_REAL ("dy"), GET_REAL ("y1")),
		"%s", GET_STRING ("Name"))) return 0;
	praat_updateSelection ();
	return praat_Fon_formula (dia);
END

FORM (Matrix_createSimple, "Create simple Matrix", "Create simple Matrix...")
	WORD ("Name", "xy")
	NATURAL ("Number of rows", "10")
	NATURAL ("Number of columns", "10")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "x*y")
	OK
DO
	if (! praat_new (Matrix_createSimple (
		GET_INTEGER ("Number of rows"), GET_INTEGER ("Number of columns")),
		"%s", GET_STRING ("Name"))) return 0;
	praat_updateSelection ();
	return praat_Fon_formula (dia);
END

FORM (Matrix_drawOneContour, "Draw one altitude contour", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Height", "0.5")
	OK
DO
	EVERY_DRAW (Matrix_drawOneContour (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Height")))
END

FORM (Matrix_drawContours, "Draw altitude contours", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_drawContours (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

FORM (Matrix_drawRows, "Draw rows", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_drawRows (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="),
		GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

DIRECT (Matrix_eigen)
	WHERE (SELECTED) {
		Matrix vec, val;
		if (! Matrix_eigen (OBJECT, & vec, & val)) return 0;
		if (! praat_new (vec, "eigenvectors")) return 0;
		if (! praat_new (val, "eigenvalues")) return 0;
	}
END

FORM (Matrix_formula, "Matrix Formula", "Formula...")
	LABEL ("label", "y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD ("formula", "self")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

DIRECT (Matrix_getHighestX) Matrix me = ONLY_OBJECT; Melder_informationReal (my xmax, NULL); END
DIRECT (Matrix_getHighestY) Matrix me = ONLY_OBJECT; Melder_informationReal (my ymax, NULL); END
DIRECT (Matrix_getLowestX) Matrix me = ONLY_OBJECT; Melder_informationReal (my xmin, NULL); END
DIRECT (Matrix_getLowestY) Matrix me = ONLY_OBJECT; Melder_informationReal (my ymin, NULL); END
DIRECT (Matrix_getNumberOfColumns) Matrix me = ONLY_OBJECT; Melder_information ("%ld", my nx); END
DIRECT (Matrix_getNumberOfRows) Matrix me = ONLY_OBJECT; Melder_information ("%ld", my ny); END
DIRECT (Matrix_getColumnDistance) Matrix me = ONLY_OBJECT; Melder_informationReal (my dx, NULL); END
DIRECT (Matrix_getRowDistance) Matrix me = ONLY_OBJECT; Melder_informationReal (my dy, NULL); END
DIRECT (Matrix_getSum) Matrix me = ONLY_OBJECT; Melder_informationReal (Matrix_getSum (me), NULL); END

FORM (Matrix_getValueAtXY, "Matrix: Get value at xy", 0)
	REAL ("X", "0")
	REAL ("Y", "0")
	OK
DO
	Matrix me = ONLY_OBJECT;
	double x = GET_REAL ("X"), y = GET_REAL ("Y");
	MelderInfo_open ();
	MelderInfo_write1 (Melder_double (Matrix_getValueAtXY (me, x, y)));
	MelderInfo_write5 (" (at x = ", Melder_double (x), " and y = ", Melder_double (y), ")");
	MelderInfo_close ();
END

FORM (Matrix_getValueInCell, "Matrix: Get value in cell", 0)
	NATURAL ("Row number", "1") NATURAL ("Column number", "1") OK DO Matrix me = ONLY_OBJECT;
	long row = GET_INTEGER ("Row number"), column = GET_INTEGER ("Column number");
	REQUIRE (row <= my ny, "Row number must not exceed number of rows.")
	REQUIRE (column <= my nx, "Column number must not exceed number of columns.")
	Melder_informationReal (my z [row] [column], NULL); END
FORM (Matrix_getXofColumn, "Matrix: Get x of column", 0)
	NATURAL ("Column number", "1") OK DO
	Melder_informationReal (Matrix_columnToX (ONLY_OBJECT, GET_INTEGER ("Column number")), NULL); END
FORM (Matrix_getYofRow, "Matrix: Get y of row", 0)
	NATURAL ("Row number", "1") OK DO
	Melder_informationReal (Matrix_rowToY (ONLY_OBJECT, GET_INTEGER ("Row number")), NULL); END

DIRECT (Matrix_help) Melder_help ("Matrix"); END

DIRECT (Matrix_movie)
	Graphics g = Movie_create ("Matrix movie", 300, 300);
	WHERE (SELECTED) Matrix_movie (OBJECT, g);
END

FORM (Matrix_paintCells, "Matrix: Paint cells with greys", "Matrix: Paint cells...")
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintCells (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

FORM (Matrix_paintContours, "Matrix: Paint altitude contours with greys", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintContours (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

FORM (Matrix_paintImage, "Matrix: Paint grey image", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintImage (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum")))
END

FORM (Matrix_paintSurface, "Matrix: Paint 3-D surface plot", 0)
	REAL ("From x =", "0.0")
	REAL ("To x =", "0.0")
	REAL ("From y =", "0.0")
	REAL ("To y =", "0.0")
	REAL ("Minimum", "0.0")
	REAL ("Maximum", "0.0")
	OK
DO
	EVERY_DRAW (Matrix_paintSurface (OBJECT, GRAPHICS,
		GET_REAL ("From x ="), GET_REAL ("To x ="), GET_REAL ("From y ="), GET_REAL ("To y ="),
		GET_REAL ("Minimum"), GET_REAL ("Maximum"), 30, 45))
END

FORM (Matrix_power, "Matrix: Power...", 0)
	NATURAL ("Power", "2")
	OK
DO
	EVERY_TO (Matrix_power (OBJECT, GET_INTEGER ("Power")))
END

FORM_READ (Matrix_readFromRawTextFile, "Read Matrix from raw text file", 0)
	if (! praat_new (Matrix_readFromRawTextFile (file), MelderFile_name (file))) return 0;
END

FORM_READ (Matrix_readAP, "Read Matrix from LVS AP file", 0)
	if (! praat_new (Matrix_readAP (file), MelderFile_name (file))) return 0;
END

FORM (Matrix_setValue, "Matrix: Set value", "Matrix: Set value...")
	NATURAL ("Row number", "1")
	NATURAL ("Column number", "1")
	REAL ("New value", "0.0")
	OK
DO
	WHERE (SELECTED) {
		Matrix me = OBJECT;
		long row = GET_INTEGER ("Row number"), column = GET_INTEGER ("Column number");
		REQUIRE (row <= my ny, "Row number must not be greater than number of rows.")
		REQUIRE (column <= my nx, "Column number must not be greater than number of columns.")
		my z [row] [column] = GET_REAL ("New value");
		praat_dataChanged (me);
	}
END

DIRECT (Matrix_to_Cochleagram)
	EVERY_TO (Matrix_to_Cochleagram (OBJECT))
END

DIRECT (Matrix_to_Excitation)
	EVERY_TO (Matrix_to_Excitation (OBJECT))
END

DIRECT (Matrix_to_Harmonicity)
	EVERY_TO (Matrix_to_Harmonicity (OBJECT))
END

DIRECT (Matrix_to_Intensity)
	EVERY_TO (Matrix_to_Intensity (OBJECT))
END

DIRECT (Matrix_to_Pitch)
	EVERY_TO (Matrix_to_Pitch (OBJECT))
END

DIRECT (Matrix_to_Spectrogram)
	EVERY_TO (Matrix_to_Spectrogram (OBJECT))
END

DIRECT (Matrix_to_Spectrum)
	EVERY_TO (Matrix_to_Spectrum (OBJECT))
END

DIRECT (Matrix_to_Ltas)
	EVERY_TO (Matrix_to_Ltas (OBJECT))
END

DIRECT (Matrix_to_ParamCurve)
	Matrix m1 = NULL, m2 = NULL;
	WHERE (SELECTED) if (m1) m2 = OBJECT; else m1 = OBJECT;
	if (! praat_new (ParamCurve_create (m1, m2), "%s_%s", m1 -> name, m2 -> name)) return 0;
END

DIRECT (Matrix_to_PointProcess)
	EVERY_TO (Matrix_to_PointProcess (OBJECT))
END

DIRECT (Matrix_to_Polygon)
	EVERY_TO (Matrix_to_Polygon (OBJECT))
END

FORM (Matrix_to_Sound, "Matrix: To Sound", 0)
	INTEGER ("Row", "1")
	LABEL ("", "(negative values count from last row)")
	OK
DO
	EVERY_TO (Matrix_to_Sound (OBJECT, GET_INTEGER ("Row")))
END

DIRECT (Matrix_to_TableOfReal)
	EVERY_TO (Matrix_to_TableOfReal (OBJECT))
END

DIRECT (Matrix_to_Transition)
	EVERY_TO (Matrix_to_Transition (OBJECT))
END

DIRECT (Matrix_to_VocalTract)
	EVERY_TO (Matrix_to_VocalTract (OBJECT))
END

FORM_WRITE (Matrix_writeToMatrixTextFile, "Write Matrix to matrix text file", 0, "mat")
	if (! Matrix_writeToMatrixTextFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITE (Matrix_writeToHeaderlessSpreadsheetFile, "Write Matrix to spreadsheet", 0, "txt")
	if (! Matrix_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

/***** PAIRDISTRIBUTION *****/

DIRECT (PairDistribution_help) Melder_help ("PairDistribution"); END

FORM (PairDistribution_to_Stringses, "Generate two Strings objects", 0)
	NATURAL ("Number", "1000")
	SENTENCE ("Name of first Strings", "input")
	SENTENCE ("Name of second Strings", "output")
	OK
DO
	Strings strings1, strings2;
	if (! PairDistribution_to_Stringses (ONLY (classPairDistribution), GET_INTEGER ("Number"), & strings1, & strings2)) return 0;
	if (! praat_new (strings1, "%s", GET_STRING ("Name of first Strings"))) { forget (strings2); return 0; }
	if (! praat_new (strings2, "%s", GET_STRING ("Name of second Strings"))) return 0;
END
	
DIRECT (PairDistribution_getFractionCorrect_maximumLikelihood)
	Melder_informationReal (PairDistribution_getFractionCorrect_maximumLikelihood (ONLY_OBJECT), NULL);
	iferror return 0;
END

DIRECT (PairDistribution_getFractionCorrect_probabilityMatching)
	Melder_informationReal (PairDistribution_getFractionCorrect_probabilityMatching (ONLY_OBJECT), NULL);
	iferror return 0;
END

/***** PAIRDISTRIBUTION & DISTRIBUTIONS *****/

FORM (PairDistribution_Distributions_getFractionCorrect, "PairDistribution & Distributions: Get fraction correct", 0)
	NATURAL ("Column", "1")
	OK
DO
	Melder_informationReal (PairDistribution_Distributions_getFractionCorrect
		(ONLY (classPairDistribution), ONLY (classDistributions), GET_INTEGER ("Column")), NULL);
END

/***** PARAMCURVE *****/

FORM (ParamCurve_draw, "Draw parametrized curve", 0)
	REAL ("Tmin", "0.0")
	REAL ("Tmax", "0.0")
	REAL ("Step", "0.0")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (ParamCurve_draw (OBJECT, GRAPHICS,
		GET_REAL ("Tmin"), GET_REAL ("Tmax"), GET_REAL ("Step"),
		GET_REAL ("Xmin"), GET_REAL ("Xmax"), GET_REAL ("Ymin"), GET_REAL ("Ymax"),
		GET_INTEGER ("Garnish")))
END

DIRECT (ParamCurve_help) Melder_help ("ParamCurve"); END

/***** PITCH *****/

DIRECT (Pitch_getNumberOfVoicedFrames)
	Pitch me = ONLY (classPitch);
	Melder_information ("%ld voiced frames", Pitch_countVoicedFrames (me));
END

DIRECT (Pitch_difference)
	Pitch pit1 = NULL, pit2 = NULL;
	WHERE (SELECTED && CLASS == classPitch)
		if (pit1) pit2 = OBJECT; else pit1 = OBJECT;
	Pitch_difference (pit1, pit2);
END

FORM (Pitch_draw, "Pitch: Draw", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_NO, Pitch_yscale_LINEAR))
END

FORM (Pitch_drawErb, "Pitch: Draw erb", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_NO, Pitch_yscale_ERB))
END

FORM (Pitch_drawLogarithmic, "Pitch: Draw logarithmic", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_NO, Pitch_yscale_LOGARITHMIC))
END

FORM (Pitch_drawMel, "Pitch: Draw mel", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_NO, Pitch_yscale_MEL))
END

FORM (Pitch_drawSemitones, "Pitch: Draw semitones", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_NO, Pitch_yscale_SEMITONES))
END

DIRECT (Pitch_edit)
	if (praat.batch)
		return Melder_error ("Cannot edit a Pitch from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor (PitchEditor_create (praat.topShell, FULL_NAME, OBJECT), IOBJECT))
				return 0;
END

FORM (Pitch_formula, "Pitch: Formula", "Formula...")
	LABEL ("", "x = time; col = frame; row = candidate (1 = current path); frequency (time, candidate) :=")
	TEXTFIELD ("formula", "self*2; Example: octave jump up")
	OK
DO
	WHERE (SELECTED) {
		if (! Pitch_formula (OBJECT, GET_STRING ("formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Pitch_getMaximum, "Pitch: Get maximum", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Pitch_getMaximum (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), GET_STRING ("Units"));
END

FORM (Pitch_getMean, "Pitch: Get mean", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	Melder_informationReal (Pitch_getMean (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

FORM (Pitch_getMeanAbsoluteSlope, "Pitch: Get mean absolute slope", 0)
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	int units = GET_INTEGER ("Units");
	double slope;
	long nVoiced = (units == 1 ? Pitch_getMeanAbsSlope_hertz : units == 2 ? Pitch_getMeanAbsSlope_mel : units == 3 ? Pitch_getMeanAbsSlope_semitones : Pitch_getMeanAbsSlope_erb)
		(ONLY (classPitch), & slope);
	Melder_information (nVoiced < 2 ? "--undefined--" : "%.17g %s/s", slope, GET_STRING ("Units"));
END

DIRECT (Pitch_getMeanAbsSlope_noOctave)
	double slope;
	long nVoiced = Pitch_getMeanAbsSlope_noOctave (ONLY (classPitch), & slope);
	Melder_information (nVoiced < 2 ? "--undefined--" : "%.17g Semitones/s", slope);
END

FORM (Pitch_getMinimum, "Pitch: Get minimum", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Pitch_getMinimum (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), GET_STRING ("Units"));
END

FORM (Pitch_getQuantile, "Pitch: Get quantile", 0)
	dia_timeRange (dia);
	REAL ("Quantile", "0.50 (= median)")
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	Melder_informationReal (Pitch_getQuantile (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Quantile"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

/*
FORM (Pitch_getSpreading, "Pitch: Get spreading", 0)
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	REAL ("From quantile", "0.16")
	REAL ("To quantile", "0.84")
	OK
DO
	int units = GET_INTEGER ("Units") - 1;
	double fromQuantile = GET_REAL ("From quantile");
	double toQuantile = GET_REAL ("To quantile");
	double lowValue, highValue;
	Pitch pitch = ONLY (classPitch);
	long nVoiced = (units == 1 ? Pitch_getQuantile_hertz : units == 2 ? Pitch_getQuantile_mel : units == 3 ? Pitch_getQuantile_semitones : Pitch_getQuantile_erb)
		(pitch, fromQuantile, & lowValue);
	(void) (units == 1 ? Pitch_getQuantile_hertz : units == 2 ? Pitch_getQuantile_mel : units == 3 ? Pitch_getQuantile_semitones : Pitch_getQuantile_erb)
		(pitch, toQuantile, & highValue);
	Melder_information (nVoiced < 2 ? "<undefined>" : "%.17g %s", highValue - lowValue, GET_STRING ("Units"));
END*/

FORM (Pitch_getStandardDeviation, "Pitch: Get standard deviation", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	Melder_informationReal (Pitch_getStandardDeviation (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

FORM (Pitch_getTimeOfMaximum, "Pitch: Get time of maximum", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Pitch_getTimeOfMaximum (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Pitch_getTimeOfMinimum, "Pitch: Get time of minimum", 0)
	dia_timeRange (dia);
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	OK
DO
	Melder_informationReal (Pitch_getTimeOfMinimum (ONLY (classPitch),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Pitch_getValueAtTime, "Pitch: Get value", "Pitch: Get value at time...")
	REAL ("Time (s)", "0.5")
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	RADIO ("Interpolation", 2)
	RADIOBUTTON ("Nearest")
	RADIOBUTTON ("Linear")
	OK
DO
	Melder_informationReal (Pitch_getValueAtTime (ONLY (classPitch), GET_REAL ("Time"),
		GET_INTEGER ("Units") - 1, GET_INTEGER ("Interpolation") - 1), GET_STRING ("Units"));
END
	
FORM (Pitch_getValueInFrame, "Pitch: Get value in frame", "Pitch: Get value in frame...")
	INTEGER ("Frame number", "10")
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	Melder_informationReal (Pitch_getValueInFrame (ONLY (classPitch), GET_INTEGER ("Frame number"),
		GET_INTEGER ("Units") - 1), GET_STRING ("Units"));
END

DIRECT (Pitch_help) Melder_help ("Pitch"); END

DIRECT (Pitch_hum)
	EVERY_CHECK (Pitch_hum (OBJECT, 0, 0))
END

DIRECT (Pitch_interpolate)
	EVERY_TO (Pitch_interpolate (OBJECT))
END

DIRECT (Pitch_killOctaveJumps)
	EVERY_TO (Pitch_killOctaveJumps (OBJECT))
END

DIRECT (Pitch_play)
	EVERY_CHECK (Pitch_play (OBJECT, 0, 0))
END

FORM (Pitch_smooth, "Pitch: Smooth", "Pitch: Smooth...")
	REAL ("Bandwidth (Hertz)", "10.0")
	OK
DO
	EVERY_TO (Pitch_smooth (OBJECT, GET_REAL ("Bandwidth")))
END

FORM (Pitch_speckle, "Pitch: Speckle", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_YES, Pitch_yscale_LINEAR))
END

FORM (Pitch_speckleErb, "Pitch: Speckle erb", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_YES, Pitch_yscale_ERB))
END

FORM (Pitch_speckleLogarithmic, "Pitch: Speckle logarithmic", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_YES, Pitch_yscale_LOGARITHMIC))
END

FORM (Pitch_speckleMel, "Pitch: Speckle mel", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_YES, Pitch_yscale_MEL))
END

FORM (Pitch_speckleSemitones, "Pitch: Speckle semitones", "Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	EVERY_DRAW (Pitch_draw (OBJECT, GRAPHICS, tmin, tmax, fmin, fmax,
		GET_INTEGER ("Garnish"), Pitch_speckle_YES, Pitch_yscale_SEMITONES))
END

FORM (Pitch_subtractLinearFit, "Pitch: subtract linear fit", 0)
	RADIO ("Units", 1)   RADIOBUTTON ("Hertz")   RADIOBUTTON ("Mel")   RADIOBUTTON ("Semitones")   RADIOBUTTON ("ERB")
	OK
DO
	EVERY_TO (Pitch_subtractLinearFit (OBJECT, GET_INTEGER ("Units") - 1))
END

DIRECT (Pitch_to_IntervalTier)
	EVERY_TO (IntervalTier_create (((Pitch) OBJECT) -> xmin, ((Pitch) OBJECT) -> xmax))
END

DIRECT (Pitch_to_Matrix)
	EVERY_TO (Pitch_to_Matrix (OBJECT))
END

DIRECT (Pitch_to_PitchTier)
	EVERY_TO (Pitch_to_PitchTier (OBJECT))
END

DIRECT (Pitch_to_PointProcess)
	EVERY_TO (Pitch_to_PointProcess (OBJECT))
END

DIRECT (Pitch_to_Sound_pulses)
	EVERY_TO (Pitch_to_Sound (OBJECT, 0, 0, FALSE))
END

DIRECT (Pitch_to_Sound_hum)
	EVERY_TO (Pitch_to_Sound (OBJECT, 0, 0, TRUE))
END

FORM (Pitch_to_Sound_sine, "Pitch: To Sound (sine)", 0)
	POSITIVE ("Sampling frequency (Hz)", "22050")
	RADIO ("Cut voiceless stretches", 2)
		OPTION ("exactly")
		OPTION ("at nearest zero crossings")
	OK
DO
	EVERY_TO (Pitch_to_Sound_sine (OBJECT, 0, 0, GET_REAL ("Sampling frequency"), GET_INTEGER ("Cut voiceless stretches") - 1))
END

FORM (Pitch_to_TextGrid, "To TextGrid...", "Pitch: To TextGrid...")
	SENTENCE ("Tier names", "Mary John bell")
	SENTENCE ("Point tiers", "bell")
	OK
DO
	EVERY_TO (TextGrid_create (((Pitch) OBJECT) -> xmin, ((Pitch) OBJECT) -> xmax,
		GET_STRING ("Tier names"), GET_STRING ("Point tiers")))
END

DIRECT (Pitch_to_TextTier)
	EVERY_TO (TextTier_create (((Pitch) OBJECT) -> xmin, ((Pitch) OBJECT) -> xmax))
END

/***** PITCH & PITCHTIER *****/

FORM (PitchTier_Pitch_draw, "PitchTier & Pitch: Draw", 0)
	dia_timeRange (dia);
	REAL ("From frequency (Hz)", "0.0")
	REAL ("To frequency (Hz)", "500.0")
	RADIO ("Line type for non-periodic intervals", 2)
		RADIOBUTTON ("Normal")
		RADIOBUTTON ("Dotted")
		RADIOBUTTON ("Blank")
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	PitchTier_Pitch_draw (ONLY (classPitchTier), ONLY (classPitch), GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("From frequency"), GET_REAL ("To frequency"),
		GET_INTEGER ("Line type for non-periodic intervals") - 1,
		GET_INTEGER ("Garnish"));
	praat_picture_close ();
END

DIRECT (Pitch_PitchTier_to_Pitch)
	Pitch pitch = ONLY (classPitch);
	PitchTier tier = ONLY (classPitchTier);
	if (! praat_new (Pitch_PitchTier_to_Pitch (pitch, tier),
		"%s_stylized", pitch -> name)) return 0;
END

/***** PITCH & POINTPROCESS *****/

DIRECT (Pitch_PointProcess_to_PitchTier)
	Pitch pitch = ONLY (classPitch);
	if (! praat_new (Pitch_PointProcess_to_PitchTier (pitch, ONLY (classPointProcess)), pitch -> name)) return 0;
END

/***** PITCH & SOUND *****/

DIRECT (Sound_Pitch_to_Manipulation)
	Pitch pitch = ONLY (classPitch);
	if (! praat_new (Sound_Pitch_to_Manipulation (ONLY (classSound), pitch), pitch -> name)) return 0;
END

DIRECT (Sound_Pitch_to_PointProcess_cc)
	char name [200];
	praat_name2 (name, classSound, classPitch);
	if (! praat_new (Sound_Pitch_to_PointProcess_cc (ONLY (classSound), ONLY (classPitch)), name)) return 0;
END

FORM (Sound_Pitch_to_PointProcess_peaks, "Sound & Pitch: To PointProcess (peaks)", 0)
	BOOLEAN ("Include maxima", 1)
	BOOLEAN ("Include minima", 0)
	OK
DO
	char name [200];
	praat_name2 (name, classSound, classPitch);
	if (! praat_new (Sound_Pitch_to_PointProcess_peaks (ONLY (classSound), ONLY (classPitch),
		GET_INTEGER ("Include maxima"), GET_INTEGER ("Include minima")), name)) return 0;
END

/***** PITCH & TEXTGRID *****/

static int pr_TextGrid_Pitch_draw (Any dia, int speckle, int yscale) {
	GET_TMIN_TMAX_FMIN_FMAX
	praat_picture_open ();
	TextGrid_Pitch_draw (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		GET_INTEGER ("Tier"), tmin, tmax, fmin, fmax, GET_INTEGER ("Font size"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish"), speckle, yscale);
	praat_picture_close ();
	return 1;
}

FORM (TextGrid_Pitch_draw, "TextGrid & Pitch: Draw", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_yscale_LINEAR)) return 0;
END

FORM (TextGrid_Pitch_drawErb, "TextGrid & Pitch: Draw erb", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_yscale_ERB)) return 0;
END

FORM (TextGrid_Pitch_drawLogarithmic, "TextGrid & Pitch: Draw logarithmic", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_yscale_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_drawMel, "TextGrid & Pitch: Draw mel", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_yscale_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSemitones, "TextGrid & Pitch: Draw semitones", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_yscale_SEMITONES)) return 0;
END

static int pr_TextGrid_Pitch_drawSeparately (Any dia, int speckle, int yscale) {
	GET_TMIN_TMAX_FMIN_FMAX
	praat_picture_open ();
	TextGrid_Pitch_drawSeparately (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		tmin, tmax, fmin, fmax, GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish"), speckle, yscale);
	praat_picture_close ();
	return 1;
}

FORM (TextGrid_Pitch_drawSeparately, "TextGrid & Pitch: Draw separately", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	REAL (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_yscale_LINEAR)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyErb, "TextGrid & Pitch: Draw separately erb", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_yscale_ERB)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyLogarithmic, "TextGrid & Pitch: Draw separately logarithmic", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_yscale_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyMel, "TextGrid & Pitch: Draw separately mel", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_yscale_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelySemitones, "TextGrid & Pitch: Draw separately semitones", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_yscale_SEMITONES)) return 0;
END

FORM (TextGrid_Pitch_speckle, "TextGrid & Pitch: Speckle", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_yscale_LINEAR)) return 0;
END

FORM (TextGrid_Pitch_speckleErb, "TextGrid & Pitch: Speckle erb", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_yscale_ERB)) return 0;
END

FORM (TextGrid_Pitch_speckleLogarithmic, "TextGrid & Pitch: Speckle logarithmic", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_yscale_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_speckleMel, "TextGrid & Pitch: Speckle mel", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_yscale_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSemitones, "TextGrid & Pitch: Speckle semitones", 0)
	INTEGER ("Tier", "1")
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_yscale_SEMITONES)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparately, "TextGrid & Pitch: Speckle separately", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	REAL (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_yscale_LINEAR)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyErb, "TextGrid & Pitch: Speckle separately erb", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_yscale_ERB)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyLogarithmic, "TextGrid & Pitch: Speckle separately logarithmic", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_yscale_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyMel, "TextGrid & Pitch: Speckle separately mel", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_yscale_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelySemitones, "TextGrid & Pitch: Speckle separately semitones", 0)
	REAL (STRING_FROM_TIME_SECONDS, "0.0")
	REAL (STRING_TO_TIME_SECONDS, "0.0 (= all)")
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_yscale_SEMITONES)) return 0;
END

/***** PITCH & TEXTTIER *****/

FORM (Pitch_TextTier_to_PitchTier, "Pitch & TextTier to PitchTier", "Pitch & TextTier: To PitchTier...")
	RADIO ("Unvoiced strategy", 3)
	RADIOBUTTON ("Zero")
	RADIOBUTTON ("Error")
	RADIOBUTTON ("Interpolate")
	OK
DO
	if (! praat_new (Pitch_AnyTier_to_PitchTier (ONLY (classPitch), ONLY (classTextTier),
		GET_INTEGER ("Unvoiced strategy") - 1), ((Pitch) (ONLY (classPitch))) -> name)) return 0;
END

/***** PITCHTIER *****/

FORM (PitchTier_addPoint, "PitchTier: Add point", "PitchTier: Add point...")
	REAL ("Time (s)", "0.5")
	REAL ("Pitch (Hz)", "200")
	OK
DO
	WHERE (SELECTED) {
		if (! RealTier_addPoint (OBJECT, GET_REAL ("Time"), GET_REAL ("Pitch"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (PitchTier_create, "Create empty PitchTier", NULL)
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double startingTime = GET_REAL ("Starting time"), finishingTime = GET_REAL ("Finishing time");
	REQUIRE (finishingTime > startingTime, "Finishing time must be greater than starting time.")
	if (! praat_new (PitchTier_create (startingTime, finishingTime), GET_STRING ("Name"))) return 0;
END

DIRECT (PitchTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

FORM (PitchTier_downto_TableOfReal, "PitchTier: Down to TableOfReal", NULL)
	RADIO ("Units", 1)
	RADIOBUTTON ("Hertz")
	RADIOBUTTON ("Semitones")
	OK
DO
	EVERY_TO (PitchTier_downto_TableOfReal (OBJECT, GET_INTEGER ("Units") - 1))
END

FORM (PitchTier_draw, "PitchTier: Draw", 0)
	dia_timeRange (dia);
	REAL ("Minimum frequency (Hz)", "0.0")
	POSITIVE ("Maximum frequency (Hz)", "500.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	double minimumFrequency = GET_REAL ("Minimum frequency");
	double maximumFrequency = GET_REAL ("Maximum frequency");
	REQUIRE (maximumFrequency > minimumFrequency,
		"Maximum frequency must be greater than minimum frequency.")
	EVERY_DRAW (PitchTier_draw (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), minimumFrequency, maximumFrequency,
		GET_INTEGER ("Garnish")))
END

DIRECT (PitchTier_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a PitchTier from batch.");
	} else {
		Sound sound = NULL;
		WHERE (SELECTED)
			if (CLASS == classSound) sound = OBJECT;
		WHERE (SELECTED && CLASS == classPitchTier)
			if (! praat_installEditor (PitchTierEditor_create (praat.topShell, FULL_NAME,
				OBJECT, sound, TRUE), IOBJECT)) return 0;
	}
END

FORM (PitchTier_formula, "PitchTier: Formula", "PitchTier: Formula...")
	LABEL ("", "# ncol = the number of points")
	LABEL ("", "for col from 1 to ncol")
	LABEL ("", "   # x = the time of the colth point, in seconds")
	LABEL ("", "   # self = the value of the colth point, in Hertz")
	LABEL ("", "   self = `formula'")
	LABEL ("", "endfor")
	TEXTFIELD ("formula", "self * 2 ; one octave up")
	OK
DO
	WHERE_DOWN (SELECTED) {
		RealTier_formula (OBJECT, GET_STRING ("formula"), NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (PitchTier_getMean_curve, "PitchTier: Get mean (curve)", "PitchTier: Get mean (curve)...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getMean_curve (ONLY_OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Hz");
END
	
FORM (PitchTier_getMean_points, "PitchTier: Get mean (points)", "PitchTier: Get mean (points)...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getMean_points (ONLY_OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Hz");
END
	
FORM (PitchTier_getStandardDeviation_curve, "PitchTier: Get standard deviation (curve)", "PitchTier: Get standard deviation (curve)...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_curve (ONLY_OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Hz");
END
	
FORM (PitchTier_getStandardDeviation_points, "PitchTier: Get standard deviation (points)", "PitchTier: Get standard deviation (points)...")
	dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_points (ONLY_OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Hz");
END
	
FORM (PitchTier_getValueAtTime, "PitchTier: Get value at time", "PitchTier: Get value at time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL ("Time")), "Hz");
END
	
FORM (PitchTier_getValueAtIndex, "PitchTier: Get value at index", "PitchTier: Get value at index...")
	INTEGER ("Point number", "10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER ("Point number")), "Hz");
END

DIRECT (PitchTier_help) Melder_help ("PitchTier"); END

DIRECT (PitchTier_hum)
	EVERY_CHECK (PitchTier_hum (OBJECT))
END

FORM (PitchTier_interpolateQuadratically, "PitchTier: Interpolate quadratically", 0)
	NATURAL ("Number of points per parabola", "4")
	RADIO ("Units", 2)
	RADIOBUTTON ("Hz")
	RADIOBUTTON ("Semitones")
	OK
DO
	WHERE (SELECTED) {
		RealTier_interpolateQuadratically (OBJECT, GET_INTEGER ("Number of points per parabola"), GET_INTEGER ("Units") - 1);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (PitchTier_play)
	EVERY_CHECK (PitchTier_play (OBJECT))
END

DIRECT (PitchTier_playSine)
	EVERY_CHECK (PitchTier_playPart_sine (OBJECT, 0.0, 0.0))
END

FORM (PitchTier_shiftFrequencies, "PitchTier: Shift frequencies", 0)
	REAL ("left Time domain (s)", "0.0")
	REAL ("right Time domain (s)", "1000.0")
	REAL ("Frequency shift", "-20.0")
	RADIO ("Units", 1)
		RADIOBUTTON ("Hertz")
		RADIOBUTTON ("Mel")
		RADIOBUTTON ("Semitones")
		RADIOBUTTON ("Erb")
	OK
DO
	WHERE (SELECTED) {
		PitchTier_shiftFrequencies (OBJECT, GET_REAL ("left Time domain"), GET_REAL ("right Time domain"),
			GET_REAL ("Frequency shift"), GET_INTEGER ("Units") - 1);
		iferror return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (PitchTier_multiplyFrequencies, "PitchTier: Multiply frequencies", 0)
	REAL ("left Time domain (s)", "0.0")
	REAL ("right Time domain (s)", "1000.0")
	POSITIVE ("Factor", "1.2")
	OK
DO
	WHERE (SELECTED) {
		PitchTier_multiplyFrequencies (OBJECT,
			GET_REAL ("left Time domain"), GET_REAL ("right Time domain"), GET_REAL ("Factor"));
		praat_dataChanged (OBJECT);
	}
END

FORM (PitchTier_stylize, "PitchTier: Stylize", 0)
	REAL ("Frequency resolution", "4.0")
	RADIO ("Units", 2)
	RADIOBUTTON ("Hz")
	RADIOBUTTON ("Semitones")
	OK
DO
	WHERE (SELECTED) {
		PitchTier_stylize (OBJECT, GET_REAL ("Frequency resolution"), GET_INTEGER ("Units") - 1);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (PitchTier_to_PointProcess)
	EVERY_TO (PitchTier_to_PointProcess (OBJECT))
END

FORM (PitchTier_to_Sound_pulseTrain, "PitchTier: To Sound (pulse train)", 0)
	POSITIVE ("Sampling frequency (Hz)", "22050")
	POSITIVE ("Adaptation factor", "1.0")
	POSITIVE ("Adaptation time", "0.05")
	NATURAL ("Interpolation depth (samples)", "2000")
	BOOLEAN ("Hum", 0)
	OK
DO
	EVERY_TO (PitchTier_to_Sound_pulseTrain (OBJECT, GET_REAL ("Sampling frequency"),
		GET_REAL ("Adaptation factor"), GET_REAL ("Adaptation time"),
		GET_INTEGER ("Interpolation depth"), GET_INTEGER ("Hum")))
END

FORM (PitchTier_to_Sound_sine, "PitchTier: To Sound (sine)", 0)
	POSITIVE ("Sampling frequency (Hz)", "22050")
	OK
DO
	EVERY_TO (PitchTier_to_Sound_sine (OBJECT, 0.0, 0.0, GET_REAL ("Sampling frequency")))
END

DIRECT (info_PitchTier_Sound_edit)
	Melder_information ("To include a copy of a Sound in your PitchTier editor:\n"
		"   select a PitchTier and a Sound, and click \"Edit\".");
END

FORM_WRITE (PitchTier_writeToPitchTierSpreadsheetFile, "Write PitchTier to spreadsheet", 0, "PitchTier")
	if (! PitchTier_writeToPitchTierSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITE (PitchTier_writeToHeaderlessSpreadsheetFile, "Write PitchTier to spreadsheet", 0, "txt")
	if (! PitchTier_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

/***** PITCHTIER & POINTPROCESS *****/

DIRECT (PitchTier_PointProcess_to_PitchTier)
	PitchTier pitch = ONLY (classPitchTier);
	if (! praat_new (PitchTier_PointProcess_to_PitchTier (pitch, ONLY (classPointProcess)), pitch -> name)) return 0;
END

/***** POINTPROCESS *****/

FORM (PointProcess_addPoint, "PointProcess: Add point", "PointProcess: Add point...")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! PointProcess_addPoint (OBJECT, GET_REAL ("Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (PointProcess_createEmpty, "Create an empty PointProcess", "Create empty PointProcess...")
	WORD ("Name", "empty")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	OK
DO
	double tmin = GET_REAL ("Starting time"), tmax = GET_REAL ("Finishing time");
	if (tmax < tmin)
		return Melder_error ("Finishing time (%.8g) should not be less than starting time (%.8g).", tmax, tmin);
	if (! praat_new (PointProcess_create (tmin, tmax, 0), GET_STRING ("Name"))) return 0;
END

FORM (PointProcess_createPoissonProcess, "Create Poisson process", "Create Poisson process...")
	WORD ("Name", "poisson")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	POSITIVE ("Density (/s)", "100.0")
	OK
DO
	double tmin = GET_REAL ("Starting time"), tmax = GET_REAL ("Finishing time");
	if (tmax < tmin)
		return Melder_error ("Finishing time (%.8g) should not be less than starting time (%.8g).", tmax, tmin);
	if (! praat_new (PointProcess_createPoissonProcess (tmin, tmax, GET_REAL ("Density")), GET_STRING ("Name"))) return 0;
END

DIRECT (PointProcess_difference)
	PointProcess point1 = NULL, point2 = NULL;
	WHERE (SELECTED) if (point1) point2 = OBJECT; else point1 = OBJECT;
	if (! praat_new (PointProcesses_difference (point1, point2), "difference")) return 0;
END

FORM (PointProcess_draw, "PointProcess: Draw", 0)
	dia_timeRange (dia);
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (PointProcess_draw (OBJECT, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Garnish")))
END

DIRECT (PointProcess_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a PointProcess from batch.");
	} else {
		Sound sound = NULL;
		WHERE (SELECTED)
			if (CLASS == classSound) sound = OBJECT;
		WHERE (SELECTED && CLASS == classPointProcess)
			if (! praat_installEditor (PointEditor_create (praat.topShell, FULL_NAME,
				OBJECT, sound, TRUE), IOBJECT)) return 0;
	}
END

FORM (PointProcess_fill, "PointProcess: Fill", 0)
	dia_timeRange (dia);
	POSITIVE ("Period (s)", "0.01")
	OK
DO
	WHERE (SELECTED) {
		int status = PointProcess_fill (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_REAL ("Period"));
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (PointProcess_getInterval, "PointProcess: Get interval", "PointProcess: Get interval...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (PointProcess_getInterval (ONLY (classPointProcess), GET_REAL ("Time")), "seconds");
END

static void dia_PointProcess_getRangeProperty (Any dia) {
	dia_timeRange (dia);
	REAL ("Shortest period (s)", "0.0001")
	REAL ("Longest period (s)", "0.02")
	POSITIVE ("Maximum period factor", "1.3")
}

FORM (PointProcess_getJitter_local, "PointProcess: Get jitter (local)", "PointProcess: Get jitter (local)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_local_absolute, "PointProcess: Get jitter (local, absolute)", "PointProcess: Get jitter (local, absolute)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local_absolute (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), " seconds");
END

FORM (PointProcess_getJitter_rap, "PointProcess: Get jitter (rap)", "PointProcess: Get jitter (rap)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_rap (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_ppq5, "PointProcess: Get jitter (ppq5)", "PointProcess: Get jitter (ppq5)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ppq5 (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_ddp, "PointProcess: Get jitter (ddp)", "PointProcess: Get jitter (ddp)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ddp (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), NULL);
END

FORM (PointProcess_getMeanPeriod, "PointProcess: Get mean period", "PointProcess: Get mean period...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getMeanPeriod (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), "seconds");
END

FORM (PointProcess_getStdevPeriod, "PointProcess: Get stdev period", "PointProcess: Get stdev period...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getStdevPeriod (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), "seconds");
END

FORM (PointProcess_getLowIndex, "PointProcess: Get low index", "PointProcess: Get low index...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_information ("%ld", PointProcess_getLowIndex (ONLY_OBJECT, GET_REAL ("Time")));
END

FORM (PointProcess_getHighIndex, "PointProcess: Get high index", "PointProcess: Get high index...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_information ("%ld", PointProcess_getHighIndex (ONLY_OBJECT, GET_REAL ("Time")));
END

FORM (PointProcess_getNearestIndex, "PointProcess: Get nearest index", "PointProcess: Get nearest index...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_information ("%ld", PointProcess_getNearestIndex (ONLY_OBJECT, GET_REAL ("Time")));
END

DIRECT (PointProcess_getNumberOfPoints)
	PointProcess me = ONLY_OBJECT;
	Melder_information ("%ld", my nt);
END

FORM (PointProcess_getNumberOfPeriods, "PointProcess: Get number of periods", "PointProcess: Get number of periods...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_information ("%ld", PointProcess_getNumberOfPeriods (ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")));
END

FORM (PointProcess_getTimeFromIndex, "Get time", 0 /*"PointProcess: Get time from index..."*/)
	NATURAL ("Point number", "10")
	OK
DO
	PointProcess me = ONLY_OBJECT;
	long i = GET_INTEGER ("Point number");
	if (i > my nt) Melder_information ("--undefined--");
	else Melder_informationReal (my t [i], "seconds");
END

DIRECT (PointProcess_help) Melder_help ("PointProcess"); END

DIRECT (PointProcess_hum)
	EVERY_CHECK (PointProcess_hum (OBJECT,
		((PointProcess) OBJECT) -> xmin, ((PointProcess) OBJECT) -> xmax))
END

DIRECT (PointProcess_intersection)
	PointProcess point1 = NULL, point2 = NULL;
	WHERE (SELECTED) if (point1) point2 = OBJECT; else point1 = OBJECT;
	if (! praat_new (PointProcesses_intersection (point1, point2), "intersection")) return 0;
END

DIRECT (PointProcess_play)
	EVERY_CHECK (PointProcess_play (OBJECT))
END

FORM (PointProcess_removePoint, "PointProcess: Remove point", "PointProcess: Remove point...")
	NATURAL ("Index", "1")
	OK
DO
	WHERE (SELECTED) {
		PointProcess_removePoint (OBJECT, GET_INTEGER ("Index"));
		praat_dataChanged (OBJECT);
	}
END

FORM (PointProcess_removePointNear, "PointProcess: Remove point near", "PointProcess: Remove point near...")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		PointProcess_removePointNear (OBJECT, GET_REAL ("Time"));
		praat_dataChanged (OBJECT);
	}
END

FORM (PointProcess_removePoints, "PointProcess: Remove points", "PointProcess: Remove points...")
	NATURAL ("From index", "1")
	NATURAL ("To index", "10")
	OK
DO
	WHERE (SELECTED) {
		PointProcess_removePoints (OBJECT, GET_INTEGER ("From index"), GET_INTEGER ("To index"));
		praat_dataChanged (OBJECT);
	}
END

FORM (PointProcess_removePointsBetween, "PointProcess: Remove points between", "PointProcess: Remove points between...")
	REAL ("left Time range (s)", "0.3")
	REAL ("right Time range (s)", "0.7")
	OK
DO
	WHERE (SELECTED) {
		PointProcess_removePointsBetween (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (PointProcess_to_IntervalTier)
	EVERY_TO (IntervalTier_create (((PointProcess) OBJECT) -> xmin, ((PointProcess) OBJECT) -> xmax))
END

DIRECT (PointProcess_to_Matrix)
	EVERY_TO (PointProcess_to_Matrix (OBJECT))
END

FORM (PointProcess_to_PitchTier, "PointProcess: To PitchTier", "PointProcess: To PitchTier...")
	POSITIVE ("Maximum interval (s)", "0.02")
	OK
DO
	EVERY_TO (PointProcess_to_PitchTier (OBJECT, GET_REAL ("Maximum interval")))
END

FORM (PointProcess_to_TextGrid, "PointProcess: To TextGrid...", "PointProcess: To TextGrid...")
	SENTENCE ("Tier names", "Mary John bell")
	SENTENCE ("Point tiers", "bell")
	OK
DO
	EVERY_TO (TextGrid_create (((PointProcess) OBJECT) -> xmin, ((PointProcess) OBJECT) -> xmax,
		GET_STRING ("Tier names"), GET_STRING ("Point tiers")))
END

FORM (PointProcess_to_TextGrid_vuv, "PointProcess: To TextGrid (vuv)...", "PointProcess: To TextGrid (vuv)...")
	POSITIVE ("Maximum period (s)", "0.02")
	REAL ("Mean period (s)", "0.01")
	OK
DO
	EVERY_TO (PointProcess_to_TextGrid_vuv (OBJECT, GET_REAL ("Maximum period"),
		GET_REAL ("Mean period")))
END

DIRECT (PointProcess_to_TextTier)
	EVERY_TO (TextTier_create (((PointProcess) OBJECT) -> xmin, ((PointProcess) OBJECT) -> xmax))
END

FORM (PointProcess_to_Sound, "PointProcess: To Sound (pulse train)", "PointProcess: To Sound (pulse train)...")
	POSITIVE ("Sampling frequency (Hz)", "22050")
	POSITIVE ("Adaptation factor", "1.0")
	POSITIVE ("Adaptation time", "0.05")
	NATURAL ("Interpolation depth (samples)", "2000")
	OK
DO
	EVERY_TO (PointProcess_to_Sound (OBJECT, GET_REAL ("Sampling frequency"),
		GET_REAL ("Adaptation factor"), GET_REAL ("Adaptation time"),
		GET_INTEGER ("Interpolation depth")))
END

DIRECT (PointProcess_to_Sound_hum)
	EVERY_TO (PointProcess_to_Sound_hum (OBJECT))
END

DIRECT (PointProcess_union)
	PointProcess point1 = NULL, point2 = NULL;
	WHERE (SELECTED) if (point1) point2 = OBJECT; else point1 = OBJECT;
	if (! praat_new (PointProcesses_union (point1, point2), "union")) return 0;
END

FORM (PointProcess_upto_IntensityTier, "PointProcess: Up to IntensityTier", "PointProcess: Up to IntensityTier...")
	POSITIVE ("Intensity (dB)", "70.0")
	OK
DO
	EVERY_TO (PointProcess_upto_IntensityTier (OBJECT, GET_REAL ("Intensity")))
END

FORM (PointProcess_upto_PitchTier, "PointProcess: Up to PitchTier", "PointProcess: Up to PitchTier...")
	POSITIVE ("Frequency (Hz)", "190.0")
	OK
DO
	EVERY_TO (PointProcess_upto_PitchTier (OBJECT, GET_REAL ("Frequency")))
END

FORM (PointProcess_upto_TextTier, "PointProcess: Up to TextTier", "PointProcess: Up to TextTier...")
	SENTENCE ("Text", "")
	OK
DO
	EVERY_TO (PointProcess_upto_TextTier (OBJECT, GET_STRING ("Text")))
END

FORM (PointProcess_voice, "PointProcess: Fill unvoiced parts", 0)
	POSITIVE ("Period (s)", "0.01")
	POSITIVE ("Maximum voiced period (s)", "0.02000000001")
	OK
DO
	WHERE (SELECTED) {
		int status = PointProcess_voice (OBJECT, GET_REAL ("Period"), GET_REAL ("Maximum voiced period"));
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

DIRECT (info_PointProcess_Sound_edit)
	Melder_information ("To include a copy of a Sound in your PointProcess editor:\n"
		"   select a PointProcess and a Sound, and click \"Edit\".");
END

/***** POINTPROCESS & SOUND *****/

/*DIRECT (Sound_PointProcess_to_Manipulation)
	Sound sound = ONLY (classSound);
	PointProcess point = ONLY (classPointProcess);
	if (! praat_new (Sound_PointProcess_to_Manipulation (sound, point), point -> name)) return 0;
END*/

DIRECT (Point_Sound_transplantDomain)
	PointProcess point = ONLY (classPointProcess);
	Sound sound = ONLY (classSound);
	point -> xmin = sound -> xmin;
	point -> xmax = sound -> xmax;
	praat_dataChanged (point);
END

FORM (Point_Sound_getShimmer_local, "PointProcess & Sound: Get shimmer (local)", "PointProcess & Sound: Get shimmer (local)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_local (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (Point_Sound_getShimmer_local_dB, "PointProcess & Sound: Get shimmer (local, dB)", "PointProcess & Sound: Get shimmer (local, dB)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_local_dB (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (Point_Sound_getShimmer_apq3, "PointProcess & Sound: Get shimmer (apq3)", "PointProcess & Sound: Get shimmer (apq3)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_apq3 (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (Point_Sound_getShimmer_apq5, "PointProcess & Sound: Get shimmer (apq)", "PointProcess & Sound: Get shimmer (apq5)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_apq5 (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (Point_Sound_getShimmer_apq11, "PointProcess & Sound: Get shimmer (apq11)", "PointProcess & Sound: Get shimmer (apq11)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_apq11 (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (Point_Sound_getShimmer_dda, "PointProcess & Sound: Get shimmer (dda)", "PointProcess & Sound: Get shimmer (dda)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE ("Maximum amplitude factor", "1.6")
	OK
DO
	Melder_informationReal (PointProcess_Sound_getShimmer_dda (ONLY (classPointProcess), ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor")), NULL);
END

FORM (PointProcess_Sound_to_AmplitudeTier_period, "PointProcess & Sound: To AmplitudeTier (period)", 0)
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	PointProcess point = ONLY (classPointProcess);
	Sound sound = ONLY (classSound);
	if (! praat_new (PointProcess_Sound_to_AmplitudeTier_period (point, sound,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), "%s_%s", sound -> name, point -> name)) return 0;
END

DIRECT (PointProcess_Sound_to_AmplitudeTier_point)
	PointProcess point = ONLY (classPointProcess);
	Sound sound = ONLY (classSound);
	if (! praat_new (PointProcess_Sound_to_AmplitudeTier_point (point, sound), "%s_%s", sound -> name, point -> name)) return 0;
END

FORM (PointProcess_Sound_to_Ltas, "PointProcess & Sound: To Ltas", 0)
	POSITIVE ("Maximum frequency (Hz)", "5000")
	POSITIVE ("Band width (Hz)", "100")
	REAL ("Shortest period (s)", "0.0001")
	REAL ("Longest period (s)", "0.02")
	POSITIVE ("Maximum period factor", "1.3")
	OK
DO
	PointProcess point = ONLY (classPointProcess);
	Sound sound = ONLY (classSound);
	if (! praat_new (PointProcess_Sound_to_Ltas (point, sound,
		GET_REAL ("Maximum frequency"), GET_REAL ("Band width"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), "%s", sound -> name)) return 0;
END

FORM (PointProcess_Sound_to_Ltas_harmonics, "PointProcess & Sound: To Ltas (harmonics", 0)
	NATURAL ("Maximum harmonic", "20")
	REAL ("Shortest period (s)", "0.0001")
	REAL ("Longest period (s)", "0.02")
	POSITIVE ("Maximum period factor", "1.3")
	OK
DO
	PointProcess point = ONLY (classPointProcess);
	Sound sound = ONLY (classSound);
	if (! praat_new (PointProcess_Sound_to_Ltas_harmonics (point, sound,
		GET_INTEGER ("Maximum harmonic"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")), "%s", sound -> name)) return 0;
END

/***** POLYGON *****/

FORM (Polygon_draw, "Polygon: Draw", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0")
	OK
DO
	EVERY_DRAW (Polygon_draw (OBJECT, GRAPHICS, GET_REAL ("Xmin"), GET_REAL ("Xmax"),
		GET_REAL ("Ymin"), GET_REAL ("Ymax")))
END

FORM (Polygon_drawCircles, "Polygon: Draw circles", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0 (= all)")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0 (= all)")
	POSITIVE ("Diameter (mm)", "3")
	OK
DO
	EVERY_DRAW (Polygon_drawCircles (OBJECT, GRAPHICS,
		GET_REAL ("Xmin"), GET_REAL ("Xmax"), GET_REAL ("Ymin"), GET_REAL ("Ymax"),
		GET_REAL ("Diameter")))
END

FORM (Polygons_drawConnection, "Polygons: Draw connection", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0 (= all)")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0 (= all)")
	BOOLEAN ("Arrow", 0)
	POSITIVE ("Relative length", "0.9")
	OK
DO
	Polygon polygon1 = NULL, polygon2 = NULL;
	WHERE (SELECTED) if (polygon1) polygon2 = OBJECT; else polygon1 = OBJECT;
	EVERY_DRAW (Polygons_drawConnection (polygon1, polygon2, GRAPHICS,
		GET_REAL ("Xmin"), GET_REAL ("Xmax"), GET_REAL ("Ymin"), GET_REAL ("Ymax"),
		GET_INTEGER ("Arrow"), GET_REAL ("Relative length")))
END

DIRECT (Polygon_help) Melder_help ("Polygon"); END

FORM (Polygon_paint, "Polygon: Paint", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0 (= all)")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0 (= all)")
	OK
DO
	EVERY_DRAW (Polygon_paint (OBJECT, GRAPHICS, GET_REAL ("Colour"),
		GET_REAL ("Xmin"), GET_REAL ("Xmax"), GET_REAL ("Ymin"), GET_REAL ("Ymax")))
END

FORM (Polygon_paintCircles, "Polygon: Paint circles", 0)
	REAL ("Xmin", "0.0")
	REAL ("Xmax", "0.0 (= all)")
	REAL ("Ymin", "0.0")
	REAL ("Ymax", "0.0 (= all)")
	POSITIVE ("Diameter (mm)", "3")
	OK
DO
	EVERY_DRAW (Polygon_paintCircles (OBJECT, GRAPHICS,
		GET_REAL ("Xmin"), GET_REAL ("Xmax"), GET_REAL ("Ymin"), GET_REAL ("Ymax"),
		GET_REAL ("Diameter")))
END

DIRECT (Polygon_randomize)
	EVERY (Polygon_randomize (OBJECT))
END

FORM (Polygon_salesperson, "Polygon: Find shortest path", 0)
	NATURAL ("Number of iterations", "1")
	OK
DO
	EVERY (Polygon_salesperson (OBJECT, GET_INTEGER ("Number of iterations")))
END

DIRECT (Polygon_to_Matrix)
	EVERY_TO (Polygon_to_Matrix (OBJECT))
END

/***** SOUND & PITCH & POINTPROCESS *****/

FORM (Sound_Pitch_PointProcess_voiceReport, "Voice report", "Voice")
	dia_timeRange (dia);
	POSITIVE ("left Pitch range (Hz)", "75.0")
	POSITIVE ("right Pitch range (Hz)", "600.0")
	POSITIVE ("Maximum period factor", "1.3")
	POSITIVE ("Maximum amplitude factor", "1.6")
	REAL ("Silence threshold", "0.03")
	REAL ("Voicing threshold", "0.45")
	OK
DO
	MelderInfo_open ();
	Sound_Pitch_PointProcess_voiceReport (ONLY (classSound), ONLY (classPitch), ONLY (classPointProcess),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("left Pitch range"), GET_REAL ("right Pitch range"),
		GET_REAL ("Maximum period factor"), GET_REAL ("Maximum amplitude factor"),
		GET_REAL ("Silence threshold"), GET_REAL ("Voicing threshold"));
	MelderInfo_close ();
END

/***** SOUND & POINTPROCESS & PITCHTIER & DURATIONTIER *****/

FORM (Sound_Point_Pitch_Duration_to_Sound, "To Sound", 0)
	POSITIVE ("Longest period (s)", "0.02")
	OK
DO
	if (! praat_new (Sound_Point_Pitch_Duration_to_Sound (ONLY (classSound), ONLY (classPointProcess),
		ONLY (classPitchTier), ONLY (classDurationTier), GET_REAL ("Longest period")), "manip")) return 0;
END

/***** SOUND & TEXTGRID *****/

FORM (TextGrid_Sound_draw, "TextGrid & Sound: Draw...", 0)
	dia_timeRange (dia);
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	TextGrid_Sound_draw (ONLY (classTextGrid), ONLY (classSound), GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish"));
	praat_picture_close ();
END

FORM (TextGrid_Sound_extractAllIntervals, "TextGrid & Sound: Extract all intervals", 0)
	INTEGER ("Tier", "1")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	if (! praat_new (TextGrid_Sound_extractAllIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER ("Tier"), GET_INTEGER ("Preserve times")), "dummy")) return 0;
END

FORM (TextGrid_Sound_extractNonemptyIntervals, "TextGrid & Sound: Extract non-empty intervals", 0)
	INTEGER ("Tier", "1")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	if (! praat_new (TextGrid_Sound_extractNonemptyIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER ("Tier"), GET_INTEGER ("Preserve times")), "dummy")) return 0;
END

FORM (TextGrid_Sound_extractIntervals, "TextGrid & Sound: Extract intervals", 0)
	INTEGER ("Tier", "1")
	BOOLEAN ("Preserve times", 0)
	SENTENCE ("Label text", "")
	OK
DO
	if (! praat_new (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER ("Tier"), Melder_STRING_EQUAL_TO, GET_STRING ("Label text"),
		GET_INTEGER ("Preserve times")), GET_STRING ("Label text"))) return 0;
END

FORM (TextGrid_Sound_extractIntervalsWhere, "TextGrid & Sound: Extract intervals", 0)
	INTEGER ("Tier", "1")
	BOOLEAN ("Preserve times", 0)
	OPTIONMENU ("Extract all intervals whose label...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb, Melder_STRING_max)
	SENTENCE ("...the text", "")
	OK
DO
	if (! praat_new (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER ("Tier"), GET_INTEGER ("Extract all intervals whose label..."), GET_STRING ("...the text"),
		GET_INTEGER ("Preserve times")), GET_STRING ("...the text"))) return 0;
END

DIRECT (TextGrid_Sound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	TextGrid_Function_scaleTimes (grid, ONLY (classSound));
	praat_dataChanged (grid);
END

DIRECT (TextGrid_Sound_cloneTimeDomain)
	TextGrid grid = ONLY (classTextGrid);
	Sound sound = ONLY (classSound);
	sound -> x1 += grid -> xmin - sound -> xmin;
	sound -> xmin = grid -> xmin;
	sound -> xmax = grid -> xmax;
	praat_dataChanged (sound);
END

/***** SPECTROGRAM *****/

FORM (Spectrogram_paint, "Spectrogram: Paint", "Spectrogram: Paint...")
	dia_timeRange (dia);
	REAL ("left Frequency range (Hz)", "0.0")
	REAL ("right Frequency range (Hz)", "0.0 (= all)")
	REAL ("Maximum (dB/Hz)", "100.0")
	BOOLEAN ("Autoscaling", 1)
	POSITIVE ("Dynamic range (dB)", "50.0")
	REAL ("Pre-emphasis (dB/oct)", "6.0")
	REAL ("Dynamic compression (0-1)", "0.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Spectrogram_paint (OBJECT, GRAPHICS, GET_REAL ("left Time range"),
		GET_REAL ("right Time range"), GET_REAL ("left Frequency range"), GET_REAL ("right Frequency range"),
		GET_REAL ("Maximum"), GET_INTEGER ("Autoscaling"),
		GET_REAL ("Dynamic range"), GET_REAL ("Pre-emphasis"),
		GET_REAL ("Dynamic compression"), GET_INTEGER ("Garnish")))
END

FORM (Spectrogram_formula, "Spectrogram: Formula", "Spectrogram: Formula...")
	LABEL ("label", "Do for all times and frequencies:")
	LABEL ("label", "   `x' is the time in seconds")
	LABEL ("label", "   `y' is the frequency in Hertz")
	LABEL ("label", "   `self' is the current value in Pa^2/Hz")
	LABEL ("label", "   Replace all values with:")
	TEXTFIELD ("formula", "self * exp (- x / 0.1)")
	OK
DO
	return praat_Fon_formula (dia);
END

FORM (Spectrogram_getPowerAt, "Spectrogram: Get power at (time, frequency)", 0)
	REAL ("Time (s)", "0.5")
	REAL ("Frequency (Hz)", "1000")
	OK
DO
	Spectrogram me = ONLY_OBJECT;
	double time = GET_REAL ("Time"), frequency = GET_REAL ("Frequency");
	MelderInfo_open ();
	MelderInfo_write1 (Melder_double (Matrix_getValueAtXY (me, time, frequency)));
	MelderInfo_write5 (" Pa2/Hz (at time = ", Melder_double (time), " seconds and frequency = ", Melder_double (frequency), " Hz)");
	MelderInfo_close ();
END

DIRECT (Spectrogram_help) Melder_help ("Spectrogram"); END

DIRECT (Spectrogram_movie)
	Graphics g = Movie_create ("Spectrogram movie", 300, 300);
	WHERE (SELECTED) Matrix_movie (OBJECT, g);
END

DIRECT (Spectrogram_to_Matrix)
	EVERY_TO (Spectrogram_to_Matrix (OBJECT))
END

FORM (Spectrogram_to_Sound, "Spectrogram: To Sound", 0)
	REAL ("Sampling frequency (Hz)", "22050")
	OK
DO
	EVERY_TO (Spectrogram_to_Sound (OBJECT, GET_REAL ("Sampling frequency")))
END

FORM (Spectrogram_to_Spectrum, "Spectrogram: To Spectrum (slice)", 0)
	REAL ("Time (seconds)", "0.0")
	OK
DO
	EVERY_TO (Spectrogram_to_Spectrum (OBJECT, GET_REAL ("Time")))
END

DIRECT (Spectrogram_view)
	if (praat.batch)
		return Melder_error ("Cannot view a Spectrogram from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor
				(SpectrogramEditor_create (praat.topShell, FULL_NAME, OBJECT), IOBJECT))
					return 0;
END

/***** SPECTRUM *****/

FORM (Spectrum_cepstralSmoothing, "Spectrum: Cepstral smoothing", 0)
	POSITIVE ("Bandwidth (Hz)", "500.0")
	OK
DO
	EVERY_TO (Spectrum_cepstralSmoothing (OBJECT, GET_REAL ("Bandwidth")))
END

FORM (Spectrum_draw, "Spectrum: Draw", 0)
	REAL ("left Frequency range (Hz)", "0.0")
	REAL ("right Frequency range (Hz)", "0.0 (= all)")
	REAL ("Minimum power (dB/Hz)", "0 (= auto)")
	REAL ("Maximum power (dB/Hz)", "0 (= auto)")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Spectrum_draw (OBJECT, GRAPHICS, GET_REAL ("left Frequency range"),
		GET_REAL ("right Frequency range"), GET_REAL ("Minimum power"), GET_REAL ("Maximum power"),
		GET_INTEGER ("Garnish")))
END

FORM (Spectrum_drawLogFreq, "Spectrum: Draw (log freq)", 0)
	POSITIVE ("left Frequency range (Hz)", "10.0")
	POSITIVE ("right Frequency range (Hz)", "10000.0")
	REAL ("Minimum power (dB/Hz)", "0 (= auto)")
	REAL ("Maximum power (dB/Hz)", "0 (= auto)")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Spectrum_drawLogFreq (OBJECT, GRAPHICS, GET_REAL ("left Frequency range"),
		GET_REAL ("right Frequency range"), GET_REAL ("Minimum power"), GET_REAL ("Maximum power"),
		GET_INTEGER ("Garnish")))
END

DIRECT (Spectrum_edit)
	if (praat.batch) return Melder_error ("Cannot edit a Spectrum from batch.");
	else WHERE (SELECTED)
		if (! praat_installEditor (SpectrumEditor_create (praat.topShell, FULL_NAME, OBJECT), IOBJECT)) return 0;
END

FORM (Spectrum_formula, "Spectrum: Formula", "Spectrum: Formula...")
	LABEL ("label", "`x' is the frequency in Hertz, `col' is the bin number;   "
		"`y' = `row' is 1 (real part) or 2 (imaginary part)")
	LABEL ("label", "y := 1;   row := 1;   "
		"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	LABEL ("label", "y := 2;   row := 2;   "
		"x := 0;   for col := 1 to ncol do { self [2, col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "0")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Spectrum_getBandDensity, "Spectrum: Get band density", 0)
	REAL ("Band floor (Hz)", "200.0") REAL ("Band ceiling (Hz)", "1000") OK DO
	Melder_informationReal (Spectrum_getBandDensity (ONLY_OBJECT,
		GET_REAL ("Band floor"), GET_REAL ("Band ceiling")), " Pa2 / Hz2"); END
FORM (Spectrum_getBandDensityDifference, "Spectrum: Get band density difference", 0)
	REAL ("Low band floor (Hz)", "0") REAL ("Low band ceiling (Hz)", "500")
	REAL ("High band floor (Hz)", "500") REAL ("High band ceiling (Hz)", "4000") OK DO
	Melder_informationReal (Spectrum_getBandDensityDifference (ONLY_OBJECT,
		GET_REAL ("Low band floor"), GET_REAL ("Low band ceiling"), GET_REAL ("High band floor"), GET_REAL ("High band ceiling")), "dB"); END
FORM (Spectrum_getBandEnergy, "Spectrum: Get band energy", 0)
	REAL ("Band floor (Hz)", "200.0") REAL ("Band ceiling (Hz)", "1000") OK DO
	Melder_informationReal (Spectrum_getBandEnergy (ONLY_OBJECT, GET_REAL ("Band floor"), GET_REAL ("Band ceiling")), "Pa2 sec"); END
FORM (Spectrum_getBandEnergyDifference, "Spectrum: Get band energy difference", 0)
	REAL ("Low band floor (Hz)", "0") REAL ("Low band ceiling (Hz)", "500")
	REAL ("High band floor (Hz)", "500") REAL ("High band ceiling (Hz)", "4000") OK DO
	Melder_informationReal (Spectrum_getBandEnergyDifference (ONLY_OBJECT,
		GET_REAL ("Low band floor"), GET_REAL ("Low band ceiling"), GET_REAL ("High band floor"), GET_REAL ("High band ceiling")), "dB"); END	
FORM (Spectrum_getBinFromFrequency, "Spectrum: Get bin from frequency", 0)
	REAL ("Frequency (Hz)", "2000") OK DO
	Melder_informationReal (Sampled_xToIndex (ONLY_OBJECT, GET_REAL ("Frequency")), NULL); END
DIRECT (Spectrum_getBinWidth) Spectrum me = ONLY_OBJECT; Melder_informationReal (my dx, "Hertz"); END
FORM (Spectrum_getCentralMoment, "Spectrum: Get central moment", "Spectrum: Get central moment...")
	POSITIVE ("Moment", "3.0")
	POSITIVE ("Power", "2.0") OK DO
	Melder_informationReal (Spectrum_getCentralMoment (ONLY_OBJECT,
	GET_REAL ("Moment"), GET_REAL ("Power")), "Hertz to the power 'moment'"); END
FORM (Spectrum_getCentreOfGravity, "Spectrum: Get centre of gravity", "Spectrum: Get centre of gravity...")
	POSITIVE ("Power", "2.0") OK DO
	Melder_informationReal (Spectrum_getCentreOfGravity (ONLY_OBJECT, GET_REAL ("Power")), "Hertz"); END
FORM (Spectrum_getFrequencyFromBin, "Spectrum: Get frequency from bin", 0)
	NATURAL ("Band number", "1") OK DO
	Melder_informationReal (Sampled_indexToX (ONLY_OBJECT, GET_INTEGER ("Band number")), "Hertz"); END
DIRECT (Spectrum_getHighestFrequency) Spectrum me = ONLY_OBJECT; Melder_informationReal (my xmax, "Hertz"); END
FORM (Spectrum_getImaginaryValueInBin, "Spectrum: Get imaginary value in bin", 0)
	NATURAL ("Bin number", "100") OK DO Spectrum me = ONLY_OBJECT;
	long binNumber = GET_INTEGER ("Bin number");
	REQUIRE (binNumber <= my nx, "Bin number must not exceed number of bins.");
	Melder_informationReal (my z [2] [binNumber], NULL); END
FORM (Spectrum_getKurtosis, "Spectrum: Get kurtosis", "Spectrum: Get kurtosis...")
	POSITIVE ("Power", "2.0") OK DO
	Melder_informationReal (Spectrum_getKurtosis (ONLY_OBJECT, GET_REAL ("Power")), NULL); END
DIRECT (Spectrum_getLowestFrequency) Spectrum me = ONLY_OBJECT; Melder_informationReal (my xmin, "Hertz"); END
DIRECT (Spectrum_getNumberOfBins) Spectrum me = ONLY_OBJECT; Melder_information ("%ld bins", my nx); END
FORM (Spectrum_getRealValueInBin, "Spectrum: Get real value in bin", 0)
	NATURAL ("Bin number", "100") OK DO Spectrum me = ONLY_OBJECT;
	long binNumber = GET_INTEGER ("Bin number");
	REQUIRE (binNumber <= my nx, "Bin number must not exceed number of bins.");
	Melder_informationReal (my z [1] [binNumber], NULL); END
FORM (Spectrum_getSkewness, "Spectrum: Get skewness", "Spectrum: Get skewness...")
	POSITIVE ("Power", "2.0") OK DO
	Melder_informationReal (Spectrum_getSkewness (ONLY_OBJECT, GET_REAL ("Power")), NULL); END
FORM (Spectrum_getStandardDeviation, "Spectrum: Get standard deviation", "Spectrum: Get standard deviation...")
	POSITIVE ("Power", "2.0") OK DO
	Melder_informationReal (Spectrum_getStandardDeviation (ONLY_OBJECT, GET_REAL ("Power")), "Hertz"); END

DIRECT (Spectrum_help) Melder_help ("Spectrum"); END

FORM (Spectrum_lpcSmoothing, "Spectrum: LPC smoothing", 0)
	NATURAL ("Number of peaks", "5")
	POSITIVE ("Pre-emphasis from (Hz)", "50.0")
	OK
DO
	EVERY_TO (Spectrum_lpcSmoothing (OBJECT, GET_INTEGER ("Number of peaks"), GET_REAL ("Pre-emphasis from")))
END

FORM (Spectrum_passHannBand, "Spectrum: Filter (pass Hann band)", "Spectrum: Filter (pass Hann band)...")
	REAL ("From frequency (Hz)", "500")
	REAL ("To frequency (s)", "1000")
	POSITIVE ("Smoothing (Hz)", "100")
	OK
DO
	EVERY (Spectrum_passHannBand (OBJECT, GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_REAL ("Smoothing")))
END

FORM (Spectrum_stopHannBand, "Spectrum: Filter (stop Hann band)", "Spectrum: Filter (stop Hann band)...")
	REAL ("From frequency (Hz)", "500")
	REAL ("To frequency (s)", "1000")
	POSITIVE ("Smoothing (Hz)", "100")
	OK
DO
	EVERY (Spectrum_stopHannBand (OBJECT, GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_REAL ("Smoothing")))
END

FORM (Spectrum_to_Excitation, "Spectrum: To Excitation", 0)
	POSITIVE ("Frequency resolution (Bark)", "0.1")
	OK
DO
	EVERY_TO (Spectrum_to_Excitation (OBJECT, GET_REAL ("Frequency resolution")))
END

FORM (Spectrum_to_Formant_peaks, "Spectrum: To Formant (peaks)", 0)
	LABEL ("", "Warning: this simply picks peaks from 0 Hz up!")
	NATURAL ("Maximum number of formants", "1000")
	OK
DO
	EVERY_TO (Spectrum_to_Formant (OBJECT, GET_INTEGER ("Maximum number of formants")))
END

FORM (Spectrum_to_Ltas, "Spectrum: To Long-term average spectrum", 0)
	POSITIVE ("Bandwidth (Hz)", "1000")
	OK
DO
	EVERY_TO (Spectrum_to_Ltas (OBJECT, GET_REAL ("Bandwidth")))
END

DIRECT (Spectrum_to_Ltas_1to1)
	EVERY_TO (Spectrum_to_Ltas_1to1 (OBJECT))
END

DIRECT (Spectrum_to_Matrix)
	EVERY_TO (Spectrum_to_Matrix (OBJECT))
END

DIRECT (Spectrum_to_Sound)
	EVERY_TO (Spectrum_to_Sound (OBJECT))
END

DIRECT (Spectrum_to_Spectrogram)
	EVERY_TO (Spectrum_to_Spectrogram (OBJECT))
END

/***** SPELLINGCHECKER *****/

FORM (SpellingChecker_addNewWord, "Add word to user dictionary", "SpellingChecker")
	SENTENCE ("New word", "")
	OK
DO
	WHERE (SELECTED) {
		if (! SpellingChecker_addNewWord (OBJECT, GET_STRING ("New word"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (SpellingChecker_edit, "Edit spelling checker", "SpellingChecker")
	LABEL ("", "-- Syntax --")
	SENTENCE ("Forbidden strings", "")
	BOOLEAN ("Check matching parentheses", 0)
	SENTENCE ("Separating characters", "")
	BOOLEAN ("Allow all parenthesized", 0)
	LABEL ("", "-- Capitals --")
	BOOLEAN ("Allow all names", 0)
	SENTENCE ("Name prefixes", "")
	BOOLEAN ("Allow all abbreviations", 0)
	LABEL ("", "-- Capitalization --")
	BOOLEAN ("Allow caps sentence-initially", 0)
	BOOLEAN ("Allow caps after colon", 0)
	LABEL ("", "-- Word parts --")
	SENTENCE ("Allow all words containing", "")
	SENTENCE ("Allow all words starting with", "")
	SENTENCE ("Allow all words ending in", "")
	OK
SpellingChecker me = ONLY_OBJECT;
SET_STRING ("Forbidden strings", my forbiddenStrings)
SET_INTEGER ("Check matching parentheses", my checkMatchingParentheses)
SET_STRING ("Separating characters", my separatingCharacters)
SET_INTEGER ("Allow all parenthesized", my allowAllParenthesized)
SET_INTEGER ("Allow all names", my allowAllNames)
SET_STRING ("Name prefixes", my namePrefixes)
SET_INTEGER ("Allow all abbreviations", my allowAllAbbreviations)
SET_INTEGER ("Allow caps sentence-initially", my allowCapsSentenceInitially)
SET_INTEGER ("Allow caps after colon", my allowCapsAfterColon)
SET_STRING ("Allow all words containing", my allowAllWordsContaining)
SET_STRING ("Allow all words starting with", my allowAllWordsStartingWith)
SET_STRING ("Allow all words ending in", my allowAllWordsEndingIn)
DO
	SpellingChecker me = ONLY_OBJECT;
	Melder_free (my forbiddenStrings); my forbiddenStrings = Melder_strdup (GET_STRING ("Forbidden strings"));
	my checkMatchingParentheses = GET_INTEGER ("Check matching parentheses");
	Melder_free (my separatingCharacters); my separatingCharacters = Melder_strdup (GET_STRING ("Separating characters"));
	my allowAllParenthesized = GET_INTEGER ("Allow all parenthesized");
	my allowAllNames = GET_INTEGER ("Allow all names");
	Melder_free (my namePrefixes); my namePrefixes = Melder_strdup (GET_STRING ("Name prefixes"));
	my allowAllAbbreviations = GET_INTEGER ("Allow all abbreviations");
	my allowCapsSentenceInitially = GET_INTEGER ("Allow caps sentence-initially");
	my allowCapsAfterColon = GET_INTEGER ("Allow caps after colon");
	Melder_free (my allowAllWordsContaining); my allowAllWordsContaining = Melder_strdup (GET_STRING ("Allow all words containing"));
	Melder_free (my allowAllWordsStartingWith); my allowAllWordsStartingWith = Melder_strdup (GET_STRING ("Allow all words starting with"));
	Melder_free (my allowAllWordsEndingIn); my allowAllWordsEndingIn = Melder_strdup (GET_STRING ("Allow all words ending in"));
	praat_dataChanged (me);
END

DIRECT (SpellingChecker_extractWordList)
	EVERY_TO (SpellingChecker_extractWordList (OBJECT))
END

DIRECT (SpellingChecker_extractUserDictionary)
	EVERY_TO (SpellingChecker_extractUserDictionary (OBJECT))
END

FORM (SpellingChecker_isWordAllowed, "Is word allowed?", "SpellingChecker")
	SENTENCE ("Word", "")
	OK
DO
	Melder_information (SpellingChecker_isWordAllowed (ONLY_OBJECT, GET_STRING ("Word")) ?
		"1 (allowed)" : "0 (not allowed)");
END


FORM (SpellingChecker_nextNotAllowedWord, "Next not allowed word?", "SpellingChecker")
	LABEL ("", "Sentence:")
	TEXTFIELD ("sentence", "")
	INTEGER ("Starting character", "0")
	OK
DO
	char *sentence = GET_STRING ("sentence"), *result;
	int startingCharacter = GET_INTEGER ("Starting character");
	REQUIRE (startingCharacter >= 0, "Starting character should be 0 or positive.")
	REQUIRE (startingCharacter <= strlen (sentence), "Starting character should not exceed end of sentence.")
	result = SpellingChecker_nextNotAllowedWord (ONLY_OBJECT, sentence, & startingCharacter);
	Melder_information ("%s", result ? result : "");
END

DIRECT (SpellingChecker_replaceWordList)
	if (! SpellingChecker_replaceWordList (ONLY (classSpellingChecker), ONLY (classWordList))) return 0;
END

DIRECT (SpellingChecker_replaceWordList_help)
	Melder_information ("To replace the checker's word list\nby the contents of a Strings object:\n"
		"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
		"4. choose Replace.");
END

DIRECT (SpellingChecker_replaceUserDictionary)
	if (! SpellingChecker_replaceUserDictionary (ONLY (classSpellingChecker), ONLY (classSortedSetOfString))) return 0;
END

/***** STRINGS *****/

FORM (Strings_createAsFileList, "Create Strings as file list", "Create Strings as file list...")
	SENTENCE ("Name", "fileList")
	LABEL ("", "Path:")
	TEXTFIELD ("path", "/people/Miep/*.aifc")
	OK
static int inited;
if (! inited) {
	structMelderDir defaultDir;
	char *workingDirectory, path [300];
	Melder_getDefaultDir (& defaultDir);
	workingDirectory = Melder_dirToPath (& defaultDir);
	#if defined (UNIX)
		sprintf (path, "%s/*.aifc", workingDirectory);
	#elif defined (_WIN32)
	{
		int len = strlen (workingDirectory);
		sprintf (path, "%s%s*.wav", workingDirectory, len == 0 || workingDirectory [len - 1] != '\\' ? "\\" : "");
	}
	#else
		sprintf (path, "%s*.aifc", workingDirectory);
	#endif
	SET_STRING ("path", path);
	inited = TRUE;
}
DO
	if (! praat_new (Strings_createAsFileList (GET_STRING ("path")), GET_STRING ("Name"))) return 0;
END

DIRECT (Strings_equal)
	Strings s1 = NULL, s2 = NULL;
	WHERE (SELECTED) if (s1) s2 = OBJECT; else s1 = OBJECT;
	Melder_information ("%d", Data_equal (s1, s2));
END

DIRECT (Strings_genericize)
	WHERE (SELECTED) {
		int status = Strings_genericize (OBJECT);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

DIRECT (Strings_getNumberOfStrings)
	Strings me = ONLY_OBJECT;
	Melder_information ("%ld", my numberOfStrings);
END

FORM (Strings_getString, "Get string", 0)
	NATURAL ("Index", "1")
	OK
DO
	Strings me = ONLY_OBJECT;
	long index = GET_INTEGER ("Index");
	Melder_information ("%s", index > my numberOfStrings ? "" : my strings [index]);
END

DIRECT (Strings_help) Melder_help ("Strings"); END

DIRECT (Strings_randomize)
	WHERE (SELECTED) {
		Strings_randomize (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM_READ (Strings_readFromRawTextFile, "Read Strings from raw text file", 0)
	if (! praat_new (Strings_readFromRawTextFile (file), MelderFile_name (file))) return 0;
END

DIRECT (Strings_sort)
	WHERE (SELECTED) {
		Strings_sort (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Strings_to_Distributions)
	EVERY_TO (Strings_to_Distributions (OBJECT))
END

DIRECT (Strings_to_WordList)
	EVERY_TO (Strings_to_WordList (OBJECT))
END

FORM_WRITE (Strings_writeToRawTextFile, "Write Strings to text file", 0, "txt")
	if (! Strings_writeToRawTextFile (ONLY_OBJECT, file)) return 0;
END

/***** TABLE, rest in praat_Stat.c *****/

FORM (Table_create, "Create Table", 0)
	WORD ("Name", "table")
	NATURAL ("Number of rows", "10")
	NATURAL ("Number of columns", "3")
	OK
DO
	if (! praat_new (Table_create (GET_INTEGER ("Number of rows"), GET_INTEGER ("Number of columns")),
		GET_STRING ("Name"))) return 0;
END

FORM_READ (Table_readFromTableFile, "Read Table from table file", 0)
	if (! praat_new (Table_readFromTableFile (file), MelderFile_name (file))) return 0;
END

/***** TABLEOFREAL *****/

DIRECT (TablesOfReal_append)
	Collection me = Collection_create (classTableOfReal, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new (TablesOfReal_appendMany (me), "appended")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (TableOfReal_create, "Create TableOfReal", 0)
	WORD ("Name", "table")
	NATURAL ("Number of rows", "10")
	NATURAL ("Number of columns", "3")
	OK
DO
	if (! praat_new (TableOfReal_create (GET_INTEGER ("Number of rows"), GET_INTEGER ("Number of columns")),
		GET_STRING ("Name"))) return 0;
END

FORM (TableOfReal_drawAsNumbers, "Draw as numbers", 0)
	NATURAL ("From row", "1")
	INTEGER ("To row", "0 (= all)")
	RADIO ("Format", 3)
	RADIOBUTTON ("decimal")
	RADIOBUTTON ("exponential")
	RADIOBUTTON ("free")
	RADIOBUTTON ("rational")
	NATURAL ("Precision", "5")
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsNumbers (OBJECT, GRAPHICS,
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("Format"), GET_INTEGER ("Precision")))
END

FORM (TableOfReal_drawAsNumbers_if, "Draw as numbers if...", 0)
	NATURAL ("From row", "1")
	INTEGER ("To row", "0 (= all)")
	RADIO ("Format", 3)
	RADIOBUTTON ("decimal")
	RADIOBUTTON ("exponential")
	RADIOBUTTON ("free")
	RADIOBUTTON ("rational")
	NATURAL ("Precision", "5")
	LABEL ("", "Condition:")
	TEXTFIELD ("condition", "self <> 0")
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsNumbers_if (OBJECT, GRAPHICS,
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("Format"), GET_INTEGER ("Precision"), GET_STRING ("condition")))
END

FORM (TableOfReal_drawAsSquares, "Draw table as squares", 0)
	INTEGER ("From row", "1")
	INTEGER ("To row", "0")
	INTEGER ("From column", "1")
	INTEGER ("To column", "0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsSquares (OBJECT, GRAPHICS, 
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("From column"), GET_INTEGER ("To column"),
		GET_INTEGER ("Garnish")))
END

FORM (TableOfReal_drawHorizontalLines, "Draw horizontal lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawHorizontalLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawLeftAndRightLines, "Draw left and right lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawLeftAndRightLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawTopAndBottomLines, "Draw top and bottom lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawTopAndBottomLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawVerticalLines, "Draw vertical lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawVerticalLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END

DIRECT (TableOfReal_extractColumnLabelsAsStrings)
	EVERY_TO (TableOfReal_extractColumnLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractColumnRanges, "Extract column ranges", 0)
	LABEL ("", "Create a new TableOfReal from the following columns:")
	TEXTFIELD ("ranges", "1 2")
	LABEL ("", "To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnRanges (OBJECT, GET_STRING ("ranges")), "%s_cols", NAME)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhere, "Extract columns where", 0)
	LABEL ("", "Extract all columns with at least one cell where:")
	TEXTFIELD ("condition", "col mod 3 = 0 ; this example extracts every third column")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhere (OBJECT, GET_STRING ("condition")), "%s_cols", NAME)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhereLabel, "Extract column where label", 0)
	OPTIONMENU ("Extract all columns whose label...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb, Melder_STRING_max)
	SENTENCE ("...the text", "a")
	OK
DO
	const char *text = GET_STRING ("...the text");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhereLabel (OBJECT, GET_INTEGER ("Extract all columns whose label..."), text),
			"%s_%s", NAME, text)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhereRow, "Extract columns where row", 0)
	NATURAL ("Extract all columns where row...", "1")
	OPTIONMENU ("...is...", 1)
	OPTIONS_ENUM (Melder_NUMBER_text_adjective, Melder_NUMBER_max)
	REAL ("...the value", "0.0")
	OK
DO
	long row = GET_INTEGER ("Extract all columns where row...");
	double value = GET_REAL ("...the value");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhereRow (OBJECT,
			row, GET_INTEGER ("...is..."), value),
			"%s_%ld_%ld", NAME, row, (long) floor (value+0.5))) return 0;
	}
END

DIRECT (TableOfReal_extractRowLabelsAsStrings)
	EVERY_TO (TableOfReal_extractRowLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractRowRanges, "Extract row ranges", 0)
	LABEL ("", "Create a new TableOfReal from the following rows:")
	TEXTFIELD ("ranges", "1 2")
	LABEL ("", "To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowRanges (OBJECT, GET_STRING ("ranges")), "%s_rows", NAME)) return 0;
	}
END

FORM (TableOfReal_extractRowsWhere, "Extract rows where", 0)
	LABEL ("", "Extract all rows with at least one cell where:")
	TEXTFIELD ("condition", "row mod 3 = 0 ; this example extracts every third row")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhere (OBJECT, GET_STRING ("condition")), "%s_rows", NAME)) return 0;
	}
END

FORM (TableOfReal_extractRowsWhereColumn, "Extract rows where column", 0)
	NATURAL ("Extract all rows where column...", "1")
	OPTIONMENU ("...is...", 1)
	OPTIONS_ENUM (Melder_NUMBER_text_adjective, Melder_NUMBER_max)
	REAL ("...the value", "0.0")
	OK
DO
	long column = GET_INTEGER ("Extract all rows where column...");
	double value = GET_REAL ("...the value");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhereColumn (OBJECT,
			column, GET_INTEGER ("...is..."), value),
			"%s_%ld_%ld", NAME, column, (long) floor (value+0.5))) return 0;
	}
END

FORM (TableOfReal_extractRowsWhereLabel, "Extract rows where label", 0)
	OPTIONMENU ("Extract all rows whose label...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb, Melder_STRING_max)
	SENTENCE ("...the text", "a")
	OK
DO
	const char *text = GET_STRING ("...the text");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhereLabel (OBJECT, GET_INTEGER ("Extract all rows whose label..."), text),
			"%s_%s", NAME, text)) return 0;
	}
END

FORM (TableOfReal_formula, "TableOfReal: Formula", "Formula...")
	LABEL ("", "for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	TEXTFIELD ("formula", "if col = 5 then self + self [6] else self fi")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_formula (OBJECT, GET_STRING ("formula"), NULL)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_getColumnIndex, "Get column index", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Melder_information ("%ld", TableOfReal_columnLabelToIndex (ONLY_OBJECT, GET_STRING ("Column label")));
END
	
FORM (TableOfReal_getColumnLabel, "Get column label", 0)
	NATURAL ("Column number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER ("Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information ("%s", ! table -> columnLabels || ! table -> columnLabels [columnNumber] ? "" :
		table -> columnLabels [columnNumber]);
END
	
FORM (TableOfReal_getColumnMean_index, "Get column mean", 0)
	NATURAL ("Column number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER ("Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnMean_label, "Get column mean", 0)
	SENTENCE ("Column label", "")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRING ("Column label"));
	REQUIRE (columnNumber > 0, "Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnStdev_index, "Get column standard deviation", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Melder_informationReal (TableOfReal_getColumnStdev (ONLY_OBJECT, GET_INTEGER ("Column number")), NULL);
END
	
FORM (TableOfReal_getColumnStdev_label, "Get column standard deviation", 0)
	SENTENCE ("Column label", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRING ("Column label"));
	REQUIRE (columnNumber > 0, "Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnStdev (table, columnNumber), NULL);
END

DIRECT (TableOfReal_getNumberOfColumns) TableOfReal me = ONLY_OBJECT; Melder_information ("%ld", my numberOfColumns); END
DIRECT (TableOfReal_getNumberOfRows) TableOfReal me = ONLY_OBJECT; Melder_information ("%ld", my numberOfRows); END

FORM (TableOfReal_getRowIndex, "Get row index", 0)
	SENTENCE ("Row label", "")
	OK
DO
	Melder_information ("%ld", TableOfReal_rowLabelToIndex (ONLY_OBJECT, GET_STRING ("Row label")));
END
	
FORM (TableOfReal_getRowLabel, "Get row label", 0)
	NATURAL ("Row number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long rowNumber = GET_INTEGER ("Row number");
	REQUIRE (rowNumber <= table -> numberOfRows, "Row number must not be greater than number of rows.")
	Melder_information ("%s", ! table -> rowLabels || ! table -> rowLabels [rowNumber] ? "" :
		table -> rowLabels [rowNumber]);
END

FORM (TableOfReal_getValue, "Get value", 0)
	NATURAL ("Row number", "1") NATURAL ("Column number", "1") OK DO TableOfReal me = ONLY_OBJECT;
	long row = GET_INTEGER ("Row number"), column = GET_INTEGER ("Column number");
	REQUIRE (row <= my numberOfRows, "Row number must not exceed number of rows.")
	REQUIRE (column <= my numberOfColumns, "Column number must not exceed number of columns.")
	Melder_informationReal (my data [row] [column], NULL); END

DIRECT (TableOfReal_help) Melder_help ("TableOfReal"); END

FORM (TableOfReal_insertColumn, "Insert column", 0)
	NATURAL ("Column number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertColumn (OBJECT, GET_INTEGER ("Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_insertRow, "Insert row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertRow (OBJECT, GET_INTEGER ("Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM_READ (TableOfReal_readFromHeaderlessSpreadsheetFile, "Read TableOfReal from headerless spreadsheet file", 0)
	if (! praat_new (TableOfReal_readFromHeaderlessSpreadsheetFile (file), MelderFile_name (file))) return 0;
END

FORM (TableOfReal_removeColumn, "Remove column", 0)
	NATURAL ("Column number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeColumn (OBJECT, GET_INTEGER ("Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_removeRow, "Remove row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeRow (OBJECT, GET_INTEGER ("Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_index, "Set column label", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, GET_INTEGER ("Column number"), GET_STRING ("Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_label, "Set column label", 0)
	SENTENCE ("Old label", "")
	SENTENCE ("New label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, TableOfReal_columnLabelToIndex (OBJECT, GET_STRING ("Old label")),
			GET_STRING ("New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setRowLabel_index, "Set row label", 0)
	NATURAL ("Row number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, GET_INTEGER ("Row number"), GET_STRING ("Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setValue, "Set value", "TableOfReal: Set value...")
	NATURAL ("Row number", "1")
	NATURAL ("Column number", "1")
	REAL ("New value", "0.0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal me = OBJECT;
		long irow = GET_INTEGER ("Row number"), icol = GET_INTEGER ("Column number");
		REQUIRE (irow <= my numberOfRows, "Row number too large.")
		REQUIRE (icol <= my numberOfColumns, "Column number too large.")
		my data [irow] [icol] = GET_REAL ("New value");
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setRowLabel_label, "Set row label", 0)
	SENTENCE ("Old label", "")
	SENTENCE ("New label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, TableOfReal_rowLabelToIndex (OBJECT, GET_STRING ("Old label")),
			GET_STRING ("New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByColumn, "Sort rows by column", 0)
	INTEGER ("Column", "1")
	INTEGER ("Secondary column", "0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByColumn (OBJECT, GET_INTEGER ("Column"), GET_INTEGER ("Secondary column"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByLabel, "Sort rows by label", 0)
	LABEL ("", "Secondary sorting keys:")
	INTEGER ("Column1", "1")
	INTEGER ("Column2", "0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByLabel (OBJECT, GET_INTEGER ("Column1"), GET_INTEGER ("Column2"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TableOfReal_to_Matrix)
	EVERY_TO (TableOfReal_to_Matrix (OBJECT))
END

FORM_WRITE (TableOfReal_writeToHeaderlessSpreadsheetFile, "Write TableOfReal to spreadsheet", 0, "txt")
	if (! TableOfReal_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

/***** TEXTGRID *****/

FORM (TextGrid_countLabels, "Count labels", "TextGrid: Count labels...")
	INTEGER ("Tier number", "1")
	SENTENCE ("Label text", "a")
	OK
DO
	Melder_information ("%ld", TextGrid_countLabels (ONLY (classTextGrid),
		GET_INTEGER ("Tier number"), GET_STRING ("Label text")));
END

FORM (TextGrid_create, "Create TextGrid", "Create TextGrid...")
	LABEL ("", "Hint: to label or segment an existing Sound,")
	LABEL ("", "select that Sound and choose \"To TextGrid...\".")
	REAL ("Starting time (s)", "0.0")
	REAL ("Finishing time (s)", "1.0")
	SENTENCE ("All tier names", "Mary John bell")
	SENTENCE ("Which of these are point tiers?", "bell")
	OK
DO
	double tmin = GET_REAL ("Starting time"), tmax = GET_REAL ("Finishing time");
	REQUIRE (tmax > tmin, "Finishing time should be greater than starting time")
	if (! praat_new (TextGrid_create (tmin, tmax, GET_STRING ("All tier names"), GET_STRING ("Which of these are point tiers?")),
		GET_STRING ("All tier names"))) return 0;
END

FORM (TextGrid_draw, "TextGrid: Draw", 0)
	dia_timeRange (dia);
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TextGrid_Sound_draw (OBJECT, NULL, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish")))
END

FORM (TextGrid_duplicateTier, "TextGrid: Duplicate tier", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER ("Tier number");
		int position = GET_INTEGER ("Position");
		const char *name = GET_STRING ("Name");
		AnyTier newTier;
		if (itier > grid -> tiers -> size) itier = grid -> tiers -> size;
		newTier = Data_copy (grid -> tiers -> item [itier]);
		if (! newTier) return 0;
		Thing_setName (newTier, name);
		if (! Ordered_addItemPos (grid -> tiers, newTier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

static void cb_TextGridEditor_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
	if (Thing_member (publish, classSpectrum) && strequ (Thing_getName (publish), "slice")) {
		int IOBJECT;
		WHERE (SELECTED) {
			SpectrumEditor editor2 = SpectrumEditor_create (praat.topShell, FULL_NAME, OBJECT);
			if (! editor2) return;
			if (! praat_installEditor (editor2, IOBJECT)) Melder_flushError (NULL);
		}
	}
}
DIRECT (TextGrid_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		WHERE (SELECTED && CLASS == classTextGrid) {
			TextGridEditor editor = TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, ONLY (classSound), NULL);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_TextGridEditor_publish, NULL);
		}
	}
END

DIRECT (TextGrid_LongSound_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		LongSound longSound = NULL;
		int ilongSound = 0;
		WHERE (SELECTED)
			if (CLASS == classLongSound) longSound = OBJECT, ilongSound = IOBJECT;
		Melder_assert (ilongSound != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, longSound, NULL), IOBJECT, ilongSound)) return 0;
	}
END

DIRECT (TextGrid_SpellingChecker_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		SpellingChecker spellingChecker = NULL;
		int ispellingChecker = 0;
		WHERE (SELECTED)
			if (CLASS == classSpellingChecker) spellingChecker = OBJECT, ispellingChecker = IOBJECT;
		Melder_assert (ispellingChecker != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, ONLY (classSound), spellingChecker), IOBJECT, ispellingChecker)) return 0;
	}
END

DIRECT (TextGrid_LongSound_SpellingChecker_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		LongSound longSound = NULL;
		SpellingChecker spellingChecker = NULL;
		int ilongSound = 0, ispellingChecker = 0;
		WHERE (SELECTED) {
			if (CLASS == classLongSound) longSound = OBJECT, ilongSound = IOBJECT;
			if (CLASS == classSpellingChecker) spellingChecker = OBJECT, ispellingChecker = IOBJECT;
		}
		Melder_assert (ilongSound != 0 && ispellingChecker != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor3 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, longSound, spellingChecker), IOBJECT, ilongSound, ispellingChecker)) return 0;
	}
END

FORM (TextGrid_extractPart, "TextGrid: Extract part", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "1.0")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	TextGrid grid = ONLY (classTextGrid);
	if (! praat_new (TextGrid_extractPart (grid, GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Preserve times")), "%s_part", grid -> name)) return 0;
END

static Data pr_TextGrid_getTier (Any dia) {
	TextGrid grid = ONLY_OBJECT;
	long tierNumber = GET_INTEGER ("Tier number");
	if (tierNumber > grid -> tiers -> size) return Melder_errorp
		("Tier number (%ld) should not be larger than number of tiers (%ld).", tierNumber, grid -> tiers -> size);
	return grid -> tiers -> item [tierNumber];
}

static IntervalTier pr_TextGrid_getIntervalTier (Any dia) {
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return NULL;
	if (tier -> methods != (Data_Table) classIntervalTier) return Melder_errorp ("Tier should be interval tier.");
	return (IntervalTier) tier;
}

static TextTier pr_TextGrid_getTextTier (Any dia) {
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return NULL;
	if (tier -> methods != (Data_Table) classTextTier) return Melder_errorp ("Tier should be point tier (TextTier).");
	return (TextTier) tier;
}

static TextInterval pr_TextGrid_getInterval (Any dia) {
	int intervalNumber = GET_INTEGER ("Interval number");
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return NULL;
	if (intervalNumber > intervalTier -> intervals -> size) return Melder_errorp ("Interval number too large.");
	return intervalTier -> intervals -> item [intervalNumber];
}

static TextPoint pr_TextGrid_getPoint (Any dia) {	
	long pointNumber = GET_INTEGER ("Point number");
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return NULL;
	if (pointNumber > textTier -> points -> size) return Melder_errorp ("Point number too large.");
	return textTier -> points -> item [pointNumber];
}

FORM (TextGrid_extractTier, "TextGrid: Extract tier", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	if (! praat_new (Data_copy (tier), "%s", tier -> name)) return 0;
END

DIRECT (TextGrid_genericize)
	WHERE (SELECTED) {
		if (! TextGrid_genericize (OBJECT)) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextGrid_nativize)
	WHERE (SELECTED) {
		if (! TextGrid_nativize (OBJECT)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_getIntervalAtTime, "TextGrid: Get interval at time", 0)
	NATURAL ("Tier number", "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information ("%ld", IntervalTier_timeToIndex (intervalTier, GET_REAL ("Time")));
END

FORM (TextGrid_getNumberOfIntervals, "TextGrid: Get number of intervals", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information ("%ld", intervalTier -> intervals -> size);
END

DIRECT (TextGrid_getNumberOfTiers)
	TextGrid grid = ONLY_OBJECT;
	Melder_information ("%ld", grid -> tiers -> size);
END

FORM (TextGrid_getStartingPoint, "TextGrid: Get starting point", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Interval number", "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmin, "seconds");
END
	
FORM (TextGrid_getEndPoint, "TextGrid: Get end point", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Interval number", "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmax, "seconds");
END
	
FORM (TextGrid_getLabelOfInterval, "TextGrid: Get label of interval", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Interval number", "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_information (interval -> text ? "%s" : "", interval -> text);
END
	
FORM (TextGrid_getNumberOfPoints, "TextGrid: Get number of points", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information ("%ld", textTier -> points -> size);
END
	
FORM (TextGrid_getTierName, "TextGrid: Get tier name", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	Melder_information ("%s", tier -> name);
END

FORM (TextGrid_getTimeOfPoint, "TextGrid: Get time of point", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Point number", "1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_informationReal (point -> time, "seconds");
END
	
FORM (TextGrid_getLabelOfPoint, "TextGrid: Get label of point", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Point number", "1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_information (point -> mark ? "%s" : "", point -> mark);
END
	
DIRECT (TextGrid_help) Melder_help ("TextGrid"); END

FORM (TextGrid_insertBoundary, "TextGrid: Insert boundary", 0)
	NATURAL ("Tier number", "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertBoundary (OBJECT, GET_INTEGER ("Tier number"), GET_REAL ("Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertIntervalTier, "TextGrid: Insert interval tier", 0)
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER ("Position");
		char *name = GET_STRING ("Name");
		IntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPoint, "TextGrid: Insert point", 0)
	NATURAL ("Tier number", "1")
	REAL ("Time (s)", "0.5")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertPoint (OBJECT, GET_INTEGER ("Tier number"), GET_REAL ("Time"), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPointTier, "TextGrid: Insert point tier", 0)
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER ("Position");
		char *name = GET_STRING ("Name");
		TextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_isIntervalTier, "TextGrid: Is interval tier?", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	Melder_information (tier -> methods == (Data_Table) classIntervalTier ?
		"1 (yes, tier %ld is an interval tier)" : "0 (no, tier %ld is a point tier)",
		GET_INTEGER ("Tier number"));
END

DIRECT (TextGrids_merge)
	Collection textGrids;
	int n = 0;
	WHERE (SELECTED) n ++;
	textGrids = Collection_create (classTextGrid, n);
	WHERE (SELECTED) Collection_addItem (textGrids, OBJECT);
	if (! praat_new (TextGrid_merge (textGrids), "merged")) {
		textGrids -> size = 0;   /* Undangle. */
		forget (textGrids);
		return 0;
	}
	textGrids -> size = 0;   /* Undangle. */
	forget (textGrids);
END

DIRECT (info_TextGrid_Pitch_draw)
	Melder_information ("You can draw a TextGrid together with a Pitch after selecting them both.");
END

FORM (TextGrid_removeBoundaryAtTime, "TextGrid: Remove boundary at time", 0)
	NATURAL ("Tier number", "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_removeBoundaryAtTime (OBJECT, GET_INTEGER ("Tier number"), GET_REAL ("Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_removeTier, "TextGrid: Remove tier", 0)
	NATURAL ("Tier number", "1")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER ("Tier number");
		if (grid -> tiers -> size <= 1) {
			return Melder_error ("Sorry, I refuse to remove the last tier.");
		}
		if (itier > grid -> tiers -> size) itier = grid -> tiers -> size;
		Collection_removeItem (grid -> tiers, itier);
		praat_dataChanged (grid);
	}
END

DIRECT (info_TextGrid_Sound_edit)
	Melder_information ("To include a copy of a Sound in your TextGrid editor:\n"
		"   select a TextGrid and a Sound, and click \"Edit\".");
END

DIRECT (info_TextGrid_Sound_draw)
	Melder_information ("You can draw a TextGrid together with a Sound after selecting them both.");
END

FORM (TextGrid_scaleTimes, "TextGrid: Scale times", 0)
	REAL ("left New time domain (s)", "0.0")
	REAL ("right New time domain (s)", "1.0")
	OK
DO
	double tmin = GET_REAL ("left New time domain"), tmax = GET_REAL ("right New time domain");
	if (tmin >= tmax) return Melder_error ("Duration of time domain must be positive.");
	WHERE (SELECTED) {
		TextGrid_scaleTimes (OBJECT, tmin, tmax);
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setIntervalText, "TextGrid: Set interval text", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Interval", "1")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setIntervalText (OBJECT, GET_INTEGER ("Tier number"), GET_INTEGER ("Interval"), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setPointText, "TextGrid: Set point text", 0)
	NATURAL ("Tier number", "1")
	NATURAL ("Point", "1")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setPointText (OBJECT, GET_INTEGER ("Tier number"), GET_INTEGER ("Point"), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_shiftTimes, "TextGrid: Shift times", 0)
	REAL ("Time shift (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		TextGrid_shiftTimes (OBJECT, GET_REAL ("Time shift"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextGrid_shiftToZero)
	WHERE (SELECTED) {
		TextGrid_shiftToZero (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM_WRITE (TextGrid_writeToChronologicalTextFile, "Text file", 0, 0)
	if (! TextGrid_writeToChronologicalTextFile (ONLY_OBJECT, file)) return 0;
END

/***** TEXTGRID & ANYTIER *****/

DIRECT (TextGrid_AnyTier_append)
	TextGrid oldGrid = ONLY (classTextGrid), newGrid = Data_copy (oldGrid);
	if (! newGrid) return 0;
	WHERE (SELECTED && OBJECT != oldGrid) if (! TextGrid_add (newGrid, OBJECT)) { forget (newGrid); return 0; }
	if (! praat_new (newGrid, oldGrid -> name)) return 0;
END

/***** TEXTGRID & LONGSOUND *****/

DIRECT (TextGrid_LongSound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	TextGrid_Function_scaleTimes (grid, ONLY (classLongSound));
	praat_dataChanged (grid);
END

/***** TEXTTIER *****/

FORM (TextTier_addPoint, "TextTier: Add point", "TextTier: Add point...")
	REAL ("Time (s)", "0.5")
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextTier_addPoint (OBJECT, GET_REAL ("Time"), GET_STRING ("Text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

FORM (TextTier_downto_TableOfReal, "TextTier: Down to TableOfReal", 0)
	SENTENCE ("Label", "")
	OK
DO
	EVERY_TO (TextTier_downto_TableOfReal (OBJECT, GET_STRING ("Label")))
END

DIRECT (TextTier_downto_TableOfReal_any)
	EVERY_TO (TextTier_downto_TableOfReal_any (OBJECT))
END

FORM (TextTier_getPoints, "Get points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (TextTier_getPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

DIRECT (TextTier_help) Melder_help ("TextTier"); END

FORM_READ (TextTier_readFromXwaves, "Read TextTier from Xwaves", 0)
	if (! praat_new (TextTier_readFromXwaves (file), MelderFile_name (file))) return 0;
END

/***** TIMEFRAMESAMPLED *****/

DIRECT (TimeFrameSampled_getNumberOfFrames)
	Sampled me = ONLY_OBJECT;
	Melder_information ("%ld frames", my nx);
END

FORM (TimeFrameSampled_getFrameFromTime, "Get frame number from time", "Get frame number from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY_OBJECT, GET_REAL ("Time")), NULL);
END

DIRECT (TimeFrameSampled_getFrameLength)
	Sampled me = ONLY_OBJECT;
	Melder_informationReal (my dx, "seconds");
END

FORM (TimeFrameSampled_getTimeFromFrame, "Get time from frame number", "Get time from frame number...")
	NATURAL ("Frame number", "1")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY_OBJECT, GET_INTEGER ("Frame number")), "seconds");
END

/***** TIMEFUNCTION *****/

DIRECT (TimeFunction_getDuration)
	Function me = ONLY_OBJECT;
	Melder_informationReal (my xmax - my xmin, "seconds");
END

DIRECT (TimeFunction_getFinishingTime)
	Function me = ONLY_OBJECT;
	Melder_informationReal (my xmax, "seconds");
END

DIRECT (TimeFunction_getStartingTime)
	Function me = ONLY_OBJECT;
	Melder_informationReal (my xmin, "seconds");
END

/***** TIMETIER *****/

FORM (TimeTier_getHighIndexFromTime, "Get high index", "AnyTier: Get high index from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	AnyTier me = ONLY_OBJECT;
	Melder_information (my points -> size ? "%ld" : "--undefined--", AnyTier_timeToHighIndex (me, GET_REAL ("Time")));
END

FORM (TimeTier_getLowIndexFromTime, "Get low index", "AnyTier: Get low index from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	AnyTier me = ONLY_OBJECT;
	Melder_information (my points -> size ? "%ld" : "--undefined--", AnyTier_timeToLowIndex (me, GET_REAL ("Time")));
END

FORM (TimeTier_getNearestIndexFromTime, "Get nearest index", "AnyTier: Get nearest index from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	AnyTier me = ONLY_OBJECT;
	Melder_information (my points -> size ? "%ld" : "--undefined--", AnyTier_timeToNearestIndex (me, GET_REAL ("Time")));
END

DIRECT (TimeTier_getNumberOfPoints)
	AnyTier me = ONLY_OBJECT;
	Melder_information ("%ld points", my points -> size);
END

FORM (TimeTier_getTimeFromIndex, "Get time", 0 /*"AnyTier: Get time from index..."*/)
	NATURAL ("Point number", "10")
	OK
DO
	AnyTier me = ONLY_OBJECT;
	long i = GET_INTEGER ("Point number");
	if (i > my points -> size) Melder_information ("--undefined--");
	else Melder_informationReal (((AnyPoint) my points -> item [i]) -> time, "seconds");
END

FORM (TimeTier_removePoint, "Remove one point", "AnyTier: Remove point...")
	NATURAL ("Point number", "1")
	OK
DO
	WHERE (SELECTED) {
		AnyTier_removePoint (OBJECT, GET_INTEGER ("Point number"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TimeTier_removePointNear, "Remove one point", "AnyTier: Remove point near...")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		AnyTier_removePointNear (OBJECT, GET_REAL ("Time"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TimeTier_removePointsBetween, "Remove points", "AnyTier: Remove points between...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "1.0")
	OK
DO
	WHERE (SELECTED) {
		AnyTier_removePointsBetween (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"));
		praat_dataChanged (OBJECT);
	}
END

/***** TRANSITION *****/

DIRECT (Transition_conflate)
	EVERY_TO (Transition_to_Distributions_conflate (OBJECT))
END

FORM (Transition_drawAsNumbers, "Draw as numbers", 0)
	RADIO ("Format", 1)
	RADIOBUTTON ("decimal")
	RADIOBUTTON ("exponential")
	RADIOBUTTON ("free")
	RADIOBUTTON ("rational")
	NATURAL ("Precision", "2")
	OK
DO
	EVERY_DRAW (Transition_drawAsNumbers (OBJECT, GRAPHICS,
		GET_INTEGER ("Format"), GET_INTEGER ("Precision")))
END

DIRECT (Transition_eigen)
	WHERE (SELECTED) {
		Matrix vec, val;
		if (! Transition_eigen (OBJECT, & vec, & val)) return 0;
		if (! praat_new (vec, "eigenvectors")) return 0;
		if (! praat_new (val, "eigenvalues")) return 0;
	}
END

DIRECT (Transition_help) Melder_help ("Transition"); END

FORM (Transition_power, "Transition: Power...", 0)
	NATURAL ("Power", "2")
	OK
DO
	EVERY_TO (Transition_power (OBJECT, GET_INTEGER ("Power")))
END

DIRECT (Transition_to_Matrix)
	EVERY_TO (Transition_to_Matrix (OBJECT))
END

/***** WORDLIST *****/

FORM (WordList_hasWord, "Does word occur in list?", "WordList")
	SENTENCE ("Word", "")
	OK
DO
	Melder_information (WordList_hasWord (ONLY_OBJECT, GET_STRING ("Word")) ? "1" : "0");
END

DIRECT (WordList_to_Strings)
	EVERY_TO (WordList_to_Strings (OBJECT))
END

DIRECT (WordList_upto_SpellingChecker)
	EVERY_TO (WordList_upto_SpellingChecker (OBJECT))
END

/***** Control menu *****/

FORM (Praat_test, "Praat test", 0)
	ENUM ("Test", PraatTests, 1)
	SENTENCE ("arg1", "1000000")
	SENTENCE ("arg2", "")
	SENTENCE ("arg3", "")
	SENTENCE ("arg4", "")
	OK
DO
	Praat_tests (GET_INTEGER ("Test"), GET_STRING ("arg1"),
		GET_STRING ("arg2"), GET_STRING ("arg3"), GET_STRING ("arg4"));
END

/***** Help menu *****/

DIRECT (ObjectWindow) Melder_help ("Object window"); END
DIRECT (Intro) Melder_help ("Intro"); END
DIRECT (WhatsNew) Melder_help ("What's new?"); END
DIRECT (TypesOfObjects) Melder_help ("Types of objects"); END
DIRECT (Editors) Melder_help ("Editors"); END
DIRECT (FrequentlyAskedQuestions) Melder_help ("FAQ (Frequently Asked Questions)"); END
DIRECT (Acknowledgments) Melder_help ("Acknowledgments"); END
DIRECT (FormulasTutorial) Melder_help ("Formulas"); END
DIRECT (ScriptingTutorial) Melder_help ("Scripting"); END
DIRECT (StatisticsTutorial) Melder_help ("Statistics"); END
DIRECT (Programming) Melder_help ("Programming with Praat"); END
DIRECT (SearchManual) Melder_search (); END

/***** file recognizers *****/

static Any cgnSyntaxFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 57) return NULL;
	if (! strnequ (& header [0], "<?xml version=\"1.0\"?>", 21) ||
	    ! strnequ (& header [22], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35) &&
	    ! strnequ (& header [23], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35)) return NULL;
	return TextGrid_readFromCgnSyntaxFile (file);
}

static Any chronologicalTextGridTextFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 100) return NULL;
	if (! strnequ (& header [0], "\"Praat chronological TextGrid text file\"", 40)) return NULL;
	return TextGrid_readFromChronologicalTextFile (file);
}

/***** buttons *****/

void praat_TableOfReal_init (void *klas);   /* Buttons for TableOfReal and for its subclasses. */
void praat_TableOfReal_init (void *klas) {
	praat_addAction1 (klas, 1, "Write to headerless spreadsheet file...", 0, 0, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 0, "Draw -                 ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Draw as numbers...", 0, 1, DO_TableOfReal_drawAsNumbers);
		praat_addAction1 (klas, 0, "Draw as numbers if...", 0, 1, DO_TableOfReal_drawAsNumbers_if);
		praat_addAction1 (klas, 0, "Draw as squares...", 0, 1, DO_TableOfReal_drawAsSquares);	
		praat_addAction1 (klas, 0, "-- draw lines --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Draw vertical lines...", 0, 1, DO_TableOfReal_drawVerticalLines);
		praat_addAction1 (klas, 0, "Draw horizontal lines...", 0, 1, DO_TableOfReal_drawHorizontalLines);
		praat_addAction1 (klas, 0, "Draw left and right lines...", 0, 1, DO_TableOfReal_drawLeftAndRightLines);
		praat_addAction1 (klas, 0, "Draw top and bottom lines...", 0, 1, DO_TableOfReal_drawTopAndBottomLines);
	praat_addAction1 (klas, 0, "Query -                ", 0, 0, 0);
		praat_addAction1 (klas, 1, "Get number of rows", 0, 1, DO_TableOfReal_getNumberOfRows);
		praat_addAction1 (klas, 1, "Get number of columns", 0, 1, DO_TableOfReal_getNumberOfColumns);
		praat_addAction1 (klas, 1, "Get row label...", 0, 1, DO_TableOfReal_getRowLabel);
		praat_addAction1 (klas, 1, "Get column label...", 0, 1, DO_TableOfReal_getColumnLabel);
		praat_addAction1 (klas, 1, "Get row index...", 0, 1, DO_TableOfReal_getRowIndex);
		praat_addAction1 (klas, 1, "Get column index...", 0, 1, DO_TableOfReal_getColumnIndex);
		praat_addAction1 (klas, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (klas, 1, "Get value...", 0, 1, DO_TableOfReal_getValue);
		if (klas == classTableOfReal) {
			praat_addAction1 (klas, 1, "-- get statistics --", 0, 1, 0);
			praat_addAction1 (klas, 1, "Get column mean (index)...", 0, 1, DO_TableOfReal_getColumnMean_index);
			praat_addAction1 (klas, 1, "Get column mean (label)...", 0, 1, DO_TableOfReal_getColumnMean_label);
			praat_addAction1 (klas, 1, "Get column stdev (index)...", 0, 1, DO_TableOfReal_getColumnStdev_index);
			praat_addAction1 (klas, 1, "Get column stdev (label)...", 0, 1, DO_TableOfReal_getColumnStdev_label);
		}
	praat_addAction1 (klas, 0, "Modify -               ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Formula...", 0, 1, DO_TableOfReal_formula);
		praat_addAction1 (klas, 0, "Set value...", 0, 1, DO_TableOfReal_setValue);
		praat_addAction1 (klas, 0, "Sort by label...", 0, 1, DO_TableOfReal_sortByLabel);
		praat_addAction1 (klas, 0, "Sort by column...", 0, 1, DO_TableOfReal_sortByColumn);
		praat_addAction1 (klas, 0, "-- structure --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Remove row (index)...", 0, 1, DO_TableOfReal_removeRow);
		praat_addAction1 (klas, 0, "Remove column (index)...", 0, 1, DO_TableOfReal_removeColumn);
		praat_addAction1 (klas, 0, "Insert row (index)...", 0, 1, DO_TableOfReal_insertRow);
		praat_addAction1 (klas, 0, "Insert column (index)...", 0, 1, DO_TableOfReal_insertColumn);
		praat_addAction1 (klas, 0, "-- set --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Set row label (index)...", 0, 1, DO_TableOfReal_setRowLabel_index);
		praat_addAction1 (klas, 0, "Set row label (label)...", 0, 1, DO_TableOfReal_setRowLabel_label);
		praat_addAction1 (klas, 0, "Set column label (index)...", 0, 1, DO_TableOfReal_setColumnLabel_index);
		praat_addAction1 (klas, 0, "Set column label (label)...", 0, 1, DO_TableOfReal_setColumnLabel_label);
	praat_addAction1 (klas, 0, "Synthesize -     ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Append", 0, 1, DO_TablesOfReal_append);
	praat_addAction1 (klas, 0, "Extract part -", 0, 0, 0);
		praat_addAction1 (klas, 0, "Extract row ranges...", 0, 1, DO_TableOfReal_extractRowRanges);
		praat_addAction1 (klas, 0, "Extract rows where column...", 0, 1, DO_TableOfReal_extractRowsWhereColumn);
		praat_addAction1 (klas, 0, "Extract rows where label...", 0, 1, DO_TableOfReal_extractRowsWhereLabel);
		praat_addAction1 (klas, 0, "Extract rows where...", 0, 1, DO_TableOfReal_extractRowsWhere);
		praat_addAction1 (klas, 0, "Extract column ranges...", 0, 1, DO_TableOfReal_extractColumnRanges);
		praat_addAction1 (klas, 0, "Extract columns where row...", 0, 1, DO_TableOfReal_extractColumnsWhereRow);
		praat_addAction1 (klas, 0, "Extract columns where label...", 0, 1, DO_TableOfReal_extractColumnsWhereLabel);
		praat_addAction1 (klas, 0, "Extract columns where...", 0, 1, DO_TableOfReal_extractColumnsWhere);
	praat_addAction1 (klas, 0, "Extract -", 0, 0, 0);
		praat_addAction1 (klas, 0, "Extract row labels as Strings", 0, 1, DO_TableOfReal_extractRowLabelsAsStrings);
		praat_addAction1 (klas, 0, "Extract column labels as Strings", 0, 1, DO_TableOfReal_extractColumnLabelsAsStrings);
	praat_addAction1 (klas, 0, "Convert -     ", 0, 0, 0);
		praat_addAction1 (klas, 0, "To Matrix", 0, 1, DO_TableOfReal_to_Matrix);
}

void praat_TimeFunction_query_init (void *klas);   /* Query buttons for time-based subclasses of Function. */
void praat_TimeFunction_query_init (void *klas) {
	praat_addAction1 (klas, 1, "Time domain", 0, 1, 0);
	praat_addAction1 (klas, 1, "Get starting time", 0, 2, DO_TimeFunction_getStartingTime);
	praat_addAction1 (klas, 1, "Get finishing time", 0, 2, DO_TimeFunction_getFinishingTime);
	praat_addAction1 (klas, 1, "Get total duration", 0, 2, DO_TimeFunction_getDuration);
						praat_addAction1 (klas, 1, "Get duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getDuration);
}

void praat_TimeFrameSampled_query_init (void *klas);   /* Query buttons for frame-based time-based subclasses of Sampled. */
void praat_TimeFrameSampled_query_init (void *klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, "Time sampling", 0, 1, 0);
	praat_addAction1 (klas, 1, "Get number of frames", 0, 2, DO_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, "Get time step", 0, 2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, "Get frame length", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, "Get frame duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, "Get time from frame number...", 0, 2, DO_TimeFrameSampled_getTimeFromFrame);
						praat_addAction1 (klas, 1, "Get time from frame...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, "Get frame number from time...", 0, 2, DO_TimeFrameSampled_getFrameFromTime);
						praat_addAction1 (klas, 1, "Get frame from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameFromTime);
}

void praat_TimeTier_query_init (void *klas);   /* Query buttons for time-based subclasses of AnyTier. */
void praat_TimeTier_query_init (void *klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, "Get number of points", 0, 1, DO_TimeTier_getNumberOfPoints);
	praat_addAction1 (klas, 1, "Get low index from time...", 0, 1, DO_TimeTier_getLowIndexFromTime);
	praat_addAction1 (klas, 1, "Get high index from time...", 0, 1, DO_TimeTier_getHighIndexFromTime);
	praat_addAction1 (klas, 1, "Get nearest index from time...", 0, 1, DO_TimeTier_getNearestIndexFromTime);
	praat_addAction1 (klas, 1, "Get time from index...", 0, 1, DO_TimeTier_getTimeFromIndex);
}

void praat_TimeTier_modify_init (void *klas);   /* Modification buttons for time-based subclasses of AnyTier. */
void praat_TimeTier_modify_init (void *klas) {
	praat_addAction1 (klas, 0, "Remove point...", 0, 1, DO_TimeTier_removePoint);
	praat_addAction1 (klas, 0, "Remove point near...", 0, 1, DO_TimeTier_removePointNear);
	praat_addAction1 (klas, 0, "Remove points between...", 0, 1, DO_TimeTier_removePointsBetween);
}

void praat_uvafon_init (void);
void praat_uvafon_init (void) {
	Thing_recognizeClassesByName (classSound, classMatrix, classPolygon, classPointProcess, classParamCurve,
		classSpectrum, classLtas, classSpectrogram, classFormant,
		classExcitation, classCochleagram, classVocalTract, classFormantPoint, classFormantTier,
		classLabel, classTier, classAutosegment,   /* Three obsolete classes. */
		classIntensity, classPitch, classHarmonicity,
		classStrings,
		classTableOfReal, classDistributions, classTransition, classPairDistribution,
		classRealPoint, classRealTier, classPitchTier, classIntensityTier, classDurationTier, classAmplitudeTier,
		classManipulation, classTextPoint, classTextInterval, classTextTier,
		classIntervalTier, classTextGrid, classLongSound, classWordList, classSpellingChecker,
		NULL);
	Thing_recognizeClassByOtherName (classManipulation, "Psola");
	Thing_recognizeClassByOtherName (classManipulation, "Analysis");
	Thing_recognizeClassByOtherName (classPitchTier, "StylPitch");
	Thing_recognizeClassByOtherName (classTextTier, "MarkTier");

	Data_recognizeFileType (cgnSyntaxFileRecognizer);
	Data_recognizeFileType (chronologicalTextGridTextFileRecognizer);

	ManipulationEditor_prefs ();
	SpectrumEditor_prefs ();
	TextGridEditor_prefs ();

	INCLUDE_LIBRARY (praat_uvafon_Sound_init)

	praat_addMenuCommand ("Objects", "Control", "Praat test...", 0, praat_HIDDEN, DO_Praat_test);

	praat_addMenuCommand ("Objects", "New", "-- new numerics --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "Matrix", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "Create Matrix...", 0, 1, DO_Matrix_create);
		praat_addMenuCommand ("Objects", "New", "Create simple Matrix...", 0, 1, DO_Matrix_createSimple);
	praat_addMenuCommand ("Objects", "New", "Tables", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "Create Table...", 0, 1, DO_Table_create);
		praat_addMenuCommand ("Objects", "New", "Create TableOfReal...", 0, 1, DO_TableOfReal_create);
	praat_addMenuCommand ("Objects", "New", "Tiers", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "Create empty PointProcess...", 0, 1, DO_PointProcess_createEmpty);
		praat_addMenuCommand ("Objects", "New", "Create Poisson process...", 0, 1, DO_PointProcess_createPoissonProcess);
		praat_addMenuCommand ("Objects", "New", "-- new tiers ---", 0, 1, 0);
		praat_addMenuCommand ("Objects", "New", "Create PitchTier...", 0, 1, DO_PitchTier_create);
		praat_addMenuCommand ("Objects", "New", "Create FormantTier...", 0, 1, DO_FormantTier_create);
		praat_addMenuCommand ("Objects", "New", "Create IntensityTier...", 0, 1, DO_IntensityTier_create);
		praat_addMenuCommand ("Objects", "New", "Create DurationTier...", 0, 1, DO_DurationTier_create);
		praat_addMenuCommand ("Objects", "New", "Create AmplitudeTier...", 0, 1, DO_AmplitudeTier_create);
	praat_addMenuCommand ("Objects", "New", "-- new textgrid --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "Create TextGrid...", 0, 0, DO_TextGrid_create);
	praat_addMenuCommand ("Objects", "New", "Create Strings as file list...", 0, 0, DO_Strings_createAsFileList);

	praat_addMenuCommand ("Objects", "Read", "-- read raw --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Read", "Read Matrix from raw text file...", 0, 0, DO_Matrix_readFromRawTextFile);
	praat_addMenuCommand ("Objects", "Read", "Read Matrix from LVS AP file...", 0, praat_HIDDEN, DO_Matrix_readAP);
	praat_addMenuCommand ("Objects", "Read", "Read Strings from raw text file...", 0, 0, DO_Strings_readFromRawTextFile);
	praat_addMenuCommand ("Objects", "Read", "Read TableOfReal from headerless spreadsheet file...", 0, 0, DO_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand ("Objects", "Read", "Read Table from table file...", 0, 0, DO_Table_readFromTableFile);
	praat_addMenuCommand ("Objects", "Read", "-- read tier --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Read", "Read from special tier file...", 0, 0, 0);
		praat_addMenuCommand ("Objects", "Read", "Read TextTier from Xwaves...", 0, 1, DO_TextTier_readFromXwaves);
		praat_addMenuCommand ("Objects", "Read", "Read IntervalTier from Xwaves...", 0, 1, DO_IntervalTier_readFromXwaves);

	praat_addMenuCommand ("Objects", "ApplicationHelp", "Praat Intro", 0, '?', DO_Intro);
	#ifndef macintosh
		praat_addMenuCommand ("Objects", "Help", "Object window", 0, 0, DO_ObjectWindow);
	#endif
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Frequently asked questions", 0, 0, DO_FrequentlyAskedQuestions);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "What's new?", 0, 0, DO_WhatsNew);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Types of objects", 0, 0, DO_TypesOfObjects);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Editors", 0, 0, DO_Editors);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Acknowledgments", 0, 0, DO_Acknowledgments);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "-- shell help --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Formulas tutorial", 0, 0, DO_FormulasTutorial);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Scripting tutorial", 0, 0, DO_ScriptingTutorial);
	praat_addMenuCommand ("Objects", "ApplicationHelp", "Programming", 0, 0, DO_Programming);
	#ifdef macintosh
		praat_addMenuCommand ("Objects", "Help", "Praat Intro", 0, '?', DO_Intro);
		praat_addMenuCommand ("Objects", "Help", "Object window help", 0, 0, DO_ObjectWindow);
		praat_addMenuCommand ("Objects", "Help", "-- manual --", 0, 0, 0);
		praat_addMenuCommand ("Objects", "Help", "Search Praat manual...", 0, 'M', DO_SearchManual);
	#endif

	praat_addAction1 (classAmplitudeTier, 0, "AmplitudeTier help", 0, 0, DO_AmplitudeTier_help);
	praat_addAction1 (classAmplitudeTier, 1, "Edit", 0, 0, DO_AmplitudeTier_edit);
	praat_addAction1 (classAmplitudeTier, 0, "Edit with Sound?", 0, 0, DO_info_AmplitudeTier_Sound_edit);
	praat_addAction1 (classAmplitudeTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (local)...", 0, 1, DO_AmplitudeTier_getShimmer_local);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (local_dB)...", 0, 1, DO_AmplitudeTier_getShimmer_local_dB);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (apq3)...", 0, 1, DO_AmplitudeTier_getShimmer_apq3);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (apq5)...", 0, 1, DO_AmplitudeTier_getShimmer_apq5);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (apq11)...", 0, 1, DO_AmplitudeTier_getShimmer_apq11);
		praat_addAction1 (classAmplitudeTier, 1, "Get shimmer (dda)...", 0, 1, DO_AmplitudeTier_getShimmer_dda);
	praat_addAction1 (classAmplitudeTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 0, "Add point...", 0, 1, DO_AmplitudeTier_addPoint);
		praat_addAction1 (classAmplitudeTier, 0, "Formula...", 0, 1, DO_AmplitudeTier_formula);
praat_addAction1 (classAmplitudeTier, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classAmplitudeTier, 0, "To Sound (pulse train)...", 0, 0, DO_AmplitudeTier_to_Sound);
praat_addAction1 (classAmplitudeTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classAmplitudeTier, 0, "To IntensityTier...", 0, 0, DO_AmplitudeTier_to_IntensityTier);
	praat_addAction1 (classAmplitudeTier, 0, "Down to PointProcess", 0, 0, DO_AmplitudeTier_downto_PointProcess);
	praat_addAction1 (classAmplitudeTier, 0, "Down to TableOfReal", 0, 0, DO_AmplitudeTier_downto_TableOfReal);

	praat_addAction1 (classCochleagram, 0, "Cochleagram help", 0, 0, DO_Cochleagram_help);
	praat_addAction1 (classCochleagram, 1, "Movie", 0, 0, DO_Cochleagram_movie);
praat_addAction1 (classCochleagram, 0, "Info", 0, 0, 0);
	praat_addAction1 (classCochleagram, 2, "Difference...", 0, 0, DO_Cochleagram_difference);
praat_addAction1 (classCochleagram, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, "Paint...", 0, 0, DO_Cochleagram_paint);
praat_addAction1 (classCochleagram, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, "Formula...", 0, 0, DO_Cochleagram_formula);
praat_addAction1 (classCochleagram, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, "To Excitation (slice)...", 0, 0, DO_Cochleagram_to_Excitation);
praat_addAction1 (classCochleagram, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, "To Matrix", 0, 0, DO_Cochleagram_to_Matrix);

	praat_addAction1 (classDistributions, 0, "Distributions help", 0, 0, DO_Distributions_help);
	praat_TableOfReal_init (classDistributions);
	praat_addAction1 (classDistributions, 1, "Get probability (label)...", "Get value...", 1, DO_Distributions_getProbability);
	praat_addAction1 (classDistributions, 0, "-- get from two --", "Get probability (label)...", 1, 0);
	praat_addAction1 (classDistributions, 2, "Get mean absolute difference...", "-- get from two --", 1, DO_Distributionses_getMeanAbsoluteDifference);
	praat_addAction1 (classDistributions, 0, "-- add --", "Append", 1, 0);
	praat_addAction1 (classDistributions, 0, "Add", "-- add --", 1, DO_Distributionses_add);
praat_addAction1 (classDistributions, 0, "Generate", 0, 0, 0);
	praat_addAction1 (classDistributions, 0, "To Strings...", 0, 0, DO_Distributions_to_Strings);
	praat_addAction1 (classDistributions, 0, "To Strings (exact)...", 0, 0, DO_Distributions_to_Strings_exact);
praat_addAction1 (classDistributions, 0, "Learn", 0, 0, 0);
	praat_addAction1 (classDistributions, 1, "To Transition...", 0, 0, DO_Distributions_to_Transition);
	praat_addAction1 (classDistributions, 2, "To Transition (noise)...", 0, 0, DO_Distributions_to_Transition_noise);

	praat_addAction1 (classDurationTier, 0, "DurationTier help", 0, 0, DO_DurationTier_help);
	praat_addAction1 (classDurationTier, 1, "Edit", 0, 0, DO_DurationTier_edit);
	praat_addAction1 (classDurationTier, 0, "Edit with Sound?", 0, 0, DO_info_DurationTier_Sound_edit);
	praat_addAction1 (classDurationTier, 0, "& Manipulation: Replace?", 0, 0, DO_info_DurationTier_Manipulation_replace);
	praat_addAction1 (classDurationTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classDurationTier);
		praat_addAction1 (classDurationTier, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classDurationTier, 1, "Get target duration...", 0, 1, DO_DurationTier_getTargetDuration);
	praat_addAction1 (classDurationTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classDurationTier);
		praat_addAction1 (classDurationTier, 0, "Add point...", 0, 1, DO_DurationTier_addPoint);
		praat_addAction1 (classDurationTier, 0, "Formula...", 0, 1, DO_DurationTier_formula);
praat_addAction1 (classDurationTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classDurationTier, 0, "Down to PointProcess", 0, 0, DO_DurationTier_downto_PointProcess);

	praat_addAction1 (classExcitation, 0, "Excitation help", 0, 0, DO_Excitation_help);
praat_addAction1 (classExcitation, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, "Draw...", 0, 0, DO_Excitation_draw);
praat_addAction1 (classExcitation, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, "To Formant...", 0, 0, DO_Excitation_to_Formant);
praat_addAction1 (classExcitation, 1, "Query -          ", 0, 0, 0);
	praat_addAction1 (classExcitation, 1, "Get loudness", 0, 0, DO_Excitation_getLoudness);
praat_addAction1 (classExcitation, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, "Formula...", 0, 0, DO_Excitation_formula);
praat_addAction1 (classExcitation, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, "To Matrix", 0, 0, DO_Excitation_to_Matrix);

	praat_addAction1 (classFormant, 0, "Formant help", 0, 0, DO_Formant_help);
	praat_addAction1 (classFormant, 0, "Draw -          ", 0, 0, 0);
		praat_addAction1 (classFormant, 0, "Speckle...", 0, 1, DO_Formant_drawSpeckles);
		praat_addAction1 (classFormant, 0, "Draw tracks...", 0, 1, DO_Formant_drawTracks);
		praat_addAction1 (classFormant, 0, "Scatter plot...", 0, 1, DO_Formant_scatterPlot);
	praat_addAction1 (classFormant, 0, "Query -          ", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classFormant);
		praat_addAction1 (classFormant, 1, "Get number of formants...", 0, 1, DO_Formant_getNumberOfFormants);
		praat_addAction1 (classFormant, 1, "Get minimum number of formants", 0, 1, DO_Formant_getMinimumNumberOfFormants);
		praat_addAction1 (classFormant, 1, "Get maximum number of formants", 0, 1, DO_Formant_getMaximumNumberOfFormants);
		praat_addAction1 (classFormant, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, "Get value at time...", 0, 1, DO_Formant_getValueAtTime);
		praat_addAction1 (classFormant, 1, "Get bandwidth at time...", 0, 1, DO_Formant_getBandwidthAtTime);
		praat_addAction1 (classFormant, 1, "-- get extreme --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, "Get minimum...", 0, 1, DO_Formant_getMinimum);
		praat_addAction1 (classFormant, 1, "Get time of minimum...", 0, 1, DO_Formant_getTimeOfMinimum);
		praat_addAction1 (classFormant, 1, "Get maximum...", 0, 1, DO_Formant_getMaximum);
		praat_addAction1 (classFormant, 1, "Get time of maximum...", 0, 1, DO_Formant_getTimeOfMaximum);
		praat_addAction1 (classFormant, 1, "-- get distribution --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, "Get quantile...", 0, 1, DO_Formant_getQuantile);
		praat_addAction1 (classFormant, 1, "Get mean...", 0, 1, DO_Formant_getMean);
		praat_addAction1 (classFormant, 1, "Get standard deviation...", 0, 1, DO_Formant_getStandardDeviation);
	praat_addAction1 (classFormant, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classFormant, 0, "Sort", 0, 1, DO_Formant_sort);
		praat_addAction1 (classFormant, 0, "Formula (frequencies)...", 0, 1, DO_Formant_formula_frequencies);
		praat_addAction1 (classFormant, 0, "Formula (bandwidths)...", 0, 1, DO_Formant_formula_bandwidths);
praat_addAction1 (classFormant, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classFormant, 0, "Track...", 0, 0, DO_Formant_tracker);
	praat_addAction1 (classFormant, 0, "Down to FormantTier", 0, 0, DO_Formant_downto_FormantTier);
praat_addAction1 (classFormant, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classFormant, 0, "To Matrix...", 0, 0, DO_Formant_to_Matrix);

	praat_addAction1 (classFormantTier, 0, "FormantTier help", 0, 0, DO_FormantTier_help);
	praat_addAction1 (classFormantTier, 0, "Draw -          ", 0, 0, 0);
		praat_addAction1 (classFormantTier, 0, "Speckle...", 0, 1, DO_FormantTier_speckle);
	praat_addAction1 (classFormantTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classFormantTier);
		praat_addAction1 (classFormantTier, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (classFormantTier, 1, "Get value at time...", 0, 1, DO_FormantTier_getValueAtTime);
		praat_addAction1 (classFormantTier, 1, "Get bandwidth at time...", 0, 1, DO_FormantTier_getBandwidthAtTime);
	praat_addAction1 (classFormantTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classFormantTier);
		praat_addAction1 (classFormantTier, 0, "Add point...", 0, 1, DO_FormantTier_addPoint);
praat_addAction1 (classFormantTier, 0, "Down", 0, 0, 0);
	praat_addAction1 (classFormantTier, 0, "Down to TableOfReal...", 0, 0, DO_FormantTier_downto_TableOfReal);

	praat_addAction1 (classHarmonicity, 0, "Harmonicity help", 0, 0, DO_Harmonicity_help);
praat_addAction1 (classHarmonicity, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classHarmonicity, 0, "Draw...", 0, 0, DO_Harmonicity_draw);
	praat_addAction1 (classHarmonicity, 1, "Query -          ", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, "Get value at time...", 0, 1, DO_Harmonicity_getValueAtTime);
		praat_addAction1 (classHarmonicity, 1, "Get value in frame...", 0, 1, DO_Harmonicity_getValueInFrame);
		praat_addAction1 (classHarmonicity, 1, "-- get extreme --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, "Get minimum...", 0, 1, DO_Harmonicity_getMinimum);
		praat_addAction1 (classHarmonicity, 1, "Get time of minimum...", 0, 1, DO_Harmonicity_getTimeOfMinimum);
		praat_addAction1 (classHarmonicity, 1, "Get maximum...", 0, 1, DO_Harmonicity_getMaximum);
		praat_addAction1 (classHarmonicity, 1, "Get time of maximum...", 0, 1, DO_Harmonicity_getTimeOfMaximum);
		praat_addAction1 (classHarmonicity, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, "Get mean...", 0, 1, DO_Harmonicity_getMean);
		praat_addAction1 (classHarmonicity, 1, "Get standard deviation...", 0, 1, DO_Harmonicity_getStandardDeviation);
praat_addAction1 (classHarmonicity, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classHarmonicity, 0, "Formula...", 0, 0, DO_Harmonicity_formula);
praat_addAction1 (classHarmonicity, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classHarmonicity, 0, "To Matrix", 0, 0, DO_Harmonicity_to_Matrix);

	praat_addAction1 (classIntensity, 0, "Intensity help", 0, 0, DO_Intensity_help);
	praat_addAction1 (classIntensity, 0, "Draw...", 0, 0, DO_Intensity_draw);
	praat_addAction1 (classIntensity, 1, "Query -          ", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classIntensity);
		praat_addAction1 (classIntensity, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, "Get value at time...", 0, 1, DO_Intensity_getValueAtTime);
		praat_addAction1 (classIntensity, 1, "Get value in frame...", 0, 1, DO_Intensity_getValueInFrame);
		praat_addAction1 (classIntensity, 1, "-- get extreme --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, "Get minimum...", 0, 1, DO_Intensity_getMinimum);
		praat_addAction1 (classIntensity, 1, "Get time of minimum...", 0, 1, DO_Intensity_getTimeOfMinimum);
		praat_addAction1 (classIntensity, 1, "Get maximum...", 0, 1, DO_Intensity_getMaximum);
		praat_addAction1 (classIntensity, 1, "Get time of maximum...", 0, 1, DO_Intensity_getTimeOfMaximum);
		praat_addAction1 (classIntensity, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, "Get quantile...", 0, 1, DO_Intensity_getQuantile);
		praat_addAction1 (classIntensity, 1, "Get mean...", 0, 1, DO_Intensity_getMean);
		praat_addAction1 (classIntensity, 1, "Get standard deviation...", 0, 1, DO_Intensity_getStandardDeviation);
	praat_addAction1 (classIntensity, 0, "Modify", 0, 0, 0);
		praat_addAction1 (classIntensity, 0, "Formula...", 0, 0, DO_Intensity_formula);
	praat_addAction1 (classIntensity, 0, "Analyse", 0, 0, 0);
		praat_addAction1 (classIntensity, 0, "To IntensityTier (peaks)", 0, 0, DO_Intensity_to_IntensityTier_peaks);
		praat_addAction1 (classIntensity, 0, "To IntensityTier (valleys)", 0, 0, DO_Intensity_to_IntensityTier_valleys);
	praat_addAction1 (classIntensity, 0, "Convert", 0, 0, 0);
		praat_addAction1 (classIntensity, 0, "Down to IntensityTier", 0, 0, DO_Intensity_downto_IntensityTier);
		praat_addAction1 (classIntensity, 0, "Down to Matrix", 0, 0, DO_Intensity_downto_Matrix);

	praat_addAction1 (classIntensityTier, 0, "IntensityTier help", 0, 0, DO_IntensityTier_help);
	praat_addAction1 (classIntensityTier, 1, "Edit", 0, 0, DO_IntensityTier_edit);
	praat_addAction1 (classIntensityTier, 0, "Edit with Sound?", 0, 0, DO_info_IntensityTier_Sound_edit);
	praat_addAction1 (classIntensityTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classIntensityTier);
	praat_addAction1 (classIntensityTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 0, "Add point...", 0, 1, DO_IntensityTier_addPoint);
		praat_addAction1 (classIntensityTier, 0, "Formula...", 0, 1, DO_IntensityTier_formula);
praat_addAction1 (classIntensityTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classIntensityTier, 0, "To AmplitudeTier", 0, 0, DO_IntensityTier_to_AmplitudeTier);
	praat_addAction1 (classIntensityTier, 0, "Down to PointProcess", 0, 0, DO_IntensityTier_downto_PointProcess);
	praat_addAction1 (classIntensityTier, 0, "Down to TableOfReal", 0, 0, DO_IntensityTier_downto_TableOfReal);

	praat_addAction1 (classIntervalTier, 0, "IntervalTier help", 0, 0, DO_IntervalTier_help);
praat_addAction1 (classIntervalTier, 0, "Collect", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
praat_addAction1 (classIntervalTier, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Get starting points...", 0, 0, DO_IntervalTier_getStartingPoints);
	praat_addAction1 (classIntervalTier, 0, "Get centre points...", 0, 0, DO_IntervalTier_getCentrePoints);
	praat_addAction1 (classIntervalTier, 0, "Get end points...", 0, 0, DO_IntervalTier_getEndPoints);
praat_addAction1 (classIntervalTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Down to TableOfReal (any)", 0, 0, DO_IntervalTier_downto_TableOfReal_any);
	praat_addAction1 (classIntervalTier, 0, "Down to TableOfReal...", 0, 0, DO_IntervalTier_downto_TableOfReal);

	praat_addAction1 (classLabel, 0, "& Sound: To TextGrid?", 0, 0, DO_info_Label_Sound_to_TextGrid);

	praat_addAction1 (classLtas, 0, "Ltas help", 0, 0, DO_Ltas_help);
	praat_addAction1 (classLtas, 0, "Draw...", 0, 0, DO_Ltas_draw);
	praat_addAction1 (classLtas, 1, "Query -          ", 0, 0, 0);
		praat_addAction1 (classLtas, 1, "Frequency domain", 0, 1, 0);
		praat_addAction1 (classLtas, 1, "Get lowest frequency", 0, 2, DO_Ltas_getLowestFrequency);
		praat_addAction1 (classLtas, 1, "Get highest frequency", 0, 2, DO_Ltas_getHighestFrequency);
		praat_addAction1 (classLtas, 1, "Frequency sampling", 0, 1, 0);
		praat_addAction1 (classLtas, 1, "Get number of bins", 0, 2, DO_Ltas_getNumberOfBins);
			praat_addAction1 (classLtas, 1, "Get number of bands", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getNumberOfBins);
		praat_addAction1 (classLtas, 1, "Get bin width", 0, 2, DO_Ltas_getBinWidth);
			praat_addAction1 (classLtas, 1, "Get band width", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinWidth);
		praat_addAction1 (classLtas, 1, "Get frequency from bin number...", 0, 2, DO_Ltas_getFrequencyFromBinNumber);
			praat_addAction1 (classLtas, 1, "Get frequency from band...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getFrequencyFromBinNumber);
		praat_addAction1 (classLtas, 1, "Get bin number from frequency...", 0, 2, DO_Ltas_getBinNumberFromFrequency);
			praat_addAction1 (classLtas, 1, "Get band from frequency...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinNumberFromFrequency);
		praat_addAction1 (classLtas, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, "Get value at frequency...", 0, 1, DO_Ltas_getValueAtFrequency);
		praat_addAction1 (classLtas, 1, "Get value in bin...", 0, 1, DO_Ltas_getValueInBin);
			praat_addAction1 (classLtas, 1, "Get value in band...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Ltas_getValueInBin);
		praat_addAction1 (classLtas, 1, "-- get extreme --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, "Get minimum...", 0, 1, DO_Ltas_getMinimum);
		praat_addAction1 (classLtas, 1, "Get frequency of minimum...", 0, 1, DO_Ltas_getFrequencyOfMinimum);
		praat_addAction1 (classLtas, 1, "Get maximum...", 0, 1, DO_Ltas_getMaximum);
		praat_addAction1 (classLtas, 1, "Get frequency of maximum...", 0, 1, DO_Ltas_getFrequencyOfMaximum);
		praat_addAction1 (classLtas, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, "Get mean...", 0, 1, DO_Ltas_getMean);
		praat_addAction1 (classLtas, 1, "Get slope...", 0, 1, DO_Ltas_getSlope);
		praat_addAction1 (classLtas, 1, "Get local peak height...", 0, 1, DO_Ltas_getLocalPeakHeight);
		praat_addAction1 (classLtas, 1, "Get standard deviation...", 0, 1, DO_Ltas_getStandardDeviation);
	praat_addAction1 (classLtas, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classLtas, 0, "Formula...", 0, 0, DO_Ltas_formula);
	praat_addAction1 (classLtas, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classLtas, 0, "Compute trend line...", 0, 0, DO_Ltas_computeTrendLine);
	praat_addAction1 (classLtas, 0, "Subtract trend line...", 0, 0, DO_Ltas_subtractTrendLine);
	praat_addAction1 (classLtas, 0, "Combine", 0, 0, 0);
	praat_addAction1 (classLtas, 0, "Merge", 0, 0, DO_Ltases_merge);
	praat_addAction1 (classLtas, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classLtas, 0, "To Matrix", 0, 0, DO_Ltas_to_Matrix);

	praat_addAction1 (classManipulation, 0, "Manipulation help", 0, 0, DO_Manipulation_help);
	praat_addAction1 (classManipulation, 1, "Edit", 0, 0, DO_Manipulation_edit);
	praat_addAction1 (classManipulation, 0, "Play (PSOLA)", 0, 0, DO_Manipulation_play_psola);
	praat_addAction1 (classManipulation, 0, "Play (LPC)", 0, 0, DO_Manipulation_play_lpc);
	praat_addAction1 (classManipulation, 0, "Get resynthesis (PSOLA)", 0, 0, DO_Manipulation_getResynthesis_psola);
	praat_addAction1 (classManipulation, 0, "Get resynthesis (LPC)", 0, 0, DO_Manipulation_getResynthesis_lpc);
	praat_addAction1 (classManipulation, 0, "Extract original sound", 0, 0, DO_Manipulation_extractOriginalSound);
	praat_addAction1 (classManipulation, 0, "Extract pulses", 0, 0, DO_Manipulation_extractPulses);
	praat_addAction1 (classManipulation, 0, "Extract pitch tier", 0, 0, DO_Manipulation_extractPitchTier);
	praat_addAction1 (classManipulation, 0, "Extract duration tier", 0, 0, DO_Manipulation_extractDurationTier);
	praat_addAction1 (classManipulation, 0, "Replace pitch tier?", 0, 0, DO_Manipulation_replacePitchTier_help);
	praat_addAction1 (classManipulation, 0, "Replace duration tier?", 0, 0, DO_Manipulation_replaceDurationTier_help);
	praat_addAction1 (classManipulation, 1, "Write to text file without Sound...", 0, 0, DO_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, "Write to binary file without Sound...", 0, 0, DO_Manipulation_writeToBinaryFileWithoutSound);

	praat_addAction1 (classMatrix, 0, "Matrix help", 0, 0, DO_Matrix_help);
	praat_addAction1 (classMatrix, 1, "Write to matrix text file...", 0, 0, DO_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, "Write to headerless spreadsheet file...", 0, 0, DO_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, "Play movie", 0, 0, DO_Matrix_movie);
	praat_addAction1 (classMatrix, 0, "Draw -          ", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, "Draw rows...", 0, 1, DO_Matrix_drawRows);
		praat_addAction1 (classMatrix, 0, "Draw one contour...", 0, 1, DO_Matrix_drawOneContour);
		praat_addAction1 (classMatrix, 0, "Draw contours...", 0, 1, DO_Matrix_drawContours);
		praat_addAction1 (classMatrix, 0, "Paint image...", 0, 1, DO_Matrix_paintImage);
		praat_addAction1 (classMatrix, 0, "Paint contours...", 0, 1, DO_Matrix_paintContours);
		praat_addAction1 (classMatrix, 0, "Paint cells...", 0, 1, DO_Matrix_paintCells);
		praat_addAction1 (classMatrix, 0, "Paint surface...", 0, 1, DO_Matrix_paintSurface);
	praat_addAction1 (classMatrix, 1, "Query -          ", 0, 0, 0);
		praat_addAction1 (classMatrix, 1, "Get lowest x", 0, 1, DO_Matrix_getLowestX);
		praat_addAction1 (classMatrix, 1, "Get highest x", 0, 1, DO_Matrix_getHighestX);
		praat_addAction1 (classMatrix, 1, "Get lowest y", 0, 1, DO_Matrix_getLowestY);
		praat_addAction1 (classMatrix, 1, "Get highest y", 0, 1, DO_Matrix_getHighestY);
		praat_addAction1 (classMatrix, 1, "-- get structure --", 0, 1, 0);
		praat_addAction1 (classMatrix, 1, "Get number of rows", 0, 1, DO_Matrix_getNumberOfRows);
		praat_addAction1 (classMatrix, 1, "Get number of columns", 0, 1, DO_Matrix_getNumberOfColumns);
		praat_addAction1 (classMatrix, 1, "Get row distance", 0, 1, DO_Matrix_getRowDistance);
		praat_addAction1 (classMatrix, 1, "Get column distance", 0, 1, DO_Matrix_getColumnDistance);
		praat_addAction1 (classMatrix, 1, "Get y of row...", 0, 1, DO_Matrix_getYofRow);
		praat_addAction1 (classMatrix, 1, "Get x of column...", 0, 1, DO_Matrix_getXofColumn);
		praat_addAction1 (classMatrix, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (classMatrix, 1, "Get value in cell...", 0, 1, DO_Matrix_getValueInCell);
		praat_addAction1 (classMatrix, 1, "Get value at xy...", 0, 1, DO_Matrix_getValueAtXY);
		praat_addAction1 (classMatrix, 1, "Get sum", 0, 1, DO_Matrix_getSum);
	praat_addAction1 (classMatrix, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, "Formula...", 0, 1, DO_Matrix_formula);
		praat_addAction1 (classMatrix, 0, "Set value...", 0, 1, DO_Matrix_setValue);
praat_addAction1 (classMatrix, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classMatrix, 0, "Eigen", 0, 0, DO_Matrix_eigen);
	praat_addAction1 (classMatrix, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classMatrix, 0, "Power...", 0, 0, DO_Matrix_power);
	praat_addAction1 (classMatrix, 0, "Combine two Matrices -", 0, 0, 0);
		praat_addAction1 (classMatrix, 2, "Merge (append rows)", 0, 1, DO_Matrix_appendRows);
		praat_addAction1 (classMatrix, 2, "To ParamCurve", 0, 1, DO_Matrix_to_ParamCurve);
	praat_addAction1 (classMatrix, 0, "Cast -", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, "To Cochleagram", 0, 1, DO_Matrix_to_Cochleagram);
		praat_addAction1 (classMatrix, 0, "To Excitation", 0, 1, DO_Matrix_to_Excitation);
		praat_addAction1 (classMatrix, 0, "To Harmonicity", 0, 1, DO_Matrix_to_Harmonicity);
		praat_addAction1 (classMatrix, 0, "To Intensity", 0, 1, DO_Matrix_to_Intensity);
		praat_addAction1 (classMatrix, 0, "To Ltas", 0, 1, DO_Matrix_to_Ltas);
		praat_addAction1 (classMatrix, 0, "To Pitch", 0, 1, DO_Matrix_to_Pitch);
		praat_addAction1 (classMatrix, 0, "To PointProcess", 0, 1, DO_Matrix_to_PointProcess);
		praat_addAction1 (classMatrix, 0, "To Polygon", 0, 1, DO_Matrix_to_Polygon);
		praat_addAction1 (classMatrix, 0, "To Sound (slice)...", 0, 1, DO_Matrix_to_Sound);
		praat_addAction1 (classMatrix, 0, "To Spectrogram", 0, 1, DO_Matrix_to_Spectrogram);
		praat_addAction1 (classMatrix, 0, "To TableOfReal", 0, 1, DO_Matrix_to_TableOfReal);
		praat_addAction1 (classMatrix, 0, "To Spectrum", 0, 1, DO_Matrix_to_Spectrum);
		praat_addAction1 (classMatrix, 0, "To Transition", 0, 1, DO_Matrix_to_Transition);
		praat_addAction1 (classMatrix, 0, "To VocalTract", 0, 1, DO_Matrix_to_VocalTract);

	praat_addAction1 (classPairDistribution, 0, "PairDistribution help", 0, 0, DO_PairDistribution_help);
	praat_addAction1 (classPairDistribution, 1, "To Stringses...", 0, 0, DO_PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, "Query -          ", 0, 0, 0);
	praat_addAction1 (classPairDistribution, 1, "Get fraction correct (maximum likelihood)", 0, 1, DO_PairDistribution_getFractionCorrect_maximumLikelihood);
	praat_addAction1 (classPairDistribution, 1, "Get fraction correct (probability matching)", 0, 1, DO_PairDistribution_getFractionCorrect_probabilityMatching);

	praat_addAction1 (classParamCurve, 0, "ParamCurve help", 0, 0, DO_ParamCurve_help);
	praat_addAction1 (classParamCurve, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classParamCurve, 0, "Draw...", 0, 0, DO_ParamCurve_draw);

	praat_addAction1 (classPitch, 0, "Pitch help", 0, 0, DO_Pitch_help);
	praat_addAction1 (classPitch, 1, "Edit", 0, 0, DO_Pitch_edit);
	praat_addAction1 (classPitch, 0, "Play -           ", 0, 0, 0);
		praat_addAction1 (classPitch, 0, "Play pulses", 0, 1, DO_Pitch_play);
		praat_addAction1 (classPitch, 0, "Hum", 0, 1, DO_Pitch_hum);
	praat_addAction1 (classPitch, 0, "Draw -           ", 0, 0, 0);
		praat_addAction1 (classPitch, 0, "Draw...", 0, 1, DO_Pitch_draw);
		praat_addAction1 (classPitch, 0, "Draw logarithmic...", 0, 1, DO_Pitch_drawLogarithmic);
		praat_addAction1 (classPitch, 0, "Draw semitones...", 0, 1, DO_Pitch_drawSemitones);
		praat_addAction1 (classPitch, 0, "Draw mel...", 0, 1, DO_Pitch_drawMel);
		praat_addAction1 (classPitch, 0, "Draw erb...", 0, 1, DO_Pitch_drawErb);
		praat_addAction1 (classPitch, 0, "Speckle...", 0, 1, DO_Pitch_speckle);
		praat_addAction1 (classPitch, 0, "Speckle logarithmic...", 0, 1, DO_Pitch_speckleLogarithmic);
		praat_addAction1 (classPitch, 0, "Speckle semitones...", 0, 1, DO_Pitch_speckleSemitones);
		praat_addAction1 (classPitch, 0, "Speckle mel...", 0, 1, DO_Pitch_speckleMel);
		praat_addAction1 (classPitch, 0, "Speckle erb...", 0, 1, DO_Pitch_speckleErb);
	praat_addAction1 (classPitch, 0, "Query -          ", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classPitch);
		praat_addAction1 (classPitch, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, "Count voiced frames", 0, 1, DO_Pitch_getNumberOfVoicedFrames);
		praat_addAction1 (classPitch, 1, "Get value at time...", 0, 1, DO_Pitch_getValueAtTime);
		praat_addAction1 (classPitch, 1, "Get value in frame...", 0, 1, DO_Pitch_getValueInFrame);
		praat_addAction1 (classPitch, 1, "-- get extreme --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, "Get minimum...", 0, 1, DO_Pitch_getMinimum);
		praat_addAction1 (classPitch, 1, "Get time of minimum...", 0, 1, DO_Pitch_getTimeOfMinimum);
		praat_addAction1 (classPitch, 1, "Get maximum...", 0, 1, DO_Pitch_getMaximum);
		praat_addAction1 (classPitch, 1, "Get time of maximum...", 0, 1, DO_Pitch_getTimeOfMaximum);
		praat_addAction1 (classPitch, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, "Get quantile...", 0, 1, DO_Pitch_getQuantile);
		/*praat_addAction1 (classPitch, 1, "Get spreading...", 0, 1, DO_Pitch_getSpreading);*/
		praat_addAction1 (classPitch, 1, "Get mean...", 0, 1, DO_Pitch_getMean);
		praat_addAction1 (classPitch, 1, "Get standard deviation...", 0, 1, DO_Pitch_getStandardDeviation);
		praat_addAction1 (classPitch, 1, "-- get slope --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, "Get mean absolute slope...", 0, 1, DO_Pitch_getMeanAbsoluteSlope);
		praat_addAction1 (classPitch, 1, "Get slope without octave jumps", 0, 1, DO_Pitch_getMeanAbsSlope_noOctave);
		praat_addAction1 (classPitch, 2, "-- query two --", 0, 1, 0);
		praat_addAction1 (classPitch, 2, "Count differences", 0, 1, DO_Pitch_difference);
	praat_addAction1 (classPitch, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classPitch, 0, "Formula...", 0, 1, DO_Pitch_formula);
	praat_addAction1 (classPitch, 0, "Annotate -   ", 0, 0, 0);
		praat_addAction1 (classPitch, 0, "To TextGrid...", 0, 1, DO_Pitch_to_TextGrid);
		praat_addAction1 (classPitch, 0, "-- to single tier --", 0, 1, 0);
		praat_addAction1 (classPitch, 0, "To TextTier", 0, 1, DO_Pitch_to_TextTier);
		praat_addAction1 (classPitch, 0, "To IntervalTier", 0, 1, DO_Pitch_to_IntervalTier);
praat_addAction1 (classPitch, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classPitch, 0, "To PointProcess", 0, 0, DO_Pitch_to_PointProcess);
praat_addAction1 (classPitch, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classPitch, 0, "To Sound (pulses)", 0, 0, DO_Pitch_to_Sound_pulses);
	praat_addAction1 (classPitch, 0, "To Sound (hum)", 0, 0, DO_Pitch_to_Sound_hum);
	praat_addAction1 (classPitch, 0, "To Sound (sine)...", 0, 1, DO_Pitch_to_Sound_sine);
praat_addAction1 (classPitch, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classPitch, 0, "Kill octave jumps", 0, 0, DO_Pitch_killOctaveJumps);
	praat_addAction1 (classPitch, 0, "Interpolate", 0, 0, DO_Pitch_interpolate);
	praat_addAction1 (classPitch, 0, "Smooth...", 0, 0, DO_Pitch_smooth);
	praat_addAction1 (classPitch, 0, "Subtract linear fit...", 0, 0, DO_Pitch_subtractLinearFit);
	praat_addAction1 (classPitch, 0, "Down to PitchTier", 0, 0, DO_Pitch_to_PitchTier);
	praat_addAction1 (classPitch, 0, "To Matrix", 0, 0, DO_Pitch_to_Matrix);

	praat_addAction1 (classPitchTier, 1, "Write to PitchTier spreadsheet file...", 0, 0, DO_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, "Write to headerless spreadsheet file...", 0, 0, DO_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 0, "PitchTier help", 0, 0, DO_PitchTier_help);
	praat_addAction1 (classPitchTier, 1, "Edit", 0, 0, DO_PitchTier_edit);
	praat_addAction1 (classPitchTier, 0, "Edit with Sound?", 0, 0, DO_info_PitchTier_Sound_edit);
	praat_addAction1 (classPitchTier, 0, "Play pulses", 0, 0, DO_PitchTier_play);
	praat_addAction1 (classPitchTier, 0, "Hum", 0, 0, DO_PitchTier_hum);
	praat_addAction1 (classPitchTier, 0, "Play sine", 0, 0, DO_PitchTier_playSine);
	praat_addAction1 (classPitchTier, 0, "Draw...", 0, 0, DO_PitchTier_draw);
	praat_addAction1 (classPitchTier, 0, "& Manipulation: Replace?", 0, 0, DO_info_PitchTier_Manipulation_replace);
	praat_addAction1 (classPitchTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classPitchTier);
		praat_addAction1 (classPitchTier, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 1, "Get value at time...", 0, 1, DO_PitchTier_getValueAtTime);
		praat_addAction1 (classPitchTier, 1, "Get value at index...", 0, 1, DO_PitchTier_getValueAtIndex);
		praat_addAction1 (classPitchTier, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 1, "Get mean (curve)...", 0, 1, DO_PitchTier_getMean_curve);
		praat_addAction1 (classPitchTier, 1, "Get mean (points)...", 0, 1, DO_PitchTier_getMean_points);
		praat_addAction1 (classPitchTier, 1, "Get standard deviation (curve)...", 0, 1, DO_PitchTier_getStandardDeviation_curve);
		praat_addAction1 (classPitchTier, 1, "Get standard deviation (points)...", 0, 1, DO_PitchTier_getStandardDeviation_points);
	praat_addAction1 (classPitchTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classPitchTier);
		praat_addAction1 (classPitchTier, 0, "Add point...", 0, 1, DO_PitchTier_addPoint);
		praat_addAction1 (classPitchTier, 0, "Formula...", 0, 1, DO_PitchTier_formula);
		praat_addAction1 (classPitchTier, 0, "-- stylize --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 0, "Stylize...", 0, 1, DO_PitchTier_stylize);
		praat_addAction1 (classPitchTier, 0, "Interpolate quadratically...", 0, 1, DO_PitchTier_interpolateQuadratically);
		praat_addAction1 (classPitchTier, 0, "-- modify frequencies --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 0, "Shift frequencies...", 0, 1, DO_PitchTier_shiftFrequencies);
		praat_addAction1 (classPitchTier, 0, "Multiply frequencies...", 0, 1, DO_PitchTier_multiplyFrequencies);
	praat_addAction1 (classPitchTier, 0, "Synthesize -", 0, 0, 0);
		praat_addAction1 (classPitchTier, 0, "To PointProcess", 0, 1, DO_PitchTier_to_PointProcess);
		praat_addAction1 (classPitchTier, 0, "To Sound (pulse train)...", 0, 1, DO_PitchTier_to_Sound_pulseTrain);
		praat_addAction1 (classPitchTier, 0, "To Sound (sine)...", 0, 1, DO_PitchTier_to_Sound_sine);
	praat_addAction1 (classPitchTier, 0, "Convert -", 0, 0, 0);
		praat_addAction1 (classPitchTier, 0, "Down to PointProcess", 0, 1, DO_PitchTier_downto_PointProcess);
		praat_addAction1 (classPitchTier, 0, "Down to TableOfReal...", 0, 1, DO_PitchTier_downto_TableOfReal);

	praat_addAction1 (classPointProcess, 0, "PointProcess help", 0, 0, DO_PointProcess_help);
	praat_addAction1 (classPointProcess, 0, "Edit -          ", 0, 0, 0);
		praat_addAction1 (classPointProcess, 1, "Edit alone", 0, 1, DO_PointProcess_edit);
		praat_addAction1 (classPointProcess, 0, "Edit with Sound?", 0, 1, DO_info_PointProcess_Sound_edit);
	praat_addAction1 (classPointProcess, 0, "Play -          ", 0, 0, 0);
		praat_addAction1 (classPointProcess, 0, "Play as pulse train", 0, 1, DO_PointProcess_play);
		praat_addAction1 (classPointProcess, 0, "Hum", 0, 1, DO_PointProcess_hum);
	praat_addAction1 (classPointProcess, 0, "Draw...", 0, 0, DO_PointProcess_draw);
	praat_addAction1 (classPointProcess, 0, "Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classPointProcess);
		praat_addAction1 (classPointProcess, 1, "-- script get --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 1, "Get number of points", 0, 1, DO_PointProcess_getNumberOfPoints);
		praat_addAction1 (classPointProcess, 1, "Get low index...", 0, 1, DO_PointProcess_getLowIndex);
		praat_addAction1 (classPointProcess, 1, "Get high index...", 0, 1, DO_PointProcess_getHighIndex);
		praat_addAction1 (classPointProcess, 1, "Get nearest index...", 0, 1, DO_PointProcess_getNearestIndex);
		praat_addAction1 (classPointProcess, 1, "Get time from index...", 0, 1, DO_PointProcess_getTimeFromIndex);
		praat_addAction1 (classPointProcess, 1, "Get interval...", 0, 1, DO_PointProcess_getInterval);
		praat_addAction1 (classPointProcess, 1, "-- periods --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 1, "Get number of periods...", 0, 1, DO_PointProcess_getNumberOfPeriods);
		praat_addAction1 (classPointProcess, 1, "Get mean period...", 0, 1, DO_PointProcess_getMeanPeriod);
		praat_addAction1 (classPointProcess, 1, "Get stdev period...", 0, 1, DO_PointProcess_getStdevPeriod);
		praat_addAction1 (classPointProcess, 1, "Get jitter (local)...", 0, 1, DO_PointProcess_getJitter_local);
		praat_addAction1 (classPointProcess, 1, "Get jitter (local, absolute)...", 0, 1, DO_PointProcess_getJitter_local_absolute);
		praat_addAction1 (classPointProcess, 1, "Get jitter (rap)...", 0, 1, DO_PointProcess_getJitter_rap);
		praat_addAction1 (classPointProcess, 1, "Get jitter (ppq5)...", 0, 1, DO_PointProcess_getJitter_ppq5);
		praat_addAction1 (classPointProcess, 1, "Get jitter (ddp)...", 0, 1, DO_PointProcess_getJitter_ddp);
	praat_addAction1 (classPointProcess, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classPointProcess, 0, "Add point...", 0, 1, DO_PointProcess_addPoint);
		praat_addAction1 (classPointProcess, 0, "Remove point...", 0, 1, DO_PointProcess_removePoint);
		praat_addAction1 (classPointProcess, 0, "Remove point near...", 0, 1, DO_PointProcess_removePointNear);
		praat_addAction1 (classPointProcess, 0, "Remove points...", 0, 1, DO_PointProcess_removePoints);
		praat_addAction1 (classPointProcess, 0, "Remove points between...", 0, 1, DO_PointProcess_removePointsBetween);
		praat_addAction1 (classPointProcess, 0, "-- voice --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 0, "Fill...", 0, 1, DO_PointProcess_fill);
		praat_addAction1 (classPointProcess, 0, "Voice...", 0, 1, DO_PointProcess_voice);
	praat_addAction1 (classPointProcess, 0, "Annotate -   ", 0, 0, 0);
		praat_addAction1 (classPointProcess, 0, "To TextGrid...", 0, 1, DO_PointProcess_to_TextGrid);
		praat_addAction1 (classPointProcess, 0, "-- to single tier --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 0, "To TextTier", 0, 1, DO_PointProcess_to_TextTier);
		praat_addAction1 (classPointProcess, 0, "To IntervalTier", 0, 1, DO_PointProcess_to_IntervalTier);
praat_addAction1 (classPointProcess, 0, "Set calculus", 0, 0, 0);
	praat_addAction1 (classPointProcess, 2, "Union", 0, 0, DO_PointProcess_union);
	praat_addAction1 (classPointProcess, 2, "Intersection", 0, 0, DO_PointProcess_intersection);
	praat_addAction1 (classPointProcess, 2, "Difference", 0, 0, DO_PointProcess_difference);
praat_addAction1 (classPointProcess, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, "To PitchTier...", 0, 0, DO_PointProcess_to_PitchTier);
	praat_addAction1 (classPointProcess, 0, "To TextGrid (vuv)...", 0, 0, DO_PointProcess_to_TextGrid_vuv);
praat_addAction1 (classPointProcess, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, "To Sound (pulse train)...", 0, 0, DO_PointProcess_to_Sound);
	praat_addAction1 (classPointProcess, 0, "To Sound (hum)...", 0, 0, DO_PointProcess_to_Sound_hum);
praat_addAction1 (classPointProcess, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, "To Matrix", 0, 0, DO_PointProcess_to_Matrix);
	praat_addAction1 (classPointProcess, 0, "Up to TextTier...", 0, 0, DO_PointProcess_upto_TextTier);
	praat_addAction1 (classPointProcess, 0, "Up to PitchTier...", 0, 0, DO_PointProcess_upto_PitchTier);
	praat_addAction1 (classPointProcess, 0, "Up to IntensityTier...", 0, 0, DO_PointProcess_upto_IntensityTier);

	praat_addAction1 (classPolygon, 0, "Polygon help", 0, 0, DO_Polygon_help);
praat_addAction1 (classPolygon, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, "Draw...", 0, 0, DO_Polygon_draw);
	praat_addAction1 (classPolygon, 0, "Paint...", 0, 0, DO_Polygon_paint);
	praat_addAction1 (classPolygon, 0, "Draw circles...", 0, 0, DO_Polygon_drawCircles);
	praat_addAction1 (classPolygon, 0, "Paint circles...", 0, 0, DO_Polygon_paintCircles);
	praat_addAction1 (classPolygon, 2, "Draw connection...", 0, 0, DO_Polygons_drawConnection);
praat_addAction1 (classPolygon, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, "Randomize", 0, 0, DO_Polygon_randomize);
	praat_addAction1 (classPolygon, 0, "Salesperson...", 0, 0, DO_Polygon_salesperson);
praat_addAction1 (classPolygon, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, "To Matrix", 0, 0, DO_Polygon_to_Matrix);

	praat_addAction1 (classSpectrogram, 0, "Spectrogram help", 0, 0, DO_Spectrogram_help);
	praat_addAction1 (classSpectrogram, 1, "View", 0, 0, DO_Spectrogram_view);
	praat_addAction1 (classSpectrogram, 1, "Movie", 0, 0, DO_Spectrogram_movie);
praat_addAction1 (classSpectrogram, 0, "Query -          ", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classSpectrogram);
	praat_addAction1 (classSpectrogram, 1, "Get power at...", 0, 1, DO_Spectrogram_getPowerAt);
praat_addAction1 (classSpectrogram, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classSpectrogram, 0, "Paint...", 0, 0, DO_Spectrogram_paint);
praat_addAction1 (classSpectrogram, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classSpectrogram, 0, "To Spectrum (slice)...", 0, 0, DO_Spectrogram_to_Spectrum);
praat_addAction1 (classSpectrogram, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classSpectrogram, 0, "To Sound...", 0, 0, DO_Spectrogram_to_Sound);
praat_addAction1 (classSpectrogram, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classSpectrogram, 0, "Formula...", 0, 0, DO_Spectrogram_formula);
praat_addAction1 (classSpectrogram, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classSpectrogram, 0, "To Matrix", 0, 0, DO_Spectrogram_to_Matrix);

	praat_addAction1 (classSpectrum, 0, "Spectrum help", 0, 0, DO_Spectrum_help);
	praat_addAction1 (classSpectrum, 1, "Edit", 0, 0, DO_Spectrum_edit);
	praat_addAction1 (classSpectrum, 0, "Draw -          ", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "Draw...", 0, 1, DO_Spectrum_draw);
		praat_addAction1 (classSpectrum, 0, "Draw (log freq)...", 0, 1, DO_Spectrum_drawLogFreq);
	praat_addAction1 (classSpectrum, 1, "Query -          ", 0, 0, 0);
		praat_addAction1 (classSpectrum, 1, "Frequency domain", 0, 1, 0);
			praat_addAction1 (classSpectrum, 1, "Get lowest frequency", 0, 2, DO_Spectrum_getLowestFrequency);
			praat_addAction1 (classSpectrum, 1, "Get highest frequency", 0, 2, DO_Spectrum_getHighestFrequency);
		praat_addAction1 (classSpectrum, 1, "Frequency sampling", 0, 1, 0);
			praat_addAction1 (classSpectrum, 1, "Get number of bins", 0, 2, DO_Spectrum_getNumberOfBins);
			praat_addAction1 (classSpectrum, 1, "Get bin width", 0, 2, DO_Spectrum_getBinWidth);
			praat_addAction1 (classSpectrum, 1, "Get frequency from bin number...", 0, 2, DO_Spectrum_getFrequencyFromBin);
						praat_addAction1 (classSpectrum, 1, "Get frequency from bin...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getFrequencyFromBin);
			praat_addAction1 (classSpectrum, 1, "Get bin number from frequency...", 0, 2, DO_Spectrum_getBinFromFrequency);
						praat_addAction1 (classSpectrum, 1, "Get bin from frequency...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getBinFromFrequency);
		praat_addAction1 (classSpectrum, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, "Get real value in bin...", 0, 1, DO_Spectrum_getRealValueInBin);
		praat_addAction1 (classSpectrum, 1, "Get imaginary value in bin...", 0, 1, DO_Spectrum_getImaginaryValueInBin);
		praat_addAction1 (classSpectrum, 1, "-- get energy --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, "Get band energy...", 0, 1, DO_Spectrum_getBandEnergy);
		praat_addAction1 (classSpectrum, 1, "Get band density...", 0, 1, DO_Spectrum_getBandDensity);
		praat_addAction1 (classSpectrum, 1, "Get band energy difference...", 0, 1, DO_Spectrum_getBandEnergyDifference);
		praat_addAction1 (classSpectrum, 1, "Get band density difference...", 0, 1, DO_Spectrum_getBandDensityDifference);
		praat_addAction1 (classSpectrum, 1, "-- get moments --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, "Get centre of gravity...", 0, 1, DO_Spectrum_getCentreOfGravity);
		praat_addAction1 (classSpectrum, 1, "Get standard deviation...", 0, 1, DO_Spectrum_getStandardDeviation);
		praat_addAction1 (classSpectrum, 1, "Get skewness...", 0, 1, DO_Spectrum_getSkewness);
		praat_addAction1 (classSpectrum, 1, "Get kurtosis...", 0, 1, DO_Spectrum_getKurtosis);
		praat_addAction1 (classSpectrum, 1, "Get central moment...", 0, 1, DO_Spectrum_getCentralMoment);
	praat_addAction1 (classSpectrum, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "Formula...", 0, 1, DO_Spectrum_formula);
		praat_addAction1 (classSpectrum, 0, "Filter (pass Hann band)...", 0, 1, DO_Spectrum_passHannBand);
		praat_addAction1 (classSpectrum, 0, "Filter (stop Hann band)...", 0, 1, DO_Spectrum_stopHannBand);
	praat_addAction1 (classSpectrum, 0, "Analyse", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "To Excitation...", 0, 0, DO_Spectrum_to_Excitation);
		praat_addAction1 (classSpectrum, 0, "To Formant (peaks)...", 0, 0, DO_Spectrum_to_Formant_peaks);
		praat_addAction1 (classSpectrum, 0, "To Ltas...", 0, 0, DO_Spectrum_to_Ltas);
		praat_addAction1 (classSpectrum, 0, "To Ltas (1-to-1)", 0, 0, DO_Spectrum_to_Ltas_1to1);
		praat_addAction1 (classSpectrum, 0, "To Spectrogram", 0, 0, DO_Spectrum_to_Spectrogram);
	praat_addAction1 (classSpectrum, 0, "Convert", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "Cepstral smoothing...", 0, 0, DO_Spectrum_cepstralSmoothing);
		praat_addAction1 (classSpectrum, 0, "LPC smoothing...", 0, 0, DO_Spectrum_lpcSmoothing);
	praat_addAction1 (classSpectrum, 0, "Synthesize", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "To Sound", 0, 0, DO_Spectrum_to_Sound);
						praat_addAction1 (classSpectrum, 0, "To Sound (fft)", 0, praat_HIDDEN, DO_Spectrum_to_Sound);
	praat_addAction1 (classSpectrum, 0, "Hack", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, "To Matrix", 0, 0, DO_Spectrum_to_Matrix);

	praat_addAction1 (classSpellingChecker, 1, "Edit...", 0, 0, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 0, "Query", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 1, "Is word allowed...", 0, 0, DO_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, "Next not allowed word...", 0, 0, DO_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, "Add new word...", 0, 0, DO_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, "Analyze", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, "Extract WordList", 0, 0, DO_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, "Extract user dictionary", 0, 0, DO_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classStrings, 0, "Strings help", 0, 0, DO_Strings_help);
	praat_addAction1 (classStrings, 1, "Write to raw text file...", 0, 0, DO_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 0, "Query", 0, 0, 0);
		praat_addAction1 (classStrings, 2, "Equal?", 0, 0, DO_Strings_equal);
		praat_addAction1 (classStrings, 1, "Get number of strings", 0, 0, DO_Strings_getNumberOfStrings);
		praat_addAction1 (classStrings, 1, "Get string...", 0, 0, DO_Strings_getString);
	praat_addAction1 (classStrings, 0, "Modify", 0, 0, 0);
		praat_addAction1 (classStrings, 0, "Randomize", 0, 0, DO_Strings_randomize);
		praat_addAction1 (classStrings, 0, "Sort", 0, 0, DO_Strings_sort);
		praat_addAction1 (classStrings, 0, "Genericize", 0, 0, DO_Strings_genericize);
	praat_addAction1 (classStrings, 0, "Analyze", 0, 0, 0);
		praat_addAction1 (classStrings, 0, "To Distributions", 0, 0, DO_Strings_to_Distributions);
	praat_addAction1 (classStrings, 0, "Synthesize", 0, 0, 0);
		praat_addAction1 (classStrings, 0, "To WordList", 0, 0, DO_Strings_to_WordList);

	praat_addAction1 (classTableOfReal, 0, "TableOfReal help", 0, 0, DO_TableOfReal_help);
	praat_TableOfReal_init (classTableOfReal);

	praat_addAction1 (classTextGrid, 0, "TextGrid help", 0, 0, DO_TextGrid_help);
	praat_addAction1 (classTextGrid, 0, "Write to chronological text file...", 0, 0, DO_TextGrid_writeToChronologicalTextFile);
praat_addAction1 (classTextGrid, 0, "Edit", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, "& Sound: Edit?", 0, 0, DO_info_TextGrid_Sound_edit);
praat_addAction1 (classTextGrid, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, "Draw...", 0, 0, DO_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, "& Sound: Draw?", 0, 0, DO_info_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, "& Pitch: Draw?", 0, 0, DO_info_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 0, "Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, "-- query tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of tiers", 0, 1, DO_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, "Get tier name...", 0, 1, DO_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, "Is interval tier...", 0, 1, DO_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, "-- query intervals --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of intervals...", 0, 1, DO_TextGrid_getNumberOfIntervals);
		praat_addAction1 (classTextGrid, 1, "Get starting point...", 0, 1, DO_TextGrid_getStartingPoint);
		praat_addAction1 (classTextGrid, 1, "Get end point...", 0, 1, DO_TextGrid_getEndPoint);
		praat_addAction1 (classTextGrid, 1, "Get label of interval...", 0, 1, DO_TextGrid_getLabelOfInterval);
		praat_addAction1 (classTextGrid, 1, "Get interval at time...", 0, 1, DO_TextGrid_getIntervalAtTime);
		praat_addAction1 (classTextGrid, 1, "-- query points --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of points...", 0, 1, DO_TextGrid_getNumberOfPoints);
		praat_addAction1 (classTextGrid, 1, "Get time of point...", 0, 1, DO_TextGrid_getTimeOfPoint);
		praat_addAction1 (classTextGrid, 1, "Get label of point...", 0, 1, DO_TextGrid_getLabelOfPoint);
		praat_addAction1 (classTextGrid, 1, "-- query labels --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Count labels...", 0, 1, DO_TextGrid_countLabels);
	praat_addAction1 (classTextGrid, 0, "Modify -        ", 0, 0, 0);
		praat_addAction1 (classTextGrid, 0, "Genericize", 0, 1, DO_TextGrid_genericize);
		praat_addAction1 (classTextGrid, 0, "Nativize", 0, 1, DO_TextGrid_nativize);
		praat_addAction1 (classTextGrid, 0, "Shift times...", 0, 1, DO_TextGrid_shiftTimes);
		praat_addAction1 (classTextGrid, 0, "Shift to zero", 0, 1, DO_TextGrid_shiftToZero);
		praat_addAction1 (classTextGrid, 0, "Scale times...", 0, 1, DO_TextGrid_scaleTimes);
		praat_addAction1 (classTextGrid, 0, "-- modify intervals --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, "Insert boundary...", 0, 1, DO_TextGrid_insertBoundary);
		praat_addAction1 (classTextGrid, 0, "Remove boundary at time...", 0, 1, DO_TextGrid_removeBoundaryAtTime);
		praat_addAction1 (classTextGrid, 0, "Set interval text...", 0, 1, DO_TextGrid_setIntervalText);
		praat_addAction1 (classTextGrid, 0, "-- modify points --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, "Insert point...", 0, 1, DO_TextGrid_insertPoint);
		praat_addAction1 (classTextGrid, 0, "Set point text...", 0, 1, DO_TextGrid_setPointText);
		praat_addAction1 (classTextGrid, 0, "-- modify tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, "Insert interval tier...", 0, 1, DO_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, "Insert point tier...", 0, 1, DO_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, "Duplicate tier...", 0, 1, DO_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, "Remove tier...", 0, 1, DO_TextGrid_removeTier);
praat_addAction1 (classTextGrid, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, "Extract tier...", 0, 0, DO_TextGrid_extractTier);
	praat_addAction1 (classTextGrid, 1, "Extract part...", 0, 0, DO_TextGrid_extractPart);
praat_addAction1 (classTextGrid, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, "Merge", 0, 0, DO_TextGrids_merge);

	praat_addAction1 (classTextTier, 0, "TextTier help", 0, 0, DO_TextTier_help);
	praat_addAction1 (classTextTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classTextTier);
	praat_addAction1 (classTextTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classTextTier);
		praat_addAction1 (classTextTier, 0, "Add point...", 0, 1, DO_TextTier_addPoint);
praat_addAction1 (classTextTier, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Get points...", 0, 0, DO_TextTier_getPoints);
praat_addAction1 (classTextTier, 0, "Collect", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
praat_addAction1 (classTextTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Down to PointProcess", 0, 0, DO_TextTier_downto_PointProcess);
	praat_addAction1 (classTextTier, 0, "Down to TableOfReal (any)", 0, 0, DO_TextTier_downto_TableOfReal_any);
	praat_addAction1 (classTextTier, 0, "Down to TableOfReal...", 0, 0, DO_TextTier_downto_TableOfReal);

	praat_addAction1 (classTransition, 0, "Transition help", 0, 0, DO_Transition_help);
praat_addAction1 (classTransition, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classTransition, 0, "Draw as numbers...", 0, 0, DO_Transition_drawAsNumbers);
praat_addAction1 (classTransition, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classTransition, 0, "Eigen", 0, 0, DO_Transition_eigen);
	praat_addAction1 (classTransition, 0, "Conflate", 0, 0, DO_Transition_conflate);
praat_addAction1 (classTransition, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classTransition, 0, "Power...", 0, 0, DO_Transition_power);
praat_addAction1 (classTransition, 0, "Cast", 0, 0, 0);
	praat_addAction1 (classTransition, 0, "To Matrix", 0, 0, DO_Transition_to_Matrix);

	praat_addAction1 (classWordList, 0, "Query", 0, 0, 0);
		praat_addAction1 (classWordList, 1, "Has word...", 0, 0, DO_WordList_hasWord);
	praat_addAction1 (classWordList, 0, "Analyze", 0, 0, 0);
		praat_addAction1 (classWordList, 0, "To Strings", 0, 0, DO_WordList_to_Strings);
	praat_addAction1 (classWordList, 0, "Synthesize", 0, 0, 0);
		praat_addAction1 (classWordList, 0, "Up to SpellingChecker", 0, 0, DO_WordList_upto_SpellingChecker);

	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, "Edit", 0, 0, DO_AmplitudeTier_edit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, "Multiply", 0, 0, DO_Sound_AmplitudeTier_multiply);
	praat_addAction2 (classDistributions, 1, classTransition, 1, "Map", 0, 0, DO_Distributions_Transition_map);
	praat_addAction2 (classDistributions, 1, classTransition, 1, "To Transition...", 0, 0, DO_Distributions_to_Transition_adj);
	praat_addAction2 (classDistributions, 2, classTransition, 1, "To Transition (noise)...", 0, 0, DO_Distributions_to_Transition_noise_adj);
	praat_addAction2 (classDurationTier, 1, classSound, 1, "Edit", 0, 0, DO_DurationTier_edit);
	praat_addAction2 (classFormant, 1, classPointProcess, 1, "To FormantTier", 0, 0, DO_Formant_PointProcess_to_FormantTier);
	praat_addAction2 (classFormant, 1, classSound, 1, "Filter", 0, 0, DO_Sound_Formant_filter);
	praat_addAction2 (classFormant, 1, classSound, 1, "Filter (no scale)", 0, 0, DO_Sound_Formant_filter_noscale);
	praat_addAction2 (classFormantTier, 1, classSound, 1, "Filter", 0, 0, DO_Sound_FormantTier_filter);
	praat_addAction2 (classFormantTier, 1, classSound, 1, "Filter (no scale)", 0, 0, DO_Sound_FormantTier_filter_noscale);
	praat_addAction2 (classIntensity, 1, classPitch, 1, "Draw", 0, 0, 0);
	praat_addAction2 (classIntensity, 1, classPitch, 1, "Draw (phonetogram)...", 0, 0, DO_Pitch_Intensity_draw);
	praat_addAction2 (classIntensity, 1, classPitch, 1, "Speckle (phonetogram)...", 0, praat_HIDDEN, DO_Pitch_Intensity_speckle);   /* grandfathered 2005 */
	praat_addAction2 (classIntensity, 1, classPointProcess, 1, "To IntensityTier", 0, 0, DO_Intensity_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classPointProcess, 1, "To IntensityTier", 0, 0, DO_IntensityTier_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, "Edit", 0, 0, DO_IntensityTier_edit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, "Multiply", 0, praat_HIDDEN, DO_Sound_IntensityTier_multiply_old);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, "Multiply...", 0, 0, DO_Sound_IntensityTier_multiply);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, "Start to centre...", 0, 0, DO_IntervalTier_PointProcess_startToCentre);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, "End to centre...", 0, 0, DO_IntervalTier_PointProcess_endToCentre);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, "Collect", 0, 0, 0);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction2 (classLabel, 1, classSound, 1, "To TextGrid", 0, 0, DO_Label_Sound_to_TextGrid);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_LongSound_edit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, "Scale times", 0, 0, DO_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classManipulation, 1, classSound, 1, "Replace original sound", 0, 0, DO_Manipulation_replaceOriginalSound);
	praat_addAction2 (classManipulation, 1, classPointProcess, 1, "Replace pulses", 0, 0, DO_Manipulation_replacePulses);
	praat_addAction2 (classManipulation, 1, classPitchTier, 1, "Replace pitch tier", 0, 0, DO_Manipulation_replacePitchTier);
	praat_addAction2 (classManipulation, 1, classDurationTier, 1, "Replace duration tier", 0, 0, DO_Manipulation_replaceDurationTier);
	praat_addAction2 (classManipulation, 1, classTextTier, 1, "To Manipulation", 0, 0, DO_Manipulation_TextTier_to_Manipulation);
	praat_addAction2 (classMatrix, 1, classSound, 1, "To ParamCurve", 0, 0, DO_Matrix_to_ParamCurve);
	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, "Get fraction correct...", 0, 0, DO_PairDistribution_Distributions_getFractionCorrect);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, "Draw...", 0, 0, DO_PitchTier_Pitch_draw);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, "To Pitch", 0, 0, DO_Pitch_PitchTier_to_Pitch);
	praat_addAction2 (classPitch, 1, classPointProcess, 1, "To PitchTier", 0, 0, DO_Pitch_PointProcess_to_PitchTier);
	praat_addAction3 (classPitch, 1, classPointProcess, 1, classSound, 1, "Voice report...", 0, 0, DO_Sound_Pitch_PointProcess_voiceReport);
	praat_addAction2 (classPitch, 1, classSound, 1, "To PointProcess (cc)", 0, 0, DO_Sound_Pitch_to_PointProcess_cc);
	praat_addAction2 (classPitch, 1, classSound, 1, "To PointProcess (peaks)...", 0, 0, DO_Sound_Pitch_to_PointProcess_peaks);
	praat_addAction2 (classPitch, 1, classSound, 1, "To Manipulation", 0, 0, DO_Sound_Pitch_to_Manipulation);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw -           ", 0, 0, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw...", 0, 1, DO_TextGrid_Pitch_draw);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw logarithmic...", 0, 1, DO_TextGrid_Pitch_drawLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw semitones...", 0, 1, DO_TextGrid_Pitch_drawSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw mel...", 0, 1, DO_TextGrid_Pitch_drawMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw erb...", 0, 1, DO_TextGrid_Pitch_drawErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle...", 0, 1, DO_TextGrid_Pitch_speckle);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle logarithmic...", 0, 1, DO_TextGrid_Pitch_speckleLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle semitones...", 0, 1, DO_TextGrid_Pitch_speckleSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle mel...", 0, 1, DO_TextGrid_Pitch_speckleMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle erb...", 0, 1, DO_TextGrid_Pitch_speckleErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "-- draw separately --", 0, 1, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately...", 0, 1, DO_TextGrid_Pitch_drawSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (semitones)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (mel)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (erb)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately...", 0, 1, DO_TextGrid_Pitch_speckleSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (semitones)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (mel)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (erb)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextTier, 1, "To PitchTier...", 0, 0, DO_Pitch_TextTier_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classPointProcess, 1, "To PitchTier", 0, 0, DO_PitchTier_PointProcess_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classSound, 1, "Edit", 0, 0, DO_PitchTier_edit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Edit", 0, 0, DO_PointProcess_edit);
praat_addAction2 (classPointProcess, 1, classSound, 1, "Query", 0, 0, 0);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (local)...", 0, 0, DO_Point_Sound_getShimmer_local);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (local_dB)...", 0, 0, DO_Point_Sound_getShimmer_local_dB);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (apq3)...", 0, 0, DO_Point_Sound_getShimmer_apq3);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (apq5)...", 0, 0, DO_Point_Sound_getShimmer_apq5);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (apq11)...", 0, 0, DO_Point_Sound_getShimmer_apq11);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Get shimmer (dda)...", 0, 0, DO_Point_Sound_getShimmer_dda);
praat_addAction2 (classPointProcess, 1, classSound, 1, "Modify", 0, 0, 0);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "Transplant time domain", 0, 0, DO_Point_Sound_transplantDomain);
praat_addAction2 (classPointProcess, 1, classSound, 1, "Analyse", 0, 0, 0);
	/*praat_addAction2 (classPointProcess, 1, classSound, 1, "To Manipulation", 0, 0, DO_Sound_PointProcess_to_Manipulation);*/
	praat_addAction2 (classPointProcess, 1, classSound, 1, "To AmplitudeTier (point)", 0, 0, DO_PointProcess_Sound_to_AmplitudeTier_point);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "To AmplitudeTier (period)...", 0, 0, DO_PointProcess_Sound_to_AmplitudeTier_period);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "To Ltas...", 0, 0, DO_PointProcess_Sound_to_Ltas);
	praat_addAction2 (classPointProcess, 1, classSound, 1, "To Ltas (only harmonics)...", 0, 0, DO_PointProcess_Sound_to_Ltas_harmonics);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Draw...", 0, 0, DO_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract -       ", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract all intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractAllIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract non-empty intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractNonemptyIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract intervals...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_TextGrid_Sound_extractIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract intervals where...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractIntervalsWhere);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Modify TextGrid", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Scale times", 0, 0, DO_TextGrid_Sound_scaleTimes);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Modify Sound", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Clone time domain", 0, 0, DO_TextGrid_Sound_cloneTimeDomain);
	praat_addAction2 (classSpellingChecker, 1, classWordList, 1, "Replace WordList", 0, 0, DO_SpellingChecker_replaceWordList);
	praat_addAction2 (classSpellingChecker, 1, classSortedSetOfString, 1, "Replace user dictionary", 0, 0, DO_SpellingChecker_replaceUserDictionary);
	praat_addAction2 (classSpellingChecker, 1, classStrings, 1, "Replace word list?", 0, 0, DO_SpellingChecker_replaceWordList_help);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, "Append", 0, 0, DO_TextGrid_AnyTier_append);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, "Append", 0, 0, DO_TextGrid_AnyTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);

	praat_addAction4 (classDurationTier, 1, classPitchTier, 1, classPointProcess, 1, classSound, 1, "To Sound...", 0, 0, DO_Sound_Point_Pitch_Duration_to_Sound);

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
	INCLUDE_MANPAGES (manual_OT_init)
	INCLUDE_MANPAGES (manual_Exp_init)

	INCLUDE_LIBRARY (praat_uvafon_OT_init)
	INCLUDE_LIBRARY (praat_uvafon_Artsynth_init)
	INCLUDE_LIBRARY (praat_uvafon_David_init)
	INCLUDE_LIBRARY (praat_uvafon_FFNet_init)
	INCLUDE_LIBRARY (praat_uvafon_LPC_init)
	INCLUDE_LIBRARY (praat_uvafon_Stat_init)
	INCLUDE_LIBRARY (praat_uvafon_Exp_init)
}

/* End of file praat_Fon.c */
