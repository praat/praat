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
	DEFINE_FunctionArea (1, SoundArea, soundArea)
	DEFINE_FunctionArea (2, SoundAnalysisArea, soundAnalysisArea)

	friend void SoundEditor_init (SoundEditor me, conststring32 title, SampledXY soundOrLongSound) {
		if (Thing_isa (soundOrLongSound, classSound))
			my soundArea() = SoundArea_create (true, nullptr, me);
		else
			my soundArea() = LongSoundArea_create (false, nullptr, me);
		my soundAnalysisArea() = SoundAnalysisArea_create (false, nullptr, me);
		FunctionEditor_init (me, title, soundOrLongSound);

		Melder_assert (my soundArea() -> soundOrLongSound());
		if (my soundArea() -> longSound() && my endWindow - my startWindow > 30.0) {   // BUG: should be in dataChanged?
			my endWindow = my startWindow + 30.0;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
			FunctionEditor_marksChanged (me, false);
		}
	}

	void v1_dataChanged () override {
		SoundEditor_Parent :: v1_dataChanged ();
		Thing_cast (SampledXY, soundOrLongSound, our data());
		our soundArea() -> functionChanged (soundOrLongSound);
		our soundAnalysisArea() -> functionChanged (soundOrLongSound);
	}
	void v1_info () override {
		structFunctionEditor :: v1_info ();
		our soundArea() -> v1_info ();
		our soundAnalysisArea() -> v1_info ();
	}
	void v_createMenus () override {
		structFunctionEditor :: v_createMenus ();
		our soundArea() -> v_createMenus ();
		our soundAnalysisArea() -> v_createMenus ();
	}
	void v_createMenuItems_file (EditorMenu menu) override {
		structFunctionEditor :: v_createMenuItems_file (menu);
		our v_createMenuItems_file_write (menu);
		our soundArea() -> v_createMenuItems_file (menu);
		EditorMenu_addCommand (menu, U"-- after file write --", 0, nullptr);
	}
	void v_createMenuItems_edit (EditorMenu menu) override {
		structFunctionEditor :: v_createMenuItems_edit (menu);
		our soundArea() -> v_createMenuItems_edit (menu);
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_updateMenuItems () override {
		our soundArea() -> v_updateMenuItems ();
		our soundAnalysisArea() -> v_updateMenuItems ();
	}
	void v_distributeAreas () override {
		if (our soundAnalysisArea() -> hasContentToShow ()) {
			our soundArea() -> setGlobalYRange_fraction (0.5, 1.0);
			our soundAnalysisArea() -> setGlobalYRange_fraction (0.0, 0.5);
		} else {
			our soundArea() -> setGlobalYRange_fraction (0.0, 1.0);
			our soundAnalysisArea() -> setGlobalYRange_fraction (0.0, 0.0);
		}
	}
	void v_draw () override {
		FunctionArea_prepareCanvas (our soundArea().get());
		if (our soundAnalysisArea() -> instancePref_pulses_show())
			our soundAnalysisArea() -> v_draw_analysis_pulses ();
		FunctionArea_drawInside (our soundArea().get());
		if (our soundAnalysisArea() -> hasContentToShow ()) {
			FunctionArea_prepareCanvas (our soundAnalysisArea().get());
			our soundAnalysisArea() -> v_draw_analysis ();
		}
	}
	void v_play (double startTime, double endTime) override {
		SoundArea_play (our soundArea().get(), startTime, endTime);
	}
};

autoSoundEditor SoundEditor_create (
	conststring32 title,
	SampledXY data   // either a Sound or a LongSound
);

/* End of file SoundEditor.h */
#endif
