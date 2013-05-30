#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
 *
 * Copyright (C) 1992-2012, 2013 Paul Boersma
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

#include "FunctionEditor.h"
#include "Sound.h"
#include "LongSound.h"

#include "TimeSoundEditor_enums.h"

struct TimeSoundEditor_sound {
	Sound data;
	double minimum, maximum;
	long channelOffset;
};

Thing_define (TimeSoundEditor, FunctionEditor) {
	// new data:
		public:
			bool d_ownSound;
			struct TimeSoundEditor_sound d_sound;
			struct { LongSound data; } d_longSound;
			GuiMenuItem drawButton, publishButton, publishPreserveButton, publishWindowButton, publishOverlapButton;
			GuiMenuItem writeAiffButton, d_saveAs24BitWavButton, d_saveAs32BitWavButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;
	// messages:
		public:
			void f_init (const wchar_t *title, Function data, Sampled sound, bool ownSound);
			void f_drawSound (double globalMinimum, double globalMaximum);
	// overridden methods:
		protected:
			virtual void v_destroy ();
			virtual void v_info ();
			virtual void v_createMenuItems_file (EditorMenu menu);
			virtual void v_createMenuItems_query_info (EditorMenu menu);
			virtual void v_createMenuItems_file_draw (EditorMenu menu);
			virtual void v_createMenuItems_file_extract (EditorMenu menu);
			virtual void v_createMenuItems_file_write (EditorMenu menu);
			virtual void v_createMenuItems_view (EditorMenu menu);
			virtual int v_click (double xbegin, double ybegin, bool shiftKeyPressed);   // catch channel scrolling
	// new methods:
		protected:
			virtual void v_createMenuItems_view_sound (EditorMenu menu);
			virtual void v_updateMenuItems_file ();
			virtual const wchar_t * v_getChannelName (long channelNumber) { (void) channelNumber; return NULL; }
	#include "TimeSoundEditor_prefs.h"
};

/* End of file TimeSoundEditor.h */
#endif
