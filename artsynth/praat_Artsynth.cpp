/* praat_Artsynth.cpp
 *
 * Copyright (C) 1992-2009,2011,2012,2014-2019 Paul Boersma
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

#include "Art_Speaker.h"
#include "Artword_Speaker.h"
#include "Art_Speaker_to_VocalTract.h"
#include "Artword_Speaker_Sound.h"
#include "Artword_Speaker_to_Sound.h"
#include "Artword_to_Art.h"
#include "ArtwordEditor.h"
#include "VocalTract_to_Spectrum.h"

#include "praat_Matrix.h"

// MARK: - ART

FORM (NEW1_Art_create, U"Create a default Articulation", U"Articulatory synthesis") {
	WORD (name, U"Name", U"articulation")
	OK
DO
	CREATE_ONE
		autoArt result = Art_create ();
	CREATE_ONE_END (name);
}

FORM (WINDOW_Art_viewAndEdit, U"View & Edit Articulation", nullptr) {
	static double muscles [1 + (int) kArt_muscle::MAX];
	for (kArt_muscle muscle = (kArt_muscle) 1; muscle <= kArt_muscle::MAX; ++ muscle)
		UiForm_addReal (_dia_.get(), & muscles [(int) muscle], nullptr /* GUI-only */, kArt_muscle_getText (muscle), U"0.0");
OK
	FIND_ONE (Art)
	for (int i = 1; i <= (int) kArt_muscle::MAX; i ++)
		SET_REAL (muscles [i], my art [i])
DO
	FIND_ONE (Art)
		if (theCurrentPraatApplication -> batch)
			Melder_throw (U"Cannot edit an Art from batch.");
		for (int i = 1; i <= (int) kArt_muscle::MAX; i ++)
			my art [i] = muscles [i];
	END
}

// MARK: - ARTWORD

FORM (NEW1_Artword_create, U"Create an empty Artword", U"Create Artword...") {
	WORD (name, U"Name", U"hallo")
	POSITIVE (duration, U"Duration (seconds)", U"1.0")
	OK
DO
	CREATE_ONE
		autoArtword result = Artword_create (duration);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_Artword_draw, U"Draw one Artword tier", nullptr) {
	OPTIONMENU (muscle, U"Muscle", (int) kArt_muscle::LUNGS)
	for (int ienum = 1; ienum <= (int) kArt_muscle::MAX; ienum ++)
		OPTION (kArt_muscle_getText ((kArt_muscle) ienum))
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Artword)
		Artword_draw (me, GRAPHICS, (kArt_muscle) muscle, garnish);
	GRAPHICS_EACH_END
}

DIRECT (WINDOW_Artword_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an Artword from batch.");
	FIND_ONE_WITH_IOBJECT (Artword)
		autoArtwordEditor editor = ArtwordEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM (REAL_Artword_getTarget, U"Get one Artword target", nullptr) {
	REAL (time, U"Time (seconds)", U"0.0")
	OPTIONMENU (muscle, U"Muscle", (int) kArt_muscle::LUNGS)
	for (int ienum = 1; ienum <= (int) kArt_muscle::MAX; ienum ++)
		OPTION (kArt_muscle_getText ((kArt_muscle) ienum))
	OK
DO
	NUMBER_ONE (Artword)
		double result = Artword_getTarget (me, (kArt_muscle) muscle, time);
	NUMBER_ONE_END (U"")
}

DIRECT (HELP_Artword_help) {
	HELP (U"Artword")
}

FORM (MODIFY_Artword_setTarget, U"Set one Artword target", nullptr) {
	REAL (time, U"Time (seconds)", U"0.0")
	REAL (targetValue, U"Target value (0-1)", U"0.0")
	OPTIONMENU (muscle, U"Muscle", (int) kArt_muscle::LUNGS)
	for (int ienum = 1; ienum <= (int) kArt_muscle::MAX; ienum ++)
		OPTION (kArt_muscle_getText ((kArt_muscle) ienum))
	OK
DO
	if (time < 0.0) Melder_throw (U"The specified time should not be less than 0.");
	MODIFY_EACH (Artword)
		Artword_setTarget (me, (kArt_muscle) muscle, time, targetValue);
	MODIFY_EACH_END
}

FORM (NEW_Artword_to_Art, U"From Artword to Art", nullptr) {
	REAL (time, U"Time (seconds)", U"0.0")
	OK
DO
	CONVERT_EACH (Artword)
		autoArt result = Artword_to_Art (me, time);
	CONVERT_EACH_END (my name.get())
}

// MARK: - ART & SPEAKER

DIRECT (GRAPHICS_Art_Speaker_draw) {
	GRAPHICS_TWO (Art, Speaker)
		Art_Speaker_draw (me, you, GRAPHICS);
	GRAPHICS_TWO_END
}

DIRECT (GRAPHICS_Art_Speaker_fillInnerContour) {
	GRAPHICS_TWO (Art, Speaker)
		Art_Speaker_fillInnerContour (me, you, GRAPHICS);
	GRAPHICS_TWO_END
}

DIRECT (GRAPHICS_Art_Speaker_drawMesh) {
	GRAPHICS_TWO (Art, Speaker)
		Art_Speaker_drawMesh (me, you, GRAPHICS);
	GRAPHICS_TWO_END
}

DIRECT (NEW1_Art_Speaker_to_VocalTract) {
	CONVERT_TWO (Art, Speaker)
		autoVocalTract result = Art_Speaker_to_VocalTract (me, you);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

// MARK: - ARTWORD & SPEAKER

FORM (GRAPHICS_Artword_Speaker_draw, U"Draw Artword & Speaker", nullptr) {
	NATURAL (numberOfSteps, U"Number of steps", U"5")
	OK
DO
	GRAPHICS_TWO (Artword, Speaker)
		Artword_Speaker_draw (me, you, GRAPHICS, numberOfSteps);
	GRAPHICS_TWO_END
}

FORM (NEW1_Artword_Speaker_to_Sound, U"Articulatory synthesizer", U"Artword & Speaker: To Sound...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"22050.0")
	NATURAL (oversamplingFactor, U"Oversampling factor", U"25")
	INTEGER (width1, U"Width 1", U"0")
	INTEGER (width2, U"Width 2", U"0")
	INTEGER (width3, U"Width 3", U"0")
	INTEGER (pressure1, U"Pressure 1", U"0")
	INTEGER (pressure2, U"Pressure 2", U"0")
	INTEGER (pressure3, U"Pressure 3", U"0")
	INTEGER (velocity1, U"Velocity 1", U"0")
	INTEGER (velocity2, U"Velocity 2", U"0")
	INTEGER (velocity3, U"Velocity 3", U"0")
	OK
DO
	FIND_TWO (Artword, Speaker)
		autoSound w1, w2, w3, p1, p2, p3, v1, v2, v3;
		autoSound result = Artword_Speaker_to_Sound (me, you,
			samplingFrequency, oversamplingFactor,
			& w1, width1, & w2, width2, & w3, width3,
			& p1, pressure1, & p2, pressure2, & p3, pressure3,
			& v1, velocity1, & v2, velocity2, & v3, velocity3);
		praat_new (result.move(), my name.get(), U"_", your name.get());
		if (width1) praat_new (w1.move(), U"width", width1);
		if (width2) praat_new (w2.move(), U"width", width2);
		if (width3) praat_new (w3.move(), U"width", width3);
		if (pressure1) praat_new (p1.move(), U"pressure", pressure1);
		if (pressure2) praat_new (p2.move(), U"pressure", pressure2);
		if (pressure3) praat_new (p3.move(), U"pressure", pressure3);
		if (velocity1) praat_new (v1.move(), U"velocity", velocity1);
		if (velocity2) praat_new (v2.move(), U"velocity", velocity2);
		if (velocity3) praat_new (v3.move(), U"velocity", velocity3);
	END
}

DIRECT (MOVIE_Artword_Speaker_playMovie) {
	MOVIE_TWO (Artword, Speaker, U"Artword & Speaker movie", 300, 300)
		Artword_Speaker_playMovie (me, you, graphics);
	MOVIE_TWO_END
}

// MARK: - ARTWORD & SPEAKER & SOUND

DIRECT (MOVIE_Artword_Speaker_Sound_playMovie) {
	MOVIE_THREE (Artword, Speaker, Sound, U"Artword & Speaker & Sound movie", 300, 300)
		Artword_Speaker_Sound_playMovie (me, you, him, graphics);
	MOVIE_THREE_END
}

// MARK: - SPEAKER

FORM (NEW1_Speaker_create, U"Create a Speaker", U"Create Speaker...") {
	WORD (name, U"Name", U"speaker")
	OPTIONMENUSTR (kindOfSpeaker, U"Kind of speaker", 1)
		OPTION (U"female")
		OPTION (U"male")
		OPTION (U"child")
	OPTIONMENUSTR (numberOfTubesInGlottis, U"Number of tubes in glottis", 2)
		OPTION (U"1")
		OPTION (U"2")
		OPTION (U"10")
	OK
DO
	CREATE_ONE
		autoSpeaker result = Speaker_create (kindOfSpeaker, (int16) Melder_atoi (numberOfTubesInGlottis));   // conversion OK because the values are 1, 2, 10
	CREATE_ONE_END (name)
}

DIRECT (HELP_Speaker_help) {
	HELP (U"Speaker")
}

// MARK: - VOCALTRACT

FORM (NEW1_VocalTract_createFromPhone, U"Create Vocal Tract from phone", U"Create Vocal Tract from phone...") {
	OPTIONMENUSTR (phone, U"Phone", 1)
		OPTION (U"a")
		OPTION (U"e")
		OPTION (U"i")
		OPTION (U"o")
		OPTION (U"u")
		OPTION (U"y1")
		OPTION (U"y2")
		OPTION (U"y3")
		OPTION (U"jery")
		OPTION (U"p")
		OPTION (U"t")
		OPTION (U"k")
		OPTION (U"x")
		OPTION (U"pa")
		OPTION (U"ta")
		OPTION (U"ka")
		OPTION (U"pi")
		OPTION (U"ti")
		OPTION (U"ki")
		OPTION (U"pu")
		OPTION (U"tu")
		OPTION (U"ku")
	OK
DO
	CREATE_ONE
		autoVocalTract result = VocalTract_createFromPhone (phone);
	CREATE_ONE_END (phone)
}

DIRECT (GRAPHICS_VocalTract_draw) {
	GRAPHICS_EACH (VocalTract)
		VocalTract_draw (me, GRAPHICS);
	GRAPHICS_EACH_END
}

FORM (MODIFY_VocalTract_formula, U"VocalTract Formula", U"Matrix: Formula...") {
	LABEL (U"`x` is the distance form the glottis in metres, `col` is the section number, `self` is in m\u00B2")
	LABEL (U"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (formula, U"Formula:", U"0")
	OK
DO
	MODIFY_EACH_WEAK (VocalTract)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

DIRECT (HELP_VocalTract_help) {
	HELP (U"VocalTract")
}

DIRECT (NEW_VocalTract_to_Matrix) {
	CONVERT_EACH (VocalTract)
		autoMatrix result = VocalTract_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_VocalTract_to_Spectrum, U"From Vocal Tract to Spectrum", nullptr) {
	LABEL (U"Compute transfer function")
	NATURAL (numberOfFequencies, U"Number of frequencies", U"4097")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	REAL (glottalDamping, U"Glottal damping", U"0.1")
	BOOLEAN (radiationDamping, U"Radiation damping", true)
	BOOLEAN (internalDamping, U"Internal damping", true)
	OK
DO
	CONVERT_EACH (VocalTract)
		autoSpectrum result = VocalTract_to_Spectrum (me, numberOfFequencies,
			maximumFrequency, glottalDamping, radiationDamping, internalDamping);
	CONVERT_EACH_END (my name.get())
}

DIRECT (HELP_ArticulatorySynthesisTutorial) {
	HELP (U"Articulatory synthesis")
}

void manual_Artsynth_init (ManPages me);

// MARK: - buttons

void praat_uvafon_Artsynth_init ();
void praat_uvafon_Artsynth_init () {
	Thing_recognizeClassesByName (classArt, classArtword, classSpeaker, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Articulatory synthesis", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Articulatory synthesis tutorial", nullptr, praat_DEPTH_1, HELP_ArticulatorySynthesisTutorial);
	praat_addMenuCommand (U"Objects", U"New", U"-- new articulatory synthesis -- ", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Articulation...", nullptr, 1, NEW1_Art_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Speaker...", nullptr, 1, NEW1_Speaker_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Artword...", nullptr, 1, NEW1_Artword_create);
	praat_addMenuCommand (U"Objects", U"New", U"-- new vocal tract --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Vocal Tract from phone...", nullptr, 1, NEW1_VocalTract_createFromPhone);

	praat_addAction1 (classArt, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Art_viewAndEdit);
	praat_addAction1 (classArt, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_Art_viewAndEdit);

	praat_addAction1 (classArtword, 0, U"Artword help", nullptr, 0, HELP_Artword_help);
	praat_addAction1 (classArtword, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Artword_viewAndEdit);
	praat_addAction1 (classArtword, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_Artword_viewAndEdit);
	praat_addAction1 (classArtword, 0, U"Info", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 1, U"Get target...", nullptr, 0, REAL_Artword_getTarget);
	praat_addAction1 (classArtword, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 0, U"Draw...", nullptr, 0, GRAPHICS_Artword_draw);
	praat_addAction1 (classArtword, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 1, U"Set target...", nullptr, 0, MODIFY_Artword_setTarget);
	praat_addAction1 (classArtword, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 0, U"To Art (slice)...", nullptr, 0, NEW_Artword_to_Art);

	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw", nullptr, 0, GRAPHICS_Art_Speaker_draw);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Fill inner contour", nullptr, 0, GRAPHICS_Art_Speaker_fillInnerContour);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw mesh", nullptr, 0, GRAPHICS_Art_Speaker_drawMesh);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"To VocalTract", nullptr, 0, NEW1_Art_Speaker_to_VocalTract);

	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Play movie", nullptr, 0, MOVIE_Artword_Speaker_playMovie);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Movie", nullptr, praat_HIDDEN, MOVIE_Artword_Speaker_playMovie);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Draw...", nullptr, 0, GRAPHICS_Artword_Speaker_draw);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"To Sound...", nullptr, 0, NEW1_Artword_Speaker_to_Sound);

	praat_addAction3 (classArtword, 1, classSpeaker, 1, classSound, 1, U"Play movie", nullptr, 0, MOVIE_Artword_Speaker_Sound_playMovie);
	praat_addAction3 (classArtword, 1, classSpeaker, 1, classSound, 1, U"Movie", nullptr, praat_HIDDEN, MOVIE_Artword_Speaker_Sound_playMovie);

	praat_addAction1 (classSpeaker, 0, U"Speaker help", nullptr, 0, HELP_Speaker_help);

	praat_addAction1 (classVocalTract, 0, U"VocalTract help", nullptr, 0, HELP_VocalTract_help);
	praat_addAction1 (classVocalTract, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"Draw", nullptr, 0, GRAPHICS_VocalTract_draw);
	praat_addAction1 (classVocalTract, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"To Spectrum...", nullptr, 0, NEW_VocalTract_to_Spectrum);
	praat_addAction1 (classVocalTract, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"Formula...", nullptr, 0, MODIFY_VocalTract_formula);
	praat_addAction1 (classVocalTract, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"To Matrix", nullptr, 0, NEW_VocalTract_to_Matrix);

	manual_Artsynth_init (theCurrentPraatApplication -> manPages);
}

/* End of file praat_Artsynth.cpp */
