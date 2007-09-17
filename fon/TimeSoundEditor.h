#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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

/*
 * pb 2007/09/08
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _LongSound_h_
	#include "LongSound.h"
#endif

struct TimeSoundEditor_sound {
	/* KEEP IN SYNC WITH PREFS. */
	Sound data; int autoscaling;
	struct { int windowType; double relativeWidth; int preserveTimes; } publish;
};

#define TimeSoundEditor_members FunctionEditor_members \
	bool ownSound; \
	struct TimeSoundEditor_sound sound; \
	struct { LongSound data; } longSound; \
	Widget drawButton, publishButton, publishPreserveButton, publishWindowButton; \
	Widget writeAiffButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;
#define TimeSoundEditor_methods FunctionEditor_methods \
	void (*createMenuItems_view_sound) (I, EditorMenu menu); \
	void (*updateMenuItems_file) (I);

class_create (TimeSoundEditor, FunctionEditor);

void TimeSoundEditor_prefs (void);

int TimeSoundEditor_init (I, Widget parent, const wchar_t *title, Any data, Any sound, bool ownSound);

void TimeSoundEditor_draw_sound (I, double globalMinimum, double globalMaximum);

/* End of file TimeSoundEditor.h */
#endif
