/* praat_logo.c
 *
 * Copyright (C) 1996-2010 Paul Boersma
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
 * sdk 2008/03/24 GTK
 * pb 2009/06/02 date updated
 * pb 2010/01/10 date and authorship updated
 */

#include "praatP.h"
#include "Picture.h"

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
	Graphics_text (g, 0.5, 0.25, L"\\s{Built on the} %%Praat shell%\\s{,\\co Paul Boersma, 1992-2010");
}

static struct {
	double width_mm, height_mm;
	void (*draw) (Graphics g);
	Widget dia, form, drawingArea;
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
	if (theLogo.graphics == NULL)
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
#if gtk
	Graphics_x_setCR (theLogo.graphics, gdk_cairo_create (GDK_DRAWABLE (event -> widget -> window)));
	cairo_rectangle (Graphics_x_getCR (theLogo.graphics), (double) event->x, (double) event->y, (double) event->width, (double) event->height);
	cairo_clip (Graphics_x_getCR (theLogo.graphics));
	theLogo.draw (theLogo.graphics);
	cairo_destroy (Graphics_x_getCR (theLogo.graphics));
#elif motif
	(void) void_me;
	(void) event;
	if (theLogo.graphics == NULL)
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
	theLogo.draw (theLogo.graphics);
#endif
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
	#if gtk
		static const gchar *authors [3] = { "Paul Boersma", "David Weenink", NULL };

		Widget dialog = gtk_about_dialog_new ();
//		gtk_about_dialog_set_version (dialog, xstr(PRAAT_VERSION_STR));
//		gtk_about_dialog_set_copyright (dialog, "Copyright (C) 1992-" xstr(PRAAT_YEAR) " by Paul Boersma and David Weenink");
		gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "GPL");
		gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://www.praat.org");
		gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
		g_signal_connect (GTK_DIALOG (dialog), "response", G_CALLBACK (gtk_widget_destroy), NULL);

		gtk_dialog_run (GTK_DIALOG (dialog));

	#else
	#ifdef UNIX
		#if motif
			XWindowAttributes windowAttributes;
		#endif
	#endif
	if (theCurrentPraatApplication -> batch || ! theLogo.draw) return;
	if (! theLogo.dia) {
		theLogo.dia = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, Gui_AUTOMATIC, Gui_AUTOMATIC, L"About", gui_cb_goAway, NULL, 0);
		#if gtk
			theLogo.form = GTK_DIALOG (theLogo.dia) -> vbox;
		#else
			theLogo.form = theLogo.dia;
		#endif

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
	GuiDialog_show (theLogo.dia);
	
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
		XtAppNextEvent (theCurrentPraatApplication -> context, & event);
		XtDispatchEvent (& event);
	}
	gui_drawingarea_cb_expose (NULL, NULL);   // BUG
	#endif
	#endif

	#if motif
	if (autoPopDown)
		XtAppAddTimeOut (theCurrentPraatApplication -> context, 2000, logo_timeOut, (XtPointer) NULL);
	#endif
	#endif
}

/* End of file praat_logo.c */
