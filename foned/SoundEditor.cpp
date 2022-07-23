/* SoundEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "SoundEditor.h"
#include "EditorM.h"

Thing_implement (SoundEditor, FunctionEditor, 0);

static void menu_cb_SoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"SoundEditor"); }
static void menu_cb_LongSoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"LongSoundEditor"); }

void structSoundEditor :: v_createMenuItems_help (EditorMenu menu) {
	structFunctionEditor :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"SoundEditor help", '?', menu_cb_SoundEditorHelp);
	EditorMenu_addCommand (menu, U"LongSoundEditor help", 0, menu_cb_LongSoundEditorHelp);
	// BUG: add help on Sound area and Sound analysis area
}

void structSoundEditor :: v_distributeAreas () {
	if (our soundAnalysisArea -> hasContentToShow ()) {
		our soundArea -> setGlobalYRange_fraction (0.5, 1.0);
		our soundAnalysisArea -> setGlobalYRange_fraction (0.0, 0.5);
	} else {
		our soundArea -> setGlobalYRange_fraction (0.0, 1.0);
		our soundAnalysisArea -> setGlobalYRange_fraction (0.0, 0.0);
	}
}

void structSoundEditor :: v_draw () {
	FunctionArea_prepareCanvas (our soundArea.get());
	if (our soundAnalysisArea -> instancePref_pulses_show())
		our soundAnalysisArea -> v_draw_analysis_pulses ();
	FunctionArea_drawInside (our soundArea.get());
	if (our soundAnalysisArea -> hasContentToShow ()) {
		FunctionArea_prepareCanvas (our soundAnalysisArea.get());
		our soundAnalysisArea -> v_draw_analysis ();
	}
}

void structSoundEditor :: v_play (double startTime, double endTime) {
	SoundArea_play (our soundArea.get(), startTime, endTime);
}

bool structSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	if (event -> isClick ()) {
		our soundArea -> isClickAnchor = our soundArea -> y_fraction_globalIsInside (globalY_fraction);
		our soundAnalysisArea -> isClickAnchor = our soundAnalysisArea -> y_fraction_globalIsInside (globalY_fraction);
	}
	bool result = false;
	if (our soundArea -> isClickAnchor) {
		result = SoundArea_mouse (our soundArea.get(), event, x_world, globalY_fraction);
	} else if (our soundAnalysisArea && our soundAnalysisArea -> isClickAnchor) {
		result = SoundAnalysisArea_mouse (our soundAnalysisArea.get(), event, x_world, globalY_fraction);
	} else {
		result = our structFunctionEditor :: v_mouseInWideDataView (event, x_world, globalY_fraction);
	}
	if (event -> isDrop()) {
		our soundArea -> isClickAnchor = false;
		our soundAnalysisArea -> isClickAnchor = false;
	}
	return result;
}

autoSoundEditor SoundEditor_create (conststring32 title, SampledXY soundOrLongSound) {
	Melder_assert (soundOrLongSound);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		if (Thing_isa (soundOrLongSound, classSound))
			my soundArea = SoundArea_create (true, nullptr, me.get());
		else
			my soundArea = LongSoundArea_create (false, nullptr, me.get());
		my soundAnalysisArea = SoundAnalysisArea_create (false, nullptr, me.get());
		FunctionEditor_init (me.get(), title, soundOrLongSound);

		Melder_assert (my soundOrLongSound());
		if (my longSound() && my endWindow - my startWindow > 30.0) {   // BUG: should be in dataChanged?
			my endWindow = my startWindow + 30.0;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
			FunctionEditor_marksChanged (me.get(), false);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound window not created.");
	}
}

/* End of file SoundEditor.cpp */
