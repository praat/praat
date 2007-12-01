/* Graphics_mouse.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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

/*
 * pb 2000/09/21
 * pb 2002/03/07 GPL
 */

#include "GraphicsP.h"
#include "Gui.h"

#if xwin
	static int mouseDown = TRUE;
#endif

/*
 * Graphics_mouseStillDown () can only be used in a loop
 * if Graphics_getMouseLocation () is called in that same loop.
 * This is because the Xwin version requires that.
 */

int Graphics_mouseStillDown (I) {
	iam (Graphics);
	if (my screen) {
		#if xwin
			if (mouseDown) return TRUE;
			else { mouseDown = TRUE; return FALSE; }
		#elif win
			return motif_win_mouseStillDown ();
		#elif mac
			return StillDown ();
		#endif
	} else {
		return FALSE;
	}
}

void Graphics_getMouseLocation (I, double *xWC, double *yWC) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XEvent event;
			XButtonEvent *button;
			XMaskEvent (my display, ButtonReleaseMask | ButtonMotionMask | ExposureMask, & event);
			if (event. type == ButtonRelease) mouseDown = FALSE;
			button = (XButtonEvent *) & event;
			Graphics_DCtoWC (me, button -> x, button -> y, xWC, yWC);
		#elif win
			POINT pos;
			if (! GetCursorPos (& pos)) { Melder_warning1 (L"Cannot get cursor position."); return; }
			ScreenToClient (my window, & pos);
			Graphics_DCtoWC (me, pos. x, pos. y, xWC, yWC);
		#elif mac
			Point mouseLoc;
			GetMouse (& mouseLoc);
			Graphics_DCtoWC (me, mouseLoc. h, mouseLoc. v, xWC, yWC);
		#endif
	}
}

void Graphics_waitMouseUp (I) {
	iam (Graphics);
	while (Graphics_mouseStillDown (me)) {
		double xWC, yWC;
		Graphics_getMouseLocation (me, & xWC, & yWC);
	}
}

/* End of file Graphics_mouse.c */
