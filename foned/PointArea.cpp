/* PointArea.cpp
 *
 * Copyright (C) 1992-2024 Paul Boersma
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

#include "Prefs_define.h"
#include "PointArea_prefs.h"
#include "Prefs_install.h"
#include "PointArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "PointArea_prefs.h"


#pragma mark - PointArea settings

static void menu_cb_pulsesSettings (PointArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pulses settings", U"Pulses settings...")
		COMMENT   (U"Settings that influence both jitter and shimmer:")
		POSITIVE (periodFloor,            U"Period floor (s)",         my default_periodFloor            ())
		POSITIVE (periodCeiling,          U"Period ceiling (s)",       my default_periodCeiling          ())
		POSITIVE (maximumPeriodFactor,    U"Maximum period factor",    my default_maximumPeriodFactor    ())
		COMMENT   (U"A setting that influences shimmer only:")
		POSITIVE (maximumAmplitudeFactor, U"Maximum amplitude factor", my default_maximumAmplitudeFactor ())
	EDITOR_OK
		SET_REAL (periodFloor,            my instancePref_periodFloor())
		SET_REAL (periodCeiling,          my instancePref_periodCeiling())
		SET_REAL (maximumPeriodFactor,    my instancePref_maximumPeriodFactor())
		SET_REAL (maximumAmplitudeFactor, my instancePref_maximumAmplitudeFactor())
	EDITOR_DO
		Melder_require (periodCeiling > periodFloor,
			U"The period ceiling should be greater than the period floor.");
		Melder_require (maximumPeriodFactor > 1.0,
			U"The maximmum period factor should be greater than 1.0.");
		Melder_require (maximumAmplitudeFactor > 1.0,
			U"The maximmum amplitude factor should be greater than 1.0.");
		my setInstancePref_periodFloor (periodFloor);
		my setInstancePref_periodCeiling (periodCeiling);
		my setInstancePref_maximumPeriodFactor (maximumPeriodFactor);
		my setInstancePref_maximumAmplitudeFactor (maximumAmplitudeFactor);
		//FunctionEditor_redraw (my functionEditor());   // include once voiced stretches are visualized
	EDITOR_END
}


#pragma mark - PointArea drawing

void structPointArea :: v_drawInside () {
	Graphics_setColour (our graphics(), our editable() ? DataGuiColour_EDITABLE : Melder_SILVER);
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), -1.0, +1.0);
	Graphics_setLineWidth (our graphics(), 2.0);
	for (integer i = 1; i <= our pointProcess() -> nt; i ++) {
		const double t = our pointProcess() -> t [i];
		if (t >= our startWindow() && t <= our endWindow())
			Graphics_line (our graphics(), t, -0.9, t, +0.9);
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
	Graphics_setLineWidth (our graphics(), 1.0);
}


#pragma mark - PointArea Query submenu

static void QUERY_DATA_FOR_REAL__getJitter_local (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local (my pointProcess(), my startSelection(), my endSelection(),
				my instancePref_periodFloor(), my instancePref_periodCeiling(), my instancePref_maximumPeriodFactor());
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_local_absolute (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_local_absolute (my pointProcess(), my startSelection(), my endSelection(),
				my instancePref_periodFloor(), my instancePref_periodCeiling(), my instancePref_maximumPeriodFactor());
	QUERY_DATA_FOR_REAL_END (U" seconds");
}

static void QUERY_DATA_FOR_REAL__getJitter_rap (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_rap (my pointProcess(), my startSelection(), my endSelection(),
				my instancePref_periodFloor(), my instancePref_periodCeiling(), my instancePref_maximumPeriodFactor());
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ppq5 (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ppq5 (my pointProcess(), my startSelection(), my endSelection(),
				my instancePref_periodFloor(), my instancePref_periodCeiling(), my instancePref_maximumPeriodFactor());
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getJitter_ddp (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure jitter, make a selection first.");
		const double result = PointProcess_getJitter_ddp (my pointProcess(), my startSelection(), my endSelection(),
				my instancePref_periodFloor(), my instancePref_periodCeiling(), my instancePref_maximumPeriodFactor());
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_local_dB (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_local_dB (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U" dB")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq3 (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq3 (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq5 (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq5 (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_apq11 (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_apq11 (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void QUERY_DATA_FOR_REAL__getShimmer_dda (PointArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		Melder_require (my startSelection() != my endSelection(),
			U"To measure shimmer, make a selection first.");
		const double result = PointProcess_Sound_getShimmer_dda (
			my pointProcess(), my borrowedSoundArea -> sound(),
			my startSelection(), my endSelection(),
			my instancePref_periodFloor(), my instancePref_periodCeiling(),
			my instancePref_maximumPeriodFactor(), my instancePref_maximumAmplitudeFactor()
		);
	QUERY_DATA_FOR_REAL_END (U"")
}

static void MODIFY_DATA__removePoints (PointArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Remove point(s)")
		if (my startSelection() == my endSelection())
			PointProcess_removePointNear (my pointProcess(), my startSelection());
		else
			PointProcess_removePointsBetween (my pointProcess(), my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAtCursor (PointArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Add point")
		PointProcess_addPoint (my pointProcess(), 0.5 * (my startSelection() + my endSelection()));
	MODIFY_DATA_END
}

static void MODIFY_DATA__addPointAt (PointArea me, EDITOR_ARGS) {
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

	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pulses", 0);

	FunctionAreaMenu_addCommand (menu, U"- Pulses settings:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Pulses settings...", 0, menu_cb_pulsesSettings, this);

	FunctionAreaMenu_addCommand (menu, U"- Modify pulses:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Add point at cursor", 'P',
			MODIFY_DATA__addPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add point at...", 0,
			MODIFY_DATA__addPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"-- remove point --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'P',
			MODIFY_DATA__removePoints, this);

	FunctionAreaMenu_addCommand (menu, U"- Query selected pulses:", 0, nullptr, this);
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
		FunctionAreaMenu_addCommand (menu, U"- Query selected pulses and sound:", 0, nullptr, this);
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
}

/* End of file PointArea.cpp */
