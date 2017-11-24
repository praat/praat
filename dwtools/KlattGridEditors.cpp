/* KlattGridEditors.c
 *
 * Copyright (C) 2009-2017 david Weenink
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

#include "Preferences.h"
#include "KlattGridEditors.h"
#include "EditorM.h"

static void KlattGrid_Editor_defaultPlay (KlattGrid me, double tmin, double tmax) {
	my options -> xmin = tmin; my options-> xmax = tmax;
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

void structKlattGrid_RealTierEditor :: v_play (double ltmin, double ltmax) {
	KlattGrid_Editor_defaultPlay (klattgrid, ltmin, ltmax);
}

void KlattGrid_RealTierEditor_init (KlattGrid_RealTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier data) {
	my klattgrid = klattgrid;
	RealTierEditor_init (me, title, data, nullptr, false);
}

/************************** KlattGrid_PitchTierEditor *********************************/

Thing_implement (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor, 0);

static void menu_cb_KlattGrid_PitchTierEditorHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"PitchTierEditor");
}

static void menu_cb_PitchTierHelp (KlattGrid_PitchTierEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"PitchTier");
}

void structKlattGrid_PitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_KlattGrid_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_PitchTierEditor me = Thing_new (KlattGrid_PitchTierEditor);
		RealTier tier = klattgrid -> phonation -> pitch.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
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

void KlattGrid_IntensityTierEditor_init (KlattGrid_IntensityTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier tier) {
	KlattGrid_RealTierEditor_init (me, title, klattgrid, tier);
}


/************************** KlattGrid_DecibelTierEditor *********************************/

Thing_implement (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (const char32 *title, KlattGrid klattgrid, RealTier tier) {
	try {
		autoKlattGrid_DecibelTierEditor me = Thing_new (KlattGrid_DecibelTierEditor);
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid decibel window not created.");
	}
}

/************************** KlattGrid_VoicingAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_VoicingAmplitudeTierEditor me = Thing_new (KlattGrid_VoicingAmplitudeTierEditor);
		RealTier tier = klattgrid -> phonation -> voicingAmplitude.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid voicing amplitude window not created.");
	}
}

/************************** KlattGrid_AspirationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_AspirationAmplitudeTierEditor me = Thing_new (KlattGrid_AspirationAmplitudeTierEditor);
		RealTier tier = klattgrid -> phonation -> aspirationAmplitude.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid aspiration amplitude window not created.");
	}
}

/************************** KlattGrid_BreathinessAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_BreathinessAmplitudeTierEditor me = Thing_new (KlattGrid_BreathinessAmplitudeTierEditor);
		RealTier tier = klattgrid -> phonation -> breathinessAmplitude.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid breathiness amplitude window not created.");
	}
}

/************************** KlattGrid_SpectralTiltTierEditor *********************************/

Thing_implement (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_SpectralTiltTierEditor me = Thing_new (KlattGrid_SpectralTiltTierEditor);
		RealTier tier = klattgrid -> phonation -> spectralTilt.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid spectral tilt window not created.");
	}
}

/************************** KlattGrid_FricationBypassTierEditor *********************************/

Thing_implement (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationBypassTierEditor me = Thing_new (KlattGrid_FricationBypassTierEditor);
		RealTier tier = klattgrid -> frication -> bypass.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication bypass window not created.");
	}
}

/************************** KlattGrid_FricationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor, 0);

autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FricationAmplitudeTierEditor me = Thing_new (KlattGrid_FricationAmplitudeTierEditor);
		RealTier tier = klattgrid -> frication -> fricationAmplitude.get();
		KlattGrid_IntensityTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid frication amplitude window not created.");
	}
}

/************************** KlattGrid_OpenPhaseTierEditor *********************************/

Thing_implement (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_OpenPhaseTierEditor me = Thing_new (KlattGrid_OpenPhaseTierEditor);
		RealTier tier = klattgrid -> phonation -> openPhase.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid open phase window not created.");
	}
}

/************************** KlattGrid_CollisionPhaseTierEditor *********************************/

Thing_implement (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_CollisionPhaseTierEditor me = Thing_new (KlattGrid_CollisionPhaseTierEditor);
		RealTier tier = klattgrid -> phonation -> collisionPhase.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid collision phase window not created.");
	}
}

/************************** KlattGrid_Power1TierEditor *********************************/

Thing_implement (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power1TierEditor me = Thing_new (KlattGrid_Power1TierEditor);
		RealTier tier = klattgrid -> phonation -> power1.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power1 window not created.");
	}
}

/************************** KlattGrid_Power2TierEditor *********************************/

Thing_implement (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_Power2TierEditor me = Thing_new (KlattGrid_Power2TierEditor);
		RealTier tier = klattgrid -> phonation -> power2.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid power2 window not created.");
	}
}

/************************** KlattGrid_FlutterTierEditor *********************************/

Thing_implement (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_FlutterTierEditor me = Thing_new (KlattGrid_FlutterTierEditor);
		RealTier tier = klattgrid -> phonation -> flutter.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid flutter window not created.");
	}
}

/************************** KlattGrid_DoublePulsingTierEditor *********************************/

Thing_implement (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor, 0);

autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (const char32 *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_DoublePulsingTierEditor me = Thing_new (KlattGrid_DoublePulsingTierEditor);
		RealTier tier = klattgrid -> phonation -> doublePulsing.get();
		KlattGrid_RealTierEditor_init (me.get(), title, klattgrid, tier);
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

void structKlattGrid_FormantGridEditor :: v_play (double ltmin, double ltmax) {
	KlattGrid_Editor_defaultPlay (klattgrid, ltmin, ltmax);
}

autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (const char32 *title, KlattGrid data, int formantType) {
	try {
		Melder_assert (data);
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (data, formantType);
		Melder_require (fg, U"Formant type unknown.");
		Melder_require (! FormantGrid_isEmpty (fg -> get()), U"Cannot edit an empty formant grid.");
		
		autoKlattGrid_FormantGridEditor me = Thing_new (KlattGrid_FormantGridEditor);
		my klattgrid = data;
		FormantGridEditor_init (me.get(), title, fg->get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid formant window not created.");
	}
}

/* End of file KlattGridEditors.cpp */
