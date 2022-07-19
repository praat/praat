#ifndef _TimeSoundEditor_h_
#define _TimeSoundEditor_h_
/* TimeSoundEditor.h
 *
 * Copyright (C) 1992-2007,2009-2020,2022 Paul Boersma
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
#include "SoundArea.h"
#include "LongSoundArea.h"

Thing_define (TimeSoundEditor, FunctionEditor) {
	autoSoundArea soundArea;
	SampledXY soundOrLongSound() { return our soundArea ? our soundArea -> soundOrLongSound() : nullptr; }
	Sound sound() { return our soundArea ? our soundArea -> sound() : nullptr; }
	LongSound longSound() { return our soundArea ? our soundArea -> longSound() : nullptr; }

	void v1_info ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_file (EditorMenu menu)
		override;
	void v_createMenuItems_edit (EditorMenu menu)
		override;
	void v_updateMenuItems ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;   // catch channel scrolling and channel muting (last checked 2020-07-22)

	bool clickedInWideSoundArea = false;
};

/* End of file TimeSoundEditor.h */
#endif
