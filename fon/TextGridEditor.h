#ifndef _TextGridEditor_h_
#define _TextGridEditor_h_
/* TextGridEditor.h
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
#include "TextGrid.h"
#include "SpellingChecker.h"
#include "Preferences.h"

#include "TextGridEditor_enums.h"

Thing_declare1cpp (TextGridEditor);
#define TextGridEditor__methods(Klas) TimeSoundAnalysisEditor__methods(Klas)
Thing_declare2cpp (TextGridEditor, TimeSoundAnalysisEditor);
struct structTextGridEditor : public structTimeSoundAnalysisEditor {
	// new data:
		SpellingChecker spellingChecker;
		long selectedTier;
		bool useTextStyles, shiftDragMultiple, suppressRedraw;
		int fontSize;
		enum kGraphics_horizontalAlignment alignment;
		wchar *findString, greenString [Preferences_STRING_BUFFER_SIZE];
		enum kTextGridEditor_showNumberOf showNumberOf;
		enum kMelder_string greenMethod;
		GuiObject extractSelectedTextGridPreserveTimesButton, extractSelectedTextGridTimeFromZeroButton, writeSelectedTextGridButton;
	// overridden methods:
		void v_destroy ();
		void v_info ();
		void v_createChildren ();
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
		void v_dataChanged ();
};

TextGridEditor TextGridEditor_create (GuiObject parent, const wchar *title, TextGrid grid,
	Function sound,   // either a Sound or a LongSound, or null
	SpellingChecker spellingChecker);

void TextGridEditor_prefs (void);

/* End of file TextGridEditor.h */
#endif
