/* PitchTierEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2009/01/23 minimum and maximum legal values
 * pb 2011/03/22 C++
 */

#include "PitchTierEditor.h"
#include "PitchTier_to_Sound.h"
#include "EditorM.h"

static int menu_cb_PitchTierEditorHelp (EDITOR_ARGS) { EDITOR_IAM (PitchTierEditor); Melder_help (L"PitchTierEditor"); return 1; }
static int menu_cb_PitchTierHelp (EDITOR_ARGS) { EDITOR_IAM (PitchTierEditor); Melder_help (L"PitchTier"); return 1; }

static void createHelpMenuItems (PitchTierEditor me, EditorMenu menu) {
	inherited (PitchTierEditor) createHelpMenuItems (PitchTierEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"PitchTierEditor help", 0, menu_cb_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, L"PitchTier help", 0, menu_cb_PitchTierHelp);
}

static void play (PitchTierEditor me, double tmin, double tmax) {
	if (my sound.data) Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	else if (! PitchTier_playPart ((PitchTier) my data, tmin, tmax, FALSE)) Melder_flushError (NULL);
}

class_methods (PitchTierEditor, RealTierEditor) {
	class_method (createHelpMenuItems)
	class_method (play)
	us -> minimumLegalValue = 0.0;
	us -> quantityText = L"Frequency (Hz)", us -> quantityKey = L"Frequency";
	us -> rightTickUnits = L" Hz";
	us -> defaultYmin = 50.0, us -> defaultYmax = 600.0;
	us -> setRangeTitle = L"Set frequency range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"600.0";
	us -> yminText = L"Minimum frequency (Hz)", us -> ymaxText = L"Maximum frequency (Hz)";
	us -> yminKey = L"Minimum frequency", us -> ymaxKey = L"Maximum frequency";
	class_methods_end
}

PitchTierEditor PitchTierEditor_create (GuiObject parent, const wchar_t *title, PitchTier pitch, Sound sound, int ownSound) {
	PitchTierEditor me = Thing_new (PitchTierEditor); cherror
	RealTierEditor_init (PitchTierEditor_as_parent (me), parent, title, (RealTier) pitch, sound, ownSound); cherror
end:
	iferror forget (me);
	return me;
}

/* End of file PitchTierEditor.cpp */
