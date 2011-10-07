/* Graphics_mouse.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "GraphicsP.h"
#include "Gui.h"

#if cairo
	static bool theMouseDown = true;
#endif

/*
 * Graphics_mouseStillDown () can only be used in a loop
 * if Graphics_getMouseLocation () is called in that same loop.
 * This is because the Xwin version requires that.
 */

bool structGraphicsScreen :: v_mouseStillDown () {
	#if cairo
		if (theMouseDown) return true;
		else { theMouseDown = true; return false; }
	#elif win
		return motif_win_mouseStillDown ();
	#elif mac
		return StillDown ();
	#endif
}

bool Graphics_mouseStillDown (Graphics me) {
	return my v_mouseStillDown ();
}

void structGraphicsScreen :: v_getMouseLocation (double *xWC, double *yWC) {
	#if cairo
		GdkEvent *gevent = gdk_display_get_event (d_display);
		if (gevent != NULL) {
			if (gevent -> type == GDK_BUTTON_RELEASE) {
				theMouseDown = false;
			}
			gdk_event_free (gevent);
		}
		gint xDC, yDC;
		gdk_window_get_pointer (d_window, & xDC, & yDC, NULL);
		Graphics_DCtoWC (this, xDC, yDC, xWC, yWC);
	#elif win
		POINT pos;
		if (! GetCursorPos (& pos)) { Melder_warning (L"Cannot get cursor position."); return; }
		ScreenToClient (d_winWindow, & pos);
		Graphics_DCtoWC (this, pos. x, pos. y, xWC, yWC);
	#elif mac
		if (HIGetMousePosition != NULL && false) {   // AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
			//Melder_casual ("HIGetMousePosition exists");
			HIPoint mouseLoc;
			HIGetMousePosition (kHICoordSpaceWindow, GetWindowFromPort (d_macPort), & mouseLoc);
			Graphics_DCtoWC (this, mouseLoc. x, mouseLoc. y, xWC, yWC);
		} else {
			Point mouseLoc;
			GetMouse (& mouseLoc);   // AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
			Graphics_DCtoWC (this, mouseLoc. h, mouseLoc. v, xWC, yWC);
		}
	#endif
}

void Graphics_getMouseLocation (Graphics me, double *xWC, double *yWC) {
	my v_getMouseLocation (xWC, yWC);
}

void Graphics_waitMouseUp (Graphics me) {
	while (Graphics_mouseStillDown (me)) {
		double xWC, yWC;
		Graphics_getMouseLocation (me, & xWC, & yWC);
	}
}

/* End of file Graphics_mouse.cpp */
