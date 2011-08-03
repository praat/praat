/* DurationTierEditor.cpp
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

#include "DurationTierEditor.h"
#include "EditorM.h"

static int menu_cb_DurationTierHelp (EDITOR_ARGS) { EDITOR_IAM (DurationTierEditor); Melder_help (L"DurationTier"); return 1; }

void structDurationTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	DurationTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"DurationTier help", 0, menu_cb_DurationTierHelp);
}

static void play (DurationTierEditor me, double tmin, double tmax) {
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, NULL, NULL);
	} else {
		/*if (! DurationTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (DurationTierEditor, RealTierEditor) {
	class_method (play)
	us -> minimumLegalValue = 0.0;
	us -> quantityText = L"Relative duration", us -> quantityKey = L"Relative duration";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0.25, us -> defaultYmax = 3.0;
	us -> setRangeTitle = L"Set duration range...";
	us -> defaultYminText = L"0.25", us -> defaultYmaxText = L"3.0";
	us -> yminText = L"Minimum duration", us -> ymaxText = L"Maximum duration";
	us -> yminKey = L"Minimum duration", us -> ymaxKey = L"Maximum duration";
	class_methods_end
}

DurationTierEditor DurationTierEditor_create (GuiObject parent, const wchar *title, DurationTier duration, Sound sound, bool ownSound) {
	try {
		autoDurationTierEditor me = Thing_new (DurationTierEditor);
		RealTierEditor_init (me.peek(), parent, title, (RealTier) duration, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("DurationTier window not created.");
	}
}

/* End of file DurationTierEditor.cpp */
