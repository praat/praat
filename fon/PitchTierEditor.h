#ifndef _PitchTierEditor_h_
#define _PitchTierEditor_h_
/* PitchTierEditor.h
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

#include "RealTierEditor.h"
#include "PitchTier.h"
#include "Sound.h"

Thing_declare1cpp (PitchTierEditor);
struct structPitchTierEditor : public structRealTierEditor {
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
};
#define PitchTierEditor__methods(Klas) RealTierEditor__methods(Klas)
Thing_declare2cpp (PitchTierEditor, RealTierEditor);

PitchTierEditor PitchTierEditor_create (GuiObject parent, const wchar *title,
	PitchTier pitch,
	Sound sound,   // may be NULL
	bool ownSound);

/* End of file PitchTierEditor.h */
#endif
