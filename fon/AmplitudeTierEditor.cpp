/* AmplitudeTierEditor.cpp
 *
 * Copyright (C) 2003-2011 Paul Boersma
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

#include "AmplitudeTierEditor.h"
#include "EditorM.h"

#undef our
#define our ((AmplitudeTierEditor_Table) my methods) ->

static int menu_cb_AmplitudeTierHelp (EDITOR_ARGS) { EDITOR_IAM (AmplitudeTierEditor); Melder_help (L"AmplitudeTier"); return 1; }

void structAmplitudeTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	AmplitudeTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"AmplitudeTier help", 0, menu_cb_AmplitudeTierHelp);
}

static void play (AmplitudeTierEditor me, double tmin, double tmax) {
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	} else {
		/*if (! AmplitudeTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (AmplitudeTierEditor, RealTierEditor) {
	class_method (play)
	us -> quantityText = L"Sound pressure (Pa)", us -> quantityKey = L"Sound pressure";
	us -> rightTickUnits = L" Pa";
	us -> defaultYmin = -1.0, us -> defaultYmax = +1.0;
	us -> setRangeTitle = L"Set amplitude range...";
	us -> defaultYminText = L"-1.0", us -> defaultYmaxText = L"+1.0";
	us -> yminText = L"Minimum amplitude (Pa)", us -> ymaxText = L"Maximum amplitude (Pa)";
	us -> yminKey = L"Minimum amplitude", us -> ymaxKey = L"Maximum amplitude";
	class_methods_end
}

AmplitudeTierEditor AmplitudeTierEditor_create (GuiObject parent, const wchar *title, AmplitudeTier amplitude, Sound sound, bool ownSound) {
	try {
		autoAmplitudeTierEditor me = Thing_new (AmplitudeTierEditor);
		RealTierEditor_init (me.peek(), parent, title, (RealTier) amplitude, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("AmplitudeTier window not created.");
	}
}

/* End of file AmplitudeTierEditor.cpp */
