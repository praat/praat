/* praat_Artsynth.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/12/18
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

extern int praat_Fon_formula (Any dia);

/***** ART *****/

FORM (Art_create, "Create a default Articulation", "Articulatory synthesis")
	WORD ("Name", "articulation")
	OK
DO
	if (! praat_new (Art_create (), GET_STRING ("Name"))) return 0;
END

FORM (Art_edit, "Edit Art", 0)
	{
		int i;
		for (i = 1; i <= enumlength (Art_MUSCLE); i ++)
			REAL (enumstring (Art_MUSCLE, i), "0.0")
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
	if (praat.batch) return Melder_error ("Cannot edit an Art from batch.");
	for (i = 1; i <= enumlength (Art_MUSCLE); i ++)
		object -> art [i] = GET_REAL (enumstring (Art_MUSCLE, i));
END

/***** ARTWORD *****/

FORM (Artword_create, "Create an empty Artword", "Create Artword...")
	WORD ("Name", "hallo")
	POSITIVE ("Duration (seconds)", "1.0")
	OK
DO
	if (! praat_new (Artword_create (GET_REAL ("Duration")), GET_STRING ("Name"))) return 0;
END

FORM (Artword_draw, "Draw one Artword tier", NULL)
	ENUM ("Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Artword_draw ((Artword) OBJECT, GRAPHICS, GET_INTEGER ("Muscle"), GET_INTEGER ("Garnish")))
END

DIRECT (Artword_edit)
	if (praat.batch)
		return Melder_error ("Cannot edit an Artword from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor (ArtwordEditor_create (praat.topShell, FULL_NAME, OBJECT), IOBJECT)) return 0;
END

FORM (Artword_getTarget, "Get one Artword target", 0)
	REAL ("Time (seconds)", "0.0")
	ENUM ("Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	OK
DO
	Melder_information1
		(Melder_double (Artword_getTarget ((Artword) ONLY (classArtword), GET_INTEGER ("Muscle"),
		GET_REAL ("Time"))));
END

DIRECT (Artword_help) Melder_help ("Artword"); END

FORM (Artword_setTarget, "Set one Artword target", 0)
	REAL ("Time (seconds)", "0.0")
	REAL ("Target value (0-1)", "0.0")
	ENUM ("Muscle", Art_MUSCLE, enumi (Art_MUSCLE, Lungs))
	OK
DO
	double tim = GET_REAL ("Time");
	if (tim < 0.0) return Melder_error ("'Time' must not be less than 0.");
	WHERE (SELECTED) {
		Artword_setTarget (OBJECT, GET_INTEGER ("Muscle"),
		tim, GET_REAL ("Target value"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Artword_to_Art, "From Artword to Art", 0)
	REAL ("Time (seconds)", "0.0")
	OK
DO
	EVERY_TO (Artword_to_Art (OBJECT, GET_REAL ("Time")))
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
	char name [200];
	praat_name2 (name, classArt, classSpeaker);
	if (! praat_new (Art_Speaker_to_VocalTract (ONLY (classArt), ONLY (classSpeaker)), name)) return 0;
END

/***** ARTWORD & SPEAKER *****/

FORM (Artword_Speaker_draw, "Draw Artword & Speaker", 0)
	NATURAL ("Number of steps", "5")
	OK
DO
	praat_picture_open ();
	Artword_Speaker_draw (ONLY (classArtword), ONLY (classSpeaker), GRAPHICS,
		GET_INTEGER ("Number of steps"));
	praat_picture_close ();
END

FORM (Artword_Speaker_to_Sound, "Articulatory synthesizer", "Artword & Speaker: To Sound...")
	POSITIVE ("Sampling frequency (Hz)", "22050")
	NATURAL ("Oversampling factor", "25")
	INTEGER ("Width 1", "0")
	INTEGER ("Width 2", "0")
	INTEGER ("Width 3", "0")
	INTEGER ("Pressure 1", "0")
	INTEGER ("Pressure 2", "0")
	INTEGER ("Pressure 3", "0")
	INTEGER ("Velocity 1", "0")
	INTEGER ("Velocity 2", "0")
	INTEGER ("Velocity 3", "0")
	OK
DO
	Sound w1, w2, w3, p1, p2, p3, v1, v2, v3;
	int iw1 = GET_INTEGER ("Width 1"), iw2 = GET_INTEGER ("Width 2");
	int iw3 = GET_INTEGER ("Width 3"), ip1 = GET_INTEGER ("Pressure 1");
	int ip2 = GET_INTEGER ("Pressure 2"), ip3 = GET_INTEGER ("Pressure 3");
	int iv1 = GET_INTEGER ("Velocity 1"), iv2 = GET_INTEGER ("Velocity 2");
	int iv3 = GET_INTEGER ("Velocity 3");
	char name [200];
	int result;
	praat_name2 (name, classArtword, classSpeaker);
	result = praat_new (
		Artword_Speaker_to_Sound (ONLY (classArtword), ONLY (classSpeaker),
			GET_REAL ("Sampling frequency"), GET_INTEGER ("Oversampling factor"),
			& w1, iw1, & w2, iw2, & w3, iw3,
			& p1, ip1, & p2, ip2, & p3, ip3,
			& v1, iv1, & v2, iv2, & v3, iv3),
		name);
	if (iw1) { sprintf (name, "width%d", iw1); praat_new (w1, name); }
	if (iw2) { sprintf (name, "width%d", iw2); praat_new (w2, name); }
	if (iw3) { sprintf (name, "width%d", iw3); praat_new (w3, name); }
	if (ip1) { sprintf (name, "pressure%d", ip1); praat_new (p1, name); }
	if (ip2) { sprintf (name, "pressure%d", ip2); praat_new (p2, name); }
	if (ip3) { sprintf (name, "pressure%d", ip3); praat_new (p3, name); }
	if (iv1) { sprintf (name, "velocity%d", iv1); praat_new (v1, name); }
	if (iv2) { sprintf (name, "velocity%d", iv2); praat_new (v2, name); }
	if (iv3) { sprintf (name, "velocity%d", iv3); praat_new (v3, name); }
	if (! result) return 0;
END

/***** ARTWORD & SPEAKER [ & SOUND ] *****/

DIRECT (Artword_Speaker_movie)
	extern Graphics Movie_create (const char *title, int width, int height);
	Graphics g = Movie_create ("Artword & Speaker movie", 300, 300);
	Artword_Speaker_Sound_movie (ONLY (classArtword), ONLY (classSpeaker), ONLY (classSound), g);
END

/***** SPEAKER *****/

FORM (Speaker_create, "Create a Speaker", "Create Speaker...")
	WORD ("Name", "speaker")
	OPTIONMENU ("Kind of speaker", 1)
		OPTION ("Female")
		OPTION ("Male")
		OPTION ("Child")
	OPTIONMENU ("Number of tubes in glottis", 2)
		OPTION ("1")
		OPTION ("2")
		OPTION ("10")
	OK
DO
	if (! praat_new (Speaker_create (GET_STRING ("Kind of speaker"),
		atol (GET_STRING ("Number of tubes in glottis"))), GET_STRING ("Name"))) return 0;
END

DIRECT (Speaker_help) Melder_help ("Speaker"); END

/***** VOCAL TRACT *****/

FORM (VocalTract_createFromPhone, "Create Vocal Tract from phone", "Create Vocal Tract from phone...")
	OPTIONMENU ("Phone", 1)
		OPTION ("a")
		OPTION ("e")
		OPTION ("i")
		OPTION ("o")
		OPTION ("u")
		OPTION ("y1")
		OPTION ("y2")
		OPTION ("y3")
		OPTION ("jery")
		OPTION ("p")
		OPTION ("t")
		OPTION ("k")
		OPTION ("x")
		OPTION ("pa")
		OPTION ("ta")
		OPTION ("ka")
		OPTION ("pi")
		OPTION ("ti")
		OPTION ("ki")
		OPTION ("pu")
		OPTION ("tu")
		OPTION ("ku")
	OK
DO
	NEW (VocalTract_createFromPhone (GET_STRING ("Phone")))
END

DIRECT (VocalTract_draw)
	EVERY_DRAW (VocalTract_draw (OBJECT, GRAPHICS))
END

FORM (VocalTract_formula, "VocalTract Formula", "Matrix: Formula...")
	LABEL ("label", "`x' is the distance form the glottis in metres, `col' is the section number, `self' is in m^2")
	LABEL ("label", "x := x1;   for col := 1 to ncol do { self [col] := `formula' ; x := x + dx }")
	TEXTFIELD ("formula", "0")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

DIRECT (VocalTract_help) Melder_help ("VocalTract"); END

DIRECT (VocalTract_to_Matrix)
	EVERY_TO (VocalTract_to_Matrix (OBJECT))
END

FORM (VocalTract_to_Spectrum, "From Vocal Tract to Spectrum", 0)
	LABEL ("", "Compute transfer function")
	NATURAL ("Number of frequencies", "4097")
	POSITIVE ("Maximum frequency (Hz)", "5000")
	REAL ("Glottal damping", "0.1")
	BOOLEAN ("Radiation damping", 1)
	BOOLEAN ("Internal damping", 1)
	OK
DO
	EVERY_TO (VocalTract_to_Spectrum (OBJECT, GET_INTEGER ("Number of frequencies"),
		GET_REAL ("Maximum frequency"), GET_REAL ("Glottal damping"),
		GET_INTEGER ("Radiation damping"), GET_INTEGER ("Internal damping")));
END

DIRECT (ArticulatorySynthesisTutorial) Melder_help ("Articulatory synthesis"); END

void praat_uvafon_Artsynth_init (void);
void praat_uvafon_Artsynth_init (void) {
	Thing_recognizeClassesByName (classArt, classArtword, classSpeaker, NULL);

	praat_addMenuCommand ("Objects", "New", "Articulatory synthesis", 0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "Articulatory synthesis tutorial", 0, 1, DO_ArticulatorySynthesisTutorial);
	praat_addMenuCommand ("Objects", "New", "-- new articulatory synthesis -- ", 0, 1, 0);
	praat_addMenuCommand ("Objects", "New", "Create Articulation...", 0, 1, DO_Art_create);
	praat_addMenuCommand ("Objects", "New", "Create Speaker...", 0, 1, DO_Speaker_create);
	praat_addMenuCommand ("Objects", "New", "Create Artword...", 0, 1, DO_Artword_create);
	/*praat_addMenuCommand ("Objects", "New", "-- figures --", 0, 1, 0);
	praat_addMenuCommand ("Objects", "New", "Figures", 0, 1, 0);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 4.1: many tubes", 0, 2, DO_Fig_manyTubes);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 4.4: tube connections", 0, 2, DO_Fig_tubeConnections);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 4.7: vocal-tract construction", 0, 2, DO_Fig_vocalTractConstruction);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 5.2: pumping and sucking", 0, 2, DO_Fig_pumpingAndSucking);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 5.8: closed boundary", 0, 2, DO_Fig_closedBoundary);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 5.9: open boundary", 0, 2, DO_Fig_openBoundary);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 5.10: two-way boundary", 0, 2, DO_Fig_twoWayBoundary);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 5.11: three-way boundary", 0, 2, DO_Fig_threeWayBoundary);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 6.3: smoothing errors", 0, 2, DO_Fig_smoothingErrors);
	praat_addMenuCommand ("Objects", "New", "PB Fig. 6.8: HNR window ripples", 0, 2, DO_Fig_hnrWindowRipples);*/
	praat_addMenuCommand ("Objects", "New", "-- new vocal tract --", 0, 1, 0);
	praat_addMenuCommand ("Objects", "New", "Create Vocal Tract from phone...", 0, 1, DO_VocalTract_createFromPhone);

	praat_addAction1 (classArt, 1, "Edit", 0, 0, DO_Art_edit);

	praat_addAction1 (classArtword, 0, "Artword help", 0, 0, DO_Artword_help);
	praat_addAction1 (classArtword, 1, "Edit", 0, 0, DO_Artword_edit);
	praat_addAction1 (classArtword, 0, "Info", 0, 0, 0);
	praat_addAction1 (classArtword, 1, "Get target...", 0, 0, DO_Artword_getTarget);
	praat_addAction1 (classArtword, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classArtword, 0, "Draw...", 0, 0, DO_Artword_draw);
	praat_addAction1 (classArtword, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classArtword, 1, "Set target...", 0, 0, DO_Artword_setTarget);
	praat_addAction1 (classArtword, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classArtword, 0, "To Art (slice)...", 0, 0, DO_Artword_to_Art);

	praat_addAction2 (classArt, 1, classSpeaker, 1, "Draw", 0, 0, 0);
	praat_addAction2 (classArt, 1, classSpeaker, 1, "Draw", 0, 0, DO_Art_Speaker_draw);
	praat_addAction2 (classArt, 1, classSpeaker, 1, "Fill inner contour", 0, 0, DO_Art_Speaker_fillInnerContour);
	praat_addAction2 (classArt, 1, classSpeaker, 1, "Draw mesh", 0, 0, DO_Art_Speaker_drawMesh);
	praat_addAction2 (classArt, 1, classSpeaker, 1, "Synthesize", 0, 0, 0);
	praat_addAction2 (classArt, 1, classSpeaker, 1, "To VocalTract", 0, 0, DO_Art_Speaker_to_VocalTract);

	praat_addAction2 (classArtword, 1, classSpeaker, 1, "Movie", 0, 0, DO_Artword_Speaker_movie);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, "Draw", 0, 0, 0);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, "Draw...", 0, 0, DO_Artword_Speaker_draw);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, "Synthesize", 0, 0, 0);
	praat_addAction2 (classArtword, 1, classSpeaker, 1, "To Sound...", 0, 0, DO_Artword_Speaker_to_Sound);

	praat_addAction3 (classArtword, 1, classSpeaker, 1, classSound, 1, "Movie", 0, 0, DO_Artword_Speaker_movie);

	praat_addAction1 (classSpeaker, 0, "Speaker help", 0, 0, DO_Speaker_help);

	praat_addAction1 (classVocalTract, 0, "VocalTract help", 0, 0, DO_VocalTract_help);
	praat_addAction1 (classVocalTract, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, "Draw", 0, 0, DO_VocalTract_draw);
	praat_addAction1 (classVocalTract, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, "To Spectrum...", 0, 0, DO_VocalTract_to_Spectrum);
	praat_addAction1 (classVocalTract, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, "Formula...", 0, 0, DO_VocalTract_formula);
	praat_addAction1 (classVocalTract, 0, "Hack", 0, 0, 0);
	praat_addAction1 (classVocalTract, 0, "To Matrix", 0, 0, DO_VocalTract_to_Matrix);

	INCLUDE_MANPAGES (manual_Artsynth_init)
}

/* End of file praat_Artsynth.c */
