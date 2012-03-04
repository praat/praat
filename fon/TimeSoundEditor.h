#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

struct TimeSoundEditor_sound {
	/* KEEP IN SYNC WITH PREFS. */
	Sound data;
	bool autoscaling;
	double minimum, maximum;
	long channelOffset;
};

Thing_define (TimeSoundEditor, FunctionEditor) {
	// new data:
	public:
		bool ownSound;
		struct TimeSoundEditor_sound sound;
		struct { LongSound data; } longSound;
		GuiObject drawButton, publishButton, publishPreserveButton, publishWindowButton;
		GuiObject writeAiffButton, d_saveAs24BitWavButton, d_saveAs32BitWavButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;
	// overridden methods:
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
		virtual void v_createMenuItems_view_sound (EditorMenu menu);
		virtual void v_updateMenuItems_file ();
		virtual const wchar * v_getChannelName (long channelNumber) { (void) channelNumber; return NULL; }
};

void TimeSoundEditor_prefs (void);

void TimeSoundEditor_init (TimeSoundEditor me, GuiObject parent, const wchar *title, Function data, Sampled sound, bool ownSound);

void TimeSoundEditor_draw_sound (TimeSoundEditor me, double globalMinimum, double globalMaximum);

/* End of file TimeSoundEditor.h */
#endif
