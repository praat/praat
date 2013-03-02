#ifndef _EEGWindow_h_
#define _EEGWindow_h_
/* EEGWindow.h
 *
 * Copyright (C) 2011-2012,2013 Paul Boersma
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

Thing_define (EEGWindow, TextGridEditor) { public:
	// new data:
		EEG d_eeg;
		GuiMenuItem d_extractSelectedEEGPreserveTimesButton, d_extractSelectedEEGTimeFromZeroButton;
	// functions:
		void f_init (const wchar_t *title, EEG eeg);
	// overridden methods:
		virtual bool v_hasPitch     () { return false; }
		virtual bool v_hasIntensity () { return false; }
		virtual bool v_hasFormants  () { return false; }
		virtual bool v_hasPulses    () { return false; }
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual const wchar_t * v_getChannelName (long channelNumber);
		virtual void v_createMenuItems_file_extract (EditorMenu menu);
		virtual void v_updateMenuItems_file ();
	#include "EEGWindow_prefs.h"
};

EEGWindow EEGWindow_create (const wchar_t *title, EEG eeg);

/* End of file EEGWindow.h */
#endif
