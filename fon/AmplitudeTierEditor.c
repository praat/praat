/* AmplitudeTierEditor.c
 *
 * Copyright (C) 2003-2009 Paul Boersma
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
 * pb 2003/05/31 created
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2009/01/23 minimum and maximum legal values
 */

#include "AmplitudeTierEditor.h"
#include "EditorM.h"

static int menu_cb_AmplitudeTierHelp (EDITOR_ARGS) { EDITOR_IAM (AmplitudeTierEditor); Melder_help (L"AmplitudeTier"); return 1; }

static void createHelpMenuItems (AmplitudeTierEditor me, EditorMenu menu) {
	inherited (AmplitudeTierEditor) createHelpMenuItems (AmplitudeTierEditor_as_parent (me), menu);
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
	class_method (createHelpMenuItems)
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

AmplitudeTierEditor AmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, AmplitudeTier amplitude, Sound sound, int ownSound) {
	AmplitudeTierEditor me = new (AmplitudeTierEditor);
	if (! me || ! RealTierEditor_init (AmplitudeTierEditor_as_parent (me), parent, title, (RealTier) amplitude, sound, ownSound))
		{ forget (me); return NULL; }
	return me;
}

/* End of file AmplitudeTierEditor.c */
