#ifndef _TextGridEditor_h_
#define _TextGridEditor_h_
/* TextGridEditor.h
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
 * pb 2011/03/23
 */

#include "TimeSoundAnalysisEditor.h"
#include "TextGrid.h"

#ifdef __cplusplus
	extern "C" {
#endif

#include "TextGridEditor_enums.h"

#define TextGridEditor__parents(Klas) TimeSoundAnalysisEditor__parents(Klas) Thing_inherit (Klas, TimeSoundAnalysisEditor)
Thing_declare1 (TextGridEditor);

TextGridEditor TextGridEditor_create (GuiObject parent, const wchar_t *title, TextGrid grid,
	Any sound,   /* 'sound' could be a Sound or a LongSound */
	Any spellingChecker);

void TextGridEditor_prefs (void);

#ifdef __cplusplus
	}
#endif

/* End of file TextGridEditor.h */
#endif
