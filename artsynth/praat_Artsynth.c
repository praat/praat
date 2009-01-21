/* praat_Artsynth.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/12
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

extern int praat_Fon_formula (UiForm dia, Interpreter interpreter);

/***** ART *****/

FORM (Art_create, L"Create a default Articulation", L"Articulatory synthesis")
	WORD (L"Name", L"articulation")
	OK
DO
	if (! praat_new1 (Art_create (), GET_STRING (L"Name"))) return 0;
END

FORM (Art_edit, L"Edit Art", 0)
	{
		int i;
		for (i = 1; i <= enumlength (Art_MUSCLE); i ++)
			REAL (enumstring (Art_MUSCLE, i), L"0.0")
	}
	OK
{
	Art object = (Art) ONLY_OBJECT;
	int i; for (i = 1; i <= enumlength (Art_MUSCLE); i ++)
		SET_REAL (enumstring (Art_MUSCLE, i), object -> art [i]);
}
DO
	Art object = (Art) ONLY_OBJECT;
	int i;
	if (theCurrentPraat -> batch) return Melder_error1 (L"Cannot edit an Art from batch.");
	for (i = 1; i <= enumlength (Art_MUSCLE); i ++)
		object -> art [i] = GET_REAL (enumstring (Art_MUSCLE, i));
END

/***** ARTWORD *****/

FORM (Artword_create, L"Create an empty Artword", L"Create Artword...")
	WORD (L"Name", L"hallo")
	POSITIVE (L"Duration (seconds)", L"1.0")
	OK
DO
	if (! praat_new1 (Artword_create (GET_REAL (L"Duration")), GET_STRING (L"Name"))) return 0;
END

FORM (Artword_draw, L"Draw one Artword tier", NULL)
	ENUM (L"Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Artword_draw ((Artword) OBJECT, GRAPHICS, GET_INTEGER (L"Muscle"), GET_INTEGER (L"Garnish")))
END

DIRECT (Artword_edit)
	if (theCurrentPraat -> batch)
		return Melder_error1 (L"Cannot edit an Artword from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor (ArtwordEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME, OBJECT), IOBJECT)) return 0;
END

FORM (Artword_getTarget, L"Get one Artword target", 0)
	REAL (L"Time (seconds)", L"0.0")
	ENUM (L"Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	OK
DO
	Melder_information1 (Melder_double (Artword_getTarget ((Artword) ONLY (classArtword), GET_INTEGER (L"Muscle"), GET_REAL (L"Time"))));
END

DIRECT (Artword_help) Melder_help (L"Artword"); END

FORM (Artword_setTarget, L"Set one Artword target", 0)
	REAL (L"Time (seconds)", L"0.0")
	REAL (L"Target value (0-1)", L"0.0")
	ENUM (L"Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	OK
DO
	double tim = GET_REAL (L"Time");
	if (tim < 0.0) return Melder_error1 (L"'Time' must not be less than 0.");
	WHERE (SELECTED) {
		Artword_setTarget (OBJECT, GET_INTEGER (L"Muscle"), tim, GET_REAL (L"Target value"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Artword_to_Art, L"From Artword to Art", 0)
	REAL (L"Time (seconds)", L"0.0")
	OK
DO
	EVERY_TO (Artword_to_Art (OBJECT, GET_REAL (L"Time")))
END

/***** ART & SPEAKER *****/

DIRECT (Art_Speaker_draw)
	praat_picture_open ();
	Art_Speaker_draw (ONLY (classArt), ONLY (classSpeaker), GRAPHICS);
	praat_picture_close ();
END

DIRECT (Art_Speaker_fillInnerContour)
	praat_picture_open ();
	Art_Speaker_fillInnerContour (ONLY (classArt), ONLY (classSpeaker), GRAPHICS);
	praat_picture_close ();
END

DIRECT (Art_Speaker_drawMesh)
	praat_picture_open ();
	Art_Speaker_drawMesh (ONLY (classArt), ONLY (classSpeaker), GRAPHICS);
	praat_picture_close ();
END

DIRECT (Art_Speaker_to_VocalTract)
	wchar_t name [200];
	praat_name2 (name, classArt, classSpeaker);
	if (! praat_new1 (Art_Speaker_to_VocalTract (ONLY (classArt), ONLY (classSpeaker)), name)) return 0;
END

/***** ARTWORD & SPEAKER *****/

FORM (Artword_Speaker_draw, L"Draw Artword & Speaker", 0)
	NATURAL (L"Number of steps", L"5")
	OK
DO
	praat_picture_open ();
	Artword_Speaker_draw (ONLY (classArtword), ONLY (classSpeaker), GRAPHICS,
		GET_INTEGER (L"Number of steps"));
	praat_picture_close ();
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
	int iw1 = GET_INTEGER (L"Width 1"), iw2 = GET_INTEGER (L"Width 2");
	int iw3 = GET_INTEGER (L"Width 3"), ip1 = GET_INTEGER (L"Pressure 1");
	int ip2 = GET_INTEGER (L"Pressure 2"), ip3 = GET_INTEGER (L"Pressure 3");
	int iv1 = GET_INTEGER (L"Velocity 1"), iv2 = GET_INTEGER (L"Velocity 2");
	int iv3 = GET_INTEGER (L"Velocity 3");
	wchar_t name [200];
	int result;
	praat_name2 (name, classArtword, classSpeaker);
	result = praat_new1 (
		Artword_Speaker_to_Sound (ONLY (classArtword), ONLY (classSpeaker),
			GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Oversampling factor"),
			& w1, iw1, & w2, iw2, & w3, iw3,
			& p1, ip1, & p2, ip2, & p3, ip3,
			& v1, iv1, & v2, iv2, & v3, iv3),
		name);
	if (iw1) praat_new2 (w1, L"width", Melder_integer (iw1));
	if (iw2) praat_new2 (w2, L"width", Melder_integer (iw2));
	if (iw3) praat_new2 (w3, L"width", Melder_integer (iw3));
	if (ip1) praat_new2 (p1, L"pressure", Melder_integer (ip1));
	if (ip2) praat_new2 (p2, L"pressure", Melder_integer (ip2));
	if (ip3) praat_new2 (p3, L"pressure", Melder_integer (ip3));
	if (iv1) praat_new2 (v1, L"velocity", Melder_integer (iv1));
	if (iv2) praat_new2 (v2, L"velocity", Melder_integer (iv2));
	if (iv3) praat_new2 (v3, L"velocity", Melder_integer (iv3));
	if (! result) return 0;
END

/***** ARTWORD & SPEAKER [ & SOUND ] *****/

DIRECT (Artword_Speaker_movie)
	extern Graphics Movie_create (const wchar_t *title, int width, int height);
	Graphics g = Movie_create (L"Artword & Speaker movie", 300, 300);
	Artword_Speaker_Sound_movie (ONLY (classArtword), ONLY (classSpeaker), ONLY (classSound), g);
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
	if (! praat_new1 (Speaker_create (GET_STRING (L"Kind of speaker"),
		wcstol (GET_STRING (L"Number of tubes in glottis"), NULL, 10)), GET_STRING (L"Name"))) return 0;
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
	if (! praat_new1 (VocalTract_createFromPhone (GET_STRING (L"Phone")), GET_STRING (L"Phone"))) return 0;
END

DIRECT (VocalTract_draw)
	EVERY_DRAW (VocalTract_draw (OBJECT, GRAPHICS))
END

FORM (VocalTract_formula, L"VocalTract Formula", L"Matrix: Formula...")
	LABEL (L"label", L"`x' is the distance form the glottis in metres, `col' is the section number, `self' is in m\u00B2")
	LABEL (L"label", L"x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD (L"formula", L"0")
	OK
DO
	if (! praat_Fon_formula (dia, interpreter)) return 0;
END

DIRECT (VocalTract_help) Melder_help (L"VocalTract"); END

DIRECT (VocalTract_to_Matrix)
	EVERY_TO (VocalTract_to_Matrix (OBJECT))
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
	EVERY_TO (VocalTract_to_Spectrum (OBJECT, GET_INTEGER (L"Number of frequencies"),
		GET_REAL (L"Maximum frequency"), GET_REAL (L"Glottal damping"),
		GET_INTEGER (L"Radiation damping"), GET_INTEGER (L"Internal damping")));
END

DIRECT (ArticulatorySynthesisTutorial) Melder_help (L"Articulatory synthesis"); END

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

	praat_addAction1 (classArt, 1, L"Edit", 0, 0, DO_Art_edit);

	praat_addAction1 (classArtword, 0, L"Artword help", 0, 0, DO_Artword_help);
	praat_addAction1 (classArtword, 1, L"Edit", 0, 0, DO_Artword_edit);
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

	INCLUDE_MANPAGES (manual_Artsynth_init)
}

/* End of file praat_Artsynth.c */
