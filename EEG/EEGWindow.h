#ifndef _EEGWindow_h_
#define _EEGWindow_h_
/* EEGWindow.h
 *
 * Copyright (C) 2011-2018,2022 Paul Boersma
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
#include "EEGArea.h"
#include "EEGAnalysisArea.h"
#include "TextGridArea.h"

Thing_define (EEGWindow, FunctionEditor) {
	DEFINE_FunctionArea (1, EEGArea, eegArea)
	DEFINE_FunctionArea (2, EEGAnalysisArea, eegAnalysisArea)
	DEFINE_FunctionArea (3, TextGridArea, textGridArea)

	EEG eeg() { return static_cast <EEG> (our data()); }

	bool v_hasText () override { return true; }
	bool v_hasSelectionViewer () override { return true; }
	void v_drawSelectionViewer () override {
		TextGridArea_drawSelectionViewer (our textGridArea().get());
	}
	void v_clickSelectionViewer (double x_fraction, double y_fraction) override {
		TextGridArea_clickSelectionViewer (our textGridArea().get(), x_fraction, y_fraction);
	}
	conststring32 v_selectionViewerName ()
		override { return U"IPA chart"; }
	void v1_dataChanged (Editor sender) override {
		our EEGWindow_Parent :: v1_dataChanged (sender);
		our eegArea() -> functionChanged (our eeg() -> sound.get());
		our eegAnalysisArea() -> functionChanged (our eeg() -> sound.get());
		our textGridArea() -> functionChanged (our eeg() -> textgrid.get());
	}
	void v_distributeAreas () override {
		our eegArea() -> setGlobalYRange_fraction (0.65, 1.0);
		our eegAnalysisArea() -> setGlobalYRange_fraction (0.35, 0.65);
		our textGridArea() -> setGlobalYRange_fraction (0.0, 0.35);
	}
	void v_draw () override {
		FunctionArea_drawOne (our eegArea().get());
		if (our eegAnalysisArea() -> hasContentToShow()) {
			FunctionArea_prepareCanvas (our eegAnalysisArea().get());
			our eegAnalysisArea() -> v_draw_analysis ();
		}
		FunctionArea_drawOne (our textGridArea().get());
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_createMenus ()
		override;
	void v_updateMenuItems ()
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our textGridArea().get(),
			FunctionArea_legend_TEXTGRID U" ##modifiable TextGrid",
			DataGui_defaultForegroundColour (our textGridArea().get(), false)
		);
		FunctionArea_drawLegend (our eegArea().get(),
			FunctionArea_legend_WAVEFORM U" %%non-modifiable EEG-internal sound",
			DataGui_defaultForegroundColour (our eegArea().get(), false)
		);
		SoundAnalysisArea_drawDefaultLegends (our eegAnalysisArea().get());
	}

	GuiMenuItem extractSelectedEEGPreserveTimesButton, extractSelectedEEGTimeFromZeroButton;

	#include "EEGWindow_prefs.h"
};

autoEEGWindow EEGWindow_create (conststring32 title, EEG eeg);

/* End of file EEGWindow.h */
#endif
