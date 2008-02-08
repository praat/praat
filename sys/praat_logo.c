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
 * pb 2007/12/09 enums
 */

#include "praatP.h"

static void logo_defaultDraw (Graphics g) {
	Graphics_setColour (g, Graphics_MAGENTA);
	Graphics_fillRectangle (g, 0, 1, 0, 1);
	Graphics_setGrey (g, 0.5);
	Graphics_fillRectangle (g, 0.05, 0.95, 0.1, 0.9);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setColour (g, Graphics_YELLOW);
	Graphics_setFont (g, kGraphics_font_TIMES);
	Graphics_setFontSize (g, 24);
	Graphics_setFontStyle (g, Graphics_ITALIC);
	Graphics_setUnderscoreIsSubscript (g, FALSE);   /* Because program names may contain underscores. */
	Graphics_text (g, 0.5, 0.6, Melder_peekUtf8ToWcs (praatP.title));
	Graphics_setFontStyle (g, 0);
	Graphics_setFontSize (g, 12);
	Graphics_text (g, 0.5, 0.25, L"\\s{Built on the} %%Praat shell%\\s{,\\co Paul Boersma, 1992-2007");
}

static struct {
	double width_mm, height_mm;
	void (*draw) (Graphics g);
	Widget form, drawingArea;
	Graphics graphics;
} theLogo = { 90, 40, logo_defaultDraw };

#if motif
static void logo_timeOut (XtPointer closure, XtIntervalId *id) {
	(void) closure;
	(void) id;
 	GuiObject_hide (theLogo.form);
}
#endif

void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g)) {
	theLogo.width_mm = width_mm;
	theLogo.height_mm = height_mm;
	theLogo.draw = draw;
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	(void) void_me;
	(void) event;
	if (theLogo.graphics == NULL)
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
	theLogo.draw (theLogo.graphics);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	(void) void_me;
	(void) event;
 	GuiObject_hide (theLogo.form);
}

static void gui_cb_goAway (I) {
	(void) void_me;
 	GuiObject_hide (theLogo.form);
}

void praat_showLogo (int autoPopDown) {
	#ifdef UNIX
		#if motif
			XWindowAttributes windowAttributes;
		#endif
	#endif
	if (theCurrentPraat -> batch || ! theLogo.draw) return;
	if (! theLogo.form) {
		theLogo.form = GuiDialog_create (theCurrentPraat -> topShell, 100, 100, Gui_AUTOMATIC, Gui_AUTOMATIC, L"About", gui_cb_goAway, NULL, 0);
		#ifdef UNIX
			#if motif
				XtVaSetValues (XtParent (theLogo.form), XmNmwmDecorations, 4, XmNmwmFunctions, 0, NULL);
			#endif
		#endif
		theLogo.drawingArea = GuiDrawingArea_createShown (theLogo.form,
			0, (int) (theLogo.width_mm / 25.4 * Gui_getResolution (theLogo.drawingArea)),
			0, (int) (theLogo.height_mm / 25.4 * Gui_getResolution (theLogo.drawingArea)),
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, NULL, NULL, 0);
	}
	GuiObject_show (theLogo.form);
	/*
	 * Do not wait for the first expose event before drawing:
	 * at start-up time, this would take too long.
	 */
	#ifdef UNIX
	#if motif
	while (XGetWindowAttributes (XtDisplay (theLogo.form), XtWindow (theLogo.form), & windowAttributes),
	       windowAttributes. map_state != IsViewable)
	{
		XEvent event;
		XtAppNextEvent (theCurrentPraat -> context, & event);
		XtDispatchEvent (& event);
	}
	#endif
	gui_drawingarea_cb_expose (NULL, NULL);   // BUG
	#endif

	#if motif
	if (autoPopDown)
		XtAppAddTimeOut (theCurrentPraat -> context, 2000, logo_timeOut, (XtPointer) NULL);
	#endif
}

/* End of file praat_logo.c */
