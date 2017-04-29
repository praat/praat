#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FunctionEditor.h"
#include "Sound.h"
#include "LongSound.h"

#include "TimeSoundEditor_enums.h"

struct TimeSoundEditor_sound {
	Sound data;
	double minimum, maximum;
	long channelOffset;
	bool *muteChannels;
};

Thing_define (TimeSoundEditor, FunctionEditor) {
	bool d_ownSound;
	struct TimeSoundEditor_sound d_sound;
	struct { LongSound data; } d_longSound;
	GuiMenuItem drawButton, publishButton, publishPreserveButton, publishWindowButton, publishOverlapButton;
	GuiMenuItem writeAiffButton, d_saveAs24BitWavButton, d_saveAs32BitWavButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;

	void v_destroy () noexcept
		override;
	void v_info ()
		override;
	void v_createMenuItems_file (EditorMenu menu)
		override;
	void v_createMenuItems_query_info (EditorMenu menu)
		override;
	void v_createMenuItems_file_draw (EditorMenu menu)
		override;
	void v_createMenuItems_file_extract (EditorMenu menu)
		override;
	void v_createMenuItems_file_write (EditorMenu menu)
		override;
	void v_createMenuItems_view (EditorMenu menu)
		override;
	bool v_click (double xbegin, double ybegin, bool shiftKeyPressed)
		override;   // catch channel scrolling
	bool v_clickB (double xbegin, double ybegin)
		override;   // catch channel muting

	virtual void v_createMenuItems_view_sound (EditorMenu menu);
	virtual void v_updateMenuItems_file ();
	virtual const char32 * v_getChannelName (long /* channelNumber */) { return nullptr; }

	#include "TimeSoundEditor_prefs.h"
};

void TimeSoundEditor_init (TimeSoundEditor me, const char32 *title, Function data, Sampled sound, bool ownSound);

void TimeSoundEditor_drawSound (TimeSoundEditor me, double globalMinimum, double globalMaximum);

/* End of file TimeSoundEditor.h */
#endif
