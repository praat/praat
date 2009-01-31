/* KlattGridEditors.c
 *
 * Copyright (C) 2009 david Weenink
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
 */
 
#include "Preferences.h"
#include "EditorM.h"

#include "KlattGridEditors.h"

static int KlattGrid_Editor_defaultPlay (KlattGrid me, double tmin, double tmax)
{
	my options -> xmin = tmin; my options-> xmax = tmax;
	return KlattGrid_playSpecial (me);
}

/************************** KlattGrid_realTierEditor *********************************/

static int menu_cb_KlattGridHelp (EDITOR_ARGS) { EDITOR_IAM (KlattGrid_realTierEditor); Melder_help (L"KlattGrid"); return 1; }

static void classKlattGrid_realTierEditor_createHelpMenuItems (KlattGrid_realTierEditor me, EditorMenu menu)
{
	inherited (KlattGrid_realTierEditor) createHelpMenuItems (KlattGrid_realTierEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"KlattGrid help", 0, menu_cb_KlattGridHelp);
}

static void classKlattGrid_realTierEditor_play (KlattGrid_realTierEditor me, double tmin, double tmax)
{
	KlattGrid_Editor_defaultPlay (my klattgrid, tmin, tmax);
}

class_methods (KlattGrid_realTierEditor, RealTierEditor)
//	us -> createHelpMenuItems = KlattGrid_realTierEditor_createHelpMenuItems;
//	us -> play = KlattGrid_realTierEditor_play;
	class_method_local (KlattGrid_realTierEditor, createHelpMenuItems)
	class_method_local (KlattGrid_realTierEditor, play)
class_methods_end

int KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, Widget parent, const wchar_t *title, KlattGrid klattgrid, RealTier data)
{
	my klattgrid = klattgrid;
	return RealTierEditor_init (KlattGrid_realTierEditor_as_RealTierEditor (me), parent, title, data, NULL, 0);
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

static void classKlattGrid_pitchTierEditor_createHelpMenuItems (KlattGrid_pitchTierEditor me, EditorMenu menu)
{
	inherited (KlattGrid_pitchTierEditor) createHelpMenuItems (KlattGrid_pitchTierEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"PitchTierEditor help", 0, menu_cb_KlattGrid_pitchTierEditorHelp);
	EditorMenu_addCommand (menu, L"PitchTier help", 0, menu_cb_PitchTierHelp);
}

class_methods (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor)
{
	class_method_local (KlattGrid_pitchTierEditor, createHelpMenuItems)
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

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid) {
	KlattGrid_pitchTierEditor me = new (KlattGrid_pitchTierEditor);
	if (me == NULL) return NULL;
	RealTier tier = (RealTier) klattgrid -> phonation -> pitch;
	KlattGrid_realTierEditor_init (KlattGrid_pitchTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier);
	if (Melder_hasError()) forget (me);
	return me;
}

/************************** KlattGrid_intensityTierEditor *********************************/

static int menu_cb_IntensityTierHelp (EDITOR_ARGS) { EDITOR_IAM (KlattGrid_intensityTierEditor); Melder_help (L"IntensityTier"); return 1; }

static void classKlattGrid_intensityTierEditor_createHelpMenuItems (KlattGrid_intensityTierEditor me, EditorMenu menu)
{
	inherited (KlattGrid_intensityTierEditor) createHelpMenuItems (KlattGrid_intensityTierEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

class_methods (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor)
	class_method_local (KlattGrid_intensityTierEditor, createHelpMenuItems)
	us -> quantityText = L"Intensity (dB)", us -> quantityKey = L"Intensity";
	us -> rightTickUnits = L" dB";
	us -> defaultYmin = 50.0, us -> defaultYmax = 100.0;
	us -> setRangeTitle = L"Set intensity range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"100.0";
	us -> yminText = L"Minimum intensity (dB)", us -> ymaxText = L"Maximum intensity (dB)";
	us -> yminKey = L"Minimum intensity", us -> ymaxKey = L"Maximum intensity";
class_methods_end

int KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, Widget parent, const wchar_t *title, KlattGrid klattgrid, RealTier tier)
{
	return  KlattGrid_realTierEditor_init (KlattGrid_intensityTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier);
}


/************************** KlattGrid_voicingAmplitudeTierEditor *********************************/

class_methods (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_voicingAmplitudeTierEditor me = new (KlattGrid_voicingAmplitudeTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> voicingAmplitude;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init
		(KlattGrid_voicingAmplitudeTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_aspirationAmplitudeTierEditor *********************************/

class_methods (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_aspirationAmplitudeTierEditor me = new (KlattGrid_aspirationAmplitudeTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> aspirationAmplitude;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init (KlattGrid_aspirationAmplitudeTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_breathinessAmplitudeTierEditor *********************************/

class_methods (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_breathinessAmplitudeTierEditor me = new (KlattGrid_breathinessAmplitudeTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> breathinessAmplitude;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init (KlattGrid_breathinessAmplitudeTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_spectralTiltTierEditor *********************************/

class_methods (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_spectralTiltTierEditor me = new (KlattGrid_spectralTiltTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> spectralTilt;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init (KlattGrid_spectralTiltTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_fricationBypassTierEditor *********************************/

class_methods (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_fricationBypassTierEditor me = new (KlattGrid_fricationBypassTierEditor);
	RealTier tier = (RealTier) klattgrid -> frication -> bypass;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init (KlattGrid_fricationBypassTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_fricationAmplitudeTierEditor *********************************/

class_methods (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor)
class_methods_end

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_fricationAmplitudeTierEditor me = new (KlattGrid_fricationAmplitudeTierEditor);
	RealTier tier = (RealTier) klattgrid -> frication -> fricationAmplitude;
	if (me == NULL || ! KlattGrid_intensityTierEditor_init (KlattGrid_fricationAmplitudeTierEditor_as_KlattGrid_intensityTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}


/************************** KlattGrid_openPhaseTierEditor *********************************/

class_methods (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Open phase (0..1)", us -> quantityKey = L"Open phase";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set open phase range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_openPhaseTierEditor me = new (KlattGrid_openPhaseTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> openPhase;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_openPhaseTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_collisionPhaseTierEditor *********************************/

class_methods (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Collision phase (0..1)", us -> quantityKey = L"Collision phase";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 0.1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set collision phase range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"0.1";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_collisionPhaseTierEditor me = new (KlattGrid_collisionPhaseTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> collisionPhase;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_collisionPhaseTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_power1TierEditor *********************************/

class_methods (KlattGrid_power1TierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Power1", us -> quantityKey = L"Power1";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 4;
	us -> minimumLegalValue = 0;
	us -> setRangeTitle = L"Set power1 range...";
	us -> defaultYminText = L"0", us -> defaultYmaxText = L"4";
	us -> yminText = L"Minimum", us -> ymaxText = L"Maximum";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_power1TierEditor me = new (KlattGrid_power1TierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> power1;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_power1TierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_power2TierEditor *********************************/

class_methods (KlattGrid_power2TierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Power2", us -> quantityKey = L"Power2";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 5;
	us -> minimumLegalValue = 0;
	us -> setRangeTitle = L"Set power2 range...";
	us -> defaultYminText = L"0", us -> defaultYmaxText = L"5";
	us -> yminText = L"Minimum", us -> ymaxText = L"Maximum";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_power2TierEditor me = new (KlattGrid_power2TierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> power2;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_power2TierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_flutterTierEditor *********************************/

class_methods (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Flutter (0..1)", us -> quantityKey = L"Flutter";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set flutter range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_flutterTierEditor me = new (KlattGrid_flutterTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> flutter;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_flutterTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_doublePulsingTierEditor *********************************/

class_methods (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor)
	us -> quantityText = L"Double pulsing (0..1)", us -> quantityKey = L"Double pulsing";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0, us -> defaultYmax = 1;
	us -> minimumLegalValue = 0; us -> maximumLegalValue = 1;
	us -> setRangeTitle = L"Set double pulsing range...";
	us -> defaultYminText = L"0.0", us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum (0..1)", us -> ymaxText = L"Maximum (0..1)";
	us -> yminKey = L"Minimum", us -> ymaxKey = L"Maximum";
class_methods_end

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid)
{
	KlattGrid_doublePulsingTierEditor me = new (KlattGrid_doublePulsingTierEditor);
	RealTier tier = (RealTier) klattgrid -> phonation -> doublePulsing;
	if (me == NULL || ! KlattGrid_realTierEditor_init (KlattGrid_doublePulsingTierEditor_as_KlattGrid_realTierEditor (me), parent, title, klattgrid, tier)) forget (me);
	return me;
}

/************************** KlattGrid_formantGridEditor *********************************/

static void classKlattGrid_formantGridEditor_play (KlattGrid_formantGridEditor me, double tmin, double tmax)
{
	KlattGrid_Editor_defaultPlay (my klattgrid, tmin, tmax);
}

class_methods (KlattGrid_formantGridEditor, FormantGridEditor)
	us -> hasSourceMenu = false;
	class_method_local (KlattGrid_formantGridEditor, play)
class_methods_end

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (Widget parent, const wchar_t *title, KlattGrid data, int formantType)
{
	Melder_assert (data != NULL);
	KlattGrid_formantGridEditor me = new (KlattGrid_formantGridEditor);
	if (me == NULL) return NULL;
	my klattgrid = data;
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (my klattgrid, formantType);
	if (fg == NULL) goto end;
end:
	if (! FormantGridEditor_init (KlattGrid_formantGridEditor_as_FormantGridEditor (me), parent, title, *fg)) forget (me);
	return me;
}

/* End of file KlattGridEditors.c */
