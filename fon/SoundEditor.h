#ifndef _SoundEditor_h_
#define _SoundEditor_h_
/* SoundEditor.h
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

#include "TimeSoundAnalysisEditor.h"

Thing_declare1cpp (SoundEditor);
struct structSoundEditor : public structTimeSoundAnalysisEditor {
	// new data
		GuiObject cutButton, copyButton, pasteButton, zeroButton, reverseButton;
		double maxBuffer;
	// overridden methods:
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
		void v_dataChanged ();
};
#define SoundEditor__methods(Klas) TimeSoundAnalysisEditor__methods(Klas)
Thing_declare2cpp (SoundEditor, TimeSoundAnalysisEditor);

SoundEditor SoundEditor_create (
	GuiObject parent,
	const wchar *title,
	Function data   // either a Sound or a LongSound
);

/* End of file SoundEditor.h */
#endif
