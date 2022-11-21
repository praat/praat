/* KlattGridEditors.cpp
 *
 * Copyright (C) 2009-2019 David Weenink, 2021,2022 Paul Boersma
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

static void KlattGrid_Editor_defaultPlay (KlattGrid me, FunctionEditor editor, double tmin, double tmax) {
	my options -> xmin = tmin;
	my options -> xmax = tmax;
	KlattGrid_playSpecial (me, theFunctionEditor_playCallback, editor);
}


#pragma mark - KlattGrid_RealTierEditor

Thing_implement (KlattGrid_RealTierEditor, RealTierEditor, 0);

static void menu_cb_KlattGridHelp (KlattGrid_RealTierEditor, EDITOR_ARGS) {
	Melder_help (U"KlattGrid");
}

void structKlattGrid_RealTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	KlattGrid_RealTierEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"KlattGrid help", 0, menu_cb_KlattGridHelp);
}

void structKlattGrid_RealTierEditor :: v_play (double startTime, double endTime) {
	KlattGrid_Editor_defaultPlay (our klattgrid(), this, startTime, endTime);
}


#pragma mark - KlattGrid_IntensityTierEditor

Thing_implement (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor, 0);

static void menu_cb_IntensityTierHelp (KlattGrid_IntensityTierEditor, EDITOR_ARGS) {
	Melder_help (U"IntensityTier");
}

void structKlattGrid_IntensityTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	KlattGrid_IntensityTierEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}


#pragma mark - KlattGrid_PitchTierEditor

Thing_implement (KlattGrid_PitchTierArea, PitchTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_PitchTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_PitchTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_PitchTierArea_prefs.h"

Thing_implement (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor, 0);

static void menu_cb_KlattGrid_PitchTierEditorHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS) {
	Melder_help (U"PitchTierEditor");
}

static void menu_cb_PitchTierHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS) {
	Melder_help (U"PitchTier");
}

void structKlattGrid_PitchTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_KlattGrid_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_PitchTierEditor me = Thing_new (KlattGrid_PitchTierEditor);
		my realTierArea() = KlattGrid_PitchTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid pitch window not created.");
	}
}


#pragma mark - KlattGrid_VoicingAmplitudeTierEditor

Thing_implement (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_VoicingAmplitudeTierEditor me = Thing_new (KlattGrid_VoicingAmplitudeTierEditor);
		my realTierArea() = KlattGrid_VoicingAmplitudeTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid voicing amplitude window not created.");
	}
}


#pragma mark - KlattGrid_FlutterTierEditor

Thing_implement (KlattGrid_FlutterTierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_FlutterTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_FlutterTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_FlutterTierArea_prefs.h"

Thing_implement (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FlutterTierEditor me = Thing_new (KlattGrid_FlutterTierEditor);
		my realTierArea() = KlattGrid_FlutterTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid flutter window not created.");
	}
}


#pragma mark - KlattGrid_Power1TierEditor

Thing_implement (KlattGrid_Power1TierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_Power1TierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_Power1TierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_Power1TierArea_prefs.h"

Thing_implement (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power1TierEditor me = Thing_new (KlattGrid_Power1TierEditor);
		my realTierArea() = KlattGrid_Power1TierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power1 window not created.");
	}
}


#pragma mark - KlattGrid_Power2TierEditor

Thing_implement (KlattGrid_Power2TierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_Power2TierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_Power2TierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_Power2TierArea_prefs.h"

Thing_implement (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power2TierEditor me = Thing_new (KlattGrid_Power2TierEditor);
		my realTierArea() = KlattGrid_Power2TierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power2 window not created.");
	}
}


#pragma mark - KlattGrid_OpenPhaseTierEditor

Thing_implement (KlattGrid_OpenPhaseTierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"

Thing_implement (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_OpenPhaseTierEditor me = Thing_new (KlattGrid_OpenPhaseTierEditor);
		my realTierArea() = KlattGrid_OpenPhaseTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid open phase window not created.");
	}
}


#pragma mark - KlattGrid_CollisionPhaseTierEditor

Thing_implement (KlattGrid_CollisionPhaseTierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"

Thing_implement (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_CollisionPhaseTierEditor me = Thing_new (KlattGrid_CollisionPhaseTierEditor);
		my realTierArea() = KlattGrid_CollisionPhaseTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid collision phase window not created.");
	}
}


#pragma mark - KlattGrid_DoublePulsingTierEditor

Thing_implement (KlattGrid_DoublePulsingTierArea, RealTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"

Thing_implement (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_DoublePulsingTierEditor me = Thing_new (KlattGrid_DoublePulsingTierEditor);
		my realTierArea() = KlattGrid_DoublePulsingTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid double pulsing window not created.");
	}
}


#pragma mark - KlattGrid_SpectralTiltTierEditor

Thing_implement (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"

Thing_implement (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_SpectralTiltTierEditor me = Thing_new (KlattGrid_SpectralTiltTierEditor);
		my realTierArea() = KlattGrid_SpectralTiltTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid spectral tilt window not created.");
	}
}

#pragma mark - KlattGrid_AspirationAmplitudeTierEditor

Thing_implement (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_AspirationAmplitudeTierEditor me = Thing_new (KlattGrid_AspirationAmplitudeTierEditor);
		my realTierArea() = KlattGrid_AspirationAmplitudeTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid aspiration amplitude window not created.");
	}
}


#pragma mark - KlattGrid_BreathinessAmplitudeTierEditor

Thing_implement (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_BreathinessAmplitudeTierEditor me = Thing_new (KlattGrid_BreathinessAmplitudeTierEditor);
		my realTierArea() = KlattGrid_BreathinessAmplitudeTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid breathiness amplitude window not created.");
	}
}


#pragma mark - KlattGrid_DecibelTierEditor

Thing_implement (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_DecibelTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_DecibelTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_DecibelTierArea_prefs.h"

Thing_implement (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid,
	kKlattGridFormantType formantType, integer iformant)
{
	try {
		autoKlattGrid_DecibelTierEditor me = Thing_new (KlattGrid_DecibelTierEditor);
		my formantType = formantType;   // before FunctionArea_init() BUG: move
		my iformant = iformant;
		my realTierArea() = KlattGrid_DecibelTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid decibel window not created.");
	}
}


#pragma mark - KlattGrid_FricationBypassTierEditor

Thing_implement (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea, 0);
#include "Prefs_define.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"
#include "Prefs_install.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"

Thing_implement (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationBypassTierEditor me = Thing_new (KlattGrid_FricationBypassTierEditor);
		my realTierArea() = KlattGrid_FricationBypassTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication bypass window not created.");
	}
}


#pragma mark - KlattGrid_FricationAmplitudeTierEditor

Thing_implement (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationAmplitudeTierEditor me = Thing_new (KlattGrid_FricationAmplitudeTierEditor);
		my realTierArea() = KlattGrid_FricationAmplitudeTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication amplitude window not created.");
	}
}


#pragma mark - KlattGrid_FormantGridEditor

Thing_implement (KlattGrid_FormantGridEditor, FormantGridEditor, 0);
Thing_implement (KlattGrid_FormantGridArea, FormantGridArea, 0);

static bool FormantGrid_isEmpty (FormantGrid me) {
	return my formants.size == 0 || my bandwidths.size == 0;
}

void structKlattGrid_FormantGridEditor :: v_play (double startTime, double endTime) {
	KlattGrid_Editor_defaultPlay (static_cast <KlattGrid> (our data()), this, startTime, endTime);
}

autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid klattgrid, kKlattGridFormantType formantType) {
	try {
		Melder_assert (klattgrid);
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (klattgrid, formantType);
		Melder_require (! FormantGrid_isEmpty (fg -> get()),
			U"Cannot edit an empty formant grid.");
		
		autoKlattGrid_FormantGridEditor me = Thing_new (KlattGrid_FormantGridEditor);
		my formantType = formantType;
		my formantGridArea() = KlattGrid_FormantGridArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, klattgrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid formant window not created.");
	}
}

/* End of file KlattGridEditors.cpp */
