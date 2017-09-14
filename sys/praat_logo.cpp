/* praat_logo.cpp
 *
 * Copyright (C) 1996-2012,2013,2014,2015,2016,2017 Paul Boersma, 2008 Stefan de Konink
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

#include "praatP.h"
#include "Picture.h"
#include "praat_version.h"

static void logo_defaultDraw (Graphics g) {
	Graphics_setColour (g, Graphics_MAGENTA);
	Graphics_fillRectangle (g, 0, 1, 0, 1);
	Graphics_setGrey (g, 0.5);
	Graphics_fillRectangle (g, 0.05, 0.95, 0.1, 0.9);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setColour (g, Graphics_YELLOW);
	Graphics_setFont (g, kGraphics_font::TIMES);
	Graphics_setFontSize (g, 24);
	Graphics_setFontStyle (g, Graphics_ITALIC);
	Graphics_setUnderscoreIsSubscript (g, false);   // because program names may contain underscores
	Graphics_text (g, 0.5, 0.6, praatP.title);
	Graphics_setFontStyle (g, 0);
	Graphics_setFontSize (g, 12);
	Graphics_text (g, 0.5, 0.25, U"\\s{Built on the} %%Praat shell%\\s{,© Paul Boersma, 1992-2017");
}

static struct {
	double width_mm, height_mm;
	void (*draw) (Graphics g);
	GuiDialog dia;
	GuiForm form;
	GuiDrawingArea drawingArea;
	autoGraphics graphics;
} theLogo = { 90, 40, logo_defaultDraw };

#if motif
static void logo_timeOut (XtPointer closure, XtIntervalId *id) {
	(void) closure;
	(void) id;
 	GuiThing_hide (theLogo.form);
}
#endif

void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g)) {
	theLogo.width_mm = width_mm;
	theLogo.height_mm = height_mm;
	theLogo.draw = draw;
}

static void gui_drawingarea_cb_expose (Thing /* me */, GuiDrawingArea_ExposeEvent event) {
	if (! theLogo.graphics)
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
	#if gtk
		#if ALLOW_GDK_DRAWING
			Graphics_x_setCR (theLogo.graphics.get(), gdk_cairo_create (GDK_DRAWABLE (GTK_WIDGET (event -> widget -> d_widget) -> window)));
		#else
			Graphics_x_setCR (theLogo.graphics.get(), gdk_cairo_create (gtk_widget_get_window (GTK_WIDGET (event -> widget -> d_widget))));
		#endif
		cairo_rectangle ((cairo_t *) Graphics_x_getCR (theLogo.graphics.get()), (double) event->x, (double) event->y, (double) event->width, (double) event->height);
		cairo_clip ((cairo_t *) Graphics_x_getCR (theLogo.graphics.get()));
		theLogo.draw (theLogo.graphics.get());
		cairo_destroy ((cairo_t *) Graphics_x_getCR (theLogo.graphics.get()));
	#elif motif || cocoa
		(void) event;
		if (! theLogo.graphics)
			theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
		theLogo.draw (theLogo.graphics.get());
	#endif
}

static void gui_drawingarea_cb_click (Thing /* me */, GuiDrawingArea_ClickEvent /* event */) {
 	GuiThing_hide (theLogo.form);
}

static void gui_cb_goAway (Thing /* boss */) {
 	GuiThing_hide (theLogo.form);
}

void praat_showLogo (bool autoPopDown) {
	#if gtk
		static const gchar *authors [3] = { "Paul Boersma", "David Weenink", nullptr };

		GuiObject dialog = gtk_about_dialog_new ();
		#define xstr(s) str(s)
		#define str(s) #s
		gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), xstr (PRAAT_VERSION_STR));
		gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), "Copyright © 1992–" xstr(PRAAT_YEAR) " by Paul Boersma and David Weenink");
		gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "GPL");
		gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://www.praat.org");
		//gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
		g_signal_connect (GTK_DIALOG (dialog), "response", G_CALLBACK (gtk_widget_destroy), nullptr);

		gtk_dialog_run (GTK_DIALOG (dialog));

	#elif motif || cocoa
		if (theCurrentPraatApplication -> batch || ! theLogo.draw) return;
		if (! theLogo.dia) {
			int width  = theLogo.width_mm  / 25.4 * Gui_getResolution (nullptr);
			int height = theLogo.height_mm / 25.4 * Gui_getResolution (nullptr);
			theLogo.dia = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width, height,
				U"About", gui_cb_goAway, nullptr, 0);
			theLogo.form = theLogo.dia;
			theLogo.drawingArea = GuiDrawingArea_createShown (theLogo.form, 0, width, 0, height,
				gui_drawingarea_cb_expose, gui_drawingarea_cb_click, nullptr, nullptr, nullptr, 0);
		}
		GuiThing_show (theLogo.form);
		GuiThing_show (theLogo.dia);
		#if motif
			if (autoPopDown)
				GuiAddTimeOut (2000, logo_timeOut, (XtPointer) nullptr);
		#endif
	#endif
}

/* End of file praat_logo.cpp */
