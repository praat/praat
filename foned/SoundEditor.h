#ifndef _SoundEditor_h_
#define _SoundEditor_h_
/* SoundEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2014-2016,2018,2020,2022 Paul Boersma
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
#include "LongSoundArea.h"
#include "SoundAnalysisArea.h"

Thing_define (SoundEditor, FunctionEditor) {
	SampledXY soundOrLongSound() { return our soundArea ? our soundArea -> soundOrLongSound() : nullptr; }
	Sound sound() { return our soundArea ? our soundArea -> sound() : nullptr; }
	LongSound longSound() { return our soundArea ? our soundArea -> longSound() : nullptr; }

	autoSoundArea soundArea;
	autoSoundAnalysisArea soundAnalysisArea;

	void v1_dataChanged () override {
		SoundEditor_Parent :: v1_dataChanged ();
		Thing_cast (SampledXY, soundOrLongSound, our data());
		our soundArea -> functionChanged (soundOrLongSound);
		our soundAnalysisArea -> functionChanged (soundOrLongSound);
	}
	void v_windowChanged () override {
		our soundArea -> v_windowChanged ();
		our soundAnalysisArea -> v_windowChanged ();
	}
	void v1_info () override {
		structFunctionEditor :: v1_info ();
		our soundArea -> v1_info ();
		our soundAnalysisArea -> v1_info ();
	}
	void v_createMenus () override {
		structFunctionEditor :: v_createMenus ();
		our soundArea -> v_createMenus ();
		our soundAnalysisArea -> v_createMenus ();
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
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_updateMenuItems () override {
		our soundArea -> v_updateMenuItems ();
	}
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	void v_play (double tmin, double tmax)
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double xWC, double yWC)
		override;
};

void SoundEditor_init (SoundEditor me, autoSoundArea soundArea,
	conststring32 title,
	SampledXY data
);

autoSoundEditor SoundEditor_create (
	conststring32 title,
	SampledXY data   // either a Sound or a LongSound
);

/* End of file SoundEditor.h */
#endif
