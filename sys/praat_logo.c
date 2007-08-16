/* praat_logo.c
 *
 * Copyright (C) 1996-2007 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2006/12/28 theCurrentPraat
 * pb 2007/06/10 wchar_t
 */

#include "praatP.h"

static void logo_defaultDraw (Graphics g) {
	Graphics_setColour (g, Graphics_MAGENTA);
	Graphics_fillRectangle (g, 0, 1, 0, 1);
	Graphics_setGrey (g, 0.5);
	Graphics_fillRectangle (g, 0.05, 0.95, 0.1, 0.9);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setColour (g, Graphics_YELLOW);
	Graphics_setFont (g, Graphics_TIMES);
	Graphics_setFontSize (g, 24);
	Graphics_setFontStyle (g, Graphics_ITALIC);
	Graphics_setUnderscoreIsSubscript (g, FALSE);   /* Because program names may contain underscores. */
	Graphics_text (g, 0.5, 0.6, Melder_peekAsciiToWcs (praatP.title));
	Graphics_setFontStyle (g, 0);
	Graphics_setFontSize (g, 12);
	Graphics_text (g, 0.5, 0.25, L"\\s{Built on the} %%Praat shell%\\s{,\\co Paul Boersma, 1992-2006");
}

static struct {
	double width_mm, height_mm;
	void (*draw) (Graphics g);
	Widget form, drawingArea;
	Graphics graphics;
} theLogo = { 90, 40, logo_defaultDraw };

static void logo_timeOut (XtPointer closure, XtIntervalId *id) {
	(void) closure;
	(void) id;
 	XtUnmanageChild (theLogo.form);
}

void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g)) {
	theLogo.width_mm = width_mm;
	theLogo.height_mm = height_mm;
	theLogo.draw = draw;
}

MOTIF_CALLBACK (logo_cb_expose)
	if (! theLogo.graphics)
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
	theLogo.draw (theLogo.graphics);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (logo_cb_input)
 	XtUnmanageChild (theLogo.form);
MOTIF_CALLBACK_END

void praat_showLogo (int autoPopDown) {
	#ifdef UNIX
	XWindowAttributes windowAttributes;
	#endif
	if (theCurrentPraat -> batch || ! theLogo.draw) return;
	if (! theLogo.form) {
		theLogo.form = XmCreateFormDialog (theCurrentPraat -> topShell, "logo", NULL, 0);
		XtVaSetValues (XtParent (theLogo.form), XmNx, 100, XmNy, 100,
			XmNtitle, "About",
			#ifdef UNIX
			XmNmwmDecorations, 4, XmNmwmFunctions, 0,
			#else
			XmNdeleteResponse, XmDO_NOTHING,
			#endif
			NULL);
		#if ! defined (sun4)
		{
			/* Catch Window Manager "Close" and "Quit". */
			Widget shell = XtParent (theLogo.form);
			Atom atom = XmInternAtom (XtDisplay (shell), "WM_DELETE_WINDOW", True);
			XmAddWMProtocols (shell, & atom, 1);
			XmAddWMProtocolCallback (shell, atom, logo_cb_input, NULL);
		}
		#endif
		theLogo.drawingArea = XmCreateDrawingArea (theLogo.form, "drawingArea", NULL, 0);
		XtVaSetValues (theLogo.drawingArea,
			XmNwidth, (int) (theLogo.width_mm / 25.4 * motif_getResolution (theLogo.drawingArea)),
			XmNheight, (int) (theLogo.height_mm / 25.4 * motif_getResolution (theLogo.drawingArea)), NULL);
		XtManageChild (theLogo.drawingArea);
		XtAddCallback (theLogo.drawingArea, XmNexposeCallback, logo_cb_expose, (XtPointer) NULL);
		XtAddCallback (theLogo.drawingArea, XmNinputCallback, logo_cb_input, (XtPointer) NULL);
	}
	XtManageChild (theLogo.form);
	/*
	 * Do not wait for the first expose event before drawing:
	 * at start-up time, this would take too long.
	 */
	#ifdef UNIX
	while (XGetWindowAttributes (XtDisplay (theLogo.form), XtWindow (theLogo.form), & windowAttributes),
	       windowAttributes. map_state != IsViewable)
	{
		XEvent event;
		XtAppNextEvent (theCurrentPraat -> context, & event);
		XtDispatchEvent (& event);
	}
	logo_cb_expose (0, 0, 0);
	#endif

	if (autoPopDown)
		XtAppAddTimeOut (theCurrentPraat -> context, 2000, logo_timeOut, (XtPointer) NULL);
}

/* End of file praat_logo.c */
