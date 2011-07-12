#ifndef _SoundEditor_h_
#define _SoundEditor_h_
/* SoundEditor.h
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

#include "TimeSoundAnalysisEditor.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (SoundEditor);

SoundEditor SoundEditor_create (GuiObject parent, const wchar *title, Data data);

#ifdef __cplusplus
	}

	struct structSoundEditor : public structTimeSoundAnalysisEditor {
		GuiObject cutButton, copyButton, pasteButton, zeroButton, reverseButton;
		double maxBuffer;
	};
	#define SoundEditor__methods(Klas) TimeSoundAnalysisEditor__methods(Klas)
	Thing_declare2cpp (SoundEditor, TimeSoundAnalysisEditor);

#endif // __cplusplus

/* End of file SoundEditor.h */
#endif
