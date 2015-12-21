/* praat_Fon.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
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
#include "Photo.h"
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

#undef iam
#define iam iam_LOOP

static const char32 *STRING_FROM_TIME_SECONDS = U"left Time range (s)";
static const char32 *STRING_TO_TIME_SECONDS = U"right Time range (s)";
static const char32 *STRING_FROM_TIME = U"left Time range";
static const char32 *STRING_TO_TIME = U"right Time range";
static const char32 *STRING_FROM_FREQUENCY_HZ = U"left Frequency range (Hz)";
static const char32 *STRING_TO_FREQUENCY_HZ = U"right Frequency range (Hz)";
static const char32 *STRING_FROM_FREQUENCY = U"left Frequency range";
static const char32 *STRING_TO_FREQUENCY = U"right Frequency range";

/***** Common dialog contents. *****/

void praat_dia_timeRange (UiForm dia);
void praat_dia_timeRange (UiForm dia) {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
}
void praat_get_timeRange (UiForm dia, double *tmin, double *tmax);
void praat_get_timeRange (UiForm dia, double *tmin, double *tmax) {
	*tmin = GET_REAL (STRING_FROM_TIME);
	*tmax = GET_REAL (STRING_TO_TIME);
}
int praat_get_frequencyRange (UiForm dia, double *fmin, double *fmax);
int praat_get_frequencyRange (UiForm dia, double *fmin, double *fmax) {
	*fmin = GET_REAL (STRING_FROM_FREQUENCY);
	*fmax = GET_REAL (STRING_TO_FREQUENCY);
	REQUIRE (*fmax > *fmin, U"Maximum frequency must be greater than minimum frequency.")
	return 1;
}
static void dia_Vector_getExtremum (UiForm dia) {
	UiField radio;
	praat_dia_timeRange (dia);
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
}
static void dia_Vector_getValue (UiForm dia) {
	UiField radio;
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Interpolation", 3)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
}

static void getTminTmaxFminFmax (UiForm dia, double *tmin, double *tmax, double *fmin, double *fmax) {
	*tmin = GET_REAL (STRING_FROM_TIME);
	*tmax = GET_REAL (STRING_TO_TIME);
	*fmin = GET_REAL (STRING_FROM_FREQUENCY);
	*fmax = GET_REAL (STRING_TO_FREQUENCY);
	REQUIRE (*fmax > *fmin, U"Maximum frequency must be greater than minimum frequency.")
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
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
	return 1;
}

extern "C" Graphics Movie_create (const char32 *title, int width, int height);
extern "C" Graphics Movie_create (const char32 *title, int width, int height) {
	static autoGraphics graphics;
	static GuiDialog dialog;
	static GuiDrawingArea drawingArea;
	if (! graphics) {
		dialog = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width + 2, height + 2, title, nullptr, nullptr, 0);
		drawingArea = GuiDrawingArea_createShown (dialog, 0, width, 0, height, nullptr, nullptr, nullptr, nullptr, nullptr, 0);
		GuiThing_show (dialog);
		graphics = Graphics_create_xmdrawingarea (drawingArea);
	}
	GuiShell_setTitle (dialog, title);
	GuiControl_setSize (dialog, width + 2, height + 2);
	GuiControl_setSize (drawingArea, width, height);
	GuiThing_show (dialog);
	return graphics.get();
}

/***** AMPLITUDETIER *****/

FORM (AmplitudeTier_addPoint, U"Add one point", U"AmplitudeTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Sound pressure (Pa)", U"0.8")
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Sound pressure"));
		praat_dataChanged (me);
	}
END2 }

FORM (AmplitudeTier_create, U"Create empty AmplitudeTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoAmplitudeTier thee = AmplitudeTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (AmplitudeTier_downto_PointProcess) {
	LOOP {
		iam (AmplitudeTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (AmplitudeTier_downto_TableOfReal) {
	LOOP {
		iam (AmplitudeTier);
		autoTableOfReal thee = AmplitudeTier_downto_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (AmplitudeTier_edit) {
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
END2 }

FORM (AmplitudeTier_formula, U"AmplitudeTier: Formula", U"AmplitudeTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in Pascal")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"- self ; upside down")
	OK2
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
END2 }

static void dia_AmplitudeTier_getRangeProperty (UiForm dia) {
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum amplitude factor", U"1.6")
}

FORM (AmplitudeTier_getShimmer_local, U"AmplitudeTier: Get shimmer (local)", U"AmplitudeTier: Get shimmer (local)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

FORM (AmplitudeTier_getShimmer_local_dB, U"AmplitudeTier: Get shimmer (local, dB)", U"AmplitudeTier: Get shimmer (local, dB)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_local_dB (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

FORM (AmplitudeTier_getShimmer_apq3, U"AmplitudeTier: Get shimmer (apq3)", U"AmplitudeTier: Get shimmer (apq3)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq3 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

FORM (AmplitudeTier_getShimmer_apq5, U"AmplitudeTier: Get shimmer (apq5)", U"AmplitudeTier: Get shimmer (apq5)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq5 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

FORM (AmplitudeTier_getShimmer_apq11, U"AmplitudeTier: Get shimmer (apq11)", U"AmplitudeTier: Get shimmer (apq11)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_apq11 (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

FORM (AmplitudeTier_getShimmer_dda, U"AmplitudeTier: Get shimmer (dda)", U"AmplitudeTier: Get shimmer (dda)...") {
	dia_AmplitudeTier_getRangeProperty (dia);
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		double shimmer = AmplitudeTier_getShimmer_dda (me,
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum amplitude factor"));
		Melder_informationReal (shimmer, nullptr);
	}
END2 }

/*FORM (AmplitudeTier_getValueAtTime, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_informationReal (RealTier_getValueAtTime (ONLY_OBJECT, GET_REAL (U"Time")), U"Pa");
END2 }
	
FORM (AmplitudeTier_getValueAtIndex, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK2
DO
	Melder_informationReal (RealTier_getValueAtIndex (ONLY_OBJECT, GET_INTEGER (U"Point number")), U"Pa");
END2 }*/

DIRECT2 (AmplitudeTier_help) {
	Melder_help (U"AmplitudeTier");
END2 }

FORM (AmplitudeTier_to_IntensityTier, U"AmplitudeTier: To IntensityTier", U"AmplitudeTier: To IntensityTier...") {
	REAL (U"Threshold (dB)", U"-10000.0")
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		autoIntensityTier thee = AmplitudeTier_to_IntensityTier (me, GET_REAL (U"Threshold"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (AmplitudeTier_to_Sound, U"AmplitudeTier: To Sound (pulse train)", U"AmplitudeTier: To Sound (pulse train)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	OK2
DO
	LOOP {
		iam (AmplitudeTier);
		autoSound thee = AmplitudeTier_to_Sound (me, GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Interpolation depth"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (info_AmplitudeTier_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your AmplitudeTier editor:\n"
		"   select an AmplitudeTier and a Sound, and click \"View & Edit\".");
END2 }

/***** AMPLITUDETIER & SOUND *****/

DIRECT2 (Sound_AmplitudeTier_multiply) {
	Sound sound = nullptr;
	AmplitudeTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classAmplitudeTier) tier = (AmplitudeTier) OBJECT;
	}
	autoSound thee = Sound_AmplitudeTier_multiply (sound, tier);
	praat_new (thee.move(), sound -> name, U"_amp");
END2 }

/***** COCHLEAGRAM *****/

FORM (Cochleagram_difference, U"Cochleagram difference", nullptr) {
	praat_dia_timeRange (dia);
	OK2
DO
	Cochleagram coch1 = nullptr, coch2 = nullptr;
	LOOP (coch1 ? coch2 : coch1) = (Cochleagram) OBJECT;
	Melder_informationReal (Cochleagram_difference (coch1, coch2,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range")), U"Hertz (root-mean-square)");
END2 }

FORM (Cochleagram_formula, U"Cochleagram Formula", U"Cochleagram: Formula...") {
	LABEL (U"label", U"`x' is time in seconds, `y' is place in Bark")
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
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
END2 }

DIRECT2 (Cochleagram_help) {
	Melder_help (U"Cochleagram");
END2 }

DIRECT2 (Cochleagram_movie) {
	Graphics g = Movie_create (U"Cochleagram movie", 300, 300);
	LOOP {
		iam (Cochleagram);
		Matrix_movie (me, g);
	}
END2 }

FORM (Cochleagram_paint, U"Paint Cochleagram", nullptr) {
	praat_dia_timeRange (dia);
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (Cochleagram);
		autoPraatPicture picture;
		Cochleagram_paint (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Cochleagram_to_Excitation, U"From Cochleagram to Excitation", nullptr) {
	REAL (U"Time (s)", U"0.0")
	OK2
DO
	LOOP {
		iam (Cochleagram);
		autoExcitation thee = Cochleagram_to_Excitation (me, GET_REAL (U"Time"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Cochleagram_to_Matrix) {
	LOOP {
		iam (Cochleagram);
		autoMatrix thee = Cochleagram_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** CORPUS *****/

FORM (Corpus_create, U"Create Corpus", U"Create Corpus...") {
	WORD (U"Name", U"myCorpus")
	LABEL (U"", U"Folder with sound files:")
	TEXTFIELD (U"folderWithSoundFiles", U"")
	LABEL (U"", U"Folder with annotation files:")
	TEXTFIELD (U"folderWithAnnotationFiles", U"")
	OK2
DO
END2 }

DIRECT2 (Corpus_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a Corpus from batch.");
	LOOP {
		iam (Corpus);
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

/***** DISTRIBUTIONS *****/

FORM (Distributions_to_Transition, U"To Transition", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK2
DO
	LOOP {
		iam (Distributions);
		autoTransition thee = Distributions_to_Transition (me, nullptr, GET_INTEGER (U"Environment"), nullptr, GET_INTEGER (U"Greedy"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Distributions_to_Transition_adj, U"To Transition", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK2
DO
	Distributions dist = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (dist, nullptr, GET_INTEGER (U"Environment"), trans, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END2 }

FORM (Distributions_to_Transition_noise, U"To Transition (noise)", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK2
DO
	Distributions underlying = nullptr, surface = nullptr;
	LOOP (underlying ? surface : underlying) = (Distributions) OBJECT;
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (U"Environment"), nullptr, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END2 }

FORM (Distributions_to_Transition_noise_adj, U"To Transition (noise)", nullptr) {
	NATURAL (U"Environment", U"1")
	BOOLEAN (U"Greedy", true)
	OK2
DO
	Distributions underlying = nullptr, surface = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) (underlying ? surface : underlying) = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoTransition thee = Distributions_to_Transition (underlying, surface, GET_INTEGER (U"Environment"), trans, GET_INTEGER (U"Greedy"));
	praat_new (thee.move());
END2 }

/***** DISTRIBUTIONS & TRANSITION *****/

DIRECT2 (Distributions_Transition_map) {
	Distributions dist = nullptr;
	Transition trans = nullptr;
	LOOP {
		if (CLASS == classDistributions) dist = (Distributions) OBJECT;
		if (CLASS == classTransition) trans = (Transition) OBJECT;
	}
	autoDistributions thee = Distributions_Transition_map (dist, trans);
	praat_new (thee.move(), U"surface");
END2 }

/***** DURATIONTIER *****/

FORM (DurationTier_addPoint, U"Add one point to DurationTier", U"DurationTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Relative duration", U"1.5")
	OK2
DO
	LOOP {
		iam (DurationTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Relative duration"));
		praat_dataChanged (me);
	}
END2 }

FORM (DurationTier_create, U"Create empty DurationTier", U"Create DurationTier...") {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoDurationTier thee = DurationTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (DurationTier_downto_PointProcess) {
	LOOP {
		iam (DurationTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (DurationTier_edit) {
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
END2 }

FORM (DurationTier_formula, U"DurationTier: Formula", U"DurationTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in relative units")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self * 1.5 ; slow down")
	OK2
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
END2 }

FORM (DurationTier_getTargetDuration, U"Get target duration", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	OK2
DO
	LOOP {
		iam (DurationTier);
		double area = RealTier_getArea (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (area, U"seconds");
	}
END2 }

FORM (DurationTier_getValueAtTime, U"Get DurationTier value", U"DurationTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (U"Time"));
		Melder_informationReal (value, nullptr);
	}
END2 }
	
FORM (DurationTier_getValueAtIndex, U"Get DurationTier value", U"Duration: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK2
DO
	LOOP {
		iam (DurationTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (U"Point number"));
		Melder_informationReal (value, nullptr);
	}
END2 }

DIRECT2 (DurationTier_help) {
	Melder_help (U"DurationTier");
END2 }

DIRECT2 (info_DurationTier_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your DurationTier editor:\n"
		"   select a DurationTier and a Sound, and click \"View & Edit\".");
END2 }

/***** EXCITATION *****/

FORM (Excitation_draw, U"Draw Excitation", 0) {
	REAL (U"From frequency (Bark)", U"0")
	REAL (U"To frequency (Bark)", U"25.6")
	REAL (U"Minimum (phon)", U"0")
	REAL (U"Maximum (phon)", U"100")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	LOOP {
		iam (Excitation);
		autoPraatPicture picture;
		Excitation_draw (me, GRAPHICS,
			GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Excitation_formula, U"Excitation Formula", U"Excitation: Formula...") {
	LABEL (U"label", U"`x' is the place in Bark, `col' is the bin number")
	LABEL (U"label", U"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"self")
	OK2
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
END2 }

DIRECT2 (Excitation_getLoudness) {
	LOOP {
		iam (Excitation);
		double loudness = Excitation_getLoudness (me);
		Melder_informationReal (loudness, U"sones");
	}
END2 }

DIRECT2 (Excitation_help) {
	Melder_help (U"Excitation");
END2 }

FORM (Excitation_to_Formant, U"From Excitation to Formant", 0) {
	NATURAL (U"Maximum number of formants", U"20")
	OK2
DO
	LOOP {
		iam (Excitation);
		autoFormant thee = Excitation_to_Formant (me, GET_INTEGER (U"Maximum number of formants"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Excitation_to_Matrix) {
	LOOP {
		iam (Excitation);
		autoMatrix thee = Excitation_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** FORMANT *****/

DIRECT2 (Formant_downto_FormantGrid) {
	LOOP {
		iam (Formant);
		autoFormantGrid thee = Formant_downto_FormantGrid (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Formant_downto_FormantTier) {
	LOOP {
		iam (Formant);
		autoFormantTier thee = Formant_downto_FormantTier (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Formant_drawSpeckles, U"Draw Formant", U"Formant: Draw speckles...") {
	praat_dia_timeRange (dia);
	POSITIVE (U"Maximum frequency (Hz)", U"5500.0")
	REAL (U"Dynamic range (dB)", U"30.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_drawSpeckles (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Maximum frequency"),
			GET_REAL (U"Dynamic range"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Formant_drawTracks, U"Draw formant tracks", U"Formant: Draw tracks...") {
	praat_dia_timeRange (dia);
	POSITIVE (U"Maximum frequency (Hz)", U"5500.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_drawTracks (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Maximum frequency"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Formant_formula_bandwidths, U"Formant: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	LABEL (U"", U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	TEXTFIELD (U"formula", U"self / 2 ; sharpen all peaks")
	OK2
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
END2 }

FORM (Formant_formula_frequencies, U"Formant: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	LABEL (U"", U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK2
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
END2 }

FORM (Formant_getBandwidthAtTime, U"Formant: Get bandwidth", U"Formant: Get bandwidth at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Linear")
	OK2
DO
	LOOP {
		iam (Formant);
		double bandwidth = Formant_getBandwidthAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (bandwidth, GET_STRING (U"Unit"));
		break;
	}
END2 }
	
FORM (Formant_getMaximum, U"Formant: Get maximum", U"Formant: Get maximum...") {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	LOOP {
		iam (Formant);
		double maximum = Formant_getMaximum (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, GET_STRING (U"Unit"));
		break;
	}
END2 }

DIRECT2 (Formant_getMaximumNumberOfFormants) {
	LOOP {
		iam (Formant);
		long maximumNumberOfFormants = Formant_getMaxNumFormants (me);
		Melder_information (maximumNumberOfFormants, U" (there are at most this many formants in every frame)");
		break;
	}
END2 }

FORM (Formant_getMean, U"Formant: Get mean", U"Formant: Get mean...") {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	OK2
DO
	LOOP {
		iam (Formant);
		double mean = Formant_getMean (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (mean, GET_STRING (U"Unit"));
		break;
	}
END2 }

FORM (Formant_getMinimum, U"Formant: Get minimum", U"Formant: Get minimum...") {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	LOOP {
		iam (Formant);
		double minimum = Formant_getMinimum (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1,
			GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, GET_STRING (U"Unit"));
		break;
	}
END2 }

DIRECT2 (Formant_getMinimumNumberOfFormants) {
	LOOP {
		iam (Formant);
		long minimumNumberOfFormants = Formant_getMinNumFormants (me);
		Melder_information (minimumNumberOfFormants, U" (there are at least this many formants in every frame)");
		break;
	}
END2 }

FORM (Formant_getNumberOfFormants, U"Formant: Get number of formants", U"Formant: Get number of formants...") {
	NATURAL (U"Frame number", U"1")
	OK2
DO
	LOOP {
		iam (Formant);
		long frame = GET_INTEGER (U"Frame number");
		if (frame > my nx) Melder_throw (U"There is no frame ", frame, U" in a Formant with only ", my nx, U" frames.");
		Melder_information (my d_frames [frame]. nFormants, U" formants");
		break;
	}
END2 }

FORM (Formant_getQuantile, U"Formant: Get quantile", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (U"Quantile", U"0.50 (= median)")
	OK2
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantile (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"Quantile"), GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (U"Unit"));
		break;
	}
END2 }

FORM (Formant_getQuantileOfBandwidth, U"Formant: Get quantile of bandwidth", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	REAL (U"Quantile", U"0.50 (= median)")
	OK2
DO
	LOOP {
		iam (Formant);
		double quantile = Formant_getQuantileOfBandwidth (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"Quantile"), GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (quantile, GET_STRING (U"Unit"));
		break;
	}
END2 }

FORM (Formant_getStandardDeviation, U"Formant: Get standard deviation", nullptr) {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	OK2
DO
	LOOP {
		iam (Formant);
		double stdev = Formant_getStandardDeviation (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (stdev, GET_STRING (U"Unit"));
		break;
	}
END2 }

FORM (Formant_getTimeOfMaximum, U"Formant: Get time of maximum", U"Formant: Get time of maximum...") {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMaximum (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
		break;
	}
END2 }

FORM (Formant_getTimeOfMinimum, U"Formant: Get time of minimum", U"Formant: Get time of minimum...") {
	NATURAL (U"Formant number", U"1")
	praat_dia_timeRange (dia);
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	LOOP {
		iam (Formant);
		double time = Formant_getTimeOfMinimum (me, GET_INTEGER (U"Formant number"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"Unit") - 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
		break;
	}
END2 }

FORM (Formant_getValueAtTime, U"Formant: Get value", U"Formant: Get value at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Bark")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Linear")
	OK2
DO
	LOOP {
		iam (Formant);
		double value = Formant_getValueAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_INTEGER (U"Unit") - 1);
		Melder_informationReal (value, GET_STRING (U"Unit"));
		break;
	}
END2 }
	
DIRECT2 (Formant_help) {
	Melder_help (U"Formant");
END2 }

FORM (Formant_downto_Table, U"Formant: Down to Table", nullptr) {
	BOOLEAN (U"Include frame number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include intensity", false)
	NATURAL (U"Intensity decimals", U"3")
	BOOLEAN (U"Include number of formants", true)
	NATURAL (U"Frequency decimals", U"3")
	BOOLEAN (U"Include bandwidths", true)
	OK2
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
END2 }

FORM (Formant_list, U"Formant: List", 0) {
	BOOLEAN (U"Include frame number", false)
	BOOLEAN (U"Include time", true)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include intensity", false)
	NATURAL (U"Intensity decimals", U"3")
	BOOLEAN (U"Include number of formants", true)
	NATURAL (U"Frequency decimals", U"3")
	BOOLEAN (U"Include bandwidths", true)
	OK2
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
END2 }

FORM (Formant_scatterPlot, U"Formant: Scatter plot", nullptr) {
	praat_dia_timeRange (dia);
	NATURAL (U"Horizontal formant number", U"2")
	REAL (U"left Horizontal range (Hz)", U"3000")
	REAL (U"right Horizontal range (Hz)", U"400")
	NATURAL (U"Vertical formant number", U"1")
	REAL (U"left Vertical range (Hz)", U"1500")
	REAL (U"right Vertical range (Hz)", U"100")
	POSITIVE (U"Mark size (mm)", U"1.0")
	BOOLEAN (U"Garnish", true)
	SENTENCE (U"Mark string (+xo.)", U"+")
	OK2
DO
	LOOP {
		iam (Formant);
		autoPraatPicture picture;
		Formant_scatterPlot (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_INTEGER (U"Horizontal formant number"),
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_INTEGER (U"Vertical formant number"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Mark size"), GET_STRING (U"Mark string"), GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT2 (Formant_sort) {
	LOOP {
		iam (Formant);
		Formant_sort (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Formant_to_Matrix, U"From Formant to Matrix", nullptr) {
	INTEGER (U"Formant", U"1")
	OK2
DO
	LOOP {
		iam (Formant);
		autoMatrix thee = Formant_to_Matrix (me, GET_INTEGER (U"Formant"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Formant_tracker, U"Formant tracker", U"Formant: Track...") {
	NATURAL (U"Number of tracks (1-5)", U"3")
	REAL (U"Reference F1 (Hz)", U"550")
	REAL (U"Reference F2 (Hz)", U"1650")
	REAL (U"Reference F3 (Hz)", U"2750")
	REAL (U"Reference F4 (Hz)", U"3850")
	REAL (U"Reference F5 (Hz)", U"4950")
	REAL (U"Frequency cost (/kHz)", U"1.0")
	REAL (U"Bandwidth cost", U"1.0")
	REAL (U"Transition cost (/octave)", U"1.0")
	OK2
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
END2 }

/***** FORMANT & POINTPROCESS *****/

DIRECT2 (Formant_PointProcess_to_FormantTier) {
	Formant formant = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (formant && point) break;
	}
	autoFormantTier thee = Formant_PointProcess_to_FormantTier (formant, point);
	praat_new (thee.move(), formant -> name, U"_", point -> name);
END2 }

/***** FORMANT & SOUND *****/

DIRECT2 (Sound_Formant_filter) {
	Sound sound = nullptr;
	Formant formant = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter (sound, formant);
	praat_new (thee.move(), sound -> name, U"_filt");
END2 }

DIRECT2 (Sound_Formant_filter_noscale) {
	Sound sound = nullptr;
	Formant formant = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classFormant) formant = (Formant) OBJECT;
		if (sound && formant) break;
	}
	autoSound thee = Sound_Formant_filter_noscale (sound, formant);
	praat_new (thee.move(), sound -> name, U"_filt");
END2 }

/***** FORMANTGRID *****/

FORM (FormantGrid_addBandwidthPoint, U"FormantGrid: Add bandwidth point", U"FormantGrid: Add bandwidth point...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	OK2
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addBandwidthPoint (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_REAL (U"Bandwidth"));
		praat_dataChanged (me);
	}
END2 }

FORM (FormantGrid_addFormantPoint, U"FormantGrid: Add formant point", U"FormantGrid: Add formant point...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	POSITIVE (U"Frequency (Hz)", U"550.0")
	OK2
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_addFormantPoint (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"), GET_REAL (U"Frequency"));
		praat_dataChanged (OBJECT);
	}
END2 }

FORM (FormantGrid_create, U"Create FormantGrid", nullptr) {
	WORD (U"Name", U"schwa")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	NATURAL (U"Number of formants", U"10")
	POSITIVE (U"Initial first formant (Hz)", U"550.0")
	POSITIVE (U"Initial formant spacing (Hz)", U"1100.0")
	REAL (U"Initial first bandwidth (Hz)", U"60.0")
	REAL (U"Initial bandwidth spacing (Hz)", U"50.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoFormantGrid thee = FormantGrid_create (startTime, endTime, GET_INTEGER (U"Number of formants"),
		GET_REAL (U"Initial first formant"), GET_REAL (U"Initial formant spacing"),
		GET_REAL (U"Initial first bandwidth"), GET_REAL (U"Initial bandwidth spacing"));
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

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
DIRECT2 (FormantGrid_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a FormantGrid from batch.");
	LOOP {
		iam (FormantGrid);
		autoFormantGridEditor editor = FormantGridEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.peek(), cb_FormantGridEditor_publish);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (FormantGrid_formula_bandwidths, U"FormantGrid: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	LABEL (U"", U"self [] is the FormantGrid itself, so it returns frequencies, not bandwidths!")
	TEXTFIELD (U"formula", U"self / 10 ; one tenth of the formant frequency")
	OK2
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
END2 }

FORM (FormantGrid_formula_frequencies, U"FormantGrid: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD (U"formula", U"if row = 2 then self + 200 else self fi")
	OK2
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
END2 }

DIRECT2 (FormantGrid_help) {
	Melder_help (U"FormantGrid");
END2 }

FORM (FormantGrid_removeBandwidthPointsBetween, U"Remove bandwidth points between", U"FormantGrid: Remove bandwidth points between...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"From time (s)", U"0.3")
	REAL (U"To time (s)", U"0.7")
	OK2
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeBandwidthPointsBetween (me, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END2 }

FORM (FormantGrid_removeFormantPointsBetween, U"Remove formant points between", U"FormantGrid: Remove formant points between...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"From time (s)", U"0.3")
	REAL (U"To time (s)", U"0.7")
	OK2
DO
	LOOP {
		iam (FormantGrid);
		FormantGrid_removeFormantPointsBetween (me, GET_INTEGER (U"Formant number"), GET_REAL (U"From time"), GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END2 }

FORM (FormantGrid_to_Formant, U"FormantGrid: To Formant", nullptr) {
	POSITIVE (U"Time step (s)", U"0.01")
	REAL (U"Intensity (Pa\u00B2)", U"0.1")
	OK2
DO
	double intensity = GET_REAL (U"Intensity");
	if (intensity < 0.0) Melder_throw (U"Intensity cannot be negative.");
	LOOP {
		iam (FormantGrid);
		autoFormant thee = FormantGrid_to_Formant (me, GET_REAL (U"Time step"), intensity);
		praat_new (thee.move(), my name);
	}
END2 }

/***** FORMANTGRID & SOUND *****/

DIRECT2 (Sound_FormantGrid_filter) {
	Sound me = nullptr;
	FormantGrid grid = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter (me, grid);
	praat_new (thee.move(), my name, U"_filt");
END2 }

DIRECT2 (Sound_FormantGrid_filter_noscale) {
	Sound me = nullptr;
	FormantGrid grid = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantGrid) grid = (FormantGrid) OBJECT;
		if (me && grid) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantGrid_filter_noscale (me, grid);
	praat_new (thee.move(), my name, U"_filt");
END2 }

/***** FORMANTTIER *****/

FORM (FormantTier_addPoint, U"Add one point", U"FormantTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	LABEL (U"", U"Frequencies and bandwidths (Hz):")
	TEXTFIELD (U"fb pairs", U"500 50 1500 100 2500 150 3500 200 4500 300")
	OK2
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
		autoFormantPoint point2 = Data_copy (point.peek());
		AnyTier_addPoint_move (me->asAnyTier(), point2.move());
		praat_dataChanged (me);
	}
END2 }

FORM (FormantTier_create, U"Create empty FormantTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoFormantTier thee = FormantTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

FORM (FormantTier_downto_TableOfReal, U"Down to TableOfReal", nullptr) {
	BOOLEAN (U"Include formants", true)
	BOOLEAN (U"Include bandwidths", false)
	OK2
DO
	LOOP {
		iam (FormantTier);
		autoTableOfReal thee = FormantTier_downto_TableOfReal (me, GET_INTEGER (U"Include formants"), GET_INTEGER (U"Include bandwidths"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (FormantTier_getBandwidthAtTime, U"FormantTier: Get bandwidth", U"FormantTier: Get bandwidth at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (FormantTier);
		double bandwidth = FormantTier_getBandwidthAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"));
		Melder_informationReal (bandwidth, U"hertz");
	}
END2 }
	
FORM (FormantTier_getValueAtTime, U"FormantTier: Get value", U"FormantTier: Get value at time...") {
	NATURAL (U"Formant number", U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (FormantTier);
		double value = FormantTier_getValueAtTime (me, GET_INTEGER (U"Formant number"), GET_REAL (U"Time"));
		Melder_informationReal (value, U"hertz");
	}
END2 }
	
DIRECT2 (FormantTier_help) {
	Melder_help (U"FormantTier");
END2 }

FORM (FormantTier_speckle, U"Draw FormantTier", nullptr) {
	praat_dia_timeRange (dia);
	POSITIVE (U"Maximum frequency (Hz)", U"5500.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	LOOP {
		iam (FormantTier);
		autoPraatPicture picture;
		FormantTier_speckle (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Maximum frequency"), GET_INTEGER (U"Garnish"));
	}
END2 }

/***** FORMANTTIER & SOUND *****/

DIRECT2 (Sound_FormantTier_filter) {
	Sound me = nullptr;
	FormantTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter (me, tier);
	praat_new (thee.move(), my name, U"_filt");
END2 }

DIRECT2 (Sound_FormantTier_filter_noscale) {
	Sound me = nullptr;
	FormantTier tier = nullptr;
	LOOP {
		if (CLASS == classSound) me = (Sound) OBJECT;
		if (CLASS == classFormantTier) tier = (FormantTier) OBJECT;
		if (me && tier) break;   // OPTIMIZE
	}
	autoSound thee = Sound_FormantTier_filter_noscale (me, tier);
	praat_new (thee.move(), my name, U"_filt");
END2 }

/***** HARMONICITY *****/

FORM (Harmonicity_draw, U"Draw harmonicity", 0) {
	praat_dia_timeRange (dia);
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0 (= auto)")
	OK2
DO
	LOOP {
		iam (Harmonicity);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 0.0, 0.0,
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

FORM (Harmonicity_formula, U"Harmonicity Formula", U"Harmonicity: Formula...") {
	LABEL (U"label", U"x is time")
	LABEL (U"label", U"for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"self")
	OK2
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
END2 }

FORM (Harmonicity_getMaximum, U"Harmonicity: Get maximum", U"Harmonicity: Get maximum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getMaximum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, U"dB");
	}
END2 }

FORM (Harmonicity_getMean, U"Harmonicity: Get mean", U"Harmonicity: Get mean...") {
	praat_dia_timeRange (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double mean = Harmonicity_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (mean, U"dB");
	}
END2 }

FORM (Harmonicity_getMinimum, U"Harmonicity: Get minimum", U"Harmonicity: Get minimum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getMinimum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, U"dB");
	}
END2 }

FORM (Harmonicity_getStandardDeviation, U"Harmonicity: Get standard deviation", U"Harmonicity: Get standard deviation...") {
	praat_dia_timeRange (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double stdev = Harmonicity_getStandardDeviation (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (stdev, U"dB");
	}
END2 }

FORM (Harmonicity_getTimeOfMaximum, U"Harmonicity: Get time of maximum", U"Harmonicity: Get time of maximum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double maximum = Vector_getXOfMaximum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, U"seconds");
	}
END2 }

FORM (Harmonicity_getTimeOfMinimum, U"Harmonicity: Get time of minimum", U"Harmonicity: Get time of minimum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double minimum = Vector_getXOfMinimum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, U"seconds");
	}
END2 }

FORM (Harmonicity_getValueAtTime, U"Harmonicity: Get value", U"Harmonicity: Get value at time...") {
	dia_Vector_getValue (dia);
	OK2
DO
	LOOP {
		iam (Harmonicity);
		double value = Vector_getValueAtX (me, GET_REAL (U"Time"), 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (value, U"dB");
	}
END2 }
	
FORM (Harmonicity_getValueInFrame, U"Get value in frame", U"Harmonicity: Get value in frame...") {
	INTEGER (U"Frame number", U"10")
	OK2
DO
	LOOP {
		iam (Harmonicity);
		long frameNumber = GET_INTEGER (U"Frame number");
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], U"dB");
	}
END2 }

DIRECT2 (Harmonicity_help) {
	Melder_help (U"Harmonicity");
END2 }

DIRECT2 (Harmonicity_to_Matrix) {
	LOOP {
		iam (Harmonicity);
		autoMatrix thee = Harmonicity_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** INTENSITY *****/

FORM (Intensity_draw, U"Draw Intensity", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"Minimum (dB)", U"0.0")
	REAL (U"Maximum (dB)", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (Intensity);
		autoPraatPicture picture;
		Intensity_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT2 (Intensity_downto_IntensityTier) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_downto_IntensityTier (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Intensity_downto_Matrix) {
	LOOP {
		iam (Intensity);
		autoMatrix thee = Intensity_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Intensity_formula, U"Intensity Formula", 0) {
	LABEL (U"label", U"`x' is the time in seconds, `col' is the frame number, `self' is in dB")
	LABEL (U"label", U"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK2
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
END2 }

FORM (Intensity_getMaximum, U"Intensity: Get maximum", U"Intensity: Get maximum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double maximum = Vector_getMaximum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, U"dB");
		break;   // OPTIMIZE
	}
END2 }

FORM (old_Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_dia_timeRange (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 0, 0, true);
		Melder_informationReal (mean, U"dB");
		break;   // OPTIMIZE
	}
END2 }

FORM (Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_dia_timeRange (dia);
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK2
DO_ALTERNATIVE (old_Intensity_getMean)
	LOOP {
		iam (Intensity);
		double mean = Sampled_getMean_standardUnit (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			0, GET_INTEGER (U"Averaging method"), true);
		Melder_informationReal (mean, U"dB");
		break;   // OPTIMIZE
	}
END2 }

FORM (Intensity_getMinimum, U"Intensity: Get minimum", U"Intensity: Get minimum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double minimum = Vector_getMinimum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, U"dB");
		break;   // OPTIMIZE
	}
END2 }

FORM (Intensity_getQuantile, U"Intensity: Get quantile", 0) {
	praat_dia_timeRange (dia);
	REAL (U"Quantile (0-1)", U"0.50")
	OK2
DO
	LOOP {
		iam (Intensity);
		double quantile = Intensity_getQuantile (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Quantile"));
		Melder_informationReal (quantile, U"dB");
	}
END2 }

FORM (Intensity_getStandardDeviation, U"Intensity: Get standard deviation", U"Intensity: Get standard deviation...") {
	praat_dia_timeRange (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double stdev = Vector_getStandardDeviation (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), 1);
		Melder_informationReal (stdev, U"dB");
	}
END2 }

FORM (Intensity_getTimeOfMaximum, U"Intensity: Get time of maximum", U"Intensity: Get time of maximum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMaximum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
	}
END2 }

FORM (Intensity_getTimeOfMinimum, U"Intensity: Get time of minimum", U"Intensity: Get time of minimum...") {
	dia_Vector_getExtremum (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double time = Vector_getXOfMinimum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
	}
END2 }

FORM (Intensity_getValueAtTime, U"Intensity: Get value", U"Intensity: Get value at time...") {
	dia_Vector_getValue (dia);
	OK2
DO
	LOOP {
		iam (Intensity);
		double value = Vector_getValueAtX (me, GET_REAL (U"Time"), 1, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (value, U"dB");
	}
END2 }
	
FORM (Intensity_getValueInFrame, U"Get value in frame", U"Intensity: Get value in frame...") {
	INTEGER (U"Frame number", U"10")
	OK2
DO
	LOOP {
		iam (Intensity);
		long frameNumber = GET_INTEGER (U"Frame number");
		Melder_informationReal (frameNumber < 1 || frameNumber > my nx ? NUMundefined : my z [1] [frameNumber], U"dB");
	}
END2 }

DIRECT2 (Intensity_help) {
	Melder_help (U"Intensity");
END2 }

DIRECT2 (Intensity_to_IntensityTier_peaks) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Intensity_to_IntensityTier_valleys) {
	LOOP {
		iam (Intensity);
		autoIntensityTier thee = Intensity_to_IntensityTier_valleys (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** INTENSITY & PITCH *****/

FORM (Pitch_Intensity_draw, U"Plot intensity by pitch", nullptr) {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= auto)")
	REAL (U"From intensity (dB)", U"0.0")
	REAL (U"To intensity (dB)", U"100.0")
	BOOLEAN (U"Garnish", true)
	RADIO (U"Drawing method", 1)
		RADIOBUTTON (U"Speckles")
		RADIOBUTTON (U"Curve")
		RADIOBUTTON (U"Speckles and curve")
	OK2
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
END2 }

FORM (Pitch_Intensity_speckle, U"Plot intensity by pitch", nullptr) {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= auto)")
	REAL (U"From intensity (dB)", U"0.0")
	REAL (U"To intensity (dB)", U"100.0")
	BOOLEAN (U"Garnish", true)
	OK2
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
END2 }

/***** INTENSITY & POINTPROCESS *****/

DIRECT2 (Intensity_PointProcess_to_IntensityTier) {
	Intensity intensity = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntensity) intensity = (Intensity) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = Intensity_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.move(), intensity -> name);
END2 }

/***** INTENSITYTIER *****/

FORM (IntensityTier_addPoint, U"Add one point", U"IntensityTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Intensity (dB)", U"75")
	OK2
DO
	LOOP {
		iam (IntensityTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Intensity"));
		praat_dataChanged (me);
	}
END2 }

FORM (IntensityTier_create, U"Create empty IntensityTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoIntensityTier thee = IntensityTier_create (startTime, endTime);
	praat_new (thee.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (IntensityTier_downto_PointProcess) {
	LOOP {
		iam (IntensityTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (IntensityTier_downto_TableOfReal) {
	LOOP {
		iam (IntensityTier);
		autoTableOfReal thee = IntensityTier_downto_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (IntensityTier_edit) {
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
END2 }

FORM (IntensityTier_formula, U"IntensityTier: Formula", U"IntensityTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in dB")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self + 3.0")
	OK2
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
END2 }

FORM (IntensityTier_getValueAtTime, U"Get IntensityTier value", U"IntensityTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtTime (me, GET_REAL (U"Time"));
		Melder_informationReal (value, U"dB");
	}
END2 }
	
FORM (IntensityTier_getValueAtIndex, U"Get IntensityTier value", U"IntensityTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK2
DO
	LOOP {
		iam (IntensityTier);
		double value = RealTier_getValueAtIndex (me, GET_INTEGER (U"Point number"));
		Melder_informationReal (value, U"dB");
	}
END2 }

DIRECT2 (IntensityTier_help) {
	Melder_help (U"IntensityTier");
END2 }

DIRECT2 (IntensityTier_to_AmplitudeTier) {
	LOOP {
		iam (IntensityTier);
		autoAmplitudeTier thee = IntensityTier_to_AmplitudeTier (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (info_IntensityTier_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your IntensityTier editor:\n"
		"   select an IntensityTier and a Sound, and click \"View & Edit\".");
END2 }

/***** INTENSITYTIER & POINTPROCESS *****/

DIRECT2 (IntensityTier_PointProcess_to_IntensityTier) {
	IntensityTier intensity = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
		if (intensity && point) break;   // OPTIMIZE
	}
	autoIntensityTier thee = IntensityTier_PointProcess_to_IntensityTier (intensity, point);
	praat_new (thee.move(), intensity -> name);
END2 }

/***** INTENSITYTIER & SOUND *****/

DIRECT2 (Sound_IntensityTier_multiply_old) {
	Sound sound = nullptr;
	IntensityTier intensity = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;
		if (CLASS == classIntensityTier) intensity = (IntensityTier) OBJECT;
		if (sound && intensity) break;   // OPTIMIZE
	}
	autoSound thee = Sound_IntensityTier_multiply (sound, intensity, true);
	praat_new (thee.move(), sound -> name, U"_int");
END2 }

FORM (Sound_IntensityTier_multiply, U"Sound & IntervalTier: Multiply", nullptr) {
	BOOLEAN (U"Scale to 0.9", true)
	OK2
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
END2 }

/***** INTERVALTIER, rest in praat_TextGrid_init.cpp *****/

FORM_READ2 (IntervalTier_readFromXwaves, U"Read IntervalTier from Xwaves", 0, true) {
	autoIntervalTier me = IntervalTier_readFromXwaves (file);
	praat_newWithFile (me.move(), file, MelderFile_name (file));
END2 }

/***** LTAS *****/

DIRECT2 (Ltases_average) {
	autoCollection ltases = praat_getSelectedObjects ();
	autoLtas thee = Ltases_average (ltases.peek());
	praat_new (thee.move(), U"averaged");
END2 }

FORM (Ltas_computeTrendLine, U"Ltas: Compute trend line", U"Ltas: Compute trend line...") {
	REAL (U"left Frequency range (Hz)", U"600.0")
	POSITIVE (U"right Frequency range (Hz)", U"4000.0")
	OK2
DO
	LOOP {
		iam (Ltas);
		autoLtas thee = Ltas_computeTrendLine (me, GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"));
		praat_new (thee.move(), my name, U"_trend");
	}
END2 }

FORM (old_Ltas_draw, U"Ltas: Draw", nullptr) {
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (U"left Power range (dB/Hz)", U"-20.0")
	REAL (U"right Power range (dB/Hz)", U"80.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (Ltas);
		autoPraatPicture picture;
		Ltas_draw (me, GRAPHICS, GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"), GET_INTEGER (U"Garnish"), U"Bars");
	}
END2 }

FORM (Ltas_draw, U"Ltas: Draw", nullptr) {
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (U"left Power range (dB/Hz)", U"-20.0")
	REAL (U"right Power range (dB/Hz)", U"80.0")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 2)
		OPTION (U"Curve")
		OPTION (U"Bars")
		OPTION (U"Poles")
		OPTION (U"Speckles")
	OK2
DO_ALTERNATIVE (old_Ltas_draw)
	LOOP {
		iam (Ltas);
		autoPraatPicture picture;
		Ltas_draw (me, GRAPHICS, GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"), GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END2 }

FORM (Ltas_formula, U"Ltas Formula", nullptr) {
	LABEL (U"label", U"`x' is the frequency in hertz, `col' is the bin number")
	LABEL (U"label", U"x := x1;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK2
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
END2 }

FORM (Ltas_getBinNumberFromFrequency, U"Ltas: Get band from frequency", U"Ltas: Get band from frequency...") {
	REAL (U"Frequency (Hz)", U"2000.0")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double binNumber = Sampled_xToIndex (me, GET_REAL (U"Frequency"));
	Melder_informationReal (binNumber, nullptr);
END2 }

DIRECT2 (Ltas_getBinWidth) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my dx, U"hertz");
END2 }

FORM (Ltas_getFrequencyFromBinNumber, U"Ltas: Get frequency from bin number", U"Ltas: Get frequency from bin number...") {
	NATURAL (U"Bin number", U"1")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double frequency = Sampled_indexToX (me, GET_INTEGER (U"Bin number"));
	Melder_informationReal (frequency, U"hertz");
END2 }

FORM (Ltas_getFrequencyOfMaximum, U"Ltas: Get frequency of maximum", U"Ltas: Get frequency of maximum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMaximum (me,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (frequency, U"hertz");
END2 }

FORM (Ltas_getFrequencyOfMinimum, U"Ltas: Get frequency of minimum", U"Ltas: Get frequency of minimum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double frequency = Vector_getXOfMinimum (me,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (frequency, U"hertz");
END2 }

DIRECT2 (Ltas_getHighestFrequency) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmax, U"Hz");
END2 }

FORM (Ltas_getLocalPeakHeight, U"Ltas: Get local peak height", nullptr) {
	REAL (U"left Environment (Hz)", U"1700.0")
	REAL (U"right Environment (Hz)", U"4200.0")
	REAL (U"left Peak (Hz)", U"2400.0")
	REAL (U"right Peak (Hz)", U"3200.0")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double environmentMin = GET_REAL (U"left Environment"), environmentMax = GET_REAL (U"right Environment");
	double peakMin = GET_REAL (U"left Peak"), peakMax = GET_REAL (U"right Peak");
	if (environmentMin >= peakMin) Melder_throw (U"The beginning of the environment must lie before the peak.");
	if (peakMin >= peakMax) Melder_throw (U"The end of the peak must lie after its beginning.");
	if (environmentMax <= peakMax) Melder_throw (U"The end of the environment must lie after the peak.");
	double localPeakHeight = Ltas_getLocalPeakHeight (me, environmentMin, environmentMax,
		peakMin, peakMax, GET_INTEGER (U"Averaging method"));
	Melder_informationReal (localPeakHeight, U"dB");
END2 }

DIRECT2 (Ltas_getLowestFrequency) {
	Ltas me = FIRST (Ltas);
	Melder_informationReal (my xmin, U"hertz");
END2 }

FORM (Ltas_getMaximum, U"Ltas: Get maximum", U"Ltas: Get maximum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double maximum = Vector_getMaximum (me,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (maximum, U"dB");
END2 }

FORM (Ltas_getMean, U"Ltas: Get mean", U"Ltas: Get mean...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double mean = Sampled_getMean_standardUnit (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		0, GET_INTEGER (U"Averaging method"), false);
	Melder_informationReal (mean, U"dB");
END2 }

FORM (Ltas_getMinimum, U"Ltas: Get minimum", U"Ltas: Get minimum...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double minimum = Vector_getMinimum (me,
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (minimum, U"dB");
END2 }

DIRECT2 (Ltas_getNumberOfBins) {
	Ltas me = FIRST (Ltas);
	long numberOfBins = my nx;
	Melder_information (numberOfBins, U" bins");
END2 }

FORM (Ltas_getSlope, U"Ltas: Get slope", 0) {
	REAL (U"left Low band (Hz)", U"0.0")
	REAL (U"right Low band (Hz)", U"1000.0")
	REAL (U"left High band (Hz)", U"1000.0")
	REAL (U"right High band (Hz)", U"4000.0")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double slope = Ltas_getSlope (me, GET_REAL (U"left Low band"), GET_REAL (U"right Low band"),
		GET_REAL (U"left High band"), GET_REAL (U"right High band"), GET_INTEGER (U"Averaging method"));
	Melder_informationReal (slope, U"dB");
END2 }

FORM (Ltas_getStandardDeviation, U"Ltas: Get standard deviation", U"Ltas: Get standard deviation...") {
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"0.0 (= all)")
	RADIO (U"Averaging method", 1)
		RADIOBUTTON (U"energy")
		RADIOBUTTON (U"sones")
		RADIOBUTTON (U"dB")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double stdev = Sampled_getStandardDeviation_standardUnit (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		0, GET_INTEGER (U"Averaging method"), false);
	Melder_informationReal (stdev, U"dB");
END2 }

FORM (Ltas_getValueAtFrequency, U"Ltas: Get value", U"Ltas: Get value at frequency...") {
	REAL (U"Frequency (Hz)", U"1500")
	RADIO (U"Interpolation", 1)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	Ltas me = FIRST (Ltas);
	double value = Vector_getValueAtX (me, GET_REAL (U"Frequency"), 1, GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (value, U"dB");
END2 }
	
FORM (Ltas_getValueInBin, U"Get value in bin", U"Ltas: Get value in bin...") {
	INTEGER (U"Bin number", U"100")
	OK2
DO
	Ltas me = FIRST (Ltas);
	long binNumber = GET_INTEGER (U"Bin number");
	double value = binNumber < 1 || binNumber > my nx ? NUMundefined : my z [1] [binNumber];
	Melder_informationReal (value, U"dB");
END2 }

DIRECT2 (Ltas_help) {
	Melder_help (U"Ltas");
END2 }

DIRECT2 (Ltases_merge) {
	autoCollection ltases = praat_getSelectedObjects ();
	autoLtas thee = Ltases_merge (ltases.peek());
	praat_new (thee.move(), U"merged");
END2 }

FORM (Ltas_subtractTrendLine, U"Ltas: Subtract trend line", U"Ltas: Subtract trend line...") {
	REAL (U"left Frequency range (Hz)", U"600.0")
	POSITIVE (U"right Frequency range (Hz)", U"4000.0")
	OK2
DO
	LOOP {
		iam (Ltas);
		autoLtas thee = Ltas_subtractTrendLine (me, GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"));
		praat_new (thee.move(), my name, U"_fit");
	}
END2 }

DIRECT2 (Ltas_to_Matrix) {
	LOOP {
		iam (Ltas);
		autoMatrix thee = Ltas_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Ltas_to_SpectrumTier_peaks) {
	LOOP {
		iam (Ltas);
		autoSpectrumTier thee = Ltas_to_SpectrumTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** MANIPULATION *****/

static void cb_ManipulationEditor_publication (Editor /* editor */, autoDaata publication) {
	try {
		praat_new (publication.move(), U"fromManipulationEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT2 (Manipulation_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Manipulation from batch.");
	LOOP {
		iam (Manipulation);
		autoManipulationEditor editor = ManipulationEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.peek(), cb_ManipulationEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

DIRECT2 (Manipulation_extractDurationTier) {
	LOOP {
		iam (Manipulation);
		if (! my duration) Melder_throw (me, U": I don't contain a DurationTier.");
		autoDurationTier thee = Data_copy (my duration.get());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_extractOriginalSound) {
	LOOP {
		iam (Manipulation);
		if (! my sound) Melder_throw (me, U": I don't contain a Sound.");
		autoSound thee = Data_copy (my sound.get());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_extractPitchTier) {
	LOOP {
		iam (Manipulation);
		if (! my pitch) Melder_throw (me, U": I don't contain a PitchTier.");
		autoPitchTier thee = Data_copy (my pitch.get());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_extractPulses) {
	LOOP {
		iam (Manipulation);
		if (! my pulses) Melder_throw (me, U": I don't contain a PointProcess.");
		autoPointProcess thee = Data_copy (my pulses.get());
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_getResynthesis_lpc) {
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_PITCH_LPC);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_getResynthesis_overlapAdd) {
	LOOP {
		iam (Manipulation);
		autoSound thee = Manipulation_to_Sound (me, Manipulation_OVERLAPADD);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Manipulation_help) {
	Melder_help (U"Manipulation");
END2 }

DIRECT2 (Manipulation_play_lpc) {
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_PITCH_LPC);
	}
END2 }

DIRECT2 (Manipulation_play_overlapAdd) {
	LOOP {
		iam (Manipulation);
		Manipulation_play (me, Manipulation_OVERLAPADD);
	}
END2 }

DIRECT2 (Manipulation_removeDuration) {
	LOOP {
		iam (Manipulation);
		my duration = autoDurationTier();
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Manipulation_removeOriginalSound) {
	LOOP {
		iam (Manipulation);
		my sound = autoSound();
		praat_dataChanged (me);
	}
END2 }

FORM_WRITE (Manipulation_writeToBinaryFileWithoutSound, U"Binary file without Sound", nullptr, nullptr)
	Manipulation_writeToBinaryFileWithoutSound (FIRST_ANY (Manipulation), file);
END

FORM_WRITE (Manipulation_writeToTextFileWithoutSound, U"Text file without Sound", nullptr, nullptr)
	Manipulation_writeToTextFileWithoutSound (FIRST_ANY (Manipulation), file);
END

DIRECT2 (info_DurationTier_Manipulation_replace) {
	Melder_information (U"To replace the DurationTier in a Manipulation object,\n"
		"select a DurationTier object and a Manipulation object\nand choose \"Replace duration\".");
END2 }

DIRECT2 (info_PitchTier_Manipulation_replace) {
	Melder_information (U"To replace the PitchTier in a Manipulation object,\n"
		"select a PitchTier object and a Manipulation object\nand choose \"Replace pitch\".");
END2 }

/***** MANIPULATION & DURATIONTIER *****/

DIRECT2 (Manipulation_replaceDurationTier) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceDurationTier (me, FIRST (DurationTier));
	praat_dataChanged (me);
END2 }

DIRECT2 (Manipulation_replaceDurationTier_help) {
	Melder_help (U"Manipulation: Replace duration tier");
END2 }

/***** MANIPULATION & PITCHTIER *****/

DIRECT2 (Manipulation_replacePitchTier) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePitchTier (me, FIRST (PitchTier));
	praat_dataChanged (me);
END2 }

DIRECT2 (Manipulation_replacePitchTier_help) {
	Melder_help (U"Manipulation: Replace pitch tier");
END2 }

/***** MANIPULATION & POINTPROCESS *****/

DIRECT2 (Manipulation_replacePulses) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replacePulses (me, FIRST (PointProcess));
	praat_dataChanged (me);
END2 }

/***** MANIPULATION & SOUND *****/

DIRECT2 (Manipulation_replaceOriginalSound) {
	Manipulation me = FIRST (Manipulation);
	Manipulation_replaceOriginalSound (me, FIRST (Sound));
	praat_dataChanged (me);
END2 }

/***** MANIPULATION & TEXTTIER *****/

DIRECT2 (Manipulation_TextTier_to_Manipulation) {
	Manipulation me = FIRST (Manipulation);
	TextTier thee = FIRST (TextTier);
	autoManipulation him = Manipulation_AnyTier_to_Manipulation (me, reinterpret_cast <AnyTier> (thee));
	praat_new (him.move(), my name);
END2 }

/***** MATRIX *****/

DIRECT2 (Matrix_appendRows) {
	Matrix m1 = nullptr, m2 = nullptr;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoMatrix thee = Matrix_appendRows (m1, m2, classMatrix);
	praat_new (thee.move(), m1 -> name, U"_", m2 -> name);
END2 }

FORM (Matrix_create, U"Create Matrix", U"Create Matrix...") {
	WORD (U"Name", U"xy")
	REAL (U"xmin", U"1.0")
	REAL (U"xmax", U"1.0")
	NATURAL (U"Number of columns", U"1")
	POSITIVE (U"dx", U"1.0")
	REAL (U"x1", U"1.0")
	REAL (U"ymin", U"1.0")
	REAL (U"ymax", U"1.0")
	NATURAL (U"Number of rows", U"1")
	POSITIVE (U"dy", U"1.0")
	REAL (U"y1", U"1.0")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"x*y")
	OK2
DO
	double xmin = GET_REAL (U"xmin"), xmax = GET_REAL (U"xmax");
	double ymin = GET_REAL (U"ymin"), ymax = GET_REAL (U"ymax");
	if (xmax < xmin) Melder_throw (U"xmax (", Melder_single (xmax), U") should not be less than xmin (", Melder_single (xmin), U").");
	if (ymax < ymin) Melder_throw (U"ymax (", Melder_single (ymax), U") should not be less than ymin (", Melder_single (ymin), U").");
	autoMatrix me = Matrix_create (
		xmin, xmax, GET_INTEGER (U"Number of columns"), GET_REAL (U"dx"), GET_REAL (U"x1"),
		ymin, ymax, GET_INTEGER (U"Number of rows"), GET_REAL (U"dy"), GET_REAL (U"y1"));
	Matrix_formula (me.peek(), GET_STRING (U"formula"), interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Matrix_createSimple, U"Create simple Matrix", U"Create simple Matrix...") {
	WORD (U"Name", U"xy")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"10")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"x*y")
	OK2
DO
	autoMatrix me = Matrix_createSimple (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	Matrix_formula (me.peek(), GET_STRING (U"formula"), interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Matrix_drawOneContour, U"Draw one altitude contour", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Height", U"0.5")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawOneContour (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Height"));
	}
END2 }

FORM (Matrix_drawContours, U"Draw altitude contours", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawContours (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

FORM (Matrix_drawRows, U"Draw rows", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="),
			GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

DIRECT2 (Matrix_eigen) {
	LOOP {
		iam (Matrix);
		autoMatrix vectors, values;
		Matrix_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	}
END2 }

FORM (Matrix_formula, U"Matrix Formula", U"Formula...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
DO
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Matrix may have partially changed
			throw;
		}
	}
END2 }

DIRECT2 (Matrix_getHighestX) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmax, nullptr);
END2 }

DIRECT2 (Matrix_getHighestY) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymax, nullptr);
END2 }

DIRECT2 (Matrix_getLowestX) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmin, nullptr);
END2 }

DIRECT2 (Matrix_getLowestY) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymin, nullptr);
END2 }

DIRECT2 (Matrix_getNumberOfColumns) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (my nx);
END2 }

DIRECT2 (Matrix_getNumberOfRows) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (my ny);
END2 }

DIRECT2 (Matrix_getColumnDistance) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dx, nullptr);
END2 }

DIRECT2 (Matrix_getRowDistance) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dy, nullptr);
END2 }

DIRECT2 (Matrix_getSum) {
	Matrix me = FIRST_ANY (Matrix);
	double sum = Matrix_getSum (me);
	Melder_informationReal (sum, nullptr);
END2 }

DIRECT2 (Matrix_getMaximum) {
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (maximum, nullptr);
END2 }

DIRECT2 (Matrix_getMinimum) {
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (minimum, nullptr);
END2 }

FORM (Matrix_getValueAtXY, U"Matrix: Get value at xy", nullptr) {
	REAL (U"X", U"0")
	REAL (U"Y", U"0")
	OK2
DO
	Matrix me = FIRST_ANY (Matrix);
	double x = GET_REAL (U"X"), y = GET_REAL (U"Y");
	double value = Matrix_getValueAtXY (me, x, y);
	Melder_information (value, U" (at x = ", x, U" and y = ", y, U")");
END2 }

FORM (Matrix_getValueInCell, U"Matrix: Get value in cell", nullptr) {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	OK2
DO
	Matrix me = FIRST_ANY (Matrix);
	long row = GET_INTEGER (U"Row number"), column = GET_INTEGER (U"Column number");
	if (row > my ny) Melder_throw (U"Row number must not exceed number of rows.");
	if (column > my nx) Melder_throw (U"Column number must not exceed number of columns.");
	Melder_informationReal (my z [row] [column], nullptr);
END2 }

FORM (Matrix_getXofColumn, U"Matrix: Get x of column", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	Matrix me = FIRST_ANY (Matrix);
	double x = Matrix_columnToX (me, GET_INTEGER (U"Column number"));
	Melder_informationReal (x, nullptr);
END2 }

FORM (Matrix_getYofRow, U"Matrix: Get y of row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	Matrix me = FIRST_ANY (Matrix);
	double y = Matrix_rowToY (me, GET_INTEGER (U"Row number"));
	Melder_informationReal (y, nullptr);
END2 }

DIRECT2 (Matrix_help) {
	Melder_help (U"Matrix");
END2 }

DIRECT2 (Matrix_movie) {
	Graphics g = Movie_create (U"Matrix movie", 300, 300);
	LOOP {
		iam (Matrix);
		Matrix_movie (me, g);
	}
END2 }

FORM (Matrix_paintCells, U"Matrix: Paint cells with greys", U"Matrix: Paint cells...") {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintCells (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

FORM (Matrix_paintContours, U"Matrix: Paint altitude contours with greys", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintContours (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

FORM (Matrix_paintImage, U"Matrix: Paint grey image", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintImage (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END2 }

FORM (Matrix_paintSurface, U"Matrix: Paint 3-D surface plot", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintSurface (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), 30, 45);
	}
END2 }

FORM (Matrix_power, U"Matrix: Power...", nullptr) {
	NATURAL (U"Power", U"2")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoMatrix thee = Matrix_power (me, GET_INTEGER (U"Power"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM_READ2 (Matrix_readFromRawTextFile, U"Read Matrix from raw text file", nullptr, true) {
	autoMatrix me = Matrix_readFromRawTextFile (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM_READ2 (Matrix_readAP, U"Read Matrix from LVS AP file", nullptr, true) {
	autoMatrix me = Matrix_readAP (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM (Matrix_setValue, U"Matrix: Set value", U"Matrix: Set value...") {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	REAL (U"New value", U"0.0")
	OK2
DO
	LOOP {
		iam (Matrix);
		long row = GET_INTEGER (U"Row number"), column = GET_INTEGER (U"Column number");
		if (row > my ny) Melder_throw (U"Row number must not be greater than number of rows.");
		if (column > my nx) Melder_throw (U"Column number must not be greater than number of columns.");
		my z [row] [column] = GET_REAL (U"New value");
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Matrix_to_Cochleagram) {
	LOOP {
		iam (Matrix);
		autoCochleagram thee = Matrix_to_Cochleagram (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Excitation) {
	LOOP {
		iam (Matrix);
		autoExcitation thee = Matrix_to_Excitation (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Harmonicity) {
	LOOP {
		iam (Matrix);
		autoHarmonicity thee = Matrix_to_Harmonicity (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Intensity) {
	LOOP {
		iam (Matrix);
		autoIntensity thee = Matrix_to_Intensity (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Pitch) {
	LOOP {
		iam (Matrix);
		autoPitch thee = Matrix_to_Pitch (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Spectrogram) {
	LOOP {
		iam (Matrix);
		autoSpectrogram thee = Matrix_to_Spectrogram (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Spectrum) {
	LOOP {
		iam (Matrix);
		autoSpectrum thee = Matrix_to_Spectrum (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Ltas) {
	LOOP {
		iam (Matrix);
		autoLtas thee = Matrix_to_Ltas (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_ParamCurve) {
	Matrix m1 = nullptr, m2 = nullptr;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoSound sound1 = Matrix_to_Sound (m1), sound2 = Matrix_to_Sound (m2);
	autoParamCurve thee = ParamCurve_create (sound1.peek(), sound2.peek());
	praat_new (thee.move(), m1 -> name, U"_", m2 -> name);
END2 }

DIRECT2 (Matrix_to_PointProcess) {
	LOOP {
		iam (Matrix);
		autoPointProcess thee = Matrix_to_PointProcess (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Polygon) {
	LOOP {
		iam (Matrix);
		autoPolygon thee = Matrix_to_Polygon (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Sound) {
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Matrix_to_Sound_mono, U"Matrix: To Sound (mono)", 0) {
	INTEGER (U"Row", U"1")
	LABEL (U"", U"(negative values count from last row)")
	OK2
DO
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound_mono (me, GET_INTEGER (U"Row"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_TableOfReal) {
	LOOP {
		iam (Matrix);
		autoTableOfReal thee = Matrix_to_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_Transition) {
	LOOP {
		iam (Matrix);
		autoTransition thee = Matrix_to_Transition (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Matrix_to_VocalTract) {
	LOOP {
		iam (Matrix);
		autoVocalTract thee = Matrix_to_VocalTract (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM_WRITE (Matrix_writeToMatrixTextFile, U"Save Matrix as matrix text file", nullptr, U"mat")
	Matrix me = FIRST (Matrix);
	Matrix_writeToMatrixTextFile (me, file);
END

FORM_WRITE (Matrix_writeToHeaderlessSpreadsheetFile, U"Save Matrix as spreadsheet", nullptr, U"txt")
	Matrix me = FIRST (Matrix);
	Matrix_writeToHeaderlessSpreadsheetFile (me, file);
END

/***** MOVIE *****/

FORM_READ2 (Movie_openFromSoundFile, U"Open movie file", nullptr, true) {
	autoMovie me = Movie_openFromSoundFile (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM (Movie_paintOneImage, U"Movie: Paint one image", nullptr) {
	NATURAL (U"Frame number", U"1")
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"1.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"1.0")
	OK2
DO
	LOOP {
		iam (Movie);
		autoPraatPicture picture;
		Movie_paintOneImage (me, GRAPHICS, GET_INTEGER (U"Frame number"),
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="));
	}
END2 }

DIRECT2 (Movie_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Movie from batch.");
	LOOP {
		iam (Movie);
		autoMovieWindow editor = MovieWindow_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

/***** PARAMCURVE *****/

FORM (ParamCurve_draw, U"Draw parametrized curve", nullptr) {
	REAL (U"Tmin", U"0.0")
	REAL (U"Tmax", U"0.0")
	REAL (U"Step", U"0.0")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (ParamCurve);
		autoPraatPicture picture;
		ParamCurve_draw (me, GRAPHICS,
			GET_REAL (U"Tmin"), GET_REAL (U"Tmax"), GET_REAL (U"Step"),
			GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT (ParamCurve_help) Melder_help (U"ParamCurve"); END

/***** PHOTO *****/

FORM (Photo_create, U"Create Photo", U"Create Photo...") {
	WORD (U"Name", U"xy")
	REAL (U"xmin", U"1.0")
	REAL (U"xmax", U"1.0")
	NATURAL (U"Number of columns", U"1")
	POSITIVE (U"dx", U"1.0")
	REAL (U"x1", U"1.0")
	REAL (U"ymin", U"1.0")
	REAL (U"ymax", U"1.0")
	NATURAL (U"Number of rows", U"1")
	POSITIVE (U"dy", U"1.0")
	REAL (U"y1", U"1.0")
	LABEL (U"", U"Red formula:")
	TEXTFIELD (U"redFormula", U"x*y/100")
	LABEL (U"", U"Green formula:")
	TEXTFIELD (U"greenFormula", U"x*y/1000")
	LABEL (U"", U"Blue formula:")
	TEXTFIELD (U"blueFormula", U"x*y/100")
	OK2
DO
	double xmin = GET_REAL (U"xmin"), xmax = GET_REAL (U"xmax");
	double ymin = GET_REAL (U"ymin"), ymax = GET_REAL (U"ymax");
	if (xmax < xmin) Melder_throw (U"xmax (", Melder_single (xmax), U") should not be less than xmin (", Melder_single (xmin), U").");
	if (ymax < ymin) Melder_throw (U"ymax (", Melder_single (ymax), U") should not be less than ymin (", Melder_single (ymin), U").");
	autoPhoto me = Photo_create (
		xmin, xmax, GET_INTEGER (U"Number of columns"), GET_REAL (U"dx"), GET_REAL (U"x1"),
		ymin, ymax, GET_INTEGER (U"Number of rows"), GET_REAL (U"dy"), GET_REAL (U"y1"));
	Matrix_formula (my d_red  .get(), GET_STRING (U"redFormula"),   interpreter, nullptr);
	Matrix_formula (my d_green.get(), GET_STRING (U"greenFormula"), interpreter, nullptr);
	Matrix_formula (my d_blue .get(), GET_STRING (U"blueFormula"),  interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Photo_createSimple, U"Create simple Photo", U"Create simple Photo...") {
	WORD (U"Name", U"xy")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"10")
	LABEL (U"", U"Red formula:")
	TEXTFIELD (U"redFormula", U"x*y/100")
	LABEL (U"", U"Green formula:")
	TEXTFIELD (U"greenFormula", U"x*y/1000")
	LABEL (U"", U"Blue formula:")
	TEXTFIELD (U"blueFormula", U"x*y/100")
	OK2
DO
	autoPhoto me = Photo_createSimple (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	Matrix_formula (my d_red.get(),   GET_STRING (U"redFormula"),   interpreter, nullptr);
	Matrix_formula (my d_green.get(), GET_STRING (U"greenFormula"), interpreter, nullptr);
	Matrix_formula (my d_blue.get(),  GET_STRING (U"blueFormula"),  interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (Photo_extractBlue) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_blue.get());
		praat_new (thee.move(), my name, U"_blue");
	}
END2 }

DIRECT2 (Photo_extractGreen) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_green.get());
		praat_new (thee.move(), my name, U"_green");
	}
END2 }

DIRECT2 (Photo_extractRed) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_red.get());
		praat_new (thee.move(), my name, U"_red");
	}
END2 }

DIRECT2 (Photo_extractTransparency) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_transparency.get());
		praat_new (thee.move(), my name, U"_transparency");
	}
END2 }

FORM (Photo_formula_red, U"Photo Formula (red)", U"Formula (red)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_red.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END2 }

FORM (Photo_formula_green, U"Photo Formula (green)", U"Formula (green)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_green.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END2 }

FORM (Photo_formula_blue, U"Photo Formula (blue)", U"Formula (blue)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_blue.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END2 }

FORM (Photo_formula_transparency, U"Photo Formula (transparency)", U"Formula (transparency)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK2
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_transparency.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END2 }

FORM (Photo_paintCells, U"Photo: Paint cells with colour", U"Photo: Paint cells...") {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	OK2
DO
	LOOP {
		iam (Photo);
		autoPraatPicture picture;
		Photo_paintCells (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="));
	}
END2 }

FORM (Photo_paintImage, U"Photo: Paint colour image", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	OK2
DO
	LOOP {
		iam (Photo);
		autoPraatPicture picture;
		Photo_paintImage (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="));
	}
END2 }

FORM_WRITE (Photo_saveAsAppleIconFile, U"Save as Apple icon file", nullptr, U"icns")
	LOOP {
		iam (Photo);
		Photo_saveAsAppleIconFile (me, file);
	}
END

FORM_WRITE (Photo_saveAsGIF, U"Save as GIF file", nullptr, U"gif")
	LOOP {
		iam (Photo);
		Photo_saveAsGIF (me, file);
	}
END

FORM_WRITE (Photo_saveAsJPEG, U"Save as JPEG file", nullptr, U"jpg")
	LOOP {
		iam (Photo);
		Photo_saveAsJPEG (me, file);
	}
END

FORM_WRITE (Photo_saveAsJPEG2000, U"Save as JPEG-2000 file", nullptr, U"jpg")
	LOOP {
		iam (Photo);
		Photo_saveAsJPEG2000 (me, file);
	}
END

FORM_WRITE (Photo_saveAsPNG, U"Save as PNG file", nullptr, U"png")
	LOOP {
		iam (Photo);
		Photo_saveAsPNG (me, file);
	}
END

FORM_WRITE (Photo_saveAsTIFF, U"Save as TIFF file", nullptr, U"tiff")
	LOOP {
		iam (Photo);
		Photo_saveAsTIFF (me, file);
	}
END

FORM_WRITE (Photo_saveAsWindowsBitmapFile, U"Save as Windows bitmap file", nullptr, U"bmp")
	LOOP {
		iam (Photo);
		Photo_saveAsWindowsBitmapFile (me, file);
	}
END

FORM_WRITE (Photo_saveAsWindowsIconFile, U"Save as Windows icon file", nullptr, U"ico")
	LOOP {
		iam (Photo);
		Photo_saveAsWindowsIconFile (me, file);
	}
END

/***** PHOTO & MATRIX *****/

DIRECT2 (Photo_Matrix_replaceBlue) {
	Photo me = FIRST (Photo);
	Matrix thee = FIRST (Matrix);
	Photo_replaceBlue (me, thee);
	praat_dataChanged (me);
END2 }

DIRECT2 (Photo_Matrix_replaceGreen) {
	Photo me = FIRST (Photo);
	Matrix thee = FIRST (Matrix);
	Photo_replaceGreen (me, thee);
	praat_dataChanged (me);
END2 }

DIRECT2 (Photo_Matrix_replaceRed) {
	Photo me = FIRST (Photo);
	Matrix thee = FIRST (Matrix);
	Photo_replaceRed (me, thee);
	praat_dataChanged (me);
END2 }

DIRECT2 (Photo_Matrix_replaceTransparency) {
	Photo me = FIRST (Photo);
	Matrix thee = FIRST (Matrix);
	Photo_replaceTransparency (me, thee);
	praat_dataChanged (me);
END2 }

/***** PITCH *****/

DIRECT2 (Pitch_getNumberOfVoicedFrames) {
	Pitch me = FIRST (Pitch);
	Melder_information (Pitch_countVoicedFrames (me), U" voiced frames");
END2 }

DIRECT2 (Pitch_difference) {
	Pitch pit1 = nullptr, pit2 = nullptr;
	LOOP (pit1 ? pit2 : pit1) = (Pitch) OBJECT;
	Pitch_difference (pit1, pit2);
END2 }

FORM (Pitch_draw, U"Pitch: Draw", U"Pitch: Draw...") {
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ);
	}
END2 }

FORM (Pitch_drawErb, U"Pitch: Draw erb", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_ERB);
	}
END2 }

FORM (Pitch_drawLogarithmic, U"Pitch: Draw logarithmic", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
	}
END2 }

FORM (Pitch_drawMel, U"Pitch: Draw mel", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	REAL (U"left Frequency range (mel)", U"0.0")
	REAL (U"right Frequency range (mel)", U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_MEL);
	}
END2 }

FORM (Pitch_drawSemitones100, U"Pitch: Draw semitones (re 100 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 100 Hz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
	}
END2 }

FORM (Pitch_drawSemitones200, U"Pitch: Draw semitones (re 200 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 200 Hz:")
	REAL (U"left Frequency range (st)", U"-24.0")
	REAL (U"right Frequency range (st)", U"18.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_200);
	}
END2 }

FORM (Pitch_drawSemitones440, U"Pitch: Draw semitones (re 440 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 440 Hz:")
	REAL (U"left Frequency range (st)", U"-36.0")
	REAL (U"right Frequency range (st)", U"6.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax,
			GET_INTEGER (U"Garnish"), Pitch_speckle_NO, kPitch_unit_SEMITONES_440);
	}
END2 }

DIRECT2 (Pitch_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Pitch from batch.");
	LOOP {
		iam (Pitch);
		autoPitchEditor editor = PitchEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (Pitch_formula, U"Pitch: Formula", U"Formula...") {
	LABEL (U"", U"x = time; col = frame; row = candidate (1 = current path); frequency (time, candidate) :=")
	TEXTFIELD (U"formula", U"self*2; Example: octave jump up")
	OK2
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
END2 }

FORM (Pitch_getMaximum, U"Pitch: Get maximum", nullptr) {
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Pitch_getMaximum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), unit, GET_INTEGER (U"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }

FORM (Pitch_getMean, U"Pitch: Get mean", nullptr) {
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Pitch_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }

FORM (Pitch_getMeanAbsoluteSlope, U"Pitch: Get mean absolute slope", 0) {
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Mel")
		RADIOBUTTON (U"Semitones")
		RADIOBUTTON (U"ERB")
	OK2
DO
	int unit = GET_INTEGER (U"Unit");
	Pitch me = FIRST (Pitch);
	double slope;
	long nVoiced = (unit == 1 ? Pitch_getMeanAbsSlope_hertz : unit == 2 ? Pitch_getMeanAbsSlope_mel : unit == 3 ? Pitch_getMeanAbsSlope_semitones : Pitch_getMeanAbsSlope_erb)
		(me, & slope);
	if (nVoiced < 2) {
		Melder_information (U"--undefined--");
	} else {
		Melder_information (slope, U" ", GET_STRING (U"Unit"), U"/s");
	}
END2 }

DIRECT2 (Pitch_getMeanAbsSlope_noOctave) {
	Pitch me = FIRST (Pitch);
	double slope;
	(void) Pitch_getMeanAbsSlope_noOctave (me, & slope);
	Melder_informationReal (slope, U"Semitones/s");
END2 }

FORM (Pitch_getMinimum, U"Pitch: Get minimum", 0) {
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getMinimum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		Pitch_LEVEL_FREQUENCY, unit, GET_INTEGER (U"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }

FORM (Pitch_getQuantile, U"Pitch: Get quantile", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"Quantile", U"0.50 (= median)")
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getQuantile (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Quantile"), Pitch_LEVEL_FREQUENCY, unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }

FORM (Pitch_getStandardDeviation, U"Pitch: Get standard deviation", nullptr) {
	praat_dia_timeRange (dia);
	OPTIONMENU (U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK2
DO
	int unit = GET_INTEGER (U"Unit");
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	Pitch me = FIRST (Pitch);
	double value = Pitch_getStandardDeviation (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), unit);
	const char32 *unitText =
		unit == kPitch_unit_HERTZ ? U"Hz" :
		unit == kPitch_unit_MEL ? U"mel" :
		unit == kPitch_unit_LOG_HERTZ ? U"logHz" :
		unit == kPitch_unit_SEMITONES_1 ? U"semitones" :
		U"ERB";
	Melder_informationReal (value, unitText);
END2 }

FORM (Pitch_getTimeOfMaximum, U"Pitch: Get time of maximum", nullptr) {
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	Pitch me = FIRST (Pitch);
	double time = Pitch_getTimeOfMaximum (me,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_ENUM (kPitch_unit, U"Unit"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (time, U"seconds");
END2 }

FORM (Pitch_getTimeOfMinimum, U"Pitch: Get time of minimum", nullptr) {
	praat_dia_timeRange (dia);
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
	OK2
DO
	Pitch me = FIRST (Pitch);
	double time = Pitch_getTimeOfMinimum (me,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_ENUM (kPitch_unit, U"Unit"), GET_INTEGER (U"Interpolation") - 1);
	Melder_informationReal (time, U"seconds");
END2 }

FORM (Pitch_getValueAtTime, U"Pitch: Get value at time", U"Pitch: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	RADIO (U"Interpolation", 2)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getValueAtX (me, GET_REAL (U"Time"), Pitch_LEVEL_FREQUENCY, unit, GET_INTEGER (U"Interpolation") - 1);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }
	
FORM (Pitch_getValueInFrame, U"Pitch: Get value in frame", U"Pitch: Get value in frame...") {
	INTEGER (U"Frame number", U"10")
	OPTIONMENU_ENUM (U"Unit", kPitch_unit, DEFAULT)
	OK2
DO
	enum kPitch_unit unit = GET_ENUM (kPitch_unit, U"Unit");
	Pitch me = FIRST (Pitch);
	double value = Sampled_getValueAtSample (me, GET_INTEGER (U"Frame number"), Pitch_LEVEL_FREQUENCY, unit);
	value = Function_convertToNonlogarithmic (me, value, Pitch_LEVEL_FREQUENCY, unit);
	Melder_informationReal (value, Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, 0));
END2 }

DIRECT2 (Pitch_help) {
	Melder_help (U"Pitch");
END2 }

DIRECT2 (Pitch_hum) {
	LOOP {
		iam (Pitch);
		Pitch_hum (me, 0.0, 0.0);
	}
END2 }

DIRECT2 (Pitch_interpolate) {
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_interpolate (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_killOctaveJumps) {
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_killOctaveJumps (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_play) {
	LOOP {
		iam (Pitch);
		Pitch_play (me, 0.0, 0.0);
	}
END2 }

FORM (Pitch_smooth, U"Pitch: Smooth", U"Pitch: Smooth...") {
	REAL (U"Bandwidth (Hz)", U"10.0")
	OK2
DO
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_smooth (me, GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Pitch_speckle, U"Pitch: Speckle", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_HERTZ);
	}
END2 }

FORM (Pitch_speckleErb, U"Pitch: Speckle erb", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_ERB);
	}
END2 }

FORM (Pitch_speckleLogarithmic, U"Pitch: Speckle logarithmic", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
	}
END2 }

FORM (Pitch_speckleMel, U"Pitch: Speckle mel", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	REAL (U"left Frequency range (mel)", U"0")
	REAL (U"right Frequency range (mel)", U"500")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_MEL);
	}
END2 }

FORM (Pitch_speckleSemitones100, U"Pitch: Speckle semitones (re 100 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
	}
END2 }

FORM (Pitch_speckleSemitones200, U"Pitch: Speckle semitones (re 200 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 200 hertz:")
	REAL (U"left Frequency range (st)", U"-24.0")
	REAL (U"right Frequency range (st)", U"18.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_SEMITONES_200);
	}
END2 }

FORM (Pitch_speckleSemitones440, U"Pitch: Speckle semitones (re 440 Hz)", U"Pitch: Draw...") {
	REAL (STRING_FROM_TIME_SECONDS, U"0.0")
	REAL (STRING_TO_TIME_SECONDS, U"0.0 (= all)")
	LABEL (U"", U"Range in semitones re 440 hertz:")
	REAL (U"left Frequency range (st)", U"-36.0")
	REAL (U"right Frequency range (st)", U"6.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	GET_TMIN_TMAX_FMIN_FMAX
	LOOP {
		iam (Pitch);
		autoPraatPicture picture;
		Pitch_draw (me, GRAPHICS, tmin, tmax, fmin, fmax, GET_INTEGER (U"Garnish"), Pitch_speckle_YES, kPitch_unit_SEMITONES_440);
	}
END2 }

FORM (Pitch_subtractLinearFit, U"Pitch: subtract linear fit", nullptr) {
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Hertz (logarithmic)")
		RADIOBUTTON (U"Mel")
		RADIOBUTTON (U"Semitones")
		RADIOBUTTON (U"ERB")
	OK2
DO
	LOOP {
		iam (Pitch);
		autoPitch thee = Pitch_subtractLinearFit (me, GET_INTEGER (U"Unit") - 1);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_IntervalTier) {
	LOOP {
		iam (Pitch);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_Matrix) {
	LOOP {
		iam (Pitch);
		autoMatrix thee = Pitch_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_PitchTier) {
	LOOP {
		iam (Pitch);
		autoPitchTier thee = Pitch_to_PitchTier (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_PointProcess) {
	LOOP {
		iam (Pitch);
		autoPointProcess thee = Pitch_to_PointProcess (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_Sound_pulses) {
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound (me, 0.0, 0.0, false);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_Sound_hum) {
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound (me, 0.0, 0.0, true);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Pitch_to_Sound_sine, U"Pitch: To Sound (sine)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	RADIO (U"Cut voiceless stretches", 2)
		OPTION (U"exactly")
		OPTION (U"at nearest zero crossings")
	OK2
DO
	LOOP {
		iam (Pitch);
		autoSound thee = Pitch_to_Sound_sine (me, 0.0, 0.0, GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Cut voiceless stretches") - 1);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Pitch_to_TextGrid, U"To TextGrid...", U"Pitch: To TextGrid...") {
	SENTENCE (U"Tier names", U"Mary John bell")
	SENTENCE (U"Point tiers", U"bell")
	OK2
DO
	LOOP {
		iam (Pitch);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (U"Tier names"), GET_STRING (U"Point tiers"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Pitch_to_TextTier) {
	LOOP {
		iam (Pitch);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

/***** PITCH & PITCHTIER *****/

FORM (old_PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"500.0")
	RADIO (U"Line type for non-periodic intervals", 2)
		RADIOBUTTON (U"Normal")
		RADIOBUTTON (U"Dotted")
		RADIOBUTTON (U"Blank")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	PitchTier me = FIRST (PitchTier);
	Pitch thee = FIRST (Pitch);
	autoPraatPicture picture;
	PitchTier_Pitch_draw (me, thee, GRAPHICS,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		GET_INTEGER (U"Line type for non-periodic intervals") - 1,
		GET_INTEGER (U"Garnish"), U"lines and speckles");
END2 }

FORM (PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"From frequency (Hz)", U"0.0")
	REAL (U"To frequency (Hz)", U"500.0")
	RADIO (U"Line type for non-periodic intervals", 2)
		RADIOBUTTON (U"Normal")
		RADIOBUTTON (U"Dotted")
		RADIOBUTTON (U"Blank")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK2
DO_ALTERNATIVE (old_PitchTier_Pitch_draw)
	PitchTier me = FIRST (PitchTier);
	Pitch thee = FIRST (Pitch);
	autoPraatPicture picture;
	PitchTier_Pitch_draw (me, thee, GRAPHICS,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
		GET_INTEGER (U"Line type for non-periodic intervals") - 1,
		GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
END2 }

DIRECT2 (Pitch_PitchTier_to_Pitch) {
	Pitch pitch = FIRST (Pitch);
	PitchTier tier = FIRST (PitchTier);
	autoPitch thee = Pitch_PitchTier_to_Pitch (pitch, tier);
	praat_new (thee.move(), pitch -> name, U"_stylized");
END2 }

/***** PITCH & POINTPROCESS *****/

DIRECT2 (Pitch_PointProcess_to_PitchTier) {
	Pitch pitch = FIRST (Pitch);
	PointProcess point = FIRST (PointProcess);
	autoPitchTier thee = Pitch_PointProcess_to_PitchTier (pitch, point);
	praat_new (thee.move(), pitch -> name);
END2 }

/***** PITCH & SOUND *****/

DIRECT2 (Sound_Pitch_to_Manipulation) {
	Pitch pitch = FIRST (Pitch);
	Sound sound = FIRST (Sound);
	autoManipulation thee = Sound_Pitch_to_Manipulation (sound, pitch);
	praat_new (thee.move(), pitch -> name);
END2 }

DIRECT2 (Sound_Pitch_to_PointProcess_cc) {
	Sound sound = FIRST (Sound);
	Pitch pitch = FIRST (Pitch);
	autoPointProcess thee = Sound_Pitch_to_PointProcess_cc (sound, pitch);
	praat_new (thee.move(), sound -> name, U"_", pitch -> name);
END2 }

FORM (Sound_Pitch_to_PointProcess_peaks, U"Sound & Pitch: To PointProcess (peaks)", 0) {
	BOOLEAN (U"Include maxima", 1)
	BOOLEAN (U"Include minima", 0)
	OK2
DO
	Sound sound = FIRST (Sound);
	Pitch pitch = FIRST (Pitch);
	autoPointProcess thee = Sound_Pitch_to_PointProcess_peaks (sound, pitch, GET_INTEGER (U"Include maxima"), GET_INTEGER (U"Include minima"));
	praat_new (thee.move(), sound -> name, U"_", pitch -> name);
END2 }

/***** PITCHTIER *****/

FORM (PitchTier_addPoint, U"PitchTier: Add point", U"PitchTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Pitch (Hz)", U"200.0")
	OK2
DO
	LOOP {
		iam (PitchTier);
		RealTier_addPoint (me, GET_REAL (U"Time"), GET_REAL (U"Pitch"));
		praat_dataChanged (me);
	}
END2 }

FORM (PitchTier_create, U"Create empty PitchTier", nullptr) {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double startTime = GET_REAL (U"Start time"), endTime = GET_REAL (U"End time");
	if (endTime <= startTime) Melder_throw (U"End time must be greater than start time.");
	autoPitchTier me = PitchTier_create (startTime, endTime);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (PitchTier_downto_PointProcess) {
	LOOP {
		iam (PitchTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me->asAnyTier());
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PitchTier_downto_TableOfReal, U"PitchTier: Down to TableOfReal", nullptr) {
	RADIO (U"Unit", 1)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Semitones")
	OK2
DO
	LOOP {
		iam (PitchTier);
		autoTableOfReal thee = PitchTier_downto_TableOfReal (me, GET_INTEGER (U"Unit") - 1);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (old_PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	double minimumFrequency = GET_REAL (STRING_FROM_FREQUENCY);
	double maximumFrequency = GET_REAL (STRING_TO_FREQUENCY);
	if (maximumFrequency <= minimumFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), minimumFrequency, maximumFrequency,
			GET_INTEGER (U"Garnish"), U"lines and speckles");
	}
END2 }

FORM (PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK2
DO_ALTERNATIVE (old_PitchTier_draw)
	double minimumFrequency = GET_REAL (STRING_FROM_FREQUENCY);
	double maximumFrequency = GET_REAL (STRING_TO_FREQUENCY);
	if (maximumFrequency <= minimumFrequency) Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	LOOP {
		iam (PitchTier);
		autoPraatPicture picture;
		PitchTier_draw (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), minimumFrequency, maximumFrequency,
			GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END2 }

DIRECT2 (PitchTier_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PitchTier from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPitchTier) {
		iam (PitchTier);
		autoPitchTierEditor editor = PitchTierEditor_create (ID_AND_FULL_NAME, me, sound, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (PitchTier_formula, U"PitchTier: Formula", U"PitchTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in hertz")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD (U"formula", U"self * 2 ; one octave up")
	OK2
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
END2 }

FORM (PitchTier_getMean_curve, U"PitchTier: Get mean (curve)", U"PitchTier: Get mean (curve)...") {
	praat_dia_timeRange (dia);
	OK2
DO
	Melder_informationReal (RealTier_getMean_curve (FIRST_ANY (PitchTier), GET_REAL (U"left Time range"), GET_REAL (U"right Time range")), U"Hz");
END2 }
	
FORM (PitchTier_getMean_points, U"PitchTier: Get mean (points)", U"PitchTier: Get mean (points)...") {
	praat_dia_timeRange (dia);
	OK2
DO
	Melder_informationReal (RealTier_getMean_points (FIRST_ANY (PitchTier), GET_REAL (U"left Time range"), GET_REAL (U"right Time range")), U"Hz");
END2 }
	
FORM (PitchTier_getStandardDeviation_curve, U"PitchTier: Get standard deviation (curve)", U"PitchTier: Get standard deviation (curve)...") {
	praat_dia_timeRange (dia);
	OK2
DO
	Melder_informationReal (RealTier_getStandardDeviation_curve (FIRST_ANY (PitchTier), GET_REAL (U"left Time range"), GET_REAL (U"right Time range")), U"Hz");
END2 }
	
FORM (PitchTier_getStandardDeviation_points, U"PitchTier: Get standard deviation (points)", U"PitchTier: Get standard deviation (points)...") {
	praat_dia_timeRange (dia);
	OK2
DO
	Melder_informationReal (RealTier_getStandardDeviation_points (FIRST_ANY (PitchTier), GET_REAL (U"left Time range"), GET_REAL (U"right Time range")), U"Hz");
END2 }
	
FORM (PitchTier_getValueAtTime, U"PitchTier: Get value at time", U"PitchTier: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_informationReal (RealTier_getValueAtTime (FIRST_ANY (PitchTier), GET_REAL (U"Time")), U"Hz");
END2 }
	
FORM (PitchTier_getValueAtIndex, U"PitchTier: Get value at index", U"PitchTier: Get value at index...") {
	INTEGER (U"Point number", U"10")
	OK2
DO
	Melder_informationReal (RealTier_getValueAtIndex (FIRST_ANY (PitchTier), GET_INTEGER (U"Point number")), U"Hz");
END2 }

DIRECT2 (PitchTier_help) {
	Melder_help (U"PitchTier");
END2 }

DIRECT2 (PitchTier_hum) {
	LOOP {
		iam (PitchTier);
		PitchTier_hum (me);
	}
END2 }

FORM (PitchTier_interpolateQuadratically, U"PitchTier: Interpolate quadratically", nullptr) {
	NATURAL (U"Number of points per parabola", U"4")
	RADIO (U"Unit", 2)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK2
DO
	LOOP {
		iam (PitchTier);
		RealTier_interpolateQuadratically (me, GET_INTEGER (U"Number of points per parabola"), GET_INTEGER (U"Unit") - 1);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (PitchTier_play) {
	LOOP {
		iam (PitchTier);
		PitchTier_play (me);
	}
END2 }

DIRECT2 (PitchTier_playSine) {
	LOOP {
		iam (PitchTier);
		PitchTier_playPart_sine (me, 0.0, 0.0);
	}
END2 }

FORM (PitchTier_shiftFrequencies, U"PitchTier: Shift frequencies", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1000.0")
	REAL (U"Frequency shift", U"-20.0")
	OPTIONMENU (U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK2
DO
	int unit = GET_INTEGER (U"Unit");
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	LOOP {
		iam (PitchTier);
		try {
			PitchTier_shiftFrequencies (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Frequency shift"), unit);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PitchTier may have partially changed
			throw;
		}
	}
END2 }

FORM (PitchTier_multiplyFrequencies, U"PitchTier: Multiply frequencies", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1000.0")
	POSITIVE (U"Factor", U"1.2")
	OK2
DO
	LOOP {
		iam (PitchTier);
		PitchTier_multiplyFrequencies (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Factor"));
		praat_dataChanged (me);
	}
END2 }

FORM (PitchTier_stylize, U"PitchTier: Stylize", U"PitchTier: Stylize...") {
	REAL (U"Frequency resolution", U"4.0")
	RADIO (U"Unit", 2)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK2
DO
	LOOP {
		iam (PitchTier);
		PitchTier_stylize (me, GET_REAL (U"Frequency resolution"), GET_INTEGER (U"Unit") - 1);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (PitchTier_to_PointProcess) {
	LOOP {
		iam (PitchTier);
		autoPointProcess thee = PitchTier_to_PointProcess (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PitchTier_to_Sound_phonation, U"PitchTier: To Sound (phonation)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Maximum period (s)", U"0.05")
	POSITIVE (U"Open phase", U"0.7")
	REAL (U"Collision phase", U"0.03")
	POSITIVE (U"Power 1", U"3.0")
	POSITIVE (U"Power 2", U"4.0")
	BOOLEAN (U"Hum", false)
	OK2
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_phonation (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Maximum period"),
			GET_REAL (U"Open phase"), GET_REAL (U"Collision phase"), GET_REAL (U"Power 1"), GET_REAL (U"Power 2"), GET_INTEGER (U"Hum"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PitchTier_to_Sound_pulseTrain, U"PitchTier: To Sound (pulse train)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Adaptation time", U"0.05")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	BOOLEAN (U"Hum", false)
	OK2
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_pulseTrain (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Adaptation time"),
			GET_INTEGER (U"Interpolation depth"), GET_INTEGER (U"Hum"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PitchTier_to_Sound_sine, U"PitchTier: To Sound (sine)", nullptr) {
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	OK2
DO
	LOOP {
		iam (PitchTier);
		autoSound thee = PitchTier_to_Sound_sine (me, 0.0, 0.0, GET_REAL (U"Sampling frequency"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (info_PitchTier_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your PitchTier editor:\n"
		"   select a PitchTier and a Sound, and click \"View & Edit\".");
END2 }

FORM_WRITE (PitchTier_writeToPitchTierSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"PitchTier")
	LOOP {
		iam (PitchTier);
		PitchTier_writeToPitchTierSpreadsheetFile (me, file);
	}
END

FORM_WRITE (PitchTier_writeToHeaderlessSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"txt")
	LOOP {
		iam (PitchTier);
		PitchTier_writeToHeaderlessSpreadsheetFile (me, file);
	}
END

/***** PITCHTIER & POINTPROCESS *****/

DIRECT2 (PitchTier_PointProcess_to_PitchTier) {
	PitchTier pitch = FIRST (PitchTier);
	PointProcess point = FIRST (PointProcess);
	autoPitchTier thee = PitchTier_PointProcess_to_PitchTier (pitch, point);
	praat_new (thee.move(), pitch -> name);
END2 }

/***** POINTPROCESS *****/

FORM (PointProcess_addPoint, U"PointProcess: Add point", U"PointProcess: Add point...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (PointProcess);
		PointProcess_addPoint (me, GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END2 }

FORM (PointProcess_createEmpty, U"Create an empty PointProcess", U"Create empty PointProcess...") {
	WORD (U"Name", U"empty")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	OK2
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax < tmin) Melder_throw (U"End time (", tmax, U") should not be less than start time (", tmin, U").");
	autoPointProcess me = PointProcess_create (tmin, tmax, 0);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (PointProcess_createPoissonProcess, U"Create Poisson process", U"Create Poisson process...") {
	WORD (U"Name", U"poisson")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	POSITIVE (U"Density (/s)", U"100.0")
	OK2
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax < tmin)
		Melder_throw (U"End time (", tmax, U") should not be less than start time (", tmin, U").");
	autoPointProcess me = PointProcess_createPoissonProcess (tmin, tmax, GET_REAL (U"Density"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (PointProcess_difference) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_difference (point1, point2);
	praat_new (thee.move(), U"difference");
END2 }

FORM (PointProcess_draw, U"PointProcess: Draw", nullptr) {
	praat_dia_timeRange (dia);
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoPraatPicture picture;
		PointProcess_draw (me, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT2 (PointProcess_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PointProcess from batch.");
	Sound sound = FIRST (Sound);
	LOOP if (CLASS == classPointProcess) {
		iam (PointProcess);
		autoPointEditor editor = PointEditor_create (ID_AND_FULL_NAME, me, sound);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (PointProcess_fill, U"PointProcess: Fill", nullptr) {
	praat_dia_timeRange (dia);
	POSITIVE (U"Period (s)", U"0.01")
	OK2
DO
	LOOP {
		iam (PointProcess);
		try {
			PointProcess_fill (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_REAL (U"Period"));
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the PointProcess may have partially changed
			throw;
		}
	}
END2 }

FORM (PointProcess_getInterval, U"PointProcess: Get interval", U"PointProcess: Get interval...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_informationReal (PointProcess_getInterval (FIRST_ANY (PointProcess), GET_REAL (U"Time")), U"seconds");
END2 }

static void dia_PointProcess_getRangeProperty (UiForm dia) {
	praat_dia_timeRange (dia);
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
}

FORM (PointProcess_getJitter_local, U"PointProcess: Get jitter (local)", U"PointProcess: Get jitter (local)...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getJitter_local (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), nullptr);
END2 }

FORM (PointProcess_getJitter_local_absolute, U"PointProcess: Get jitter (local, absolute)", U"PointProcess: Get jitter (local, absolute)...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getJitter_local_absolute (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), U"seconds");
END2 }

FORM (PointProcess_getJitter_rap, U"PointProcess: Get jitter (rap)", U"PointProcess: Get jitter (rap)...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getJitter_rap (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), nullptr);
END2 }

FORM (PointProcess_getJitter_ppq5, U"PointProcess: Get jitter (ppq5)", U"PointProcess: Get jitter (ppq5)...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getJitter_ppq5 (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), nullptr);
END2 }

FORM (PointProcess_getJitter_ddp, U"PointProcess: Get jitter (ddp)", U"PointProcess: Get jitter (ddp)...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getJitter_ddp (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), nullptr);
END2 }

FORM (PointProcess_getMeanPeriod, U"PointProcess: Get mean period", U"PointProcess: Get mean period...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getMeanPeriod (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), U"seconds");
END2 }

FORM (PointProcess_getStdevPeriod, U"PointProcess: Get stdev period", U"PointProcess: Get stdev period...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_informationReal (PointProcess_getStdevPeriod (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")), U"seconds");
END2 }

FORM (PointProcess_getLowIndex, U"PointProcess: Get low index", U"PointProcess: Get low index...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_information (PointProcess_getLowIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END2 }

FORM (PointProcess_getHighIndex, U"PointProcess: Get high index", U"PointProcess: Get high index...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_information (PointProcess_getHighIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END2 }

FORM (PointProcess_getNearestIndex, U"PointProcess: Get nearest index", U"PointProcess: Get nearest index...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	Melder_information (PointProcess_getNearestIndex (FIRST_ANY (PointProcess), GET_REAL (U"Time")));
END2 }

DIRECT2 (PointProcess_getNumberOfPoints) {
	PointProcess me = FIRST_ANY (PointProcess);
	Melder_information (my nt);
END2 }

FORM (PointProcess_getNumberOfPeriods, U"PointProcess: Get number of periods", U"PointProcess: Get number of periods...") {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	Melder_information (PointProcess_getNumberOfPeriods (FIRST_ANY (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor")));
END2 }

FORM (PointProcess_getTimeFromIndex, U"Get time", 0 /*"PointProcess: Get time from index..."*/) {
	NATURAL (U"Point number", U"10")
	OK2
DO
	PointProcess me = FIRST_ANY (PointProcess);
	long i = GET_INTEGER (U"Point number");
	if (i > my nt) Melder_information (U"--undefined--");
	else Melder_informationReal (my t [i], U"seconds");
END2 }

DIRECT2 (PointProcess_help) {
	Melder_help (U"PointProcess");
END2 }

DIRECT2 (PointProcess_hum) {
	LOOP {
		iam (PointProcess);
		PointProcess_hum (me, my xmin, my xmax);
	}
END2 }

DIRECT2 (PointProcess_intersection) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_intersection (point1, point2);
	praat_new (thee.move(), U"intersection");
END2 }

DIRECT2 (PointProcess_play) {
	LOOP {
		iam (PointProcess);
		PointProcess_play (me);
	}
END2 }

FORM (PointProcess_removePoint, U"PointProcess: Remove point", U"PointProcess: Remove point...") {
	NATURAL (U"Index", U"1")
	OK2
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoint (me, GET_INTEGER (U"Index"));
		praat_dataChanged (me);
	}
END2 }

FORM (PointProcess_removePointNear, U"PointProcess: Remove point near", U"PointProcess: Remove point near...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointNear (me, GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END2 }

FORM (PointProcess_removePoints, U"PointProcess: Remove points", U"PointProcess: Remove points...") {
	NATURAL (U"From index", U"1")
	NATURAL (U"To index", U"10")
	OK2
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePoints (me, GET_INTEGER (U"From index"), GET_INTEGER (U"To index"));
		praat_dataChanged (me);
	}
END2 }

FORM (PointProcess_removePointsBetween, U"PointProcess: Remove points between", U"PointProcess: Remove points between...") {
	REAL (U"left Time range (s)", U"0.3")
	REAL (U"right Time range (s)", U"0.7")
	OK2
DO
	LOOP {
		iam (PointProcess);
		PointProcess_removePointsBetween (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (PointProcess_to_IntervalTier) {
	LOOP {
		iam (PointProcess);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (PointProcess_to_Matrix) {
	LOOP {
		iam (PointProcess);
		autoMatrix thee = PointProcess_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_to_PitchTier, U"PointProcess: To PitchTier", U"PointProcess: To PitchTier...") {
	POSITIVE (U"Maximum interval (s)", U"0.02")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_to_PitchTier (me, GET_REAL (U"Maximum interval"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_to_TextGrid, U"PointProcess: To TextGrid...", U"PointProcess: To TextGrid...") {
	SENTENCE (U"Tier names", U"Mary John bell")
	SENTENCE (U"Point tiers", U"bell")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (U"Tier names"), GET_STRING (U"Point tiers"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_to_TextGrid_vuv, U"PointProcess: To TextGrid (vuv)...", U"PointProcess: To TextGrid (vuv)...") {
	POSITIVE (U"Maximum period (s)", U"0.02")
	REAL (U"Mean period (s)", U"0.01")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoTextGrid thee = PointProcess_to_TextGrid_vuv (me, GET_REAL (U"Maximum period"), GET_REAL (U"Mean period"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (PointProcess_to_TextTier) {
	LOOP {
		iam (PointProcess);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_to_Sound_phonation, U"PointProcess: To Sound (phonation)", U"PointProcess: To Sound (phonation)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Maximum period (s)", U"0.05")
	POSITIVE (U"Open phase", U"0.7")
	REAL (U"Collision phase", U"0.03")
	POSITIVE (U"Power 1", U"3.0")
	POSITIVE (U"Power 2", U"4.0")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_phonation (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Maximum period"),
			GET_REAL (U"Open phase"), GET_REAL (U"Collision phase"), GET_REAL (U"Power 1"), GET_REAL (U"Power 2"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_to_Sound_pulseTrain, U"PointProcess: To Sound (pulse train)", U"PointProcess: To Sound (pulse train)...") {
	POSITIVE (U"Sampling frequency (Hz)", U"44100")
	POSITIVE (U"Adaptation factor", U"1.0")
	POSITIVE (U"Adaptation time (s)", U"0.05")
	NATURAL (U"Interpolation depth (samples)", U"2000")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_pulseTrain (me, GET_REAL (U"Sampling frequency"),
			GET_REAL (U"Adaptation factor"), GET_REAL (U"Adaptation time"),
			GET_INTEGER (U"Interpolation depth"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (PointProcess_to_Sound_hum) {
	LOOP {
		iam (PointProcess);
		autoSound thee = PointProcess_to_Sound_hum (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (PointProcess_union) {
	PointProcess point1 = nullptr, point2 = nullptr;
	LOOP (point1 ? point2 : point1) = (PointProcess) OBJECT;
	autoPointProcess thee = PointProcesses_union (point1, point2);
	praat_new (thee.move(), U"union");
END2 }

FORM (PointProcess_upto_IntensityTier, U"PointProcess: Up to IntensityTier", U"PointProcess: Up to IntensityTier...") {
	POSITIVE (U"Intensity (dB)", U"70.0")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoIntensityTier thee = PointProcess_upto_IntensityTier (me, GET_REAL (U"Intensity"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_upto_PitchTier, U"PointProcess: Up to PitchTier", U"PointProcess: Up to PitchTier...") {
	POSITIVE (U"Frequency (Hz)", U"190.0")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoPitchTier thee = PointProcess_upto_PitchTier (me, GET_REAL (U"Frequency"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_upto_TextTier, U"PointProcess: Up to TextTier", U"PointProcess: Up to TextTier...") {
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (PointProcess);
		autoTextTier thee = PointProcess_upto_TextTier (me, GET_STRING (U"Text"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (PointProcess_voice, U"PointProcess: Fill unvoiced parts", nullptr) {
	POSITIVE (U"Period (s)", U"0.01")
	POSITIVE (U"Maximum voiced period (s)", U"0.02000000001")
	OK2
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
END2 }

DIRECT2 (info_PointProcess_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your PointProcess editor:\n"
		"   select a PointProcess and a Sound, and click \"View & Edit\".");
END2 }

/***** POINTPROCESS & SOUND *****/

DIRECT2 (Point_Sound_transplantDomain) {
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	point -> xmin = sound -> xmin;
	point -> xmax = sound -> xmax;
	praat_dataChanged (point);
END2 }

FORM (Point_Sound_getShimmer_local, U"PointProcess & Sound: Get shimmer (local)", U"PointProcess & Sound: Get shimmer (local)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (Point_Sound_getShimmer_local_dB, U"PointProcess & Sound: Get shimmer (local, dB)", U"PointProcess & Sound: Get shimmer (local, dB)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_local_dB (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (Point_Sound_getShimmer_apq3, U"PointProcess & Sound: Get shimmer (apq3)", U"PointProcess & Sound: Get shimmer (apq3)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq3 (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (Point_Sound_getShimmer_apq5, U"PointProcess & Sound: Get shimmer (apq)", U"PointProcess & Sound: Get shimmer (apq5)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq5 (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (Point_Sound_getShimmer_apq11, U"PointProcess & Sound: Get shimmer (apq11)", U"PointProcess & Sound: Get shimmer (apq11)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_apq11 (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (Point_Sound_getShimmer_dda, U"PointProcess & Sound: Get shimmer (dda)", U"PointProcess & Sound: Get shimmer (dda)...") {
	dia_PointProcess_getRangeProperty (dia);
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	double shimmer = PointProcess_Sound_getShimmer_dda (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"));
	Melder_informationReal (shimmer, nullptr);
END2 }

FORM (PointProcess_Sound_to_AmplitudeTier_period, U"PointProcess & Sound: To AmplitudeTier (period)", nullptr) {
	dia_PointProcess_getRangeProperty (dia);
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_period (point, sound,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
	praat_new (thee.move(), sound -> name, U"_", point -> name);
END2 }

DIRECT2 (PointProcess_Sound_to_AmplitudeTier_point) {
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoAmplitudeTier thee = PointProcess_Sound_to_AmplitudeTier_point (point, sound);
	praat_new (thee.move(), sound -> name, U"_", point -> name);
END2 }

FORM (PointProcess_Sound_to_Ltas, U"PointProcess & Sound: To Ltas", nullptr) {
	POSITIVE (U"Maximum frequency (Hz)", U"5000")
	POSITIVE (U"Band width (Hz)", U"100")
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas (point, sound,
		GET_REAL (U"Maximum frequency"), GET_REAL (U"Band width"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
	praat_new (thee.move(), sound -> name);
END2 }

FORM (PointProcess_Sound_to_Ltas_harmonics, U"PointProcess & Sound: To Ltas (harmonics", nullptr) {
	NATURAL (U"Maximum harmonic", U"20")
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoLtas thee = PointProcess_Sound_to_Ltas_harmonics (point, sound,
		GET_INTEGER (U"Maximum harmonic"),
		GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
	praat_new (thee.move(), sound -> name);
END2 }

FORM (Sound_PointProcess_to_SoundEnsemble_correlate, U"Sound & PointProcess: To SoundEnsemble (correlate)", nullptr) {
	REAL (U"From time (s)", U"-0.1")
	REAL (U"To time (s)", U"1.0")
	OK2
DO
	PointProcess point = FIRST (PointProcess);
	Sound sound = FIRST (Sound);
	autoSound thee = Sound_PointProcess_to_SoundEnsemble_correlate (sound, point, GET_REAL (U"From time"), GET_REAL (U"To time"));
	praat_new (thee.move(), point -> name);
END2 }

/***** POLYGON *****/

FORM (Polygon_draw, U"Polygon: Draw", nullptr) {
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0")
	OK2
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_draw (me, GRAPHICS, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"));
	}
END2 }

FORM (Polygon_drawCircles, U"Polygon: Draw circles", nullptr) {
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0 (= all)")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0 (= all)")
	POSITIVE (U"Diameter (mm)", U"3.0")
	OK2
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_drawCircles (me, GRAPHICS,
			GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"),
			GET_REAL (U"Diameter"));
	}
END2 }

FORM (Polygon_drawClosed, U"Polygon: Draw", nullptr) {
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0")
	OK2
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_drawClosed (me, GRAPHICS, GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"));
	}
END2 }

FORM (Polygons_drawConnection, U"Polygons: Draw connection", nullptr) {
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0 (= all)")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0 (= all)")
	BOOLEAN (U"Arrow", 0)
	POSITIVE (U"Relative length", U"0.9")
	OK2
DO
	Polygon polygon1 = nullptr, polygon2 = nullptr;
	LOOP (polygon1 ? polygon2 : polygon1) = (Polygon) OBJECT;
	autoPraatPicture picture;
	Polygons_drawConnection (polygon1, polygon2, GRAPHICS,
		GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"),
		GET_INTEGER (U"Arrow"), GET_REAL (U"Relative length"));
END2 }

DIRECT2 (Polygon_help) {
	Melder_help (U"Polygon");
END2 }

FORM (Polygon_paint, U"Polygon: Paint", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0 (= all)")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_paint (me, GRAPHICS, GET_COLOUR (U"Colour"), GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"));
	}
END2 }

FORM (Polygon_paintCircles, U"Polygon: Paint circles", nullptr) {
	REAL (U"Xmin", U"0.0")
	REAL (U"Xmax", U"0.0 (= all)")
	REAL (U"Ymin", U"0.0")
	REAL (U"Ymax", U"0.0 (= all)")
	POSITIVE (U"Diameter (mm)", U"3.0")
	OK2
DO
	LOOP {
		iam (Polygon);
		autoPraatPicture picture;
		Polygon_paintCircles (me, GRAPHICS,
			GET_REAL (U"Xmin"), GET_REAL (U"Xmax"), GET_REAL (U"Ymin"), GET_REAL (U"Ymax"), GET_REAL (U"Diameter"));
	}
END2 }

DIRECT2 (Polygon_randomize) {
	LOOP {
		iam (Polygon);
		Polygon_randomize (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Polygon_salesperson, U"Polygon: Find shortest path", nullptr) {
	NATURAL (U"Number of iterations", U"1")
	OK2
DO
	LOOP {
		iam (Polygon);
		Polygon_salesperson (me, GET_INTEGER (U"Number of iterations"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Polygon_to_Matrix) {
	LOOP {
		iam (Polygon);
		autoMatrix thee = Polygon_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** SOUND & PITCH & POINTPROCESS *****/

FORM (Sound_Pitch_PointProcess_voiceReport, U"Voice report", U"Voice") {
	praat_dia_timeRange (dia);
	POSITIVE (U"left Pitch range (Hz)", U"75.0")
	POSITIVE (U"right Pitch range (Hz)", U"600.0")
	POSITIVE (U"Maximum period factor", U"1.3")
	POSITIVE (U"Maximum amplitude factor", U"1.6")
	REAL (U"Silence threshold", U"0.03")
	REAL (U"Voicing threshold", U"0.45")
	OK2
DO
	MelderInfo_open ();
	Sound_Pitch_PointProcess_voiceReport (FIRST (Sound), FIRST (Pitch), FIRST (PointProcess),
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
		GET_REAL (U"left Pitch range"), GET_REAL (U"right Pitch range"),
		GET_REAL (U"Maximum period factor"), GET_REAL (U"Maximum amplitude factor"),
		GET_REAL (U"Silence threshold"), GET_REAL (U"Voicing threshold"));
	MelderInfo_close ();
END2 }

/***** SOUND & POINTPROCESS & PITCHTIER & DURATIONTIER *****/

FORM (Sound_Point_Pitch_Duration_to_Sound, U"To Sound", nullptr) {
	POSITIVE (U"Longest period (s)", U"0.02")
	OK2
DO
	autoSound thee = Sound_Point_Pitch_Duration_to_Sound (FIRST (Sound), FIRST (PointProcess),
		FIRST (PitchTier), FIRST (DurationTier), GET_REAL (U"Longest period"));
	praat_new (thee.move(), U"manip");
END2 }

/***** SPECTROGRAM *****/

FORM (Spectrogram_paint, U"Spectrogram: Paint", U"Spectrogram: Paint...") {
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (U"Maximum (dB/Hz)", U"100.0")
	BOOLEAN (U"Autoscaling", 1)
	POSITIVE (U"Dynamic range (dB)", U"50.0")
	REAL (U"Pre-emphasis (dB/oct)", U"6.0")
	REAL (U"Dynamic compression (0-1)", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	LOOP {
		iam (Spectrogram);
		autoPraatPicture picture;
		Spectrogram_paint (me, GRAPHICS, GET_REAL (U"left Time range"),
			GET_REAL (U"right Time range"), GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"Maximum"), GET_INTEGER (U"Autoscaling"),
			GET_REAL (U"Dynamic range"), GET_REAL (U"Pre-emphasis"),
			GET_REAL (U"Dynamic compression"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Spectrogram_formula, U"Spectrogram: Formula", U"Spectrogram: Formula...") {
	LABEL (U"label", U"Do for all times and frequencies:")
	LABEL (U"label", U"   `x' is the time in seconds")
	LABEL (U"label", U"   `y' is the frequency in hertz")
	LABEL (U"label", U"   `self' is the current value in Pa\u00B2/Hz")
	LABEL (U"label", U"   Replace all values with:")
	TEXTFIELD (U"formula", U"self * exp (- x / 0.1)")
	OK2
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
END2 }

FORM (Spectrogram_getPowerAt, U"Spectrogram: Get power at (time, frequency)", nullptr) {
	REAL (U"Time (s)", U"0.5")
	REAL (U"Frequency (Hz)", U"1000")
	OK2
DO
	Spectrogram me = FIRST_ANY (Spectrogram);
	double time = GET_REAL (U"Time"), frequency = GET_REAL (U"Frequency");
	MelderInfo_open ();
	MelderInfo_write (Matrix_getValueAtXY (me, time, frequency));
	MelderInfo_write (U" Pa2/Hz (at time = ", time, U" seconds and frequency = ", frequency, U" Hz)");
	MelderInfo_close ();
END2 }

DIRECT2 (Spectrogram_help) {
	Melder_help (U"Spectrogram");
END2 }

DIRECT2 (Spectrogram_movie) {
	Graphics g = Movie_create (U"Spectrogram movie", 300, 300);
	LOOP {
		iam (Spectrogram);
		Matrix_movie (me, g);
	}
END2 }

DIRECT2 (Spectrogram_to_Matrix) {
	LOOP {
		iam (Spectrogram);
		autoMatrix thee = Spectrogram_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrogram_to_Sound, U"Spectrogram: To Sound", nullptr) {
	REAL (U"Sampling frequency (Hz)", U"44100")
	OK2
DO
	LOOP {
		iam (Spectrogram);
		autoSound thee = Spectrogram_to_Sound (me, GET_REAL (U"Sampling frequency"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrogram_to_Spectrum, U"Spectrogram: To Spectrum (slice)", nullptr) {
	REAL (U"Time (seconds)", U"0.0")
	OK2
DO
	LOOP {
		iam (Spectrogram);
		autoSpectrum thee = Spectrogram_to_Spectrum (me, GET_REAL (U"Time"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrogram_view) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Spectrogram from batch.");
	LOOP {
		iam (Spectrogram);
		autoSpectrogramEditor editor = SpectrogramEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

/***** SPECTRUM *****/

FORM (Spectrum_cepstralSmoothing, U"Spectrum: Cepstral smoothing", nullptr) {
	POSITIVE (U"Bandwidth (Hz)", U"500.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_cepstralSmoothing (me, GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrum_draw, U"Spectrum: Draw", nullptr) {
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL (U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoPraatPicture picture;
		Spectrum_draw (me, GRAPHICS, GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"Minimum power"), GET_REAL (U"Maximum power"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Spectrum_drawLogFreq, U"Spectrum: Draw (log freq)", nullptr) {
	POSITIVE (U"left Frequency range (Hz)", U"10.0")
	POSITIVE (U"right Frequency range (Hz)", U"10000.0")
	REAL (U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL (U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoPraatPicture picture;
		Spectrum_drawLogFreq (me, GRAPHICS, GET_REAL (U"left Frequency range"),
			GET_REAL (U"right Frequency range"), GET_REAL (U"Minimum power"), GET_REAL (U"Maximum power"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT2 (Spectrum_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Spectrum from batch.");
	LOOP {
		iam (Spectrum);
		autoSpectrumEditor editor = SpectrumEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (Spectrum_formula, U"Spectrum: Formula", U"Spectrum: Formula...") {
	LABEL (U"label", U"`x' is the frequency in hertz, `col' is the bin number;   "
		"`y' = `row' is 1 (real part) or 2 (imaginary part)")
	LABEL (U"label", U"y := 1;   row := 1;   "
		"x := 0;   for col := 1 to ncol do { self [1, col] := `formula' ; x := x + dx }")
	LABEL (U"label", U"y := 2;   row := 2;   "
		"x := 0;   for col := 1 to ncol do { self [2, col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Spectrum may have partially changed
			throw;
		}
	}
END2 }

FORM (Spectrum_getBandDensity, U"Spectrum: Get band density", nullptr) {
	REAL (U"Band floor (Hz)", U"200.0")
	REAL (U"Band ceiling (Hz)", U"1000")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double density = Spectrum_getBandDensity (me, GET_REAL (U"Band floor"), GET_REAL (U"Band ceiling"));
		Melder_informationReal (density, U"Pa2 / Hz2");
	}
END2 }

FORM (Spectrum_getBandDensityDifference, U"Spectrum: Get band density difference", nullptr) {
	REAL (U"Low band floor (Hz)", U"0.0")
	REAL (U"Low band ceiling (Hz)", U"500.0")
	REAL (U"High band floor (Hz)", U"500.0")
	REAL (U"High band ceiling (Hz)", U"4000.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandDensityDifference (me,
			GET_REAL (U"Low band floor"), GET_REAL (U"Low band ceiling"), GET_REAL (U"High band floor"), GET_REAL (U"High band ceiling"));
		Melder_informationReal (difference, U"dB");
	}
END2 }

FORM (Spectrum_getBandEnergy, U"Spectrum: Get band energy", nullptr) {
	REAL (U"Band floor (Hz)", U"200.0")
	REAL (U"Band ceiling (Hz)", U"1000.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double energy = Spectrum_getBandEnergy (me, GET_REAL (U"Band floor"), GET_REAL (U"Band ceiling"));
		Melder_informationReal (energy, U"Pa2 sec");
	}
END2 }

FORM (Spectrum_getBandEnergyDifference, U"Spectrum: Get band energy difference", nullptr) {
	REAL (U"Low band floor (Hz)", U"0.0")
	REAL (U"Low band ceiling (Hz)", U"500.0")
	REAL (U"High band floor (Hz)", U"500.0")
	REAL (U"High band ceiling (Hz)", U"4000.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double difference = Spectrum_getBandEnergyDifference (me,
			GET_REAL (U"Low band floor"), GET_REAL (U"Low band ceiling"), GET_REAL (U"High band floor"), GET_REAL (U"High band ceiling"));
		Melder_informationReal (difference, U"dB");
	}
END2 }

FORM (Spectrum_getBinFromFrequency, U"Spectrum: Get bin from frequency", nullptr) {
	REAL (U"Frequency (Hz)", U"2000")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double bin = Sampled_xToIndex (me, GET_REAL (U"Frequency"));
		Melder_informationReal (bin, nullptr);
	}
END2 }

DIRECT2 (Spectrum_getBinWidth) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my dx, U"hertz");
	}
END2 }

FORM (Spectrum_getCentralMoment, U"Spectrum: Get central moment", U"Spectrum: Get central moment...") {
	POSITIVE (U"Moment", U"3.0")
	POSITIVE (U"Power", U"2.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double moment = Spectrum_getCentralMoment (me, GET_REAL (U"Moment"), GET_REAL (U"Power"));
		Melder_informationReal (moment, U"hertz to the power 'moment'");
	}
END2 }

FORM (Spectrum_getCentreOfGravity, U"Spectrum: Get centre of gravity", U"Spectrum: Get centre of gravity...") {
	POSITIVE (U"Power", U"2.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double centreOfGravity = Spectrum_getCentreOfGravity (me, GET_REAL (U"Power"));
		Melder_informationReal (centreOfGravity, U"hertz");
	}
END2 }

FORM (Spectrum_getFrequencyFromBin, U"Spectrum: Get frequency from bin", nullptr) {
	NATURAL (U"Band number", U"1")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double frequency = Sampled_indexToX (me, GET_INTEGER (U"Band number"));
		Melder_informationReal (frequency, U"hertz");
	}
END2 }

DIRECT2 (Spectrum_getHighestFrequency) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmax, U"hertz");
	}
END2 }

FORM (Spectrum_getImaginaryValueInBin, U"Spectrum: Get imaginary value in bin", nullptr) {
	NATURAL (U"Bin number", U"100")
	OK2
DO
	long binNumber = GET_INTEGER (U"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
		Melder_informationReal (my z [2] [binNumber], nullptr);
	}
END2 }

FORM (Spectrum_getKurtosis, U"Spectrum: Get kurtosis", U"Spectrum: Get kurtosis...") {
	POSITIVE (U"Power", U"2.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double kurtosis = Spectrum_getKurtosis (me, GET_REAL (U"Power"));
		Melder_informationReal (kurtosis, nullptr);
	}
END2 }

DIRECT2 (Spectrum_getLowestFrequency) {
	LOOP {
		iam (Spectrum);
		Melder_informationReal (my xmin, U"hertz");
	}
END2 }

DIRECT2 (Spectrum_getNumberOfBins) {
	LOOP {
		iam (Spectrum);
		Melder_information (my nx, U" bins");
	}
END2 }

FORM (Spectrum_getRealValueInBin, U"Spectrum: Get real value in bin", nullptr) {
	NATURAL (U"Bin number", U"100")
	OK2
DO
	long binNumber = GET_INTEGER (U"Bin number");
	LOOP {
		iam (Spectrum);
		if (binNumber > my nx) Melder_throw (U"Bin number must not exceed number of bins.");
		Melder_informationReal (my z [1] [binNumber], nullptr);
	}
END2 }

FORM (Spectrum_getSkewness, U"Spectrum: Get skewness", U"Spectrum: Get skewness...") {
	POSITIVE (U"Power", U"2.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double skewness = Spectrum_getSkewness (me, GET_REAL (U"Power"));
		Melder_informationReal (skewness, nullptr);
	}
END2 }

FORM (Spectrum_getStandardDeviation, U"Spectrum: Get standard deviation", U"Spectrum: Get standard deviation...") {
	POSITIVE (U"Power", U"2.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		double stdev = Spectrum_getStandardDeviation (me, GET_REAL (U"Power"));
		Melder_informationReal (stdev, U"hertz");
	}
END2 }

DIRECT2 (Spectrum_help) {
	Melder_help (U"Spectrum");
END2 }

FORM (Spectrum_list, U"Spectrum: List", 0) {
	BOOLEAN (U"Include bin number", false)
	BOOLEAN (U"Include frequency", true)
	BOOLEAN (U"Include real part", false)
	BOOLEAN (U"Include imaginary part", false)
	BOOLEAN (U"Include energy density", false)
	BOOLEAN (U"Include power density", true)
	OK2
DO
	LOOP {
		iam (Spectrum);
		Spectrum_list (me, GET_INTEGER (U"Include bin number"), GET_INTEGER (U"Include frequency"),
			GET_INTEGER (U"Include real part"), GET_INTEGER (U"Include imaginary part"),
			GET_INTEGER (U"Include energy density"), GET_INTEGER (U"Include power density"));
	}
END2 }

FORM (Spectrum_lpcSmoothing, U"Spectrum: LPC smoothing", 0) {
	NATURAL (U"Number of peaks", U"5")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoSpectrum thee = Spectrum_lpcSmoothing (me, GET_INTEGER (U"Number of peaks"), GET_REAL (U"Pre-emphasis from"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrum_passHannBand, U"Spectrum: Filter (pass Hann band)", U"Spectrum: Filter (pass Hann band)...") {
	REAL (U"From frequency (Hz)", U"500")
	REAL (U"To frequency (Hz)", U"1000")
	POSITIVE (U"Smoothing (Hz)", U"100")
	OK2
DO
	LOOP {
		iam (Spectrum);
		Spectrum_passHannBand (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_dataChanged (me);
	}
END2 }

FORM (Spectrum_stopHannBand, U"Spectrum: Filter (stop Hann band)", U"Spectrum: Filter (stop Hann band)...") {
	REAL (U"From frequency (Hz)", U"500")
	REAL (U"To frequency (Hz)", U"1000")
	POSITIVE (U"Smoothing (Hz)", U"100")
	OK2
DO
	LOOP {
		iam (Spectrum);
		Spectrum_stopHannBand (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_dataChanged (me);
	}
END2 }

FORM (Spectrum_to_Excitation, U"Spectrum: To Excitation", 0) {
	POSITIVE (U"Frequency resolution (Bark)", U"0.1")
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoExcitation thee = Spectrum_to_Excitation (me, GET_REAL (U"Frequency resolution"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrum_to_Formant_peaks, U"Spectrum: To Formant (peaks)", 0) {
	LABEL (U"", U"Warning: this simply picks peaks from 0 Hz up!")
	NATURAL (U"Maximum number of formants", U"1000")
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoFormant thee = Spectrum_to_Formant (me, GET_INTEGER (U"Maximum number of formants"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Spectrum_to_Ltas, U"Spectrum: To Long-term average spectrum", nullptr) {
	POSITIVE (U"Bandwidth (Hz)", U"1000.0")
	OK2
DO
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas (me, GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrum_to_Ltas_1to1) {
	LOOP {
		iam (Spectrum);
		autoLtas thee = Spectrum_to_Ltas_1to1 (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrum_to_Matrix) {
	LOOP {
		iam (Spectrum);
		autoMatrix thee = Spectrum_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrum_to_Sound) {
	LOOP {
		iam (Spectrum);
		autoSound thee = Spectrum_to_Sound (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrum_to_Spectrogram) {
	LOOP {
		iam (Spectrum);
		autoSpectrogram thee = Spectrum_to_Spectrogram (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Spectrum_to_SpectrumTier_peaks) {
	LOOP {
		iam (Spectrum);
		autoSpectrumTier thee = Spectrum_to_SpectrumTier_peaks (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** SPECTRUMTIER *****/

DIRECT2 (SpectrumTier_downto_Table) {
	LOOP {
		iam (SpectrumTier);
		autoTable thee = SpectrumTier_downto_Table (me, true, true, true);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (old_SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {   // 2010-10-19
	REAL (U"left Frequency range (Hz)", U"0.0")
	REAL (U"right Frequency range (Hz)", U"10000.0")
	REAL (U"left Power range (dB)", U"20.0")
	REAL (U"right Power range (dB)", U"80.0")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"),
			GET_INTEGER (U"Garnish"), U"lines and speckles");
	}
END2 }

FORM (SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {
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
	OK2
DO_ALTERNATIVE (old_SpectrumTier_draw)
	LOOP {
		iam (SpectrumTier);
		autoPraatPicture picture;
		SpectrumTier_draw (me, GRAPHICS,
			GET_REAL (U"left Frequency range"), GET_REAL (U"right Frequency range"),
			GET_REAL (U"left Power range"), GET_REAL (U"right Power range"),
			GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END2 }

FORM (SpectrumTier_list, U"SpectrumTier: List", nullptr) {
	BOOLEAN (U"Include indexes", true)
	BOOLEAN (U"Include frequency", true)
	BOOLEAN (U"Include power density", true)
	OK2
DO
	LOOP {
		iam (SpectrumTier);
		SpectrumTier_list (me, GET_INTEGER (U"Include indexes"), GET_INTEGER (U"Include frequency"), GET_INTEGER (U"Include power density"));
	}
END2 }

FORM (SpectrumTier_removePointsBelow, U"SpectrumTier: Remove points below", nullptr) {
	REAL (U"Remove all points below (dB)", U"40.0")
	OK2
DO
	LOOP {
		iam (SpectrumTier);
		RealTier_removePointsBelow ((RealTier) me, GET_REAL (U"Remove all points below"));
		praat_dataChanged (me);
	}
END2 }

/***** STRINGS *****/

FORM (Strings_createAsFileList, U"Create Strings as file list", U"Create Strings as file list...") {
	SENTENCE (U"Name", U"fileList")
	LABEL (U"", U"File path:")
	TEXTFIELD (U"path", U"/people/Miep/*.wav")
	OK2
static bool inited;
if (! inited) {
	structMelderDir defaultDir { { 0 } };
	Melder_getDefaultDir (& defaultDir);
	char32 *workingDirectory = Melder_dirToPath (& defaultDir);
	char32 path [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, U"/*.wav");
	#elif defined (_WIN32)
	{
		int len = str32len (workingDirectory);
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, len == 0 || workingDirectory [len - 1] != U'\\' ? U"\\" : U"", U"*.wav");
	}
	#else
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, U"*.wav");
	#endif
	SET_STRING (U"path", path);
	inited = true;
}
DO
	autoStrings me = Strings_createAsFileList (GET_STRING (U"path"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Strings_createAsDirectoryList, U"Create Strings as directory list", U"Create Strings as directory list...") {
	SENTENCE (U"Name", U"directoryList")
	LABEL (U"", U"Path:")
	TEXTFIELD (U"path", U"/people/Miep/*")
	OK2
static bool inited;
if (! inited) {
	structMelderDir defaultDir = { { 0 } };
	Melder_getDefaultDir (& defaultDir);
	char32 *workingDirectory = Melder_dirToPath (& defaultDir);
	char32 path [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, U"/*");
	#elif defined (_WIN32)
	{
		int len = str32len (workingDirectory);
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, len == 0 || workingDirectory [len - 1] != U'\\' ? U"\\" : U"");
	}
	#else
		Melder_sprint (path, kMelder_MAXPATH+1, workingDirectory, U"*");
	#endif
	SET_STRING (U"path", path);
	inited = true;
}
DO
	autoStrings me = Strings_createAsDirectoryList (GET_STRING (U"path"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (Strings_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Strings from batch.");
	LOOP {
		iam (Strings);
		autoStringsEditor editor = StringsEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

DIRECT2 (Strings_equal) {
	Strings s1 = nullptr, s2 = nullptr;
	LOOP (s1 ? s2 : s1) = (Strings) OBJECT;
	bool equal = Data_equal (s1, s2);
	Melder_information ((int) equal);   // we need a 0 or 1
END2 }

DIRECT2 (Strings_genericize) {
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
END2 }

DIRECT2 (Strings_getNumberOfStrings) {
	LOOP {
		iam (Strings);
		Melder_information (my numberOfStrings);
	}
END2 }

FORM (Strings_getString, U"Get string", nullptr) {
	NATURAL (U"Position", U"1")
	OK2
DO
	LOOP {
		iam (Strings);
		long index = GET_INTEGER (U"Position");
		Melder_information (index > my numberOfStrings ? U"" : my strings [index]);   // TODO
	}
END2 }

DIRECT2 (Strings_help) {
	Melder_help (U"Strings");
END2 }

FORM (Strings_insertString, U"Strings: Insert string", 0) {
	NATURAL (U"At position", U"1")
	LABEL (U"", U"String:")
	TEXTFIELD (U"string", U"")
	OK2
DO
	LOOP {
		iam (Strings);
		Strings_insert (me, GET_INTEGER (U"At position"), GET_STRING (U"string"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Strings_nativize) {
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
END2 }

DIRECT2 (Strings_randomize) {
	LOOP {
		iam (Strings);
		Strings_randomize (me);
		praat_dataChanged (me);
	}
END2 }

FORM_READ2 (Strings_readFromRawTextFile, U"Read Strings from raw text file", 0, true) {
	autoStrings me = Strings_readFromRawTextFile (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM (Strings_removeString, U"Strings: Remove string", nullptr) {
	NATURAL (U"Position", U"1")
	OK2
DO
	LOOP {
		iam (Strings);
		Strings_remove (me, GET_INTEGER (U"Position"));
		praat_dataChanged (me);
	}
END2 }

FORM (Strings_replaceAll, U"Strings: Replace all", nullptr) {
	SENTENCE (U"Find", U"a")
	SENTENCE (U"Replace with", U"b")
	INTEGER (U"Replace limit per string", U"0 (=unlimited)")
	RADIO (U"Find and replace strings are", 1)
		RADIOBUTTON (U"literals")
		RADIOBUTTON (U"regular expressions")
	OK2
DO
	long numberOfMatches, numberOfStringMatches;
	LOOP {
		iam (Strings);
		autoStrings thee = Strings_change (me, GET_STRING (U"Find"), GET_STRING (U"Replace with"),
			GET_INTEGER (U"Replace limit per string"), & numberOfMatches, & numberOfStringMatches, GET_INTEGER (U"Find and replace strings are") - 1);
		praat_new (thee.move());
	}
END2 }

FORM (Strings_setString, U"Strings: Set string", nullptr) {
	NATURAL (U"Position", U"1")
	LABEL (U"", U"New string:")
	TEXTFIELD (U"newString", U"")
	OK2
DO
	LOOP {
		iam (Strings);
		Strings_replace (me, GET_INTEGER (U"Position"), GET_STRING (U"newString"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Strings_sort) {
	LOOP {
		iam (Strings);
		Strings_sort (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Strings_to_Distributions) {
	LOOP {
		iam (Strings);
		autoDistributions thee = Strings_to_Distributions (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Strings_to_WordList) {
	LOOP {
		iam (Strings);
		autoWordList thee = Strings_to_WordList (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM_WRITE (Strings_writeToRawTextFile, U"Save Strings as text file", nullptr, U"txt")
	LOOP {
		iam (Strings);
		Strings_writeToRawTextFile (me, file);
	}
END

/***** TABLE, rest in praat_Stat.cpp *****/

DIRECT2 (Table_to_Matrix) {
	LOOP {
		iam (Table);
		autoMatrix thee = Table_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** TEXTGRID, rest in praat_TextGrid_init.cpp *****/

FORM (TextGrid_create, U"Create TextGrid", U"Create TextGrid...") {
	LABEL (U"", U"Hint: to label or segment an existing Sound,")
	LABEL (U"", U"select that Sound and choose \"To TextGrid...\".")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	SENTENCE (U"All tier names", U"Mary John bell")
	SENTENCE (U"Which of these are point tiers?", U"bell")
	OK2
DO
	double tmin = GET_REAL (U"Start time"), tmax = GET_REAL (U"End time");
	if (tmax <= tmin) Melder_throw (U"End time should be greater than start time");
	autoTextGrid thee = TextGrid_create (tmin, tmax, GET_STRING (U"All tier names"), GET_STRING (U"Which of these are point tiers?"));
	praat_new (thee.move(), GET_STRING (U"All tier names"));
END2 }

/***** TEXTTIER, rest in praat_TextGrid_init.cpp *****/

FORM_READ2 (TextTier_readFromXwaves, U"Read TextTier from Xwaves", nullptr, true) {
	autoTextTier me = TextTier_readFromXwaves (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

/***** TIMEFRAMESAMPLED *****/

DIRECT2 (TimeFrameSampled_getNumberOfFrames) {
	LOOP {
		iam (Sampled);
		long numberOfFrames = my nx;
		Melder_information (numberOfFrames, U" frames");
	}
END2 }

FORM (TimeFrameSampled_getFrameFromTime, U"Get frame number from time", U"Get frame number from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (Sampled);
		double frame = Sampled_xToIndex (me, GET_REAL (U"Time"));
		Melder_informationReal (frame, nullptr);
	}
END2 }

DIRECT2 (TimeFrameSampled_getFrameLength) {
	LOOP {
		iam (Sampled);
		double frameLength = my dx;
		Melder_informationReal (frameLength, U"seconds");
	}
END2 }

FORM (TimeFrameSampled_getTimeFromFrame, U"Get time from frame number", U"Get time from frame number...") {
	NATURAL (U"Frame number", U"1")
	OK2
DO
	LOOP {
		iam (Sampled);
		double time = Sampled_indexToX (me, GET_INTEGER (U"Frame number"));
		Melder_informationReal (time, U"seconds");
	}
END2 }

/***** TIMEFUNCTION *****/

DIRECT2 (TimeFunction_getDuration) {
	LOOP {
		iam (Function);
		double duration = my xmax - my xmin;
		Melder_informationReal (duration, U"seconds");
	}
END2 }

DIRECT2 (TimeFunction_getEndTime) {
	LOOP {
		iam (Function);
		double endTime = my xmax;
		Melder_informationReal (endTime, U"seconds");
	}
END2 }

DIRECT2 (TimeFunction_getStartTime) {
	LOOP {
		iam (Function);
		double startTime = my xmin;
		Melder_informationReal (startTime, U"seconds");
	}
END2 }

FORM (TimeFunction_scaleTimesBy, U"Scale times by", nullptr) {
	POSITIVE (U"Factor", U"2.0")
	OK2
DO
	LOOP {
		iam (Function);
		Function_scaleXBy (me, GET_REAL (U"Factor"));
		praat_dataChanged (me);
	}
END2 }

FORM (TimeFunction_scaleTimesTo, U"Scale times to", nullptr) {
	REAL (U"New start time (s)", U"0.0")
	REAL (U"New end time (s)", U"1.0")
	OK2
DO
	double tminto = GET_REAL (U"New start time"), tmaxto = GET_REAL (U"New end time");
	if (tminto >= tmaxto) Melder_throw (U"New end time should be greater than new start time.");
	LOOP {
		iam (Function);
		Function_scaleXTo (me, tminto, tmaxto);
		praat_dataChanged (me);
	}
END2 }

FORM (TimeFunction_shiftTimesBy, U"Shift times by", nullptr) {
	REAL (U"Shift (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (Function);
		Function_shiftXBy (me, GET_REAL (U"Shift"));
		praat_dataChanged (me);
	}
END2 }

FORM (TimeFunction_shiftTimesTo, U"Shift times to", nullptr) {
	RADIO (U"Shift", 1)
		OPTION (U"start time")
		OPTION (U"centre time")
		OPTION (U"end time")
	REAL (U"To time (s)", U"0.0")
	OK2
DO
	int shift = GET_INTEGER (U"Shift");
	LOOP {
		iam (Function);
		Function_shiftXTo (me, shift == 1 ? my xmin : shift == 2 ? 0.5 * (my xmin + my xmax) : my xmax, GET_REAL (U"To time"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (TimeFunction_shiftToZero) {
	LOOP {
		iam (Function);
		Function_shiftXTo (me, my xmin, 0.0);
		praat_dataChanged (me);
	}
END2 }

/***** TIMETIER *****/

FORM (TimeTier_getHighIndexFromTime, U"Get high index", U"AnyTier: Get high index from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points.size() == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToHighIndex (me, GET_REAL (U"Time"))));
	}
END2 }

FORM (TimeTier_getLowIndexFromTime, U"Get low index", U"AnyTier: Get low index from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points.size() == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToLowIndex (me, GET_REAL (U"Time"))));
	}
END2 }

FORM (TimeTier_getNearestIndexFromTime, U"Get nearest index", U"AnyTier: Get nearest index from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (AnyTier);
		Melder_information (my points.size() == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToNearestIndex (me, GET_REAL (U"Time"))));
	}
END2 }

DIRECT2 (TimeTier_getNumberOfPoints) {
	LOOP {
		iam (AnyTier);
		Melder_information (my points.size(), U" points");
	}
END2 }

FORM (TimeTier_getTimeFromIndex, U"Get time", nullptr /*"AnyTier: Get time from index..."*/) {
	NATURAL (U"Point number", U"10")
	OK2
DO
	LOOP {
		iam (AnyTier);
		long i = GET_INTEGER (U"Point number");
		if (i > my points.size()) Melder_information (U"--undefined--");
		else Melder_informationReal (my points [i] -> number, U"seconds");
	}
END2 }

FORM (TimeTier_removePoint, U"Remove one point", U"AnyTier: Remove point...") {
	NATURAL (U"Point number", U"1")
	OK2
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePoint (me, GET_INTEGER (U"Point number"));
		praat_dataChanged (me);
	}
END2 }

FORM (TimeTier_removePointNear, U"Remove one point", U"AnyTier: Remove point near...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePointNear (me, GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END2 }

FORM (TimeTier_removePointsBetween, U"Remove points", U"AnyTier: Remove points between...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	OK2
DO
	LOOP {
		iam (AnyTier);
		AnyTier_removePointsBetween (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		praat_dataChanged (me);
	}
END2 }

/***** TRANSITION *****/

DIRECT2 (Transition_conflate) {
	LOOP {
		iam (Transition);
		autoDistributions thee = Transition_to_Distributions_conflate (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Transition_drawAsNumbers, U"Draw as numbers", nullptr) {
	RADIO (U"Format", 1)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"2")
	OK2
DO
	LOOP {
		iam (Transition);
		autoPraatPicture picture;
		Transition_drawAsNumbers (me, GRAPHICS, GET_INTEGER (U"Format"), GET_INTEGER (U"Precision"));
	}
END2 }

DIRECT2 (Transition_eigen) {
	LOOP {
		iam (Transition);
		autoMatrix vectors, values;
		Transition_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	}
END2 }

DIRECT2 (Transition_help) {
	Melder_help (U"Transition");
END2 }

FORM (Transition_power, U"Transition: Power...", nullptr) {
	NATURAL (U"Power", U"2")
	OK2
DO
	LOOP {
		iam (Transition);
		autoTransition thee = Transition_power (me, GET_INTEGER (U"Power"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Transition_to_Matrix) {
	LOOP {
		iam (Transition);
		autoMatrix thee = Transition_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** Praat menu *****/

FORM (Praat_test, U"Praat test", 0) {
	OPTIONMENU_ENUM (U"Test", kPraatTests, DEFAULT)
	SENTENCE (U"arg1", U"1000000")
	SENTENCE (U"arg2", U"")
	SENTENCE (U"arg3", U"")
	SENTENCE (U"arg4", U"")
	OK2
DO
	Praat_tests (GET_ENUM (kPraatTests, U"Test"), GET_STRING (U"arg1"),
		GET_STRING (U"arg2"), GET_STRING (U"arg3"), GET_STRING (U"arg4"));
END2 }

/***** Help menu *****/

DIRECT (ObjectWindow) Melder_help (U"Object window"); END
DIRECT (Intro) Melder_help (U"Intro"); END
DIRECT (WhatsNew) Melder_help (U"What's new?"); END
DIRECT (TypesOfObjects) Melder_help (U"Types of objects"); END
DIRECT (Editors) Melder_help (U"Editors"); END
DIRECT (FrequentlyAskedQuestions) Melder_help (U"FAQ (Frequently Asked Questions)"); END
DIRECT (Acknowledgments) Melder_help (U"Acknowledgments"); END
DIRECT (FormulasTutorial) Melder_help (U"Formulas"); END
DIRECT (ScriptingTutorial) Melder_help (U"Scripting"); END
DIRECT (DemoWindow) Melder_help (U"Demo window"); END
DIRECT (Interoperability) Melder_help (U"Interoperability"); END
DIRECT (Programming) Melder_help (U"Programming with Praat"); END
DIRECT (SearchManual) Melder_search (); END

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

static autoDaata imageFileRecognizer (int /* nread */, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	if (Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".jpg") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".JPG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".jpeg") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".JPEG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".png") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".PNG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".tiff") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".TIFF") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".tif") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".TIF"))
	{
		return Photo_readFromImageFile (file);
	}
	return autoDaata ();
}

/***** buttons *****/

void praat_TableOfReal_init (ClassInfo klas);   // Buttons for TableOfReal and for its subclasses.

void praat_TimeFunction_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of Function.
void praat_TimeFunction_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Query time domain", nullptr, 1, nullptr);
	praat_addAction1 (klas, 1, U"Get start time", nullptr, 2, DO_TimeFunction_getStartTime);
						praat_addAction1 (klas, 1, U"Get starting time", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getStartTime);
	praat_addAction1 (klas, 1, U"Get end time", nullptr, 2, DO_TimeFunction_getEndTime);
						praat_addAction1 (klas, 1, U"Get finishing time", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getEndTime);
	praat_addAction1 (klas, 1, U"Get total duration", nullptr, 2, DO_TimeFunction_getDuration);
						praat_addAction1 (klas, 1, U"Get duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_getDuration);
}

void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time-based subclasses of Function.
void praat_TimeFunction_modify_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Modify times", nullptr, 1, nullptr);
	praat_addAction1 (klas, 0, U"Shift times by...", nullptr, 2, DO_TimeFunction_shiftTimesBy);
	praat_addAction1 (klas, 0, U"Shift times to...", nullptr, 2, DO_TimeFunction_shiftTimesTo);
						praat_addAction1 (klas, 0, U"Shift to zero", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_shiftToZero);   // hidden 2008
	praat_addAction1 (klas, 0, U"Scale times by...", nullptr, 2, DO_TimeFunction_scaleTimesBy);
	praat_addAction1 (klas, 0, U"Scale times to...", nullptr, 2, DO_TimeFunction_scaleTimesTo);
						praat_addAction1 (klas, 0, U"Scale times...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFunction_scaleTimesTo);   // hidden 2008
}

void praat_TimeFrameSampled_query_init (ClassInfo klas);   // Query buttons for frame-based time-based subclasses of Sampled.
void praat_TimeFrameSampled_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, U"Query time sampling", nullptr, 1, nullptr);
	praat_addAction1 (klas, 1, U"Get number of frames", nullptr, 2, DO_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, U"Get time step", nullptr, 2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, U"Get frame length", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
						praat_addAction1 (klas, 1, U"Get frame duration", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, U"Get time from frame number...", nullptr, 2, DO_TimeFrameSampled_getTimeFromFrame);
						praat_addAction1 (klas, 1, U"Get time from frame...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, U"Get frame number from time...", nullptr, 2, DO_TimeFrameSampled_getFrameFromTime);
						praat_addAction1 (klas, 1, U"Get frame from time...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TimeFrameSampled_getFrameFromTime);
}

void praat_TimeTier_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of AnyTier.
void praat_TimeTier_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, U"Get number of points", nullptr, 1, DO_TimeTier_getNumberOfPoints);
	praat_addAction1 (klas, 1, U"Get low index from time...", nullptr, 1, DO_TimeTier_getLowIndexFromTime);
	praat_addAction1 (klas, 1, U"Get high index from time...", nullptr, 1, DO_TimeTier_getHighIndexFromTime);
	praat_addAction1 (klas, 1, U"Get nearest index from time...", nullptr, 1, DO_TimeTier_getNearestIndexFromTime);
	praat_addAction1 (klas, 1, U"Get time from index...", nullptr, 1, DO_TimeTier_getTimeFromIndex);
}

void praat_TimeTier_modify_init (ClassInfo klas);   // Modification buttons for time-based subclasses of AnyTier.
void praat_TimeTier_modify_init (ClassInfo klas) {
	praat_TimeFunction_modify_init (klas);
	praat_addAction1 (klas, 0, U"Remove point...", nullptr, 1, DO_TimeTier_removePoint);
	praat_addAction1 (klas, 0, U"Remove point near...", nullptr, 1, DO_TimeTier_removePointNear);
	praat_addAction1 (klas, 0, U"Remove points between...", nullptr, 1, DO_TimeTier_removePointsBetween);
}

void praat_uvafon_init ();
void praat_uvafon_init () {
	Thing_recognizeClassesByName (classSound, classMatrix, classPolygon, classPointProcess, classParamCurve,
		classSpectrum, classLtas, classSpectrogram, classFormant,
		classExcitation, classCochleagram, classVocalTract, classFormantPoint, classFormantTier, classFormantGrid,
		classLabel, classTier, classAutosegment,   // three obsolete classes
		classIntensity, classPitch, classHarmonicity,
		classTransition,
		classRealPoint, classRealTier, classPitchTier, classIntensityTier, classDurationTier, classAmplitudeTier, classSpectrumTier,
		classManipulation, classTextPoint, classTextInterval, classTextTier,
		classIntervalTier, classTextGrid, classLongSound, classWordList, classSpellingChecker,
		classMovie, classCorpus, classPhoto,
		nullptr);
	Thing_recognizeClassByOtherName (classManipulation, U"Psola");
	Thing_recognizeClassByOtherName (classManipulation, U"Analysis");
	Thing_recognizeClassByOtherName (classPitchTier, U"StylPitch");

	Data_recognizeFileType (cgnSyntaxFileRecognizer);
	Data_recognizeFileType (chronologicalTextGridTextFileRecognizer);
	Data_recognizeFileType (imageFileRecognizer);

	structManipulationEditor :: f_preferences ();
	structSpectrumEditor     :: f_preferences ();
	structFormantGridEditor  :: f_preferences ();

	INCLUDE_LIBRARY (praat_uvafon_Sound_init)
	INCLUDE_LIBRARY (praat_uvafon_TextGrid_init)

	praat_addMenuCommand (U"Objects", U"Technical", U"Praat test...", nullptr, 0, DO_Praat_test);

	praat_addMenuCommand (U"Objects", U"New", U"-- new numerics --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Matrix", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Matrix...", nullptr, 1, DO_Matrix_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Matrix...", nullptr, 1, DO_Matrix_createSimple);
		praat_addMenuCommand (U"Objects", U"New", U"-- colour matrix --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Photo...", nullptr, 1, DO_Photo_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Photo...", nullptr, 1, DO_Photo_createSimple);
	praat_addMenuCommand (U"Objects", U"Open", U"-- read movie --", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open movie file...", nullptr, praat_HIDDEN, DO_Movie_openFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"-- read raw --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from raw text file...", nullptr, 0, DO_Matrix_readFromRawTextFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from LVS AP file...", nullptr, praat_HIDDEN, DO_Matrix_readAP);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Strings from raw text file...", nullptr, 0, DO_Strings_readFromRawTextFile);

	INCLUDE_LIBRARY (praat_uvafon_stat_init)

	praat_addMenuCommand (U"Objects", U"New", U"Tiers", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty PointProcess...", nullptr, 1, DO_PointProcess_createEmpty);
		praat_addMenuCommand (U"Objects", U"New", U"Create Poisson process...", nullptr, 1, DO_PointProcess_createPoissonProcess);
		praat_addMenuCommand (U"Objects", U"New", U"-- new tiers ---", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create PitchTier...", nullptr, 1, DO_PitchTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantGrid...", nullptr, 1, DO_FormantGrid_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantTier...", nullptr, praat_HIDDEN | praat_DEPTH_1, DO_FormantTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create IntensityTier...", nullptr, 1, DO_IntensityTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create DurationTier...", nullptr, 1, DO_DurationTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create AmplitudeTier...", nullptr, 1, DO_AmplitudeTier_create);
	praat_addMenuCommand (U"Objects", U"New", U"-- new textgrid --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create TextGrid...", nullptr, 0, DO_TextGrid_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Corpus...", nullptr, praat_HIDDEN, DO_Corpus_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as file list...", nullptr, 0, DO_Strings_createAsFileList);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as directory list...", nullptr, 0, DO_Strings_createAsDirectoryList);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read tier --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special tier file...", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read TextTier from Xwaves...", nullptr, 1, DO_TextTier_readFromXwaves);
		praat_addMenuCommand (U"Objects", U"Open", U"Read IntervalTier from Xwaves...", nullptr, 1, DO_IntervalTier_readFromXwaves);

	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Praat Intro", nullptr, '?', DO_Intro);
	#ifndef macintosh
		praat_addMenuCommand (U"Objects", U"Help", U"Object window", nullptr, 0, DO_ObjectWindow);
	#endif
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Frequently asked questions", nullptr, 0, DO_FrequentlyAskedQuestions);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"What's new?", nullptr, 0, DO_WhatsNew);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Types of objects", nullptr, 0, DO_TypesOfObjects);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Editors", nullptr, 0, DO_Editors);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Acknowledgments", nullptr, 0, DO_Acknowledgments);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- shell help --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Formulas tutorial", nullptr, 0, DO_FormulasTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Scripting tutorial", nullptr, 0, DO_ScriptingTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Demo window", nullptr, 0, DO_DemoWindow);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Interoperability", nullptr, 0, DO_Interoperability);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Programming", nullptr, 0, DO_Programming);
	#ifdef macintosh
		praat_addMenuCommand (U"Objects", U"Help", U"Praat Intro", nullptr, '?', DO_Intro);
		praat_addMenuCommand (U"Objects", U"Help", U"Object window help", nullptr, 0, DO_ObjectWindow);
		praat_addMenuCommand (U"Objects", U"Help", U"-- manual --", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Help", U"Search Praat manual...", nullptr, 'M', DO_SearchManual);
	#endif

	praat_addAction1 (classAmplitudeTier, 0, U"AmplitudeTier help", nullptr, 0, DO_AmplitudeTier_help);
	praat_addAction1 (classAmplitudeTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_AmplitudeTier_edit);
	praat_addAction1 (classAmplitudeTier, 1, U"Edit", nullptr, praat_HIDDEN, DO_AmplitudeTier_edit);
	praat_addAction1 (classAmplitudeTier, 0, U"View & Edit with Sound?", 0, 0, DO_info_AmplitudeTier_Sound_edit);
	praat_addAction1 (classAmplitudeTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local)...", nullptr, 1, DO_AmplitudeTier_getShimmer_local);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local_dB)...", nullptr, 1, DO_AmplitudeTier_getShimmer_local_dB);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq3)...", nullptr, 1, DO_AmplitudeTier_getShimmer_apq3);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq5)...", nullptr, 1, DO_AmplitudeTier_getShimmer_apq5);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq11)...", nullptr, 1, DO_AmplitudeTier_getShimmer_apq11);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (dda)...", nullptr, 1, DO_AmplitudeTier_getShimmer_dda);
	praat_addAction1 (classAmplitudeTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 0, U"Add point...", nullptr, 1, DO_AmplitudeTier_addPoint);
		praat_addAction1 (classAmplitudeTier, 0, U"Formula...", nullptr, 1, DO_AmplitudeTier_formula);
praat_addAction1 (classAmplitudeTier, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classAmplitudeTier, 0, U"To Sound (pulse train)...", nullptr, 0, DO_AmplitudeTier_to_Sound);
praat_addAction1 (classAmplitudeTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classAmplitudeTier, 0, U"To IntensityTier...", nullptr, 0, DO_AmplitudeTier_to_IntensityTier);
	praat_addAction1 (classAmplitudeTier, 0, U"Down to PointProcess", nullptr, 0, DO_AmplitudeTier_downto_PointProcess);
	praat_addAction1 (classAmplitudeTier, 0, U"Down to TableOfReal", nullptr, 0, DO_AmplitudeTier_downto_TableOfReal);

	praat_addAction1 (classCochleagram, 0, U"Cochleagram help", nullptr, 0, DO_Cochleagram_help);
	praat_addAction1 (classCochleagram, 1, U"Movie", nullptr, 0, DO_Cochleagram_movie);
praat_addAction1 (classCochleagram, 0, U"Info", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 2, U"Difference...", nullptr, 0, DO_Cochleagram_difference);
praat_addAction1 (classCochleagram, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Paint...", nullptr, 0, DO_Cochleagram_paint);
praat_addAction1 (classCochleagram, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Formula...", nullptr, 0, DO_Cochleagram_formula);
praat_addAction1 (classCochleagram, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Excitation (slice)...", nullptr, 0, DO_Cochleagram_to_Excitation);
praat_addAction1 (classCochleagram, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Matrix", nullptr, 0, DO_Cochleagram_to_Matrix);

	praat_addAction1 (classCorpus, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Corpus_edit);

praat_addAction1 (classDistributions, 0, U"Learn", nullptr, 0, nullptr);
	praat_addAction1 (classDistributions, 1, U"To Transition...", nullptr, 0, DO_Distributions_to_Transition);
	praat_addAction1 (classDistributions, 2, U"To Transition (noise)...", nullptr, 0, DO_Distributions_to_Transition_noise);

	praat_addAction1 (classDurationTier, 0, U"DurationTier help", nullptr, 0, DO_DurationTier_help);
	praat_addAction1 (classDurationTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_DurationTier_edit);
	praat_addAction1 (classDurationTier, 1, U"Edit", nullptr, praat_HIDDEN, DO_DurationTier_edit);
	praat_addAction1 (classDurationTier, 0, U"View & Edit with Sound?", nullptr, 0, DO_info_DurationTier_Sound_edit);
	praat_addAction1 (classDurationTier, 0, U"& Manipulation: Replace?", nullptr, 0, DO_info_DurationTier_Manipulation_replace);
	praat_addAction1 (classDurationTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classDurationTier);
		praat_addAction1 (classDurationTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classDurationTier, 1, U"Get value at time...", nullptr, 1, DO_DurationTier_getValueAtTime);
		praat_addAction1 (classDurationTier, 1, U"Get value at index...", nullptr, 1, DO_DurationTier_getValueAtIndex);
		praat_addAction1 (classDurationTier, 1, U"Get target duration...", nullptr, 1, DO_DurationTier_getTargetDuration);
	praat_addAction1 (classDurationTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classDurationTier);
		praat_addAction1 (classDurationTier, 0, U"Add point...", nullptr, 1, DO_DurationTier_addPoint);
		praat_addAction1 (classDurationTier, 0, U"Formula...", nullptr, 1, DO_DurationTier_formula);
praat_addAction1 (classDurationTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classDurationTier, 0, U"Down to PointProcess", nullptr, 0, DO_DurationTier_downto_PointProcess);

	praat_addAction1 (classExcitation, 0, U"Excitation help", nullptr, 0, DO_Excitation_help);
praat_addAction1 (classExcitation, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Draw...", nullptr, 0, DO_Excitation_draw);
praat_addAction1 (classExcitation, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Formant...", nullptr, 0, DO_Excitation_to_Formant);
praat_addAction1 (classExcitation, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 1, U"Get loudness", nullptr, 0, DO_Excitation_getLoudness);
praat_addAction1 (classExcitation, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Formula...", nullptr, 0, DO_Excitation_formula);
praat_addAction1 (classExcitation, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Matrix", nullptr, 0, DO_Excitation_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Formant help", nullptr, 0, DO_Formant_help);
	praat_addAction1 (classFormant, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 0, U"Speckle...", nullptr, 1, DO_Formant_drawSpeckles);
		praat_addAction1 (classFormant, 0, U"Draw tracks...", nullptr, 1, DO_Formant_drawTracks);
		praat_addAction1 (classFormant, 0, U"Scatter plot...", nullptr, 1, DO_Formant_scatterPlot);
	praat_addAction1 (classFormant, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 1, U"List...", nullptr, 1, DO_Formant_list);
		praat_addAction1 (classFormant, 0, U"Down to Table...", nullptr, 1, DO_Formant_downto_Table);
	praat_addAction1 (classFormant, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classFormant);
		praat_addAction1 (classFormant, 1, U"Get number of formants...", nullptr, 1, DO_Formant_getNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get minimum number of formants", nullptr, 1, DO_Formant_getMinimumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get maximum number of formants", nullptr, 1, DO_Formant_getMaximumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get value at time...", nullptr, 1, DO_Formant_getValueAtTime);
		praat_addAction1 (classFormant, 1, U"Get bandwidth at time...", nullptr, 1, DO_Formant_getBandwidthAtTime);
		praat_addAction1 (classFormant, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get minimum...", nullptr, 1, DO_Formant_getMinimum);
		praat_addAction1 (classFormant, 1, U"Get time of minimum...", nullptr, 1, DO_Formant_getTimeOfMinimum);
		praat_addAction1 (classFormant, 1, U"Get maximum...", nullptr, 1, DO_Formant_getMaximum);
		praat_addAction1 (classFormant, 1, U"Get time of maximum...", nullptr, 1, DO_Formant_getTimeOfMaximum);
		praat_addAction1 (classFormant, 1, U"-- get distribution --", nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get quantile...", nullptr, 1, DO_Formant_getQuantile);
		praat_addAction1 (classFormant, 1, U"Get quantile of bandwidth...", nullptr, 1, DO_Formant_getQuantileOfBandwidth);
		praat_addAction1 (classFormant, 1, U"Get mean...", nullptr, 1, DO_Formant_getMean);
		praat_addAction1 (classFormant, 1, U"Get standard deviation...", nullptr, 1, DO_Formant_getStandardDeviation);
	praat_addAction1 (classFormant, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormant);
		praat_addAction1 (classFormant, 0, U"Sort", nullptr, 1, DO_Formant_sort);
		praat_addAction1 (classFormant, 0, U"Formula (frequencies)...", nullptr, 1, DO_Formant_formula_frequencies);
		praat_addAction1 (classFormant, 0, U"Formula (bandwidths)...", nullptr, 1, DO_Formant_formula_bandwidths);
praat_addAction1 (classFormant, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"Track...", nullptr, 0, DO_Formant_tracker);
	praat_addAction1 (classFormant, 0, U"Down to FormantTier", nullptr, praat_HIDDEN, DO_Formant_downto_FormantTier);
	praat_addAction1 (classFormant, 0, U"Down to FormantGrid", nullptr, 0, DO_Formant_downto_FormantGrid);
praat_addAction1 (classFormant, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"To Matrix...", nullptr, 0, DO_Formant_to_Matrix);

	praat_addAction1 (classFormantGrid, 0, U"FormantGrid help", nullptr, 0, DO_FormantGrid_help);
	praat_addAction1 (classFormantGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormantGrid);
		praat_addAction1 (classFormantGrid, 0, U"Formula (frequencies)...", nullptr, 1, DO_FormantGrid_formula_frequencies);
		//praat_addAction1 (classFormantGrid, 0, U"Formula (bandwidths)...", nullptr, 1, DO_FormantGrid_formula_bandwidths);
		praat_addAction1 (classFormantGrid, 0, U"Add formant point...", nullptr, 1, DO_FormantGrid_addFormantPoint);
		praat_addAction1 (classFormantGrid, 0, U"Add bandwidth point...", nullptr, 1, DO_FormantGrid_addBandwidthPoint);
		praat_addAction1 (classFormantGrid, 0, U"Remove formant points between...", nullptr, 1, DO_FormantGrid_removeFormantPointsBetween);
		praat_addAction1 (classFormantGrid, 0, U"Remove bandwidth points between...", nullptr, 1, DO_FormantGrid_removeBandwidthPointsBetween);
	praat_addAction1 (classFormantGrid, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantGrid, 0, U"To Formant...", nullptr, 1, DO_FormantGrid_to_Formant);

	praat_addAction1 (classFormantTier, 0, U"FormantTier help", nullptr, 0, DO_FormantTier_help);
	praat_addAction1 (classFormantTier, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantTier, 0, U"Speckle...", nullptr, 1, DO_FormantTier_speckle);
	praat_addAction1 (classFormantTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classFormantTier);
		praat_addAction1 (classFormantTier, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classFormantTier, 1, U"Get value at time...", nullptr, 1, DO_FormantTier_getValueAtTime);
		praat_addAction1 (classFormantTier, 1, U"Get bandwidth at time...", nullptr, 1, DO_FormantTier_getBandwidthAtTime);
	praat_addAction1 (classFormantTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classFormantTier);
		praat_addAction1 (classFormantTier, 0, U"Add point...", nullptr, 1, DO_FormantTier_addPoint);
praat_addAction1 (classFormantTier, 0, U"Down", nullptr, 0, nullptr);
	praat_addAction1 (classFormantTier, 0, U"Down to TableOfReal...", nullptr, 0, DO_FormantTier_downto_TableOfReal);

	praat_addAction1 (classHarmonicity, 0, U"Harmonicity help", nullptr, 0, DO_Harmonicity_help);
	praat_addAction1 (classHarmonicity, 0, U"Draw", nullptr, 0, nullptr);
		praat_addAction1 (classHarmonicity, 0, U"Draw...", nullptr, 0, DO_Harmonicity_draw);
	praat_addAction1 (classHarmonicity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get value at time...", nullptr, 1, DO_Harmonicity_getValueAtTime);
		praat_addAction1 (classHarmonicity, 1, U"Get value in frame...", nullptr, 1, DO_Harmonicity_getValueInFrame);
		praat_addAction1 (classHarmonicity, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get minimum...", nullptr, 1, DO_Harmonicity_getMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of minimum...", nullptr, 1, DO_Harmonicity_getTimeOfMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get maximum...", nullptr, 1, DO_Harmonicity_getMaximum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of maximum...", nullptr, 1, DO_Harmonicity_getTimeOfMaximum);
		praat_addAction1 (classHarmonicity, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get mean...", nullptr, 1, DO_Harmonicity_getMean);
		praat_addAction1 (classHarmonicity, 1, U"Get standard deviation...", nullptr, 1, DO_Harmonicity_getStandardDeviation);
	praat_addAction1 (classHarmonicity, 0, U"Modify", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 0, U"Formula...", nullptr, 0, DO_Harmonicity_formula);
	praat_addAction1 (classHarmonicity, 0, U"Hack", nullptr, 0, nullptr);
		praat_addAction1 (classHarmonicity, 0, U"To Matrix", nullptr, 0, DO_Harmonicity_to_Matrix);

	praat_addAction1 (classIntensity, 0, U"Intensity help", nullptr, 0, DO_Intensity_help);
	praat_addAction1 (classIntensity, 0, U"Draw...", nullptr, 0, DO_Intensity_draw);
	praat_addAction1 (classIntensity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classIntensity);
		praat_addAction1 (classIntensity, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get value at time...", nullptr, 1, DO_Intensity_getValueAtTime);
		praat_addAction1 (classIntensity, 1, U"Get value in frame...", nullptr, 1, DO_Intensity_getValueInFrame);
		praat_addAction1 (classIntensity, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get minimum...", nullptr, 1, DO_Intensity_getMinimum);
		praat_addAction1 (classIntensity, 1, U"Get time of minimum...", nullptr, 1, DO_Intensity_getTimeOfMinimum);
		praat_addAction1 (classIntensity, 1, U"Get maximum...", nullptr, 1, DO_Intensity_getMaximum);
		praat_addAction1 (classIntensity, 1, U"Get time of maximum...", nullptr, 1, DO_Intensity_getTimeOfMaximum);
		praat_addAction1 (classIntensity, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get quantile...", nullptr, 1, DO_Intensity_getQuantile);
		praat_addAction1 (classIntensity, 1, U"Get mean...", nullptr, 1, DO_Intensity_getMean);
		praat_addAction1 (classIntensity, 1, U"Get standard deviation...", nullptr, 1, DO_Intensity_getStandardDeviation);
	praat_addAction1 (classIntensity, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classIntensity);
		praat_addAction1 (classIntensity, 0, U"Formula...", nullptr, 1, DO_Intensity_formula);
	praat_addAction1 (classIntensity, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classIntensity, 0, U"To IntensityTier (peaks)", nullptr, 0, DO_Intensity_to_IntensityTier_peaks);
		praat_addAction1 (classIntensity, 0, U"To IntensityTier (valleys)", nullptr, 0, DO_Intensity_to_IntensityTier_valleys);
	praat_addAction1 (classIntensity, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classIntensity, 0, U"Down to IntensityTier", nullptr, 0, DO_Intensity_downto_IntensityTier);
		praat_addAction1 (classIntensity, 0, U"Down to Matrix", nullptr, 0, DO_Intensity_downto_Matrix);

	praat_addAction1 (classIntensityTier, 0, U"IntensityTier help", nullptr, 0, DO_IntensityTier_help);
	praat_addAction1 (classIntensityTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_IntensityTier_edit);
	praat_addAction1 (classIntensityTier, 1, U"Edit", nullptr, praat_HIDDEN, DO_IntensityTier_edit);
	praat_addAction1 (classIntensityTier, 0, U"View & Edit with Sound?", nullptr, 0, DO_info_IntensityTier_Sound_edit);
	praat_addAction1 (classIntensityTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensityTier, 1, U"Get value at time...", nullptr, 1, DO_IntensityTier_getValueAtTime);
		praat_addAction1 (classIntensityTier, 1, U"Get value at index...", nullptr, 1, DO_IntensityTier_getValueAtIndex);
	praat_addAction1 (classIntensityTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 0, U"Add point...", nullptr, 1, DO_IntensityTier_addPoint);
		praat_addAction1 (classIntensityTier, 0, U"Formula...", nullptr, 1, DO_IntensityTier_formula);
praat_addAction1 (classIntensityTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classIntensityTier, 0, U"To AmplitudeTier", nullptr, 0, DO_IntensityTier_to_AmplitudeTier);
	praat_addAction1 (classIntensityTier, 0, U"Down to PointProcess", nullptr, 0, DO_IntensityTier_downto_PointProcess);
	praat_addAction1 (classIntensityTier, 0, U"Down to TableOfReal", nullptr, 0, DO_IntensityTier_downto_TableOfReal);

	praat_addAction1 (classLtas, 0, U"Ltas help", nullptr, 0, DO_Ltas_help);
	praat_addAction1 (classLtas, 0, U"Draw...", nullptr, 0, DO_Ltas_draw);
	praat_addAction1 (classLtas, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classLtas, 1, U"Frequency domain", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get lowest frequency", nullptr, 2, DO_Ltas_getLowestFrequency);
		praat_addAction1 (classLtas, 1, U"Get highest frequency", nullptr, 2, DO_Ltas_getHighestFrequency);
		praat_addAction1 (classLtas, 1, U"Frequency sampling", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get number of bins", nullptr, 2, DO_Ltas_getNumberOfBins);
			praat_addAction1 (classLtas, 1, U"Get number of bands", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getNumberOfBins);
		praat_addAction1 (classLtas, 1, U"Get bin width", nullptr, 2, DO_Ltas_getBinWidth);
			praat_addAction1 (classLtas, 1, U"Get band width", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinWidth);
		praat_addAction1 (classLtas, 1, U"Get frequency from bin number...", nullptr, 2, DO_Ltas_getFrequencyFromBinNumber);
			praat_addAction1 (classLtas, 1, U"Get frequency from band...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getFrequencyFromBinNumber);
		praat_addAction1 (classLtas, 1, U"Get bin number from frequency...", nullptr, 2, DO_Ltas_getBinNumberFromFrequency);
			praat_addAction1 (classLtas, 1, U"Get band from frequency...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Ltas_getBinNumberFromFrequency);
		praat_addAction1 (classLtas, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get value at frequency...", nullptr, 1, DO_Ltas_getValueAtFrequency);
		praat_addAction1 (classLtas, 1, U"Get value in bin...", nullptr, 1, DO_Ltas_getValueInBin);
			praat_addAction1 (classLtas, 1, U"Get value in band...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Ltas_getValueInBin);
		praat_addAction1 (classLtas, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get minimum...", nullptr, 1, DO_Ltas_getMinimum);
		praat_addAction1 (classLtas, 1, U"Get frequency of minimum...", nullptr, 1, DO_Ltas_getFrequencyOfMinimum);
		praat_addAction1 (classLtas, 1, U"Get maximum...", nullptr, 1, DO_Ltas_getMaximum);
		praat_addAction1 (classLtas, 1, U"Get frequency of maximum...", nullptr, 1, DO_Ltas_getFrequencyOfMaximum);
		praat_addAction1 (classLtas, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get mean...", nullptr, 1, DO_Ltas_getMean);
		praat_addAction1 (classLtas, 1, U"Get slope...", nullptr, 1, DO_Ltas_getSlope);
		praat_addAction1 (classLtas, 1, U"Get local peak height...", nullptr, 1, DO_Ltas_getLocalPeakHeight);
		praat_addAction1 (classLtas, 1, U"Get standard deviation...", nullptr, 1, DO_Ltas_getStandardDeviation);
	praat_addAction1 (classLtas, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Formula...", nullptr, 0, DO_Ltas_formula);
	praat_addAction1 (classLtas, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To SpectrumTier (peaks)", nullptr, 0, DO_Ltas_to_SpectrumTier_peaks);
	praat_addAction1 (classLtas, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Compute trend line...", nullptr, 0, DO_Ltas_computeTrendLine);
	praat_addAction1 (classLtas, 0, U"Subtract trend line...", nullptr, 0, DO_Ltas_subtractTrendLine);
	praat_addAction1 (classLtas, 0, U"Combine", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Merge", nullptr, praat_HIDDEN, DO_Ltases_merge);
	praat_addAction1 (classLtas, 0, U"Average", nullptr, 0, DO_Ltases_average);
	praat_addAction1 (classLtas, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To Matrix", nullptr, 0, DO_Ltas_to_Matrix);

	praat_addAction1 (classManipulation, 0, U"Manipulation help", nullptr, 0, DO_Manipulation_help);
	praat_addAction1 (classManipulation, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Manipulation_edit);
	praat_addAction1 (classManipulation, 1, U"Edit", nullptr, praat_HIDDEN, DO_Manipulation_edit);
	praat_addAction1 (classManipulation, 0, U"Play (overlap-add)", nullptr, 0, DO_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Play (PSOLA)", nullptr, praat_HIDDEN, DO_Manipulation_play_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Play (LPC)", nullptr, 0, DO_Manipulation_play_lpc);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (overlap-add)", nullptr, 0, DO_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (PSOLA)", nullptr, praat_HIDDEN, DO_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (LPC)", nullptr, 0, DO_Manipulation_getResynthesis_lpc);
	praat_addAction1 (classManipulation, 0, U"Extract original sound", nullptr, 0, DO_Manipulation_extractOriginalSound);
	praat_addAction1 (classManipulation, 0, U"Extract pulses", nullptr, 0, DO_Manipulation_extractPulses);
	praat_addAction1 (classManipulation, 0, U"Extract pitch tier", nullptr, 0, DO_Manipulation_extractPitchTier);
	praat_addAction1 (classManipulation, 0, U"Extract duration tier", nullptr, 0, DO_Manipulation_extractDurationTier);
	praat_addAction1 (classManipulation, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classManipulation);
		praat_addAction1 (classManipulation, 0, U"Replace pitch tier?", nullptr, 1, DO_Manipulation_replacePitchTier_help);
		praat_addAction1 (classManipulation, 0, U"Replace duration tier?", nullptr, 1, DO_Manipulation_replaceDurationTier_help);
	praat_addAction1 (classManipulation, 1, U"Save as text file without Sound...", nullptr, 0, DO_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, U"Write to text file without Sound...", nullptr, praat_HIDDEN, DO_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, U"Save as binary file without Sound...", nullptr, 0, DO_Manipulation_writeToBinaryFileWithoutSound);
	praat_addAction1 (classManipulation, 1, U"Write to binary file without Sound...", nullptr, praat_HIDDEN, DO_Manipulation_writeToBinaryFileWithoutSound);

	praat_addAction1 (classMatrix, 0, U"Matrix help", nullptr, 0, DO_Matrix_help);
	praat_addAction1 (classMatrix, 1, U"Save as matrix text file...", nullptr, 0, DO_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, U"Write to matrix text file...", nullptr, praat_HIDDEN, DO_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, U"Save as headerless spreadsheet file...", nullptr, 0, DO_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, U"Write to headerless spreadsheet file...", nullptr, praat_HIDDEN, DO_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, U"Play movie", nullptr, 0, DO_Matrix_movie);
	praat_addAction1 (classMatrix, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"Draw rows...", nullptr, 1, DO_Matrix_drawRows);
		praat_addAction1 (classMatrix, 0, U"Draw one contour...", nullptr, 1, DO_Matrix_drawOneContour);
		praat_addAction1 (classMatrix, 0, U"Draw contours...", nullptr, 1, DO_Matrix_drawContours);
		praat_addAction1 (classMatrix, 0, U"Paint image...", nullptr, 1, DO_Matrix_paintImage);
		praat_addAction1 (classMatrix, 0, U"Paint contours...", nullptr, 1, DO_Matrix_paintContours);
		praat_addAction1 (classMatrix, 0, U"Paint cells...", nullptr, 1, DO_Matrix_paintCells);
		praat_addAction1 (classMatrix, 0, U"Paint surface...", nullptr, 1, DO_Matrix_paintSurface);
	praat_addAction1 (classMatrix, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get lowest x", nullptr, 1, DO_Matrix_getLowestX);
		praat_addAction1 (classMatrix, 1, U"Get highest x", nullptr, 1, DO_Matrix_getHighestX);
		praat_addAction1 (classMatrix, 1, U"Get lowest y", nullptr, 1, DO_Matrix_getLowestY);
		praat_addAction1 (classMatrix, 1, U"Get highest y", nullptr, 1, DO_Matrix_getHighestY);
		praat_addAction1 (classMatrix, 1, U"-- get structure --", nullptr, 1, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get number of rows", nullptr, 1, DO_Matrix_getNumberOfRows);
		praat_addAction1 (classMatrix, 1, U"Get number of columns", nullptr, 1, DO_Matrix_getNumberOfColumns);
		praat_addAction1 (classMatrix, 1, U"Get row distance", nullptr, 1, DO_Matrix_getRowDistance);
		praat_addAction1 (classMatrix, 1, U"Get column distance", nullptr, 1, DO_Matrix_getColumnDistance);
		praat_addAction1 (classMatrix, 1, U"Get y of row...", nullptr, 1, DO_Matrix_getYofRow);
		praat_addAction1 (classMatrix, 1, U"Get x of column...", nullptr, 1, DO_Matrix_getXofColumn);
		praat_addAction1 (classMatrix, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get value in cell...", nullptr, 1, DO_Matrix_getValueInCell);
		praat_addAction1 (classMatrix, 1, U"Get value at xy...", nullptr, 1, DO_Matrix_getValueAtXY);
		praat_addAction1 (classMatrix, 1, U"Get minimum", nullptr, 1, DO_Matrix_getMinimum);
		praat_addAction1 (classMatrix, 1, U"Get maximum", nullptr, 1, DO_Matrix_getMaximum);
		praat_addAction1 (classMatrix, 1, U"Get sum", nullptr, 1, DO_Matrix_getSum);
	praat_addAction1 (classMatrix, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"Formula...", nullptr, 1, DO_Matrix_formula);
		praat_addAction1 (classMatrix, 0, U"Set value...", nullptr, 1, DO_Matrix_setValue);
praat_addAction1 (classMatrix, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classMatrix, 0, U"Eigen", nullptr, 0, DO_Matrix_eigen);
	praat_addAction1 (classMatrix, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classMatrix, 0, U"Power...", nullptr, 0, DO_Matrix_power);
	praat_addAction1 (classMatrix, 0, U"Combine two Matrices -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 2, U"Merge (append rows)", nullptr, 1, DO_Matrix_appendRows);
		praat_addAction1 (classMatrix, 2, U"To ParamCurve", nullptr, 1, DO_Matrix_to_ParamCurve);
	praat_addAction1 (classMatrix, 0, U"Cast -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"To Cochleagram", nullptr, 1, DO_Matrix_to_Cochleagram);
		praat_addAction1 (classMatrix, 0, U"To Excitation", nullptr, 1, DO_Matrix_to_Excitation);
		praat_addAction1 (classMatrix, 0, U"To Harmonicity", nullptr, 1, DO_Matrix_to_Harmonicity);
		praat_addAction1 (classMatrix, 0, U"To Intensity", nullptr, 1, DO_Matrix_to_Intensity);
		praat_addAction1 (classMatrix, 0, U"To Ltas", nullptr, 1, DO_Matrix_to_Ltas);
		praat_addAction1 (classMatrix, 0, U"To Pitch", nullptr, 1, DO_Matrix_to_Pitch);
		praat_addAction1 (classMatrix, 0, U"To PointProcess", nullptr, 1, DO_Matrix_to_PointProcess);
		praat_addAction1 (classMatrix, 0, U"To Polygon", nullptr, 1, DO_Matrix_to_Polygon);
		praat_addAction1 (classMatrix, 0, U"To Sound", nullptr, 1, DO_Matrix_to_Sound);
		praat_addAction1 (classMatrix, 0, U"To Sound (slice)...", nullptr, 1, DO_Matrix_to_Sound_mono);
		praat_addAction1 (classMatrix, 0, U"To Spectrogram", nullptr, 1, DO_Matrix_to_Spectrogram);
		praat_addAction1 (classMatrix, 0, U"To TableOfReal", nullptr, 1, DO_Matrix_to_TableOfReal);
		praat_addAction1 (classMatrix, 0, U"To Spectrum", nullptr, 1, DO_Matrix_to_Spectrum);
		praat_addAction1 (classMatrix, 0, U"To Transition", nullptr, 1, DO_Matrix_to_Transition);
		praat_addAction1 (classMatrix, 0, U"To VocalTract", nullptr, 1, DO_Matrix_to_VocalTract);

	praat_addAction1 (classMovie, 1, U"Paint one image...", nullptr, 1, DO_Movie_paintOneImage);
	praat_addAction1 (classMovie, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Movie_viewAndEdit);

	praat_addAction1 (classParamCurve, 0, U"ParamCurve help", nullptr, 0, DO_ParamCurve_help);
	praat_addAction1 (classParamCurve, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classParamCurve, 0, U"Draw...", nullptr, 0, DO_ParamCurve_draw);

	praat_addAction1 (classPhoto, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Paint image...", nullptr, 1, DO_Photo_paintImage);
		praat_addAction1 (classPhoto, 0, U"Paint cells...", nullptr, 1, DO_Photo_paintCells);
	praat_addAction1 (classPhoto, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Formula (red)...", nullptr, 1, DO_Photo_formula_red);
		praat_addAction1 (classPhoto, 0, U"Formula (green)...", nullptr, 1, DO_Photo_formula_green);
		praat_addAction1 (classPhoto, 0, U"Formula (blue)...", nullptr, 1, DO_Photo_formula_blue);
		praat_addAction1 (classPhoto, 0, U"Formula (transparency)...", nullptr, 1, DO_Photo_formula_transparency);
	praat_addAction1 (classPhoto, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Extract red", nullptr, 1, DO_Photo_extractRed);
		praat_addAction1 (classPhoto, 0, U"Extract green", nullptr, 1, DO_Photo_extractGreen);
		praat_addAction1 (classPhoto, 0, U"Extract blue", nullptr, 1, DO_Photo_extractBlue);
		praat_addAction1 (classPhoto, 0, U"Extract transparency", nullptr, 1, DO_Photo_extractTransparency);
	praat_addAction1 (classPhoto, 1, U"Save as PNG file...", nullptr, 0, DO_Photo_saveAsPNG);
	#if defined (macintosh) || defined (_WIN32)
		praat_addAction1 (classPhoto, 1, U"Save as TIFF file...", nullptr, 0, DO_Photo_saveAsTIFF);
		praat_addAction1 (classPhoto, 1, U"Save as GIF file...", nullptr, 0, DO_Photo_saveAsGIF);
		praat_addAction1 (classPhoto, 1, U"Save as Windows bitmap file...", nullptr, 0, DO_Photo_saveAsWindowsBitmapFile);
		praat_addAction1 (classPhoto, 1, U"Save as lossy JPEG file...", nullptr, 0, DO_Photo_saveAsJPEG);
	#endif
	#if defined (macintosh)
		praat_addAction1 (classPhoto, 1, U"Save as JPEG-2000 file...", nullptr, 0, DO_Photo_saveAsJPEG2000);
		praat_addAction1 (classPhoto, 1, U"Save as Apple icon file...", nullptr, 0, DO_Photo_saveAsAppleIconFile);
		praat_addAction1 (classPhoto, 1, U"Save as Windows icon file...", nullptr, 0, DO_Photo_saveAsWindowsIconFile);
	#endif

	praat_addAction1 (classPitch, 0, U"Pitch help", nullptr, 0, DO_Pitch_help);
	praat_addAction1 (classPitch, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Pitch_edit);
	praat_addAction1 (classPitch, 1, U"Edit", nullptr, praat_HIDDEN, DO_Pitch_edit);
	praat_addAction1 (classPitch, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Play pulses", nullptr, 1, DO_Pitch_play);
		praat_addAction1 (classPitch, 0, U"Hum", nullptr, 1, DO_Pitch_hum);
		praat_addAction1 (classPitch, 0, U"-- to sound --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"To Sound (pulses)", nullptr, 1, DO_Pitch_to_Sound_pulses);
		praat_addAction1 (classPitch, 0, U"To Sound (hum)", nullptr, 1, DO_Pitch_to_Sound_hum);
		praat_addAction1 (classPitch, 0, U"To Sound (sine)...", nullptr, 1, DO_Pitch_to_Sound_sine);
	praat_addAction1 (classPitch, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Draw...", nullptr, 1, DO_Pitch_draw);
		praat_addAction1 (classPitch, 0, U"Draw logarithmic...", nullptr, 1, DO_Pitch_drawLogarithmic);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 100 Hz)...", nullptr, 1, DO_Pitch_drawSemitones100);
		praat_addAction1 (classPitch, 0, U"Draw semitones...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Pitch_drawSemitones100);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 200 Hz)...", nullptr, 1, DO_Pitch_drawSemitones200);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 440 Hz)...", nullptr, 1, DO_Pitch_drawSemitones440);
		praat_addAction1 (classPitch, 0, U"Draw mel...", nullptr, 1, DO_Pitch_drawMel);
		praat_addAction1 (classPitch, 0, U"Draw erb...", nullptr, 1, DO_Pitch_drawErb);
		praat_addAction1 (classPitch, 0, U"Speckle...", nullptr, 1, DO_Pitch_speckle);
		praat_addAction1 (classPitch, 0, U"Speckle logarithmic...", nullptr, 1, DO_Pitch_speckleLogarithmic);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 100 Hz)...", nullptr, 1, DO_Pitch_speckleSemitones100);
		praat_addAction1 (classPitch, 0, U"Speckle semitones...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Pitch_speckleSemitones100);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 200 Hz)...", nullptr, 1, DO_Pitch_speckleSemitones200);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 440 Hz)...", nullptr, 1, DO_Pitch_speckleSemitones440);
		praat_addAction1 (classPitch, 0, U"Speckle mel...", nullptr, 1, DO_Pitch_speckleMel);
		praat_addAction1 (classPitch, 0, U"Speckle erb...", nullptr, 1, DO_Pitch_speckleErb);
	praat_addAction1 (classPitch, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classPitch);
		praat_addAction1 (classPitch, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Count voiced frames", nullptr, 1, DO_Pitch_getNumberOfVoicedFrames);
		praat_addAction1 (classPitch, 1, U"Get value at time...", nullptr, 1, DO_Pitch_getValueAtTime);
		praat_addAction1 (classPitch, 1, U"Get value in frame...", nullptr, 1, DO_Pitch_getValueInFrame);
		praat_addAction1 (classPitch, 1, U"-- get extreme --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get minimum...", nullptr, 1, DO_Pitch_getMinimum);
		praat_addAction1 (classPitch, 1, U"Get time of minimum...", nullptr, 1, DO_Pitch_getTimeOfMinimum);
		praat_addAction1 (classPitch, 1, U"Get maximum...", nullptr, 1, DO_Pitch_getMaximum);
		praat_addAction1 (classPitch, 1, U"Get time of maximum...", nullptr, 1, DO_Pitch_getTimeOfMaximum);
		praat_addAction1 (classPitch, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get quantile...", nullptr, 1, DO_Pitch_getQuantile);
		/*praat_addAction1 (classPitch, 1, U"Get spreading...", nullptr, 1, DO_Pitch_getSpreading);*/
		praat_addAction1 (classPitch, 1, U"Get mean...", nullptr, 1, DO_Pitch_getMean);
		praat_addAction1 (classPitch, 1, U"Get standard deviation...", nullptr, 1, DO_Pitch_getStandardDeviation);
		praat_addAction1 (classPitch, 1, U"-- get slope --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get mean absolute slope...", nullptr, 1, DO_Pitch_getMeanAbsoluteSlope);
		praat_addAction1 (classPitch, 1, U"Get slope without octave jumps", nullptr, 1, DO_Pitch_getMeanAbsSlope_noOctave);
		praat_addAction1 (classPitch, 2, U"-- query two --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 2, U"Count differences", nullptr, 1, DO_Pitch_difference);
	praat_addAction1 (classPitch, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPitch);
		praat_addAction1 (classPitch, 0, U"Formula...", nullptr, 1, DO_Pitch_formula);
	praat_addAction1 (classPitch, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextGrid...", nullptr, 1, DO_Pitch_to_TextGrid);
		praat_addAction1 (classPitch, 0, U"-- to single tier --", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextTier", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Pitch_to_TextTier);
		praat_addAction1 (classPitch, 0, U"To IntervalTier", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Pitch_to_IntervalTier);
	praat_addAction1 (classPitch, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To PointProcess", nullptr, 1, DO_Pitch_to_PointProcess);
	praat_addAction1 (classPitch, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Interpolate", nullptr, 1, DO_Pitch_interpolate);
		praat_addAction1 (classPitch, 0, U"Smooth...", nullptr, 1, DO_Pitch_smooth);
		praat_addAction1 (classPitch, 0, U"Subtract linear fit...", nullptr, 1, DO_Pitch_subtractLinearFit);
		praat_addAction1 (classPitch, 0, U"Hack", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"Kill octave jumps", nullptr, 2, DO_Pitch_killOctaveJumps);
		praat_addAction1 (classPitch, 0, U"-- to other types --", nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"Down to PitchTier", nullptr, 1, DO_Pitch_to_PitchTier);
		praat_addAction1 (classPitch, 0, U"To Matrix", nullptr, 1, DO_Pitch_to_Matrix);

	praat_addAction1 (classPitchTier, 1, U"Save as PitchTier spreadsheet file...", nullptr, 0, DO_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, U"Write to PitchTier spreadsheet file...", nullptr, praat_HIDDEN, DO_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, U"Save as headerless spreadsheet file...", nullptr, 0, DO_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, U"Write to headerless spreadsheet file...", nullptr, praat_HIDDEN, DO_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 0, U"PitchTier help", nullptr, 0, DO_PitchTier_help);
	praat_addAction1 (classPitchTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_PitchTier_edit);
	praat_addAction1 (classPitchTier, 1, U"Edit", nullptr, praat_HIDDEN, DO_PitchTier_edit);
	praat_addAction1 (classPitchTier, 0, U"View & Edit with Sound?", nullptr, 0, DO_info_PitchTier_Sound_edit);
	praat_addAction1 (classPitchTier, 0, U"Play pulses", nullptr, 0, DO_PitchTier_play);
	praat_addAction1 (classPitchTier, 0, U"Hum", nullptr, 0, DO_PitchTier_hum);
	praat_addAction1 (classPitchTier, 0, U"Play sine", nullptr, 0, DO_PitchTier_playSine);
	praat_addAction1 (classPitchTier, 0, U"Draw...", nullptr, 0, DO_PitchTier_draw);
	praat_addAction1 (classPitchTier, 0, U"& Manipulation: Replace?", nullptr, 0, DO_info_PitchTier_Manipulation_replace);
	praat_addAction1 (classPitchTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classPitchTier);
		praat_addAction1 (classPitchTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get value at time...", nullptr, 1, DO_PitchTier_getValueAtTime);
		praat_addAction1 (classPitchTier, 1, U"Get value at index...", nullptr, 1, DO_PitchTier_getValueAtIndex);
		praat_addAction1 (classPitchTier, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get mean (curve)...", nullptr, 1, DO_PitchTier_getMean_curve);
		praat_addAction1 (classPitchTier, 1, U"Get mean (points)...", nullptr, 1, DO_PitchTier_getMean_points);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (curve)...", nullptr, 1, DO_PitchTier_getStandardDeviation_curve);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (points)...", nullptr, 1, DO_PitchTier_getStandardDeviation_points);
	praat_addAction1 (classPitchTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classPitchTier);
		praat_addAction1 (classPitchTier, 0, U"Add point...", nullptr, 1, DO_PitchTier_addPoint);
		praat_addAction1 (classPitchTier, 0, U"Formula...", nullptr, 1, DO_PitchTier_formula);
		praat_addAction1 (classPitchTier, 0, U"-- stylize --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Stylize...", nullptr, 1, DO_PitchTier_stylize);
		praat_addAction1 (classPitchTier, 0, U"Interpolate quadratically...", nullptr, 1, DO_PitchTier_interpolateQuadratically);
		praat_addAction1 (classPitchTier, 0, U"-- modify frequencies --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Shift frequencies...", nullptr, 1, DO_PitchTier_shiftFrequencies);
		praat_addAction1 (classPitchTier, 0, U"Multiply frequencies...", nullptr, 1, DO_PitchTier_multiplyFrequencies);
	praat_addAction1 (classPitchTier, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"To PointProcess", nullptr, 1, DO_PitchTier_to_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"To Sound (pulse train)...", nullptr, 1, DO_PitchTier_to_Sound_pulseTrain);
		praat_addAction1 (classPitchTier, 0, U"To Sound (phonation)...", nullptr, 1, DO_PitchTier_to_Sound_phonation);
		praat_addAction1 (classPitchTier, 0, U"To Sound (sine)...", nullptr, 1, DO_PitchTier_to_Sound_sine);
	praat_addAction1 (classPitchTier, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Down to PointProcess", nullptr, 1, DO_PitchTier_downto_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"Down to TableOfReal...", nullptr, 1, DO_PitchTier_downto_TableOfReal);

	praat_addAction1 (classPointProcess, 0, U"PointProcess help", nullptr, 0, DO_PointProcess_help);
	praat_addAction1 (classPointProcess, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 1, U"View & Edit alone", nullptr, praat_HIDDEN, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 1, U"Edit alone", nullptr, praat_HIDDEN, DO_PointProcess_edit);
	praat_addAction1 (classPointProcess, 0, U"View & Edit with Sound?", nullptr, 0, DO_info_PointProcess_Sound_edit);
	praat_addAction1 (classPointProcess, 0, U"Play -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Play as pulse train", nullptr, 1, DO_PointProcess_play);
		praat_addAction1 (classPointProcess, 0, U"Hum", nullptr, 1, DO_PointProcess_hum);
		praat_addAction1 (classPointProcess, 0, U"-- to sound --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To Sound (pulse train)...", nullptr, 1, DO_PointProcess_to_Sound_pulseTrain);
		praat_addAction1 (classPointProcess, 0, U"To Sound (phonation)...", nullptr, 1, DO_PointProcess_to_Sound_phonation);
		praat_addAction1 (classPointProcess, 0, U"To Sound (hum)", nullptr, 1, DO_PointProcess_to_Sound_hum);
	praat_addAction1 (classPointProcess, 0, U"Draw...", nullptr, 0, DO_PointProcess_draw);
	praat_addAction1 (classPointProcess, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classPointProcess);
		praat_addAction1 (classPointProcess, 1, U"-- script get --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of points", nullptr, 1, DO_PointProcess_getNumberOfPoints);
		praat_addAction1 (classPointProcess, 1, U"Get low index...", nullptr, 1, DO_PointProcess_getLowIndex);
		praat_addAction1 (classPointProcess, 1, U"Get high index...", nullptr, 1, DO_PointProcess_getHighIndex);
		praat_addAction1 (classPointProcess, 1, U"Get nearest index...", nullptr, 1, DO_PointProcess_getNearestIndex);
		praat_addAction1 (classPointProcess, 1, U"Get time from index...", nullptr, 1, DO_PointProcess_getTimeFromIndex);
		praat_addAction1 (classPointProcess, 1, U"Get interval...", nullptr, 1, DO_PointProcess_getInterval);
		praat_addAction1 (classPointProcess, 1, U"-- periods --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of periods...", nullptr, 1, DO_PointProcess_getNumberOfPeriods);
		praat_addAction1 (classPointProcess, 1, U"Get mean period...", nullptr, 1, DO_PointProcess_getMeanPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get stdev period...", nullptr, 1, DO_PointProcess_getStdevPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local)...", nullptr, 1, DO_PointProcess_getJitter_local);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local, absolute)...", nullptr, 1, DO_PointProcess_getJitter_local_absolute);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (rap)...", nullptr, 1, DO_PointProcess_getJitter_rap);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ppq5)...", nullptr, 1, DO_PointProcess_getJitter_ppq5);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ddp)...", nullptr, 1, DO_PointProcess_getJitter_ddp);
	praat_addAction1 (classPointProcess, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPointProcess);
		praat_addAction1 (classPointProcess, 0, U"Add point...", nullptr, 1, DO_PointProcess_addPoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point...", nullptr, 1, DO_PointProcess_removePoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point near...", nullptr, 1, DO_PointProcess_removePointNear);
		praat_addAction1 (classPointProcess, 0, U"Remove points...", nullptr, 1, DO_PointProcess_removePoints);
		praat_addAction1 (classPointProcess, 0, U"Remove points between...", nullptr, 1, DO_PointProcess_removePointsBetween);
		praat_addAction1 (classPointProcess, 0, U"-- voice --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Fill...", nullptr, 1, DO_PointProcess_fill);
		praat_addAction1 (classPointProcess, 0, U"Voice...", nullptr, 1, DO_PointProcess_voice);
	praat_addAction1 (classPointProcess, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid...", nullptr, 1, DO_PointProcess_to_TextGrid);
		praat_addAction1 (classPointProcess, 0, U"-- to single tier --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextTier", nullptr, 1, DO_PointProcess_to_TextTier);
		praat_addAction1 (classPointProcess, 0, U"To IntervalTier", nullptr, 1, DO_PointProcess_to_IntervalTier);
	praat_addAction1 (classPointProcess, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To PitchTier...", nullptr, 1, DO_PointProcess_to_PitchTier);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid (vuv)...", nullptr, 1, DO_PointProcess_to_TextGrid_vuv);
	praat_addAction1 (classPointProcess, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classPointProcess, 0, U"To Matrix", nullptr, 2, DO_PointProcess_to_Matrix);
			praat_addAction1 (classPointProcess, 0, U"Up to TextTier...", nullptr, 2, DO_PointProcess_upto_TextTier);
			praat_addAction1 (classPointProcess, 0, U"Up to PitchTier...", nullptr, 2, DO_PointProcess_upto_PitchTier);
			praat_addAction1 (classPointProcess, 0, U"Up to IntensityTier...", nullptr, 2, DO_PointProcess_upto_IntensityTier);
	praat_addAction1 (classPointProcess, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 2, U"Union", nullptr, 1, DO_PointProcess_union);
		praat_addAction1 (classPointProcess, 2, U"Intersection", nullptr, 1, DO_PointProcess_intersection);
		praat_addAction1 (classPointProcess, 2, U"Difference", nullptr, 1, DO_PointProcess_difference);

	praat_addAction1 (classPolygon, 0, U"Polygon help", nullptr, 0, DO_Polygon_help);
praat_addAction1 (classPolygon, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"Draw...", nullptr, 1, DO_Polygon_draw);
	praat_addAction1 (classPolygon, 0, U"Draw closed...", nullptr, 1, DO_Polygon_drawClosed);
	praat_addAction1 (classPolygon, 0, U"Paint...", nullptr, 1, DO_Polygon_paint);
	praat_addAction1 (classPolygon, 0, U"Draw circles...", nullptr, 1, DO_Polygon_drawCircles);
	praat_addAction1 (classPolygon, 0, U"Paint circles...", nullptr, 1, DO_Polygon_paintCircles);
	praat_addAction1 (classPolygon, 2, U"Draw connection...", nullptr, 1, DO_Polygons_drawConnection);
praat_addAction1 (classPolygon, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"Randomize", nullptr, 1, DO_Polygon_randomize);
	praat_addAction1 (classPolygon, 0, U"Salesperson...", nullptr, 1, DO_Polygon_salesperson);
praat_addAction1 (classPolygon, 0, U"Hack -", nullptr, 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"To Matrix", nullptr, 1, DO_Polygon_to_Matrix);

	praat_addAction1 (classSpectrogram, 0, U"Spectrogram help", nullptr, 0, DO_Spectrogram_help);
	praat_addAction1 (classSpectrogram, 1, U"View", nullptr, 0, DO_Spectrogram_view);
	praat_addAction1 (classSpectrogram, 1, U"Movie", nullptr, 0, DO_Spectrogram_movie);
	praat_addAction1 (classSpectrogram, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 1, U"Get power at...", nullptr, 1, DO_Spectrogram_getPowerAt);
	praat_addAction1 (classSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"Paint...", nullptr, 1, DO_Spectrogram_paint);
	praat_addAction1 (classSpectrogram, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 1, DO_Spectrogram_to_Spectrum);
	praat_addAction1 (classSpectrogram, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Sound...", nullptr, 1, DO_Spectrogram_to_Sound);
	praat_addAction1 (classSpectrogram, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 0, U"Formula...", nullptr, 1, DO_Spectrogram_formula);
	praat_addAction1 (classSpectrogram, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Matrix", nullptr, 1, DO_Spectrogram_to_Matrix);

	praat_addAction1 (classSpectrum, 0, U"Spectrum help", nullptr, 0, DO_Spectrum_help);
	praat_addAction1 (classSpectrum, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Spectrum_edit);
	praat_addAction1 (classSpectrum, 1, U"Edit", nullptr, praat_HIDDEN, DO_Spectrum_edit);
	praat_addAction1 (classSpectrum, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Sound", nullptr, 1, DO_Spectrum_to_Sound);
						praat_addAction1 (classSpectrum, 0, U"To Sound (fft)", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Spectrum_to_Sound);
	praat_addAction1 (classSpectrum, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Draw...", nullptr, 1, DO_Spectrum_draw);
		praat_addAction1 (classSpectrum, 0, U"Draw (log freq)...", nullptr, 1, DO_Spectrum_drawLogFreq);
	praat_addAction1 (classSpectrum, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"List...", nullptr, 1, DO_Spectrum_list);
	praat_addAction1 (classSpectrum, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Frequency domain", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get lowest frequency", nullptr, 2, DO_Spectrum_getLowestFrequency);
			praat_addAction1 (classSpectrum, 1, U"Get highest frequency", nullptr, 2, DO_Spectrum_getHighestFrequency);
		praat_addAction1 (classSpectrum, 1, U"Frequency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get number of bins", nullptr, 2, DO_Spectrum_getNumberOfBins);
			praat_addAction1 (classSpectrum, 1, U"Get bin width", nullptr, 2, DO_Spectrum_getBinWidth);
			praat_addAction1 (classSpectrum, 1, U"Get frequency from bin number...", nullptr, 2, DO_Spectrum_getFrequencyFromBin);
						praat_addAction1 (classSpectrum, 1, U"Get frequency from bin...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getFrequencyFromBin);
			praat_addAction1 (classSpectrum, 1, U"Get bin number from frequency...", nullptr, 2, DO_Spectrum_getBinFromFrequency);
						praat_addAction1 (classSpectrum, 1, U"Get bin from frequency...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Spectrum_getBinFromFrequency);
		praat_addAction1 (classSpectrum, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get real value in bin...", nullptr, 1, DO_Spectrum_getRealValueInBin);
		praat_addAction1 (classSpectrum, 1, U"Get imaginary value in bin...", nullptr, 1, DO_Spectrum_getImaginaryValueInBin);
		praat_addAction1 (classSpectrum, 1, U"-- get energy --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get band energy...", nullptr, 1, DO_Spectrum_getBandEnergy);
		praat_addAction1 (classSpectrum, 1, U"Get band density...", nullptr, 1, DO_Spectrum_getBandDensity);
		praat_addAction1 (classSpectrum, 1, U"Get band energy difference...", nullptr, 1, DO_Spectrum_getBandEnergyDifference);
		praat_addAction1 (classSpectrum, 1, U"Get band density difference...", nullptr, 1, DO_Spectrum_getBandDensityDifference);
		praat_addAction1 (classSpectrum, 1, U"-- get moments --", nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get centre of gravity...", nullptr, 1, DO_Spectrum_getCentreOfGravity);
		praat_addAction1 (classSpectrum, 1, U"Get standard deviation...", nullptr, 1, DO_Spectrum_getStandardDeviation);
		praat_addAction1 (classSpectrum, 1, U"Get skewness...", nullptr, 1, DO_Spectrum_getSkewness);
		praat_addAction1 (classSpectrum, 1, U"Get kurtosis...", nullptr, 1, DO_Spectrum_getKurtosis);
		praat_addAction1 (classSpectrum, 1, U"Get central moment...", nullptr, 1, DO_Spectrum_getCentralMoment);
	praat_addAction1 (classSpectrum, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Formula...", nullptr, 1, DO_Spectrum_formula);
		praat_addAction1 (classSpectrum, 0, U"Filter (pass Hann band)...", nullptr, 1, DO_Spectrum_passHannBand);
		praat_addAction1 (classSpectrum, 0, U"Filter (stop Hann band)...", nullptr, 1, DO_Spectrum_stopHannBand);
	praat_addAction1 (classSpectrum, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Excitation...", nullptr, 1, DO_Spectrum_to_Excitation);
		praat_addAction1 (classSpectrum, 0, U"To SpectrumTier (peaks)", nullptr, 1, DO_Spectrum_to_SpectrumTier_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Formant (peaks)...", nullptr, 1, DO_Spectrum_to_Formant_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Ltas...", nullptr, 1, DO_Spectrum_to_Ltas);
		praat_addAction1 (classSpectrum, 0, U"To Ltas (1-to-1)", nullptr, 1, DO_Spectrum_to_Ltas_1to1);
		praat_addAction1 (classSpectrum, 0, U"To Spectrogram", nullptr, 1, DO_Spectrum_to_Spectrogram);
	praat_addAction1 (classSpectrum, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Cepstral smoothing...", nullptr, 1, DO_Spectrum_cepstralSmoothing);
		praat_addAction1 (classSpectrum, 0, U"LPC smoothing...", nullptr, 1, DO_Spectrum_lpcSmoothing);
		praat_addAction1 (classSpectrum, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 0, U"To Matrix", nullptr, 2, DO_Spectrum_to_Matrix);

	praat_addAction1 (classSpectrumTier, 0, U"Draw...", nullptr, 0, DO_SpectrumTier_draw);
	praat_addAction1 (classSpectrumTier, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrumTier, 1, U"List...", nullptr, 1, DO_SpectrumTier_list);
		praat_addAction1 (classSpectrumTier, 0, U"Down to Table", nullptr, 1, DO_SpectrumTier_downto_Table);
	praat_addAction1 (classSpectrumTier, 0, U"Remove points below...", nullptr, 0, DO_SpectrumTier_removePointsBelow);

	praat_addAction1 (classStrings, 0, U"Strings help", nullptr, 0, DO_Strings_help);
	praat_addAction1 (classStrings, 1, U"Save as raw text file...", nullptr, 0, DO_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, U"Write to raw text file...", nullptr, praat_HIDDEN, DO_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Strings_edit);
	praat_addAction1 (classStrings, 1, U"Edit", nullptr, praat_HIDDEN, DO_Strings_edit);
	praat_addAction1 (classStrings, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 2, U"Equal?", nullptr, 1, DO_Strings_equal);
		praat_addAction1 (classStrings, 1, U"Get number of strings", nullptr, 1, DO_Strings_getNumberOfStrings);
		praat_addAction1 (classStrings, 1, U"Get string...", nullptr, 1, DO_Strings_getString);
	praat_addAction1 (classStrings, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"Set string...", nullptr, 1, DO_Strings_setString);
		praat_addAction1 (classStrings, 0, U"Insert string...", nullptr, 1, DO_Strings_insertString);
		praat_addAction1 (classStrings, 0, U"Remove string...", nullptr, 1, DO_Strings_removeString);
		praat_addAction1 (classStrings, 0, U"-- modify order --", nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Randomize", nullptr, 1, DO_Strings_randomize);
		praat_addAction1 (classStrings, 0, U"Sort", nullptr, 1, DO_Strings_sort);
		praat_addAction1 (classStrings, 0, U"-- convert --", nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Replace all...", nullptr, 1, DO_Strings_replaceAll);
		praat_addAction1 (classStrings, 0, U"Genericize", nullptr, 1, DO_Strings_genericize);
		praat_addAction1 (classStrings, 0, U"Nativize", nullptr, 1, DO_Strings_nativize);
	praat_addAction1 (classStrings, 0, U"Analyze", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"To Distributions", nullptr, 0, DO_Strings_to_Distributions);
	praat_addAction1 (classStrings, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"To WordList", nullptr, 0, DO_Strings_to_WordList);

	praat_addAction1 (classTable, 0, U"Down to Matrix", nullptr, 0, DO_Table_to_Matrix);

	praat_addAction1 (classTransition, 0, U"Transition help", nullptr, 0, DO_Transition_help);
praat_addAction1 (classTransition, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Draw as numbers...", nullptr, 0, DO_Transition_drawAsNumbers);
praat_addAction1 (classTransition, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Eigen", nullptr, 0, DO_Transition_eigen);
	praat_addAction1 (classTransition, 0, U"Conflate", nullptr, 0, DO_Transition_conflate);
praat_addAction1 (classTransition, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Power...", nullptr, 0, DO_Transition_power);
praat_addAction1 (classTransition, 0, U"Cast", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"To Matrix", nullptr, 0, DO_Transition_to_Matrix);

	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_AmplitudeTier_edit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_AmplitudeTier_edit);   // hidden 2011
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"Multiply", nullptr, 0, DO_Sound_AmplitudeTier_multiply);
	praat_addAction2 (classDistributions, 1, classTransition, 1, U"Map", nullptr, 0, DO_Distributions_Transition_map);
	praat_addAction2 (classDistributions, 1, classTransition, 1, U"To Transition...", nullptr, 0, DO_Distributions_to_Transition_adj);
	praat_addAction2 (classDistributions, 2, classTransition, 1, U"To Transition (noise)...", nullptr, 0, DO_Distributions_to_Transition_noise_adj);
	praat_addAction2 (classDurationTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_DurationTier_edit);
	praat_addAction2 (classDurationTier, 1, classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_DurationTier_edit);
	praat_addAction2 (classFormant, 1, classPointProcess, 1, U"To FormantTier", nullptr, 0, DO_Formant_PointProcess_to_FormantTier);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter", nullptr, 0, DO_Sound_Formant_filter);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, DO_Sound_Formant_filter_noscale);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter", nullptr, 0, DO_Sound_FormantGrid_filter);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, DO_Sound_FormantGrid_filter_noscale);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter", nullptr, 0, DO_Sound_FormantTier_filter);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, DO_Sound_FormantTier_filter_noscale);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw (phonetogram)...", nullptr, 0, DO_Pitch_Intensity_draw);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Speckle (phonetogram)...", nullptr, praat_HIDDEN, DO_Pitch_Intensity_speckle);   /* grandfathered 2005 */
	praat_addAction2 (classIntensity, 1, classPointProcess, 1, U"To IntensityTier", nullptr, 0, DO_Intensity_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classPointProcess, 1, U"To IntensityTier", nullptr, 0, DO_IntensityTier_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_IntensityTier_edit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_IntensityTier_edit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"Multiply", nullptr, praat_HIDDEN, DO_Sound_IntensityTier_multiply_old);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"Multiply...", nullptr, 0, DO_Sound_IntensityTier_multiply);
	praat_addAction2 (classManipulation, 1, classSound, 1, U"Replace original sound", nullptr, 0, DO_Manipulation_replaceOriginalSound);
	praat_addAction2 (classManipulation, 1, classPointProcess, 1, U"Replace pulses", nullptr, 0, DO_Manipulation_replacePulses);
	praat_addAction2 (classManipulation, 1, classPitchTier, 1, U"Replace pitch tier", nullptr, 0, DO_Manipulation_replacePitchTier);
	praat_addAction2 (classManipulation, 1, classDurationTier, 1, U"Replace duration tier", nullptr, 0, DO_Manipulation_replaceDurationTier);
	praat_addAction2 (classManipulation, 1, classTextTier, 1, U"To Manipulation", nullptr, 0, DO_Manipulation_TextTier_to_Manipulation);
	praat_addAction2 (classMatrix, 1, classSound, 1, U"To ParamCurve", nullptr, 0, DO_Matrix_to_ParamCurve);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace red", nullptr, 0, DO_Photo_Matrix_replaceRed);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace green", nullptr, 0, DO_Photo_Matrix_replaceGreen);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace blue", nullptr, 0, DO_Photo_Matrix_replaceBlue);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace transparency", nullptr, 0, DO_Photo_Matrix_replaceTransparency);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"Draw...", nullptr, 0, DO_PitchTier_Pitch_draw);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"To Pitch", nullptr, 0, DO_Pitch_PitchTier_to_Pitch);
	praat_addAction2 (classPitch, 1, classPointProcess, 1, U"To PitchTier", nullptr, 0, DO_Pitch_PointProcess_to_PitchTier);
	praat_addAction3 (classPitch, 1, classPointProcess, 1, classSound, 1, U"Voice report...", nullptr, 0, DO_Sound_Pitch_PointProcess_voiceReport);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (cc)", nullptr, 0, DO_Sound_Pitch_to_PointProcess_cc);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (peaks)...", nullptr, 0, DO_Sound_Pitch_to_PointProcess_peaks);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To Manipulation", nullptr, 0, DO_Sound_Pitch_to_Manipulation);
	praat_addAction2 (classPitchTier, 1, classPointProcess, 1, U"To PitchTier", nullptr, 0, DO_PitchTier_PointProcess_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_PitchTier_edit);
	praat_addAction2 (classPitchTier, 1, classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_PitchTier_edit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_PointProcess_edit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_PointProcess_edit);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Query", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local)...", nullptr, 0, DO_Point_Sound_getShimmer_local);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local_dB)...", nullptr, 0, DO_Point_Sound_getShimmer_local_dB);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq3)...", nullptr, 0, DO_Point_Sound_getShimmer_apq3);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq5)...", nullptr, 0, DO_Point_Sound_getShimmer_apq5);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq11)...", nullptr, 0, DO_Point_Sound_getShimmer_apq11);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (dda)...", nullptr, 0, DO_Point_Sound_getShimmer_dda);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Modify", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Transplant time domain", nullptr, 0, DO_Point_Sound_transplantDomain);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Analyse", nullptr, 0, nullptr);
	/*praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Manipulation", nullptr, 0, DO_Sound_PointProcess_to_Manipulation);*/
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (point)", nullptr, 0, DO_PointProcess_Sound_to_AmplitudeTier_point);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (period)...", nullptr, 0, DO_PointProcess_Sound_to_AmplitudeTier_period);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas...", nullptr, 0, DO_PointProcess_Sound_to_Ltas);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas (only harmonics)...", nullptr, 0, DO_PointProcess_Sound_to_Ltas_harmonics);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Sound ensemble...", nullptr, 0, DO_Sound_PointProcess_to_SoundEnsemble_correlate);

	praat_addAction4 (classDurationTier, 1, classPitchTier, 1, classPointProcess, 1, classSound, 1, U"To Sound...", nullptr, 0, DO_Sound_Point_Pitch_Duration_to_Sound);

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
	INCLUDE_MANPAGES (manual_EEG_init)

	INCLUDE_LIBRARY (praat_EEG_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new synthesis --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_Artsynth_init)
	INCLUDE_LIBRARY (praat_uvafon_David_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new grammars --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_gram_init)
	INCLUDE_LIBRARY (praat_uvafon_FFNet_init)
	INCLUDE_LIBRARY (praat_uvafon_LPC_init)
	INCLUDE_LIBRARY (praat_uvafon_Exp_init)
}

/* End of file praat_Fon.cpp */
