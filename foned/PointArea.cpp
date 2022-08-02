/* PointArea.cpp
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

#include "PointArea.h"
#include "PointProcess_and_Sound.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"
#include "SoundArea.h"

Thing_implement (PointArea, FunctionArea, 0);

#undef DATA_BEGIN__
#define DATA_BEGIN__ \
	Melder_assert (my function());
#undef MODIFY_DATA
#define MODIFY_DATA(undoTitle)  \
	FunctionArea_save (me, undoTitle);
#undef MODIFY_DATA_END
#define MODIFY_DATA_END  \
	Editor_broadcastDataChanged (my functionEditor());


#pragma mark - PointArea drawing

void structPointArea :: v_drawInside () {
	Graphics_setColour (our graphics(), Melder_BLUE);
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), -1.0, +1.0);
	for (integer i = 1; i <= our pointProcess() -> nt; i ++) {
		const double t = our pointProcess() -> t [i];
		if (t >= our startWindow() && t <= our endWindow())
			Graphics_line (our graphics(), t, -0.9, t, +0.9);
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
}


#pragma mark - PointArea Query menu

static void QUERY_DATA_FOR_REAL__getJitter_local (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local (my pointProcess(), my startSelection(), my endSelection(), 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_local_absolute (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local_absolute (my pointProcess(), my startSelection(), my endSelection(), 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U" seconds");
}

static void QUERY_DATA_FOR_REAL__getJitter_rap (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_rap (my pointProcess(), my startSelection(), my endSelection(), 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ppq5 (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ppq5 (my pointProcess(), my startSelection(), my endSelection(), 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ddp (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ddp (my pointProcess(), my startSelection(), my endSelection(), 1e-4, 0.02, 1.3);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local_dB (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local_dB (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U" dB")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq3 (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq3 (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq5 (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq5 (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq11 (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq11 (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_dda (PointArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_dda (my pointProcess(), my borrowedSoundArea -> sound(),
				my startSelection(), my endSelection(), 1e-4, 0.02, 1.3, 1.6);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void MODIFY_DATA__removePoints (PointArea me, EDITOR_ARGS_DIRECT) {
	MODIFY_DATA (U"Remove point(s)")
		if (my startSelection() == my endSelection())
			PointProcess_removePointNear (my pointProcess(), my startSelection());
		else
			PointProcess_removePointsBetween (my pointProcess(), my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAtCursor (PointArea me, EDITOR_ARGS_DIRECT) {
	MODIFY_DATA (U"Add point")
		PointProcess_addPoint (my pointProcess(), 0.5 * (my startSelection() + my endSelection()));
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAt (PointArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (position, U"Position", U"0.0");
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection() + my endSelection()));
	EDITOR_DO
		MODIFY_DATA (U"Add point")
			PointProcess_addPoint (my pointProcess(), position);
		MODIFY_DATA_END
	EDITOR_END
}

void structPointArea :: v_createMenus () {
	PointArea_Parent :: v_createMenus ();

	EditorMenu menu = our functionEditor() -> queryMenu;
	FunctionAreaMenu_addCommand (menu, U"-- query jitter --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Get jitter (local)", 0,
			QUERY_DATA_FOR_REAL__getJitter_local, this);
	FunctionAreaMenu_addCommand (menu, U"Get jitter (local, absolute)", 0,
			QUERY_DATA_FOR_REAL__getJitter_local_absolute, this);
	FunctionAreaMenu_addCommand (menu, U"Get jitter (rap)", 0,
			QUERY_DATA_FOR_REAL__getJitter_rap, this);
	FunctionAreaMenu_addCommand (menu, U"Get jitter (ppq5)", 0,
			QUERY_DATA_FOR_REAL__getJitter_ppq5, this);
	FunctionAreaMenu_addCommand (menu, U"Get jitter (ddp)", 0,
			QUERY_DATA_FOR_REAL__getJitter_ddp, this);
	if (our borrowedSoundArea) {   // BUG: not LongSound
		FunctionAreaMenu_addCommand (menu, U"-- query shimmer --", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (local)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_local, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (local, dB)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_local_dB, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq3)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq3, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq5)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq5, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq11)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_apq11, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (dda)", 0,
				QUERY_DATA_FOR_REAL__getShimmer_dda, this);
	}

	menu = Editor_addMenu (our functionEditor(), U"Point", 0);
	FunctionAreaMenu_addCommand (menu, U"Add point at cursor", 'P',
			MODIFY_DATA__addPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add point at...", 0,
			MODIFY_DATA__addPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"-- remove point --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'P',
			MODIFY_DATA__removePoints, this);
}

/* End of file PointArea.cpp */
