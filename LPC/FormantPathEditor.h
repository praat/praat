#ifndef _FormantPathEditor_h_
#define _FormantPathEditor_h_
/* FormantPathEditor.h
 *
 * Copyright (C) 2020-2022 David Weenink, 2022 Paul Boersma
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

#include "FormantPathArea.h"
#include "SoundArea.h"
#include "TextGridArea.h"

Thing_define (FormantPathEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, FormantPathArea, formantPathArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)
	DEFINE_FunctionArea (3, TextGridArea, textGridArea)

	FormantPath formantPath() { return static_cast <FormantPath> (our data()); }

	autoFormant previousFormant;
	Graphics_Viewport selectionViewer_viewport;
	integer selectedCandidate;
	GuiMenuItem navigateSettingsButton, navigateNextButton, navigatePreviousButton;

	void v1_info ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v1_dataChanged () override {
		FormantPathEditor_Parent :: v1_dataChanged ();
		our soundArea() -> functionChanged (nullptr);
		our formantPathArea() -> functionChanged (nullptr);
		our formantPathArea() -> d_formant = FormantPath_extractFormant (our formantPath());   // BUG: also on window changed
		our textGridArea() -> functionChanged (nullptr);
	}
	void v_distributeAreas () override {
		if (our textGridArea()) {
			our soundArea() -> setGlobalYRange_fraction (0.7, 1.0);
			our formantPathArea() -> setGlobalYRange_fraction (0.2, 0.7);
			our textGridArea() -> setGlobalYRange_fraction (0.0, 0.2);
		} else {
			our soundArea() -> setGlobalYRange_fraction (0.6, 1.0);
			our formantPathArea() -> setGlobalYRange_fraction (0.0, 0.6);
		}
	}
	void v_draw () override {
		FunctionArea_prepareCanvas (our soundArea().get());
		if (our formantPathArea() -> instancePref_pulses_show())
			our formantPathArea() -> v_draw_analysis_pulses ();
		FunctionArea_drawInside (our soundArea().get());
		if (our formantPathArea() -> hasContentToShow()) {
			FunctionArea_prepareCanvas (our formantPathArea().get());
			our formantPathArea() -> v_draw_analysis ();
		}
		if (our textGridArea())
			FunctionArea_drawOne (our textGridArea().get());
	}
	bool v_hasSelectionViewer () override { return true; }
	void v_drawSelectionViewer ()
		override;
	void v_clickSelectionViewer (double xWC, double yWC)
		override;
	void v_play (double startTime, double endTime)
		override;
	bool v_hasText () override { return false; }
	void v_updateText ()
		override {};
	conststring32 v_selectionViewerName ()
		override { return U"Formant candidates"; }

	#include "FormantPathEditor_prefs.h"
};

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid);

/* End of file FormantPathEditor.h */
#endif
