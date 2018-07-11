#ifndef _ArtwordEditor_h_
#define _ArtwordEditor_h_
/* ArtwordEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015-2018 Paul Boersma
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

#include "Editor.h"
#include "Artword.h"

Thing_define (ArtwordEditor, Editor) {
	autoGraphics graphics;
	kArt_muscle muscle;
	GuiList list;
	GuiDrawingArea drawingArea;
	GuiText time, value;
	GuiRadioButton button [1 + (int) kArt_muscle::MAX];

	void v_destroy () noexcept
		override;
	void v_createChildren ()
		override;
	void v_dataChanged ()
		override;
};

autoArtwordEditor ArtwordEditor_create (conststring32 title, Artword data);

/* End of file ArtwordEditor.h */
#endif
