/* PointEditor.c
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
 * pb 2002/07/16 GPL
 * pb 2003/05/17 more shimmer measurements
 * pb 2003/05/21 more jitter measurements
 * pb 2004/04/16 added a fixed maximum period factor of 1.3
 * pb 2004/04/21 less flashing
 * pb 2007/01/28 made compatible with stereo sounds (by converting them to mono)
 */

#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"

#define PointEditor_members FunctionEditor_members \
	int ownSound; \
	Widget addPointAtDialog;
#define PointEditor_methods FunctionEditor_methods
class_create_opaque (PointEditor, FunctionEditor)

static void updateMenus (PointEditor me) {
	Editor_setMenuSensitive (me, "Selection", my endSelection > my startSelection);
}

/********** DESTRUCTION **********/

static void destroy (I) {
	iam (PointEditor);
	if (my ownSound) forget (my sound);
	inherited (PointEditor) destroy (me);
}

/********** MENU COMMANDS **********/

DIRECT (PointEditor, cb_getJitter_local)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
END

DIRECT (PointEditor, cb_getJitter_local_absolute)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local_absolute (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
END

DIRECT (PointEditor, cb_getJitter_rap)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_rap (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
END

DIRECT (PointEditor, cb_getJitter_ppq5)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ppq5 (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
END

DIRECT (PointEditor, cb_getJitter_ddp)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ddp (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
END

DIRECT (PointEditor, cb_getShimmer_local)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_getShimmer_local_dB)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local_dB (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_getShimmer_apq3)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq3 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_getShimmer_apq5)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq5 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_getShimmer_apq11)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq11 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_getShimmer_dda)
	if (my startSelection == my endSelection) return Melder_error ("Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_dda (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
END

DIRECT (PointEditor, cb_removePoints)
	Editor_save (me, "Remove point(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear (my data, my startSelection);
	else
		PointProcess_removePointsBetween (my data, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (PointEditor, cb_addPointAtCursor)
	Editor_save (me, "Add point");
	PointProcess_addPoint (my data, 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (PointEditor, cb_addPointAt, "Add point", 0);
	REAL ("Position", "0.0");
	OK
SET_REAL ("Position", 0.5 * (my startSelection + my endSelection));
DO
	Editor_save (me, "Add point");
	PointProcess_addPoint (my data, GET_REAL ("Position"));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (PointEditor, cb_PointEditorHelp) Melder_help ("PointEditor"); END

static void createMenus (I) {
	iam (PointEditor);
	inherited (PointEditor) createMenus (me);

	Editor_addCommand (me, "Query", "-- query jitter --", 0, NULL);
	Editor_addCommand (me, "Query", "Get jitter (local)", 0, cb_getJitter_local);
	Editor_addCommand (me, "Query", "Get jitter (local, absolute)", 0, cb_getJitter_local_absolute);
	Editor_addCommand (me, "Query", "Get jitter (rap)", 0, cb_getJitter_rap);
	Editor_addCommand (me, "Query", "Get jitter (ppq5)", 0, cb_getJitter_ppq5);
	Editor_addCommand (me, "Query", "Get jitter (ddp)", 0, cb_getJitter_ddp);
	if (my sound.data) {
		Editor_addCommand (me, "Query", "-- query shimmer --", 0, NULL);
		Editor_addCommand (me, "Query", "Get shimmer (local)", 0, cb_getShimmer_local);
		Editor_addCommand (me, "Query", "Get shimmer (local, dB)", 0, cb_getShimmer_local_dB);
		Editor_addCommand (me, "Query", "Get shimmer (apq3)", 0, cb_getShimmer_apq3);
		Editor_addCommand (me, "Query", "Get shimmer (apq5)", 0, cb_getShimmer_apq5);
		Editor_addCommand (me, "Query", "Get shimmer (apq11)", 0, cb_getShimmer_apq11);
		Editor_addCommand (me, "Query", "Get shimmer (dda)", 0, cb_getShimmer_dda);
	}

	Editor_addMenu (me, "Point", 0);
	Editor_addCommand (me, "Point", "Add point at cursor", 'P', cb_addPointAtCursor);
	Editor_addCommand (me, "Point", "Add point at...", 0, cb_addPointAt);
	Editor_addCommand (me, "Point", "-- remove point --", 0, NULL);
	Editor_addCommand (me, "Point", "Remove point(s)", motif_OPTION + 'P', cb_removePoints);

	Editor_addCommand (me, "Help", "PointEditor help", '?', cb_PointEditorHelp);
}

/********** DRAWING AREA **********/

static void draw (I) {
	iam (PointEditor);
	PointProcess point = my data;
	Sound sound = my sound.data;
	long first, last, i;
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, -1.0, 1.0);
	Graphics_setColour (my graphics, Graphics_BLACK);
	if (sound != NULL && Sampled_getWindowSamples (sound, my startWindow, my endWindow, & first, & last) > 1) {
		Graphics_setLineType (my graphics, Graphics_DOTTED);
		Graphics_line (my graphics, my startWindow, 0.0, my endWindow, 0.0);
		Graphics_setLineType (my graphics, Graphics_DRAWN);      
		Graphics_function (my graphics, sound -> z [1], first, last,
			Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
	}
	Graphics_setColour (my graphics, Graphics_BLUE);
	for (i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		if (t >= my startWindow && t <= my endWindow)
			Graphics_line (my graphics, t, -0.9, t, 0.9);
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	updateMenus (me);
}

static void play (I, double tmin, double tmax) {
	iam (PointEditor);
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	} else {
		if (! PointProcess_playPart (my data, tmin, tmax)) Melder_flushError (NULL);
	}
}

class_methods (PointEditor, FunctionEditor)
	class_method (destroy)
	class_method (createMenus)
	class_method (draw)
	class_method (play)
class_methods_end

PointEditor PointEditor_create (Widget parent, const char *title, PointProcess point, Sound sound) {
	PointEditor me = new (PointEditor); cherror
	if (sound) {
		my sound.data = Sound_convertToMono (sound); cherror   /* Deep copy; ownership transferred. */
	}
	FunctionEditor_init (me, parent, title, point); cherror
	updateMenus (me);
end:
	iferror forget (me);
	return me;
}

/* End of file PointEditor.c */
