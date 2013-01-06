#ifndef _TextGridEditor_h_
#define _TextGridEditor_h_
/* TextGridEditor.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

Thing_define (TextGridEditor, TimeSoundAnalysisEditor) {
	// new data:
	public:
		SpellingChecker spellingChecker;
		long selectedTier;
		bool suppressRedraw;
		wchar_t *findString;
		GuiMenuItem extractSelectedTextGridPreserveTimesButton, extractSelectedTextGridTimeFromZeroButton;
	// functions:
	public:
		void f_init (const wchar_t *title, TextGrid grid,
			Sampled sound,   // either a Sound or a LongSound, or null
			bool ownSound,
			SpellingChecker spellingChecker);
	// overridden methods:
		virtual void v_info ();
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_dataChanged ();
		virtual void v_createMenuItems_file_extract (EditorMenu menu);
		virtual void v_createMenuItems_file_write (EditorMenu menu);
		virtual void v_createMenuItems_file_draw (EditorMenu menu);
		virtual void v_prepareDraw ();
		virtual void v_draw ();
		virtual bool v_hasText () { return true; }
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual int v_clickB (double xWC, double yWC);
		virtual int v_clickE (double xWC, double yWC);
		virtual void v_play (double tmin, double tmax);
		virtual void v_updateText ();
		virtual void v_prefs_addFields (EditorCommand cmd);
		virtual void v_prefs_setValues (EditorCommand cmd);
		virtual void v_prefs_getValues (EditorCommand cmd);
		virtual void v_createMenuItems_view_timeDomain (EditorMenu menu);
		virtual void v_highlightSelection (double left, double right, double bottom, double top);
		virtual void v_unhighlightSelection (double left, double right, double bottom, double top);
		virtual double v_getBottomOfSoundArea ();
		virtual double v_getBottomOfSoundAndAnalysisArea ();
		virtual void v_updateMenuItems_file ();
		virtual void v_createMenuItems_pitch_picture (EditorMenu menu);
	#include "TextGridEditor_prefs.h"
};

TextGridEditor TextGridEditor_create (const wchar_t *title, TextGrid grid,
	Sampled sound,   // either a Sound or a LongSound, or null
	bool ownSound,
	SpellingChecker spellingChecker);

/* End of file TextGridEditor.h */
#endif
