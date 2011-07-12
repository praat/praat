#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
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

/*
 * pb 2011/07/11
 */

#include "FunctionEditor.h"
#include "Sound.h"
#include "LongSound.h"

#ifdef __cplusplus
	extern "C" {
#endif

struct TimeSoundEditor_sound {
	/* KEEP IN SYNC WITH PREFS. */
	Sound data;
	bool autoscaling;
	double minimum, maximum;
};

Thing_declare1cpp (TimeSoundEditor);

void TimeSoundEditor_prefs (void);

void TimeSoundEditor_init (TimeSoundEditor me, GuiObject parent, const wchar *title, Data data, Data sound, bool ownSound);

void TimeSoundEditor_draw_sound (TimeSoundEditor me, double globalMinimum, double globalMaximum);

#ifdef __cplusplus
	}

	struct structTimeSoundEditor : public structFunctionEditor {
		bool ownSound;
		struct TimeSoundEditor_sound sound;
		struct { LongSound data; } longSound;
		GuiObject drawButton, publishButton, publishPreserveButton, publishWindowButton;
		GuiObject writeAiffButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton, writeFlacButton;
	};
	#define TimeSoundEditor__methods(Klas) FunctionEditor__methods(Klas) \
		void (*createMenuItems_view_sound) (Klas me, EditorMenu menu); \
		void (*updateMenuItems_file) (Klas me);
	Thing_declare2cpp (TimeSoundEditor, FunctionEditor);

#endif // __cplusplus

/* End of file TimeSoundEditor.h */
#endif
