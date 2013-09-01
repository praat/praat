/* Graphics_mouse.cpp
 *
 * Copyright (C) 1992-2011,2013 Paul Boersma, 2013 Tom Naughton
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

/*
 * Graphics_mouseStillDown () can only be used in a loop
 * if Graphics_getMouseLocation () is called in that same loop.
 * This is because the Xwin version requires that.
 */

bool structGraphicsScreen :: v_mouseStillDown () {
	#if cairo
		Graphics_flushWs (this);
		GdkEvent *gevent = gdk_display_get_event (d_display);
		if (! gevent) return true;
		int gdkEventType = gevent -> type;
		gdk_event_free (gevent);
		return gdkEventType != GDK_BUTTON_RELEASE;
	#elif cocoa
		Graphics_flushWs (this);
		NSEvent *nsEvent = [[d_macView window]
			nextEventMatchingMask: NSLeftMouseUpMask | NSLeftMouseDraggedMask | NSKeyDownMask
			untilDate: [NSDate distantFuture]
			inMode: NSEventTrackingRunLoopMode
			dequeue: YES
			];
		NSUInteger nsEventType = [nsEvent type];
		if (nsEventType == NSKeyDown) NSBeep ();
		return nsEventType != NSLeftMouseUp;
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
		gint xDC, yDC;
		gdk_window_get_pointer (d_window, & xDC, & yDC, NULL);
		Graphics_DCtoWC (this, xDC, yDC, xWC, yWC);
	#elif cocoa
        NSPoint mouseLoc = [[d_macView window]  mouseLocationOutsideOfEventStream];
        mouseLoc = [d_macView   convertPoint: mouseLoc   fromView: nil];
        mouseLoc. y = d_macView. bounds. size. height - mouseLoc. y;
        Graphics_DCtoWC (this, mouseLoc. x, mouseLoc. y, xWC, yWC);
	#elif win
		POINT pos;
		if (! GetCursorPos (& pos)) { Melder_warning (L"Cannot find the location of the mouse."); return; }
		ScreenToClient (d_winWindow, & pos);
		Graphics_DCtoWC (this, pos. x, pos. y, xWC, yWC);
	#elif mac
		Point mouseLoc;
		GetMouse (& mouseLoc);
		Graphics_DCtoWC (this, mouseLoc. h, mouseLoc. v, xWC, yWC);
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
