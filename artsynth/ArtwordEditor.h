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

#include "Editor.h"
#include "Artword.h"

Thing_define (ArtwordEditor, Editor) {
	// new data:
	public:
		Graphics graphics;
		int feature;
		GuiList list;
		GuiDrawingArea drawingArea;
		GuiText time, value;
		GuiRadioButton button [1 + kArt_muscle_MAX];
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_createChildren ();
		virtual void v_dataChanged ();
};

ArtwordEditor ArtwordEditor_create (const wchar_t *title, Artword data);

/* End of file ArtwordEditor.h */
#endif
