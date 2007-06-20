/* PitchTierEditor.c
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
 */

#include "PitchTierEditor.h"
#include "PitchTier_to_Sound.h"
#include "EditorM.h"

#define PitchTierEditor_members RealTierEditor_members
#define PitchTierEditor_methods RealTierEditor_methods
class_create_opaque (PitchTierEditor, RealTierEditor);

DIRECT (PitchTierEditor, cb_PitchTierEditorHelp) Melder_help ("PitchTierEditor"); END
DIRECT (PitchTierEditor, cb_PitchTierHelp) Melder_help ("PitchTier"); END

static void createMenus (I) {
	iam (PitchTierEditor);
	inherited (PitchTierEditor) createMenus (me);
	Editor_addCommand (me, L"Help", L"PitchTierEditor help", 0, cb_PitchTierEditorHelp);
	Editor_addCommand (me, L"Help", L"PitchTier help", 0, cb_PitchTierHelp);
}

static void play (I, double tmin, double tmax) {
	iam (PitchTierEditor);
	if (my sound.data) Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	else if (! PitchTier_playPart (my data, tmin, tmax, FALSE)) Melder_flushError (NULL);
}

class_methods (PitchTierEditor, RealTierEditor)
	class_method (createMenus)
	class_method (play)
	us -> zeroIsMinimum = TRUE;
	us -> quantityText = L"Frequency (Hz)", us -> quantityKey = L"Frequency";
	us -> leftTickFormat = L"%5g", us -> rightTickFormat = L"%5g Hz";
	us -> defaultYmin = 50.0, us -> defaultYmax = 600.0;
	us -> setRangeTitle = L"Set frequency range...";
	us -> defaultYminText = L"50.0", us -> defaultYmaxText = L"600.0";
	us -> yminText = L"Minimum frequency (Hz)", us -> ymaxText = L"Maximum frequency (Hz)";
	us -> yminKey = L"Minimum frequency", us -> ymaxKey = L"Maximum frequency";
class_methods_end

PitchTierEditor PitchTierEditor_create (Widget parent, const wchar_t *title, PitchTier pitch, Sound sound, int ownSound) {
	PitchTierEditor me = new (PitchTierEditor);
	if (! me || ! RealTierEditor_init (me, parent, title, (RealTier) pitch, sound, ownSound))
		{ forget (me); return NULL; }
	return me;
}

/* End of file PitchTierEditor.c */
