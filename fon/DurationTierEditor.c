/* DurationTierEditor.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2008/03/21
 */

#include "DurationTierEditor.h"
#include "EditorM.h"

#define DurationTierEditor_members RealTierEditor_members
#define DurationTierEditor_methods RealTierEditor_methods
class_create_opaque (DurationTierEditor, RealTierEditor);

static int menu_cb_DurationTierHelp (EDITOR_ARGS) { EDITOR_IAM (DurationTierEditor); Melder_help (L"DurationTier"); return 1; }

static void createHelpMenuItems (I, EditorMenu menu) {
	iam (DurationTierEditor);
	inherited (DurationTierEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"DurationTier help", 0, menu_cb_DurationTierHelp);
}

static void play (I, double tmin, double tmax) {
	iam (DurationTierEditor);
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, NULL, NULL);
	} else {
		/*if (! DurationTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (DurationTierEditor, RealTierEditor) {
	class_method (createHelpMenuItems)
	class_method (play)
	us -> zeroIsMinimum = TRUE;
	us -> quantityText = L"Relative duration", us -> quantityKey = L"Relative duration";
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0.25, us -> defaultYmax = 3.0;
	us -> setRangeTitle = L"Set duration range...";
	us -> defaultYminText = L"0.25", us -> defaultYmaxText = L"3.0";
	us -> yminText = L"Minimum duration", us -> ymaxText = L"Maximum duration";
	us -> yminKey = L"Minimum duration", us -> ymaxKey = L"Maximum duration";
	class_methods_end
}

DurationTierEditor DurationTierEditor_create (Widget parent, const wchar_t *title, DurationTier duration, Sound sound, int ownSound) {
	DurationTierEditor me = new (DurationTierEditor);
	if (! me || ! RealTierEditor_init (me, parent, title, (RealTier) duration, sound, ownSound))
		{ forget (me); return NULL; }
	return me;
}

/* End of file DurationTierEditor.c */
