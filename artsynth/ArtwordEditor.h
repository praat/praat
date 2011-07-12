#ifndef _ArtwordEditor_h_
#define _ArtwordEditor_h_
/* ArtwordEditor.h
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
 * pb 2011/07/02
 */

#include "Editor.h"
#include "Graphics.h"
#include "Artword.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (ArtwordEditor);

ArtwordEditor ArtwordEditor_create (GuiObject parent, const wchar *title, Artword data);

#ifdef __cplusplus
	}

	struct structArtwordEditor : public structEditor {
		Graphics graphics;
		int feature;
		GuiObject list, drawingArea, radio, time, value;
		GuiObject button [1 + kArt_muscle_MAX];
	};
	#define ArtwordEditor__methods(Klas) Editor__methods(Klas)
	Thing_declare2cpp (ArtwordEditor, Editor);

#endif // __cplusplus

/* End of file ArtwordEditor.h */
#endif

