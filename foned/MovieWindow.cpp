/* MovieWindow.cpp
 *
 * Copyright (C) 2011-2020,2022 Paul Boersma
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

#include "MovieWindow.h"
#include "EditorM.h"

Thing_implement (MovieWindow, TimeSoundAnalysisEditor, 0);

/********** MENU COMMANDS **********/

void structMovieWindow :: v_createMenuItems_view (EditorMenu menu) {
	our MovieWindow_Parent :: v_createMenuItems_view (menu);
	//EditorMenu_addCommand (menu, L"-- view/realtier --", 0, 0);
	//EditorMenu_addCommand (menu, v_setRangeTitle (), 0, menu_cb_setRange);
}

void structMovieWindow :: v_createMenus () {
	our MovieWindow_Parent :: v_createMenus ();
	//EditorMenu menu = Editor_addMenu (this, L"Movie", 0);
	//EditorMenu_addCommand (menu, L"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	our v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the Movie menus
}

/********** DRAWING AREA **********/

/**
 * @returns a value between 0.0 and 1.0; depends on whether the Sound and/or analyses are visible
 */
static double _MovieWindow_getSoundBottomPosition (MovieWindow me) {
	const bool showAnalysis = ( (my instancePref_spectrogram_show() || my instancePref_pitch_show() ||
			my instancePref_intensity_show() || my instancePref_formant_show()) && my movie() -> d_sound );
	return my movie() -> d_sound ? (showAnalysis ? 0.7 : 0.3) : 1.0;
}

void structMovieWindow :: v_distributeAreas () {
	if (our soundArea) {
		const bool showAnalysis = our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
				our instancePref_intensity_show() || our instancePref_formant_show();
		const double yminSound_fraction = ( showAnalysis ? 0.7 : 0.3 );
		our soundArea -> setGlobalYRange_fraction (yminSound_fraction, 1.0);
	}
}

void structMovieWindow :: v_draw () {
	const bool showAnalysis = (our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
			our instancePref_intensity_show() || our instancePref_formant_show()) && our movie() -> d_sound;
	const double soundY = _MovieWindow_getSoundBottomPosition (this);
	if (our movie() -> d_sound) {
		Graphics_Viewport viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		SoundArea_draw (our soundArea.get());
		Graphics_resetViewport (our graphics.get(), viewport);
	}
	if (true) {
		Graphics_Viewport viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 0.3);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
		const integer firstFrame = Melder_clippedLeft (1_integer, Sampled_xToNearestIndex (our movie(), our startWindow));
		const integer lastFrame = Melder_clippedRight (Sampled_xToNearestIndex (our movie(), our endWindow), our movie() -> nx);
		for (integer iframe = firstFrame; iframe <= lastFrame; iframe ++) {
			const double time = Sampled_indexToX (our movie(), iframe);
			const double timeLeft = Melder_clippedLeft (our startWindow, time - 0.5 * movie() -> dx);
			const double timeRight = Melder_clippedRight (time + 0.5 * our movie() -> dx, our endWindow);
			Movie_paintOneImageInside (our movie(), our graphics.get(), iframe, timeLeft, timeRight, 0.0, 1.0);
		}
		Graphics_resetViewport (our graphics.get(), viewport);
	}
	if (showAnalysis) {
		Graphics_Viewport viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.3, soundY);
		our v_draw_analysis ();
		Graphics_resetViewport (our graphics.get(), viewport);
		/* Draw pulses. */
		if (our instancePref_pulses_show()) {
			viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, 1.0);
			our v_draw_analysis_pulses ();
			SoundArea_draw (our soundArea.get());   // second time, partially across the pulses
			Graphics_resetViewport (our graphics.get(), viewport);
		}
	}
	our v_updateMenuItems_file ();
}

void structMovieWindow :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our instancePref_spectrogram_show())
		Graphics_highlight (our graphics.get(), left, right, 0.3 * bottom + 0.7 * top, top);
	else
		Graphics_highlight (our graphics.get(), left, right, 0.7 * bottom + 0.3 * top, top);
}

bool structMovieWindow :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	return our MovieWindow_Parent :: v_mouseInWideDataView(event, x_world, y_fraction);
}

void structMovieWindow :: v_play (double startTime, double endTime) {
	Movie_play (our movie(), our graphics.get(), startTime, endTime, theFunctionEditor_playCallback, this);
}

autoMovieWindow MovieWindow_create (conststring32 title, Movie movie) {
	try {
		autoMovieWindow me = Thing_new (MovieWindow);
		if (movie -> d_sound)
			my soundArea = SoundArea_create (false, nullptr, me.get());
		FunctionEditor_init (me.get(), title, movie);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Movie window not created.");
	}
}

/* End of file MovieWindow.cpp */
