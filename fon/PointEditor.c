/* PointEditor.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/09/08 inherit from TimeSoundEditor
 * pb 2007/09/22 autoscaling
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 */

#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"

/********** DESTRUCTION **********/

static void destroy (I) {
	iam (PointEditor);
	forget (my monoSound);
	inherited (PointEditor) destroy (me);
}

/********** MENU COMMANDS **********/

static int menu_cb_getJitter_local (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
	return 1;
}

static int menu_cb_getJitter_local_absolute (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local_absolute (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
	return 1;
}

static int menu_cb_getJitter_rap (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_rap (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
	return 1;
}

static int menu_cb_getJitter_ppq5 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ppq5 (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
	return 1;
}

static int menu_cb_getJitter_ddp (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ddp (my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), NULL);
	return 1;
}

static int menu_cb_getShimmer_local (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_getShimmer_local_dB (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local_dB (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_getShimmer_apq3 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq3 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_getShimmer_apq5 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq5 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_getShimmer_apq11 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq11 (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_getShimmer_dda (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my startSelection == my endSelection) return Melder_error1 (L"Make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_dda (my data, my sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
	return 1;
}

static int menu_cb_removePoints (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	Editor_save (PointEditor_as_Editor (me), L"Remove point(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear (my data, my startSelection);
	else
		PointProcess_removePointsBetween (my data, my startSelection, my endSelection);
	FunctionEditor_redraw (PointEditor_as_FunctionEditor (me));
	Editor_broadcastChange (PointEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	Editor_save (PointEditor_as_Editor (me), L"Add point");
	PointProcess_addPoint (my data, 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (PointEditor_as_FunctionEditor (me));
	Editor_broadcastChange (PointEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	EDITOR_FORM (L"Add point", 0)
		REAL (L"Position", L"0.0");
	EDITOR_OK
		SET_REAL (L"Position", 0.5 * (my startSelection + my endSelection));
	EDITOR_DO
		Editor_save (PointEditor_as_Editor (me), L"Add point");
		PointProcess_addPoint (my data, GET_REAL (L"Position"));
		FunctionEditor_redraw (PointEditor_as_FunctionEditor (me));
		Editor_broadcastChange (PointEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_PointEditorHelp (EDITOR_ARGS) { EDITOR_IAM (PointEditor); Melder_help (L"PointEditor"); return 1; }

static void createMenus (PointEditor me) {
	inherited (PointEditor) createMenus (PointEditor_as_parent (me));

	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"-- query jitter --", 0, NULL);
	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get jitter (local)", 0, menu_cb_getJitter_local);
	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get jitter (local, absolute)", 0, menu_cb_getJitter_local_absolute);
	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get jitter (rap)", 0, menu_cb_getJitter_rap);
	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get jitter (ppq5)", 0, menu_cb_getJitter_ppq5);
	Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get jitter (ddp)", 0, menu_cb_getJitter_ddp);
	if (my sound.data) {
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"-- query shimmer --", 0, NULL);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (local)", 0, menu_cb_getShimmer_local);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (local, dB)", 0, menu_cb_getShimmer_local_dB);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (apq3)", 0, menu_cb_getShimmer_apq3);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (apq5)", 0, menu_cb_getShimmer_apq5);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (apq11)", 0, menu_cb_getShimmer_apq11);
		Editor_addCommand (PointEditor_as_Editor (me), L"Query", L"Get shimmer (dda)", 0, menu_cb_getShimmer_dda);
	}

	Editor_addMenu (PointEditor_as_Editor (me), L"Point", 0);
	Editor_addCommand (PointEditor_as_Editor (me), L"Point", L"Add point at cursor", 'P', menu_cb_addPointAtCursor);
	Editor_addCommand (PointEditor_as_Editor (me), L"Point", L"Add point at...", 0, menu_cb_addPointAt);
	Editor_addCommand (PointEditor_as_Editor (me), L"Point", L"-- remove point --", 0, NULL);
	Editor_addCommand (PointEditor_as_Editor (me), L"Point", L"Remove point(s)", GuiMenu_OPTION + 'P', menu_cb_removePoints);
}

static void createHelpMenuItems (PointEditor me, EditorMenu menu) {
	inherited (PointEditor) createHelpMenuItems (PointEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"PointEditor help", '?', menu_cb_PointEditorHelp);
}

/********** DRAWING AREA **********/

static void draw (PointEditor me) {
	PointProcess point = my data;
	Sound sound = my sound.data;
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	double minimum = -1.0, maximum = +1.0;
	if (sound != NULL && my sound.autoscaling) {
		long first, last;
		if (Sampled_getWindowSamples (sound, my startWindow, my endWindow, & first, & last) >= 1) {
			Matrix_getWindowExtrema (sound, first, last, 1, 1, & minimum, & maximum);
		}
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
	Graphics_setColour (my graphics, Graphics_BLACK);
	if (sound != NULL) {
		long first, last;
		if (Sampled_getWindowSamples (sound, my startWindow, my endWindow, & first, & last) > 1) {
			Graphics_setLineType (my graphics, Graphics_DOTTED);
			Graphics_line (my graphics, my startWindow, 0.0, my endWindow, 0.0);
			Graphics_setLineType (my graphics, Graphics_DRAWN);      
			Graphics_function (my graphics, sound -> z [1], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		}
	}
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, -1.0, +1.0);
	for (long i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		if (t >= my startWindow && t <= my endWindow)
			Graphics_line (my graphics, t, -0.9, t, +0.9);
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	our updateMenuItems_file (me);
}

static void play (PointEditor me, double tmin, double tmax) {
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	} else {
		if (! PointProcess_playPart (my data, tmin, tmax)) Melder_flushError (NULL);
	}
}

class_methods (PointEditor, TimeSoundEditor) {
	class_method (destroy)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (draw)
	class_method (play)
	class_methods_end
}

PointEditor PointEditor_create (Widget parent, const wchar_t *title, PointProcess point, Sound sound) {
	PointEditor me = new (PointEditor); cherror
	if (sound) {
		my monoSound = Sound_convertToMono (sound); cherror
	}
	TimeSoundEditor_init (PointEditor_as_parent (me), parent, title, point, my monoSound, false); cherror
end:
	iferror forget (me);
	return me;
}

/* End of file PointEditor.c */
