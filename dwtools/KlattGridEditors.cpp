/* KlattGridEditors.cpp
 *
 * Copyright (C) 2009-2019 David Weenink, 2021 Paul Boersma
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

/*
 * djmw 20090123
 * djmw 20090128 Remove source menu from formant grid editor.
 * djmw 20090420 dbEditor
 * djmw 20090527 Protect FormantGridEditor against empty FormantGrids.
 * djmw 20110304 Thing_new
 */

#include "KlattGridEditors.h"
#include "EditorM.h"

static void KlattGrid_Editor_defaultPlay (KlattGrid me, double tmin, double tmax) {
	my options -> xmin = tmin;
	my options-> xmax = tmax;
	KlattGrid_playSpecial (me);
}

/************************** KlattGrid_RealTierEditor *********************************/

Thing_implement (KlattGrid_RealTierEditor, RealTierEditor, 0);

static void menu_cb_KlattGridHelp (KlattGrid_RealTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"KlattGrid");
}

void structKlattGrid_RealTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	KlattGrid_RealTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"KlattGrid help", 0, menu_cb_KlattGridHelp);
}

void structKlattGrid_RealTierEditor :: v_play (double startTime, double endTime) {
	KlattGrid_Editor_defaultPlay (our klattgrid(), startTime, endTime);
}

/************************** KlattGrid_PitchTierEditor *********************************/

Thing_implement (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor, 0);

static void menu_cb_KlattGrid_PitchTierEditorHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"PitchTierEditor");
}

static void menu_cb_PitchTierHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"PitchTier");
}

void structKlattGrid_PitchTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> pitch.get();
	KlattGrid_PitchTierEditor_Parent :: v_dataChanged ();
}

void structKlattGrid_PitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_KlattGrid_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_PitchTierEditor me = Thing_new (KlattGrid_PitchTierEditor);
		my realTierArea = KlattGrid_PitchTierArea_create (me.get(), klattgrid -> phonation -> pitch.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid pitch window not created.");
	}
}

/************************** KlattGrid_IntensityTierEditor *********************************/

Thing_implement (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor, 0);

static void menu_cb_IntensityTierHelp (KlattGrid_IntensityTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"IntensityTier");
}

void structKlattGrid_IntensityTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	KlattGrid_IntensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

/************************** KlattGrid_DecibelTierEditor *********************************/

Thing_implement (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_DecibelTierEditor :: v_dataChanged () {
	our realTierArea -> function = KlattGrid_getAddressOfAmplitudes (our klattgrid(), our formantType)->at [our iformant];
	KlattGrid_DecibelTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid,
	kKlattGridFormantType formantType, integer iformant)
{
	try {
		autoKlattGrid_DecibelTierEditor me = Thing_new (KlattGrid_DecibelTierEditor);
		my realTierArea = KlattGrid_DecibelTierArea_create (me.get(),
				KlattGrid_getAddressOfAmplitudes (klattgrid, formantType)->at [iformant]);
		my formantType = formantType;
		my iformant = iformant;
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid decibel window not created.");
	}
}

/************************** KlattGrid_VoicingAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_VoicingAmplitudeTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> voicingAmplitude.get();
	KlattGrid_VoicingAmplitudeTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_VoicingAmplitudeTierEditor me = Thing_new (KlattGrid_VoicingAmplitudeTierEditor);
		my realTierArea = KlattGrid_VoicingAmplitudeTierArea_create (me.get(), klattgrid -> phonation -> voicingAmplitude.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid voicing amplitude window not created.");
	}
}

/************************** KlattGrid_AspirationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_AspirationAmplitudeTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> aspirationAmplitude.get();
	KlattGrid_AspirationAmplitudeTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_AspirationAmplitudeTierEditor me = Thing_new (KlattGrid_AspirationAmplitudeTierEditor);
		my realTierArea = KlattGrid_AspirationAmplitudeTierArea_create (me.get(), klattgrid -> phonation -> aspirationAmplitude.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid aspiration amplitude window not created.");
	}
}

/************************** KlattGrid_BreathinessAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_BreathinessAmplitudeTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> breathinessAmplitude.get();
	KlattGrid_BreathinessAmplitudeTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_BreathinessAmplitudeTierEditor me = Thing_new (KlattGrid_BreathinessAmplitudeTierEditor);
		my realTierArea = KlattGrid_BreathinessAmplitudeTierArea_create (me.get(), klattgrid -> phonation -> breathinessAmplitude.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid breathiness amplitude window not created.");
	}
}

/************************** KlattGrid_SpectralTiltTierEditor *********************************/

Thing_implement (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_SpectralTiltTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> spectralTilt.get();
	KlattGrid_SpectralTiltTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_SpectralTiltTierEditor me = Thing_new (KlattGrid_SpectralTiltTierEditor);
		my realTierArea = KlattGrid_SpectralTiltTierArea_create (me.get(), klattgrid -> phonation -> spectralTilt.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid spectral tilt window not created.");
	}
}

/************************** KlattGrid_FricationBypassTierEditor *********************************/

Thing_implement (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_FricationBypassTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> frication -> bypass.get();
	KlattGrid_FricationBypassTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationBypassTierEditor me = Thing_new (KlattGrid_FricationBypassTierEditor);
		my realTierArea = KlattGrid_FricationBypassTierArea_create (me.get(), klattgrid -> frication -> bypass.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication bypass window not created.");
	}
}

/************************** KlattGrid_FricationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

void structKlattGrid_FricationAmplitudeTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> frication -> fricationAmplitude.get();
	KlattGrid_FricationAmplitudeTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationAmplitudeTierEditor me = Thing_new (KlattGrid_FricationAmplitudeTierEditor);
		my realTierArea = KlattGrid_FricationAmplitudeTierArea_create (me.get(), klattgrid -> frication -> fricationAmplitude.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication amplitude window not created.");
	}
}

/************************** KlattGrid_OpenPhaseTierEditor *********************************/

Thing_implement (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_OpenPhaseTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> openPhase.get();
	KlattGrid_OpenPhaseTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_OpenPhaseTierEditor me = Thing_new (KlattGrid_OpenPhaseTierEditor);
		my realTierArea = KlattGrid_OpenPhaseTierArea_create (me.get(), klattgrid -> phonation -> openPhase.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid open phase window not created.");
	}
}

/************************** KlattGrid_CollisionPhaseTierEditor *********************************/

Thing_implement (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_CollisionPhaseTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> collisionPhase.get();
	KlattGrid_CollisionPhaseTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_CollisionPhaseTierEditor me = Thing_new (KlattGrid_CollisionPhaseTierEditor);
		my realTierArea = KlattGrid_CollisionPhaseTierArea_create (me.get(), klattgrid -> phonation -> collisionPhase.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid collision phase window not created.");
	}
}

/************************** KlattGrid_Power1TierEditor *********************************/

Thing_implement (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_Power1TierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> power1.get();
	KlattGrid_Power1TierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power1TierEditor me = Thing_new (KlattGrid_Power1TierEditor);
		my realTierArea = KlattGrid_Power1TierArea_create (me.get(), klattgrid -> phonation -> power1.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power1 window not created.");
	}
}

/************************** KlattGrid_Power2TierEditor *********************************/

Thing_implement (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_Power2TierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> power2.get();
	KlattGrid_Power2TierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power2TierEditor me = Thing_new (KlattGrid_Power2TierEditor);
		my realTierArea = KlattGrid_Power2TierArea_create (me.get(), klattgrid -> phonation -> power2.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power2 window not created.");
	}
}

/************************** KlattGrid_FlutterTierEditor *********************************/

Thing_implement (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_FlutterTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> flutter.get();
	KlattGrid_FlutterTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FlutterTierEditor me = Thing_new (KlattGrid_FlutterTierEditor);
		my realTierArea = KlattGrid_FlutterTierArea_create (me.get(), klattgrid -> phonation -> flutter.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid flutter window not created.");
	}
}

/************************** KlattGrid_DoublePulsingTierEditor *********************************/

Thing_implement (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor, 0);

void structKlattGrid_DoublePulsingTierEditor :: v_dataChanged () {
	our realTierArea -> function = our klattgrid() -> phonation -> doublePulsing.get();
	KlattGrid_DoublePulsingTierEditor_Parent :: v_dataChanged ();
}

autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_DoublePulsingTierEditor me = Thing_new (KlattGrid_DoublePulsingTierEditor);
		my realTierArea = KlattGrid_DoublePulsingTierArea_create (me.get(), klattgrid -> phonation -> doublePulsing.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid double pulsing window not created.");
	}
}

/************************** KlattGrid_FormantGridEditor *********************************/

Thing_implement (KlattGrid_FormantGridEditor, FormantGridEditor, 0);

static bool FormantGrid_isEmpty (FormantGrid me) {
	return my formants.size == 0 || my bandwidths.size == 0;
}

void structKlattGrid_FormantGridEditor :: v_dataChanged () {
	//our realTierArea -> function = BUG: ... perhaps automatic?
	KlattGrid_FormantGridEditor_Parent :: v_dataChanged ();
}

void structKlattGrid_FormantGridEditor :: v_play (double startTime, double endTime) {
	KlattGrid_Editor_defaultPlay (klattgrid, startTime, endTime);
}

autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid data, kKlattGridFormantType formantType) {
	try {
		Melder_assert (data);
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (data, formantType);
		Melder_require (! FormantGrid_isEmpty (fg -> get()),
			U"Cannot edit an empty formant grid.");
		
		autoKlattGrid_FormantGridEditor me = Thing_new (KlattGrid_FormantGridEditor);
		my klattgrid = data;
		FormantGridEditor_init (me.get(), title, fg->get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid formant window not created.");
	}
}

/* End of file KlattGridEditors.cpp */
