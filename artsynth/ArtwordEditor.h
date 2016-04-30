#ifndef _ArtwordEditor_h_
#define _ArtwordEditor_h_
/* ArtwordEditor.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Editor.h"
#include "Artword.h"

Thing_define (ArtwordEditor, Editor) {
	autoGraphics graphics;
	int feature;
	GuiList list;
	GuiDrawingArea drawingArea;
	GuiText time, value;
	GuiRadioButton button [1 + kArt_muscle_MAX];

	void v_destroy () noexcept
		override;
	void v_createChildren ()
		override;
	void v_dataChanged ()
		override;
};

autoArtwordEditor ArtwordEditor_create (const char32 *title, Artword data);

/* End of file ArtwordEditor.h */
#endif
