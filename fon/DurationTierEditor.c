/* DurationTierEditor.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 2006/12/20 new Sound_play API
 */

#include "DurationTierEditor.h"
#include "EditorM.h"

#define DurationTierEditor_members RealTierEditor_members
#define DurationTierEditor_methods RealTierEditor_methods
class_create_opaque (DurationTierEditor, RealTierEditor)

/*DIRECT (DurationTierEditor, cb_DurationTierEditorHelp) Melder_help ("DurationTierEditor"); END*/
DIRECT (DurationTierEditor, cb_DurationTierHelp) Melder_help ("DurationTier"); END

static void createMenus (I) {
	iam (DurationTierEditor);
	inherited (DurationTierEditor) createMenus (me);
	/*Editor_addCommand (me, "Help", "DurationTierEditor help", 0, cb_DurationTierEditorHelp);*/
	Editor_addCommand (me, "Help", "DurationTier help", 0, cb_DurationTierHelp);
}

static void play (I, double tmin, double tmax) {
	iam (DurationTierEditor);
	if (my sound.data) {
		Sound_playPart (my sound.data, NULL, tmin, tmax, NULL, NULL);
	} else {
		/*if (! DurationTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (DurationTierEditor, RealTierEditor)
	class_method (createMenus)
	class_method (play)
	us -> zeroIsMinimum = TRUE;
	us -> quantityText = "Relative duration", us -> quantityKey = "Relative duration";
	us -> leftTickFormat = "%5g", us -> rightTickFormat = "%5g";
	us -> defaultYmin = 0.25, us -> defaultYmax = 3.0;
	us -> setRangeTitle = "Set duration range...";
	us -> defaultYminText = "0.25", us -> defaultYmaxText = "3.0";
	us -> yminText = "Minimum duration", us -> ymaxText = "Maximum duration";
	us -> yminKey = "Minimum duration", us -> ymaxKey = "Maximum duration";
class_methods_end

DurationTierEditor DurationTierEditor_create (Widget parent, const char *title, DurationTier duration, Sound sound, int ownSound) {
	DurationTierEditor me = new (DurationTierEditor);
	if (! me || ! RealTierEditor_init (me, parent, title, (RealTier) duration, sound, ownSound))
		{ forget (me); return NULL; }
	return me;
}

/* End of file DurationTierEditor.c */
