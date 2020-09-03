/* PointEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,2014,2015,2016,2017 Paul Boersma
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

#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"

Thing_implement (PointEditor, TimeSoundEditor, 0);

/********** DESTRUCTION **********/

void structPointEditor :: v_destroy () noexcept {
	PointEditor_Parent :: v_destroy ();
}

/********** MENU COMMANDS **********/

static void menu_cb_getJitter_local (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local ((PointProcess) my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), nullptr);
}

static void menu_cb_getJitter_local_absolute (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local_absolute ((PointProcess) my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), U"seconds");
}

static void menu_cb_getJitter_rap (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_rap ((PointProcess) my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), nullptr);
}

static void menu_cb_getJitter_ppq5 (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ppq5 ((PointProcess) my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), nullptr);
}

static void menu_cb_getJitter_ddp (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ddp ((PointProcess) my data, my startSelection, my endSelection, 1e-4, 0.02, 1.3), nullptr);
}

static void menu_cb_getShimmer_local (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_getShimmer_local_dB (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local_dB ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_getShimmer_apq3 (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq3 ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_getShimmer_apq5 (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq5 ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_getShimmer_apq11 (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq11 ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_getShimmer_dda (PointEditor me, EDITOR_ARGS_DIRECT) {
	if (my startSelection == my endSelection) Melder_throw (U"To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_dda ((PointProcess) my data, my d_sound.data, my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6), nullptr);
}

static void menu_cb_removePoints (PointEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Remove point(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear ((PointProcess) my data, my startSelection);
	else
		PointProcess_removePointsBetween ((PointProcess) my data, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (PointEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Add point");
	PointProcess_addPoint ((PointProcess) my data, 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (PointEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (position, U"Position", U"0.0");
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection + my endSelection));
	EDITOR_DO
		Editor_save (me, U"Add point");
		PointProcess_addPoint ((PointProcess) my data, position);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_PointEditorHelp (PointEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"PointEditor"); }

void structPointEditor :: v_createMenus () {
	PointEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"Query", U"-- query jitter --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get jitter (local)", 0, menu_cb_getJitter_local);
	Editor_addCommand (this, U"Query", U"Get jitter (local, absolute)", 0, menu_cb_getJitter_local_absolute);
	Editor_addCommand (this, U"Query", U"Get jitter (rap)", 0, menu_cb_getJitter_rap);
	Editor_addCommand (this, U"Query", U"Get jitter (ppq5)", 0, menu_cb_getJitter_ppq5);
	Editor_addCommand (this, U"Query", U"Get jitter (ddp)", 0, menu_cb_getJitter_ddp);
	if (d_sound.data) {
		Editor_addCommand (this, U"Query", U"-- query shimmer --", 0, nullptr);
		Editor_addCommand (this, U"Query", U"Get shimmer (local)", 0, menu_cb_getShimmer_local);
		Editor_addCommand (this, U"Query", U"Get shimmer (local, dB)", 0, menu_cb_getShimmer_local_dB);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq3)", 0, menu_cb_getShimmer_apq3);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq5)", 0, menu_cb_getShimmer_apq5);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq11)", 0, menu_cb_getShimmer_apq11);
		Editor_addCommand (this, U"Query", U"Get shimmer (dda)", 0, menu_cb_getShimmer_dda);
	}

	Editor_addMenu (this, U"Point", 0);
	Editor_addCommand (this, U"Point", U"Add point at cursor", 'P', menu_cb_addPointAtCursor);
	Editor_addCommand (this, U"Point", U"Add point at...", 0, menu_cb_addPointAt);
	Editor_addCommand (this, U"Point", U"-- remove point --", 0, nullptr);
	Editor_addCommand (this, U"Point", U"Remove point(s)", GuiMenu_OPTION | 'P', menu_cb_removePoints);
}

void structPointEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PointEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"PointEditor help", '?', menu_cb_PointEditorHelp);
}

/********** DRAWING AREA **********/

void structPointEditor :: v_draw () {
	PointProcess point = static_cast <PointProcess> (our data);
	Sound sound = d_sound.data;
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	double minimum = -1.0, maximum = +1.0;
	if (sound && (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy::BY_WINDOW || p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy::BY_WINDOW_AND_CHANNEL)) {
		integer first, last;
		if (Sampled_getWindowSamples (sound, our startWindow, our endWindow, & first, & last) >= 1) {
			Matrix_getWindowExtrema (sound, first, last, 1, 1, & minimum, & maximum);
			if (minimum == maximum) {
				minimum -= 1.0;
				maximum += 1.0;
			}
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, minimum, maximum);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	if (sound) {
		integer first, last;
		if (Sampled_getWindowSamples (sound, our startWindow, our endWindow, & first, & last) > 1) {
			Graphics_setLineType (our graphics.get(), Graphics_DOTTED);
			Graphics_line (our graphics.get(), our startWindow, 0.0, our endWindow, 0.0);
			Graphics_setLineType (our graphics.get(), Graphics_DRAWN);
			Graphics_function (our graphics.get(), & sound -> z [1] [0], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		}
	}
	Graphics_setColour (our graphics.get(), Melder_BLUE);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, -1.0, +1.0);
	for (integer i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		if (t >= our startWindow && t <= our endWindow)
			Graphics_line (our graphics.get(), t, -0.9, t, +0.9);
	}
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	v_updateMenuItems_file ();
}

void structPointEditor :: v_play (double startTime, double endTime) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		PointProcess_playPart ((PointProcess) data, startTime, endTime);
	}
}

autoPointEditor PointEditor_create (conststring32 title, PointProcess point, Sound sound) {
	try {
		autoPointEditor me = Thing_new (PointEditor);
		if (sound) {
			my monoSound = Sound_convertToMono (sound);
		}
		TimeSoundEditor_init (me.get(), title, point, my monoSound.get(), false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess window not created.");
	}
}

/* End of file PointEditor.cpp */
