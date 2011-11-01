#ifndef _MovieWindow_h_
#define _MovieWindow_h_
/* MovieWindow.h
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "TimeSoundAnalysisEditor.h"
#include "Movie.h"

Thing_define (MovieWindow, TimeSoundAnalysisEditor) {
	// functions:
		public:
			void f_init (GuiObject parent, const wchar *title, Movie data);
	// overridden methods:
		protected:
			virtual void v_createMenus ();
			virtual void v_draw ();
			virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
			virtual void v_play (double tmin, double tmax);
			virtual void v_createMenuItems_view (EditorMenu menu);
			virtual void v_highlightSelection (double left, double right, double bottom, double top);
			virtual void v_unhighlightSelection (double left, double right, double bottom, double top);
	// helper functions:
		private:
			double h_getSoundBottomPosition ();   // between 0.0 and 1.0; depends on whether the Sound and/or analyses are visible
};

MovieWindow MovieWindow_create (GuiObject parent, const wchar *title, Movie movie);

/* End of file MovieWindow.h */
#endif
