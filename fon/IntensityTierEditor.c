/* IntensityTierEditor.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 */

#include "IntensityTierEditor.h"
#include "EditorM.h"

#define IntensityTierEditor_members RealTierEditor_members
#define IntensityTierEditor_methods RealTierEditor_methods
class_create_opaque (IntensityTierEditor, RealTierEditor);

/*DIRECT (IntensityTierEditor, cb_IntensityTierEditorHelp) Melder_help (L"IntensityTierEditor"); END*/
DIRECT (IntensityTierEditor, cb_IntensityTierHelp) Melder_help (L"IntensityTier"); END

static void createMenus (I) {
	iam (IntensityTierEditor);
	inherited (IntensityTierEditor) createMenus (me);
	/*Editor_addCommand (me, L"Help", L"IntensityTierEditor help", 0, cb_IntensityTierEditorHelp);*/
	Editor_addCommand (me, L"Help", L"IntensityTier help", 0, cb_IntensityTierHelp);
}

static void play (I, double tmin, double tmax) {
	iam (IntensityTierEditor);
	if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	} else {
		/*if (! IntensityTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);*/
	}
}

class_methods (IntensityTierEditor, RealTierEditor)
	class_method (createMenus)
	class_method (play)
	us -> zeroIsMinimum = FALSE;
	us -> quantityText = L"Intensity (dB)", us -> quantityKey = L"Intensity";
	us -> rightTickUnits = L" dB";
	us -> defaultYmin = 50.0, us -> defaultYmax = 100.0;
	us -> setRangeTitle = L"Set intensity range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"100.0";
	us -> yminText = L"Minimum intensity (dB)", us -> ymaxText = L"Maximum intensity (dB)";
	us -> yminKey = L"Minimum intensity", us -> ymaxKey = L"Maximum intensity";
class_methods_end

IntensityTierEditor IntensityTierEditor_create (Widget parent, const wchar_t *title, IntensityTier intensity, Sound sound, int ownSound) {
	IntensityTierEditor me = new (IntensityTierEditor);
	if (! me || ! RealTierEditor_init (me, parent, title, (RealTier) intensity, sound, ownSound))
		{ forget (me); return NULL; }
	return me;
}

/* End of file IntensityTierEditor.c */
