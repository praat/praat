/* praat_Artsynth.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#include "Art_Speaker.h"
#include "Artword_Speaker.h"
#include "Art_Speaker_to_VocalTract.h"
#include "Artword_Speaker_Sound.h"
#include "Artword_Speaker_to_Sound.h"
#include "Artword_to_Art.h"
#include "ArtwordEditor.h"
#include "VocalTract_to_Spectrum.h"
#include "praat.h"

extern "C" Graphics Movie_create (const wchar_t *title, int width, int height);

#undef iam
#define iam iam_LOOP

/***** ART *****/

FORM (Art_create, L"Create a default Articulation", L"Articulatory synthesis")
	WORD (L"Name", L"articulation")
	OK
DO
	praat_new (Art_create (), GET_STRING (L"Name"));
END

FORM (Art_edit, L"View & Edit Articulation", 0)
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		REAL (kArt_muscle_getText (i), L"0.0")
	OK
{
	Art object = (Art) ONLY_OBJECT;
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		SET_REAL (kArt_muscle_getText (i), object -> art [i]);
}
DO
	Art object = (Art) ONLY_OBJECT;
	if (theCurrentPraatApplication -> batch)
		Melder_throw ("Cannot edit an Art from batch.");
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		object -> art [i] = GET_REAL (kArt_muscle_getText (i));
END

/***** ARTWORD *****/

FORM (Artword_create, L"Create an empty Artword", L"Create Artword...")
	WORD (L"Name", L"hallo")
	POSITIVE (L"Duration (seconds)", L"1.0")
	OK
DO
	praat_new (Artword_create (GET_REAL (L"Duration")), GET_STRING (L"Name"));
END

FORM (Artword_draw, L"Draw one Artword tier", NULL)
	OPTIONMENU (L"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	BOOLEAN (L"Garnish", 1)
	OK
DO
	
	autoPraatPicture picture;
	LOOP {
		iam (Artword);
		Artword_draw (me, GRAPHICS, GET_INTEGER (L"Muscle"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Artword_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit an Artword from batch.");
	WHERE (SELECTED) {
		iam_LOOP (Artword);
		autoArtwordEditor editor = ArtwordEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (Artword_getTarget, L"Get one Artword target", 0)
	REAL (L"Time (seconds)", L"0.0")
	OPTIONMENU (L"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	OK
DO
	LOOP {
		iam (Artword);
		double target = Artword_getTarget (me, GET_INTEGER (L"Muscle"), GET_REAL (L"Time"));
		Melder_information (Melder_double (target));
	}
END

DIRECT (Artword_help)
	Melder_help (L"Artword");
END

FORM (Artword_setTarget, L"Set one Artword target", 0)
	REAL (L"Time (seconds)", L"0.0")
	REAL (L"Target value (0-1)", L"0.0")
	OPTIONMENU (L"Muscle", kArt_muscle_LUNGS)
	for (int ienum = 1; ienum <= kArt_muscle_MAX; ienum ++)
		OPTION (kArt_muscle_getText (ienum))
	OK
DO
	double time = GET_REAL (L"Time");
	if (time < 0.0) Melder_throw ("Specified time should not be less than 0.");
	LOOP {
		iam (Artword);
		Artword_setTarget (me, GET_INTEGER (L"Muscle"), time, GET_REAL (L"Target value"));
		praat_dataChanged (me);
	}
END

FORM (Artword_to_Art, L"From Artword to Art", 0)
	REAL (L"Time (seconds)", L"0.0")
	OK
DO
	LOOP {
		iam (Artword);
		autoArt thee = Artword_to_Art (me, GET_REAL (L"Time"));
		praat_new (thee.transfer(), my name);
	}
END

/***** ART & SPEAKER *****/

DIRECT (Art_Speaker_draw)
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_draw (me, thee, GRAPHICS);
END

DIRECT (Art_Speaker_fillInnerContour)
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_fillInnerContour (me, thee, GRAPHICS);
END

DIRECT (Art_Speaker_drawMesh)
	autoPraatPicture picture;
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	Art_Speaker_drawMesh (me, thee, GRAPHICS);
END

DIRECT (Art_Speaker_to_VocalTract)
	iam_ONLY (Art);
	thouart_ONLY (Speaker);
	autoVocalTract him = Art_Speaker_to_VocalTract (me, thee);
	praat_new (him.transfer(), my name, L"_", thy name);
END

/***** ARTWORD & SPEAKER *****/

FORM (Artword_Speaker_draw, L"Draw Artword & Speaker", 0)
	NATURAL (L"Number of steps", L"5")
	OK
DO
	autoPraatPicture picture;
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	Artword_Speaker_draw (me, thee, GRAPHICS, GET_INTEGER (L"Number of steps"));
END

FORM (Artword_Speaker_to_Sound, L"Articulatory synthesizer", L"Artword & Speaker: To Sound...")
	POSITIVE (L"Sampling frequency (Hz)", L"22050")
	NATURAL (L"Oversampling factor", L"25")
	INTEGER (L"Width 1", L"0")
	INTEGER (L"Width 2", L"0")
	INTEGER (L"Width 3", L"0")
	INTEGER (L"Pressure 1", L"0")
	INTEGER (L"Pressure 2", L"0")
	INTEGER (L"Pressure 3", L"0")
	INTEGER (L"Velocity 1", L"0")
	INTEGER (L"Velocity 2", L"0")
	INTEGER (L"Velocity 3", L"0")
	OK
DO
	Sound w1, w2, w3, p1, p2, p3, v1, v2, v3;
	int iw1 = GET_INTEGER (L"Width 1");
	int iw2 = GET_INTEGER (L"Width 2");
	int iw3 = GET_INTEGER (L"Width 3");
	int ip1 = GET_INTEGER (L"Pressure 1");
	int ip2 = GET_INTEGER (L"Pressure 2");
	int ip3 = GET_INTEGER (L"Pressure 3");
	int iv1 = GET_INTEGER (L"Velocity 1");
	int iv2 = GET_INTEGER (L"Velocity 2");
	int iv3 = GET_INTEGER (L"Velocity 3");
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	autoSound him = Artword_Speaker_to_Sound (me, thee,
			GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Oversampling factor"),
			& w1, iw1, & w2, iw2, & w3, iw3,
			& p1, ip1, & p2, ip2, & p3, ip3,
			& v1, iv1, & v2, iv2, & v3, iv3);
	praat_new (him.transfer(), my name, L"_", thy name);
	if (iw1) praat_new (w1, L"width", Melder_integer (iw1));
	if (iw2) praat_new (w2, L"width", Melder_integer (iw2));
	if (iw3) praat_new (w3, L"width", Melder_integer (iw3));
	if (ip1) praat_new (p1, L"pressure", Melder_integer (ip1));
	if (ip2) praat_new (p2, L"pressure", Melder_integer (ip2));
	if (ip3) praat_new (p3, L"pressure", Melder_integer (ip3));
	if (iv1) praat_new (v1, L"velocity", Melder_integer (iv1));
	if (iv2) praat_new (v2, L"velocity", Melder_integer (iv2));
	if (iv3) praat_new (v3, L"velocity", Melder_integer (iv3));
END

/***** ARTWORD & SPEAKER [ & SOUND ] *****/

DIRECT (Artword_Speaker_movie)
	Graphics g = Movie_create (L"Artword & Speaker movie", 300, 300);
	iam_ONLY (Artword);
	thouart_ONLY (Speaker);
	heis_ONLY (Sound);   // can be null
	Artword_Speaker_Sound_movie (me, thee, him, g);
END

/***** SPEAKER *****/

FORM (Speaker_create, L"Create a Speaker", L"Create Speaker...")
	WORD (L"Name", L"speaker")
	OPTIONMENU (L"Kind of speaker", 1)
		OPTION (L"Female")
		OPTION (L"Male")
		OPTION (L"Child")
	OPTIONMENU (L"Number of tubes in glottis", 2)
		OPTION (L"1")
		OPTION (L"2")
		OPTION (L"10")
	OK
DO
	autoSpeaker me = Speaker_create (GET_STRING (L"Kind of speaker"), wcstol (GET_STRING (L"Number of tubes in glottis"), NULL, 10));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

DIRECT (Speaker_help) Melder_help (L"Speaker"); END

/***** VOCAL TRACT *****/

FORM (VocalTract_createFromPhone, L"Create Vocal Tract from phone", L"Create Vocal Tract from phone...")
	OPTIONMENU (L"Phone", 1)
		OPTION (L"a")
		OPTION (L"e")
		OPTION (L"i")
		OPTION (L"o")
		OPTION (L"u")
		OPTION (L"y1")
		OPTION (L"y2")
		OPTION (L"y3")
		OPTION (L"jery")
		OPTION (L"p")
		OPTION (L"t")
		OPTION (L"k")
		OPTION (L"x")
		OPTION (L"pa")
		OPTION (L"ta")
		OPTION (L"ka")
		OPTION (L"pi")
		OPTION (L"ti")
		OPTION (L"ki")
		OPTION (L"pu")
		OPTION (L"tu")
		OPTION (L"ku")
	OK
DO
	autoVocalTract me = VocalTract_createFromPhone (GET_STRING (L"Phone"));
	praat_new (me.transfer(), GET_STRING (L"Phone"));
END

DIRECT (VocalTract_draw)
	autoPraatPicture picture;
	WHERE (SELECTED) {
		iam_LOOP (VocalTract);
		VocalTract_draw (me, GRAPHICS);
	}
END

FORM (VocalTract_formula, L"VocalTract Formula", L"Matrix: Formula...")
	LABEL (L"label", L"`x' is the distance form the glottis in metres, `col' is the section number, `self' is in m\u00B2")
	LABEL (L"label", L"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"0")
	OK
DO
	LOOP {
		iam (VocalTract);
		try {
			Matrix_formula (me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

DIRECT (VocalTract_help) Melder_help (L"VocalTract"); END

DIRECT (VocalTract_to_Matrix)
	WHERE (SELECTED) {
		iam_LOOP (VocalTract);
		autoMatrix thee = VocalTract_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (VocalTract_to_Spectrum, L"From Vocal Tract to Spectrum", 0)
	LABEL (L"", L"Compute transfer function")
	NATURAL (L"Number of frequencies", L"4097")
	POSITIVE (L"Maximum frequency (Hz)", L"5000")
	REAL (L"Glottal damping", L"0.1")
	BOOLEAN (L"Radiation damping", 1)
	BOOLEAN (L"Internal damping", 1)
	OK
DO
	LOOP {
		iam (VocalTract);
		autoSpectrum thee = VocalTract_to_Spectrum (me, GET_INTEGER (L"Number of frequencies"),
			GET_REAL (L"Maximum frequency"), GET_REAL (L"Glottal damping"),
			GET_INTEGER (L"Radiation damping"), GET_INTEGER (L"Internal damping"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (ArticulatorySynthesisTutorial) Melder_help (L"Articulatory synthesis"); END

void manual_Artsynth_init (ManPages me);

void praat_uvafon_Artsynth_init (void);
void praat_uvafon_Artsynth_init (void) {
	Thing_recognizeClassesByName (classArt, classArtword, classSpeaker, NULL);

	praat_addMenuCommand (L"Objects", L"New", L"Articulatory synthesis", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Articulatory synthesis tutorial", 0, 1, DO_ArticulatorySynthesisTutorial);
	praat_addMenuCommand (L"Objects", L"New", L"-- new articulatory synthesis -- ", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create Articulation...", 0, 1, DO_Art_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Speaker...", 0, 1, DO_Speaker_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create Artword...", 0, 1, DO_Artword_create);
	/*praat_addMenuCommand (L"Objects", L"New", L"-- figures --", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Figures", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 4.1: many tubes", 0, 2, DO_Fig_manyTubes);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 4.4: tube connections", 0, 2, DO_Fig_tubeConnections);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 4.7: vocal-tract construction", 0, 2, DO_Fig_vocalTractConstruction);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 5.2: pumping and sucking", 0, 2, DO_Fig_pumpingAndSucking);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 5.8: closed boundary", 0, 2, DO_Fig_closedBoundary);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 5.9: open boundary", 0, 2, DO_Fig_openBoundary);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 5.10: two-way boundary", 0, 2, DO_Fig_twoWayBoundary);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 5.11: three-way boundary", 0, 2, DO_Fig_threeWayBoundary);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 6.3: smoothing errors", 0, 2, DO_Fig_smoothingErrors);
	praat_addMenuCommand (L"Objects", L"New", L"PB Fig. 6.8: HNR window ripples", 0, 2, DO_Fig_hnrWindowRipples);*/
	praat_addMenuCommand (L"Objects", L"New", L"-- new vocal tract --", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create Vocal Tract from phone...", 0, 1, DO_VocalTract_createFromPhone);

	praat_addAction1 (classArt, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Art_edit);
	praat_addAction1 (classArt, 1, L"Edit", 0, praat_HIDDEN, DO_Art_edit);

	praat_addAction1 (classArtword, 0, L"Artword help", 0, 0, DO_Artword_help);
	praat_addAction1 (classArtword, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Artword_edit);
	praat_addAction1 (classArtword, 1, L"Edit", 0, praat_HIDDEN, DO_Artword_edit);
	praat_addAction1 (classArtword, 0, L"Info", 0, 0, 0);
	praat_addAction1 (classArtword, 1, L"Get target...", 0, 0, DO_Artword_getTarget);
	praat_addAction1 (classArtword, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classArtword, 0, L"Draw...", 0, 0, DO_Artword_draw);
	praat_addAction1 (classArtword, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classArtword, 1, L"Set target...", 0, 0, DO_Artword_setTarget);
	praat_addAction1 (classArtword, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classArtword, 0, L"To Art (slice)...", 0, 0, DO_Artword_to_Art);

	praat_addAction2 (classArt, 1, classSpeaker, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classArt, 1, classSpeaker, 1, L"Draw", 0, 0, DO_Art_Speaker_draw);
	praat_addAction2 (classArt, 1, classSpeaker, 1, L"Fill inner contour", 0, 0, DO_Art_Speaker_fillInnerContour);
	praat_addAction2 (classArt, 1, classSpeaker, 1, L"Draw mesh", 0, 0, DO_Art_Speaker_drawMesh);
	praat_addAction2 (classArt, 1, classSpeaker, 1, L"Synthesize", 0, 0, 0);
	praat_addAction2 (classArt, 1, classSpeaker, 1, L"To VocalTract", 0, 0, DO_Art_Speaker_to_VocalTract);

	praat_addAction2 (classArtword, 1, classSpeaker, 1, L"Movie", 0, 0, DO_Artword_Speaker_movie);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, L"Draw...", 0, 0, DO_Artword_Speaker_draw);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, L"Synthesize", 0, 0, 0);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, L"To Sound...", 0, 0, DO_Artword_Speaker_to_Sound);

	praat_addAction3 (classArtword, 1, classSpeaker, 1, classSound, 1, L"Movie", 0, 0, DO_Artword_Speaker_movie);

	praat_addAction1 (classSpeaker, 0, L"Speaker help", 0, 0, DO_Speaker_help);

	praat_addAction1 (classVocalTract, 0, L"VocalTract help", 0, 0, DO_VocalTract_help);
	praat_addAction1 (classVocalTract, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, L"Draw", 0, 0, DO_VocalTract_draw);
	praat_addAction1 (classVocalTract, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, L"To Spectrum...", 0, 0, DO_VocalTract_to_Spectrum);
	praat_addAction1 (classVocalTract, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, L"Formula...", 0, 0, DO_VocalTract_formula);
	praat_addAction1 (classVocalTract, 0, L"Hack", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, L"To Matrix", 0, 0, DO_VocalTract_to_Matrix);

	manual_Artsynth_init (theCurrentPraatApplication -> manPages);
}

/* End of file praat_Artsynth.cpp */
