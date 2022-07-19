/* PointEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#pragma mark - MENU COMMANDS

static void QUERY_DATA_FOR_REAL__getJitter_local (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local (my pointProcess(), my startSelection, my endSelection, 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_local_absolute (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local_absolute (my pointProcess(), my startSelection, my endSelection, 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U" seconds");
}

static void QUERY_DATA_FOR_REAL__getJitter_rap (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_rap (my pointProcess(), my startSelection, my endSelection, 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ppq5 (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ppq5 (my pointProcess(), my startSelection, my endSelection, 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ddp (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ddp (my pointProcess(), my startSelection, my endSelection, 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local_dB (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local_dB (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U" dB")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq3 (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq3 (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq5 (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq5 (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq11 (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq11 (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_dda (PointEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection != my endSelection,
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_dda (my pointProcess(), my sound(), my startSelection, my endSelection, 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void MODIFY_DATA__removePoints (PointEditor me, EDITOR_ARGS_DIRECT) {
	MODIFY_DATA (U"Remove point(s)")
		if (my startSelection == my endSelection)
			PointProcess_removePointNear (my pointProcess(), my startSelection);
		else
			PointProcess_removePointsBetween (my pointProcess(), my startSelection, my endSelection);
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAtCursor (PointEditor me, EDITOR_ARGS_DIRECT) {
	MODIFY_DATA (U"Add point")
		PointProcess_addPoint (my pointProcess(), 0.5 * (my startSelection + my endSelection));
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAt (PointEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (position, U"Position", U"0.0");
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection + my endSelection));
	EDITOR_DO
		MODIFY_DATA (U"Add point")
			PointProcess_addPoint (my pointProcess(), position);
		MODIFY_DATA_END
	EDITOR_END
}

static void HELP__PointEditorHelp (PointEditor, EDITOR_ARGS_DIRECT) {
	HELP (U"PointEditor")
}

void structPointEditor :: v_createMenus () {
	PointEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"Query", U"-- query jitter --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get jitter (local)", 0,
			QUERY_DATA_FOR_REAL__getJitter_local);
	Editor_addCommand (this, U"Query", U"Get jitter (local, absolute)", 0,
			QUERY_DATA_FOR_REAL__getJitter_local_absolute);
	Editor_addCommand (this, U"Query", U"Get jitter (rap)", 0,
			QUERY_DATA_FOR_REAL__getJitter_rap);
	Editor_addCommand (this, U"Query", U"Get jitter (ppq5)", 0,
			QUERY_DATA_FOR_REAL__getJitter_ppq5);
	Editor_addCommand (this, U"Query", U"Get jitter (ddp)", 0,
			QUERY_DATA_FOR_REAL__getJitter_ddp);
	if (our soundArea) {   // BUG: not LongSound
		Editor_addCommand (this, U"Query", U"-- query shimmer --", 0, nullptr);
		Editor_addCommand (this, U"Query", U"Get shimmer (local)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_local);
		Editor_addCommand (this, U"Query", U"Get shimmer (local, dB)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_local_dB);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq3)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq3);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq5)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq5);
		Editor_addCommand (this, U"Query", U"Get shimmer (apq11)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq11);
		Editor_addCommand (this, U"Query", U"Get shimmer (dda)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_dda);
	}

	Editor_addMenu (this, U"Point", 0);
	Editor_addCommand (this, U"Point", U"Add point at cursor", 'P',
			MODIFY_DATA__addPointAtCursor);
	Editor_addCommand (this, U"Point", U"Add point at...", 0,
			MODIFY_DATA__addPointAt);
	Editor_addCommand (this, U"Point", U"-- remove point --", 0, nullptr);
	Editor_addCommand (this, U"Point", U"Remove point(s)", GuiMenu_OPTION | 'P',
			MODIFY_DATA__removePoints);
}

void structPointEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PointEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"PointEditor help", '?',
			HELP__PointEditorHelp);
}

#pragma mark - DRAW

void structPointEditor :: v_draw () {
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	double minimum = -1.0, maximum = +1.0;
	if (our sound() && (our soundArea -> instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW ||
			our soundArea -> instancePref_scalingStrategy() == kSoundArea_scalingStrategy::BY_WINDOW_AND_CHANNEL))
	{
		integer first, last;
		if (Sampled_getWindowSamples (our sound(), our startWindow, our endWindow, & first, & last) >= 1) {
			Matrix_getWindowExtrema (our sound(), first, last, 1, 1, & minimum, & maximum);
			if (minimum == maximum) {
				minimum -= 1.0;
				maximum += 1.0;
			}
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, minimum, maximum);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	if (our sound()) {
		integer first, last;
		if (Sampled_getWindowSamples (our sound(), our startWindow, our endWindow, & first, & last) > 1) {
			Graphics_setLineType (our graphics.get(), Graphics_DOTTED);
			Graphics_line (our graphics.get(), our startWindow, 0.0, our endWindow, 0.0);
			Graphics_setLineType (our graphics.get(), Graphics_DRAWN);
			Graphics_function (our graphics.get(), & our sound() -> z [1] [0], first, last,
				Sampled_indexToX (our sound(), first), Sampled_indexToX (our sound(), last));
		}
	}
	Graphics_setColour (our graphics.get(), Melder_BLUE);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, -1.0, +1.0);
	for (integer i = 1; i <= our pointProcess() -> nt; i ++) {
		const double t = our pointProcess() -> t [i];
		if (t >= our startWindow && t <= our endWindow)
			Graphics_line (our graphics.get(), t, -0.9, t, +0.9);
	}
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	our v_updateMenuItems ();
}

#pragma mark - PLAY

void structPointEditor :: v_play (double startTime, double endTime) {
	if (our sound())
		Sound_playPart (our sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	else
		PointProcess_playPart (our pointProcess(), startTime, endTime);
}

#pragma mark - CREATE

autoPointEditor PointEditor_create (conststring32 title, PointProcess pointProcess, Sound sound) {
	try {
		autoPointEditor me = Thing_new (PointEditor);
		if (sound) {
			autoSound monoSound = Sound_convertToMono (sound);
			my soundArea = SoundArea_create (false, monoSound.get(), me.get());   // BUG: double copy
		}
		FunctionEditor_init (me.get(), title, pointProcess);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess window not created.");
	}
}

/* End of file PointEditor.cpp */
