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

	void v1_info () override {
		structFunctionEditor :: v1_info ();
		if (our soundArea)
			our soundArea -> v1_info ();
	}
	void v_createMenus () override {
		structFunctionEditor :: v_createMenus ();
		if (our soundArea)
			our soundArea -> v_createMenus ();
	}
	void v_createMenuItems_file (EditorMenu menu) override {
		structFunctionEditor :: v_createMenuItems_file (menu);
		our v_createMenuItems_file_write (menu);
		if (our soundArea)
			our soundArea -> v_createMenuItems_file (menu);
		EditorMenu_addCommand (menu, U"-- after file write --", 0, nullptr);
	}
	void v_createMenuItems_edit (EditorMenu menu) override {
		structFunctionEditor :: v_createMenuItems_edit (menu);
		if (our soundArea)
			our soundArea -> v_createMenuItems_edit (menu);
	}
	void v_updateMenuItems () override {
		if (! our soundOrLongSound())
			return;
		if (our soundArea)
			our soundArea -> v_updateMenuItems ();
	}
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) override {
		if (event -> isClick ()) {
			if (our soundArea)
				our soundArea -> isClickAnchor = our soundArea -> y_fraction_globalIsInside (globalY_fraction);
		}
		bool result = false;
		if (our soundArea && our soundArea -> isClickAnchor) {
			result = SoundArea_mouse (our soundArea.get(), event, x_world, globalY_fraction);
		} else {
			result = FunctionEditor_defaultMouseInWideDataView (this, event, x_world);
		}
		if (event -> isDrop()) {
			if (our soundArea)
				our soundArea -> isClickAnchor = false;
		}
		return result;
	}
};

/* End of file TimeSoundEditor.h */
#endif
