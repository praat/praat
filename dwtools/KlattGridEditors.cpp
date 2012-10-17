/* KlattGridEditors.c
 *
 * Copyright (C) 2009-2011 david Weenink
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

/************************** KlattGrid_realTierEditor *********************************/

Thing_implement (KlattGrid_realTierEditor, RealTierEditor, 0);

static void menu_cb_KlattGridHelp (EDITOR_ARGS) {
	EDITOR_IAM (KlattGrid_realTierEditor);
	Melder_help (L"KlattGrid");
}

void structKlattGrid_realTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	KlattGrid_realTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"KlattGrid help", 0, menu_cb_KlattGridHelp);
}

void structKlattGrid_realTierEditor :: v_play (double ltmin, double ltmax) {
	KlattGrid_Editor_defaultPlay (klattgrid, ltmin, ltmax);
}

void KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, const wchar_t *title, KlattGrid klattgrid, RealTier data) {
	my klattgrid = klattgrid;
	RealTierEditor_init (me, title, data, 0, 0);
}

/************************** KlattGrid_pitchTierEditor *********************************/

Thing_implement (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor, 0);

static void menu_cb_KlattGrid_pitchTierEditorHelp (EDITOR_ARGS) {
	EDITOR_IAM (KlattGrid_pitchTierEditor); Melder_help (L"PitchTierEditor");
}

static void menu_cb_PitchTierHelp (EDITOR_ARGS) {
	EDITOR_IAM (KlattGrid_pitchTierEditor); Melder_help (L"PitchTier");
}

void structKlattGrid_pitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"PitchTierEditor help", 0, menu_cb_KlattGrid_pitchTierEditorHelp);
	EditorMenu_addCommand (menu, L"PitchTier help", 0, menu_cb_PitchTierHelp);
}

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_pitchTierEditor me = Thing_new (KlattGrid_pitchTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> pitch;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid pitch window not created.");
	}
}

/************************** KlattGrid_intensityTierEditor *********************************/

Thing_implement (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor, 0);

static void menu_cb_IntensityTierHelp (EDITOR_ARGS) {
	EDITOR_IAM (KlattGrid_intensityTierEditor);
	Melder_help (L"IntensityTier");
}

void structKlattGrid_intensityTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	KlattGrid_intensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

void KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, const wchar_t *title, KlattGrid klattgrid, RealTier tier) {
	KlattGrid_realTierEditor_init (me, title, klattgrid, tier);
}


/************************** KlattGrid_DecibelTierEditor *********************************/

Thing_implement (KlattGrid_decibelTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_decibelTierEditor KlattGrid_decibelTierEditor_create (const wchar_t *title, KlattGrid klattgrid, RealTier tier) {
	try {
		autoKlattGrid_decibelTierEditor me = Thing_new (KlattGrid_decibelTierEditor);
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid decibel window not created.");
	}
}

/************************** KlattGrid_voicingAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_voicingAmplitudeTierEditor me = Thing_new (KlattGrid_voicingAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> voicingAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid voicing amplitude window not created.");
	}
}

/************************** KlattGrid_aspirationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_aspirationAmplitudeTierEditor me = Thing_new (KlattGrid_aspirationAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> aspirationAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid aspiration amplitude window not created.");
	}
}

/************************** KlattGrid_breathinessAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_breathinessAmplitudeTierEditor me = Thing_new (KlattGrid_breathinessAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> breathinessAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid breathiness amplitude window not created.");
	}
}

/************************** KlattGrid_spectralTiltTierEditor *********************************/

Thing_implement (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_spectralTiltTierEditor me = Thing_new (KlattGrid_spectralTiltTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> spectralTilt;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid spectral tilt window not created.");
	}
}

/************************** KlattGrid_fricationBypassTierEditor *********************************/

Thing_implement (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_fricationBypassTierEditor me = Thing_new (KlattGrid_fricationBypassTierEditor);
		RealTier tier = (RealTier) klattgrid -> frication -> bypass;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid frication bypass window not created.");
	}
}

/************************** KlattGrid_fricationAmplitudeTierEditor *********************************/

Thing_implement (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor, 0);

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_fricationAmplitudeTierEditor me = Thing_new (KlattGrid_fricationAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> frication -> fricationAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid frication amplitude window not created.");
	}
}

/************************** KlattGrid_openPhaseTierEditor *********************************/

Thing_implement (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_openPhaseTierEditor me = Thing_new (KlattGrid_openPhaseTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> openPhase;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid_openPhaseTierEditor not created.");
	}
}

/************************** KlattGrid_collisionPhaseTierEditor *********************************/

Thing_implement (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_collisionPhaseTierEditor me = Thing_new (KlattGrid_collisionPhaseTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> collisionPhase;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid collision phase window not created.");
	}
}

/************************** KlattGrid_power1TierEditor *********************************/

Thing_implement (KlattGrid_power1TierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_power1TierEditor me = Thing_new (KlattGrid_power1TierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> power1;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid power1 window not created.");
	}
}

/************************** KlattGrid_power2TierEditor *********************************/

Thing_implement (KlattGrid_power2TierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_power2TierEditor me = Thing_new (KlattGrid_power2TierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> power2;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid power2 window not created.");
	}
}

/************************** KlattGrid_flutterTierEditor *********************************/

Thing_implement (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_flutterTierEditor me = Thing_new (KlattGrid_flutterTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> flutter;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid flutter window not created.");
	}
}

/************************** KlattGrid_doublePulsingTierEditor *********************************/

Thing_implement (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor, 0);

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_doublePulsingTierEditor me = Thing_new (KlattGrid_doublePulsingTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> doublePulsing;
		KlattGrid_realTierEditor_init (me.peek(), title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid double pulsing window not created.");
	}
}

/************************** KlattGrid_formantGridEditor *********************************/

Thing_implement (KlattGrid_formantGridEditor, FormantGridEditor, 0);

static bool FormantGrid_isEmpty (FormantGrid me) {
	return my formants -> size == 0 || my bandwidths -> size == 0;
}

void structKlattGrid_formantGridEditor :: v_play (double ltmin, double ltmax) {
	KlattGrid_Editor_defaultPlay (klattgrid, ltmin, ltmax);
}

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (const wchar_t *title, KlattGrid data, int formantType) {
	try {
		Melder_assert (data != NULL);
		FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (data, formantType);
		if (fg == NULL) {
			Melder_throw ("Formant type unknown.");
		}
		if (FormantGrid_isEmpty (*fg)) {
			Melder_throw ("Cannot edit an empty formant grid.");
		}
		autoKlattGrid_formantGridEditor me = Thing_new (KlattGrid_formantGridEditor);
		my klattgrid = data;
		FormantGridEditor_init (me.peek(), title, *fg);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid formant window not created.");
	}
}

/* End of file KlattGridEditors.cpp */