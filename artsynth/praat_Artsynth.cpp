/* praat_Artsynth.cpp
 *
 * Copyright (C) 1992-2012,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Art_Speaker.h"
#include "Artword_Speaker.h"
#include "Art_Speaker_to_VocalTract.h"
#include "Artword_Speaker_Sound.h"
#include "Artword_Speaker_to_Sound.h"
#include "Artword_to_Art.h"
#include "ArtwordEditor.h"
#include "VocalTract_to_Spectrum.h"
#include "praat.h"

extern "C" Graphics Movie_create (const char32 *title, int width, int height);

#undef iam
#define iam iam_LOOP

/***** ART *****/

FORM (Art_create, U"Create a default Articulation", U"Articulatory synthesis") {
	WORD (U"Name", U"articulation")
	OK2
DO
	autoArt me = Art_create ();
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Art_edit, U"View & Edit Articulation", 0) {
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		REAL (kArt_muscle_getText (i), U"0.0")
	OK2
{
	Art object = (Art) ONLY_OBJECT;
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		SET_REAL (kArt_muscle_getText (i), object -> art [i]);
}
DO
	Art object = (Art) ONLY_OBJECT;
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot edit an Art from batch.");
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		object -> art [i] = GET_REAL (kArt_muscle_getText (i));
END2 }

/***** ARTWORD *****/

FORM (Artword_create, U"Create an empty Artword", U"Create Artword...") {
	WORD (U"Name", U"hallo")
	POSITIVE (U"Duration (seconds)", U"1.0")
	OK2
DO
	praat_new (Artword_create (GET_REAL (U"Duration")), GET_STRING (U"Name"));
END2 }

FORM (Artword_draw, U"Draw one Artword tier", nullptr) {
	OPTIONMENU (U"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	
	autoPraatPicture picture;
	LOOP {
		iam (Artword);
		Artword_draw (me, GRAPHICS, GET_INTEGER (U"Muscle"), GET_INTEGER (U"Garnish"));
	}
END2 }

DIRECT2 (Artword_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an Artword from batch.");
	WHERE (SELECTED) {
		iam_LOOP (Artword);
		autoArtwordEditor editor = ArtwordEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (Artword_getTarget, U"Get one Artword target", 0) {
	REAL (U"Time (seconds)", U"0.0")
	OPTIONMENU (U"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	OK2
DO
	LOOP {
		iam (Artword);
		double target = Artword_getTarget (me, GET_INTEGER (U"Muscle"), GET_REAL (U"Time"));
		Melder_information (target);
	}
END2 }

DIRECT2 (Artword_help) {
	Melder_help (U"Artword");
END2 }

FORM (Artword_setTarget, U"Set one Artword target", 0) {
	REAL (U"Time (seconds)", U"0.0")
	REAL (U"Target value (0-1)", U"0.0")
	OPTIONMENU (U"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	OK2
DO
	double time = GET_REAL (U"Time");
	if (time < 0.0) Melder_throw (U"Specified time should not be less than 0.");
	LOOP {
		iam (Artword);
		Artword_setTarget (me, GET_INTEGER (U"Muscle"), time, GET_REAL (U"Target value"));
		praat_dataChanged (me);
	}
END2 }

FORM (Artword_to_Art, U"From Artword to Art", 0) {
	REAL (U"Time (seconds)", U"0.0")
	OK2
DO
	LOOP {
		iam (Artword);
		autoArt thee = Artword_to_Art (me, GET_REAL (U"Time"));
		praat_new (thee.move(), my name);
	}
END2 }

/***** ART & SPEAKER *****/

DIRECT2 (Art_Speaker_draw) {
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_draw (me, thee, GRAPHICS);
END2 }

DIRECT2 (Art_Speaker_fillInnerContour) {
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_fillInnerContour (me, thee, GRAPHICS);
END2 }

DIRECT2 (Art_Speaker_drawMesh) {
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_drawMesh (me, thee, GRAPHICS);
END2 }

DIRECT2 (Art_Speaker_to_VocalTract) {
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	autoVocalTract him = Art_Speaker_to_VocalTract (me, thee);
	praat_new (him.move(), my name, U"_", thy name);
END2 }

/***** ARTWORD & SPEAKER *****/

FORM (Artword_Speaker_draw, U"Draw Artword & Speaker", 0) {
	NATURAL (U"Number of steps", U"5")
	OK2
DO
	autoPraatPicture picture;
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	Artword_Speaker_draw (me, thee, GRAPHICS, GET_INTEGER (U"Number of steps"));
END2 }

FORM (Artword_Speaker_to_Sound, U"Articulatory synthesizer", U"Artword & Speaker: To Sound...") {
	POSITIVE (U"Sampling frequency (Hz)", U"22050")
	NATURAL (U"Oversampling factor", U"25")
	INTEGER (U"Width 1", U"0")
	INTEGER (U"Width 2", U"0")
	INTEGER (U"Width 3", U"0")
	INTEGER (U"Pressure 1", U"0")
	INTEGER (U"Pressure 2", U"0")
	INTEGER (U"Pressure 3", U"0")
	INTEGER (U"Velocity 1", U"0")
	INTEGER (U"Velocity 2", U"0")
	INTEGER (U"Velocity 3", U"0")
	OK2
DO
	autoSound w1, w2, w3, p1, p2, p3, v1, v2, v3;
	int iw1 = GET_INTEGER (U"Width 1");
	int iw2 = GET_INTEGER (U"Width 2");
	int iw3 = GET_INTEGER (U"Width 3");
	int ip1 = GET_INTEGER (U"Pressure 1");
	int ip2 = GET_INTEGER (U"Pressure 2");
	int ip3 = GET_INTEGER (U"Pressure 3");
	int iv1 = GET_INTEGER (U"Velocity 1");
	int iv2 = GET_INTEGER (U"Velocity 2");
	int iv3 = GET_INTEGER (U"Velocity 3");
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	autoSound him = Artword_Speaker_to_Sound (me, thee,
			GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Oversampling factor"),
			& w1, iw1, & w2, iw2, & w3, iw3,
			& p1, ip1, & p2, ip2, & p3, ip3,
			& v1, iv1, & v2, iv2, & v3, iv3);
	praat_new (him.move(), my name, U"_", thy name);
	if (iw1) praat_new (w1.move(), U"width", iw1);
	if (iw2) praat_new (w2.move(), U"width", iw2);
	if (iw3) praat_new (w3.move(), U"width", iw3);
	if (ip1) praat_new (p1.move(), U"pressure", ip1);
	if (ip2) praat_new (p2.move(), U"pressure", ip2);
	if (ip3) praat_new (p3.move(), U"pressure", ip3);
	if (iv1) praat_new (v1.move(), U"velocity", iv1);
	if (iv2) praat_new (v2.move(), U"velocity", iv2);
	if (iv3) praat_new (v3.move(), U"velocity", iv3);
END2 }

DIRECT2 (Artword_Speaker_movie) {
	Graphics graphics = Movie_create (U"Artword & Speaker movie", 300, 300);
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	Artword_Speaker_movie (me, thee, graphics);
END2 }

/***** ARTWORD & SPEAKER [ & SOUND ] *****/

DIRECT2 (Artword_Speaker_Sound_movie) {
	Graphics graphics = Movie_create (U"Artword & Speaker & Sound movie", 300, 300);
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	heis_ONLY (Sound);   // can be null
	Artword_Speaker_Sound_movie (me, thee, him, graphics);
END2 }

/***** SPEAKER *****/

FORM (Speaker_create, U"Create a Speaker", U"Create Speaker...") {
	WORD (U"Name", U"speaker")
	OPTIONMENU (U"Kind of speaker", 1)
		OPTION (U"Female")
		OPTION (U"Male")
		OPTION (U"Child")
	OPTIONMENU (U"Number of tubes in glottis", 2)
		OPTION (U"1")
		OPTION (U"2")
		OPTION (U"10")
	OK2
DO
	autoSpeaker me = Speaker_create (GET_STRING (U"Kind of speaker"), Melder_atoi (GET_STRING (U"Number of tubes in glottis")));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

DIRECT2 (Speaker_help) {
	Melder_help (U"Speaker");
END2 }

/***** VOCAL TRACT *****/

FORM (VocalTract_createFromPhone, U"Create Vocal Tract from phone", U"Create Vocal Tract from phone...") {
	OPTIONMENU (U"Phone", 1)
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
	OK2
DO
	autoVocalTract me = VocalTract_createFromPhone (GET_STRING (U"Phone"));
	praat_new (me.move(), GET_STRING (U"Phone"));
END2 }

DIRECT2 (VocalTract_draw) {
	autoPraatPicture picture;
	WHERE (SELECTED) {
		iam_LOOP (VocalTract);
		VocalTract_draw (me, GRAPHICS);
	}
END2 }

FORM (VocalTract_formula, U"VocalTract Formula", U"Matrix: Formula...") {
	LABEL (U"label", U"`x' is the distance form the glottis in metres, `col' is the section number, `self' is in m\u00B2")
	LABEL (U"label", U"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (U"formula", U"0")
	OK2
DO
	LOOP {
		iam (VocalTract);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END2 }

DIRECT2 (VocalTract_help) {
	Melder_help (U"VocalTract");
END2 }

DIRECT2 (VocalTract_to_Matrix) {
	WHERE (SELECTED) {
		iam_LOOP (VocalTract);
		autoMatrix thee = VocalTract_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (VocalTract_to_Spectrum, U"From Vocal Tract to Spectrum", 0) {
	LABEL (U"", U"Compute transfer function")
	NATURAL (U"Number of frequencies", U"4097")
	POSITIVE (U"Maximum frequency (Hz)", U"5000")
	REAL (U"Glottal damping", U"0.1")
	BOOLEAN (U"Radiation damping", 1)
	BOOLEAN (U"Internal damping", 1)
	OK2
DO
	LOOP {
		iam (VocalTract);
		autoSpectrum thee = VocalTract_to_Spectrum (me, GET_INTEGER (U"Number of frequencies"),
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Glottal damping"),
			GET_INTEGER (U"Radiation damping"), GET_INTEGER (U"Internal damping"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (ArticulatorySynthesisTutorial) {
	Melder_help (U"Articulatory synthesis");
END2 }

void manual_Artsynth_init (ManPages me);

void praat_uvafon_Artsynth_init ();
void praat_uvafon_Artsynth_init () {
	Thing_recognizeClassesByName (classArt, classArtword, classSpeaker, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Articulatory synthesis", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Articulatory synthesis tutorial", nullptr, 1, DO_ArticulatorySynthesisTutorial);
	praat_addMenuCommand (U"Objects", U"New", U"-- new articulatory synthesis -- ", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Articulation...", nullptr, 1, DO_Art_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Speaker...", nullptr, 1, DO_Speaker_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Artword...", nullptr, 1, DO_Artword_create);
	praat_addMenuCommand (U"Objects", U"New", U"-- new vocal tract --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Vocal Tract from phone...", nullptr, 1, DO_VocalTract_createFromPhone);

	praat_addAction1 (classArt, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Art_edit);
	praat_addAction1 (classArt, 1, U"Edit", nullptr, praat_HIDDEN, DO_Art_edit);

	praat_addAction1 (classArtword, 0, U"Artword help", nullptr, 0, DO_Artword_help);
	praat_addAction1 (classArtword, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Artword_edit);
	praat_addAction1 (classArtword, 1, U"Edit", nullptr, praat_HIDDEN, DO_Artword_edit);
	praat_addAction1 (classArtword, 0, U"Info", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 1, U"Get target...", nullptr, 0, DO_Artword_getTarget);
	praat_addAction1 (classArtword, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 0, U"Draw...", nullptr, 0, DO_Artword_draw);
	praat_addAction1 (classArtword, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 1, U"Set target...", nullptr, 0, DO_Artword_setTarget);
	praat_addAction1 (classArtword, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classArtword, 0, U"To Art (slice)...", nullptr, 0, DO_Artword_to_Art);

	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw", nullptr, 0, DO_Art_Speaker_draw);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Fill inner contour", nullptr, 0, DO_Art_Speaker_fillInnerContour);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Draw mesh", nullptr, 0, DO_Art_Speaker_drawMesh);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classArt, 1, classSpeaker, 1, U"To VocalTract", nullptr, 0, DO_Art_Speaker_to_VocalTract);

	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Movie", nullptr, 0, DO_Artword_Speaker_movie);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Draw...", nullptr, 0, DO_Artword_Speaker_draw);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, U"To Sound...", nullptr, 0, DO_Artword_Speaker_to_Sound);

	praat_addAction3 (classArtword, 1, classSpeaker, 1, classSound, 1, U"Movie", nullptr, 0, DO_Artword_Speaker_Sound_movie);

	praat_addAction1 (classSpeaker, 0, U"Speaker help", nullptr, 0, DO_Speaker_help);

	praat_addAction1 (classVocalTract, 0, U"VocalTract help", nullptr, 0, DO_VocalTract_help);
	praat_addAction1 (classVocalTract, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"Draw", nullptr, 0, DO_VocalTract_draw);
	praat_addAction1 (classVocalTract, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"To Spectrum...", nullptr, 0, DO_VocalTract_to_Spectrum);
	praat_addAction1 (classVocalTract, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"Formula...", nullptr, 0, DO_VocalTract_formula);
	praat_addAction1 (classVocalTract, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classVocalTract, 0, U"To Matrix", nullptr, 0, DO_VocalTract_to_Matrix);

	manual_Artsynth_init (theCurrentPraatApplication -> manPages);
}

/* End of file praat_Artsynth.cpp */
