#ifndef _MovieWindow_h_
#define _MovieWindow_h_
/* MovieWindow.h
 *
 * Copyright (C) 2011,2012,2014-2016,2018,2020,2022 Paul Boersma
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
#include "MovieArea.h"
#include "SoundArea.h"
#include "SoundAnalysisArea.h"

Thing_define (MovieWindow, FunctionEditor) {
	DEFINE_FunctionArea (1, MovieArea, videoArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)
	DEFINE_FunctionArea (3, SoundAnalysisArea, soundAnalysisArea)

	Movie movie() { return static_cast <Movie> (our data()); }

	void v1_dataChanged (Editor sender) override {
		MovieWindow_Parent :: v1_dataChanged (sender);   // BUG: calls multiple functionChanged()
		our videoArea() -> functionChanged (our movie());
		our soundArea() -> functionChanged (our movie() -> d_sound.get());
		our soundAnalysisArea() -> functionChanged (our movie() -> d_sound.get());
	}
	void v_distributeAreas () override {
		if (our soundAnalysisArea() -> hasContentToShow ()) {
			our videoArea() -> setGlobalYRange_fraction (0.0, 0.3);
			our soundArea() -> setGlobalYRange_fraction (0.7, 1.0);
			our soundAnalysisArea() -> setGlobalYRange_fraction (0.3, 0.7);
		} else {
			our videoArea() -> setGlobalYRange_fraction (0.0, 0.3);
			our soundArea() -> setGlobalYRange_fraction (0.3, 1.0);
			our soundAnalysisArea() -> setGlobalYRange_fraction (0.0, 0.0);
		}
	}
	void v_draw () override {
		if (our soundArea()) {
			FunctionArea_prepareCanvas (our soundArea().get());
			if (our soundAnalysisArea() -> instancePref_pulses_show())
				our soundAnalysisArea() -> v_draw_analysis_pulses ();
			FunctionArea_drawInside (our soundArea().get());
			if (our soundAnalysisArea() -> hasContentToShow ()) {
				FunctionArea_prepareCanvas (our soundAnalysisArea().get());
				our soundAnalysisArea() -> v_draw_analysis ();
			}
		}
		FunctionArea_drawOne (our videoArea().get());
	}
	void v_play (double startTime, double endTime) override {
		Movie_play (our movie(), our graphics.get(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
};

autoMovieWindow MovieWindow_create (conststring32 title, Movie movie);

/* End of file MovieWindow.h */
#endif
