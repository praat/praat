/* PointEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"

Thing_implement (PointEditor, TimeSoundEditor, 0);

/********** DESTRUCTION **********/

void structPointEditor :: v_destroy () {
	forget (monoSound);
	PointEditor_Parent :: v_destroy ();
}

/********** MENU COMMANDS **********/

static void menu_cb_getJitter_local (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local ((PointProcess) my data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3), NULL);
}

static void menu_cb_getJitter_local_absolute (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_local_absolute ((PointProcess) my data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3), L"seconds");
}

static void menu_cb_getJitter_rap (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_rap ((PointProcess) my data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3), NULL);
}

static void menu_cb_getJitter_ppq5 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ppq5 ((PointProcess) my data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3), NULL);
}

static void menu_cb_getJitter_ddp (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure jitter, make a selection first.");
	Melder_informationReal (PointProcess_getJitter_ddp ((PointProcess) my data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3), NULL);
}

static void menu_cb_getShimmer_local (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_getShimmer_local_dB (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_local_dB ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_getShimmer_apq3 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq3 ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_getShimmer_apq5 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq5 ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_getShimmer_apq11 (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_apq11 ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_getShimmer_dda (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	if (my d_startSelection == my d_endSelection) Melder_throw ("To measure shimmer, make a selection first.");
	Melder_informationReal (PointProcess_Sound_getShimmer_dda ((PointProcess) my data, my d_sound.data, my d_startSelection, my d_endSelection, 1e-4, 0.02, 1.3, 1.6), NULL);
}

static void menu_cb_removePoints (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	Editor_save (me, L"Remove point(s)");
	if (my d_startSelection == my d_endSelection)
		PointProcess_removePointNear ((PointProcess) my data, my d_startSelection);
	else
		PointProcess_removePointsBetween ((PointProcess) my data, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	Editor_save (me, L"Add point");
	PointProcess_addPoint ((PointProcess) my data, 0.5 * (my d_startSelection + my d_endSelection));
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (PointEditor);
	EDITOR_FORM (L"Add point", 0)
		REAL (L"Position", L"0.0");
	EDITOR_OK
		SET_REAL (L"Position", 0.5 * (my d_startSelection + my d_endSelection));
	EDITOR_DO
		Editor_save (me, L"Add point");
		PointProcess_addPoint ((PointProcess) my data, GET_REAL (L"Position"));
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_PointEditorHelp (EDITOR_ARGS) { EDITOR_IAM (PointEditor); Melder_help (L"PointEditor"); }

void structPointEditor :: v_createMenus () {
	PointEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, L"Query", L"-- query jitter --", 0, NULL);
	Editor_addCommand (this, L"Query", L"Get jitter (local)", 0, menu_cb_getJitter_local);
	Editor_addCommand (this, L"Query", L"Get jitter (local, absolute)", 0, menu_cb_getJitter_local_absolute);
	Editor_addCommand (this, L"Query", L"Get jitter (rap)", 0, menu_cb_getJitter_rap);
	Editor_addCommand (this, L"Query", L"Get jitter (ppq5)", 0, menu_cb_getJitter_ppq5);
	Editor_addCommand (this, L"Query", L"Get jitter (ddp)", 0, menu_cb_getJitter_ddp);
	if (d_sound.data) {
		Editor_addCommand (this, L"Query", L"-- query shimmer --", 0, NULL);
		Editor_addCommand (this, L"Query", L"Get shimmer (local)", 0, menu_cb_getShimmer_local);
		Editor_addCommand (this, L"Query", L"Get shimmer (local, dB)", 0, menu_cb_getShimmer_local_dB);
		Editor_addCommand (this, L"Query", L"Get shimmer (apq3)", 0, menu_cb_getShimmer_apq3);
		Editor_addCommand (this, L"Query", L"Get shimmer (apq5)", 0, menu_cb_getShimmer_apq5);
		Editor_addCommand (this, L"Query", L"Get shimmer (apq11)", 0, menu_cb_getShimmer_apq11);
		Editor_addCommand (this, L"Query", L"Get shimmer (dda)", 0, menu_cb_getShimmer_dda);
	}

	Editor_addMenu (this, L"Point", 0);
	Editor_addCommand (this, L"Point", L"Add point at cursor", 'P', menu_cb_addPointAtCursor);
	Editor_addCommand (this, L"Point", L"Add point at...", 0, menu_cb_addPointAt);
	Editor_addCommand (this, L"Point", L"-- remove point --", 0, NULL);
	Editor_addCommand (this, L"Point", L"Remove point(s)", GuiMenu_OPTION + 'P', menu_cb_removePoints);
}

void structPointEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PointEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"PointEditor help", '?', menu_cb_PointEditorHelp);
}

/********** DRAWING AREA **********/

void structPointEditor :: v_draw () {
	PointProcess point = (PointProcess) data;
	Sound sound = d_sound.data;
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	double minimum = -1.0, maximum = +1.0;
	if (sound != NULL && (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_BY_WINDOW || p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_BY_WINDOW_AND_CHANNEL)) {
		long first, last;
		if (Sampled_getWindowSamples (sound, d_startWindow, d_endWindow, & first, & last) >= 1) {
			Matrix_getWindowExtrema (sound, first, last, 1, 1, & minimum, & maximum);
		}
	}
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, minimum, maximum);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	if (sound != NULL) {
		long first, last;
		if (Sampled_getWindowSamples (sound, d_startWindow, d_endWindow, & first, & last) > 1) {
			Graphics_setLineType (d_graphics, Graphics_DOTTED);
			Graphics_line (d_graphics, d_startWindow, 0.0, d_endWindow, 0.0);
			Graphics_setLineType (d_graphics, Graphics_DRAWN);      
			Graphics_function (d_graphics, sound -> z [1], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		}
	}
	Graphics_setColour (d_graphics, Graphics_BLUE);
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, -1.0, +1.0);
	for (long i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		if (t >= d_startWindow && t <= d_endWindow)
			Graphics_line (d_graphics, t, -0.9, t, +0.9);
	}
	Graphics_setColour (d_graphics, Graphics_BLACK);
	v_updateMenuItems_file ();
}

void structPointEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	} else {
		PointProcess_playPart ((PointProcess) data, a_tmin, a_tmax);
	}
}

PointEditor PointEditor_create (const wchar_t *title, PointProcess point, Sound sound) {
	try {
		autoPointEditor me = Thing_new (PointEditor);
		if (sound) {
			my monoSound = Sound_convertToMono (sound);
		}
		my structTimeSoundEditor :: f_init (title, point, my monoSound, false);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PointProcess window not created.");
	}
}

/* End of file PointEditor.cpp */
