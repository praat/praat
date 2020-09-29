/* praat_logo.cpp
 *
 * Copyright (C) 1996-2020 Paul Boersma
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
	Graphics_setColour (g, Melder_MAGENTA);
	Graphics_fillRectangle (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (g, 0.5);
	Graphics_fillRectangle (g, 0.05, 0.95, 0.1, 0.9);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setColour (g, Melder_YELLOW);
	Graphics_setFont (g, kGraphics_font::TIMES);
	Graphics_setFontSize (g, 24);
	Graphics_setFontStyle (g, Graphics_ITALIC);
	Graphics_setUnderscoreIsSubscript (g, false);   // because program names may contain underscores
	Graphics_text (g, 0.5, 0.6, praatP.title.get());
	Graphics_setFontStyle (g, 0);
	Graphics_setFontSize (g, 12);
	Graphics_text (g, 0.5, 0.25, U"\\s{Built on the} %%Praat shell%\\s{,© Paul Boersma, 1992-2020");
}

static struct {
	double width_mm, height_mm;
	void (*draw) (Graphics g);
	GuiDialog dia;
	GuiForm form;
	GuiDrawingArea drawingArea;
	autoGraphics graphics;
} theLogo = { 90, 40, logo_defaultDraw };

void praat_setLogo (double width_mm, double height_mm, void (*draw) (Graphics g)) {
	theLogo.width_mm = width_mm;
	theLogo.height_mm = height_mm;
	theLogo.draw = draw;
}

static void gui_drawingarea_cb_expose (Thing /* me */, GuiDrawingArea_ExposeEvent event) {
	theLogo.draw (theLogo.graphics.get());
}

static void gui_drawingarea_cb_mouse (Thing /* me */, GuiDrawingArea_MouseEvent /* event */) {
 	GuiThing_hide (theLogo.form);
}

static void gui_cb_goAway (Thing /* boss */) {
 	GuiThing_hide (theLogo.form);
}

void praat_showLogo () {
	if (theCurrentPraatApplication -> batch || ! theLogo.draw)
		return;
	if (! theLogo.dia) {
		int width  = theLogo.width_mm  / 25.4 * Gui_getResolution (nullptr);
		int height = theLogo.height_mm / 25.4 * Gui_getResolution (nullptr);
		theLogo.dia = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width, height,
				U"About", gui_cb_goAway, nullptr, 0);
		theLogo.form = theLogo.dia;
		theLogo.drawingArea = GuiDrawingArea_createShown (theLogo.form, 0, width, 0, height,
				gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse, nullptr, nullptr, nullptr, 0);
		GuiThing_show (theLogo.form);
		GuiThing_show (theLogo.dia);
		theLogo.graphics = Graphics_create_xmdrawingarea (theLogo.drawingArea);
	} else {
		GuiThing_show (theLogo.form);
		GuiThing_show (theLogo.dia);
	}
}

/* End of file praat_logo.cpp */
