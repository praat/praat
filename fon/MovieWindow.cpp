/* MovieWindow.cpp
 *
 * Copyright (C) 2011,2012 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "MovieWindow.h"
#include "Preferences.h"
#include "EditorM.h"

Thing_implement (MovieWindow, TimeSoundAnalysisEditor, 0);

/********** MENU COMMANDS **********/

void structMovieWindow :: v_createMenuItems_view (EditorMenu menu) {
	MovieWindow_Parent :: v_createMenuItems_view (menu);
	//EditorMenu_addCommand (menu, L"-- view/realtier --", 0, 0);
	//EditorMenu_addCommand (menu, v_setRangeTitle (), 0, menu_cb_setRange);
}

void structMovieWindow :: v_createMenus () {
	MovieWindow_Parent :: v_createMenus ();
	//EditorMenu menu = Editor_addMenu (this, L"Movie", 0);
	//EditorMenu_addCommand (menu, L"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the Movie menus
}

/********** DRAWING AREA **********/

double structMovieWindow :: h_getSoundBottomPosition () {
	Movie movie = (Movie) data;
	bool showAnalysis = (spectrogram.show || pitch.show || intensity.show || formant.show) && movie -> d_sound;
	return movie -> d_sound ? (showAnalysis ? 0.7 : 0.3) : 1.0;
}

void structMovieWindow :: v_draw () {
	Movie movie = (Movie) data;
	bool showAnalysis = (spectrogram.show || pitch.show || intensity.show || formant.show) && movie -> d_sound;
	double soundY = h_getSoundBottomPosition ();
	if (movie -> d_sound) {
		Graphics_Viewport viewport = Graphics_insetViewport (d_graphics, 0, 1, soundY, 1.0);
		Graphics_setColour (d_graphics, Graphics_WHITE);
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
		f_drawSound (-1.0, 1.0);
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, viewport);
	}
	if (true) {
		Graphics_Viewport viewport = Graphics_insetViewport (d_graphics, 0.0, 1.0, 0.0, 0.3);
		Graphics_setColour (d_graphics, Graphics_WHITE);
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
		Graphics_setColour (d_graphics, Graphics_BLACK);
		Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0.0, 1.0);
		long firstFrame = round (Sampled_xToIndex (movie, d_startWindow));
		long lastFrame = round (Sampled_xToIndex (movie, d_endWindow));
		if (firstFrame < 1) firstFrame = 1;
		if (lastFrame > movie -> nx) lastFrame = movie -> nx;
		for (long iframe = firstFrame; iframe <= lastFrame; iframe ++) {
			double time = Sampled_indexToX (movie, iframe);
			double timeLeft = time - 0.5 * movie -> dx, timeRight = time + 0.5 * movie -> dx;
			if (timeLeft < d_startWindow) timeLeft = d_startWindow;
			if (timeRight > d_endWindow) timeRight = d_endWindow;
			movie -> f_paintOneImageInside (d_graphics, iframe, timeLeft, timeRight, 0.0, 1.0);
		}
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, viewport);
	}
	if (showAnalysis) {
		Graphics_Viewport viewport = Graphics_insetViewport (d_graphics, 0.0, 1.0, 0.3, soundY);
		v_draw_analysis ();
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, viewport);
		/* Draw pulses. */
		if (pulses.show) {
			viewport = Graphics_insetViewport (d_graphics, 0.0, 1.0, soundY, 1.0);
			v_draw_analysis_pulses ();
			f_drawSound (-1.0, 1.0);   // second time, partially across the pulses
			Graphics_flushWs (d_graphics);
			Graphics_resetViewport (d_graphics, viewport);
		}
	}
	v_updateMenuItems_file ();
}

void structMovieWindow :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (spectrogram.show)
		Graphics_highlight (d_graphics, left, right, 0.3 * bottom + 0.7 * top, top);
	else
		Graphics_highlight (d_graphics, left, right, 0.7 * bottom + 0.3 * top, top);
}

void structMovieWindow :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (spectrogram.show)
		Graphics_highlight (d_graphics, left, right, 0.3 * bottom + 0.7 * top, top);
	else
		Graphics_highlight (d_graphics, left, right, 0.7 * bottom + 0.3 * top, top);
}

int structMovieWindow :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	return MovieWindow_Parent :: v_click (xWC, yWC, shiftKeyPressed);
}

void structMovieWindow :: v_play (double a_tmin, double a_tmax) {
	Movie movie = (Movie) data;
	movie -> f_play (d_graphics, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

void structMovieWindow :: f_init (const wchar_t *title, Movie movie) {
	Melder_assert (movie != NULL);
	TimeSoundAnalysisEditor_init (this, title, movie, movie -> d_sound, false);
}

MovieWindow MovieWindow_create (const wchar_t *title, Movie movie) {
	try {
		autoMovieWindow me = Thing_new (MovieWindow);
		my f_init (title, movie);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Movie window not created.");
	}
}

/* End of file MovieWindow.cpp */
