#ifndef _EEGWindow_h_
#define _EEGWindow_h_
/* EEGWindow.h
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "TextGridEditor.h"
#include "EEG.h"

Thing_define (EEGWindow, TextGridEditor) {
	// new data:
		public:
			EEG d_eeg;
	// functions:
		public:
			void f_init (GuiObject parent, const wchar *title, EEG eeg);
	// overridden methods:
		protected:
			virtual bool v_hasAnalysis () { return false; }
			virtual void v_createMenus ();
			virtual void v_createHelpMenuItems (EditorMenu menu);
			virtual const wchar * v_getChannelName (long channelNumber);
};

EEGWindow EEGWindow_create (GuiObject parent, const wchar *title, EEG eeg);

void EEGWindow_preferences (void);

/* End of file EEGWindow.h */
#endif
