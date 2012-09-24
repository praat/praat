#ifndef _SoundEditor_h_
#define _SoundEditor_h_
/* SoundEditor.h
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

Thing_define (SoundEditor, TimeSoundAnalysisEditor) {
	// new data:
		private:
			GuiMenuItem cutButton, copyButton, pasteButton, zeroButton, reverseButton;
			double maxBuffer;
	// functions:
		public:
			void f_init (const wchar_t *title, Sampled data);
	// overridden methods:
		private:
			virtual void v_createMenus ();
			virtual void v_createHelpMenuItems (EditorMenu menu);
			virtual void v_dataChanged ();
			virtual void v_prepareDraw ();
			virtual void v_draw ();
			virtual void v_play (double tmin, double tmax);
			virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
			virtual void v_highlightSelection (double left, double right, double bottom, double top);
			virtual void v_unhighlightSelection (double left, double right, double bottom, double top);
};

SoundEditor SoundEditor_create (
	const wchar_t *title,
	Sampled data   // either a Sound or a LongSound
);

/* End of file SoundEditor.h */
#endif
