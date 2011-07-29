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
#include "EditorM.h"

#include "KlattGridEditors.h"

static void KlattGrid_Editor_defaultPlay (KlattGrid me, double tmin, double tmax)
{
	my options -> xmin = tmin; my options-> xmax = tmax;
	KlattGrid_playSpecial (me);
}

/************************** KlattGrid_realTierEditor *********************************/

static int menu_cb_KlattGridHelp (EDITOR_ARGS) { EDITOR_IAM (KlattGrid_realTierEditor); Melder_help (L"KlattGrid"); return 1; }

void structKlattGrid_realTierEditor :: v_createHelpMenuItems (EditorMenu menu)
{
	KlattGrid_realTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"KlattGrid help", 0, menu_cb_KlattGridHelp);
}

static void classKlattGrid_realTierEditor_play (KlattGrid_realTierEditor me, double tmin, double tmax)
{
	KlattGrid_Editor_defaultPlay (my klattgrid, tmin, tmax);
}

class_methods (KlattGrid_realTierEditor, RealTierEditor)
//	us -> play = KlattGrid_realTierEditor_play;
	class_method_local (KlattGrid_realTierEditor, play)
class_methods_end

void KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier data)
{
	my klattgrid = klattgrid;
	RealTierEditor_init (me, parent, title, data, 0, 0);
}

/************************** KlattGrid_pitchTierEditor *********************************/

static int menu_cb_KlattGrid_pitchTierEditorHelp (EDITOR_ARGS)
{
	EDITOR_IAM (KlattGrid_pitchTierEditor); Melder_help (L"PitchTierEditor"); return 1;
}

static int menu_cb_PitchTierHelp (EDITOR_ARGS)
{
	EDITOR_IAM (KlattGrid_pitchTierEditor); Melder_help (L"PitchTier"); return 1;
}

void structKlattGrid_pitchTierEditor :: v_createHelpMenuItems (EditorMenu menu)
{
	EditorMenu_addCommand (menu, L"PitchTierEditor help", 0, menu_cb_KlattGrid_pitchTierEditorHelp);
	EditorMenu_addCommand (menu, L"PitchTier help", 0, menu_cb_PitchTierHelp);
}

class_methods (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor)
{
	us -> quantityText = L"Frequency (Hz)", us -> quantityKey = L"Frequency";
	us -> rightTickUnits = L" Hz";
	us -> defaultYmin = 50.0, us -> defaultYmax = 600.0;
	us -> minimumLegalValue = 0.0;
	us -> setRangeTitle = L"Set frequency range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"600.0";
	us -> yminText = L"Minimum frequency (Hz)", us -> ymaxText = L"Maximum frequency (Hz)";
	us -> yminKey = L"Minimum frequency", us -> ymaxKey = L"Maximum frequency";
	class_methods_end
}

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid) {
	try {
		autoKlattGrid_pitchTierEditor me = Thing_new (KlattGrid_pitchTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> pitch;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid pitch window not created.");
	}
}

/************************** KlattGrid_intensityTierEditor *********************************/

static int menu_cb_IntensityTierHelp (EDITOR_ARGS) { EDITOR_IAM (KlattGrid_intensityTierEditor); Melder_help (L"IntensityTier"); return 1; }

void structKlattGrid_intensityTierEditor :: v_createHelpMenuItems (EditorMenu menu)
{
	KlattGrid_intensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

class_methods (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Intensity (dB)", us -> quantityKey = L"Intensity";
	us -> rightTickUnits = L" dB";
	us -> defaultYmin = 50.0, us -> defaultYmax = 100.0;
	us -> setRangeTitle = L"Set intensity range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"100.0";
	us -> yminText = L"Minimum intensity (dB)", us -> ymaxText = L"Maximum intensity (dB)";
	us -> yminKey = L"Minimum intensity", us -> ymaxKey = L"Maximum intensity";
class_methods_end

void KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier tier)
{
	KlattGrid_realTierEditor_init (me, parent, title, klattgrid, tier);
}


/************************** KlattGrid_DecibelTierEditor *********************************/

class_methods (KlattGrid_decibelTierEditor, KlattGrid_intensityTierEditor)
	us -> quantityText = L"Amplitude (dB)", us -> quantityKey = L"Amplitude";
	us -> rightTickUnits = L" dB";
	us -> defaultYmin = -30.0, us -> defaultYmax = 30.0;
	us -> setRangeTitle = L"Set amplitude range...";
	us -> defaultYminText = L"-30.0", us -> defaultYmaxText = L"30.0";
	us -> yminText = L"Minimum amplitude (dB)", us -> ymaxText = L"Maximum amplitude (dB)";
	us -> yminKey = L"Minimum amplitude", us -> ymaxKey = L"Maximum amplitude";
class_methods_end

KlattGrid_decibelTierEditor KlattGrid_decibelTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier tier)
{
	try {
		autoKlattGrid_decibelTierEditor me = Thing_new (KlattGrid_decibelTierEditor);
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid decibel window not created.");
	}
}

/************************** KlattGrid_voicingAmplitudeTierEditor *********************************/

class_methods (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_voicingAmplitudeTierEditor me = Thing_new (KlattGrid_voicingAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> voicingAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid voicing amplitude window not created.");
	}
}

/************************** KlattGrid_aspirationAmplitudeTierEditor *********************************/

class_methods (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_aspirationAmplitudeTierEditor me = Thing_new (KlattGrid_aspirationAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> aspirationAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid aspiration amplitude window not created.");
	}
}

/************************** KlattGrid_breathinessAmplitudeTierEditor *********************************/

class_methods (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_breathinessAmplitudeTierEditor me = Thing_new (KlattGrid_breathinessAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> breathinessAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid breathiness amplitude window not created.");
	}
}

/************************** KlattGrid_spectralTiltTierEditor *********************************/

class_methods (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor)
	us -> defaultYmin = -50.0, us -> defaultYmax = 10.0;
	us -> defaultYminText = L"-50.0", us -> defaultYmaxText = L"10.0";
class_methods_end

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_spectralTiltTierEditor me = Thing_new (KlattGrid_spectralTiltTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> spectralTilt;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid spectral tilt window not created.");
	}
}

/************************** KlattGrid_fricationBypassTierEditor *********************************/

class_methods (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor) {
	us -> defaultYmin = -50.0, us -> defaultYmax = 10.0;
	us -> defaultYminText = L"-50.0", us -> defaultYmaxText = L"10.0";
	class_methods_end
}

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_fricationBypassTierEditor me = Thing_new (KlattGrid_fricationBypassTierEditor);
		RealTier tier = (RealTier) klattgrid -> frication -> bypass;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid frication bypass window not created.");
	}
}

/************************** KlattGrid_fricationAmplitudeTierEditor *********************************/

class_methods (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
	class_methods_end
}

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_fricationAmplitudeTierEditor me = Thing_new (KlattGrid_fricationAmplitudeTierEditor);
		RealTier tier = (RealTier) klattgrid -> frication -> fricationAmplitude;
		KlattGrid_intensityTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid frication amplitude window not created.");
	}
}

/************************** KlattGrid_openPhaseTierEditor *********************************/

class_methods (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Open phase (0..1)", us -> quantityKey = L"Open phase";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set open phase range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_openPhaseTierEditor me = Thing_new (KlattGrid_openPhaseTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> openPhase;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid_openPhaseTierEditor not created.");
	}
}

/************************** KlattGrid_collisionPhaseTierEditor *********************************/

class_methods (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Collision phase (0..1)", us -> quantityKey = L"Collision phase";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 0.1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set collision phase range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"0.1";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_collisionPhaseTierEditor me = Thing_new (KlattGrid_collisionPhaseTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> collisionPhase;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid collision phase window not created.");
	}
}

/************************** KlattGrid_power1TierEditor *********************************/

class_methods (KlattGrid_power1TierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Power1", us -> quantityKey = L"Power1";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 4;
	us -> minimumLegalValue = 0;
	us -> setRangeTitle = L"Set power1 range...";
	us -> defaultYminText = L"0", us -> defaultYmaxText = L"4";
	us -> yminText = L"Minimum", us -> ymaxText = L"Maximum";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_power1TierEditor me = Thing_new (KlattGrid_power1TierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> power1;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid power1 window not created.");
	}
}

/************************** KlattGrid_power2TierEditor *********************************/

class_methods (KlattGrid_power2TierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Power2", us -> quantityKey = L"Power2";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 5;
	us -> minimumLegalValue = 0;
	us -> setRangeTitle = L"Set power2 range...";
	us -> defaultYminText = L"0", us -> defaultYmaxText = L"5";
	us -> yminText = L"Minimum", us -> ymaxText = L"Maximum";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_power2TierEditor me = Thing_new (KlattGrid_power2TierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> power2;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid power2 window not created.");
	}
}

/************************** KlattGrid_flutterTierEditor *********************************/

class_methods (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Flutter (0..1)", us -> quantityKey = L"Flutter";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set flutter range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_flutterTierEditor me = Thing_new (KlattGrid_flutterTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> flutter;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid flutter window not created.");
	}
}

/************************** KlattGrid_doublePulsingTierEditor *********************************/

class_methods (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor) {
	us -> quantityText = L"Double pulsing (0..1)", us -> quantityKey = L"Double pulsing";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set double pulsing range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
	class_methods_end
}

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid)
{
	try {
		autoKlattGrid_doublePulsingTierEditor me = Thing_new (KlattGrid_doublePulsingTierEditor);
		RealTier tier = (RealTier) klattgrid -> phonation -> doublePulsing;
		KlattGrid_realTierEditor_init (me.peek(), parent, title, klattgrid, tier);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid double pulsing window not created.");
	}
}

/************************** KlattGrid_formantGridEditor *********************************/

static int FormantGrid_isEmpty (FormantGrid me)
{
	return my formants -> size == 0 || my bandwidths -> size == 0;
}

static void classKlattGrid_formantGridEditor_play (KlattGrid_formantGridEditor me, double tmin, double tmax)
{
	KlattGrid_Editor_defaultPlay (my klattgrid, tmin, tmax);
}

class_methods (KlattGrid_formantGridEditor, FormantGridEditor) {
	us -> hasSourceMenu = false;
	class_method_local (KlattGrid_formantGridEditor, play)
	class_methods_end
}

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (GuiObject parent, const wchar_t *title, KlattGrid data, int formantType)
{
	try {
		Melder_assert (data != NULL);
		FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (data, formantType);
		if (fg == NULL) Melder_throw ("Formant type unknown.");
		if (FormantGrid_isEmpty (*fg)) Melder_throw ("Cannot edit an empty formant grid.");
		autoKlattGrid_formantGridEditor me = Thing_new (KlattGrid_formantGridEditor);
		my klattgrid = data;
		FormantGridEditor_init (me.peek(), parent, title, *fg);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid formant window not created.");
	}
}

/* End of file KlattGridEditors.cpp */
