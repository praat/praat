/* IntensityTierEditor.cpp
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
 * pb 2007/08/11 wchar_t
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2009/01/23 minimum and maximum legal values
 * pb 2011/07/02 C++
 */

#include "IntensityTierEditor.h"
#include "EditorM.h"

#undef our
#define our ((IntensityTierEditor_Table) my methods) ->

static int menu_cb_IntensityTierHelp (EDITOR_ARGS) { EDITOR_IAM (IntensityTierEditor); Melder_help (L"IntensityTier"); return 1; }

static void createHelpMenuItems (IntensityTierEditor me, EditorMenu menu) {
	inherited (IntensityTierEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

static void play (IntensityTierEditor me, double tmin, double tmax) {
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	} else {
		/*if (! IntensityTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (IntensityTierEditor, RealTierEditor) {
	class_method (createHelpMenuItems)
	class_method (play)
	us -> quantityText = L"Intensity (dB)", us -> quantityKey = L"Intensity";
	us -> rightTickUnits = L" dB";
	us -> defaultYmin = 50.0, us -> defaultYmax = 100.0;
	us -> setRangeTitle = L"Set intensity range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"100.0";
	us -> yminText = L"Minimum intensity (dB)", us -> ymaxText = L"Maximum intensity (dB)";
	us -> yminKey = L"Minimum intensity", us -> ymaxKey = L"Maximum intensity";
	class_methods_end
}

IntensityTierEditor IntensityTierEditor_create (GuiObject parent, const wchar *title, IntensityTier intensity, Sound sound, bool ownSound) {
	try {
		autoIntensityTierEditor me = Thing_new (IntensityTierEditor);
		RealTierEditor_init (me.peek(), parent, title, (RealTier) intensity, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("IntensityTier window not created.");
	}
}

/* End of file IntensityTierEditor.cpp */
