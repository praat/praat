#ifndef _FormantPathEditor_h_
#define _FormantPathEditor_h_
/* FormantPathEditor.h
 *
 * Copyright (C) 2020-2023 David Weenink, 2022 Paul Boersma
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
	integer selectedCandidate = 0;
	GuiMenuItem navigateSettingsButton, navigateNextButton, navigatePreviousButton;

	void v1_info ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v1_dataChanged (Editor sender) override {
		FormantPathEditor_Parent :: v1_dataChanged (sender);
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);
		our formantPathArea() -> functionChanged (nullptr);
		our formantPathArea() -> d_formant = FormantPath_extractFormant (our formantPath());   // BUG: also on window changed
		if (our textGridArea())
			our textGridArea() -> functionChanged (nullptr);
	}
	void v_distributeAreas () override {
		if (our soundArea() && our textGridArea()) {
			our soundArea() -> setGlobalYRange_fraction (0.7, 1.0);
			our formantPathArea() -> setGlobalYRange_fraction (0.2, 0.7);
			our textGridArea() -> setGlobalYRange_fraction (0.0, 0.2);
		} else if (our soundArea()) {
			our soundArea() -> setGlobalYRange_fraction (0.6, 1.0);
			our formantPathArea() -> setGlobalYRange_fraction (0.0, 0.6);
		} else if (our textGridArea()) {
			our formantPathArea() -> setGlobalYRange_fraction (0.3, 1.0);
			our textGridArea() -> setGlobalYRange_fraction (0.0, 0.3);
		} else {
			our formantPathArea() -> setGlobalYRange_fraction (0.1, 1.0);
		}
	}
	void v_draw () override {
		if (our soundArea()) {
			FunctionArea_prepareCanvas (our soundArea().get());
			if (our formantPathArea() -> instancePref_pulses_show())
				our formantPathArea() -> v_draw_analysis_pulses ();
			FunctionArea_drawInside (our soundArea().get());
			if (our formantPathArea() -> hasContentToShow()) {
				FunctionArea_prepareCanvas (our formantPathArea().get());
				our formantPathArea() -> v_draw_analysis ();
			}
		} else {
			FunctionArea_prepareCanvas (our formantPathArea().get());
			our formantPathArea() -> v_draw_analysis_formants ();
		}
		if (our textGridArea())
			FunctionArea_drawOne (our textGridArea().get());
	}
	bool v_hasSelectionViewer () override { return true; }
	void v_drawSelectionViewer ()
		override;
	void v_drawRealTimeSelectionViewer (double /* time */) override {
		v_drawSelectionViewer (); // avoid discontinuity during play
	}
	void v_clickSelectionViewer (double xWC, double yWC)
		override;
	void v_play (double startTime, double endTime)
		override;
	bool v_hasText () override { return false; }
	void v_updateText ()
		override {};
	conststring32 v_selectionViewerName ()
		override { return U"Formant candidates"; }
	void v_drawLegends () override {
		FormantPathArea you = our formantPathArea().get();
		const bool showAnalyses = ( your endWindow() - your startWindow() <= your instancePref_longestAnalysis() );
		if (showAnalyses)
			FunctionArea_drawLegend (our formantPathArea().get(),
				your instancePref_spectrogram_show() ? FunctionArea_legend_GREYS U" %%derived spectrogram" : U"",
				1.2 * Melder_BLACK,
				your instancePref_intensity_show() ? FunctionArea_legend_LINES U" %%derived intensity" : U"",
				1.2 * Melder_GREEN,
				your instancePref_pitch_show() ? FunctionArea_legend_LINES_SPECKLES U" %%derived pitch" : U"",
				1.2 * Melder_BLUE,
				FunctionArea_legend_SPECKLES U" ##modifiable FormantPath",
				1.2 * Melder_RED
			);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
		if (our textGridArea())
			FunctionArea_drawLegend (our textGridArea().get(),
				FunctionArea_legend_TEXTGRID U" %%non-modifiable copy of TextGrid",
				DataGui_defaultForegroundColour (our textGridArea().get(), false)
			);
	}

	#include "FormantPathEditor_prefs.h"
};

autoFormantPathEditor FormantPathEditor_create (conststring32 title, FormantPath formantPath, Sound sound, TextGrid textgrid);

/* End of file FormantPathEditor.h */
#endif
