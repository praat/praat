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

#include "TimeSoundAnalysisEditor.h"
#include "Movie.h"

Thing_define (MovieWindow, TimeSoundAnalysisEditor) {
	Movie movie() { return static_cast <Movie> (our data); }

	void v1_dataChanged () override {
		MovieWindow_Parent :: v1_dataChanged ();   // BUG: calls multiple functionChanged()
		our soundArea -> functionChanged (our movie() -> d_sound.get());
	}

	void v_createMenus ()
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_createMenuItems_view (EditorMenu menu)
		override;
	void v_highlightSelection (double left, double right, double bottom, double top)
		override;
};

autoMovieWindow MovieWindow_create (conststring32 title, Movie movie);

/* End of file MovieWindow.h */
#endif
