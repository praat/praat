/* praat_Fon.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "praat.h"

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
#include "MovieWindow.h"
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
#include "StringsEditor.h"
#include "TableEditor.h"
#include "TextGrid.h"
#include "VocalTract.h"
#include "VoiceAnalysis.h"
#include "WordList.h"

#undef iam
#define iam iam_LOOP

static const wchar_t *STRING_FROM_TIME_SECONDS = L"left Time range (s)";
static const wchar_t *STRING_TO_TIME_SECONDS = L"right Time range (s)";
static const wchar_t *STRING_FROM_TIME = L"left Time range";
static const wchar_t *STRING_TO_TIME = L"right Time range";
static const wchar_t *STRING_FROM_FREQUENCY_HZ = L"left Frequency range (Hz)";
static const wchar_t *STRING_TO_FREQUENCY_HZ = L"right Frequency range (Hz)";
static const wchar_t *STRING_FROM_FREQUENCY = L"left Frequency range";
static const wchar_t *STRING_TO_FREQUENCY = L"right Frequency range";

/***** Common dialog contents. *****/

extern "C" void praat_dia_timeRange (Any dia);
extern "C" void praat_dia_timeRange (Any dia) {
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
}
extern "C" void praat_get_timeRange (Any dia, double *tmin, double *tmax);
extern "C" void praat_get_timeRange (Any dia, double *tmin, double *tmax) {
	*tmin = GET_REAL (STRING_FROM_TIME);
	*tmax = GET_REAL (STRING_TO_TIME);
}
extern "C" int praat_get_frequencyRange (Any dia, double *fmin, double *fmax);
extern "C" int praat_get_frequencyRange (Any dia, double *fmin, double *fmax) {
	*fmin = GET_REAL (STRING_FROM_FREQUENCY);
	*fmax = GET_REAL (STRING_TO_FREQUENCY);
	REQUIRE (*fmax > *fmin, L"Maximum frequency must be greater than minimum frequency.")
	return 1;
}
static void dia_Vector_getExtremum (Any dia) {
	Any radio;
	praat_dia_timeRange (dia);
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
}
static void dia_Vector_getValue (Any dia) {
	Any radio;
	REAL (L"Time (s)", L"0.5")
	RADIO (L"Interpolation", 3)
	RADIOBUTTON (L"Nearest")
	RADIOBUTTON (L"Linear")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
}

static void getTminTmaxFminFmax (Any dia, double *tmin, double *tmax, double *fmin, double *fmax) {
	*tmin = GET_REAL (STRING_FROM_TIME);
	*tmax = GET_REAL (STRING_TO_TIME);
	*fmin = GET_REAL (STRING_FROM_FREQUENCY);
	*fmax = GET_REAL (STRING_TO_FREQUENCY);
	REQUIRE (*fmax > *fmin, L"Maximum frequency must be greater than minimum frequency.")
}
#define GET_TMIN_TMAX_FMIN_FMAX \
	double tmin, tmax, fmin, fmax; \
	getTminTmaxFminFmax (dia, & tmin, & tmax, & fmin, & fmax);

/***** Two auxiliary routines, exported. *****/

int praat_Fon_formula (UiForm dia, Interpreter interpreter);
int praat_Fon_formula (UiForm dia, Interpreter interpreter) {
	int IOBJECT;
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
	return 1;
}

extern "C" Graphics Movie_create (const wchar_t *title, int width, int height);
extern "C" Graphics Movie_create (const wchar_t *title, int width, int height) {
	static Graphics graphics;
	static GuiObject dialog, drawingArea;
	if (! graphics) {
		dialog = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width + 2, height + 2, title, NULL, NULL, 0);
		drawingArea = GuiDrawingArea_createShown (dialog, 0, width, 0, height, NULL, NULL, NULL, NULL, NULL, 0);
		GuiObject_show (dialog);
		graphics = Graphics_create_xmdrawingarea (drawingArea);
	}
	GuiWindow_setTitle (GuiObject_parent (dialog), title);
	GuiObject_size (GuiObject_parent (dialog), width + 2, height + 2);
	GuiObject_size (drawingArea, width, height);
	GuiObject_show (dialog);
	return graphics;
}

/***** AMPLITUDETIER *****/

FORM (AmplitudeTier_addPoint, L"Add one point", L"AmplitudeTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	REAL (L"Sound pressure (Pa)", L"0.8")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		RealTier_addPoint (me, GET_REAL (L"Time"), GET_REAL (L"Sound pressure")); therror
		praat_dataChanged (me);
	}
END

FORM (AmplitudeTier_create, L"Create empty AmplitudeTier", NULL)
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoAmplitudeTier thee = AmplitudeTier_create (startTime, endTime);
	praat_new (thee.transfer(), GET_STRING (L"Name"));
END

DIRECT (AmplitudeTier_downto_PointProcess)
	LOOP {
		iam (AmplitudeTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (AmplitudeTier_downto_TableOfReal)
	LOOP {
		iam (AmplitudeTier);
		autoTableOfReal thee = AmplitudeTier_downto_TableOfReal (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (AmplitudeTier_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit an AmplitudeTier from batch.");
	Sound sound = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay NULL
	}
	LOOP if (CLASS == classAmplitudeTier) {
		iam (AmplitudeTier);
		autoAmplitudeTierEditor editor = AmplitudeTierEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me, sound, TRUE);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (AmplitudeTier_formula, L"AmplitudeTier: Formula", L"AmplitudeTier: Formula...")
	LABEL (L"", L"# ncol = the number of points")
	LABEL (L"", L"for col from 1 to ncol")
	LABEL (L"", L"   # x = the time of the colth point, in seconds")
	LABEL (L"", L"   # self = the value of the colth point, in Pascal")
	LABEL (L"", L"   self = `formula'")
	LABEL (L"", L"endfor")
	TEXTFIELD (L"formula", L"- self ; upside down")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		try {
			RealTier_formula (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

static void dia_AmplitudeTier_getRangeProperty (Any dia) {
	REAL (L"Shortest period (s)", L"0.0001")
	REAL (L"Longest period (s)", L"0.02")
	POSITIVE (L"Maximum amplitude factor", L"1.6")
}

FORM (AmplitudeTier_getShimmer_local, L"AmplitudeTier: Get shimmer (local)", L"AmplitudeTier: Get shimmer (local)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

FORM (AmplitudeTier_getShimmer_local_dB, L"AmplitudeTier: Get shimmer (local, dB)", L"AmplitudeTier: Get shimmer (local, dB)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local_dB (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

FORM (AmplitudeTier_getShimmer_apq3, L"AmplitudeTier: Get shimmer (apq3)", L"AmplitudeTier: Get shimmer (apq3)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq3 (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

FORM (AmplitudeTier_getShimmer_apq5, L"AmplitudeTier: Get shimmer (apq5)", L"AmplitudeTier: Get shimmer (apq5)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq5 (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

FORM (AmplitudeTier_getShimmer_apq11, L"AmplitudeTier: Get shimmer (apq11)", L"AmplitudeTier: Get shimmer (apq11)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq11 (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

FORM (AmplitudeTier_getShimmer_dda, L"AmplitudeTier: Get shimmer (dda)", L"AmplitudeTier: Get shimmer (dda)...")
	dia_AmplitudeTier_getRangeProperty (dia);
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_dda (me,
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum amplitude factor"));
		Melder_informationReal (shimmer, NULL);
	}
END

/*FORM (AmplitudeTier_getValueAtTime, L"Get AmplitudeTier value", L"AmplitudeTier: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL (L"Time")), L"Pa");
END
	
FORM (AmplitudeTier_getValueAtIndex, L"Get AmplitudeTier value", L"AmplitudeTier: Get value at index...")
	INTEGER (L"Point number", L"10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER (L"Point number")), L"Pa");
END*/

DIRECT (AmplitudeTier_help) Melder_help (L"AmplitudeTier"); END

FORM (AmplitudeTier_to_IntensityTier, L"AmplitudeTier: To IntensityTier", L"AmplitudeTier: To IntensityTier...")
	REAL (L"Threshold (dB)", L"-10000.0")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		autoIntensityTier thee = AmplitudeTier_to_IntensityTier (me, GET_REAL (L"Threshold"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (AmplitudeTier_to_Sound, L"AmplitudeTier: To Sound (pulse train)", L"AmplitudeTier: To Sound (pulse train)...")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	NATURAL (L"Interpolation depth (samples)", L"2000")
	OK
DO
	LOOP {
		iam (AmplitudeTier);
		autoSound thee = AmplitudeTier_to_Sound (me, GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Interpolation depth"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (info_AmplitudeTier_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your AmplitudeTier editor:\n"
		"   select an AmplitudeTier and a Sound, and click \"View & Edit\".");
END

/***** AMPLITUDETIER & SOUND *****/

DIRECT (Sound_AmplitudeTier_multiply)
	Sound sound = NULL;
	AmplitudeTier tier = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classAmplitudeTier) tier = (AmplitudeTier) OBJECT;
	}
	autoSound thee = Sound_AmplitudeTier_multiply (sound, tier);
	praat_new (thee.transfer(), sound -> name, L"_amp");
END

/***** COCHLEAGRAM *****/

FORM (Cochleagram_difference, L"Cochleagram difference", 0)
	praat_dia_timeRange (dia);
	OK
DO
	Cochleagram coch1 = NULL, coch2 = NULL;
	LOOP (coch1 ? coch2 : coch1) = (Cochleagram) OBJECT;
	Melder_informationReal (Cochleagram_difference (coch1, coch2,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), L"Hertz (root-mean-square)");
END

FORM (Cochleagram_formula, L"Cochleagram Formula", L"Cochleagram: Formula...")
	LABEL (L"label", L"`x' is time in seconds, `y' is place in Bark")
	LABEL (L"label", L"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (Cochleagram);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Cochleagram may have partially changed
			throw;
		}
	}
END

DIRECT (Cochleagram_help) Melder_help (L"Cochleagram"); END

DIRECT (Cochleagram_movie)
	Graphics g = Movie_create (L"Cochleagram movie", 300, 300);
	LOOP {
		iam (Cochleagram);
		Matrix_movie (me, g);
	}
END

FORM (Cochleagram_paint, L"Paint Cochleagram", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Cochleagram);
		autoPraatPicture picture;
		Cochleagram_paint (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Cochleagram_to_Excitation, L"From Cochleagram to Excitation", 0)
	REAL (L"Time (s)", L"0.0")
	OK
DO
	LOOP {
		iam (Cochleagram);
		autoExcitation thee = Cochleagram_to_Excitation (me, GET_REAL (L"Time"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Cochleagram_to_Matrix)
	LOOP {
		iam (Cochleagram);
		autoMatrix thee = Cochleagram_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** CORPUS *****/

FORM (Corpus_create, L"Create Corpus", L"Create Corpus...")
	WORD (L"Name", L"myCorpus")
	LABEL (L"", L"Folder with sound files:")
	TEXTFIELD (L"folderWithSoundFiles", L"")
	LABEL (L"", L"Folder with annotation files:")
	TEXTFIELD (L"folderWithAnnotationFiles", L"")
	OK
DO
END

DIRECT (Corpus_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot edit a Corpus from batch.");
	LOOP {
		iam (Corpus);
		autoTableEditor editor = TableEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT); therror
	}
END

/***** DISTRIBUTIONS *****/

FORM (Distributions_to_Transition, L"To Transition", 0)
	NATURAL (L"Environment", L"1")
	BOOLEAN (L"Greedy", 1)
	OK
DO
	LOOP {
		iam (Distributions);
		autoTransition thee = Distributions_to_Transition (me, NULL, GET_INTEGER (L"Environment"), NULL, GET_INTEGER (L"Greedy"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Distributions_to_Transition_adj, L"To Transition", 0)
	NATURAL (L"Environment", L"1")
	BOOLEAN (L"Greedy", 1)
	OK
DO
	Distributions dist = NULL;
	Transition trans = NULL;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (dist, NULL, GET_INTEGER (L"Environment"), trans, GET_INTEGER (L"Greedy"));
	praat_new (thee.transfer(), NULL);
END

FORM (Distributions_to_Transition_noise, L"To Transition (noise)", 0)
	NATURAL (L"Environment", L"1")
	BOOLEAN (L"Greedy", 1)
	OK
DO
	Distributions underlying = NULL, surface = NULL;
	LOOP (underlying ? surface : underlying) = (Distributions) OBJECT;
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (L"Environment"), NULL, GET_INTEGER (L"Greedy"));
	praat_new (thee.transfer(), NULL);
END

FORM (Distributions_to_Transition_noise_adj, L"To Transition (noise)", 0)
	NATURAL (L"Environment", L"1")
	BOOLEAN (L"Greedy", 1)
	OK
DO
	Distributions underlying = NULL, surface = NULL;
	Transition trans = NULL;
	LOOP {
		if (CLASS == classDistributions) (underlying ? surface : underlying) = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (L"Environment"), trans, GET_INTEGER (L"Greedy"));
	praat_new (thee.transfer(), NULL);
END

/***** DISTRIBUTIONS & TRANSITION *****/

DIRECT (Distributions_Transition_map)
	Distributions dist = NULL;
	Transition trans = NULL;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoDistributions thee = Distributions_Transition_map (dist, trans);
	praat_new (thee.transfer(), L"surface");
END

/***** DURATIONTIER *****/

FORM (DurationTier_addPoint, L"Add one point to DurationTier", L"DurationTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	REAL (L"Relative duration", L"1.5")
	OK
DO
	LOOP {
		iam (DurationTier);
		RealTier_addPoint (me, GET_REAL (L"Time"), GET_REAL (L"Relative duration"));
		praat_dataChanged (me);
	}
END

FORM (DurationTier_create, L"Create empty DurationTier", L"Create DurationTier...")
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoDurationTier thee = DurationTier_create (startTime, endTime);
	praat_new (thee.transfer(), GET_STRING (L"Name"));
END

DIRECT (DurationTier_downto_PointProcess)
	LOOP {
		iam (DurationTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (OBJECT);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (DurationTier_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a DurationTier from batch.");
	Sound sound = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay NULL
	}
	LOOP if (CLASS == classDurationTier) {
		iam (DurationTier);
		autoDurationTierEditor editor = DurationTierEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me, sound, TRUE);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (DurationTier_formula, L"DurationTier: Formula", L"DurationTier: Formula...")
	LABEL (L"", L"# ncol = the number of points")
	LABEL (L"", L"for col from 1 to ncol")
	LABEL (L"", L"   # x = the time of the colth point, in seconds")
	LABEL (L"", L"   # self = the value of the colth point, in relative units")
	LABEL (L"", L"   self = `formula'")
	LABEL (L"", L"endfor")
	TEXTFIELD (L"formula", L"self * 1.5 ; slow down")
	OK
DO
	LOOP {
		iam (DurationTier);
		try {
			RealTier_formula (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (DurationTier_getTargetDuration, L"Get target duration", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	OK
DO
	LOOP {
		iam (DurationTier);
		double area = RealTier_getArea (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (area, L"seconds");
	}
END

FORM (DurationTier_getValueAtTime, L"Get DurationTier value", L"DurationTier: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (L"Time"));
		Melder_informationReal (value, NULL);
	}
END
	
FORM (DurationTier_getValueAtIndex, L"Get DurationTier value", L"Duration: Get value at index...")
	INTEGER (L"Point number", L"10")
	OK
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (L"Point number"));
		Melder_informationReal (value, NULL);
	}
END

DIRECT (DurationTier_help) Melder_help (L"DurationTier"); END

DIRECT (info_DurationTier_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your DurationTier editor:\n"
		"   select a DurationTier and a Sound, and click \"View & Edit\".");
END

/***** EXCITATION *****/

FORM (Excitation_draw, L"Draw Excitation", 0)
	REAL (L"From frequency (Bark)", L"0")
	REAL (L"To frequency (Bark)", L"25.6")
	REAL (L"Minimum (phon)", L"0")
	REAL (L"Maximum (phon)", L"100")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Excitation);
		autoPraatPicture picture;
		Excitation_draw (me, GRAPHICS,
			GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Excitation_formula, L"Excitation Formula", L"Excitation: Formula...")
	LABEL (L"label", L"`x' is the place in Bark, `col' is the bin number")
	LABEL (L"label", L"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (Excitation);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

DIRECT (Excitation_getLoudness)
	LOOP {
		iam (Excitation);
		double loudness = Excitation_getLoudness (me);
		Melder_informationReal (loudness, L"sones");
	}
END

DIRECT (Excitation_help) Melder_help (L"Excitation"); END

FORM (Excitation_to_Formant, L"From Excitation to Formant", 0)
	NATURAL (L"Maximum number of formants", L"20")
	OK
DO
	LOOP {
		iam (Excitation);
		autoFormant thee = Excitation_to_Formant (me, GET_INTEGER (L"Maximum number of formants"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Excitation_to_Matrix)
	LOOP {
		iam (Excitation);
		autoMatrix thee = Excitation_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** FORMANT *****/

DIRECT (Formant_downto_FormantGrid)
	LOOP {
		iam (Formant);
		autoFormantGrid thee = Formant_downto_FormantGrid (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Formant_downto_FormantTier)
	LOOP {
		iam (Formant);
		autoFormantTier thee = Formant_downto_FormantTier (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Formant_drawSpeckles, L"Draw Formant", L"Formant: Draw speckles...")
	praat_dia_timeRange (dia);
	POSITIVE (L"Maximum frequency (Hz)", L"5500.0")
	REAL (L"Dynamic range (dB)", L"30.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_drawSpeckles (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Maximum frequency"),
			GET_REAL (L"Dynamic range"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Formant_drawTracks, L"Draw formant tracks", L"Formant: Draw tracks...")
	praat_dia_timeRange (dia);
	POSITIVE (L"Maximum frequency (Hz)", L"5500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_drawTracks (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Maximum frequency"),
			GET_INTEGER (L"Garnish"));
	}
END

FORM (Formant_formula_bandwidths, L"Formant: Formula (bandwidths)", L"Formant: Formula (bandwidths)...")
	LABEL (L"", L"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	TEXTFIELD (L"formula", L"self / 2 ; sharpen all peaks")
	OK
DO
	LOOP {
		iam (Formant);
		try {
			Formant_formula_bandwidths (me, GET_STRING (L"formula"), interpreter); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Formant may have partially changed
			throw;
		}
	}
END

FORM (Formant_formula_frequencies, L"Formant: Formula (frequencies)", L"Formant: Formula (frequencies)...")
	LABEL (L"", L"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (L"formula", L"if row = 2 then self + 200 else self fi")
	OK
DO
	LOOP {
		iam (Formant);
		try {
			Formant_formula_frequencies (me, GET_STRING (L"formula"), interpreter); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Formant may have partially changed
			throw;
		}
	}
END

FORM (Formant_getBandwidthAtTime, L"Formant: Get bandwidth", L"Formant: Get bandwidth at time...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 1)
	RADIOBUTTON (L"Linear")
	OK
DO
	LOOP {
		iam (Formant);
		double bandwidth = Formant_getBandwidthAtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (bandwidth, GET_STRING (L"Unit"));
		break;
	}
END
	
FORM (Formant_getMaximum, L"Formant: Get maximum", L"Formant: Get maximum...")
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double maximum = Formant_getMaximum (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (maximum, GET_STRING (L"Unit"));
		break;
	}
END

DIRECT (Formant_getMaximumNumberOfFormants)
	LOOP {
		iam (Formant);
		long maximumNumberOfFormants = Formant_getMaxNumFormants (me);
		Melder_information (Melder_integer (maximumNumberOfFormants), L" (there are at most this many formants in every frame)");
		break;
	}
END

FORM (Formant_getMean, L"Formant: Get mean", L"Formant: Get mean...")
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	OK
DO
	LOOP {
		iam (Formant);
		double mean = Formant_getMean (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (mean, GET_STRING (L"Unit"));
		break;
	}
END

FORM (Formant_getMinimum, L"Formant: Get minimum", L"Formant: Get minimum...")
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double minimum = Formant_getMinimum (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1,
			GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (minimum, GET_STRING (L"Unit"));
		break;
	}
END

DIRECT (Formant_getMinimumNumberOfFormants)
	LOOP {
		iam (Formant);
		long minimumNumberOfFormants = Formant_getMinNumFormants (me);
		Melder_information (Melder_integer (minimumNumberOfFormants), L" (there are at least this many formants in every frame)");
		break;
	}
END

FORM (Formant_getNumberOfFormants, L"Formant: Get number of formants", L"Formant: Get number of formants...")
	NATURAL (L"Frame number", L"1")
	OK
DO
	LOOP {
		iam (Formant);
		long frame = GET_INTEGER (L"Frame number");
		if (frame > my nx) Melder_throw ("There is no frame ", frame, " in a Formant with only ", my nx, " frames.");
		Melder_information (Melder_integer (my d_frames [frame]. nFormants), L" formants");
		break;
	}
END

FORM (Formant_getQuantile, L"Formant: Get quantile", 0)
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	REAL (L"Quantile", L"0.50 (= median)")
	OK
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantile (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"Quantile"), GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (L"Unit"));
		break;
	}
END

FORM (Formant_getQuantileOfBandwidth, L"Formant: Get quantile of bandwidth", 0)
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	REAL (L"Quantile", L"0.50 (= median)")
	OK
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantileOfBandwidth (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"Quantile"), GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (L"Unit"));
		break;
	}
END

FORM (Formant_getStandardDeviation, L"Formant: Get standard deviation", 0)
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	OK
DO
	LOOP {
		iam (Formant);
		double stdev = Formant_getStandardDeviation (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (stdev, GET_STRING (L"Unit"));
		break;
	}
END

FORM (Formant_getTimeOfMaximum, L"Formant: Get time of maximum", L"Formant: Get time of maximum...")
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMaximum (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"Unit") - 1, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
		break;
	}
END

FORM (Formant_getTimeOfMinimum, L"Formant: Get time of minimum", L"Formant: Get time of minimum...")
	NATURAL (L"Formant number", L"1")
	praat_dia_timeRange (dia);
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMinimum (me, GET_INTEGER (L"Formant number"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"Unit") - 1, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
		break;
	}
END

FORM (Formant_getValueAtTime, L"Formant: Get value", L"Formant: Get value at time...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Bark")
	RADIO (L"Interpolation", 1)
	RADIOBUTTON (L"Linear")
	OK
DO
	LOOP {
		iam (Formant);
		double value = Formant_getValueAtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), GET_INTEGER (L"Unit") - 1);
		Melder_informationReal (value, GET_STRING (L"Unit"));
		break;
	}
END
	
DIRECT (Formant_help) Melder_help (L"Formant"); END

FORM (Formant_downto_Table, L"Formant: Down to Table", 0)
	BOOLEAN (L"Include frame number", false)
	BOOLEAN (L"Include time", true)
	NATURAL (L"Time decimals", L"6")
	BOOLEAN (L"Include intensity", false)
	NATURAL (L"Intensity decimals", L"3")
	BOOLEAN (L"Include number of formants", true)
	NATURAL (L"Frequency decimals", L"3")
	BOOLEAN (L"Include bandwidths", true)
	OK
DO
	LOOP {
		iam (Formant);
		autoTable thee = Formant_downto_Table (me, GET_INTEGER (L"Include frame number"),
			GET_INTEGER (L"Include time"), GET_INTEGER (L"Time decimals"),
			GET_INTEGER (L"Include intensity"), GET_INTEGER (L"Intensity decimals"),
			GET_INTEGER (L"Include number of formants"), GET_INTEGER (L"Frequency decimals"),
			GET_INTEGER (L"Include bandwidths"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Formant_list, L"Formant: List", 0)
	BOOLEAN (L"Include frame number", false)
	BOOLEAN (L"Include time", true)
	NATURAL (L"Time decimals", L"6")
	BOOLEAN (L"Include intensity", false)
	NATURAL (L"Intensity decimals", L"3")
	BOOLEAN (L"Include number of formants", true)
	NATURAL (L"Frequency decimals", L"3")
	BOOLEAN (L"Include bandwidths", true)
	OK
DO
	LOOP {
		iam (Formant);
		Formant_list (me, GET_INTEGER (L"Include frame number"),
			GET_INTEGER (L"Include time"), GET_INTEGER (L"Time decimals"),
			GET_INTEGER (L"Include intensity"), GET_INTEGER (L"Intensity decimals"),
			GET_INTEGER (L"Include number of formants"), GET_INTEGER (L"Frequency decimals"),
			GET_INTEGER (L"Include bandwidths"));
		break;
	}
END

FORM (Formant_scatterPlot, L"Formant: Scatter plot", 0)
	praat_dia_timeRange (dia);
	NATURAL (L"Horizontal formant number", L"2")
	REAL (L"left Horizontal range (Hz)", L"3000")
	REAL (L"right Horizontal range (Hz)", L"400")
	NATURAL (L"Vertical formant number", L"1")
	REAL (L"left Vertical range (Hz)", L"1500")
	REAL (L"right Vertical range (Hz)", L"100")
	POSITIVE (L"Mark size (mm)", L"1.0")
	BOOLEAN (L"Garnish", 1)
	SENTENCE (L"Mark string (+xo.)", L"+")
	OK
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_scatterPlot (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_INTEGER (L"Horizontal formant number"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_INTEGER (L"Vertical formant number"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Formant_sort)
	LOOP {
		iam (Formant);
		Formant_sort (me);
		praat_dataChanged (me);
	}
END

FORM (Formant_to_Matrix, L"From Formant to Matrix", 0)
	INTEGER (L"Formant", L"1")
	OK
DO
	LOOP {
		iam (Formant);
		autoMatrix thee = Formant_to_Matrix (me, GET_INTEGER (L"Formant"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Formant_tracker, L"Formant tracker", L"Formant: Track...")
	NATURAL (L"Number of tracks (1-5)", L"3")
	REAL (L"Reference F1 (Hz)", L"550")
	REAL (L"Reference F2 (Hz)", L"1650")
	REAL (L"Reference F3 (Hz)", L"2750")
	REAL (L"Reference F4 (Hz)", L"3850")
	REAL (L"Reference F5 (Hz)", L"4950")
	REAL (L"Frequency cost (/kHz)", L"1.0")
	REAL (L"Bandwidth cost", L"1.0")
	REAL (L"Transition cost (/octave)", L"1.0")
	OK
DO
	long numberOfTracks = GET_INTEGER (L"Number of tracks");
	if (numberOfTracks > 5) Melder_throw ("Number of tracks cannot be more than 5.");
	LOOP {
		iam (Formant);
		autoFormant thee = Formant_tracker (me, GET_INTEGER (L"Number of tracks"),
			GET_REAL (L"Reference F1"), GET_REAL (L"Reference F2"),
			GET_REAL (L"Reference F3"), GET_REAL (L"Reference F4"),
			GET_REAL (L"Reference F5"), GET_REAL (L"Frequency cost"),
			GET_REAL (L"Bandwidth cost"), GET_REAL (L"Transition cost"));
		praat_new (thee.transfer(), my name);
	}
END

/***** FORMANT & POINTPROCESS *****/

DIRECT (Formant_PointProcess_to_FormantTier)
	Formant formant = NULL;
	PointProcess point = NULL;
	autoFormantTier thee = Formant_PointProcess_to_FormantTier (formant, point);
	praat_new (thee.transfer(), formant -> name, L"_", point -> name);
END

/***** FORMANT & SOUND *****/

DIRECT (Sound_Formant_filter)
	Sound sound = NULL;
	Formant formant = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter (sound, formant);
	praat_new (thee.transfer(), sound -> name, L"_filt");
END

DIRECT (Sound_Formant_filter_noscale)
	Sound sound = NULL;
	Formant formant = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter_noscale (sound, formant);
	praat_new (thee.transfer(), sound -> name, L"_filt");
END

/***** FORMANTGRID *****/

FORM (FormantGrid_addBandwidthPoint, L"FormantGrid: Add bandwidth point", L"FormantGrid: Add bandwidth point...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	POSITIVE (L"Bandwidth (Hz)", L"100")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addBandwidthPoint (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), GET_REAL (L"Bandwidth")); therror
		praat_dataChanged (me);
	}
END

FORM (FormantGrid_addFormantPoint, L"FormantGrid: Add formant point", L"FormantGrid: Add formant point...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	POSITIVE (L"Frequency (Hz)", L"550")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addFormantPoint (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"), GET_REAL (L"Frequency")); therror
		praat_dataChanged (OBJECT);
	}
END

FORM (FormantGrid_create, L"Create FormantGrid", NULL)
	WORD (L"Name", L"schwa")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	NATURAL (L"Number of formants", L"10")
	POSITIVE (L"Initial first formant (Hz)", L"550")
	POSITIVE (L"Initial formant spacing (Hz)", L"1100")
	REAL (L"Initial first bandwidth (Hz)", L"60")
	REAL (L"Initial bandwidth spacing (Hz)", L"50")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoFormantGrid thee = FormantGrid_create (startTime, endTime, GET_INTEGER (L"Number of formants"),
		GET_REAL (L"Initial first formant"), GET_REAL (L"Initial formant spacing"),
		GET_REAL (L"Initial first bandwidth"), GET_REAL (L"Initial bandwidth spacing"));
	praat_new (thee.transfer(), GET_STRING (L"Name"));
END

static void cb_FormantGridEditor_publish (Editor me, void *closure, Data publish) {
	(void) me;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publish, L"fromFormantGridEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (FormantGrid_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a FormantGrid from batch.");
	LOOP {
		iam (FormantGrid);
		autoFormantGridEditor editor = FormantGridEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_FormantGridEditor_publish, NULL);
		praat_installEditor (editor.transfer(), IOBJECT); therror
	}
END

FORM (FormantGrid_formula_bandwidths, L"FormantGrid: Formula (bandwidths)", L"Formant: Formula (bandwidths)...")
	LABEL (L"", L"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	LABEL (L"", L"self [] is the FormantGrid itself, so it returns frequencies, not bandwidths!")
	TEXTFIELD (L"formula", L"self / 10 ; one tenth of the formant frequency")
	OK
DO
	LOOP {
		iam (FormantGrid);
		try {
			FormantGrid_formula_bandwidths (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the FormantGrid may have partially changed
			throw;
		}
	}
END

FORM (FormantGrid_formula_frequencies, L"FormantGrid: Formula (frequencies)", L"Formant: Formula (frequencies)...")
	LABEL (L"", L"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (L"formula", L"if row = 2 then self + 200 else self fi")
	OK
DO
	LOOP {
		iam (FormantGrid);
		try {
			FormantGrid_formula_frequencies (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the FormantGrid may have partially changed
			throw;
		}
	}
END

DIRECT (FormantGrid_help) Melder_help (L"FormantGrid"); END

FORM (FormantGrid_removeBandwidthPointsBetween, L"Remove bandwidth points between", L"FormantGrid: Remove bandwidth points between...")
	NATURAL (L"Formant number", L"1")
	REAL (L"From time (s)", L"0.3")
	REAL (L"To time (s)", L"0.7")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeBandwidthPointsBetween (me, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time"));
		praat_dataChanged (me);
	}
END

FORM (FormantGrid_removeFormantPointsBetween, L"Remove formant points between", L"FormantGrid: Remove formant points between...")
	NATURAL (L"Formant number", L"1")
	REAL (L"From time (s)", L"0.3")
	REAL (L"To time (s)", L"0.7")
	OK
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeFormantPointsBetween (me, GET_INTEGER (L"Formant number"), GET_REAL (L"From time"), GET_REAL (L"To time"));
		praat_dataChanged (me);
	}
END

FORM (FormantGrid_to_Formant, L"FormantGrid: To Formant", 0)
	POSITIVE (L"Time step (s)", L"0.01")
	REAL (L"Intensity (Pa\u00B2)", L"0.1")
	OK
DO
	double intensity = GET_REAL (L"Intensity");
	if (intensity < 0.0) Melder_throw ("Intensity cannot be negative.");
	LOOP {
		iam (FormantGrid);
		autoFormant thee = FormantGrid_to_Formant (me, GET_REAL (L"Time step"), intensity);
		praat_new (thee.transfer(), my name);
	}
END

/***** FORMANTGRID & SOUND *****/

DIRECT (Sound_FormantGrid_filter)
	Sound me = NULL;
	FormantGrid grid = NULL;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter (me, grid);
	praat_new (thee.transfer(), my name, L"_filt");
END

DIRECT (Sound_FormantGrid_filter_noscale)
	Sound me = NULL;
	FormantGrid grid = NULL;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter_noscale (me, grid);
	praat_new (thee.transfer(), my name, L"_filt");
END

/***** FORMANTTIER *****/

FORM (FormantTier_addPoint, L"Add one point", L"FormantTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	LABEL (L"", L"Frequencies and bandwidths (Hz):")
	TEXTFIELD (L"fb pairs", L"500 50 1500 100 2500 150 3500 200 4500 300")
	OK
DO
	autoFormantPoint point = FormantPoint_create (GET_REAL (L"Time"));
	double *f = point -> formant, *b = point -> bandwidth;
	char *fbpairs = Melder_peekWcsToUtf8 (GET_STRING (L"fb pairs"));
	int numberOfFormants = sscanf (fbpairs, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
		f, b, f+1, b+1, f+2, b+2, f+3, b+3, f+4, b+4, f+5, b+5, f+6, b+6, f+7, b+7, f+8, b+8, f+9, b+9) / 2;
	if (numberOfFormants < 1)
		Melder_throw (L"Number of formant-bandwidth pairs must be at least 1.");
	point -> numberOfFormants = numberOfFormants;
	LOOP {
		iam (FormantTier);
		autoFormantPoint point2 = Data_copy (point.peek());
		AnyTier_addPoint (me, point2.transfer());
		praat_dataChanged (me);
	}
END

FORM (FormantTier_create, L"Create empty FormantTier", NULL)
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoFormantTier thee = FormantTier_create (startTime, endTime);
	praat_new (thee.transfer(), GET_STRING (L"Name"));
END

FORM (FormantTier_downto_TableOfReal, L"Down to TableOfReal", 0)
	BOOLEAN (L"Include formants", 1)
	BOOLEAN (L"Include bandwidths", 0)
	OK
DO
	LOOP {
		iam (FormantTier);
		autoTableOfReal thee = FormantTier_downto_TableOfReal (me, GET_INTEGER (L"Include formants"), GET_INTEGER (L"Include bandwidths"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (FormantTier_getBandwidthAtTime, L"FormantTier: Get bandwidth", L"FormantTier: Get bandwidth at time...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (FormantTier);
		double bandwidth = FormantTier_getBandwidthAtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"));
		Melder_informationReal (bandwidth, L"hertz");
	}
END
	
FORM (FormantTier_getValueAtTime, L"FormantTier: Get value", L"FormantTier: Get value at time...")
	NATURAL (L"Formant number", L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (FormantTier);
		double value = FormantTier_getValueAtTime (me, GET_INTEGER (L"Formant number"), GET_REAL (L"Time"));
		Melder_informationReal (value, L"hertz");
	}
END
	
DIRECT (FormantTier_help) Melder_help (L"FormantTier"); END

FORM (FormantTier_speckle, L"Draw FormantTier", 0)
	praat_dia_timeRange (dia);
	POSITIVE (L"Maximum frequency (Hz)", L"5500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (FormantTier);
		autoPraatPicture picture;
		FormantTier_speckle (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Maximum frequency"), GET_INTEGER (L"Garnish"));
	}
END

/***** FORMANTTIER & SOUND *****/

DIRECT (Sound_FormantTier_filter)
	Sound me = NULL;
	FormantTier tier = NULL;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter (me, tier);
	praat_new (thee.transfer(), my name, L"_filt");
END

DIRECT (Sound_FormantTier_filter_noscale)
	Sound me = NULL;
	FormantTier tier = NULL;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter_noscale (me, tier);
	praat_new (thee.transfer(), my name, L"_filt");
END

/***** HARMONICITY *****/

FORM (Harmonicity_draw, L"Draw harmonicity", 0)
	praat_dia_timeRange (dia);
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0 (= auto)")
	OK
DO
	LOOP {
		iam (Harmonicity);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 0.0, 0.0,
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

FORM (Harmonicity_formula, L"Harmonicity Formula", L"Harmonicity: Formula...")
	LABEL (L"label", L"x is time")
	LABEL (L"label", L"for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (Harmonicity);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Harmonicity may have partically changed
			throw;
		}
	}
END

FORM (Harmonicity_getMaximum, L"Harmonicity: Get maximum", L"Harmonicity: Get maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getMaximum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (maximum, L"dB");
	}
END

FORM (Harmonicity_getMean, L"Harmonicity: Get mean", L"Harmonicity: Get mean...")
	praat_dia_timeRange (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double mean = Harmonicity_getMean (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (mean, L"dB");
	}
END

FORM (Harmonicity_getMinimum, L"Harmonicity: Get minimum", L"Harmonicity: Get minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getMinimum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (minimum, L"dB");
	}
END

FORM (Harmonicity_getStandardDeviation, L"Harmonicity: Get standard deviation", L"Harmonicity: Get standard deviation...")
	praat_dia_timeRange (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double stdev = Harmonicity_getStandardDeviation (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (stdev, L"dB");
	}
END

FORM (Harmonicity_getTimeOfMaximum, L"Harmonicity: Get time of maximum", L"Harmonicity: Get time of maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getXOfMaximum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (maximum, L"seconds");
	}
END

FORM (Harmonicity_getTimeOfMinimum, L"Harmonicity: Get time of minimum", L"Harmonicity: Get time of minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getXOfMinimum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (minimum, L"seconds");
	}
END

FORM (Harmonicity_getValueAtTime, L"Harmonicity: Get value", L"Harmonicity: Get value at time...")
	dia_Vector_getValue (dia);
	OK
DO
	LOOP {
		iam (Harmonicity);
		double value = Vector_getValueAtX (me, GET_REAL (L"Time"), 1, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (value, L"dB");
	}
END
	
FORM (Harmonicity_getValueInFrame, L"Get value in frame", L"Harmonicity: Get value in frame...")
	INTEGER (L"Frame number", L"10")
	OK
DO
	LOOP {
		iam (Harmonicity);
		long frameNumber = GET_INTEGER (L"Frame number");
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], L"dB");
	}
END

DIRECT (Harmonicity_help) Melder_help (L"Harmonicity"); END

DIRECT (Harmonicity_to_Matrix)
	LOOP {
		iam (Harmonicity);
		autoMatrix thee = Harmonicity_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** INTENSITY *****/

FORM (Intensity_draw, L"Draw Intensity", 0)
	praat_dia_timeRange (dia);
	REAL (L"Minimum (dB)", L"0.0")
	REAL (L"Maximum (dB)", L"0.0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Intensity);
		autoPraatPicture picture;
		Intensity_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Intensity_downto_IntensityTier)
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_downto_IntensityTier (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Intensity_downto_Matrix)
	LOOP {
		iam (Intensity);
		autoMatrix thee = Intensity_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Intensity_formula, L"Intensity Formula", 0)
	LABEL (L"label", L"`x' is the time in seconds, `col' is the frame number, `self' is in dB")
	LABEL (L"label", L"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"0")
	OK
DO
	LOOP {
		iam (Intensity);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Intensity may have partially changed
			throw;
		}
	}
END

FORM (Intensity_getMaximum, L"Intensity: Get maximum", L"Intensity: Get maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double maximum = Vector_getMaximum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (maximum, L"dB");
		break;   // OPTIMIZE
	}
END

FORM (old_Intensity_getMean, L"Intensity: Get mean", L"Intensity: Get mean...")
	praat_dia_timeRange (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 0, 0, TRUE);
		Melder_informationReal (mean, L"dB");
		break;   // OPTIMIZE
	}
END

FORM (Intensity_getMean, L"Intensity: Get mean", L"Intensity: Get mean...")
	praat_dia_timeRange (dia);
	RADIO (L"Averaging method", 1)
		RADIOBUTTON (L"energy")
		RADIOBUTTON (L"sones")
		RADIOBUTTON (L"dB")
	OK
DO_ALTERNATIVE (old_Intensity_getMean)
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			0, GET_INTEGER (L"Averaging method"), TRUE);
		Melder_informationReal (mean, L"dB");
		break;   // OPTIMIZE
	}
END

FORM (Intensity_getMinimum, L"Intensity: Get minimum", L"Intensity: Get minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double minimum = Vector_getMinimum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (minimum, L"dB");
		break;   // OPTIMIZE
	}
END

FORM (Intensity_getQuantile, L"Intensity: Get quantile", 0)
	praat_dia_timeRange (dia);
	REAL (L"Quantile (0-1)", L"0.50")
	OK
DO
	LOOP {
		iam (Intensity);
		double quantile = Intensity_getQuantile (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Quantile"));
		Melder_informationReal (quantile, L"dB");
	}
END

FORM (Intensity_getStandardDeviation, L"Intensity: Get standard deviation", L"Intensity: Get standard deviation...")
	praat_dia_timeRange (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double stdev = Vector_getStandardDeviation (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), 1);
		Melder_informationReal (stdev, L"dB");
	}
END

FORM (Intensity_getTimeOfMaximum, L"Intensity: Get time of maximum", L"Intensity: Get time of maximum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMaximum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
	}
END

FORM (Intensity_getTimeOfMinimum, L"Intensity: Get time of minimum", L"Intensity: Get time of minimum...")
	dia_Vector_getExtremum (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMinimum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
	}
END

FORM (Intensity_getValueAtTime, L"Intensity: Get value", L"Intensity: Get value at time...")
	dia_Vector_getValue (dia);
	OK
DO
	LOOP {
		iam (Intensity);
		double value = Vector_getValueAtX (me, GET_REAL (L"Time"), 1, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (value, L"dB");
	}
END
	
FORM (Intensity_getValueInFrame, L"Get value in frame", L"Intensity: Get value in frame...")
	INTEGER (L"Frame number", L"10")
	OK
DO
	LOOP {
		iam (Intensity);
		long frameNumber = GET_INTEGER (L"Frame number");
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], L"dB");
	}
END

DIRECT (Intensity_help) Melder_help (L"Intensity"); END

DIRECT (Intensity_to_IntensityTier_peaks)
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_peaks (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Intensity_to_IntensityTier_valleys)
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_valleys (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** INTENSITY & PITCH *****/

FORM (Pitch_Intensity_draw, L"Plot intensity by pitch", 0)
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= auto)")
	REAL (L"From intensity (dB)", L"0.0")
	REAL (L"To intensity (dB)", L"100.0")
	BOOLEAN (L"Garnish", 1)
	RADIO (L"Drawing method", 1)
	RADIOBUTTON (L"Speckles")
	RADIOBUTTON (L"Curve")
	RADIOBUTTON (L"Speckles and curve")
	OK
DO
	Pitch pitch = NULL;
	Intensity intensity = NULL;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	autoPraatPicture picture;
	Pitch_Intensity_draw (pitch, intensity, GRAPHICS,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_REAL (L"From intensity"), GET_REAL (L"To intensity"), GET_INTEGER (L"Garnish"), GET_INTEGER (L"Drawing method"));
END

FORM (Pitch_Intensity_speckle, L"Plot intensity by pitch", 0)
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= auto)")
	REAL (L"From intensity (dB)", L"0.0")
	REAL (L"To intensity (dB)", L"100.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Pitch pitch = NULL;
	Intensity intensity = NULL;
	LOOP {
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (pitch && intensity) break;   // OPTIMIZE
	}
	autoPraatPicture picture;
	Pitch_Intensity_draw (pitch, intensity, GRAPHICS,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_REAL (L"From intensity"), GET_REAL (L"To intensity"), GET_INTEGER (L"Garnish"), 1);
END

/***** INTENSITY & POINTPROCESS *****/

DIRECT (Intensity_PointProcess_to_IntensityTier)
	Intensity intensity = NULL;
	PointProcess point = NULL;
	LOOP {
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = Intensity_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.transfer(), intensity -> name);
END

/***** INTENSITYTIER *****/

FORM (IntensityTier_addPoint, L"Add one point", L"IntensityTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	REAL (L"Intensity (dB)", L"75")
	OK
DO
	LOOP {
		iam (IntensityTier);
		RealTier_addPoint (me, GET_REAL (L"Time"), GET_REAL (L"Intensity")); therror
		praat_dataChanged (me);
	}
END

FORM (IntensityTier_create, L"Create empty IntensityTier", NULL)
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoIntensityTier thee = IntensityTier_create (startTime, endTime);
	praat_new (thee.transfer(), GET_STRING (L"Name"));
END

DIRECT (IntensityTier_downto_PointProcess)
	LOOP {
		iam (IntensityTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (IntensityTier_downto_TableOfReal)
	LOOP {
		iam (IntensityTier);
		autoTableOfReal thee = IntensityTier_downto_TableOfReal (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (IntensityTier_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit an IntensityTier from batch.");
	Sound sound = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay NULL
		if (sound) break;   // OPTIMIZE
	}
	LOOP if (CLASS == classIntensityTier) {
		iam (IntensityTier);
		autoIntensityTierEditor editor = IntensityTierEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me, sound, TRUE);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (IntensityTier_formula, L"IntensityTier: Formula", L"IntensityTier: Formula...")
	LABEL (L"", L"# ncol = the number of points")
	LABEL (L"", L"for col from 1 to ncol")
	LABEL (L"", L"   # x = the time of the colth point, in seconds")
	LABEL (L"", L"   # self = the value of the colth point, in dB")
	LABEL (L"", L"   self = `formula'")
	LABEL (L"", L"endfor")
	TEXTFIELD (L"formula", L"self + 3.0")
	OK
DO
	LOOP {
		iam (IntensityTier);
		try {
			RealTier_formula (me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the IntensityTier may have partially changed
			throw;
		}
	}
END

FORM (IntensityTier_getValueAtTime, L"Get IntensityTier value", L"IntensityTier: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (L"Time"));
		Melder_informationReal (value, L"dB");
	}
END
	
FORM (IntensityTier_getValueAtIndex, L"Get IntensityTier value", L"IntensityTier: Get value at index...")
	INTEGER (L"Point number", L"10")
	OK
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (L"Point number"));
		Melder_informationReal (value, L"dB");
	}
END

DIRECT (IntensityTier_help) Melder_help (L"IntensityTier"); END

DIRECT (IntensityTier_to_AmplitudeTier)
	LOOP {
		iam (IntensityTier);
		autoAmplitudeTier thee = IntensityTier_to_AmplitudeTier (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (info_IntensityTier_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your IntensityTier editor:\n"
		"   select an IntensityTier and a Sound, and click \"View & Edit\".");
END

/***** INTENSITYTIER & POINTPROCESS *****/

DIRECT (IntensityTier_PointProcess_to_IntensityTier)
	IntensityTier intensity = NULL;
	PointProcess point = NULL;
	LOOP {
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = IntensityTier_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.transfer(), intensity -> name);
END

/***** INTENSITYTIER & SOUND *****/

DIRECT (Sound_IntensityTier_multiply_old)
	Sound sound = NULL;
	IntensityTier intensity = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (sound && intensity) break;   // OPTIMIZE
	}
	autoSound thee = Sound_IntensityTier_multiply (sound, intensity, TRUE);
	praat_new (thee.transfer(), sound -> name, L"_int");
END

FORM (Sound_IntensityTier_multiply, L"Sound & IntervalTier: Multiply", 0)
	BOOLEAN (L"Scale to 0.9", 1)
	OK
DO
	Sound sound = NULL;
	IntensityTier intensity = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (sound && intensity) break;   // OPTIMIZE
	}
	autoSound thee = Sound_IntensityTier_multiply (sound, intensity, GET_INTEGER (L"Scale to 0.9"));
	praat_new (thee.transfer(), sound -> name, L"_int");
END

/***** INTERVALTIER, rest in praat_TextGrid_init.c *****/

FORM_READ (IntervalTier_readFromXwaves, L"Read IntervalTier from Xwaves", 0, true)
	autoIntervalTier me = IntervalTier_readFromXwaves (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

/***** LTAS *****/

DIRECT (Ltases_average)
	autoCollection ltases = praat_getSelectedObjects ();
	autoLtas thee = Ltases_average (ltases.peek());
	praat_new (thee.transfer(), L"averaged");
END

FORM (Ltas_computeTrendLine, L"Ltas: Compute trend line", L"Ltas: Compute trend line...")
	REAL (L"left Frequency range (Hz)", L"600.0")
	POSITIVE (L"right Frequency range (Hz)", L"4000.0")
	OK
DO
	LOOP {
		iam (Ltas);
		autoLtas thee = Ltas_computeTrendLine (me, GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"));
		praat_new (thee.transfer(), my name, L"_trend");
	}
END

FORM (old_Ltas_draw, L"Ltas: Draw", 0)
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (= all)")
	REAL (L"left Power range (dB/Hz)", L"-20.0")
	REAL (L"right Power range (dB/Hz)", L"80.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Ltas);
		autoPraatPicture picture;
		Ltas_draw (me, GRAPHICS, GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"left Power range"), GET_REAL (L"right Power range"), GET_INTEGER (L"Garnish"), L"Bars");
	}
END

FORM (Ltas_draw, L"Ltas: Draw", 0)
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (= all)")
	REAL (L"left Power range (dB/Hz)", L"-20.0")
	REAL (L"right Power range (dB/Hz)", L"80.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 2)
		OPTION (L"Curve")
		OPTION (L"Bars")
		OPTION (L"Poles")
		OPTION (L"Speckles")
	OK
DO_ALTERNATIVE (old_Ltas_draw)
	LOOP {
		iam (Ltas);
		autoPraatPicture picture;
		Ltas_draw (me, GRAPHICS, GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"left Power range"), GET_REAL (L"right Power range"), GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
	}
END

FORM (Ltas_formula, L"Ltas Formula", 0)
	LABEL (L"label", L"`x' is the frequency in hertz, `col' is the bin number")
	LABEL (L"label", L"x := x1;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"0")
	OK
DO
	LOOP {
		iam (Ltas);
		try {
			Matrix_formula (reinterpret_cast <Matrix> (me), GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Ltas may have partially changed
			throw;
		}
	}
END

FORM (Ltas_getBinNumberFromFrequency, L"Ltas: Get band from frequency", L"Ltas: Get band from frequency...")
	REAL (L"Frequency (Hz)", L"2000")
	OK
DO
	Ltas me = FIRST (Ltas);
	double binNumber = Sampled_xToIndex (me, GET_REAL (L"Frequency"));
	Melder_informationReal (binNumber, NULL);
END

DIRECT (Ltas_getBinWidth)
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my dx, L"hertz");
END

FORM (Ltas_getFrequencyFromBinNumber, L"Ltas: Get frequency from bin number", L"Ltas: Get frequency from bin number...")
	NATURAL (L"Bin number", L"1")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Sampled_indexToX (me, GET_INTEGER (L"Bin number"));
	Melder_informationReal (frequency, L"hertz");
END

FORM (Ltas_getFrequencyOfMaximum, L"Ltas: Get frequency of maximum", L"Ltas: Get frequency of maximum...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Interpolation", 1)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMaximum (me,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (frequency, L"hertz");
END

FORM (Ltas_getFrequencyOfMinimum, L"Ltas: Get frequency of minimum", L"Ltas: Get frequency of minimum...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Interpolation", 1)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMinimum (me,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (frequency, L"hertz");
END

DIRECT (Ltas_getHighestFrequency)
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmax, L"Hz");
END

FORM (Ltas_getLocalPeakHeight, L"Ltas: Get local peak height", 0)
	REAL (L"left Environment (Hz)", L"1700.0")
	REAL (L"right Environment (Hz)", L"4200.0")
	REAL (L"left Peak (Hz)", L"2400.0")
	REAL (L"right Peak (Hz)", L"3200.0")
	RADIO (L"Averaging method", 1)
		RADIOBUTTON (L"energy")
		RADIOBUTTON (L"sones")
		RADIOBUTTON (L"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double environmentMin = GET_REAL (L"left Environment"), environmentMax = GET_REAL (L"right Environment");
	double peakMin = GET_REAL (L"left Peak"), peakMax = GET_REAL (L"right Peak");
	if (environmentMin >= peakMin) Melder_throw ("The beginning of the environment must lie before the peak.");
	if (peakMin >= peakMax) Melder_throw ("The end of the peak must lie after its beginning.");
	if (environmentMax <= peakMax) Melder_throw ("The end of the environment must lie after the peak.");
	double localPeakHeight = Ltas_getLocalPeakHeight (me, environmentMin, environmentMax,
		peakMin, peakMax, GET_INTEGER (L"Averaging method"));
	Melder_informationReal (localPeakHeight, L"dB");
END

DIRECT (Ltas_getLowestFrequency)
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmin, L"hertz");
END

FORM (Ltas_getMaximum, L"Ltas: Get maximum", L"Ltas: Get maximum...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Interpolation", 1)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double maximum = Vector_getMaximum (me,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (maximum, L"dB");
END

FORM (Ltas_getMean, L"Ltas: Get mean", L"Ltas: Get mean...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Averaging method", 1)
		RADIOBUTTON (L"energy")
		RADIOBUTTON (L"sones")
		RADIOBUTTON (L"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double mean = Sampled_getMean_standardUnit (me, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		0, GET_INTEGER (L"Averaging method"), FALSE);
	Melder_informationReal (mean, L"dB");
END

FORM (Ltas_getMinimum, L"Ltas: Get minimum", L"Ltas: Get minimum...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Interpolation", 1)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double minimum = Vector_getMinimum (me,
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (minimum, L"dB");
END

DIRECT (Ltas_getNumberOfBins)
	Ltas me = FIRST (Ltas);
	long numberOfBins = my nx;
	Melder_information (Melder_integer (numberOfBins), L" bins");
END

FORM (Ltas_getSlope, L"Ltas: Get slope", 0)
	REAL (L"left Low band (Hz)", L"0.0")
	REAL (L"right Low band (Hz)", L"1000.0")
	REAL (L"left High band (Hz)", L"1000.0")
	REAL (L"right High band (Hz)", L"4000.0")
	RADIO (L"Averaging method", 1)
		RADIOBUTTON (L"energy")
		RADIOBUTTON (L"sones")
		RADIOBUTTON (L"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double slope = Ltas_getSlope (me, GET_REAL (L"left Low band"), GET_REAL (L"right Low band"),
		GET_REAL (L"left High band"), GET_REAL (L"right High band"), GET_INTEGER (L"Averaging method"));
	Melder_informationReal (slope, L"dB");
END

FORM (Ltas_getStandardDeviation, L"Ltas: Get standard deviation", L"Ltas: Get standard deviation...")
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"0.0 (= all)")
	RADIO (L"Averaging method", 1)
		RADIOBUTTON (L"energy")
		RADIOBUTTON (L"sones")
		RADIOBUTTON (L"dB")
	OK
DO
	Ltas me = FIRST (Ltas);
	double stdev = Sampled_getStandardDeviation_standardUnit (me, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		0, GET_INTEGER (L"Averaging method"), FALSE);
	Melder_informationReal (stdev, L"dB");
END

FORM (Ltas_getValueAtFrequency, L"Ltas: Get value", L"Ltas: Get value at frequency...")
	REAL (L"Frequency (Hz)", L"1500")
	RADIO (L"Interpolation", 1)
		RADIOBUTTON (L"Nearest")
		RADIOBUTTON (L"Linear")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	Ltas me = FIRST (Ltas);
	double value = Vector_getValueAtX (me, GET_REAL (L"Frequency"), 1, GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (value, L"dB");
END
	
FORM (Ltas_getValueInBin, L"Get value in bin", L"Ltas: Get value in bin...")
	INTEGER (L"Bin number", L"100")
	OK
DO
	Ltas me = FIRST (Ltas);
	long binNumber = GET_INTEGER (L"Bin number");
	double value = binNumber < 1 || binNumber > my nx ? NUMundefined : my z [1] [binNumber];
	Melder_informationReal (value, L"dB");
END

DIRECT (Ltas_help) Melder_help (L"Ltas"); END

DIRECT (Ltases_merge)
	autoCollection ltases = praat_getSelectedObjects ();
	autoLtas thee = Ltases_merge (ltases.peek());
	praat_new (thee.transfer(), L"merged");
END

FORM (Ltas_subtractTrendLine, L"Ltas: Subtract trend line", L"Ltas: Subtract trend line...")
	REAL (L"left Frequency range (Hz)", L"600.0")
	POSITIVE (L"right Frequency range (Hz)", L"4000.0")
	OK
DO
	LOOP {
		iam (Ltas);
		autoLtas thee = Ltas_subtractTrendLine (me, GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"));
		praat_new (thee.transfer(), my name, L"_fit");
	}
END

DIRECT (Ltas_to_Matrix)
	LOOP {
		iam (Ltas);
		autoMatrix thee = Ltas_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Ltas_to_SpectrumTier_peaks)
	LOOP {
		iam (Ltas);
		autoSpectrumTier thee = Ltas_to_SpectrumTier_peaks (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** MANIPULATION *****/

static void cb_ManipulationEditor_publication (Editor editor, void *closure, Data publication) {
	(void) editor;
	(void) closure;
	try {
		praat_new (publication, L"fromManipulationEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (Manipulation_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Manipulation from batch.");
	LOOP {
		iam (Manipulation);
		autoManipulationEditor editor = ManipulationEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_ManipulationEditor_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

DIRECT (Manipulation_extractDurationTier)
	LOOP {
		iam (Manipulation);
		if (! my duration) Melder_throw (me, ": I don't contain a DurationTier.");
		autoDurationTier thee = Data_copy (my duration);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_extractOriginalSound)
	LOOP {
		iam (Manipulation);
		if (! my sound) Melder_throw (me, ": I don't contain a Sound.");
		autoSound thee = Data_copy (my sound);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_extractPitchTier)
	LOOP {
		iam (Manipulation);
		if (! my pitch) Melder_throw (me, ": I don't contain a PitchTier.");
		autoPitchTier thee = Data_copy (my pitch);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_extractPulses)
	LOOP {
		iam (Manipulation);
		if (! my pulses) Melder_throw (me, ": I don't contain a PointProcess.");
		autoPointProcess thee = Data_copy (my pulses);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_getResynthesis_lpc)
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_PITCH_LPC);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_getResynthesis_overlapAdd)
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_OVERLAPADD);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Manipulation_help) Melder_help (L"Manipulation"); END

DIRECT (Manipulation_play_lpc)
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_PITCH_LPC); therror
	}
END

DIRECT (Manipulation_play_overlapAdd)
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_OVERLAPADD); therror
	}
END

DIRECT (Manipulation_removeDuration)
	LOOP {
		iam (Manipulation);
		forget (my duration);
		praat_dataChanged (me);
	}
END

DIRECT (Manipulation_removeOriginalSound)
	LOOP {
		iam (Manipulation);
		forget (my sound);
		praat_dataChanged (me);
	}
END

FORM_WRITE (Manipulation_writeToBinaryFileWithoutSound, L"Binary file without Sound", 0, 0)
	Manipulation_writeToBinaryFileWithoutSound (FIRST_ANY (Manipulation), file);
END

FORM_WRITE (Manipulation_writeToTextFileWithoutSound, L"Text file without Sound", 0, 0)
	Manipulation_writeToTextFileWithoutSound (FIRST_ANY (Manipulation), file);
END

DIRECT (info_DurationTier_Manipulation_replace)
	Melder_information (L"To replace the DurationTier in a Manipulation object,\n"
		"select a DurationTier object and a Manipulation object\nand choose \"Replace duration\".");
END

DIRECT (info_PitchTier_Manipulation_replace)
	Melder_information (L"To replace the PitchTier in a Manipulation object,\n"
		"select a PitchTier object and a Manipulation object\nand choose \"Replace pitch\".");
END

/***** MANIPULATION & DURATIONTIER *****/

DIRECT (Manipulation_replaceDurationTier)
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceDurationTier (me, FIRST (DurationTier)); therror
	praat_dataChanged (me);
END

DIRECT (Manipulation_replaceDurationTier_help) Melder_help (L"Manipulation: Replace duration tier"); END

/***** MANIPULATION & PITCHTIER *****/

DIRECT (Manipulation_replacePitchTier)
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePitchTier (me, FIRST (PitchTier)); therror
	praat_dataChanged (me);
END

DIRECT (Manipulation_replacePitchTier_help) Melder_help (L"Manipulation: Replace pitch tier"); END

/***** MANIPULATION & POINTPROCESS *****/

DIRECT (Manipulation_replacePulses)
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePulses (me, FIRST (PointProcess)); therror
	praat_dataChanged (me);
END

/***** MANIPULATION & SOUND *****/

DIRECT (Manipulation_replaceOriginalSound)
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceOriginalSound (me, FIRST (Sound)); therror
	praat_dataChanged (me);
END

/***** MANIPULATION & TEXTTIER *****/

DIRECT (Manipulation_TextTier_to_Manipulation)
	Manipulation me = FIRST (Manipulation);
	TextTier thee = FIRST (TextTier);
	autoManipulation him = Manipulation_AnyTier_to_Manipulation (me, reinterpret_cast <AnyTier> (thee));
	praat_new (him.transfer(), my name);	
END

/***** MATRIX *****/

DIRECT (Matrix_appendRows)
	Matrix m1 = NULL, m2 = NULL;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoMatrix thee = Matrix_appendRows (m1, m2, classMatrix);
	praat_new (thee.transfer(), m1 -> name, L"_", m2 -> name);
END

FORM (Matrix_create, L"Create Matrix", L"Create Matrix...")
	WORD (L"Name", L"xy")
	REAL (L"xmin", L"1.0")
	REAL (L"xmax", L"1.0")
	NATURAL (L"Number of columns", L"1")
	POSITIVE (L"dx", L"1.0")
	REAL (L"x1", L"1.0")
	REAL (L"ymin", L"1.0")
	REAL (L"ymax", L"1.0")
	NATURAL (L"Number of rows", L"1")
	POSITIVE (L"dy", L"1.0")
	REAL (L"y1", L"1.0")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"x*y")
	OK
DO
	double xmin = GET_REAL (L"xmin"), xmax = GET_REAL (L"xmax");
	double ymin = GET_REAL (L"ymin"), ymax = GET_REAL (L"ymax");
	if (xmax < xmin) Melder_throw ("xmax (", Melder_single (xmax), ") should not be less than xmin (", Melder_single (xmin), ").");
	if (ymax < ymin) Melder_throw ("ymax (", Melder_single (ymax), ") should not be less than ymin (", Melder_single (ymin), ").");
	autoMatrix me = Matrix_create (
		xmin, xmax, GET_INTEGER (L"Number of columns"), GET_REAL (L"dx"), GET_REAL (L"x1"),
		ymin, ymax, GET_INTEGER (L"Number of rows"), GET_REAL (L"dy"), GET_REAL (L"y1"));
	Matrix_formula (me.peek(), GET_STRING (L"formula"), interpreter, NULL); therror
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Matrix_createSimple, L"Create simple Matrix", L"Create simple Matrix...")
	WORD (L"Name", L"xy")
	NATURAL (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"10")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"x*y")
	OK
DO
	autoMatrix me = Matrix_createSimple (GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns"));
	Matrix_formula (me.peek(), GET_STRING (L"formula"), interpreter, NULL); therror
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Matrix_drawOneContour, L"Draw one altitude contour", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Height", L"0.5")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawOneContour (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Height"));
	}
END

FORM (Matrix_drawContours, L"Draw altitude contours", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawContours (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

FORM (Matrix_drawRows, L"Draw rows", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="),
			GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

DIRECT (Matrix_eigen)
	LOOP {
		iam (Matrix);
		Matrix vec_ = NULL, val_ = NULL;
		Matrix_eigen (me, & vec_, & val_);
		autoMatrix vec = vec_;
		autoMatrix val = val_;
		praat_new (vec.transfer(), L"eigenvectors");
		praat_new (val.transfer(), L"eigenvalues");
	}
END

FORM (Matrix_formula, L"Matrix Formula", L"Formula...")
	LABEL (L"label", L"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (L"formula", L"self")
	OK
DO
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Matrix may have partially changed
			throw;
		}
	}
END

DIRECT (Matrix_getHighestX)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmax, NULL);
END

DIRECT (Matrix_getHighestY)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymax, NULL);
END

DIRECT (Matrix_getLowestX)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmin, NULL);
END

DIRECT (Matrix_getLowestY)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymin, NULL);
END

DIRECT (Matrix_getNumberOfColumns)
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (Melder_integer (my nx));
END

DIRECT (Matrix_getNumberOfRows)
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (Melder_integer (my ny));
END

DIRECT (Matrix_getColumnDistance)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dx, NULL);
END

DIRECT (Matrix_getRowDistance)
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dy, NULL);
END

DIRECT (Matrix_getSum)
	Matrix me = FIRST_ANY (Matrix);
	double sum = Matrix_getSum (me);
	Melder_informationReal (sum, NULL);
END

DIRECT (Matrix_getMaximum)
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (maximum, NULL);
END

DIRECT (Matrix_getMinimum)
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (minimum, NULL);
END

FORM (Matrix_getValueAtXY, L"Matrix: Get value at xy", 0)
	REAL (L"X", L"0")
	REAL (L"Y", L"0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double x = GET_REAL (L"X"), y = GET_REAL (L"Y");
	double value = Matrix_getValueAtXY (me, x, y);
	Melder_information (Melder_double (value), L" (at x = ", Melder_double (x), L" and y = ", Melder_double (y), L")");
END

FORM (Matrix_getValueInCell, L"Matrix: Get value in cell", 0)
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	long row = GET_INTEGER (L"Row number"), column = GET_INTEGER (L"Column number");
	if (row > my ny) Melder_throw ("Row number must not exceed number of rows.");
	if (column > my nx) Melder_throw ("Column number must not exceed number of columns.");
	Melder_informationReal (my z [row] [column], NULL);
END

FORM (Matrix_getXofColumn, L"Matrix: Get x of column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double x = Matrix_columnToX (me, GET_INTEGER (L"Column number"));
	Melder_informationReal (x, NULL);
END

FORM (Matrix_getYofRow, L"Matrix: Get y of row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double y = Matrix_rowToY (me, GET_INTEGER (L"Row number"));
	Melder_informationReal (y, NULL);
END

DIRECT (Matrix_help) Melder_help (L"Matrix"); END

DIRECT (Matrix_movie)
	Graphics g = Movie_create (L"Matrix movie", 300, 300);
	LOOP {
		iam (Matrix);
		Matrix_movie (me, g);
	}
END

FORM (Matrix_paintCells, L"Matrix: Paint cells with greys", L"Matrix: Paint cells...")
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintCells (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

FORM (Matrix_paintContours, L"Matrix: Paint altitude contours with greys", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintContours (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

FORM (Matrix_paintImage, L"Matrix: Paint grey image", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintImage (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"));
	}
END

FORM (Matrix_paintSurface, L"Matrix: Paint 3-D surface plot", 0)
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"0.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"0.0")
	REAL (L"Minimum", L"0.0")
	REAL (L"Maximum", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintSurface (me, GRAPHICS,
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="),
			GET_REAL (L"Minimum"), GET_REAL (L"Maximum"), 30, 45);
	}
END

FORM (Matrix_power, L"Matrix: Power...", 0)
	NATURAL (L"Power", L"2")
	OK
DO
	LOOP {
		iam (Matrix);
		autoMatrix thee = Matrix_power (me, GET_INTEGER (L"Power"));
		praat_new (thee.transfer(), my name);
	}
END

FORM_READ (Matrix_readFromRawTextFile, L"Read Matrix from raw text file", 0, true)
	autoMatrix me = Matrix_readFromRawTextFile (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

FORM_READ (Matrix_readAP, L"Read Matrix from LVS AP file", 0, true)
	autoMatrix me = Matrix_readAP (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

FORM (Matrix_setValue, L"Matrix: Set value", L"Matrix: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL (L"New value", L"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		long row = GET_INTEGER (L"Row number"), column = GET_INTEGER (L"Column number");
		if (row > my ny) Melder_throw ("Row number must not be greater than number of rows.");
		if (column > my nx) Melder_throw ("Column number must not be greater than number of columns.");
		my z [row] [column] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

DIRECT (Matrix_to_Cochleagram)
	LOOP {
		iam (Matrix);
		autoCochleagram thee = Matrix_to_Cochleagram (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Excitation)
	LOOP {
		iam (Matrix);
		autoExcitation thee = Matrix_to_Excitation (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Harmonicity)
	LOOP {
		iam (Matrix);
		autoHarmonicity thee = Matrix_to_Harmonicity (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Intensity)
	LOOP {
		iam (Matrix);
		autoIntensity thee = Matrix_to_Intensity (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Pitch)
	LOOP {
		iam (Matrix);
		autoPitch thee = Matrix_to_Pitch (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Spectrogram)
	LOOP {
		iam (Matrix);
		autoSpectrogram thee = Matrix_to_Spectrogram (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Spectrum)
	LOOP {
		iam (Matrix);
		autoSpectrum thee = Matrix_to_Spectrum (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Ltas)
	LOOP {
		iam (Matrix);
		autoLtas thee = Matrix_to_Ltas (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_ParamCurve)
	Matrix m1 = NULL, m2 = NULL;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoParamCurve thee = ParamCurve_create (m1, m2);
	praat_new (thee.transfer(), m1 -> name, L"_", m2 -> name);
END

DIRECT (Matrix_to_PointProcess)
	LOOP {
		iam (Matrix);
		autoPointProcess thee = Matrix_to_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Polygon)
	LOOP {
		iam (Matrix);
		autoPolygon thee = Matrix_to_Polygon (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Sound)
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Matrix_to_Sound_mono, L"Matrix: To Sound (mono)", 0)
	INTEGER (L"Row", L"1")
	LABEL (L"", L"(negative values count from last row)")
	OK
DO
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound_mono (me, GET_INTEGER (L"Row"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_TableOfReal)
	LOOP {
		iam (Matrix);
		autoTableOfReal thee = Matrix_to_TableOfReal (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_Transition)
	LOOP {
		iam (Matrix);
		autoTransition thee = Matrix_to_Transition (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Matrix_to_VocalTract)
	LOOP {
		iam (Matrix);
		autoVocalTract thee = Matrix_to_VocalTract (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM_WRITE (Matrix_writeToMatrixTextFile, L"Save Matrix as matrix text file", 0, L"mat")
	Matrix me = FIRST (Matrix);
	Matrix_writeToMatrixTextFile (me, file);
END

FORM_WRITE (Matrix_writeToHeaderlessSpreadsheetFile, L"Save Matrix as spreadsheet", 0, L"txt")
	Matrix me = FIRST (Matrix);
	Matrix_writeToHeaderlessSpreadsheetFile (me, file);
END

/***** MOVIE *****/

FORM_READ (Movie_openFromSoundFile, L"Open movie file", 0, true)
	autoMovie me = Movie_openFromSoundFile (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

FORM (Movie_paintOneImage, L"Movie: Paint one image", 0)
	NATURAL (L"Frame number", L"1")
	REAL (L"From x =", L"0.0")
	REAL (L"To x =", L"1.0")
	REAL (L"From y =", L"0.0")
	REAL (L"To y =", L"1.0")
	OK
DO
	LOOP {
		iam (Movie);
		autoPraatPicture picture;
		my f_paintOneImage (GRAPHICS, GET_INTEGER (L"Frame number"),
			GET_REAL (L"From x ="), GET_REAL (L"To x ="), GET_REAL (L"From y ="), GET_REAL (L"To y ="));
	}
END

DIRECT (Movie_viewAndEdit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Movie from batch.");
	LOOP {
		iam (Movie);
		autoMovieWindow editor = MovieWindow_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

/***** PARAMCURVE *****/

FORM (ParamCurve_draw, L"Draw parametrized curve", 0)
	REAL (L"Tmin", L"0.0")
	REAL (L"Tmax", L"0.0")
	REAL (L"Step", L"0.0")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (ParamCurve);
		autoPraatPicture picture;
		ParamCurve_draw (me, GRAPHICS,
			GET_REAL (L"Tmin"), GET_REAL (L"Tmax"), GET_REAL (L"Step"),
			GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"),
			GET_INTEGER (L"Garnish"));
	}
END

DIRECT (ParamCurve_help) Melder_help (L"ParamCurve"); END

/***** PITCH *****/

DIRECT (Pitch_getNumberOfVoicedFrames)
	Pitch me = FIRST (Pitch);
	Melder_information (Melder_integer (Pitch_countVoicedFrames (me)), L" voiced frames");
END

DIRECT (Pitch_difference)
	Pitch pit1 = NULL, pit2 = NULL;
	LOOP (pit1 ? pit2 : pit1) = (Pitch) OBJECT;
	Pitch_difference (pit1, pit2);
END

FORM (Pitch_draw, L"Pitch: Draw", L"Pitch: Draw...")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (L"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ);
	}
END

FORM (Pitch_drawErb, L"Pitch: Draw erb", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (L"Garnish"), Pitch_speckle_NO, kPitch_unit_ERB);
	}
END

FORM (Pitch_drawLogarithmic, L"Pitch: Draw logarithmic", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (L"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
	}
END

FORM (Pitch_drawMel, L"Pitch: Draw mel", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	REAL (L"left Frequency range (mel)", L"0.0")
	REAL (L"right Frequency range (mel)", L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (L"Garnish"), Pitch_speckle_NO, kPitch_unit_MEL);
	}
END

FORM (Pitch_drawSemitones, L"Pitch: Draw semitones", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	LABEL (L"", L"Range in semitones re 100 Hz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (L"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
	}
END

DIRECT (Pitch_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Pitch from batch.");
	LOOP {
		iam (Pitch);
		autoPitchEditor editor = PitchEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (Pitch_formula, L"Pitch: Formula", L"Formula...")
	LABEL (L"", L"x = time; col = frame; row = candidate (1 = current path); frequency (time, candidate) :=")
	TEXTFIELD (L"formula", L"self*2; Example: octave jump up")
	OK
DO
	LOOP {
		iam (Pitch);
		try {
			Pitch_formula (me, GET_STRING (L"formula"), interpreter); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Pitch may have partially changed
			throw;
		}
	}
END

FORM (Pitch_getMaximum, L"Pitch: Get maximum", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Pitch_getMaximum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), unit, GET_INTEGER (L"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END

FORM (Pitch_getMean, L"Pitch: Get mean", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Pitch_getMean (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END

FORM (Pitch_getMeanAbsoluteSlope, L"Pitch: Get mean absolute slope", 0)
	RADIO (L"Unit", 1)
		RADIOBUTTON (L"Hertz")
		RADIOBUTTON (L"Mel")
		RADIOBUTTON (L"Semitones")
		RADIOBUTTON (L"ERB")
	OK
DO
	int unit = GET_INTEGER (L"Unit");
	Pitch me = FIRST (Pitch);
	double slope;
	long nVoiced = (unit == 1 ? Pitch_getMeanAbsSlope_hertz : unit == 2 ? Pitch_getMeanAbsSlope_mel : unit == 3 ? Pitch_getMeanAbsSlope_semitones : Pitch_getMeanAbsSlope_erb)
		(me, & slope);
	if (nVoiced < 2) {
		Melder_information (L"--undefined--");
	} else {
		Melder_information (Melder_double (slope), L" ", GET_STRING (L"Unit"), L"/s");
	}
END

DIRECT (Pitch_getMeanAbsSlope_noOctave)
	Pitch me = FIRST (Pitch);
	double slope;
	(void) Pitch_getMeanAbsSlope_noOctave (me, & slope);
	Melder_informationReal (slope, L"Semitones/s");
END

FORM (Pitch_getMinimum, L"Pitch: Get minimum", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getMinimum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		Pitch_LEVEL_FREQUENCY, unit, GET_INTEGER (L"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END

FORM (Pitch_getQuantile, L"Pitch: Get quantile", 0)
	praat_dia_timeRange (dia);
	REAL (L"Quantile", L"0.50 (= median)")
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getQuantile (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Quantile"), Pitch_LEVEL_FREQUENCY, unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END

FORM (Pitch_getStandardDeviation, L"Pitch: Get standard deviation", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU (L"Unit", 1)
		OPTION (L"Hertz")
		OPTION (L"mel")
		OPTION (L"logHertz")
		OPTION (L"semitones")
		OPTION (L"ERB")
	OK
DO
	int unit = GET_INTEGER (L"Unit");
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	Pitch me = FIRST (Pitch);
	double value = Pitch_getStandardDeviation (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), unit);
	const wchar *unitText =
		unit == kPitch_unit_HERTZ ? L"Hz" :
		unit == kPitch_unit_MEL ? L"mel" :
		unit == kPitch_unit_LOG_HERTZ ? L"logHz" :
		unit == kPitch_unit_SEMITONES_1 ? L"semitones" :
		L"ERB";
	Melder_informationReal (value, unitText);
END

FORM (Pitch_getTimeOfMaximum, L"Pitch: Get time of maximum", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	Pitch me = FIRST (Pitch);
	double time = Pitch_getTimeOfMaximum (me,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_ENUM (kPitch_unit, L"Unit"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (time, L"seconds");
END

FORM (Pitch_getTimeOfMinimum, L"Pitch: Get time of minimum", 0)
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	OK
DO
	Pitch me = FIRST (Pitch);
	double time = Pitch_getTimeOfMinimum (me,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_ENUM (kPitch_unit, L"Unit"), GET_INTEGER (L"Interpolation") - 1);
	Melder_informationReal (time, L"seconds");
END

FORM (Pitch_getValueAtTime, L"Pitch: Get value at time", L"Pitch: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	RADIO (L"Interpolation", 2)
	RADIOBUTTON (L"Nearest")
	RADIOBUTTON (L"Linear")
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getValueAtX (me, GET_REAL (L"Time"), Pitch_LEVEL_FREQUENCY, unit, GET_INTEGER (L"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END
	
FORM (Pitch_getValueInFrame, L"Pitch: Get value in frame", L"Pitch: Get value in frame...")
	INTEGER (L"Frame number", L"10")
	OPTIONMENU_ENUM (L"Unit", kPitch_unit, DEFAULT)
	OK
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, L"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getValueAtSample (me, GET_INTEGER (L"Frame number"), Pitch_LEVEL_FREQUENCY, unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END

DIRECT (Pitch_help) Melder_help (L"Pitch"); END

DIRECT (Pitch_hum)
	LOOP {
		iam (Pitch);
		Pitch_hum (me, 0, 0); therror
	}
END

DIRECT (Pitch_interpolate)
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_interpolate (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_killOctaveJumps)
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_killOctaveJumps (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_play)
	LOOP {
		iam (Pitch);
		Pitch_play (me, 0, 0); therror
	}
END

FORM (Pitch_smooth, L"Pitch: Smooth", L"Pitch: Smooth...")
	REAL (L"Bandwidth (Hz)", L"10.0")
	OK
DO
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_smooth (me, GET_REAL (L"Bandwidth"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Pitch_speckle, L"Pitch: Speckle", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (L"Garnish"), Pitch_speckle_YES, kPitch_unit_HERTZ);
	}
END

FORM (Pitch_speckleErb, L"Pitch: Speckle erb", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (L"Garnish"), Pitch_speckle_YES, kPitch_unit_ERB);
	}
END

FORM (Pitch_speckleLogarithmic, L"Pitch: Speckle logarithmic", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (L"Garnish"), Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
	}
END

FORM (Pitch_speckleMel, L"Pitch: Speckle mel", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	REAL (L"left Frequency range (mel)", L"0")
	REAL (L"right Frequency range (mel)", L"500")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (L"Garnish"), Pitch_speckle_YES, kPitch_unit_MEL);
	}
END

FORM (Pitch_speckleSemitones, L"Pitch: Speckle semitones", L"Pitch: Draw...")
	REAL (STRING_FROM_TIME_SECONDS, L"0.0")
	REAL (STRING_TO_TIME_SECONDS, L"0.0 (= all)")
	LABEL (L"", L"Range in semitones re 100 hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (L"Garnish"), Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
	}
END

FORM (Pitch_subtractLinearFit, L"Pitch: subtract linear fit", 0)
	RADIO (L"Unit", 1)
		RADIOBUTTON (L"Hertz")
		RADIOBUTTON (L"Hertz (logarithmic)")
		RADIOBUTTON (L"Mel")
		RADIOBUTTON (L"Semitones")
		RADIOBUTTON (L"ERB")
	OK
DO
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_subtractLinearFit (me, GET_INTEGER (L"Unit") - 1);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_IntervalTier)
	LOOP {
		iam (Pitch);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_Matrix)
	LOOP {
		iam (Pitch);
		autoMatrix thee = Pitch_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_PitchTier)
	LOOP {
		iam (Pitch);
		autoPitchTier thee = Pitch_to_PitchTier (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_PointProcess)
	LOOP {
		iam (Pitch);
		autoPointProcess thee = Pitch_to_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_Sound_pulses)
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound (me, 0, 0, FALSE);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_Sound_hum)
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound (me, 0, 0, TRUE);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Pitch_to_Sound_sine, L"Pitch: To Sound (sine)", 0)
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	RADIO (L"Cut voiceless stretches", 2)
		OPTION (L"exactly")
		OPTION (L"at nearest zero crossings")
	OK
DO
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound_sine (me, 0, 0, GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Cut voiceless stretches") - 1);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Pitch_to_TextGrid, L"To TextGrid...", L"Pitch: To TextGrid...")
	SENTENCE (L"Tier names", L"Mary John bell")
	SENTENCE (L"Point tiers", L"bell")
	OK
DO
	LOOP {
		iam (Pitch);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (L"Tier names"), GET_STRING (L"Point tiers"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Pitch_to_TextTier)
	LOOP {
		iam (Pitch);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

/***** PITCH & PITCHTIER *****/

FORM (old_PitchTier_Pitch_draw, L"PitchTier & Pitch: Draw", 0)
	praat_dia_timeRange (dia);
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"500.0")
	RADIO (L"Line type for non-periodic intervals", 2)
		RADIOBUTTON (L"Normal")
		RADIOBUTTON (L"Dotted")
		RADIOBUTTON (L"Blank")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	PitchTier me = FIRST (PitchTier);
	Pitch thee = FIRST (Pitch);
	autoPraatPicture picture;
	PitchTier_Pitch_draw (me, thee, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_INTEGER (L"Line type for non-periodic intervals") - 1,
		GET_INTEGER (L"Garnish"), L"lines and speckles");
END

FORM (PitchTier_Pitch_draw, L"PitchTier & Pitch: Draw", 0)
	praat_dia_timeRange (dia);
	REAL (L"From frequency (Hz)", L"0.0")
	REAL (L"To frequency (Hz)", L"500.0")
	RADIO (L"Line type for non-periodic intervals", 2)
		RADIOBUTTON (L"Normal")
		RADIOBUTTON (L"Dotted")
		RADIOBUTTON (L"Blank")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
		OPTION (L"lines")
		OPTION (L"speckles")
		OPTION (L"lines and speckles")
	OK
DO_ALTERNATIVE (old_PitchTier_Pitch_draw)
	PitchTier me = FIRST (PitchTier);
	Pitch thee = FIRST (Pitch);
	autoPraatPicture picture;
	PitchTier_Pitch_draw (me, thee, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
		GET_INTEGER (L"Line type for non-periodic intervals") - 1,
		GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
END

DIRECT (Pitch_PitchTier_to_Pitch)
	Pitch pitch = FIRST (Pitch);
	PitchTier tier = FIRST (PitchTier);
	autoPitch thee = Pitch_PitchTier_to_Pitch (pitch, tier);
	praat_new (thee.transfer(), pitch -> name, L"_stylized");
END

/***** PITCH & POINTPROCESS *****/

DIRECT (Pitch_PointProcess_to_PitchTier)
	Pitch pitch = FIRST (Pitch);
	PointProcess point = FIRST (PointProcess);
	autoPitchTier thee = Pitch_PointProcess_to_PitchTier (pitch, point);
	praat_new (thee.transfer(), pitch -> name);
END

/***** PITCH & SOUND *****/

DIRECT (Sound_Pitch_to_Manipulation)
	Pitch pitch = FIRST (Pitch);
	Sound sound = FIRST (Sound);
	autoManipulation thee = Sound_Pitch_to_Manipulation (sound, pitch);
	praat_new (thee.transfer(), pitch -> name);
END

DIRECT (Sound_Pitch_to_PointProcess_cc)
	Sound sound = FIRST (Sound);
	Pitch pitch = FIRST (Pitch);
	autoPointProcess thee = Sound_Pitch_to_PointProcess_cc (sound, pitch);
	praat_new (thee.transfer(), sound -> name, L"_", pitch -> name);
END

FORM (Sound_Pitch_to_PointProcess_peaks, L"Sound & Pitch: To PointProcess (peaks)", 0)
	BOOLEAN (L"Include maxima", 1)
	BOOLEAN (L"Include minima", 0)
	OK
DO
	Sound sound = FIRST (Sound);
	Pitch pitch = FIRST (Pitch);
	autoPointProcess thee = Sound_Pitch_to_PointProcess_peaks (sound, pitch, GET_INTEGER (L"Include maxima"), GET_INTEGER (L"Include minima"));
	praat_new (thee.transfer(), sound -> name, L"_", pitch -> name);
END

/***** PITCHTIER *****/

FORM (PitchTier_addPoint, L"PitchTier: Add point", L"PitchTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	REAL (L"Pitch (Hz)", L"200")
	OK
DO
	LOOP {
		iam (PitchTier);
		RealTier_addPoint (me, GET_REAL (L"Time"), GET_REAL (L"Pitch")); therror
		praat_dataChanged (me);
	}
END

FORM (PitchTier_create, L"Create empty PitchTier", NULL)
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double startTime = GET_REAL (L"Start time"), endTime = GET_REAL (L"End time");
	if (endTime <= startTime) Melder_throw ("End time must be greater than start time.");
	autoPitchTier me = PitchTier_create (startTime, endTime);
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

DIRECT (PitchTier_downto_PointProcess)
	LOOP {
		iam (PitchTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (PitchTier_downto_TableOfReal, L"PitchTier: Down to TableOfReal", NULL)
	RADIO (L"Unit", 1)
	RADIOBUTTON (L"Hertz")
	RADIOBUTTON (L"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		autoTableOfReal thee = PitchTier_downto_TableOfReal (me, GET_INTEGER (L"Unit") - 1);
		praat_new (thee.transfer(), my name);
	}
END

FORM (old_PitchTier_draw, L"PitchTier: Draw", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	double minimumFrequency = GET_REAL (STRING_FROM_FREQUENCY);
	double maximumFrequency = GET_REAL (STRING_TO_FREQUENCY);
	if (maximumFrequency <= minimumFrequency) Melder_throw ("Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), minimumFrequency, maximumFrequency,
			GET_INTEGER (L"Garnish"), L"lines and speckles");
	}
END

FORM (PitchTier_draw, L"PitchTier: Draw", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
		OPTION (L"lines")
		OPTION (L"speckles")
		OPTION (L"lines and speckles")
	OK
DO_ALTERNATIVE (old_PitchTier_draw)
	double minimumFrequency = GET_REAL (STRING_FROM_FREQUENCY);
	double maximumFrequency = GET_REAL (STRING_TO_FREQUENCY);
	if (maximumFrequency <= minimumFrequency) Melder_throw ("Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), minimumFrequency, maximumFrequency,
			GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
	}
END

DIRECT (PitchTier_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a PitchTier from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPitchTier) {
		iam (PitchTier);
		autoPitchTierEditor editor = PitchTierEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me, sound, TRUE);
		praat_installEditor (editor.transfer(), IOBJECT); therror
	}
END

FORM (PitchTier_formula, L"PitchTier: Formula", L"PitchTier: Formula...")
	LABEL (L"", L"# ncol = the number of points")
	LABEL (L"", L"for col from 1 to ncol")
	LABEL (L"", L"   # x = the time of the colth point, in seconds")
	LABEL (L"", L"   # self = the value of the colth point, in hertz")
	LABEL (L"", L"   self = `formula'")
	LABEL (L"", L"endfor")
	TEXTFIELD (L"formula", L"self * 2 ; one octave up")
	OK
DO
	LOOP {
		iam (PitchTier);
		try {
			RealTier_formula (me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PitchTier may have partially changed
			throw;
		}
	}
END

FORM (PitchTier_getMean_curve, L"PitchTier: Get mean (curve)", L"PitchTier: Get mean (curve)...")
	praat_dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getMean_curve (FIRST_ANY (PitchTier), GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), L"Hz");
END
	
FORM (PitchTier_getMean_points, L"PitchTier: Get mean (points)", L"PitchTier: Get mean (points)...")
	praat_dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getMean_points (FIRST_ANY (PitchTier), GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), L"Hz");
END
	
FORM (PitchTier_getStandardDeviation_curve, L"PitchTier: Get standard deviation (curve)", L"PitchTier: Get standard deviation (curve)...")
	praat_dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_curve (FIRST_ANY (PitchTier), GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), L"Hz");
END
	
FORM (PitchTier_getStandardDeviation_points, L"PitchTier: Get standard deviation (points)", L"PitchTier: Get standard deviation (points)...")
	praat_dia_timeRange (dia);
	OK
DO
	Melder_informationReal (RealTier_getStandardDeviation_points (FIRST_ANY (PitchTier), GET_REAL (L"left Time range"), GET_REAL (L"right Time range")), L"Hz");
END
	
FORM (PitchTier_getValueAtTime, L"PitchTier: Get value at time", L"PitchTier: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_informationReal (RealTier_getValueAtTime (FIRST_ANY (PitchTier), GET_REAL (L"Time")), L"Hz");
END
	
FORM (PitchTier_getValueAtIndex, L"PitchTier: Get value at index", L"PitchTier: Get value at index...")
	INTEGER (L"Point number", L"10")
	OK
DO
	Melder_informationReal (RealTier_getValueAtIndex (FIRST_ANY (PitchTier), GET_INTEGER (L"Point number")), L"Hz");
END

DIRECT (PitchTier_help) Melder_help (L"PitchTier"); END

DIRECT (PitchTier_hum)
	LOOP {
		iam (PitchTier);
		PitchTier_hum (me); therror
	}
END

FORM (PitchTier_interpolateQuadratically, L"PitchTier: Interpolate quadratically", 0)
	NATURAL (L"Number of points per parabola", L"4")
	RADIO (L"Unit", 2)
	RADIOBUTTON (L"Hz")
	RADIOBUTTON (L"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		RealTier_interpolateQuadratically (me, GET_INTEGER (L"Number of points per parabola"), GET_INTEGER (L"Unit") - 1);
		praat_dataChanged (me);
	}
END

DIRECT (PitchTier_play)
	LOOP {
		iam (PitchTier);
		PitchTier_play (me); therror
	}
END

DIRECT (PitchTier_playSine)
	LOOP {
		iam (PitchTier);
		PitchTier_playPart_sine (me, 0.0, 0.0);
	}
END

FORM (PitchTier_shiftFrequencies, L"PitchTier: Shift frequencies", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1000.0")
	REAL (L"Frequency shift", L"-20.0")
	OPTIONMENU (L"Unit", 1)
		OPTION (L"Hertz")
		OPTION (L"mel")
		OPTION (L"logHertz")
		OPTION (L"semitones")
		OPTION (L"ERB")
	OK
DO
	int unit = GET_INTEGER (L"Unit");
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	LOOP {
		iam (PitchTier);
		try {
			PitchTier_shiftFrequencies (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Frequency shift"), unit); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PitchTier may have partially changed
			throw;
		}
	}
END

FORM (PitchTier_multiplyFrequencies, L"PitchTier: Multiply frequencies", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1000.0")
	POSITIVE (L"Factor", L"1.2")
	OK
DO
	LOOP {
		iam (PitchTier);
		PitchTier_multiplyFrequencies (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Factor"));
		praat_dataChanged (me);
	}
END

FORM (PitchTier_stylize, L"PitchTier: Stylize", L"PitchTier: Stylize...")
	REAL (L"Frequency resolution", L"4.0")
	RADIO (L"Unit", 2)
	RADIOBUTTON (L"Hz")
	RADIOBUTTON (L"Semitones")
	OK
DO
	LOOP {
		iam (PitchTier);
		PitchTier_stylize (me, GET_REAL (L"Frequency resolution"), GET_INTEGER (L"Unit") - 1);
		praat_dataChanged (me);
	}
END

DIRECT (PitchTier_to_PointProcess)
	LOOP {
		iam (PitchTier);
		autoPointProcess thee = PitchTier_to_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (PitchTier_to_Sound_phonation, L"PitchTier: To Sound (phonation)", 0)
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	POSITIVE (L"Adaptation factor", L"1.0")
	POSITIVE (L"Maximum period (s)", L"0.05")
	POSITIVE (L"Open phase", L"0.7")
	REAL (L"Collision phase", L"0.03")
	POSITIVE (L"Power 1", L"3.0")
	POSITIVE (L"Power 2", L"4.0")
	BOOLEAN (L"Hum", 0)
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_phonation (me, GET_REAL (L"Sampling frequency"),
			GET_REAL (L"Adaptation factor"), GET_REAL (L"Maximum period"),
			GET_REAL (L"Open phase"), GET_REAL (L"Collision phase"), GET_REAL (L"Power 1"), GET_REAL (L"Power 2"), GET_INTEGER (L"Hum"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PitchTier_to_Sound_pulseTrain, L"PitchTier: To Sound (pulse train)", 0)
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	POSITIVE (L"Adaptation factor", L"1.0")
	POSITIVE (L"Adaptation time", L"0.05")
	NATURAL (L"Interpolation depth (samples)", L"2000")
	BOOLEAN (L"Hum", 0)
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_pulseTrain (me, GET_REAL (L"Sampling frequency"),
			GET_REAL (L"Adaptation factor"), GET_REAL (L"Adaptation time"),
			GET_INTEGER (L"Interpolation depth"), GET_INTEGER (L"Hum"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PitchTier_to_Sound_sine, L"PitchTier: To Sound (sine)", 0)
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	OK
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_sine (me, 0.0, 0.0, GET_REAL (L"Sampling frequency"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (info_PitchTier_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your PitchTier editor:\n"
		"   select a PitchTier and a Sound, and click \"View & Edit\".");
END

FORM_WRITE (PitchTier_writeToPitchTierSpreadsheetFile, L"Save PitchTier as spreadsheet", 0, L"PitchTier")
	LOOP {
		iam (PitchTier);
		PitchTier_writeToPitchTierSpreadsheetFile (me, file); therror
	}
END

FORM_WRITE (PitchTier_writeToHeaderlessSpreadsheetFile, L"Save PitchTier as spreadsheet", 0, L"txt")
	LOOP {
		iam (PitchTier);
		PitchTier_writeToHeaderlessSpreadsheetFile (me, file); therror
	}
END

/***** PITCHTIER & POINTPROCESS *****/

DIRECT (PitchTier_PointProcess_to_PitchTier)
	PitchTier pitch = FIRST (PitchTier);
	PointProcess point = FIRST (PointProcess);
	autoPitchTier thee = PitchTier_PointProcess_to_PitchTier (pitch, point);
	praat_new (thee.transfer(), pitch -> name);
END

/***** POINTPROCESS *****/

FORM (PointProcess_addPoint, L"PointProcess: Add point", L"PointProcess: Add point...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_addPoint (me, GET_REAL (L"Time")); therror
		praat_dataChanged (me);
	}
END

FORM (PointProcess_createEmpty, L"Create an empty PointProcess", L"Create empty PointProcess...")
	WORD (L"Name", L"empty")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	OK
DO
	double tmin = GET_REAL (L"Start time"), tmax = GET_REAL (L"End time");
	if (tmax < tmin) Melder_throw ("End time (", tmax, ") should not be less than start time (", tmin, ").");
	autoPointProcess me = PointProcess_create (tmin, tmax, 0);
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (PointProcess_createPoissonProcess, L"Create Poisson process", L"Create Poisson process...")
	WORD (L"Name", L"poisson")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	POSITIVE (L"Density (/s)", L"100.0")
	OK
DO
	double tmin = GET_REAL (L"Start time"), tmax = GET_REAL (L"End time");
	if (tmax < tmin)
		Melder_throw ("End time (", tmax, ") should not be less than start time (", tmin, ").");
	autoPointProcess me = PointProcess_createPoissonProcess (tmin, tmax, GET_REAL (L"Density"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

DIRECT (PointProcess_difference)
	PointProcess point1 = NULL, point2 = NULL;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_difference (point1, point2);
	praat_new (thee.transfer(), L"difference");
END

FORM (PointProcess_draw, L"PointProcess: Draw", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPraatPicture picture;
		PointProcess_draw (me, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (PointProcess_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a PointProcess from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPointProcess) {
		iam (PointProcess);
		autoPointEditor editor = PointEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me, sound);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (PointProcess_fill, L"PointProcess: Fill", 0)
	praat_dia_timeRange (dia);
	POSITIVE (L"Period (s)", L"0.01")
	OK
DO
	LOOP {
		iam (PointProcess);
		try {
			PointProcess_fill (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_REAL (L"Period")); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PointProcess may have partially changed
			throw;
		}
	}
END

FORM (PointProcess_getInterval, L"PointProcess: Get interval", L"PointProcess: Get interval...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_informationReal (PointProcess_getInterval (FIRST_ANY (PointProcess), GET_REAL (L"Time")), L"seconds");
END

static void dia_PointProcess_getRangeProperty (Any dia) {
	praat_dia_timeRange (dia);
	REAL (L"Shortest period (s)", L"0.0001")
	REAL (L"Longest period (s)", L"0.02")
	POSITIVE (L"Maximum period factor", L"1.3")
}

FORM (PointProcess_getJitter_local, L"PointProcess: Get jitter (local)", L"PointProcess: Get jitter (local)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_local_absolute, L"PointProcess: Get jitter (local, absolute)", L"PointProcess: Get jitter (local, absolute)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_local_absolute (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), L"seconds");
END

FORM (PointProcess_getJitter_rap, L"PointProcess: Get jitter (rap)", L"PointProcess: Get jitter (rap)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_rap (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_ppq5, L"PointProcess: Get jitter (ppq5)", L"PointProcess: Get jitter (ppq5)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ppq5 (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), NULL);
END

FORM (PointProcess_getJitter_ddp, L"PointProcess: Get jitter (ddp)", L"PointProcess: Get jitter (ddp)...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getJitter_ddp (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), NULL);
END

FORM (PointProcess_getMeanPeriod, L"PointProcess: Get mean period", L"PointProcess: Get mean period...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getMeanPeriod (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), L"seconds");
END

FORM (PointProcess_getStdevPeriod, L"PointProcess: Get stdev period", L"PointProcess: Get stdev period...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_informationReal (PointProcess_getStdevPeriod (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor")), L"seconds");
END

FORM (PointProcess_getLowIndex, L"PointProcess: Get low index", L"PointProcess: Get low index...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_information (Melder_integer (PointProcess_getLowIndex (FIRST_ANY (PointProcess), GET_REAL (L"Time"))));
END

FORM (PointProcess_getHighIndex, L"PointProcess: Get high index", L"PointProcess: Get high index...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_information (Melder_integer (PointProcess_getHighIndex (FIRST_ANY (PointProcess), GET_REAL (L"Time"))));
END

FORM (PointProcess_getNearestIndex, L"PointProcess: Get nearest index", L"PointProcess: Get nearest index...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	Melder_information (Melder_integer (PointProcess_getNearestIndex (FIRST_ANY (PointProcess), GET_REAL (L"Time"))));
END

DIRECT (PointProcess_getNumberOfPoints)
	PointProcess me = FIRST_ANY (PointProcess);
	Melder_information (Melder_integer (my nt));
END

FORM (PointProcess_getNumberOfPeriods, L"PointProcess: Get number of periods", L"PointProcess: Get number of periods...")
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	Melder_information (Melder_integer (PointProcess_getNumberOfPeriods (FIRST_ANY (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor"))));
END

FORM (PointProcess_getTimeFromIndex, L"Get time", 0 /*"PointProcess: Get time from index..."*/)
	NATURAL (L"Point number", L"10")
	OK
DO
	PointProcess me = FIRST_ANY (PointProcess);
	long i = GET_INTEGER (L"Point number");
	if (i > my nt) Melder_information (L"--undefined--");
	else Melder_informationReal (my t [i], L"seconds");
END

DIRECT (PointProcess_help) Melder_help (L"PointProcess"); END

DIRECT (PointProcess_hum)
	LOOP {
		iam (PointProcess);
		PointProcess_hum (me, my xmin, my xmax); therror
	}
END

DIRECT (PointProcess_intersection)
	PointProcess point1 = NULL, point2 = NULL;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_intersection (point1, point2);
	praat_new (thee.transfer(), L"intersection");
END

DIRECT (PointProcess_play)
	LOOP {
		iam (PointProcess);
		PointProcess_play (me); therror
	}
END

FORM (PointProcess_removePoint, L"PointProcess: Remove point", L"PointProcess: Remove point...")
	NATURAL (L"Index", L"1")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoint (me, GET_INTEGER (L"Index")); therror
		praat_dataChanged (me);
	}
END

FORM (PointProcess_removePointNear, L"PointProcess: Remove point near", L"PointProcess: Remove point near...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointNear (me, GET_REAL (L"Time")); therror
		praat_dataChanged (me);
	}
END

FORM (PointProcess_removePoints, L"PointProcess: Remove points", L"PointProcess: Remove points...")
	NATURAL (L"From index", L"1")
	NATURAL (L"To index", L"10")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoints (me, GET_INTEGER (L"From index"), GET_INTEGER (L"To index")); therror
		praat_dataChanged (me);
	}
END

FORM (PointProcess_removePointsBetween, L"PointProcess: Remove points between", L"PointProcess: Remove points between...")
	REAL (L"left Time range (s)", L"0.3")
	REAL (L"right Time range (s)", L"0.7")
	OK
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointsBetween (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range")); therror
		praat_dataChanged (me);
	}
END

DIRECT (PointProcess_to_IntervalTier)
	LOOP {
		iam (PointProcess);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PointProcess_to_Matrix)
	LOOP {
		iam (PointProcess);
		autoMatrix thee = PointProcess_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_to_PitchTier, L"PointProcess: To PitchTier", L"PointProcess: To PitchTier...")
	POSITIVE (L"Maximum interval (s)", L"0.02")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_to_PitchTier (me, GET_REAL (L"Maximum interval"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_to_TextGrid, L"PointProcess: To TextGrid...", L"PointProcess: To TextGrid...")
	SENTENCE (L"Tier names", L"Mary John bell")
	SENTENCE (L"Point tiers", L"bell")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (L"Tier names"), GET_STRING (L"Point tiers"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_to_TextGrid_vuv, L"PointProcess: To TextGrid (vuv)...", L"PointProcess: To TextGrid (vuv)...")
	POSITIVE (L"Maximum period (s)", L"0.02")
	REAL (L"Mean period (s)", L"0.01")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = PointProcess_to_TextGrid_vuv (me, GET_REAL (L"Maximum period"), GET_REAL (L"Mean period"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PointProcess_to_TextTier)
	LOOP {
		iam (PointProcess);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_to_Sound_phonation, L"PointProcess: To Sound (phonation)", L"PointProcess: To Sound (phonation)...")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	POSITIVE (L"Adaptation factor", L"1.0")
	POSITIVE (L"Maximum period (s)", L"0.05")
	POSITIVE (L"Open phase", L"0.7")
	REAL (L"Collision phase", L"0.03")
	POSITIVE (L"Power 1", L"3.0")
	POSITIVE (L"Power 2", L"4.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_phonation (me, GET_REAL (L"Sampling frequency"),
			GET_REAL (L"Adaptation factor"), GET_REAL (L"Maximum period"),
			GET_REAL (L"Open phase"), GET_REAL (L"Collision phase"), GET_REAL (L"Power 1"), GET_REAL (L"Power 2"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_to_Sound_pulseTrain, L"PointProcess: To Sound (pulse train)", L"PointProcess: To Sound (pulse train)...")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	POSITIVE (L"Adaptation factor", L"1.0")
	POSITIVE (L"Adaptation time (s)", L"0.05")
	NATURAL (L"Interpolation depth (samples)", L"2000")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_pulseTrain (me, GET_REAL (L"Sampling frequency"),
			GET_REAL (L"Adaptation factor"), GET_REAL (L"Adaptation time"),
			GET_INTEGER (L"Interpolation depth"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PointProcess_to_Sound_hum)
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_hum (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (PointProcess_union)
	PointProcess point1 = NULL, point2 = NULL;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_union (point1, point2);
	praat_new (thee.transfer(), L"union");
END

FORM (PointProcess_upto_IntensityTier, L"PointProcess: Up to IntensityTier", L"PointProcess: Up to IntensityTier...")
	POSITIVE (L"Intensity (dB)", L"70.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoIntensityTier thee = PointProcess_upto_IntensityTier (me, GET_REAL (L"Intensity"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_upto_PitchTier, L"PointProcess: Up to PitchTier", L"PointProcess: Up to PitchTier...")
	POSITIVE (L"Frequency (Hz)", L"190.0")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_upto_PitchTier (me, GET_REAL (L"Frequency"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_upto_TextTier, L"PointProcess: Up to TextTier", L"PointProcess: Up to TextTier...")
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (PointProcess);
		autoTextTier thee = PointProcess_upto_TextTier (me, GET_STRING (L"Text"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (PointProcess_voice, L"PointProcess: Fill unvoiced parts", 0)
	POSITIVE (L"Period (s)", L"0.01")
	POSITIVE (L"Maximum voiced period (s)", L"0.02000000001")
	OK
DO
	LOOP {
		iam (PointProcess);
		try {
			PointProcess_voice (me, GET_REAL (L"Period"), GET_REAL (L"Maximum voiced period")); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PointProcess may have partially changed
			throw;
		}
	}
END

DIRECT (info_PointProcess_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your PointProcess editor:\n"
		"   select a PointProcess and a Sound, and click \"View & Edit\".");
END

/***** POINTPROCESS & SOUND *****/

/*DIRECT (Sound_PointProcess_to_Manipulation)
	Sound sound = ONLY (classSound);
	PointProcess point = ONLY (classPointProcess);
	if (! praat_new1 (Sound_PointProcess_to_Manipulation (sound, point), point -> name)) return 0;
END*/

DIRECT (Point_Sound_transplantDomain)
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	point -> xmin = sound -> xmin;
	point -> xmax = sound -> xmax;
	praat_dataChanged (point);
END

FORM (Point_Sound_getShimmer_local, L"PointProcess & Sound: Get shimmer (local)", L"PointProcess & Sound: Get shimmer (local)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (Point_Sound_getShimmer_local_dB, L"PointProcess & Sound: Get shimmer (local, dB)", L"PointProcess & Sound: Get shimmer (local, dB)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local_dB (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (Point_Sound_getShimmer_apq3, L"PointProcess & Sound: Get shimmer (apq3)", L"PointProcess & Sound: Get shimmer (apq3)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq3 (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (Point_Sound_getShimmer_apq5, L"PointProcess & Sound: Get shimmer (apq)", L"PointProcess & Sound: Get shimmer (apq5)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq5 (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (Point_Sound_getShimmer_apq11, L"PointProcess & Sound: Get shimmer (apq11)", L"PointProcess & Sound: Get shimmer (apq11)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq11 (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (Point_Sound_getShimmer_dda, L"PointProcess & Sound: Get shimmer (dda)", L"PointProcess & Sound: Get shimmer (dda)...")
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_dda (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"));
	Melder_informationReal (shimmer, NULL);
END

FORM (PointProcess_Sound_to_AmplitudeTier_period, L"PointProcess & Sound: To AmplitudeTier (period)", 0)
	dia_PointProcess_getRangeProperty (dia);
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_period (point, sound,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor"));
	praat_new (thee.transfer(), sound -> name, L"_", point -> name);
END

DIRECT (PointProcess_Sound_to_AmplitudeTier_point)
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_point (point, sound);
	praat_new (thee.transfer(), sound -> name, L"_", point -> name);
END

FORM (PointProcess_Sound_to_Ltas, L"PointProcess & Sound: To Ltas", 0)
	POSITIVE (L"Maximum frequency (Hz)", L"5000")
	POSITIVE (L"Band width (Hz)", L"100")
	REAL (L"Shortest period (s)", L"0.0001")
	REAL (L"Longest period (s)", L"0.02")
	POSITIVE (L"Maximum period factor", L"1.3")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas (point, sound,
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Band width"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor"));
	praat_new (thee.transfer(), sound -> name);
END

FORM (PointProcess_Sound_to_Ltas_harmonics, L"PointProcess & Sound: To Ltas (harmonics", 0)
	NATURAL (L"Maximum harmonic", L"20")
	REAL (L"Shortest period (s)", L"0.0001")
	REAL (L"Longest period (s)", L"0.02")
	POSITIVE (L"Maximum period factor", L"1.3")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas_harmonics (point, sound,
		GET_INTEGER (L"Maximum harmonic"),
		GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor"));
	praat_new (thee.transfer(), sound -> name);
END

FORM (Sound_PointProcess_to_SoundEnsemble_correlate, L"Sound & PointProcess: To SoundEnsemble (correlate)", 0)
	REAL (L"From time (s)", L"-0.1")
	REAL (L"To time (s)", L"1.0")
	OK
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoSound thee = Sound_PointProcess_to_SoundEnsemble_correlate (sound, point, GET_REAL (L"From time"), GET_REAL (L"To time"));
	praat_new (thee.transfer(), point -> name);
END

/***** POLYGON *****/

FORM (Polygon_draw, L"Polygon: Draw", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_draw (me, GRAPHICS, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"));
	}
END

FORM (Polygon_drawCircles, L"Polygon: Draw circles", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0 (= all)")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0 (= all)")
	POSITIVE (L"Diameter (mm)", L"3")
	OK
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_drawCircles (me, GRAPHICS,
			GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"),
			GET_REAL (L"Diameter"));
	}
END

FORM (Polygon_drawClosed, L"Polygon: Draw", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0")
	OK
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_drawClosed (me, GRAPHICS, GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"));
	}
END

FORM (Polygons_drawConnection, L"Polygons: Draw connection", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0 (= all)")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0 (= all)")
	BOOLEAN (L"Arrow", 0)
	POSITIVE (L"Relative length", L"0.9")
	OK
DO
	Polygon polygon1 = NULL, polygon2 = NULL;
	LOOP (polygon1 ? polygon2 : polygon1) = (Polygon) OBJECT;
	autoPraatPicture picture;
	Polygons_drawConnection (polygon1, polygon2, GRAPHICS,
		GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"),
		GET_INTEGER (L"Arrow"), GET_REAL (L"Relative length"));
END

DIRECT (Polygon_help) Melder_help (L"Polygon"); END

FORM (Polygon_paint, L"Polygon: Paint", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0 (= all)")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_paint (me, GRAPHICS, GET_COLOUR (L"Colour"), GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"));
	}
END

FORM (Polygon_paintCircles, L"Polygon: Paint circles", 0)
	REAL (L"Xmin", L"0.0")
	REAL (L"Xmax", L"0.0 (= all)")
	REAL (L"Ymin", L"0.0")
	REAL (L"Ymax", L"0.0 (= all)")
	POSITIVE (L"Diameter (mm)", L"3")
	OK
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_paintCircles (me, GRAPHICS,
			GET_REAL (L"Xmin"), GET_REAL (L"Xmax"), GET_REAL (L"Ymin"), GET_REAL (L"Ymax"), GET_REAL (L"Diameter"));
	}
END

DIRECT (Polygon_randomize)
	LOOP {
		iam (Polygon);
		Polygon_randomize (me);
		praat_dataChanged (me);
	}
END

FORM (Polygon_salesperson, L"Polygon: Find shortest path", 0)
	NATURAL (L"Number of iterations", L"1")
	OK
DO
	LOOP {
		iam (Polygon);
		Polygon_salesperson (me, GET_INTEGER (L"Number of iterations"));
		praat_dataChanged (me);
	}
END

DIRECT (Polygon_to_Matrix)
	LOOP {
		iam (Polygon);
		autoMatrix thee = Polygon_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** SOUND & PITCH & POINTPROCESS *****/

FORM (Sound_Pitch_PointProcess_voiceReport, L"Voice report", L"Voice")
	praat_dia_timeRange (dia);
	POSITIVE (L"left Pitch range (Hz)", L"75.0")
	POSITIVE (L"right Pitch range (Hz)", L"600.0")
	POSITIVE (L"Maximum period factor", L"1.3")
	POSITIVE (L"Maximum amplitude factor", L"1.6")
	REAL (L"Silence threshold", L"0.03")
	REAL (L"Voicing threshold", L"0.45")
	OK
DO
	MelderInfo_open ();
	Sound_Pitch_PointProcess_voiceReport (FIRST (Sound), FIRST (Pitch), FIRST (PointProcess),
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
		GET_REAL (L"left Pitch range"), GET_REAL (L"right Pitch range"),
		GET_REAL (L"Maximum period factor"), GET_REAL (L"Maximum amplitude factor"),
		GET_REAL (L"Silence threshold"), GET_REAL (L"Voicing threshold"));
	MelderInfo_close ();
END

/***** SOUND & POINTPROCESS & PITCHTIER & DURATIONTIER *****/

FORM (Sound_Point_Pitch_Duration_to_Sound, L"To Sound", 0)
	POSITIVE (L"Longest period (s)", L"0.02")
	OK
DO
	autoSound thee = Sound_Point_Pitch_Duration_to_Sound (FIRST (Sound), FIRST (PointProcess),
		FIRST (PitchTier), FIRST (DurationTier), GET_REAL (L"Longest period"));
	praat_new (thee.transfer(), L"manip");
END

/***** SPECTROGRAM *****/

FORM (Spectrogram_paint, L"Spectrogram: Paint", L"Spectrogram: Paint...")
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (= all)")
	REAL (L"Maximum (dB/Hz)", L"100.0")
	BOOLEAN (L"Autoscaling", 1)
	POSITIVE (L"Dynamic range (dB)", L"50.0")
	REAL (L"Pre-emphasis (dB/oct)", L"6.0")
	REAL (L"Dynamic compression (0-1)", L"0.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoPraatPicture picture;
		Spectrogram_paint (me, GRAPHICS, GET_REAL (L"left Time range"),
			GET_REAL (L"right Time range"), GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"Maximum"), GET_INTEGER (L"Autoscaling"),
			GET_REAL (L"Dynamic range"), GET_REAL (L"Pre-emphasis"),
			GET_REAL (L"Dynamic compression"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Spectrogram_formula, L"Spectrogram: Formula", L"Spectrogram: Formula...")
	LABEL (L"label", L"Do for all times and frequencies:")
	LABEL (L"label", L"   `x' is the time in seconds")
	LABEL (L"label", L"   `y' is the frequency in hertz")
	LABEL (L"label", L"   `self' is the current value in Pa\u00B2/Hz")
	LABEL (L"label", L"   Replace all values with:")
	TEXTFIELD (L"formula", L"self * exp (- x / 0.1)")
	OK
DO
	LOOP {
		iam (Spectrogram);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Spectrogram may have partially changed
			throw;
		}
	}
END

FORM (Spectrogram_getPowerAt, L"Spectrogram: Get power at (time, frequency)", 0)
	REAL (L"Time (s)", L"0.5")
	REAL (L"Frequency (Hz)", L"1000")
	OK
DO
	Spectrogram me = FIRST_ANY (Spectrogram);
	double time = GET_REAL (L"Time"), frequency = GET_REAL (L"Frequency");
	MelderInfo_open ();
	MelderInfo_write1 (Melder_double (Matrix_getValueAtXY (me, time, frequency)));
	MelderInfo_write5 (L" Pa2/Hz (at time = ", Melder_double (time), L" seconds and frequency = ", Melder_double (frequency), L" Hz)");
	MelderInfo_close ();
END

DIRECT (Spectrogram_help) Melder_help (L"Spectrogram"); END

DIRECT (Spectrogram_movie)
	Graphics g = Movie_create (L"Spectrogram movie", 300, 300);
	LOOP {
		iam (Spectrogram);
		Matrix_movie (me, g);
	}
END

DIRECT (Spectrogram_to_Matrix)
	LOOP {
		iam (Spectrogram);
		autoMatrix thee = Spectrogram_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrogram_to_Sound, L"Spectrogram: To Sound", 0)
	REAL (L"Sampling frequency (Hz)", L"44100")
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoSound thee = Spectrogram_to_Sound (me, GET_REAL (L"Sampling frequency"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrogram_to_Spectrum, L"Spectrogram: To Spectrum (slice)", 0)
	REAL (L"Time (seconds)", L"0.0")
	OK
DO
	LOOP {
		iam (Spectrogram);
		autoSpectrum thee = Spectrogram_to_Spectrum (me, GET_REAL (L"Time"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrogram_view)
	if (theCurrentPraatApplication -> batch) Melder_throw (L"Cannot view or edit a Spectrogram from batch.");
	LOOP {
		iam (Spectrogram);
		autoSpectrogramEditor editor = SpectrogramEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT); therror
	}
END

/***** SPECTRUM *****/

FORM (Spectrum_cepstralSmoothing, L"Spectrum: Cepstral smoothing", 0)
	POSITIVE (L"Bandwidth (Hz)", L"500.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_cepstralSmoothing (me, GET_REAL (L"Bandwidth"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrum_draw, L"Spectrum: Draw", 0)
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"0.0 (= all)")
	REAL (L"Minimum power (dB/Hz)", L"0 (= auto)")
	REAL (L"Maximum power (dB/Hz)", L"0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Spectrum);
		autoPraatPicture picture;
		Spectrum_draw (me, GRAPHICS, GET_REAL (L"left Frequency range"),
			GET_REAL (L"right Frequency range"), GET_REAL (L"Minimum power"), GET_REAL (L"Maximum power"),
			GET_INTEGER (L"Garnish"));
	}
END

FORM (Spectrum_drawLogFreq, L"Spectrum: Draw (log freq)", 0)
	POSITIVE (L"left Frequency range (Hz)", L"10.0")
	POSITIVE (L"right Frequency range (Hz)", L"10000.0")
	REAL (L"Minimum power (dB/Hz)", L"0 (= auto)")
	REAL (L"Maximum power (dB/Hz)", L"0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Spectrum);
		autoPraatPicture picture;
		Spectrum_drawLogFreq (me, GRAPHICS, GET_REAL (L"left Frequency range"),
			GET_REAL (L"right Frequency range"), GET_REAL (L"Minimum power"), GET_REAL (L"Maximum power"),
			GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Spectrum_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Spectrum from batch.");
	LOOP {
		iam (Spectrum);
		autoSpectrumEditor editor = SpectrumEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT); therror
	}
END

FORM (Spectrum_formula, L"Spectrum: Formula", L"Spectrum: Formula...")
	LABEL (L"label", L"`x' is the frequency in hertz, `col' is the bin number;   "
		"`y' = `row' is 1 (real part) or 2 (imaginary part)")
	LABEL (L"label", L"y := 1;   row := 1;   "
		"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	LABEL (L"label", L"y := 2;   row := 2;   "
		"x := 0;   for col := 1 to ncol do { self [2, col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"0")
	OK
DO
	LOOP {
		iam (Spectrum);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Spectrum may have partially changed
			throw;
		}
	}
END

FORM (Spectrum_getBandDensity, L"Spectrum: Get band density", 0)
	REAL (L"Band floor (Hz)", L"200.0")
	REAL (L"Band ceiling (Hz)", L"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double density = Spectrum_getBandDensity (me, GET_REAL (L"Band floor"), GET_REAL (L"Band ceiling"));
		Melder_informationReal (density, L"Pa2 / Hz2");
	}
END

FORM (Spectrum_getBandDensityDifference, L"Spectrum: Get band density difference", 0)
	REAL (L"Low band floor (Hz)", L"0")
	REAL (L"Low band ceiling (Hz)", L"500")
	REAL (L"High band floor (Hz)", L"500")
	REAL (L"High band ceiling (Hz)", L"4000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandDensityDifference (me,
			GET_REAL (L"Low band floor"), GET_REAL (L"Low band ceiling"), GET_REAL (L"High band floor"), GET_REAL (L"High band ceiling"));
		Melder_informationReal (difference, L"dB");
	}
END

FORM (Spectrum_getBandEnergy, L"Spectrum: Get band energy", 0)
	REAL (L"Band floor (Hz)", L"200.0")
	REAL (L"Band ceiling (Hz)", L"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double energy = Spectrum_getBandEnergy (me, GET_REAL (L"Band floor"), GET_REAL (L"Band ceiling"));
		Melder_informationReal (energy, L"Pa2 sec");
	}
END

FORM (Spectrum_getBandEnergyDifference, L"Spectrum: Get band energy difference", 0)
	REAL (L"Low band floor (Hz)", L"0")
	REAL (L"Low band ceiling (Hz)", L"500")
	REAL (L"High band floor (Hz)", L"500")
	REAL (L"High band ceiling (Hz)", L"4000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandEnergyDifference (me,
			GET_REAL (L"Low band floor"), GET_REAL (L"Low band ceiling"), GET_REAL (L"High band floor"), GET_REAL (L"High band ceiling"));
		Melder_informationReal (difference, L"dB");
	}
END	

FORM (Spectrum_getBinFromFrequency, L"Spectrum: Get bin from frequency", 0)
	REAL (L"Frequency (Hz)", L"2000")
	OK
DO
	LOOP {
		iam (Spectrum);
		double bin = Sampled_xToIndex (me, GET_REAL (L"Frequency"));
		Melder_informationReal (bin, NULL);
	}
END

DIRECT (Spectrum_getBinWidth)
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my dx, L"hertz");
	}
END

FORM (Spectrum_getCentralMoment, L"Spectrum: Get central moment", L"Spectrum: Get central moment...")
	POSITIVE (L"Moment", L"3.0")
	POSITIVE (L"Power", L"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double moment = Spectrum_getCentralMoment (me, GET_REAL (L"Moment"), GET_REAL (L"Power"));
		Melder_informationReal (moment, L"hertz to the power 'moment'");
	}
END

FORM (Spectrum_getCentreOfGravity, L"Spectrum: Get centre of gravity", L"Spectrum: Get centre of gravity...")
	POSITIVE (L"Power", L"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double centreOfGravity = Spectrum_getCentreOfGravity (me, GET_REAL (L"Power"));
		Melder_informationReal (centreOfGravity, L"hertz");
	}
END

FORM (Spectrum_getFrequencyFromBin, L"Spectrum: Get frequency from bin", 0)
	NATURAL (L"Band number", L"1")
	OK
DO
	LOOP {
		iam (Spectrum);
		double frequency = Sampled_indexToX (me, GET_INTEGER (L"Band number"));
		Melder_informationReal (frequency, L"hertz");
	}
END

DIRECT (Spectrum_getHighestFrequency)
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmax, L"hertz");
	}
END

FORM (Spectrum_getImaginaryValueInBin, L"Spectrum: Get imaginary value in bin", 0)
	NATURAL (L"Bin number", L"100")
	OK
DO
	long binNumber = GET_INTEGER (L"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw ("Bin number must not exceed number of bins.");
		Melder_informationReal (my z [2] [binNumber], NULL);
	}
END

FORM (Spectrum_getKurtosis, L"Spectrum: Get kurtosis", L"Spectrum: Get kurtosis...")
	POSITIVE (L"Power", L"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double kurtosis = Spectrum_getKurtosis (me, GET_REAL (L"Power"));
		Melder_informationReal (kurtosis, NULL);
	}
END

DIRECT (Spectrum_getLowestFrequency)
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmin, L"hertz");
	}
END

DIRECT (Spectrum_getNumberOfBins)
	LOOP {
		iam (Spectrum);
		Melder_information (Melder_integer (my nx), L" bins");
	}
END

FORM (Spectrum_getRealValueInBin, L"Spectrum: Get real value in bin", 0)
	NATURAL (L"Bin number", L"100")
	OK
DO
	long binNumber = GET_INTEGER (L"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw ("Bin number must not exceed number of bins.");
		Melder_informationReal (my z [1] [binNumber], NULL);
	}
END

FORM (Spectrum_getSkewness, L"Spectrum: Get skewness", L"Spectrum: Get skewness...")
	POSITIVE (L"Power", L"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double skewness = Spectrum_getSkewness (me, GET_REAL (L"Power"));
		Melder_informationReal (skewness, NULL);
	}
END

FORM (Spectrum_getStandardDeviation, L"Spectrum: Get standard deviation", L"Spectrum: Get standard deviation...")
	POSITIVE (L"Power", L"2.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		double stdev = Spectrum_getStandardDeviation (me, GET_REAL (L"Power"));
		Melder_informationReal (stdev, L"hertz");
	}
END

DIRECT (Spectrum_help) Melder_help (L"Spectrum"); END

FORM (Spectrum_list, L"Spectrum: List", 0)
	BOOLEAN (L"Include bin number", false)
	BOOLEAN (L"Include frequency", true)
	BOOLEAN (L"Include real part", false)
	BOOLEAN (L"Include imaginary part", false)
	BOOLEAN (L"Include energy density", false)
	BOOLEAN (L"Include power density", true)
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_list (me, GET_INTEGER (L"Include bin number"), GET_INTEGER (L"Include frequency"),
			GET_INTEGER (L"Include real part"), GET_INTEGER (L"Include imaginary part"),
			GET_INTEGER (L"Include energy density"), GET_INTEGER (L"Include power density"));
	}
END

FORM (Spectrum_lpcSmoothing, L"Spectrum: LPC smoothing", 0)
	NATURAL (L"Number of peaks", L"5")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_lpcSmoothing (me, GET_INTEGER (L"Number of peaks"), GET_REAL (L"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrum_passHannBand, L"Spectrum: Filter (pass Hann band)", L"Spectrum: Filter (pass Hann band)...")
	REAL (L"From frequency (Hz)", L"500")
	REAL (L"To frequency (Hz)", L"1000")
	POSITIVE (L"Smoothing (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_passHannBand (me, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_REAL (L"Smoothing"));
		praat_dataChanged (me);
	}
END

FORM (Spectrum_stopHannBand, L"Spectrum: Filter (stop Hann band)", L"Spectrum: Filter (stop Hann band)...")
	REAL (L"From frequency (Hz)", L"500")
	REAL (L"To frequency (Hz)", L"1000")
	POSITIVE (L"Smoothing (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Spectrum);
		Spectrum_stopHannBand (me, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_REAL (L"Smoothing"));
		praat_dataChanged (me);
	}
END

FORM (Spectrum_to_Excitation, L"Spectrum: To Excitation", 0)
	POSITIVE (L"Frequency resolution (Bark)", L"0.1")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoExcitation thee = Spectrum_to_Excitation (me, GET_REAL (L"Frequency resolution"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrum_to_Formant_peaks, L"Spectrum: To Formant (peaks)", 0)
	LABEL (L"", L"Warning: this simply picks peaks from 0 Hz up!")
	NATURAL (L"Maximum number of formants", L"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoFormant thee = Spectrum_to_Formant (me, GET_INTEGER (L"Maximum number of formants"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Spectrum_to_Ltas, L"Spectrum: To Long-term average spectrum", 0)
	POSITIVE (L"Bandwidth (Hz)", L"1000")
	OK
DO
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas (me, GET_REAL (L"Bandwidth"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrum_to_Ltas_1to1)
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas_1to1 (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrum_to_Matrix)
	LOOP {
		iam (Spectrum);
		autoMatrix thee = Spectrum_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrum_to_Sound)
	LOOP {
		iam (Spectrum);
		autoSound thee = Spectrum_to_Sound (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrum_to_Spectrogram)
	LOOP {
		iam (Spectrum);
		autoSpectrogram thee = Spectrum_to_Spectrogram (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Spectrum_to_SpectrumTier_peaks)
	LOOP {
		iam (Spectrum);
		autoSpectrumTier thee = Spectrum_to_SpectrumTier_peaks (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** SPECTRUMTIER *****/

DIRECT (SpectrumTier_downto_Table)
	LOOP {
		iam (SpectrumTier);
		autoTable thee = SpectrumTier_downto_Table (me, true, true, true);
		praat_new (thee.transfer(), my name);
	}
END

FORM (old_SpectrumTier_draw, L"SpectrumTier: Draw", 0)   // 2010/10/19
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"10000.0")
	REAL (L"left Power range (dB)", L"20.0")
	REAL (L"right Power range (dB)", L"80.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"left Power range"), GET_REAL (L"right Power range"),
			GET_INTEGER (L"Garnish"), L"lines and speckles");
	}
END

FORM (SpectrumTier_draw, L"SpectrumTier: Draw", 0)
	REAL (L"left Frequency range (Hz)", L"0.0")
	REAL (L"right Frequency range (Hz)", L"10000.0")
	REAL (L"left Power range (dB)", L"20.0")
	REAL (L"right Power range (dB)", L"80.0")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
		OPTION (L"lines")
		OPTION (L"speckles")
		OPTION (L"lines and speckles")
	OK
DO_ALTERNATIVE (old_SpectrumTier_draw)
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (L"left Frequency range"), GET_REAL (L"right Frequency range"),
			GET_REAL (L"left Power range"), GET_REAL (L"right Power range"),
			GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
	}
END

FORM (SpectrumTier_list, L"SpectrumTier: List", 0)
	BOOLEAN (L"Include indexes", true)
	BOOLEAN (L"Include frequency", true)
	BOOLEAN (L"Include power density", true)
	OK
DO
	LOOP {
		iam (SpectrumTier);
		SpectrumTier_list (me, GET_INTEGER (L"Include indexes"), GET_INTEGER (L"Include frequency"), GET_INTEGER (L"Include power density"));
	}
END

FORM (SpectrumTier_removePointsBelow, L"SpectrumTier: Remove points below", 0)
	REAL (L"Remove all points below (dB)", L"40.0")
	OK
DO
	LOOP {
		iam (SpectrumTier);
		RealTier_removePointsBelow ((RealTier) me, GET_REAL (L"Remove all points below"));
		praat_dataChanged (me);
	}
END

/***** STRINGS *****/

FORM (Strings_createAsFileList, L"Create Strings as file list", L"Create Strings as file list...")
	SENTENCE (L"Name", L"fileList")
	LABEL (L"", L"Path:")
	TEXTFIELD (L"path", L"/people/Miep/*.wav")
	OK
static int inited;
if (! inited) {
	structMelderDir defaultDir = { { 0 } };
	wchar_t *workingDirectory, path [kMelder_MAXPATH+1];
	Melder_getDefaultDir (& defaultDir);
	workingDirectory = Melder_dirToPath (& defaultDir);
	#if defined (UNIX)
		swprintf (path, kMelder_MAXPATH+1, L"%ls/*.wav", workingDirectory);
	#elif defined (_WIN32)
	{
		int len = wcslen (workingDirectory);
		swprintf (path, kMelder_MAXPATH+1, L"%ls%ls*.wav", workingDirectory, len == 0 || workingDirectory [len - 1] != '\\' ? L"\\" : L"");
	}
	#else
		swprintf (path, kMelder_MAXPATH+1, L"%ls*.wav", workingDirectory);
	#endif
	SET_STRING (L"path", path);
	inited = TRUE;
}
DO
	autoStrings me = Strings_createAsFileList (GET_STRING (L"path"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Strings_createAsDirectoryList, L"Create Strings as directory list", L"Create Strings as directory list...")
	SENTENCE (L"Name", L"directoryList")
	LABEL (L"", L"Path:")
	TEXTFIELD (L"path", L"/people/Miep/*")
	OK
static int inited;
if (! inited) {
	structMelderDir defaultDir = { { 0 } };
	wchar_t *workingDirectory, path [kMelder_MAXPATH+1];
	Melder_getDefaultDir (& defaultDir);
	workingDirectory = Melder_dirToPath (& defaultDir);
	#if defined (UNIX)
		swprintf (path, kMelder_MAXPATH+1, L"%ls/*", workingDirectory);
	#elif defined (_WIN32)
	{
		int len = wcslen (workingDirectory);
		swprintf (path, kMelder_MAXPATH+1, L"%ls%ls*", workingDirectory, len == 0 || workingDirectory [len - 1] != '\\' ? L"\\" : L"");
	}
	#else
		swprintf (path, 300, L"%ls*", workingDirectory);
	#endif
	SET_STRING (L"path", path);
	inited = TRUE;
}
DO
	autoStrings me = Strings_createAsDirectoryList (GET_STRING (L"path"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

DIRECT (Strings_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Strings from batch.");
	LOOP {
		iam (Strings);
		autoStringsEditor editor = StringsEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

DIRECT (Strings_equal)
	Strings s1 = NULL, s2 = NULL;
	LOOP (s1 ? s2 : s1) = (Strings) OBJECT;
	bool equal = Data_equal (s1, s2);
	Melder_information (Melder_integer (equal));
END

DIRECT (Strings_genericize)
	LOOP {
		iam (Strings);
		try {
			Strings_genericize (me); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // BUG: in case of error, the Strings may have partially changed
			throw;
		}
	}
END

DIRECT (Strings_getNumberOfStrings)
	LOOP {
		iam (Strings);
		Melder_information (Melder_integer (my numberOfStrings));
	}
END

FORM (Strings_getString, L"Get string", 0)
	NATURAL (L"Index", L"1")
	OK
DO
	LOOP {
		iam (Strings);
		long index = GET_INTEGER (L"Index");
		Melder_information (index > my numberOfStrings ? L"" : my strings [index]);   // TODO
	}
END

DIRECT (Strings_help) Melder_help (L"Strings"); END

DIRECT (Strings_nativize)
	LOOP {
		iam (Strings);
		try {
			Strings_nativize (me); therror
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // BUG: in case of error, the Strings may have partially changed
			throw;
		}
	}
END

DIRECT (Strings_randomize)
	LOOP {
		iam (Strings);
		Strings_randomize (me);
		praat_dataChanged (me);
	}
END

FORM_READ (Strings_readFromRawTextFile, L"Read Strings from raw text file", 0, true)
	autoStrings me = Strings_readFromRawTextFile (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

DIRECT (Strings_sort)
	LOOP {
		iam (Strings);
		Strings_sort (me);
		praat_dataChanged (me);
	}
END

DIRECT (Strings_to_Distributions)
	LOOP {
		iam (Strings);
		autoDistributions thee = Strings_to_Distributions (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Strings_to_WordList)
	LOOP {
		iam (Strings);
		autoWordList thee = Strings_to_WordList (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM_WRITE (Strings_writeToRawTextFile, L"Save Strings as text file", 0, L"txt")
	LOOP {
		iam (Strings);
		Strings_writeToRawTextFile (me, file); therror
	}
END

/***** TABLE, rest in praat_Stat.c *****/

DIRECT (Table_to_Matrix)
	LOOP {
		iam (Table);
		autoMatrix thee = Table_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** TEXTGRID, rest in praat_TextGrid_init.c *****/

FORM (TextGrid_create, L"Create TextGrid", L"Create TextGrid...")
	LABEL (L"", L"Hint: to label or segment an existing Sound,")
	LABEL (L"", L"select that Sound and choose \"To TextGrid...\".")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	SENTENCE (L"All tier names", L"Mary John bell")
	SENTENCE (L"Which of these are point tiers?", L"bell")
	OK
DO
	double tmin = GET_REAL (L"Start time"), tmax = GET_REAL (L"End time");
	if (tmax <= tmin) Melder_throw ("End time should be greater than start time");
	autoTextGrid thee = TextGrid_create (tmin, tmax, GET_STRING (L"All tier names"), GET_STRING (L"Which of these are point tiers?"));
	praat_new (thee.transfer(), GET_STRING (L"All tier names"));
END

/***** TEXTTIER, rest in praat_TextGrid_init.c *****/

FORM_READ (TextTier_readFromXwaves, L"Read TextTier from Xwaves", 0, true)
	autoTextTier me = TextTier_readFromXwaves (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

/***** TIMEFRAMESAMPLED *****/

DIRECT (TimeFrameSampled_getNumberOfFrames)
	LOOP {
		iam (Sampled);
		long numberOfFrames = my nx;
		Melder_information (Melder_integer (numberOfFrames), L" frames");
	}
END

FORM (TimeFrameSampled_getFrameFromTime, L"Get frame number from time", L"Get frame number from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (Sampled);
		double frame = Sampled_xToIndex (me, GET_REAL (L"Time"));
		Melder_informationReal (frame, NULL);
	}
END

DIRECT (TimeFrameSampled_getFrameLength)
	LOOP {
		iam (Sampled);
		double frameLength = my dx;
		Melder_informationReal (frameLength, L"seconds");
	}
END

FORM (TimeFrameSampled_getTimeFromFrame, L"Get time from frame number", L"Get time from frame number...")
	NATURAL (L"Frame number", L"1")
	OK
DO
	LOOP {
		iam (Sampled);
		double time = Sampled_indexToX (me, GET_INTEGER (L"Frame number"));
		Melder_informationReal (time, L"seconds");
	}
END

/***** TIMEFUNCTION *****/

DIRECT (TimeFunction_getDuration)
	LOOP {
		iam (Function);
		double duration = my xmax - my xmin;
		Melder_informationReal (duration, L"seconds");
	}
END

DIRECT (TimeFunction_getEndTime)
	LOOP {
		iam (Function);
		double endTime = my xmax;
		Melder_informationReal (endTime, L"seconds");
	}
END

DIRECT (TimeFunction_getStartTime)
	LOOP {
		iam (Function);
		double startTime = my xmin;
		Melder_informationReal (startTime, L"seconds");
	}
END

FORM (TimeFunction_scaleTimesBy, L"Scale times by", 0)
	POSITIVE (L"Factor", L"2.0")
	OK
DO
	LOOP {
		iam (Function);
		Function_scaleXBy (me, GET_REAL (L"Factor"));
		praat_dataChanged (me);
	}
END

FORM (TimeFunction_scaleTimesTo, L"Scale times to", 0)
	REAL (L"New start time (s)", L"0.0")
	REAL (L"New end time (s)", L"1.0")
	OK
DO
	double tminto = GET_REAL (L"New start time"), tmaxto = GET_REAL (L"New end time");
	if (tminto >= tmaxto) Melder_throw ("New end time should be greater than new start time.");
	LOOP {
		iam (Function);
		Function_scaleXTo (me, tminto, tmaxto);
		praat_dataChanged (me);
	}
END

FORM (TimeFunction_shiftTimesBy, L"Shift times by", 0)
	REAL (L"Shift (s)", L"0.5")
	OK
DO
	LOOP {
		iam (Function);
		Function_shiftXBy (me, GET_REAL (L"Shift"));
		praat_dataChanged (me);
	}
END

FORM (TimeFunction_shiftTimesTo, L"Shift times to", 0)
	RADIO (L"Shift", 1)
		OPTION (L"start time")
		OPTION (L"centre time")
		OPTION (L"end time")
	REAL (L"To time (s)", L"0.0")
	OK
DO
	int shift = GET_INTEGER (L"Shift");
	LOOP {
		iam (Function);
		Function_shiftXTo (me, shift == 1 ? my xmin : shift == 2 ? 0.5 * (my xmin + my xmax) : my xmax, GET_REAL (L"To time"));
		praat_dataChanged (me);
	}
END

DIRECT (TimeFunction_shiftToZero)
	LOOP {
		iam (Function);
		Function_shiftXTo (me, my xmin, 0.0);
		praat_dataChanged (me);
	}
END

/***** TIMETIER *****/

FORM (TimeTier_getHighIndexFromTime, L"Get high index", L"AnyTier: Get high index from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points -> size == 0 ? L"--undefined--" : Melder_integer (AnyTier_timeToHighIndex (me, GET_REAL (L"Time"))));
	}
END

FORM (TimeTier_getLowIndexFromTime, L"Get low index", L"AnyTier: Get low index from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points -> size == 0 ? L"--undefined--" : Melder_integer (AnyTier_timeToLowIndex (me, GET_REAL (L"Time"))));
	}
END

FORM (TimeTier_getNearestIndexFromTime, L"Get nearest index", L"AnyTier: Get nearest index from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points -> size == 0 ? L"--undefined--" : Melder_integer (AnyTier_timeToNearestIndex (me, GET_REAL (L"Time"))));
	}
END

DIRECT (TimeTier_getNumberOfPoints)
	LOOP {
		iam (AnyTier);
		Melder_information (Melder_integer (my points -> size), L" points");
	}
END

FORM (TimeTier_getTimeFromIndex, L"Get time", 0 /*"AnyTier: Get time from index..."*/)
	NATURAL (L"Point number", L"10")
	OK
DO
	LOOP {
		iam (AnyTier);
		long i = GET_INTEGER (L"Point number");
		if (i > my points -> size) Melder_information (L"--undefined--");
		else Melder_informationReal (((AnyPoint) my points -> item [i]) -> number, L"seconds");
	}
END

FORM (TimeTier_removePoint, L"Remove one point", L"AnyTier: Remove point...")
	NATURAL (L"Point number", L"1")
	OK
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePoint (me, GET_INTEGER (L"Point number"));
		praat_dataChanged (me);
	}
END

FORM (TimeTier_removePointNear, L"Remove one point", L"AnyTier: Remove point near...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePointNear (me, GET_REAL (L"Time"));
		praat_dataChanged (me);
	}
END

FORM (TimeTier_removePointsBetween, L"Remove points", L"AnyTier: Remove points between...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	OK
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePointsBetween (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		praat_dataChanged (me);
	}
END

/***** TRANSITION *****/

DIRECT (Transition_conflate)
	LOOP {
		iam (Transition);
		autoDistributions thee = Transition_to_Distributions_conflate (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Transition_drawAsNumbers, L"Draw as numbers", 0)
	RADIO (L"Format", 1)
	RADIOBUTTON (L"decimal")
	RADIOBUTTON (L"exponential")
	RADIOBUTTON (L"free")
	RADIOBUTTON (L"rational")
	NATURAL (L"Precision", L"2")
	OK
DO
	LOOP {
		iam (Transition);
		autoPraatPicture picture;
		Transition_drawAsNumbers (me, GRAPHICS, GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"));
	}
END

DIRECT (Transition_eigen)
	LOOP {
		iam (Transition);
		Matrix vec_, val_;
		Transition_eigen (me, & vec_, & val_); therror
		autoMatrix vec = vec_;
		autoMatrix val = val_;
		praat_new (vec.transfer(), L"eigenvectors");
		praat_new (val.transfer(), L"eigenvalues");
	}
END

DIRECT (Transition_help) Melder_help (L"Transition"); END

FORM (Transition_power, L"Transition: Power...", 0)
	NATURAL (L"Power", L"2")
	OK
DO
	LOOP {
		iam (Transition);
		autoTransition thee = Transition_power (me, GET_INTEGER (L"Power"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Transition_to_Matrix)
	LOOP {
		iam (Transition);
		autoMatrix thee = Transition_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** Praat menu *****/

FORM (Praat_test, L"Praat test", 0)
	OPTIONMENU_ENUM (L"Test", kPraatTests, DEFAULT)
	SENTENCE (L"arg1", L"1000000")
	SENTENCE (L"arg2", L"")
	SENTENCE (L"arg3", L"")
	SENTENCE (L"arg4", L"")
	OK
DO
	Praat_tests (GET_INTEGER (L"Test"), GET_STRING (L"arg1"),
		GET_STRING (L"arg2"), GET_STRING (L"arg3"), GET_STRING (L"arg4"));
END

/***** Help menu *****/

DIRECT (ObjectWindow) Melder_help (L"Object window"); END
DIRECT (Intro) Melder_help (L"Intro"); END
DIRECT (WhatsNew) Melder_help (L"What's new?"); END
DIRECT (TypesOfObjects) Melder_help (L"Types of objects"); END
DIRECT (Editors) Melder_help (L"Editors"); END
DIRECT (FrequentlyAskedQuestions) Melder_help (L"FAQ (Frequently Asked Questions)"); END
DIRECT (Acknowledgments) Melder_help (L"Acknowledgments"); END
DIRECT (FormulasTutorial) Melder_help (L"Formulas"); END
DIRECT (ScriptingTutorial) Melder_help (L"Scripting"); END
DIRECT (DemoWindow) Melder_help (L"Demo window"); END
DIRECT (Programming) Melder_help (L"Programming with Praat"); END
DIRECT (SearchManual) Melder_search (); END

/***** file recognizers *****/

static Any cgnSyntaxFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 57) return NULL;
	if (! strnequ (& header [0], "<?xml version=\"1.0\"?>", 21) ||
	    (! strnequ (& header [22], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35) &&
	     ! strnequ (& header [23], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35))) return NULL;
	return TextGrid_readFromCgnSyntaxFile (file);
}

static Any chronologicalTextGridTextFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 100) return NULL;
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
	return NULL;
}

/***** buttons *****/

void praat_TableOfReal_init (ClassInfo klas);   // Buttons for TableOfReal and for its subclasses.

void praat_TimeFunction_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of Function.
void praat_TimeFunction_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Query time domain", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get start time", 0, 2, DO_TimeFunction_getStartTime);
						praat_addAction1 (klas, 1, L"Get starting time", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getStartTime);
	praat_addAction1 (klas, 1, L"Get end time", 0, 2, DO_TimeFunction_getEndTime);
						praat_addAction1 (klas, 1, L"Get finishing time", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getEndTime);
	praat_addAction1 (klas, 1, L"Get total duration", 0, 2, DO_TimeFunction_getDuration);
						praat_addAction1 (klas, 1, L"Get duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getDuration);
}

void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time-based subclasses of Function.
void praat_TimeFunction_modify_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, L"Modify times", 0, 1, 0);
	praat_addAction1 (klas, 0, L"Shift times by...", 0, 2, DO_TimeFunction_shiftTimesBy);
	praat_addAction1 (klas, 0, L"Shift times to...", 0, 2, DO_TimeFunction_shiftTimesTo);
						praat_addAction1 (klas, 0, L"Shift to zero", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_shiftToZero);   // hidden 2008
	praat_addAction1 (klas, 0, L"Scale times by...", 0, 2, DO_TimeFunction_scaleTimesBy);
	praat_addAction1 (klas, 0, L"Scale times to...", 0, 2, DO_TimeFunction_scaleTimesTo);
						praat_addAction1 (klas, 0, L"Scale times...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_scaleTimesTo);   // hidden 2008
}

void praat_TimeFrameSampled_query_init (ClassInfo klas);   // Query buttons for frame-based time-based subclasses of Sampled.
void praat_TimeFrameSampled_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, L"Query time sampling", 0, 1, 0);
	praat_addAction1 (klas, 1, L"Get number of frames", 0, 2, DO_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, L"Get time step", 0, 2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, L"Get frame length", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, L"Get frame duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, L"Get time from frame number...", 0, 2, DO_TimeFrameSampled_getTimeFromFrame);
						praat_addAction1 (klas, 1, L"Get time from frame...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, L"Get frame number from time...", 0, 2, DO_TimeFrameSampled_getFrameFromTime);
						praat_addAction1 (klas, 1, L"Get frame from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameFromTime);
}

void praat_TimeTier_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of AnyTier.
void praat_TimeTier_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, L"Get number of points", 0, 1, DO_TimeTier_getNumberOfPoints);
	praat_addAction1 (klas, 1, L"Get low index from time...", 0, 1, DO_TimeTier_getLowIndexFromTime);
	praat_addAction1 (klas, 1, L"Get high index from time...", 0, 1, DO_TimeTier_getHighIndexFromTime);
	praat_addAction1 (klas, 1, L"Get nearest index from time...", 0, 1, DO_TimeTier_getNearestIndexFromTime);
	praat_addAction1 (klas, 1, L"Get time from index...", 0, 1, DO_TimeTier_getTimeFromIndex);
}

void praat_TimeTier_modify_init (ClassInfo klas);   // Modification buttons for time-based subclasses of AnyTier.
void praat_TimeTier_modify_init (ClassInfo klas) {
	praat_TimeFunction_modify_init (klas);
	praat_addAction1 (klas, 0, L"Remove point...", 0, 1, DO_TimeTier_removePoint);
	praat_addAction1 (klas, 0, L"Remove point near...", 0, 1, DO_TimeTier_removePointNear);
	praat_addAction1 (klas, 0, L"Remove points between...", 0, 1, DO_TimeTier_removePointsBetween);
}

void praat_uvafon_init ();
void praat_uvafon_init () {
	Thing_recognizeClassesByName (classSound, classMatrix, classPolygon, classPointProcess, classParamCurve,
		classSpectrum, classLtas, classSpectrogram, classFormant,
		classExcitation, classCochleagram, classVocalTract, classFormantPoint, classFormantTier, classFormantGrid,
		classLabel, classTier, classAutosegment,   /* Three obsolete classes. */
		classIntensity, classPitch, classHarmonicity,
		classTransition,
		classRealPoint, classRealTier, classPitchTier, classIntensityTier, classDurationTier, classAmplitudeTier, classSpectrumTier,
		classManipulation, classTextPoint, classTextInterval, classTextTier,
		classIntervalTier, classTextGrid, classLongSound, classWordList, classSpellingChecker,
		classMovie, classCorpus,
		NULL);
	Thing_recognizeClassByOtherName (classManipulation, L"Psola");
	Thing_recognizeClassByOtherName (classManipulation, L"Analysis");
	Thing_recognizeClassByOtherName (classPitchTier, L"StylPitch");

	Data_recognizeFileType (cgnSyntaxFileRecognizer);
	Data_recognizeFileType (chronologicalTextGridTextFileRecognizer);

	ManipulationEditor_prefs ();
	SpectrumEditor_prefs ();
	FormantGridEditor_prefs ();

	INCLUDE_LIBRARY (praat_uvafon_Sound_init)
	INCLUDE_LIBRARY (praat_uvafon_TextGrid_init)

	praat_addMenuCommand (L"Objects", L"Praat", L"Praat test...", 0, praat_HIDDEN, DO_Praat_test);

	praat_addMenuCommand (L"Objects", L"New", L"-- new numerics --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Matrix", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create Matrix...", 0, 1, DO_Matrix_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create simple Matrix...", 0, 1, DO_Matrix_createSimple);
	praat_addMenuCommand (L"Objects", L"Open", L"-- read movie --", 0, praat_HIDDEN, 0);
	praat_addMenuCommand (L"Objects", L"Open", L"Open movie file...", 0, praat_HIDDEN, DO_Movie_openFromSoundFile);
	praat_addMenuCommand (L"Objects", L"Open", L"-- read raw --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Matrix from raw text file...", 0, 0, DO_Matrix_readFromRawTextFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Matrix from LVS AP file...", 0, praat_HIDDEN, DO_Matrix_readAP);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Strings from raw text file...", 0, 0, DO_Strings_readFromRawTextFile);

	INCLUDE_LIBRARY (praat_uvafon_stat_init)

	praat_addMenuCommand (L"Objects", L"New", L"Tiers", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create empty PointProcess...", 0, 1, DO_PointProcess_createEmpty);
		praat_addMenuCommand (L"Objects", L"New", L"Create Poisson process...", 0, 1, DO_PointProcess_createPoissonProcess);
		praat_addMenuCommand (L"Objects", L"New", L"-- new tiers ---", 0, 1, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create PitchTier...", 0, 1, DO_PitchTier_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create FormantGrid...", 0, 1, DO_FormantGrid_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create FormantTier...", 0, praat_HIDDEN | praat_DEPTH_1, DO_FormantTier_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create IntensityTier...", 0, 1, DO_IntensityTier_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create DurationTier...", 0, 1, DO_DurationTier_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create AmplitudeTier...", 0, 1, DO_AmplitudeTier_create);
	praat_addMenuCommand (L"Objects", L"New", L"-- new textgrid --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create TextGrid...", 0, 0, DO_TextGrid_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Corpus...", 0, praat_HIDDEN, DO_Corpus_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Strings as file list...", 0, 0, DO_Strings_createAsFileList);
	praat_addMenuCommand (L"Objects", L"New", L"Create Strings as directory list...", 0, 0, DO_Strings_createAsDirectoryList);

	praat_addMenuCommand (L"Objects", L"Open", L"-- read tier --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Open", L"Read from special tier file...", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Open", L"Read TextTier from Xwaves...", 0, 1, DO_TextTier_readFromXwaves);
		praat_addMenuCommand (L"Objects", L"Open", L"Read IntervalTier from Xwaves...", 0, 1, DO_IntervalTier_readFromXwaves);

	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Praat Intro", 0, '?', DO_Intro);
	#ifndef macintosh
		praat_addMenuCommand (L"Objects", L"Help", L"Object window", 0, 0, DO_ObjectWindow);
	#endif
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Frequently asked questions", 0, 0, DO_FrequentlyAskedQuestions);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"What's new?", 0, 0, DO_WhatsNew);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Types of objects", 0, 0, DO_TypesOfObjects);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Editors", 0, 0, DO_Editors);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Acknowledgments", 0, 0, DO_Acknowledgments);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"-- shell help --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Formulas tutorial", 0, 0, DO_FormulasTutorial);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Scripting tutorial", 0, 0, DO_ScriptingTutorial);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Demo window", 0, 0, DO_DemoWindow);
	praat_addMenuCommand (L"Objects", L"ApplicationHelp", L"Programming", 0, 0, DO_Programming);
	#ifdef macintosh
		praat_addMenuCommand (L"Objects", L"Help", L"Praat Intro", 0, '?', DO_Intro);
		praat_addMenuCommand (L"Objects", L"Help", L"Object window help", 0, 0, DO_ObjectWindow);
		praat_addMenuCommand (L"Objects", L"Help", L"-- manual --", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Help", L"Search Praat manual...", 0, 'M', DO_SearchManual);
	#endif

	praat_addAction1 (classAmplitudeTier, 0, L"AmplitudeTier help", 0, 0, DO_AmplitudeTier_help);
	praat_addAction1 (classAmplitudeTier, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_AmplitudeTier_edit);
	praat_addAction1 (classAmplitudeTier, 1, L"Edit", 0, praat_HIDDEN, DO_AmplitudeTier_edit);
	praat_addAction1 (classAmplitudeTier, 0, L"View & Edit with Sound?", 0, 0, DO_info_AmplitudeTier_Sound_edit);
	praat_addAction1 (classAmplitudeTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (local)...", 0, 1, DO_AmplitudeTier_getShimmer_local);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (local_dB)...", 0, 1, DO_AmplitudeTier_getShimmer_local_dB);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (apq3)...", 0, 1, DO_AmplitudeTier_getShimmer_apq3);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (apq5)...", 0, 1, DO_AmplitudeTier_getShimmer_apq5);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (apq11)...", 0, 1, DO_AmplitudeTier_getShimmer_apq11);
		praat_addAction1 (classAmplitudeTier, 1, L"Get shimmer (dda)...", 0, 1, DO_AmplitudeTier_getShimmer_dda);
	praat_addAction1 (classAmplitudeTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 0, L"Add point...", 0, 1, DO_AmplitudeTier_addPoint);
		praat_addAction1 (classAmplitudeTier, 0, L"Formula...", 0, 1, DO_AmplitudeTier_formula);
praat_addAction1 (classAmplitudeTier, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classAmplitudeTier, 0, L"To Sound (pulse train)...", 0, 0, DO_AmplitudeTier_to_Sound);
praat_addAction1 (classAmplitudeTier, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classAmplitudeTier, 0, L"To IntensityTier...", 0, 0, DO_AmplitudeTier_to_IntensityTier);
	praat_addAction1 (classAmplitudeTier, 0, L"Down to PointProcess", 0, 0, DO_AmplitudeTier_downto_PointProcess);
	praat_addAction1 (classAmplitudeTier, 0, L"Down to TableOfReal", 0, 0, DO_AmplitudeTier_downto_TableOfReal);

	praat_addAction1 (classCochleagram, 0, L"Cochleagram help", 0, 0, DO_Cochleagram_help);
	praat_addAction1 (classCochleagram, 1, L"Movie", 0, 0, DO_Cochleagram_movie);
praat_addAction1 (classCochleagram, 0, L"Info", 0, 0, 0);
	praat_addAction1 (classCochleagram, 2, L"Difference...", 0, 0, DO_Cochleagram_difference);
praat_addAction1 (classCochleagram, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, L"Paint...", 0, 0, DO_Cochleagram_paint);
praat_addAction1 (classCochleagram, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, L"Formula...", 0, 0, DO_Cochleagram_formula);
praat_addAction1 (classCochleagram, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, L"To Excitation (slice)...", 0, 0, DO_Cochleagram_to_Excitation);
praat_addAction1 (classCochleagram, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classCochleagram, 0, L"To Matrix", 0, 0, DO_Cochleagram_to_Matrix);

	praat_addAction1 (classCorpus, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Corpus_edit);

praat_addAction1 (classDistributions, 0, L"Learn", 0, 0, 0);
	praat_addAction1 (classDistributions, 1, L"To Transition...", 0, 0, DO_Distributions_to_Transition);
	praat_addAction1 (classDistributions, 2, L"To Transition (noise)...", 0, 0, DO_Distributions_to_Transition_noise);

	praat_addAction1 (classDurationTier, 0, L"DurationTier help", 0, 0, DO_DurationTier_help);
	praat_addAction1 (classDurationTier, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_DurationTier_edit);
	praat_addAction1 (classDurationTier, 1, L"Edit", 0, praat_HIDDEN, DO_DurationTier_edit);
	praat_addAction1 (classDurationTier, 0, L"View & Edit with Sound?", 0, 0, DO_info_DurationTier_Sound_edit);
	praat_addAction1 (classDurationTier, 0, L"& Manipulation: Replace?", 0, 0, DO_info_DurationTier_Manipulation_replace);
	praat_addAction1 (classDurationTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classDurationTier);
		praat_addAction1 (classDurationTier, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classDurationTier, 1, L"Get target duration...", 0, 1, DO_DurationTier_getTargetDuration);
	praat_addAction1 (classDurationTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classDurationTier);
		praat_addAction1 (classDurationTier, 0, L"Add point...", 0, 1, DO_DurationTier_addPoint);
		praat_addAction1 (classDurationTier, 0, L"Formula...", 0, 1, DO_DurationTier_formula);
praat_addAction1 (classDurationTier, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classDurationTier, 0, L"Down to PointProcess", 0, 0, DO_DurationTier_downto_PointProcess);

	praat_addAction1 (classExcitation, 0, L"Excitation help", 0, 0, DO_Excitation_help);
praat_addAction1 (classExcitation, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, L"Draw...", 0, 0, DO_Excitation_draw);
praat_addAction1 (classExcitation, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, L"To Formant...", 0, 0, DO_Excitation_to_Formant);
praat_addAction1 (classExcitation, 1, L"Query -", 0, 0, 0);
	praat_addAction1 (classExcitation, 1, L"Get loudness", 0, 0, DO_Excitation_getLoudness);
praat_addAction1 (classExcitation, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, L"Formula...", 0, 0, DO_Excitation_formula);
praat_addAction1 (classExcitation, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classExcitation, 0, L"To Matrix", 0, 0, DO_Excitation_to_Matrix);

	praat_addAction1 (classFormant, 0, L"Formant help", 0, 0, DO_Formant_help);
	praat_addAction1 (classFormant, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classFormant, 0, L"Speckle...", 0, 1, DO_Formant_drawSpeckles);
		praat_addAction1 (classFormant, 0, L"Draw tracks...", 0, 1, DO_Formant_drawTracks);
		praat_addAction1 (classFormant, 0, L"Scatter plot...", 0, 1, DO_Formant_scatterPlot);
	praat_addAction1 (classFormant, 1, L"List...", 0, 0, DO_Formant_list);
	praat_addAction1 (classFormant, 0, L"Down to Table...", 0, 0, DO_Formant_downto_Table);
	praat_addAction1 (classFormant, 0, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classFormant);
		praat_addAction1 (classFormant, 1, L"Get number of formants...", 0, 1, DO_Formant_getNumberOfFormants);
		praat_addAction1 (classFormant, 1, L"Get minimum number of formants", 0, 1, DO_Formant_getMinimumNumberOfFormants);
		praat_addAction1 (classFormant, 1, L"Get maximum number of formants", 0, 1, DO_Formant_getMaximumNumberOfFormants);
		praat_addAction1 (classFormant, 1, L"-- get value --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, L"Get value at time...", 0, 1, DO_Formant_getValueAtTime);
		praat_addAction1 (classFormant, 1, L"Get bandwidth at time...", 0, 1, DO_Formant_getBandwidthAtTime);
		praat_addAction1 (classFormant, 1, L"-- get extreme --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, L"Get minimum...", 0, 1, DO_Formant_getMinimum);
		praat_addAction1 (classFormant, 1, L"Get time of minimum...", 0, 1, DO_Formant_getTimeOfMinimum);
		praat_addAction1 (classFormant, 1, L"Get maximum...", 0, 1, DO_Formant_getMaximum);
		praat_addAction1 (classFormant, 1, L"Get time of maximum...", 0, 1, DO_Formant_getTimeOfMaximum);
		praat_addAction1 (classFormant, 1, L"-- get distribution --", 0, 1, 0);
		praat_addAction1 (classFormant, 1, L"Get quantile...", 0, 1, DO_Formant_getQuantile);
		praat_addAction1 (classFormant, 1, L"Get quantile of bandwidth...", 0, 1, DO_Formant_getQuantileOfBandwidth);
		praat_addAction1 (classFormant, 1, L"Get mean...", 0, 1, DO_Formant_getMean);
		praat_addAction1 (classFormant, 1, L"Get standard deviation...", 0, 1, DO_Formant_getStandardDeviation);
	praat_addAction1 (classFormant, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classFormant);
		praat_addAction1 (classFormant, 0, L"Sort", 0, 1, DO_Formant_sort);
		praat_addAction1 (classFormant, 0, L"Formula (frequencies)...", 0, 1, DO_Formant_formula_frequencies);
		praat_addAction1 (classFormant, 0, L"Formula (bandwidths)...", 0, 1, DO_Formant_formula_bandwidths);
praat_addAction1 (classFormant, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classFormant, 0, L"Track...", 0, 0, DO_Formant_tracker);
	praat_addAction1 (classFormant, 0, L"Down to FormantTier", 0, praat_HIDDEN, DO_Formant_downto_FormantTier);
	praat_addAction1 (classFormant, 0, L"Down to FormantGrid", 0, 0, DO_Formant_downto_FormantGrid);
praat_addAction1 (classFormant, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classFormant, 0, L"To Matrix...", 0, 0, DO_Formant_to_Matrix);

	praat_addAction1 (classFormantGrid, 0, L"FormantGrid help", 0, 0, DO_FormantGrid_help);
	praat_addAction1 (classFormantGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 1, L"Edit", 0, praat_HIDDEN, DO_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classFormantGrid);
		praat_addAction1 (classFormantGrid, 0, L"Formula (frequencies)...", 0, 1, DO_FormantGrid_formula_frequencies);
		//praat_addAction1 (classFormantGrid, 0, L"Formula (bandwidths)...", 0, 1, DO_FormantGrid_formula_bandwidths);
		praat_addAction1 (classFormantGrid, 0, L"Add formant point...", 0, 1, DO_FormantGrid_addFormantPoint);
		praat_addAction1 (classFormantGrid, 0, L"Add bandwidth point...", 0, 1, DO_FormantGrid_addBandwidthPoint);
		praat_addAction1 (classFormantGrid, 0, L"Remove formant points between...", 0, 1, DO_FormantGrid_removeFormantPointsBetween);
		praat_addAction1 (classFormantGrid, 0, L"Remove bandwidth points between...", 0, 1, DO_FormantGrid_removeBandwidthPointsBetween);
	praat_addAction1 (classFormantGrid, 0, L"Convert -", 0, 0, 0);
		praat_addAction1 (classFormantGrid, 0, L"To Formant...", 0, 1, DO_FormantGrid_to_Formant);

	praat_addAction1 (classFormantTier, 0, L"FormantTier help", 0, 0, DO_FormantTier_help);
	praat_addAction1 (classFormantTier, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classFormantTier, 0, L"Speckle...", 0, 1, DO_FormantTier_speckle);
	praat_addAction1 (classFormantTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classFormantTier);
		praat_addAction1 (classFormantTier, 1, L"-- get value --", 0, 1, 0);
		praat_addAction1 (classFormantTier, 1, L"Get value at time...", 0, 1, DO_FormantTier_getValueAtTime);
		praat_addAction1 (classFormantTier, 1, L"Get bandwidth at time...", 0, 1, DO_FormantTier_getBandwidthAtTime);
	praat_addAction1 (classFormantTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classFormantTier);
		praat_addAction1 (classFormantTier, 0, L"Add point...", 0, 1, DO_FormantTier_addPoint);
praat_addAction1 (classFormantTier, 0, L"Down", 0, 0, 0);
	praat_addAction1 (classFormantTier, 0, L"Down to TableOfReal...", 0, 0, DO_FormantTier_downto_TableOfReal);

	praat_addAction1 (classHarmonicity, 0, L"Harmonicity help", 0, 0, DO_Harmonicity_help);
	praat_addAction1 (classHarmonicity, 0, L"Draw", 0, 0, 0);
		praat_addAction1 (classHarmonicity, 0, L"Draw...", 0, 0, DO_Harmonicity_draw);
	praat_addAction1 (classHarmonicity, 1, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, L"Get value at time...", 0, 1, DO_Harmonicity_getValueAtTime);
		praat_addAction1 (classHarmonicity, 1, L"Get value in frame...", 0, 1, DO_Harmonicity_getValueInFrame);
		praat_addAction1 (classHarmonicity, 1, L"-- get extreme --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, L"Get minimum...", 0, 1, DO_Harmonicity_getMinimum);
		praat_addAction1 (classHarmonicity, 1, L"Get time of minimum...", 0, 1, DO_Harmonicity_getTimeOfMinimum);
		praat_addAction1 (classHarmonicity, 1, L"Get maximum...", 0, 1, DO_Harmonicity_getMaximum);
		praat_addAction1 (classHarmonicity, 1, L"Get time of maximum...", 0, 1, DO_Harmonicity_getTimeOfMaximum);
		praat_addAction1 (classHarmonicity, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classHarmonicity, 1, L"Get mean...", 0, 1, DO_Harmonicity_getMean);
		praat_addAction1 (classHarmonicity, 1, L"Get standard deviation...", 0, 1, DO_Harmonicity_getStandardDeviation);
	praat_addAction1 (classHarmonicity, 0, L"Modify", 0, 0, 0);
		praat_TimeFunction_modify_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 0, L"Formula...", 0, 0, DO_Harmonicity_formula);
	praat_addAction1 (classHarmonicity, 0, L"Hack", 0, 0, 0);
		praat_addAction1 (classHarmonicity, 0, L"To Matrix", 0, 0, DO_Harmonicity_to_Matrix);

	praat_addAction1 (classIntensity, 0, L"Intensity help", 0, 0, DO_Intensity_help);
	praat_addAction1 (classIntensity, 0, L"Draw...", 0, 0, DO_Intensity_draw);
	praat_addAction1 (classIntensity, 1, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classIntensity);
		praat_addAction1 (classIntensity, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, L"Get value at time...", 0, 1, DO_Intensity_getValueAtTime);
		praat_addAction1 (classIntensity, 1, L"Get value in frame...", 0, 1, DO_Intensity_getValueInFrame);
		praat_addAction1 (classIntensity, 1, L"-- get extreme --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, L"Get minimum...", 0, 1, DO_Intensity_getMinimum);
		praat_addAction1 (classIntensity, 1, L"Get time of minimum...", 0, 1, DO_Intensity_getTimeOfMinimum);
		praat_addAction1 (classIntensity, 1, L"Get maximum...", 0, 1, DO_Intensity_getMaximum);
		praat_addAction1 (classIntensity, 1, L"Get time of maximum...", 0, 1, DO_Intensity_getTimeOfMaximum);
		praat_addAction1 (classIntensity, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classIntensity, 1, L"Get quantile...", 0, 1, DO_Intensity_getQuantile);
		praat_addAction1 (classIntensity, 1, L"Get mean...", 0, 1, DO_Intensity_getMean);
		praat_addAction1 (classIntensity, 1, L"Get standard deviation...", 0, 1, DO_Intensity_getStandardDeviation);
	praat_addAction1 (classIntensity, 0, L"Modify", 0, 0, 0);
		praat_TimeFunction_modify_init (classIntensity);
		praat_addAction1 (classIntensity, 0, L"Formula...", 0, 0, DO_Intensity_formula);
	praat_addAction1 (classIntensity, 0, L"Analyse", 0, 0, 0);
		praat_addAction1 (classIntensity, 0, L"To IntensityTier (peaks)", 0, 0, DO_Intensity_to_IntensityTier_peaks);
		praat_addAction1 (classIntensity, 0, L"To IntensityTier (valleys)", 0, 0, DO_Intensity_to_IntensityTier_valleys);
	praat_addAction1 (classIntensity, 0, L"Convert", 0, 0, 0);
		praat_addAction1 (classIntensity, 0, L"Down to IntensityTier", 0, 0, DO_Intensity_downto_IntensityTier);
		praat_addAction1 (classIntensity, 0, L"Down to Matrix", 0, 0, DO_Intensity_downto_Matrix);

	praat_addAction1 (classIntensityTier, 0, L"IntensityTier help", 0, 0, DO_IntensityTier_help);
	praat_addAction1 (classIntensityTier, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_IntensityTier_edit);
	praat_addAction1 (classIntensityTier, 1, L"Edit", 0, praat_HIDDEN, DO_IntensityTier_edit);
	praat_addAction1 (classIntensityTier, 0, L"View & Edit with Sound?", 0, 0, DO_info_IntensityTier_Sound_edit);
	praat_addAction1 (classIntensityTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classIntensityTier, 1, L"Get value at time...", 0, 1, DO_IntensityTier_getValueAtTime);
		praat_addAction1 (classIntensityTier, 1, L"Get value at index...", 0, 1, DO_IntensityTier_getValueAtIndex);
	praat_addAction1 (classIntensityTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 0, L"Add point...", 0, 1, DO_IntensityTier_addPoint);
		praat_addAction1 (classIntensityTier, 0, L"Formula...", 0, 1, DO_IntensityTier_formula);
praat_addAction1 (classIntensityTier, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classIntensityTier, 0, L"To AmplitudeTier", 0, 0, DO_IntensityTier_to_AmplitudeTier);
	praat_addAction1 (classIntensityTier, 0, L"Down to PointProcess", 0, 0, DO_IntensityTier_downto_PointProcess);
	praat_addAction1 (classIntensityTier, 0, L"Down to TableOfReal", 0, 0, DO_IntensityTier_downto_TableOfReal);

	praat_addAction1 (classLtas, 0, L"Ltas help", 0, 0, DO_Ltas_help);
	praat_addAction1 (classLtas, 0, L"Draw...", 0, 0, DO_Ltas_draw);
	praat_addAction1 (classLtas, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classLtas, 1, L"Frequency domain", 0, 1, 0);
		praat_addAction1 (classLtas, 1, L"Get lowest frequency", 0, 2, DO_Ltas_getLowestFrequency);
		praat_addAction1 (classLtas, 1, L"Get highest frequency", 0, 2, DO_Ltas_getHighestFrequency);
		praat_addAction1 (classLtas, 1, L"Frequency sampling", 0, 1, 0);
		praat_addAction1 (classLtas, 1, L"Get number of bins", 0, 2, DO_Ltas_getNumberOfBins);
			praat_addAction1 (classLtas, 1, L"Get number of bands", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getNumberOfBins);
		praat_addAction1 (classLtas, 1, L"Get bin width", 0, 2, DO_Ltas_getBinWidth);
			praat_addAction1 (classLtas, 1, L"Get band width", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinWidth);
		praat_addAction1 (classLtas, 1, L"Get frequency from bin number...", 0, 2, DO_Ltas_getFrequencyFromBinNumber);
			praat_addAction1 (classLtas, 1, L"Get frequency from band...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getFrequencyFromBinNumber);
		praat_addAction1 (classLtas, 1, L"Get bin number from frequency...", 0, 2, DO_Ltas_getBinNumberFromFrequency);
			praat_addAction1 (classLtas, 1, L"Get band from frequency...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinNumberFromFrequency);
		praat_addAction1 (classLtas, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, L"Get value at frequency...", 0, 1, DO_Ltas_getValueAtFrequency);
		praat_addAction1 (classLtas, 1, L"Get value in bin...", 0, 1, DO_Ltas_getValueInBin);
			praat_addAction1 (classLtas, 1, L"Get value in band...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Ltas_getValueInBin);
		praat_addAction1 (classLtas, 1, L"-- get extreme --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, L"Get minimum...", 0, 1, DO_Ltas_getMinimum);
		praat_addAction1 (classLtas, 1, L"Get frequency of minimum...", 0, 1, DO_Ltas_getFrequencyOfMinimum);
		praat_addAction1 (classLtas, 1, L"Get maximum...", 0, 1, DO_Ltas_getMaximum);
		praat_addAction1 (classLtas, 1, L"Get frequency of maximum...", 0, 1, DO_Ltas_getFrequencyOfMaximum);
		praat_addAction1 (classLtas, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classLtas, 1, L"Get mean...", 0, 1, DO_Ltas_getMean);
		praat_addAction1 (classLtas, 1, L"Get slope...", 0, 1, DO_Ltas_getSlope);
		praat_addAction1 (classLtas, 1, L"Get local peak height...", 0, 1, DO_Ltas_getLocalPeakHeight);
		praat_addAction1 (classLtas, 1, L"Get standard deviation...", 0, 1, DO_Ltas_getStandardDeviation);
	praat_addAction1 (classLtas, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classLtas, 0, L"Formula...", 0, 0, DO_Ltas_formula);
	praat_addAction1 (classLtas, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classLtas, 0, L"To SpectrumTier (peaks)", 0, 0, DO_Ltas_to_SpectrumTier_peaks);
	praat_addAction1 (classLtas, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classLtas, 0, L"Compute trend line...", 0, 0, DO_Ltas_computeTrendLine);
	praat_addAction1 (classLtas, 0, L"Subtract trend line...", 0, 0, DO_Ltas_subtractTrendLine);
	praat_addAction1 (classLtas, 0, L"Combine", 0, 0, 0);
	praat_addAction1 (classLtas, 0, L"Merge", 0, praat_HIDDEN, DO_Ltases_merge);
	praat_addAction1 (classLtas, 0, L"Average", 0, 0, DO_Ltases_average);
	praat_addAction1 (classLtas, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classLtas, 0, L"To Matrix", 0, 0, DO_Ltas_to_Matrix);

	praat_addAction1 (classManipulation, 0, L"Manipulation help", 0, 0, DO_Manipulation_help);
	praat_addAction1 (classManipulation, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Manipulation_edit);
	praat_addAction1 (classManipulation, 1, L"Edit", 0, praat_HIDDEN, DO_Manipulation_edit);
	praat_addAction1 (classManipulation, 0, L"Play (overlap-add)", 0, 0, DO_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0, L"Play (PSOLA)", 0, praat_HIDDEN, DO_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0, L"Play (LPC)", 0, 0, DO_Manipulation_play_lpc);
	praat_addAction1 (classManipulation, 0, L"Get resynthesis (overlap-add)", 0, 0, DO_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, L"Get resynthesis (PSOLA)", 0, praat_HIDDEN, DO_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, L"Get resynthesis (LPC)", 0, 0, DO_Manipulation_getResynthesis_lpc);
	praat_addAction1 (classManipulation, 0, L"Extract original sound", 0, 0, DO_Manipulation_extractOriginalSound);
	praat_addAction1 (classManipulation, 0, L"Extract pulses", 0, 0, DO_Manipulation_extractPulses);
	praat_addAction1 (classManipulation, 0, L"Extract pitch tier", 0, 0, DO_Manipulation_extractPitchTier);
	praat_addAction1 (classManipulation, 0, L"Extract duration tier", 0, 0, DO_Manipulation_extractDurationTier);
	praat_addAction1 (classManipulation, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classManipulation);
		praat_addAction1 (classManipulation, 0, L"Replace pitch tier?", 0, 1, DO_Manipulation_replacePitchTier_help);
		praat_addAction1 (classManipulation, 0, L"Replace duration tier?", 0, 1, DO_Manipulation_replaceDurationTier_help);
	praat_addAction1 (classManipulation, 1, L"Save as text file without Sound...", 0, 0, DO_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, L"Write to text file without Sound...", 0, praat_HIDDEN, DO_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, L"Save as binary file without Sound...", 0, 0, DO_Manipulation_writeToBinaryFileWithoutSound);
	praat_addAction1 (classManipulation, 1, L"Write to binary file without Sound...", 0, praat_HIDDEN, DO_Manipulation_writeToBinaryFileWithoutSound);

	praat_addAction1 (classMatrix, 0, L"Matrix help", 0, 0, DO_Matrix_help);
	praat_addAction1 (classMatrix, 1, L"Save as matrix text file...", 0, 0, DO_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, L"Write to matrix text file...", 0, praat_HIDDEN, DO_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, L"Save as headerless spreadsheet file...", 0, 0, DO_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, L"Write to headerless spreadsheet file...", 0, praat_HIDDEN, DO_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, L"Play movie", 0, 0, DO_Matrix_movie);
	praat_addAction1 (classMatrix, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, L"Draw rows...", 0, 1, DO_Matrix_drawRows);
		praat_addAction1 (classMatrix, 0, L"Draw one contour...", 0, 1, DO_Matrix_drawOneContour);
		praat_addAction1 (classMatrix, 0, L"Draw contours...", 0, 1, DO_Matrix_drawContours);
		praat_addAction1 (classMatrix, 0, L"Paint image...", 0, 1, DO_Matrix_paintImage);
		praat_addAction1 (classMatrix, 0, L"Paint contours...", 0, 1, DO_Matrix_paintContours);
		praat_addAction1 (classMatrix, 0, L"Paint cells...", 0, 1, DO_Matrix_paintCells);
		praat_addAction1 (classMatrix, 0, L"Paint surface...", 0, 1, DO_Matrix_paintSurface);
	praat_addAction1 (classMatrix, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classMatrix, 1, L"Get lowest x", 0, 1, DO_Matrix_getLowestX);
		praat_addAction1 (classMatrix, 1, L"Get highest x", 0, 1, DO_Matrix_getHighestX);
		praat_addAction1 (classMatrix, 1, L"Get lowest y", 0, 1, DO_Matrix_getLowestY);
		praat_addAction1 (classMatrix, 1, L"Get highest y", 0, 1, DO_Matrix_getHighestY);
		praat_addAction1 (classMatrix, 1, L"-- get structure --", 0, 1, 0);
		praat_addAction1 (classMatrix, 1, L"Get number of rows", 0, 1, DO_Matrix_getNumberOfRows);
		praat_addAction1 (classMatrix, 1, L"Get number of columns", 0, 1, DO_Matrix_getNumberOfColumns);
		praat_addAction1 (classMatrix, 1, L"Get row distance", 0, 1, DO_Matrix_getRowDistance);
		praat_addAction1 (classMatrix, 1, L"Get column distance", 0, 1, DO_Matrix_getColumnDistance);
		praat_addAction1 (classMatrix, 1, L"Get y of row...", 0, 1, DO_Matrix_getYofRow);
		praat_addAction1 (classMatrix, 1, L"Get x of column...", 0, 1, DO_Matrix_getXofColumn);
		praat_addAction1 (classMatrix, 1, L"-- get value --", 0, 1, 0);
		praat_addAction1 (classMatrix, 1, L"Get value in cell...", 0, 1, DO_Matrix_getValueInCell);
		praat_addAction1 (classMatrix, 1, L"Get value at xy...", 0, 1, DO_Matrix_getValueAtXY);
		praat_addAction1 (classMatrix, 1, L"Get minimum", 0, 1, DO_Matrix_getMinimum);
		praat_addAction1 (classMatrix, 1, L"Get maximum", 0, 1, DO_Matrix_getMaximum);
		praat_addAction1 (classMatrix, 1, L"Get sum", 0, 1, DO_Matrix_getSum);
	praat_addAction1 (classMatrix, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, L"Formula...", 0, 1, DO_Matrix_formula);
		praat_addAction1 (classMatrix, 0, L"Set value...", 0, 1, DO_Matrix_setValue);
praat_addAction1 (classMatrix, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classMatrix, 0, L"Eigen", 0, 0, DO_Matrix_eigen);
	praat_addAction1 (classMatrix, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classMatrix, 0, L"Power...", 0, 0, DO_Matrix_power);
	praat_addAction1 (classMatrix, 0, L"Combine two Matrices -", 0, 0, 0);
		praat_addAction1 (classMatrix, 2, L"Merge (append rows)", 0, 1, DO_Matrix_appendRows);
		praat_addAction1 (classMatrix, 2, L"To ParamCurve", 0, 1, DO_Matrix_to_ParamCurve);
	praat_addAction1 (classMatrix, 0, L"Cast -", 0, 0, 0);
		praat_addAction1 (classMatrix, 0, L"To Cochleagram", 0, 1, DO_Matrix_to_Cochleagram);
		praat_addAction1 (classMatrix, 0, L"To Excitation", 0, 1, DO_Matrix_to_Excitation);
		praat_addAction1 (classMatrix, 0, L"To Harmonicity", 0, 1, DO_Matrix_to_Harmonicity);
		praat_addAction1 (classMatrix, 0, L"To Intensity", 0, 1, DO_Matrix_to_Intensity);
		praat_addAction1 (classMatrix, 0, L"To Ltas", 0, 1, DO_Matrix_to_Ltas);
		praat_addAction1 (classMatrix, 0, L"To Pitch", 0, 1, DO_Matrix_to_Pitch);
		praat_addAction1 (classMatrix, 0, L"To PointProcess", 0, 1, DO_Matrix_to_PointProcess);
		praat_addAction1 (classMatrix, 0, L"To Polygon", 0, 1, DO_Matrix_to_Polygon);
		praat_addAction1 (classMatrix, 0, L"To Sound", 0, 1, DO_Matrix_to_Sound);
		praat_addAction1 (classMatrix, 0, L"To Sound (slice)...", 0, 1, DO_Matrix_to_Sound_mono);
		praat_addAction1 (classMatrix, 0, L"To Spectrogram", 0, 1, DO_Matrix_to_Spectrogram);
		praat_addAction1 (classMatrix, 0, L"To TableOfReal", 0, 1, DO_Matrix_to_TableOfReal);
		praat_addAction1 (classMatrix, 0, L"To Spectrum", 0, 1, DO_Matrix_to_Spectrum);
		praat_addAction1 (classMatrix, 0, L"To Transition", 0, 1, DO_Matrix_to_Transition);
		praat_addAction1 (classMatrix, 0, L"To VocalTract", 0, 1, DO_Matrix_to_VocalTract);

	praat_addAction1 (classMovie, 1, L"Paint one image...", 0, 1, DO_Movie_paintOneImage);
	praat_addAction1 (classMovie, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Movie_viewAndEdit);

	praat_addAction1 (classParamCurve, 0, L"ParamCurve help", 0, 0, DO_ParamCurve_help);
	praat_addAction1 (classParamCurve, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classParamCurve, 0, L"Draw...", 0, 0, DO_ParamCurve_draw);

	praat_addAction1 (classPitch, 0, L"Pitch help", 0, 0, DO_Pitch_help);
	praat_addAction1 (classPitch, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Pitch_edit);
	praat_addAction1 (classPitch, 1, L"Edit", 0, praat_HIDDEN, DO_Pitch_edit);
	praat_addAction1 (classPitch, 0, L"Play -", 0, 0, 0);
		praat_addAction1 (classPitch, 0, L"Play pulses", 0, 1, DO_Pitch_play);
		praat_addAction1 (classPitch, 0, L"Hum", 0, 1, DO_Pitch_hum);
	praat_addAction1 (classPitch, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classPitch, 0, L"Draw...", 0, 1, DO_Pitch_draw);
		praat_addAction1 (classPitch, 0, L"Draw logarithmic...", 0, 1, DO_Pitch_drawLogarithmic);
		praat_addAction1 (classPitch, 0, L"Draw semitones...", 0, 1, DO_Pitch_drawSemitones);
		praat_addAction1 (classPitch, 0, L"Draw mel...", 0, 1, DO_Pitch_drawMel);
		praat_addAction1 (classPitch, 0, L"Draw erb...", 0, 1, DO_Pitch_drawErb);
		praat_addAction1 (classPitch, 0, L"Speckle...", 0, 1, DO_Pitch_speckle);
		praat_addAction1 (classPitch, 0, L"Speckle logarithmic...", 0, 1, DO_Pitch_speckleLogarithmic);
		praat_addAction1 (classPitch, 0, L"Speckle semitones...", 0, 1, DO_Pitch_speckleSemitones);
		praat_addAction1 (classPitch, 0, L"Speckle mel...", 0, 1, DO_Pitch_speckleMel);
		praat_addAction1 (classPitch, 0, L"Speckle erb...", 0, 1, DO_Pitch_speckleErb);
	praat_addAction1 (classPitch, 0, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classPitch);
		praat_addAction1 (classPitch, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, L"Count voiced frames", 0, 1, DO_Pitch_getNumberOfVoicedFrames);
		praat_addAction1 (classPitch, 1, L"Get value at time...", 0, 1, DO_Pitch_getValueAtTime);
		praat_addAction1 (classPitch, 1, L"Get value in frame...", 0, 1, DO_Pitch_getValueInFrame);
		praat_addAction1 (classPitch, 1, L"-- get extreme --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, L"Get minimum...", 0, 1, DO_Pitch_getMinimum);
		praat_addAction1 (classPitch, 1, L"Get time of minimum...", 0, 1, DO_Pitch_getTimeOfMinimum);
		praat_addAction1 (classPitch, 1, L"Get maximum...", 0, 1, DO_Pitch_getMaximum);
		praat_addAction1 (classPitch, 1, L"Get time of maximum...", 0, 1, DO_Pitch_getTimeOfMaximum);
		praat_addAction1 (classPitch, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, L"Get quantile...", 0, 1, DO_Pitch_getQuantile);
		/*praat_addAction1 (classPitch, 1, L"Get spreading...", 0, 1, DO_Pitch_getSpreading);*/
		praat_addAction1 (classPitch, 1, L"Get mean...", 0, 1, DO_Pitch_getMean);
		praat_addAction1 (classPitch, 1, L"Get standard deviation...", 0, 1, DO_Pitch_getStandardDeviation);
		praat_addAction1 (classPitch, 1, L"-- get slope --", 0, 1, 0);
		praat_addAction1 (classPitch, 1, L"Get mean absolute slope...", 0, 1, DO_Pitch_getMeanAbsoluteSlope);
		praat_addAction1 (classPitch, 1, L"Get slope without octave jumps", 0, 1, DO_Pitch_getMeanAbsSlope_noOctave);
		praat_addAction1 (classPitch, 2, L"-- query two --", 0, 1, 0);
		praat_addAction1 (classPitch, 2, L"Count differences", 0, 1, DO_Pitch_difference);
	praat_addAction1 (classPitch, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classPitch);
		praat_addAction1 (classPitch, 0, L"Formula...", 0, 1, DO_Pitch_formula);
	praat_addAction1 (classPitch, 0, L"Annotate -", 0, 0, 0);
		praat_addAction1 (classPitch, 0, L"To TextGrid...", 0, 1, DO_Pitch_to_TextGrid);
		praat_addAction1 (classPitch, 0, L"-- to single tier --", 0, 1, 0);
		praat_addAction1 (classPitch, 0, L"To TextTier", 0, 1, DO_Pitch_to_TextTier);
		praat_addAction1 (classPitch, 0, L"To IntervalTier", 0, 1, DO_Pitch_to_IntervalTier);
praat_addAction1 (classPitch, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classPitch, 0, L"To PointProcess", 0, 0, DO_Pitch_to_PointProcess);
praat_addAction1 (classPitch, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classPitch, 0, L"To Sound (pulses)", 0, 0, DO_Pitch_to_Sound_pulses);
	praat_addAction1 (classPitch, 0, L"To Sound (hum)", 0, 0, DO_Pitch_to_Sound_hum);
	praat_addAction1 (classPitch, 0, L"To Sound (sine)...", 0, 1, DO_Pitch_to_Sound_sine);
praat_addAction1 (classPitch, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classPitch, 0, L"Kill octave jumps", 0, 0, DO_Pitch_killOctaveJumps);
	praat_addAction1 (classPitch, 0, L"Interpolate", 0, 0, DO_Pitch_interpolate);
	praat_addAction1 (classPitch, 0, L"Smooth...", 0, 0, DO_Pitch_smooth);
	praat_addAction1 (classPitch, 0, L"Subtract linear fit...", 0, 0, DO_Pitch_subtractLinearFit);
	praat_addAction1 (classPitch, 0, L"Down to PitchTier", 0, 0, DO_Pitch_to_PitchTier);
	praat_addAction1 (classPitch, 0, L"To Matrix", 0, 0, DO_Pitch_to_Matrix);

	praat_addAction1 (classPitchTier, 1, L"Save as PitchTier spreadsheet file...", 0, 0, DO_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, L"Write to PitchTier spreadsheet file...", 0, praat_HIDDEN, DO_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, L"Save as headerless spreadsheet file...", 0, 0, DO_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, L"Write to headerless spreadsheet file...", 0, praat_HIDDEN, DO_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 0, L"PitchTier help", 0, 0, DO_PitchTier_help);
	praat_addAction1 (classPitchTier, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_PitchTier_edit);
	praat_addAction1 (classPitchTier, 1, L"Edit", 0, praat_HIDDEN, DO_PitchTier_edit);
	praat_addAction1 (classPitchTier, 0, L"View & Edit with Sound?", 0, 0, DO_info_PitchTier_Sound_edit);
	praat_addAction1 (classPitchTier, 0, L"Play pulses", 0, 0, DO_PitchTier_play);
	praat_addAction1 (classPitchTier, 0, L"Hum", 0, 0, DO_PitchTier_hum);
	praat_addAction1 (classPitchTier, 0, L"Play sine", 0, 0, DO_PitchTier_playSine);
	praat_addAction1 (classPitchTier, 0, L"Draw...", 0, 0, DO_PitchTier_draw);
	praat_addAction1 (classPitchTier, 0, L"& Manipulation: Replace?", 0, 0, DO_info_PitchTier_Manipulation_replace);
	praat_addAction1 (classPitchTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classPitchTier);
		praat_addAction1 (classPitchTier, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 1, L"Get value at time...", 0, 1, DO_PitchTier_getValueAtTime);
		praat_addAction1 (classPitchTier, 1, L"Get value at index...", 0, 1, DO_PitchTier_getValueAtIndex);
		praat_addAction1 (classPitchTier, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 1, L"Get mean (curve)...", 0, 1, DO_PitchTier_getMean_curve);
		praat_addAction1 (classPitchTier, 1, L"Get mean (points)...", 0, 1, DO_PitchTier_getMean_points);
		praat_addAction1 (classPitchTier, 1, L"Get standard deviation (curve)...", 0, 1, DO_PitchTier_getStandardDeviation_curve);
		praat_addAction1 (classPitchTier, 1, L"Get standard deviation (points)...", 0, 1, DO_PitchTier_getStandardDeviation_points);
	praat_addAction1 (classPitchTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classPitchTier);
		praat_addAction1 (classPitchTier, 0, L"Add point...", 0, 1, DO_PitchTier_addPoint);
		praat_addAction1 (classPitchTier, 0, L"Formula...", 0, 1, DO_PitchTier_formula);
		praat_addAction1 (classPitchTier, 0, L"-- stylize --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 0, L"Stylize...", 0, 1, DO_PitchTier_stylize);
		praat_addAction1 (classPitchTier, 0, L"Interpolate quadratically...", 0, 1, DO_PitchTier_interpolateQuadratically);
		praat_addAction1 (classPitchTier, 0, L"-- modify frequencies --", 0, 1, 0);
		praat_addAction1 (classPitchTier, 0, L"Shift frequencies...", 0, 1, DO_PitchTier_shiftFrequencies);
		praat_addAction1 (classPitchTier, 0, L"Multiply frequencies...", 0, 1, DO_PitchTier_multiplyFrequencies);
	praat_addAction1 (classPitchTier, 0, L"Synthesize -", 0, 0, 0);
		praat_addAction1 (classPitchTier, 0, L"To PointProcess", 0, 1, DO_PitchTier_to_PointProcess);
		praat_addAction1 (classPitchTier, 0, L"To Sound (pulse train)...", 0, 1, DO_PitchTier_to_Sound_pulseTrain);
		praat_addAction1 (classPitchTier, 0, L"To Sound (phonation)...", 0, 1, DO_PitchTier_to_Sound_phonation);
		praat_addAction1 (classPitchTier, 0, L"To Sound (sine)...", 0, 1, DO_PitchTier_to_Sound_sine);
	praat_addAction1 (classPitchTier, 0, L"Convert -", 0, 0, 0);
		praat_addAction1 (classPitchTier, 0, L"Down to PointProcess", 0, 1, DO_PitchTier_downto_PointProcess);
		praat_addAction1 (classPitchTier, 0, L"Down to TableOfReal...", 0, 1, DO_PitchTier_downto_TableOfReal);

	praat_addAction1 (classPointProcess, 0, L"PointProcess help", 0, 0, DO_PointProcess_help);
	praat_addAction1 (classPointProcess, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 1, L"View & Edit alone", 0, praat_HIDDEN, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 1, L"Edit alone", 0, praat_HIDDEN, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 0, L"View & Edit with Sound?", 0, 0, DO_info_PointProcess_Sound_edit);
	praat_addAction1 (classPointProcess, 0, L"Play -", 0, 0, 0);
		praat_addAction1 (classPointProcess, 0, L"Play as pulse train", 0, 1, DO_PointProcess_play);
		praat_addAction1 (classPointProcess, 0, L"Hum", 0, 1, DO_PointProcess_hum);
	praat_addAction1 (classPointProcess, 0, L"Draw...", 0, 0, DO_PointProcess_draw);
	praat_addAction1 (classPointProcess, 0, L"Query -", 0, 0, 0);
		praat_TimeFunction_query_init (classPointProcess);
		praat_addAction1 (classPointProcess, 1, L"-- script get --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 1, L"Get number of points", 0, 1, DO_PointProcess_getNumberOfPoints);
		praat_addAction1 (classPointProcess, 1, L"Get low index...", 0, 1, DO_PointProcess_getLowIndex);
		praat_addAction1 (classPointProcess, 1, L"Get high index...", 0, 1, DO_PointProcess_getHighIndex);
		praat_addAction1 (classPointProcess, 1, L"Get nearest index...", 0, 1, DO_PointProcess_getNearestIndex);
		praat_addAction1 (classPointProcess, 1, L"Get time from index...", 0, 1, DO_PointProcess_getTimeFromIndex);
		praat_addAction1 (classPointProcess, 1, L"Get interval...", 0, 1, DO_PointProcess_getInterval);
		praat_addAction1 (classPointProcess, 1, L"-- periods --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 1, L"Get number of periods...", 0, 1, DO_PointProcess_getNumberOfPeriods);
		praat_addAction1 (classPointProcess, 1, L"Get mean period...", 0, 1, DO_PointProcess_getMeanPeriod);
		praat_addAction1 (classPointProcess, 1, L"Get stdev period...", 0, 1, DO_PointProcess_getStdevPeriod);
		praat_addAction1 (classPointProcess, 1, L"Get jitter (local)...", 0, 1, DO_PointProcess_getJitter_local);
		praat_addAction1 (classPointProcess, 1, L"Get jitter (local, absolute)...", 0, 1, DO_PointProcess_getJitter_local_absolute);
		praat_addAction1 (classPointProcess, 1, L"Get jitter (rap)...", 0, 1, DO_PointProcess_getJitter_rap);
		praat_addAction1 (classPointProcess, 1, L"Get jitter (ppq5)...", 0, 1, DO_PointProcess_getJitter_ppq5);
		praat_addAction1 (classPointProcess, 1, L"Get jitter (ddp)...", 0, 1, DO_PointProcess_getJitter_ddp);
	praat_addAction1 (classPointProcess, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classPointProcess);
		praat_addAction1 (classPointProcess, 0, L"Add point...", 0, 1, DO_PointProcess_addPoint);
		praat_addAction1 (classPointProcess, 0, L"Remove point...", 0, 1, DO_PointProcess_removePoint);
		praat_addAction1 (classPointProcess, 0, L"Remove point near...", 0, 1, DO_PointProcess_removePointNear);
		praat_addAction1 (classPointProcess, 0, L"Remove points...", 0, 1, DO_PointProcess_removePoints);
		praat_addAction1 (classPointProcess, 0, L"Remove points between...", 0, 1, DO_PointProcess_removePointsBetween);
		praat_addAction1 (classPointProcess, 0, L"-- voice --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 0, L"Fill...", 0, 1, DO_PointProcess_fill);
		praat_addAction1 (classPointProcess, 0, L"Voice...", 0, 1, DO_PointProcess_voice);
	praat_addAction1 (classPointProcess, 0, L"Annotate -", 0, 0, 0);
		praat_addAction1 (classPointProcess, 0, L"To TextGrid...", 0, 1, DO_PointProcess_to_TextGrid);
		praat_addAction1 (classPointProcess, 0, L"-- to single tier --", 0, 1, 0);
		praat_addAction1 (classPointProcess, 0, L"To TextTier", 0, 1, DO_PointProcess_to_TextTier);
		praat_addAction1 (classPointProcess, 0, L"To IntervalTier", 0, 1, DO_PointProcess_to_IntervalTier);
praat_addAction1 (classPointProcess, 0, L"Set calculus", 0, 0, 0);
	praat_addAction1 (classPointProcess, 2, L"Union", 0, 0, DO_PointProcess_union);
	praat_addAction1 (classPointProcess, 2, L"Intersection", 0, 0, DO_PointProcess_intersection);
	praat_addAction1 (classPointProcess, 2, L"Difference", 0, 0, DO_PointProcess_difference);
praat_addAction1 (classPointProcess, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, L"To PitchTier...", 0, 0, DO_PointProcess_to_PitchTier);
	praat_addAction1 (classPointProcess, 0, L"To TextGrid (vuv)...", 0, 0, DO_PointProcess_to_TextGrid_vuv);
praat_addAction1 (classPointProcess, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, L"To Sound (pulse train)...", 0, 0, DO_PointProcess_to_Sound_pulseTrain);
	praat_addAction1 (classPointProcess, 0, L"To Sound (phonation)...", 0, 0, DO_PointProcess_to_Sound_phonation);
	praat_addAction1 (classPointProcess, 0, L"To Sound (hum)", 0, 0, DO_PointProcess_to_Sound_hum);
praat_addAction1 (classPointProcess, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classPointProcess, 0, L"To Matrix", 0, 0, DO_PointProcess_to_Matrix);
	praat_addAction1 (classPointProcess, 0, L"Up to TextTier...", 0, 0, DO_PointProcess_upto_TextTier);
	praat_addAction1 (classPointProcess, 0, L"Up to PitchTier...", 0, 0, DO_PointProcess_upto_PitchTier);
	praat_addAction1 (classPointProcess, 0, L"Up to IntensityTier...", 0, 0, DO_PointProcess_upto_IntensityTier);

	praat_addAction1 (classPolygon, 0, L"Polygon help", 0, 0, DO_Polygon_help);
praat_addAction1 (classPolygon, 0, L"Draw -", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, L"Draw...", 0, 1, DO_Polygon_draw);
	praat_addAction1 (classPolygon, 0, L"Draw closed...", 0, 1, DO_Polygon_drawClosed);
	praat_addAction1 (classPolygon, 0, L"Paint...", 0, 1, DO_Polygon_paint);
	praat_addAction1 (classPolygon, 0, L"Draw circles...", 0, 1, DO_Polygon_drawCircles);
	praat_addAction1 (classPolygon, 0, L"Paint circles...", 0, 1, DO_Polygon_paintCircles);
	praat_addAction1 (classPolygon, 2, L"Draw connection...", 0, 1, DO_Polygons_drawConnection);
praat_addAction1 (classPolygon, 0, L"Modify -", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, L"Randomize", 0, 1, DO_Polygon_randomize);
	praat_addAction1 (classPolygon, 0, L"Salesperson...", 0, 1, DO_Polygon_salesperson);
praat_addAction1 (classPolygon, 0, L"Hack -", 0, 0, 0);
	praat_addAction1 (classPolygon, 0, L"To Matrix", 0, 1, DO_Polygon_to_Matrix);

	praat_addAction1 (classSpectrogram, 0, L"Spectrogram help", 0, 0, DO_Spectrogram_help);
	praat_addAction1 (classSpectrogram, 1, L"View", 0, 0, DO_Spectrogram_view);
	praat_addAction1 (classSpectrogram, 1, L"Movie", 0, 0, DO_Spectrogram_movie);
	praat_addAction1 (classSpectrogram, 0, L"Query -", 0, 0, 0);
		praat_TimeFrameSampled_query_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 1, L"Get power at...", 0, 1, DO_Spectrogram_getPowerAt);
	praat_addAction1 (classSpectrogram, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classSpectrogram, 0, L"Paint...", 0, 1, DO_Spectrogram_paint);
	praat_addAction1 (classSpectrogram, 0, L"Analyse -", 0, 0, 0);
		praat_addAction1 (classSpectrogram, 0, L"To Spectrum (slice)...", 0, 1, DO_Spectrogram_to_Spectrum);
	praat_addAction1 (classSpectrogram, 0, L"Synthesize -", 0, 0, 0);
		praat_addAction1 (classSpectrogram, 0, L"To Sound...", 0, 1, DO_Spectrogram_to_Sound);
	praat_addAction1 (classSpectrogram, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 0, L"Formula...", 0, 1, DO_Spectrogram_formula);
	praat_addAction1 (classSpectrogram, 0, L"Hack -", 0, 0, 0);
		praat_addAction1 (classSpectrogram, 0, L"To Matrix", 0, 1, DO_Spectrogram_to_Matrix);

	praat_addAction1 (classSpectrum, 0, L"Spectrum help", 0, 0, DO_Spectrum_help);
	praat_addAction1 (classSpectrum, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Spectrum_edit);
	praat_addAction1 (classSpectrum, 1, L"Edit", 0, praat_HIDDEN, DO_Spectrum_edit);
	praat_addAction1 (classSpectrum, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"Draw...", 0, 1, DO_Spectrum_draw);
		praat_addAction1 (classSpectrum, 0, L"Draw (log freq)...", 0, 1, DO_Spectrum_drawLogFreq);
	praat_addAction1 (classSpectrum, 1, L"List...", 0, 0, DO_Spectrum_list);
	praat_addAction1 (classSpectrum, 1, L"Query -", 0, 0, 0);
		praat_addAction1 (classSpectrum, 1, L"Frequency domain", 0, 1, 0);
			praat_addAction1 (classSpectrum, 1, L"Get lowest frequency", 0, 2, DO_Spectrum_getLowestFrequency);
			praat_addAction1 (classSpectrum, 1, L"Get highest frequency", 0, 2, DO_Spectrum_getHighestFrequency);
		praat_addAction1 (classSpectrum, 1, L"Frequency sampling", 0, 1, 0);
			praat_addAction1 (classSpectrum, 1, L"Get number of bins", 0, 2, DO_Spectrum_getNumberOfBins);
			praat_addAction1 (classSpectrum, 1, L"Get bin width", 0, 2, DO_Spectrum_getBinWidth);
			praat_addAction1 (classSpectrum, 1, L"Get frequency from bin number...", 0, 2, DO_Spectrum_getFrequencyFromBin);
						praat_addAction1 (classSpectrum, 1, L"Get frequency from bin...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getFrequencyFromBin);
			praat_addAction1 (classSpectrum, 1, L"Get bin number from frequency...", 0, 2, DO_Spectrum_getBinFromFrequency);
						praat_addAction1 (classSpectrum, 1, L"Get bin from frequency...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getBinFromFrequency);
		praat_addAction1 (classSpectrum, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, L"Get real value in bin...", 0, 1, DO_Spectrum_getRealValueInBin);
		praat_addAction1 (classSpectrum, 1, L"Get imaginary value in bin...", 0, 1, DO_Spectrum_getImaginaryValueInBin);
		praat_addAction1 (classSpectrum, 1, L"-- get energy --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, L"Get band energy...", 0, 1, DO_Spectrum_getBandEnergy);
		praat_addAction1 (classSpectrum, 1, L"Get band density...", 0, 1, DO_Spectrum_getBandDensity);
		praat_addAction1 (classSpectrum, 1, L"Get band energy difference...", 0, 1, DO_Spectrum_getBandEnergyDifference);
		praat_addAction1 (classSpectrum, 1, L"Get band density difference...", 0, 1, DO_Spectrum_getBandDensityDifference);
		praat_addAction1 (classSpectrum, 1, L"-- get moments --", 0, 1, 0);
		praat_addAction1 (classSpectrum, 1, L"Get centre of gravity...", 0, 1, DO_Spectrum_getCentreOfGravity);
		praat_addAction1 (classSpectrum, 1, L"Get standard deviation...", 0, 1, DO_Spectrum_getStandardDeviation);
		praat_addAction1 (classSpectrum, 1, L"Get skewness...", 0, 1, DO_Spectrum_getSkewness);
		praat_addAction1 (classSpectrum, 1, L"Get kurtosis...", 0, 1, DO_Spectrum_getKurtosis);
		praat_addAction1 (classSpectrum, 1, L"Get central moment...", 0, 1, DO_Spectrum_getCentralMoment);
	praat_addAction1 (classSpectrum, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"Formula...", 0, 1, DO_Spectrum_formula);
		praat_addAction1 (classSpectrum, 0, L"Filter (pass Hann band)...", 0, 1, DO_Spectrum_passHannBand);
		praat_addAction1 (classSpectrum, 0, L"Filter (stop Hann band)...", 0, 1, DO_Spectrum_stopHannBand);
	praat_addAction1 (classSpectrum, 0, L"Analyse", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"To Excitation...", 0, 0, DO_Spectrum_to_Excitation);
		praat_addAction1 (classSpectrum, 0, L"To SpectrumTier (peaks)", 0, 0, DO_Spectrum_to_SpectrumTier_peaks);
		praat_addAction1 (classSpectrum, 0, L"To Formant (peaks)...", 0, 0, DO_Spectrum_to_Formant_peaks);
		praat_addAction1 (classSpectrum, 0, L"To Ltas...", 0, 0, DO_Spectrum_to_Ltas);
		praat_addAction1 (classSpectrum, 0, L"To Ltas (1-to-1)", 0, 0, DO_Spectrum_to_Ltas_1to1);
		praat_addAction1 (classSpectrum, 0, L"To Spectrogram", 0, 0, DO_Spectrum_to_Spectrogram);
	praat_addAction1 (classSpectrum, 0, L"Convert", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"Cepstral smoothing...", 0, 0, DO_Spectrum_cepstralSmoothing);
		praat_addAction1 (classSpectrum, 0, L"LPC smoothing...", 0, 0, DO_Spectrum_lpcSmoothing);
	praat_addAction1 (classSpectrum, 0, L"Synthesize", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"To Sound", 0, 0, DO_Spectrum_to_Sound);
						praat_addAction1 (classSpectrum, 0, L"To Sound (fft)", 0, praat_HIDDEN, DO_Spectrum_to_Sound);
	praat_addAction1 (classSpectrum, 0, L"Hack", 0, 0, 0);
		praat_addAction1 (classSpectrum, 0, L"To Matrix", 0, 0, DO_Spectrum_to_Matrix);

	praat_addAction1 (classSpectrumTier, 0, L"Draw...", 0, 0, DO_SpectrumTier_draw);
	praat_addAction1 (classSpectrumTier, 1, L"List...", 0, 0, DO_SpectrumTier_list);
	praat_addAction1 (classSpectrumTier, 0, L"Down to Table", 0, 0, DO_SpectrumTier_downto_Table);
	praat_addAction1 (classSpectrumTier, 0, L"Remove points below...", 0, 0, DO_SpectrumTier_removePointsBelow);

	praat_addAction1 (classStrings, 0, L"Strings help", 0, 0, DO_Strings_help);
	praat_addAction1 (classStrings, 1, L"Save as raw text file...", 0, 0, DO_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, L"Write to raw text file...", 0, praat_HIDDEN, DO_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Strings_edit);
	praat_addAction1 (classStrings, 1, L"Edit", 0, praat_HIDDEN, DO_Strings_edit);
	praat_addAction1 (classStrings, 0, L"Query", 0, 0, 0);
		praat_addAction1 (classStrings, 2, L"Equal?", 0, 0, DO_Strings_equal);
		praat_addAction1 (classStrings, 1, L"Get number of strings", 0, 0, DO_Strings_getNumberOfStrings);
		praat_addAction1 (classStrings, 1, L"Get string...", 0, 0, DO_Strings_getString);
	praat_addAction1 (classStrings, 0, L"Modify", 0, 0, 0);
		praat_addAction1 (classStrings, 0, L"Randomize", 0, 0, DO_Strings_randomize);
		praat_addAction1 (classStrings, 0, L"Sort", 0, 0, DO_Strings_sort);
		praat_addAction1 (classStrings, 0, L"Genericize", 0, 0, DO_Strings_genericize);
		praat_addAction1 (classStrings, 0, L"Nativize", 0, 0, DO_Strings_nativize);
	praat_addAction1 (classStrings, 0, L"Analyze", 0, 0, 0);
		praat_addAction1 (classStrings, 0, L"To Distributions", 0, 0, DO_Strings_to_Distributions);
	praat_addAction1 (classStrings, 0, L"Synthesize", 0, 0, 0);
		praat_addAction1 (classStrings, 0, L"To WordList", 0, 0, DO_Strings_to_WordList);

	praat_addAction1 (classTable, 0, L"Down to Matrix", 0, 0, DO_Table_to_Matrix);

	praat_addAction1 (classTransition, 0, L"Transition help", 0, 0, DO_Transition_help);
praat_addAction1 (classTransition, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classTransition, 0, L"Draw as numbers...", 0, 0, DO_Transition_drawAsNumbers);
praat_addAction1 (classTransition, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classTransition, 0, L"Eigen", 0, 0, DO_Transition_eigen);
	praat_addAction1 (classTransition, 0, L"Conflate", 0, 0, DO_Transition_conflate);
praat_addAction1 (classTransition, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classTransition, 0, L"Power...", 0, 0, DO_Transition_power);
praat_addAction1 (classTransition, 0, L"Cast", 0, 0, 0);
	praat_addAction1 (classTransition, 0, L"To Matrix", 0, 0, DO_Transition_to_Matrix);

	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_AmplitudeTier_edit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, L"Edit", 0, praat_HIDDEN, DO_AmplitudeTier_edit);   // hidden 2011
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, L"Multiply", 0, 0, DO_Sound_AmplitudeTier_multiply);
	praat_addAction2 (classDistributions, 1, classTransition, 1, L"Map", 0, 0, DO_Distributions_Transition_map);
	praat_addAction2 (classDistributions, 1, classTransition, 1, L"To Transition...", 0, 0, DO_Distributions_to_Transition_adj);
	praat_addAction2 (classDistributions, 2, classTransition, 1, L"To Transition (noise)...", 0, 0, DO_Distributions_to_Transition_noise_adj);
	praat_addAction2 (classDurationTier, 1, classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_DurationTier_edit);
	praat_addAction2 (classDurationTier, 1, classSound, 1, L"Edit", 0, praat_HIDDEN, DO_DurationTier_edit);
	praat_addAction2 (classFormant, 1, classPointProcess, 1, L"To FormantTier", 0, 0, DO_Formant_PointProcess_to_FormantTier);
	praat_addAction2 (classFormant, 1, classSound, 1, L"Filter", 0, 0, DO_Sound_Formant_filter);
	praat_addAction2 (classFormant, 1, classSound, 1, L"Filter (no scale)", 0, 0, DO_Sound_Formant_filter_noscale);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, L"Filter", 0, 0, DO_Sound_FormantGrid_filter);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, L"Filter (no scale)", 0, 0, DO_Sound_FormantGrid_filter_noscale);
	praat_addAction2 (classFormantTier, 1, classSound, 1, L"Filter", 0, 0, DO_Sound_FormantTier_filter);
	praat_addAction2 (classFormantTier, 1, classSound, 1, L"Filter (no scale)", 0, 0, DO_Sound_FormantTier_filter_noscale);
	praat_addAction2 (classIntensity, 1, classPitch, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classIntensity, 1, classPitch, 1, L"Draw (phonetogram)...", 0, 0, DO_Pitch_Intensity_draw);
	praat_addAction2 (classIntensity, 1, classPitch, 1, L"Speckle (phonetogram)...", 0, praat_HIDDEN, DO_Pitch_Intensity_speckle);   /* grandfathered 2005 */
	praat_addAction2 (classIntensity, 1, classPointProcess, 1, L"To IntensityTier", 0, 0, DO_Intensity_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classPointProcess, 1, L"To IntensityTier", 0, 0, DO_IntensityTier_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_IntensityTier_edit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, L"Edit", 0, praat_HIDDEN, DO_IntensityTier_edit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, L"Multiply", 0, praat_HIDDEN, DO_Sound_IntensityTier_multiply_old);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, L"Multiply...", 0, 0, DO_Sound_IntensityTier_multiply);
	praat_addAction2 (classManipulation, 1, classSound, 1, L"Replace original sound", 0, 0, DO_Manipulation_replaceOriginalSound);
	praat_addAction2 (classManipulation, 1, classPointProcess, 1, L"Replace pulses", 0, 0, DO_Manipulation_replacePulses);
	praat_addAction2 (classManipulation, 1, classPitchTier, 1, L"Replace pitch tier", 0, 0, DO_Manipulation_replacePitchTier);
	praat_addAction2 (classManipulation, 1, classDurationTier, 1, L"Replace duration tier", 0, 0, DO_Manipulation_replaceDurationTier);
	praat_addAction2 (classManipulation, 1, classTextTier, 1, L"To Manipulation", 0, 0, DO_Manipulation_TextTier_to_Manipulation);
	praat_addAction2 (classMatrix, 1, classSound, 1, L"To ParamCurve", 0, 0, DO_Matrix_to_ParamCurve);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, L"Draw...", 0, 0, DO_PitchTier_Pitch_draw);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, L"To Pitch", 0, 0, DO_Pitch_PitchTier_to_Pitch);
	praat_addAction2 (classPitch, 1, classPointProcess, 1, L"To PitchTier", 0, 0, DO_Pitch_PointProcess_to_PitchTier);
	praat_addAction3 (classPitch, 1, classPointProcess, 1, classSound, 1, L"Voice report...", 0, 0, DO_Sound_Pitch_PointProcess_voiceReport);
	praat_addAction2 (classPitch, 1, classSound, 1, L"To PointProcess (cc)", 0, 0, DO_Sound_Pitch_to_PointProcess_cc);
	praat_addAction2 (classPitch, 1, classSound, 1, L"To PointProcess (peaks)...", 0, 0, DO_Sound_Pitch_to_PointProcess_peaks);
	praat_addAction2 (classPitch, 1, classSound, 1, L"To Manipulation", 0, 0, DO_Sound_Pitch_to_Manipulation);
	praat_addAction2 (classPitchTier, 1, classPointProcess, 1, L"To PitchTier", 0, 0, DO_PitchTier_PointProcess_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_PitchTier_edit);
	praat_addAction2 (classPitchTier, 1, classSound, 1, L"Edit", 0, praat_HIDDEN, DO_PitchTier_edit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_PointProcess_edit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Edit", 0, praat_HIDDEN, DO_PointProcess_edit);
praat_addAction2 (classPointProcess, 1, classSound, 1, L"Query", 0, 0, 0);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (local)...", 0, 0, DO_Point_Sound_getShimmer_local);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (local_dB)...", 0, 0, DO_Point_Sound_getShimmer_local_dB);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (apq3)...", 0, 0, DO_Point_Sound_getShimmer_apq3);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (apq5)...", 0, 0, DO_Point_Sound_getShimmer_apq5);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (apq11)...", 0, 0, DO_Point_Sound_getShimmer_apq11);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Get shimmer (dda)...", 0, 0, DO_Point_Sound_getShimmer_dda);
praat_addAction2 (classPointProcess, 1, classSound, 1, L"Modify", 0, 0, 0);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"Transplant time domain", 0, 0, DO_Point_Sound_transplantDomain);
praat_addAction2 (classPointProcess, 1, classSound, 1, L"Analyse", 0, 0, 0);
	/*praat_addAction2 (classPointProcess, 1, classSound, 1, L"To Manipulation", 0, 0, DO_Sound_PointProcess_to_Manipulation);*/
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"To AmplitudeTier (point)", 0, 0, DO_PointProcess_Sound_to_AmplitudeTier_point);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"To AmplitudeTier (period)...", 0, 0, DO_PointProcess_Sound_to_AmplitudeTier_period);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"To Ltas...", 0, 0, DO_PointProcess_Sound_to_Ltas);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"To Ltas (only harmonics)...", 0, 0, DO_PointProcess_Sound_to_Ltas_harmonics);
praat_addAction2 (classPointProcess, 1, classSound, 1, L"Synthesize", 0, 0, 0);
	praat_addAction2 (classPointProcess, 1, classSound, 1, L"To Sound ensemble...", 0, 0, DO_Sound_PointProcess_to_SoundEnsemble_correlate);

	praat_addAction4 (classDurationTier, 1, classPitchTier, 1, classPointProcess, 1, classSound, 1, L"To Sound...", 0, 0, DO_Sound_Point_Pitch_Duration_to_Sound);

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
	INCLUDE_MANPAGES (manual_Exp_init)

	praat_addMenuCommand (L"Objects", L"New", L"-- new synthesis --", 0, 0, 0);
	INCLUDE_LIBRARY (praat_uvafon_Artsynth_init)
	INCLUDE_LIBRARY (praat_uvafon_David_init)
	praat_addMenuCommand (L"Objects", L"New", L"-- new grammars --", 0, 0, 0);
	INCLUDE_LIBRARY (praat_uvafon_gram_init)
	INCLUDE_LIBRARY (praat_uvafon_FFNet_init)
	INCLUDE_LIBRARY (praat_uvafon_LPC_init)
	INCLUDE_LIBRARY (praat_uvafon_Exp_init)
	INCLUDE_LIBRARY (praat_EEG_init)
}

/* End of file praat_Fon.c */
