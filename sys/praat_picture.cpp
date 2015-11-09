/* praat_picture.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
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

#include "praatP.h"
#include "Picture.h"
#include "Printer.h"
#include "machine.h"
#include "Formula.h"

#include "GuiP.h"

static bool praat_mouseSelectsInnerViewport;

void praat_picture_prefs () {
	Preferences_addEnum (U"Picture.font", & theCurrentPraatPicture -> font, kGraphics_font, kGraphics_font_DEFAULT);
	Preferences_addInt (U"Picture.fontSize", & theCurrentPraatPicture -> fontSize, 10);
	Preferences_addBool (U"Picture.mouseSelectsInnerViewport", & praat_mouseSelectsInnerViewport, false);
}

/***** static variable *****/

static autoPicture praat_picture;

/********** CALLBACKS OF THE PICTURE MENUS **********/

/***** "Font" MENU: font part *****/

static GuiMenuItem praatButton_fonts [1 + kGraphics_font_MAX];

static void updateFontMenu () {
	if (! theCurrentPraatApplication -> batch) {
		if (theCurrentPraatPicture -> font < kGraphics_font_MIN) theCurrentPraatPicture -> font = kGraphics_font_MIN;
		if (theCurrentPraatPicture -> font > kGraphics_font_MAX) theCurrentPraatPicture -> font = kGraphics_font_MAX;   // we no longer have New Century Schoolbook
		for (int i = kGraphics_font_MIN; i <= kGraphics_font_MAX; i ++) {
			GuiMenuItem_check (praatButton_fonts [i], theCurrentPraatPicture -> font == i);
		}
	}
}
static void setFont (kGraphics_font font) {
	{// scope
		autoPraatPicture picture;
		Graphics_setFont (GRAPHICS, font);
	}
	theCurrentPraatPicture -> font = font;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updateFontMenu ();
	}
}
DIRECT (Times)     { setFont (kGraphics_font_TIMES);     } END
DIRECT (Helvetica) { setFont (kGraphics_font_HELVETICA); } END
DIRECT (Palatino)  { setFont (kGraphics_font_PALATINO);  } END
DIRECT (Courier)   { setFont (kGraphics_font_COURIER);   } END

/***** "Font" MENU: size part *****/

static GuiMenuItem praatButton_10, praatButton_12, praatButton_14, praatButton_18, praatButton_24;
static void updateSizeMenu () {
	if (! theCurrentPraatApplication -> batch) {
		GuiMenuItem_check (praatButton_10, theCurrentPraatPicture -> fontSize == 10);
		GuiMenuItem_check (praatButton_12, theCurrentPraatPicture -> fontSize == 12);
		GuiMenuItem_check (praatButton_14, theCurrentPraatPicture -> fontSize == 14);
		GuiMenuItem_check (praatButton_18, theCurrentPraatPicture -> fontSize == 18);
		GuiMenuItem_check (praatButton_24, theCurrentPraatPicture -> fontSize == 24);
	}
}
static void setFontSize (int fontSize) {
	//Melder_casual("Praat picture: set font size %d", (int) fontSize);
	{// scope
		autoPraatPicture picture;
		Graphics_setFontSize (GRAPHICS, fontSize);
	}
	theCurrentPraatPicture -> fontSize = fontSize;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updateSizeMenu ();
	}
}

DIRECT (10) { setFontSize (10); } END
DIRECT (12) { setFontSize (12); } END
DIRECT (14) { setFontSize (14); } END
DIRECT (18) { setFontSize (18); } END
DIRECT (24) { setFontSize (24); } END
FORM (Font_size, U"Praat picture: Font size", U"Font menu") {
	NATURAL (U"Font size (points)", U"10")
OK2
	SET_INTEGER (U"Font size", (long) theCurrentPraatPicture -> fontSize);
DO
	setFontSize (GET_INTEGER (U"Font size"));
END2 }

/*static void setFontSize_keepInnerViewport (int fontSize) {
	double xmargin = praat_size * 4.2 / 72.0, ymargin = praat_size * 2.8 / 72.0;
	if (xmargin > 0.4 * (x2NDC - x1NDC)) xmargin = 0.4 * (x2NDC - x1NDC);
	if (ymargin > 0.4 * (y2NDC - y1NDC)) ymargin = 0.4 * (y2NDC - y1NDC);
	x1NDC += xmargin;
	x2NDC -= xmargin;
	y1NDC += ymargin;
	y2NDC -= ymargin;
	xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
	if (xmargin > 2 * (x2NDC - x1NDC)) xmargin = 2 * (x2NDC - x1NDC);
	if (ymargin > 2 * (y2NDC - y1NDC)) ymargin = 2 * (y2NDC - y1NDC);
	x1NDC -= xmargin;
	x2NDC += xmargin;
	y1NDC -= ymargin;
	y2NDC += ymargin;
	{
		autoPraatPicture picture;
		Graphics_setFontSize (GRAPHICS, praat_size = fontSize);
	}
	Picture_setSelection (praat_picture, x1NDC, x2NDC, y1NDC, y2NDC, False);
	updateSizeMenu ();
}*/

/***** "Select" MENU *****/

static GuiMenuItem praatButton_innerViewport, praatButton_outerViewport;
static void updateViewportMenu () {
	if (! theCurrentPraatApplication -> batch) {
		GuiMenuItem_check (praatButton_innerViewport, praat_mouseSelectsInnerViewport ? 1 : 0);
		GuiMenuItem_check (praatButton_outerViewport, praat_mouseSelectsInnerViewport ? 0 : 1);
	}
}

DIRECT (MouseSelectsInnerViewport) {
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw (U"Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture.get(), praat_mouseSelectsInnerViewport = true);
	}
	updateViewportMenu ();
} END

DIRECT (MouseSelectsOuterViewport) {
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw (U"Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture.get(), praat_mouseSelectsInnerViewport = false);
	}
	updateViewportMenu ();
} END

FORM (SelectInnerViewport, U"Praat picture: Select inner viewport", U"Select inner viewport...") {
	LABEL (U"", U"The viewport is the selected rectangle in the Picture window.")
	LABEL (U"", U"It is where your next drawing will appear.")
	LABEL (U"", U"The rectangle you select here will not include the margins.")
	LABEL (U"", U"")
	REAL (U"left Horizontal range (inches)", U"0.0")
	REAL (U"right Horizontal range (inches)", U"6.0")
	REAL (U"left Vertical range (inches)", U"0.0")
	REAL (U"right Vertical range (inches)", U"6.0")
OK2
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC))
		ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
	if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC))
		xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
	SET_REAL (U"left Horizontal range", theCurrentPraatPicture -> x1NDC + xmargin)
	SET_REAL (U"right Horizontal range", theCurrentPraatPicture -> x2NDC - xmargin)
	SET_REAL (U"left Vertical range", 12 - theCurrentPraatPicture -> y2NDC + ymargin)
	SET_REAL (U"right Vertical range", 12 - theCurrentPraatPicture -> y1NDC - ymargin)
DO
	//if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Viewport commands are not available inside manuals.");
	double left = GET_REAL (U"left Horizontal range"), right = GET_REAL (U"right Horizontal range");
	double top = GET_REAL (U"left Vertical range"), bottom = GET_REAL (U"right Vertical range");
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	trace (U"1: xmargin ", xmargin, U" ymargin ", ymargin);
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		long x1DC, x2DC, y1DC, y2DC;
		Graphics_inqWsViewport (GRAPHICS, & x1DC, & x2DC, & y1DC, & y2DC);
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double wDC = (x2DC - x1DC) / (x2wNDC - x1wNDC);
		double hDC = labs (y2DC - y1DC) / (y2wNDC - y1wNDC);
		xmargin *= Graphics_getResolution (GRAPHICS) / wDC;
		ymargin *= Graphics_getResolution (GRAPHICS) / hDC;
	}
	if (xmargin > 2 * (right - left)) xmargin = 2 * (right - left);
	if (ymargin > 2 * (bottom - top)) ymargin = 2 * (bottom - top);
	trace (U"2: xmargin ", xmargin, U" ymargin ", ymargin);
	if (left == right) {
		Melder_throw (U"The left and right edges of the viewport cannot be equal.\nPlease change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_throw (U"The top and bottom edges of the viewport cannot be equal.\nPlease change the vertical range.");
	}
	theCurrentPraatPicture -> x1NDC = left - xmargin;
	theCurrentPraatPicture -> x2NDC = right + xmargin;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = 12-bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = 12-top + ymargin;
		Picture_setSelection (praat_picture.get(), theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC, false);
		Graphics_updateWs (GRAPHICS);
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects) {   // in manual?
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double height_NDC = y2wNDC - y1wNDC;
		theCurrentPraatPicture -> y1NDC = height_NDC-bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = height_NDC-top + ymargin;
	} else {
		if (top < bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = top + ymargin;
	}
	trace (U"3:"
		U" x1NDC ", theCurrentPraatPicture -> x1NDC,
		U" x2NDC ", theCurrentPraatPicture -> x2NDC,
		U" y1NDC ", theCurrentPraatPicture -> y1NDC,
		U" y2NDC ", theCurrentPraatPicture -> y2NDC
	);
END2 }

FORM (SelectOuterViewport, U"Praat picture: Select outer viewport", U"Select outer viewport...") {
	LABEL (U"", U"The viewport is the selected rectangle in the Picture window.")
	LABEL (U"", U"It is where your next drawing will appear.")
	LABEL (U"", U"The rectangle you select here will include the margins.")
	LABEL (U"", U"")
	REAL (U"left Horizontal range (inches)", U"0.0")
	REAL (U"right Horizontal range (inches)", U"6.0")
	REAL (U"left Vertical range (inches)", U"0.0")
	REAL (U"right Vertical range (inches)", U"6.0")
OK2
	SET_REAL (U"left Horizontal range", theCurrentPraatPicture -> x1NDC);
	SET_REAL (U"right Horizontal range", theCurrentPraatPicture -> x2NDC);
	SET_REAL (U"left Vertical range", 12 - theCurrentPraatPicture -> y2NDC);
	SET_REAL (U"right Vertical range", 12 - theCurrentPraatPicture -> y1NDC);
DO
	//if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Viewport commands are not available inside manuals.");
	double left = GET_REAL (U"left Horizontal range"), right = GET_REAL (U"right Horizontal range");
	double top = GET_REAL (U"left Vertical range"), bottom = GET_REAL (U"right Vertical range");
	if (left == right) {
		Melder_throw (U"The left and right edges of the viewport cannot be equal.\nPlease change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_throw (U"The top and bottom edges of the viewport cannot be equal.\nPlease change the vertical range.");
	}
	theCurrentPraatPicture -> x1NDC = left;
	theCurrentPraatPicture -> x2NDC = right;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = 12-bottom;
		theCurrentPraatPicture -> y2NDC = 12-top;
		Picture_setSelection (praat_picture.get(), theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC, false);
		Graphics_updateWs (GRAPHICS);   // BUG: needed on Cocoa, but why?
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects) {   // in manual?
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double height_NDC = y2wNDC - y1wNDC;
		theCurrentPraatPicture -> y1NDC = height_NDC-bottom;
		theCurrentPraatPicture -> y2NDC = height_NDC-top;
	} else {
		if (top < bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = bottom;
		theCurrentPraatPicture -> y2NDC = top;
	}
END2 }

FORM (ViewportText, U"Praat picture: Viewport text", U"Viewport text...") {
	RADIO (U"Horizontal alignment", 2)
		RADIOBUTTON (U"Left")
		RADIOBUTTON (U"Centre")
		RADIOBUTTON (U"Right")
	RADIO (U"Vertical alignment", 2)
		RADIOBUTTON (U"Bottom")
		RADIOBUTTON (U"Half")
		RADIOBUTTON (U"Top")
	REAL (U"Rotation (degrees)", U"0")
	TEXTFIELD (U"text", U"")
OK2
DO
	double x1WC, x2WC, y1WC, y2WC;
	int hor = GET_INTEGER (U"Horizontal alignment") - 1;
	int vert = GET_INTEGER (U"Vertical alignment") - 1;
	autoPraatPicture picture;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, 0, 1, 0, 1);
	Graphics_setTextAlignment (GRAPHICS, hor, vert);
	Graphics_setTextRotation (GRAPHICS, GET_REAL (U"Rotation"));
	Graphics_text (GRAPHICS, hor == 0 ? 0 : hor == 1 ? 0.5 : 1,
		vert == 0 ? 0 : vert == 1 ? 0.5 : 1, GET_STRING (U"text"));
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
END2 }

/***** "Pen" MENU *****/

static GuiMenuItem praatButton_lines [4];
static GuiMenuItem praatButton_black, praatButton_white, praatButton_red, praatButton_green, praatButton_blue,
	praatButton_yellow, praatButton_cyan, praatButton_magenta, praatButton_maroon, praatButton_lime, praatButton_navy,
	praatButton_teal, praatButton_purple, praatButton_olive, praatButton_pink, praatButton_silver, praatButton_grey;


static void updatePenMenu () {
	if (! theCurrentPraatApplication -> batch) {
		for (int i = Graphics_DRAWN; i <= Graphics_DASHED; i ++) {
			GuiMenuItem_check (praatButton_lines [i], theCurrentPraatPicture -> lineType == i);
		}
		GuiMenuItem_check (praatButton_black   , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_BLACK));
		GuiMenuItem_check (praatButton_white   , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_WHITE));
		GuiMenuItem_check (praatButton_red     , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_RED));
		GuiMenuItem_check (praatButton_green   , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_GREEN));
		GuiMenuItem_check (praatButton_blue    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_BLUE));
		GuiMenuItem_check (praatButton_yellow  , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_YELLOW));
		GuiMenuItem_check (praatButton_cyan    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_CYAN));
		GuiMenuItem_check (praatButton_magenta , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_MAGENTA));
		GuiMenuItem_check (praatButton_maroon  , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_MAROON));
		GuiMenuItem_check (praatButton_lime    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_LIME));
		GuiMenuItem_check (praatButton_navy    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_NAVY));
		GuiMenuItem_check (praatButton_teal    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_TEAL));
		GuiMenuItem_check (praatButton_purple  , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_PURPLE));
		GuiMenuItem_check (praatButton_olive   , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_OLIVE));
		GuiMenuItem_check (praatButton_pink    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_PINK));
		GuiMenuItem_check (praatButton_silver  , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_SILVER));
		GuiMenuItem_check (praatButton_grey    , Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_GREY));
	}
}
static void setLineType (int lineType) {
	{// scope
		autoPraatPicture picture;
		Graphics_setLineType (GRAPHICS, lineType);
	}
	theCurrentPraatPicture -> lineType = lineType;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
}
DIRECT (Solid_line)         { setLineType (Graphics_DRAWN);         } END
DIRECT (Dotted_line)        { setLineType (Graphics_DOTTED);        } END
DIRECT (Dashed_line)        { setLineType (Graphics_DASHED);        } END
DIRECT (Dashed_dotted_line) { setLineType (Graphics_DASHED_DOTTED); } END

FORM (Line_width, U"Praat picture: Line width", 0) {
	POSITIVE (U"Line width", U"1.0")
OK2
	SET_REAL (U"Line width", theCurrentPraatPicture -> lineWidth);
DO
	double lineWidth = GET_REAL (U"Line width");
	{// scope
		autoPraatPicture picture;
		Graphics_setLineWidth (GRAPHICS, lineWidth);
	}
	theCurrentPraatPicture -> lineWidth = lineWidth;
END2 }

FORM (Arrow_size, U"Praat picture: Arrow size", 0) {
	POSITIVE (U"Arrow size", U"1.0")
OK2
	SET_REAL (U"Arrow size", theCurrentPraatPicture -> arrowSize);
DO
	double arrowSize = GET_REAL (U"Arrow size");
	{// scope
		autoPraatPicture picture;
		Graphics_setArrowSize (GRAPHICS, arrowSize);
	}
	theCurrentPraatPicture -> arrowSize = arrowSize;
END2 }

FORM (Speckle_size, U"Praat picture: Speckle size", 0) {
	LABEL (U"", U"Here you determine the diameter (in millimetres)")
	LABEL (U"", U"of the dots that are drawn by \"speckle\" commands.")
	POSITIVE (U"Speckle size (mm)", U"1.0")
OK2
	SET_REAL (U"Speckle size", theCurrentPraatPicture -> speckleSize);
DO
	double speckleSize = GET_REAL (U"Speckle size");
	{// scope
		autoPraatPicture picture;
		Graphics_setSpeckleSize (GRAPHICS, speckleSize);
	}
	theCurrentPraatPicture -> speckleSize = speckleSize;
END2 }

static void setColour (Graphics_Colour colour) {
	{// scope
		autoPraatPicture picture;
		Graphics_setColour (GRAPHICS, colour);
	}
	theCurrentPraatPicture -> colour = colour;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
}
DIRECT (Black)   { setColour (Graphics_BLACK);   } END
DIRECT (White)   { setColour (Graphics_WHITE);   } END
DIRECT (Red)     { setColour (Graphics_RED);     } END
DIRECT (Green)   { setColour (Graphics_GREEN);   } END
DIRECT (Blue)    { setColour (Graphics_BLUE);    } END
DIRECT (Yellow)  { setColour (Graphics_YELLOW);  } END
DIRECT (Cyan)    { setColour (Graphics_CYAN);    } END
DIRECT (Magenta) { setColour (Graphics_MAGENTA); } END
DIRECT (Maroon)  { setColour (Graphics_MAROON);  } END
DIRECT (Lime)    { setColour (Graphics_LIME);    } END
DIRECT (Navy)    { setColour (Graphics_NAVY);    } END
DIRECT (Teal)    { setColour (Graphics_TEAL);    } END
DIRECT (Purple)  { setColour (Graphics_PURPLE);  } END
DIRECT (Olive)   { setColour (Graphics_OLIVE);   } END
DIRECT (Pink)    { setColour (Graphics_PINK);    } END
DIRECT (Silver)  { setColour (Graphics_SILVER);  } END
DIRECT (Grey)    { setColour (Graphics_GREY);    } END

FORM (Colour, U"Praat picture: Colour", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.0")
OK2
DO
	Graphics_Colour colour = GET_COLOUR (U"Colour");
	{// scope
		autoPraatPicture picture;
		Graphics_setColour (GRAPHICS, colour);
	}
	theCurrentPraatPicture -> colour = colour;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
END2 }

/***** "File" MENU *****/

FORM_READ2 (Picture_readFromPraatPictureFile, U"Read picture from praat picture file", 0, false) {
	Picture_readFromPraatPictureFile (praat_picture.get(), file);
END2 }

static void DO_Picture_writeToEpsFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as EPS file",
		DO_Picture_writeToEpsFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, true, false);
	}
}
/*FORM_WRITE (Picture_writeToEpsFile, U"Save picture as Encapsulated PostScript file", 0, U"praat.eps")
	if (! Picture_writeToEpsFile (praat_picture, fileName, true, false)) return 0;
END*/

static void DO_Picture_writeToFontlessEpsFile_xipa (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as fontless EPS file",
		DO_Picture_writeToFontlessEpsFile_xipa, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, false, false);
	}
}

static void DO_Picture_writeToFontlessEpsFile_silipa (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as fontless EPS file",
		DO_Picture_writeToFontlessEpsFile_silipa, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, false, true);
	}
}

static void DO_Picture_writeToPdfFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PDF file",
		DO_Picture_writeToPdfFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.pdf");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPdfFile (praat_picture.get(), file);
		} else {
			try {
				//autoPraatPicture picture;
				autoGraphics graphics = Graphics_create_pdffile (file, 300, NUMundefined, 10.24, NUMundefined, 7.68);
				Graphics_play (GRAPHICS, graphics.peek());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PDF file ", file, U".");
			}
		}
	}
}

static void DO_Picture_writeToPngFile_300 (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PNG file",
		DO_Picture_writeToPngFile_300, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.png");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPngFile_300 (praat_picture.get(), file);
		} else {
			try {
				autoGraphics graphics = Graphics_create_pngfile (file, 300, 0.0, 10.24, 0.0, 7.68);
				Graphics_play (GRAPHICS, graphics.peek());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PNG file ", file, U".");
			}
		}
	}
}

static void DO_Picture_writeToPngFile_600 (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PNG file",
		DO_Picture_writeToPngFile_600, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.png");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPngFile_600 (praat_picture.get(), file);
		} else {
			try {
				autoGraphics graphics = Graphics_create_pngfile (file, 600, 0.0, 10.24, 0.0, 7.68);
				Graphics_play (GRAPHICS, graphics.peek());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PNG file ", file, U".");
			}
		}
	}
}

static void DO_Picture_writeToPraatPictureFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
	static UiForm dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as Praat picture file",
		DO_Picture_writeToPraatPictureFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, U"praat.prapic");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToPraatPictureFile (praat_picture.get(), file);
	}
}

#ifdef macintosh
DIRECT (Page_setup) {
	Printer_pageSetup ();
} END
#endif

DIRECT (PostScript_settings) {
	Printer_postScriptSettings ();
} END

DIRECT (Print) {
	Picture_print (praat_picture.get());
} END

#ifdef _WIN32
	static void DO_Picture_writeToWindowsMetafile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *dummy) {
		static Any dia;
		(void) narg;
		(void) interpreter;
		(void) modified;
		(void) dummy;
		if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as Windows metafile",
			DO_Picture_writeToWindowsMetafile, NULL, invokingButtonTitle, NULL);
		if (sendingForm == NULL && args == NULL && sendingString == NULL) {
			UiOutfile_do (dia, U"praat.emf");
		} else { MelderFile file; structMelderFile file2 = { 0 };
			if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
			else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
			Picture_writeToWindowsMetafile (praat_picture, file);
		}
	}
#endif

#if defined (_WIN32) || defined (macintosh)
	DIRECT (Copy_picture_to_clipboard) {
		Picture_copyToClipboard (praat_picture.get());
	} END
#endif

/***** "Edit" MENU *****/

DIRECT (Undo) {
	Graphics_undoGroup (GRAPHICS);
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		Graphics_play (GRAPHICS, GRAPHICS);
	}
	Graphics_updateWs (GRAPHICS);
} END

DIRECT (Erase_all) {
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		Picture_erase (praat_picture.get());   // this kills the recording
	} else {
		Graphics_clearRecording (GRAPHICS);
		Graphics_clearWs (GRAPHICS);
	}
} END

/***** "World" MENU *****/

FORM (Text, U"Praat picture: Text", U"Text...") {
	REAL (U"Horizontal position", U"0.0")
	OPTIONMENU (U"Horizontal alignment", 2)
		OPTION (U"Left")
		OPTION (U"Centre")
		OPTION (U"Right")
	REAL (U"Vertical position", U"0.0")
	OPTIONMENU (U"Vertical alignment", 2)
		OPTION (U"Bottom")
		OPTION (U"Half")
		OPTION (U"Top")
	LABEL (U"", U"Text:")
	TEXTFIELD (U"text", U"")
OK2
DO
	autoPraatPicture picture;
	Graphics_setTextAlignment (GRAPHICS,
		GET_INTEGER (U"Horizontal alignment") - 1, GET_INTEGER (U"Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_text (GRAPHICS, GET_REAL (U"Horizontal position"),
		GET_REAL (U"Vertical position"), GET_STRING (U"text"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (Text_special, U"Praat picture: Text special", 0) {
	REAL (U"Horizontal position", U"0.0")
	OPTIONMENU (U"Horizontal alignment", 2)
		OPTION (U"left")
		OPTION (U"centre")
		OPTION (U"right")
	REAL (U"Vertical position", U"0.0")
	OPTIONMENU (U"Vertical alignment", 2)
		OPTION (U"bottom")
		OPTION (U"half")
		OPTION (U"top")
	OPTIONMENU_ENUM (U"Font", kGraphics_font, DEFAULT)
	NATURAL (U"Font size", U"10")
	SENTENCE (U"Rotation (degrees or dx;dy)", U"0")
	LABEL (U"", U"Text:")
	TEXTFIELD (U"text", U"")
OK2
DO
	kGraphics_font currentFont = Graphics_inqFont (GRAPHICS);
	int currentSize = Graphics_inqFontSize (GRAPHICS);
	autoPraatPicture picture;
	Graphics_setTextAlignment (GRAPHICS, GET_INTEGER (U"Horizontal alignment") - 1, GET_INTEGER (U"Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_setFont (GRAPHICS, GET_ENUM (kGraphics_font, U"Font"));
	Graphics_setFontSize (GRAPHICS, GET_INTEGER (U"Font size"));
	char32 *rotation = GET_STRING (U"Rotation"), *semicolon;
	if ((semicolon = str32chr (rotation, ';')) != NULL)
		Graphics_setTextRotation_vector (GRAPHICS, Melder_atof (rotation), Melder_atof (semicolon + 1));
	else
		Graphics_setTextRotation (GRAPHICS, Melder_atof (rotation));
	Graphics_text (GRAPHICS, GET_REAL (U"Horizontal position"), GET_REAL (U"Vertical position"), GET_STRING (U"text"));
	Graphics_setFont (GRAPHICS, currentFont);
	Graphics_setFontSize (GRAPHICS, currentSize);
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_unsetInner (GRAPHICS);
END2 }

static void dia_line (UiForm dia) {
	REAL (U"From x", U"0.0")
	REAL (U"From y", U"0.0")
	REAL (U"To x", U"1.0")
	REAL (U"To y", U"1.0")
}
FORM (DrawLine, U"Praat picture: Draw line", 0) {
	dia_line (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_line (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"From y"), GET_REAL (U"To x"),
		GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawArrow, U"Praat picture: Draw arrow", 0) {
	dia_line (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_arrow (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"From y"), GET_REAL (U"To x"),
		GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawDoubleArrow, U"Praat picture: Draw double arrow", 0) {
	dia_line (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_doubleArrow (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"From y"), GET_REAL (U"To x"),
		GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

Thing_define (PraatPictureFunction, Daata) {
	// new data:
	public:
		double xmin, xmax, dx, x1;
		long nx;
	// overridden methods:
		virtual bool v_hasGetXmin () { return true; }   virtual double v_getXmin ()        { return xmin; }
		virtual bool v_hasGetXmax () { return true; }   virtual double v_getXmax ()        { return xmax; }
		virtual bool v_hasGetNx   () { return true; }   virtual double v_getNx   ()        { return nx; }
		virtual bool v_hasGetDx   () { return true; }   virtual double v_getDx   ()        { return dx; }
		virtual bool v_hasGetX    () { return true; }   virtual double v_getX    (long ix) { return x1 + (ix - 1) * dx; }
};
Thing_implement (PraatPictureFunction, Daata, 0);

FORM (DrawFunction, U"Praat picture: Draw function", 0) {
	LABEL (U"", U"This command assumes that the x and y axes")
	LABEL (U"", U"have been set by a Draw command or by \"Axes...\".")
	REAL (U"From x", U"0.0")
	REAL (U"To x", U"0.0 (= all)")
	NATURAL (U"Number of horizontal steps", U"1000")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"x^2 - x^4")
OK2
DO
	double x1WC, x2WC, y1WC, y2WC;
	double fromX = GET_REAL (U"From x"), toX = GET_REAL (U"To x");
	long n = GET_INTEGER (U"Number of horizontal steps");
	char32 *formula = GET_STRING (U"formula");
	if (n < 2) return;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	if (fromX == toX) fromX = x1WC, toX = x2WC;
	autoNUMvector <double> y (1, n);
	autoPraatPictureFunction function = Thing_new (PraatPictureFunction);
	function -> xmin = x1WC;
	function -> xmax = x2WC;
	function -> nx = n;
	function -> x1 = fromX;
	function -> dx = (toX - fromX) / (n - 1);
	Formula_compile (interpreter, function.peek(), formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
	for (long i = 1; i <= n; i ++) {
		struct Formula_Result result;
		Formula_run (1, i, & result);
		y [i] = result. result.numericResult;
	}
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_function (GRAPHICS, y.peek(), 1, n, fromX, toX);
	Graphics_unsetInner (GRAPHICS);
END2 }

static void dia_rectangle (UiForm dia) {
	REAL (U"From x", U"0.0")
	REAL (U"To x", U"1.0")
	REAL (U"From y", U"0.0")
	REAL (U"To y", U"1.0")
}
FORM (DrawRectangle, U"Praat picture: Draw rectangle", 0) {
	dia_rectangle (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_rectangle (GRAPHICS,
		GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (PaintRectangle, U"Praat picture: Paint rectangle", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	dia_rectangle (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
	Graphics_fillRectangle (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawRoundedRectangle, U"Praat picture: Draw rounded rectangle", 0) {
	dia_rectangle (dia);
	POSITIVE (U"Radius (mm)", U"3.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_roundedRectangle (GRAPHICS,
		GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"), GET_REAL (U"Radius"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (PaintRoundedRectangle, U"Praat picture: Paint rounded rectangle", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	dia_rectangle (dia);
	POSITIVE (U"Radius (mm)", U"3.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
	Graphics_fillRoundedRectangle (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"), GET_REAL (U"Radius"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawArc, U"Praat picture: Draw arc", 0) {
	REAL (U"Centre x", U"0.0")
	REAL (U"Centre y", U"0.0")
	POSITIVE (U"Radius (along x)", U"1.0")
	REAL (U"From angle (degrees)", U"0.0")
	REAL (U"To angle (degrees)", U"90.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_arc (GRAPHICS, GET_REAL (U"Centre x"), GET_REAL (U"Centre y"), GET_REAL (U"Radius"),
		GET_REAL (U"From angle"), GET_REAL (U"To angle"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawEllipse, U"Praat picture: Draw ellipse", 0) {
	dia_rectangle (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_ellipse (GRAPHICS,
		GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (PaintEllipse, U"Praat picture: Paint ellipse", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	dia_rectangle (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
	Graphics_fillEllipse (GRAPHICS, GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawCircle, U"Praat picture: Draw circle", 0) {
	REAL (U"Centre x", U"0.0")
	REAL (U"Centre y", U"0.0")
	POSITIVE (U"Radius (along x)", U"1.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_circle (GRAPHICS, GET_REAL (U"Centre x"), GET_REAL (U"Centre y"), GET_REAL (U"Radius"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (PaintCircle, U"Praat picture: Paint circle", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (U"Centre x", U"0")
	REAL (U"Centre y", U"0")
	POSITIVE (U"Radius (along x)", U"1.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
	Graphics_fillCircle (GRAPHICS, GET_REAL (U"Centre x"), GET_REAL (U"Centre y"), GET_REAL (U"Radius"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (DrawCircle_mm, U"Praat picture: Draw circle (mm)", 0) {
	REAL (U"Centre x", U"0.0")
	REAL (U"Centre y", U"0.0")
	POSITIVE (U"Diameter (mm)", U"5.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_circle_mm (GRAPHICS, GET_REAL (U"Centre x"), GET_REAL (U"Centre y"), GET_REAL (U"Diameter"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (PaintCircle_mm, U"Praat picture: Paint circle (mm)", 0) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (U"Centre x", U"0.0")
	REAL (U"Centre y", U"0.0")
	POSITIVE (U"Diameter (mm)", U"5.0")
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
	Graphics_fillCircle_mm (GRAPHICS, GET_REAL (U"Centre x"), GET_REAL (U"Centre y"), GET_REAL (U"Diameter"));
	Graphics_unsetInner (GRAPHICS);
END2 }

FORM (InsertPictureFromFile, U"Praat picture: Insert picture from file", U"Insert picture from file...") {
	LABEL (U"", U"File name:")
	TEXTFIELD (U"fileName", U"~/Desktop/paul.jpg")
	dia_rectangle (dia);
OK2
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_imageFromFile (GRAPHICS, GET_STRING (U"fileName"), GET_REAL (U"From x"), GET_REAL (U"To x"), GET_REAL (U"From y"), GET_REAL (U"To y"));
	Graphics_unsetInner (GRAPHICS);
END2 }


FORM (Axes, U"Praat picture: Axes", U"Axes...") {
	REAL (U"left Left and right", U"0.0")
	REAL (U"right Left and right", U"1.0")
	REAL (U"left Bottom and top", U"0.0")
	REAL (U"right Bottom and top", U"1.0")
OK2
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	SET_REAL (U"left Left and right", x1WC);
	SET_REAL (U"right Left and right", x2WC);
	SET_REAL (U"left Bottom and top", y1WC);
	SET_REAL (U"right Bottom and top", y2WC);
DO
	double left = GET_REAL (U"left Left and right"), right = GET_REAL (U"right Left and right");
	double top = GET_REAL (U"right Bottom and top"), bottom = GET_REAL (U"left Bottom and top");
	REQUIRE (right != left, U"Left and right must not be equal.")
	REQUIRE (top != bottom, U"Top and bottom must not be equal.")
	autoPraatPicture picture;
	Graphics_setWindow (GRAPHICS, left, right, bottom, top);
END2 }

/***** "Margins" MENU *****/

DIRECT (DrawInnerBox) {
	autoPraatPicture picture;
	Graphics_drawInnerBox (GRAPHICS);
} END

FORM (Text_left, U"Praat picture: Text left", U"Text left/right/top/bottom...") {
	BOOLEAN (U"Far", 1)
	TEXTFIELD (U"text", U"")
OK2
DO
	autoPraatPicture picture;
	Graphics_textLeft (GRAPHICS, GET_INTEGER (U"Far"), GET_STRING (U"text"));
END2 }

FORM (Text_right, U"Praat picture: Text right", U"Text left/right/top/bottom...") {
	BOOLEAN (U"Far", 1)
	TEXTFIELD (U"text", U"")
OK2
DO
	autoPraatPicture picture;
	Graphics_textRight (GRAPHICS, GET_INTEGER (U"Far"), GET_STRING (U"text"));
END2 }

FORM (Text_top, U"Praat picture: Text top", U"Text left/right/top/bottom...") {
	BOOLEAN (U"Far", 0)
	TEXTFIELD (U"text", U"")
OK2
DO
	autoPraatPicture picture;
	Graphics_textTop (GRAPHICS, GET_INTEGER (U"Far"), GET_STRING (U"text"));
END2 }

FORM (Text_bottom, U"Praat picture: Text bottom", U"Text left/right/top/bottom...") {
	BOOLEAN (U"Far", 1)
	TEXTFIELD (U"text", U"")
OK2
DO
	autoPraatPicture picture;
	Graphics_textBottom (GRAPHICS, GET_INTEGER (U"Far"), GET_STRING (U"text"));
END2 }

static void dia_marksEvery (UiForm dia) {
	POSITIVE (U"Units", U"1.0")
	POSITIVE (U"Distance", U"0.1")
	BOOLEAN (U"Write numbers", 1)
	BOOLEAN (U"Draw ticks", 1)
	BOOLEAN (U"Draw dotted lines", 1)
}
static void do_marksEvery (UiForm dia, void (*Graphics_marksEvery) (Graphics, double, double, bool, bool, bool)) {
	autoPraatPicture picture;
	Graphics_marksEvery (GRAPHICS, GET_REAL (U"Units"), GET_REAL (U"Distance"),
		GET_INTEGER (U"Write numbers"),
		GET_INTEGER (U"Draw ticks"), GET_INTEGER (U"Draw dotted lines"));
}
FORM (Marks_left_every, U"Praat picture: Marks left every...", U"Marks left/right/top/bottom every...") {
	dia_marksEvery (dia); OK2 DO do_marksEvery (dia, Graphics_marksLeftEvery); END2 }
FORM (Marks_right_every, U"Praat picture: Marks right every...", U"Marks left/right/top/bottom every...") {
	dia_marksEvery (dia); OK2 DO do_marksEvery (dia, Graphics_marksRightEvery); END2 }
FORM (Marks_bottom_every, U"Praat picture: Marks bottom every...", U"Marks left/right/top/bottom every...") {
	dia_marksEvery (dia); OK2 DO do_marksEvery (dia, Graphics_marksBottomEvery); END2 }
FORM (Marks_top_every, U"Praat picture: Marks top every...", U"Marks left/right/top/bottom every...") {
	dia_marksEvery (dia); OK2 DO do_marksEvery (dia, Graphics_marksTopEvery); END2 }

static void dia_marks (UiForm dia) {
	NATURAL (U"Number of marks", U"6")
	BOOLEAN (U"Write numbers", 1)
	BOOLEAN (U"Draw ticks", 1)
	BOOLEAN (U"Draw dotted lines", 1)
}
static void do_marks (UiForm dia, void (*Graphics_marks) (Graphics, int, bool, bool, bool)) {
	long numberOfMarks = GET_INTEGER (U"Number of marks");
	REQUIRE (numberOfMarks >= 2, U"`Number of marks' must be at least 2.")
	autoPraatPicture picture;
	Graphics_marks (GRAPHICS, numberOfMarks, GET_INTEGER (U"Write numbers"),
		GET_INTEGER (U"Draw ticks"), GET_INTEGER (U"Draw dotted lines"));
}
FORM (Marks_left, U"Praat picture: Marks left", U"Marks left/right/top/bottom...") {
	dia_marks (dia); OK2 DO do_marks (dia, Graphics_marksLeft); END2 }
FORM (Marks_right, U"Praat picture: Marks right", U"Marks left/right/top/bottom...") {
	dia_marks (dia); OK2 DO do_marks (dia, Graphics_marksRight); END2 }
FORM (Marks_bottom, U"Praat picture: Marks bottom", U"Marks left/right/top/bottom...") {
	dia_marks (dia); OK2 DO do_marks (dia, Graphics_marksBottom); END2 }
FORM (Marks_top, U"Praat picture: Marks top", U"Marks left/right/top/bottom...") {
	dia_marks (dia); OK2 DO do_marks (dia, Graphics_marksTop); END2 }

static void dia_marksLogarithmic (UiForm dia) {
	NATURAL (U"Marks per decade", U"3")
	BOOLEAN (U"Write numbers", 1)
	BOOLEAN (U"Draw ticks", 1)
	BOOLEAN (U"Draw dotted lines", 1)
}
static void do_marksLogarithmic (UiForm dia, void (*Graphics_marksLogarithmic) (Graphics, int, bool, bool, bool)) {
	long numberOfMarksPerDecade = GET_INTEGER (U"Marks per decade");
	autoPraatPicture picture;
	Graphics_marksLogarithmic (GRAPHICS, numberOfMarksPerDecade, GET_INTEGER (U"Write numbers"),
		GET_INTEGER (U"Draw ticks"), GET_INTEGER (U"Draw dotted lines"));
}
FORM (marksLeftLogarithmic, U"Praat picture: Logarithmic marks left", U"Logarithmic marks left/right/top/bottom...") {
	dia_marksLogarithmic (dia); OK2 DO do_marksLogarithmic (dia, Graphics_marksLeftLogarithmic); END2 }
FORM (marksRightLogarithmic, U"Praat picture: Logarithmic marks right", U"Logarithmic marks left/right/top/bottom...") {
	dia_marksLogarithmic (dia); OK2 DO do_marksLogarithmic (dia, Graphics_marksRightLogarithmic); END2 }
FORM (marksBottomLogarithmic, U"Praat picture: Logarithmic marks bottom", U"Logarithmic marks left/right/top/bottom...") {
	dia_marksLogarithmic (dia); OK2 DO do_marksLogarithmic (dia, Graphics_marksBottomLogarithmic); END2 }
FORM (marksTopLogarithmic, U"Praat picture: Logarithmic marks top", U"Logarithmic marks left/right/top/bottom...") {
	dia_marksLogarithmic (dia); OK2 DO do_marksLogarithmic (dia, Graphics_marksTopLogarithmic); END2 }

static void sortBoundingBox (double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	double temp;
	if (*x1WC > *x2WC) temp = *x1WC, *x1WC = *x2WC, *x2WC = temp;
	if (*y1WC > *y2WC) temp = *y1WC, *y1WC = *y2WC, *y2WC = temp;
}

static void dia_oneMark (UiForm dia) {
	REAL (U"Position", U"0.0")
	BOOLEAN (U"Write number", 1)
	BOOLEAN (U"Draw tick", 1)
	BOOLEAN (U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD (U"text", U"")
}
FORM (Mark_left, U"Praat picture: One mark left", U"One mark left/right/top/bottom...") {
	dia_oneMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw (U"\"Position\" must be between ", y1WC, U" and ", y2WC, U".");
	autoPraatPicture picture;
	Graphics_markLeft (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (Mark_right, U"Praat picture: One mark right", U"One mark left/right/top/bottom...") {
	dia_oneMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw (U"\"Position\" must be between ", y1WC, U" and ", y2WC, U".");
	autoPraatPicture picture;
	Graphics_markRight (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (Mark_top, U"Praat picture: One mark top", U"One mark left/right/top/bottom...") {
	dia_oneMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;   // WHY?
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw (U"\"Position\" must be between ", x1WC, U" and ", x2WC, U".");
	autoPraatPicture picture;
	Graphics_markTop (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (Mark_bottom, U"Praat picture: One mark bottom", U"One mark left/right/top/bottom...") {
	dia_oneMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw (U"\"Position\" must be between ", x1WC, U" and ", x2WC, U".");
	autoPraatPicture picture;
	Graphics_markBottom (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

static void dia_oneLogarithmicMark (UiForm dia) {
	REAL (U"Position", U"1.0")
	BOOLEAN (U"Write number", 1)
	BOOLEAN (U"Draw tick", 1)
	BOOLEAN (U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD (U"text", U"")
}
FORM (LogarithmicMark_left, U"Praat picture: One logarithmic mark left", U"One logarithmic mark left/right/top/bottom...") {
	dia_oneLogarithmicMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw (U"\"Position\" must be between ", pow (10, y1WC), U" and ", pow (10, y2WC), U".");
	autoPraatPicture picture;
	Graphics_markLeftLogarithmic (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (LogarithmicMark_right, U"Praat picture: One logarithmic mark right", U"One logarithmic mark left/right/top/bottom...") {
	dia_oneLogarithmicMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw (U"\"Position\" must be between ", pow (10, y1WC), U" and ", pow (10, y2WC), U".");
	autoPraatPicture picture;
	Graphics_markRightLogarithmic (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (LogarithmicMark_top, U"Praat picture: One logarithmic mark top", U"One logarithmic mark left/right/top/bottom...") {
	dia_oneLogarithmicMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw (U"\"Position\" must be between ", pow (10, x1WC), U" and ", pow (10, x2WC), U".");
	autoPraatPicture picture;
	Graphics_markTopLogarithmic (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (LogarithmicMark_bottom, U"Praat picture: One logarithmic mark bottom", U"One logarithmic mark left/right/top/bottom...") {
	dia_oneLogarithmicMark (dia);
OK2
DO
	double position = GET_REAL (U"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw (U"\"Position\" must be between ", pow (10, x1WC), U" and ", pow (10, x2WC), U".");
	autoPraatPicture picture;
	Graphics_markBottomLogarithmic (GRAPHICS, position, GET_INTEGER (U"Write number"),
		GET_INTEGER (U"Draw tick"), GET_INTEGER (U"Draw dotted line"),
		GET_STRING (U"text"));
END2 }

FORM (dxMMtoWC, U"Compute horizontal distance in world coordinates", 0) {
	REAL (U"Distance (mm)", U"10.0")
OK2
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dxMMtoWC (GRAPHICS, GET_REAL (U"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END2 }

FORM (dxWCtoMM, U"Compute horizontal distance in millimetres", 0) {
	REAL (U"Distance (wc)", U"0.1")
OK2
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, GET_REAL (U"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END2 }

FORM (dyMMtoWC, U"Compute vertical distance in world coordinates", 0) {
	REAL (U"Distance (mm)", U"10.0")
OK2
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dyMMtoWC (GRAPHICS, GET_REAL (U"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END2 }

FORM (dyWCtoMM, U"Compute vertical distance in millimetres", 0) {
	REAL (U"Distance (wc)", U"1.0")
OK2
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dyWCtoMM (GRAPHICS, GET_REAL (U"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END2 }

FORM (textWidth_wc, U"Text width in world coordinates", 0) {
	TEXTFIELD (U"text", U"Hello world")
OK2
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth (GRAPHICS, GET_STRING (U"text"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END2 }

FORM (textWidth_mm, U"Text width in millimetres", 0) {
	TEXTFIELD (U"text", U"Hello world")
OK2
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, Graphics_textWidth (GRAPHICS, GET_STRING (U"text")));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END2 }

FORM (textWidth_ps_wc, U"PostScript text width in world coordinates", 0) {
	RADIO (U"Phonetic font", 1)
		RADIOBUTTON (U"XIPA")
		RADIOBUTTON (U"SILIPA")
	TEXTFIELD (U"text", U"Hello world")
OK2
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth_ps (GRAPHICS, GET_STRING (U"text"), GET_INTEGER (U"Phonetic font") - 1);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END2 }

FORM (textWidth_ps_mm, U"PostScript text width in millimetres", 0) {
	RADIO (U"Phonetic font", 1)
		RADIOBUTTON (U"XIPA")
		RADIOBUTTON (U"SILIPA")
	TEXTFIELD (U"text", U"Hello world")
OK2
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_textWidth_ps_mm (GRAPHICS, GET_STRING (U"text"), GET_INTEGER (U"Phonetic font") - 1);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END2 }

DIRECT (SearchManual) { Melder_search (); } END
DIRECT (PictureWindowHelp) { Melder_help (U"Picture window"); } END
DIRECT (AboutSpecialSymbols) { Melder_help (U"Special symbols"); } END
DIRECT (AboutTextStyles) { Melder_help (U"Text styles"); } END
DIRECT (PhoneticSymbols) { Melder_help (U"Phonetic symbols"); } END
DIRECT (Picture_settings_report) {
	MelderInfo_open ();
	const char32 *units = theCurrentPraatPicture == & theForegroundPraatPicture ? U" inches" : U"";
	MelderInfo_writeLine (U"Outer viewport left: ", theCurrentPraatPicture -> x1NDC, units);
	MelderInfo_writeLine (U"Outer viewport right: ", theCurrentPraatPicture -> x2NDC, units);
	MelderInfo_writeLine (U"Outer viewport top: ",
		theCurrentPraatPicture != & theForegroundPraatPicture ?
			theCurrentPraatPicture -> y1NDC :
			12 - theCurrentPraatPicture -> y2NDC, units);
	MelderInfo_writeLine (U"Outer viewport bottom: ",
		theCurrentPraatPicture != & theForegroundPraatPicture ?
			theCurrentPraatPicture -> y2NDC :
			12 - theCurrentPraatPicture -> y1NDC, units);
	MelderInfo_writeLine (U"Font size: ", theCurrentPraatPicture -> fontSize, U" points");
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		long x1DC, x2DC, y1DC, y2DC;
		Graphics_inqWsViewport (GRAPHICS, & x1DC, & x2DC, & y1DC, & y2DC);
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double wDC = (x2DC - x1DC) / (x2wNDC - x1wNDC);
		double hDC = abs (y2DC - y1DC) / (y2wNDC - y1wNDC);
		xmargin *= Graphics_getResolution (GRAPHICS) / wDC;
		ymargin *= Graphics_getResolution (GRAPHICS) / hDC;
	}
	if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC))
		ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
	if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC))
		xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
	MelderInfo_writeLine (U"Inner viewport left: ", theCurrentPraatPicture -> x1NDC + xmargin, units);
	MelderInfo_writeLine (U"Inner viewport right: ", theCurrentPraatPicture -> x2NDC - xmargin, units);
	MelderInfo_writeLine (U"Inner viewport top: ",
		theCurrentPraatPicture != & theForegroundPraatPicture ?
			theCurrentPraatPicture -> y1NDC + ymargin :
			12 - theCurrentPraatPicture -> y2NDC + ymargin, units);
	MelderInfo_writeLine (U"Inner viewport bottom: ",
		theCurrentPraatPicture != & theForegroundPraatPicture ?
			theCurrentPraatPicture -> y2NDC - ymargin :
			12 - theCurrentPraatPicture -> y1NDC - ymargin, units);
	MelderInfo_writeLine (U"Font: ", kGraphics_font_getText (theCurrentPraatPicture -> font));
	MelderInfo_writeLine (U"Line type: ",
		theCurrentPraatPicture -> lineType == Graphics_DRAWN ? U"Solid" :
		theCurrentPraatPicture -> lineType == Graphics_DOTTED ? U"Dotted" :
		theCurrentPraatPicture -> lineType == Graphics_DASHED ? U"Dashed" :
		theCurrentPraatPicture -> lineType == Graphics_DASHED_DOTTED ? U"Dashed-dotted" :
		U"(unknown)");
	MelderInfo_writeLine (U"Line width: ", theCurrentPraatPicture -> lineWidth);
	MelderInfo_writeLine (U"Arrow size: ", theCurrentPraatPicture -> arrowSize);
	MelderInfo_writeLine (U"Speckle size: ", theCurrentPraatPicture -> speckleSize);
	MelderInfo_writeLine (U"Colour: ", Graphics_Colour_name (theCurrentPraatPicture -> colour));
	MelderInfo_writeLine (U"Red: ", theCurrentPraatPicture -> colour. red);
	MelderInfo_writeLine (U"Green: ", theCurrentPraatPicture -> colour. green);
	MelderInfo_writeLine (U"Blue: ", theCurrentPraatPicture -> colour. blue);
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	MelderInfo_writeLine (U"Axis left: ", x1WC);
	MelderInfo_writeLine (U"Axis right: ", x2WC);
	MelderInfo_writeLine (U"Axis bottom: ", y1WC);
	MelderInfo_writeLine (U"Axis top: ", y2WC);
	MelderInfo_close ();
} END


/**********   **********/

static void cb_selectionChanged (Picture p, void * /* closure */,
	double selx1, double selx2, double sely1, double sely2)
	/* The user selected a new viewport in the picture window. */
{
	Melder_assert (p == praat_picture.get());
	theCurrentPraatPicture -> x1NDC = selx1;
	theCurrentPraatPicture -> x2NDC = selx2;
	theCurrentPraatPicture -> y1NDC = sely1;
	theCurrentPraatPicture -> y2NDC = sely2;
	if (praat_mouseSelectsInnerViewport) {
		int fontSize = Graphics_inqFontSize (GRAPHICS);
		double xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
		if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC)) ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
		if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC)) xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
		UiHistory_write (U"\nSelect inner viewport: ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x1NDC + xmargin));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x2NDC - xmargin));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y2NDC + ymargin));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y1NDC - ymargin));
	} else {
		UiHistory_write (U"\nSelect outer viewport: ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x1NDC));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x2NDC));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y2NDC));
		UiHistory_write (U", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y1NDC));
	}
}

/***** Public functions. *****/

static GuiWindow dialog;

static GuiMenu fileMenu, editMenu, marginsMenu, worldMenu, selectMenu, fontMenu, penMenu, helpMenu;

GuiMenu praat_picture_resolveMenu (const char32 *menu) {
	return
		str32equ (menu, U"File") ? fileMenu :
		str32equ (menu, U"Edit") ? editMenu :
		str32equ (menu, U"Margins") ? marginsMenu :
		str32equ (menu, U"World") ? worldMenu :
		str32equ (menu, U"Select") ? selectMenu :
		str32equ (menu, U"Font") ? fontMenu :
		str32equ (menu, U"Pen") ? penMenu :
		str32equ (menu, U"Help") ? helpMenu :
		editMenu;   // default
}

void praat_picture_exit () {
	praat_picture.reset();
}

void praat_picture_open () {
	Graphics_markGroup (GRAPHICS);   // we start a group of graphics output here
	if (theCurrentPraatPicture == & theForegroundPraatPicture && ! theCurrentPraatApplication -> batch) {
		#if gtk
			gtk_window_present (GTK_WINDOW (dialog -> d_gtkWindow));
		#elif cocoa
			GuiThing_show (dialog);
		#elif motif
			XtMapWidget (dialog -> d_xmShell);
			XMapRaised (XtDisplay (dialog -> d_xmShell), XtWindow (dialog -> d_xmShell));
		#endif
		Picture_unhighlight (praat_picture.get());
	}
	/* Foregoing drawing routines may have changed some of the output attributes */
	/* that can be set by the user. */
	/* Make sure that they have the right values before every drawing. */
	/* This is especially necessary after an 'erase picture': */
	/* the output attributes that were set by the user before the 'erase' */
	/* must be recorded before copying to a PostScript file. */
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setLineType (GRAPHICS, theCurrentPraatPicture -> lineType);
	Graphics_setLineWidth (GRAPHICS, theCurrentPraatPicture -> lineWidth);
	Graphics_setArrowSize (GRAPHICS, theCurrentPraatPicture -> arrowSize);
	Graphics_setSpeckleSize (GRAPHICS, theCurrentPraatPicture -> speckleSize);
	Graphics_setColour (GRAPHICS, theCurrentPraatPicture -> colour);

	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	/* The following will dump the axes to the PostScript file after Erase all. BUG: should be somewhere else. */
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
}

void praat_picture_close () {
	if (theCurrentPraatPicture != & theForegroundPraatPicture) return;
	if (! theCurrentPraatApplication -> batch) {
		Picture_highlight (praat_picture.get());
		#ifdef macintosh
			//dialog -> f_drain ();
		#endif
	}
}

Graphics praat_picture_editor_open (bool eraseFirst) {
	if (eraseFirst) Picture_erase (praat_picture.get());
	praat_picture_open ();
	return GRAPHICS;
}

void praat_picture_editor_close () {
	praat_picture_close ();
}

static Any pictureRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 2) return NULL;
	if (strnequ (header, "PraatPictureFile", 16))
	{
		Picture_readFromPraatPictureFile (praat_picture.get(), file); return (Any) 1;   // FIXME
	}
	return NULL;
}

void praat_picture_init () {
	GuiScrolledWindow scrollWindow;
	GuiDrawingArea drawingArea = NULL;
	int margin, width, height, resolution, x, y;
	theCurrentPraatPicture -> lineType = Graphics_DRAWN;
	theCurrentPraatPicture -> colour = Graphics_BLACK;
	theCurrentPraatPicture -> lineWidth = 1.0;
	theCurrentPraatPicture -> arrowSize = 1.0;
	theCurrentPraatPicture -> speckleSize = 1.0;
	theCurrentPraatPicture -> x1NDC = 0.0;
	theCurrentPraatPicture -> x2NDC = 6.0;
	theCurrentPraatPicture -> y1NDC = 8.0;
	theCurrentPraatPicture -> y2NDC = 12.0;

	Data_recognizeFileType (pictureRecognizer);

	if (! theCurrentPraatApplication -> batch) {
		double screenX, screenY, screenWidth, screenHeight;
		Gui_getWindowPositioningBounds (& screenX, & screenY, & screenWidth, & screenHeight);
		resolution = Gui_getResolution (nullptr);
		#if defined (macintosh)
			margin = 2, width = 6 * resolution + 20;
			height = 9 * resolution + Machine_getMenuBarHeight () + 24;
			x = screenX + screenWidth - width - 14;
			y = screenY + 0;
			width += margin * 2;
		#elif defined (_WIN32)
			margin = 2, width = 6 * resolution + 22;
			height = 9 * resolution + 24;
			x = screenX + screenWidth - width - 17;
			y = screenY + 0;
		#else
			margin = 0, width = 6 * resolution + 30;
			height = width * 3 / 2 + Machine_getTitleBarHeight ();
			x = screenX + screenWidth - width - 10;
			y = screenY + 0;
			width += margin * 2;
		#endif
		dialog = GuiWindow_create (x, y, width, height, 400, 200, Melder_cat (praatP.title, U" Picture"), nullptr, nullptr, 0);
		GuiWindow_addMenuBar (dialog);
	}
	if (! theCurrentPraatApplication -> batch) {
		fileMenu =    GuiMenu_createInWindow (dialog, U"File", 0);
		editMenu =    GuiMenu_createInWindow (dialog, U"Edit", 0);
		marginsMenu = GuiMenu_createInWindow (dialog, U"Margins", 0);
		worldMenu =   GuiMenu_createInWindow (dialog, U"World", 0);
		selectMenu =  GuiMenu_createInWindow (dialog, U"Select", 0);
		penMenu =     GuiMenu_createInWindow (dialog, U"Pen", 0);
		fontMenu =    GuiMenu_createInWindow (dialog, U"Font", 0);
		helpMenu =    GuiMenu_createInWindow (dialog, U"Help", 0);
	}

	praat_addMenuCommand (U"Picture", U"File", U"Picture info", 0, 0, DO_Picture_settings_report);
	praat_addMenuCommand (U"Picture", U"File", U"Picture settings report", 0, praat_HIDDEN, DO_Picture_settings_report);
	praat_addMenuCommand (U"Picture", U"File", U"-- save --", 0, 0, 0);
	#if defined (macintosh) || defined (UNIX)
		praat_addMenuCommand (U"Picture", U"File", U"Save as PDF file...", 0, 'S', DO_Picture_writeToPdfFile);
		praat_addMenuCommand (U"Picture", U"File", U"Write to PDF file...", 0, praat_HIDDEN, DO_Picture_writeToPdfFile);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Save as 300-dpi PNG file...", 0, 0, DO_Picture_writeToPngFile_300);
	#if defined (_WIN32)
		praat_addMenuCommand (U"Picture", U"File", U"Save as 600-dpi PNG file...", 0, 'S', DO_Picture_writeToPngFile_600);
	#endif
	#if defined (macintosh) || defined (UNIX)
		praat_addMenuCommand (U"Picture", U"File", U"Save as 600-dpi PNG file...", 0, 0, DO_Picture_writeToPngFile_600);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Save as EPS file", 0, 0, NULL);
		praat_addMenuCommand (U"Picture", U"File", U"PostScript settings...", 0, 1, DO_PostScript_settings);
		praat_addMenuCommand (U"Picture", U"File", U"Save as EPS file...", 0, 1, DO_Picture_writeToEpsFile);
		praat_addMenuCommand (U"Picture", U"File", U"Write to EPS file...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToEpsFile);
		praat_addMenuCommand (U"Picture", U"File", U"Save as fontless EPS file (XIPA)...", 0, 1, DO_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (U"Picture", U"File", U"Write to fontless EPS file (XIPA)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (U"Picture", U"File", U"Save as fontless EPS file (SILIPA)...", 0, 1, DO_Picture_writeToFontlessEpsFile_silipa);
		praat_addMenuCommand (U"Picture", U"File", U"Write to fontless EPS file (SILIPA)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToFontlessEpsFile_silipa);
	#ifdef _WIN32
		praat_addMenuCommand (U"Picture", U"File", U"Save as Windows metafile...", 0, 0, DO_Picture_writeToWindowsMetafile);
		praat_addMenuCommand (U"Picture", U"File", U"Write to Windows metafile...", 0, praat_HIDDEN, DO_Picture_writeToWindowsMetafile);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"-- praat picture file --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"File", U"Read from praat picture file...", 0, 0, DO_Picture_readFromPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File", U"Save as praat picture file...", 0, 0, DO_Picture_writeToPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File", U"Write to praat picture file...", 0, praat_HIDDEN, DO_Picture_writeToPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File", U"-- print --", 0, 0, 0);
	#if defined (macintosh)
		praat_addMenuCommand (U"Picture", U"File", U"Page setup...", 0, 0, DO_Page_setup);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Print...", 0, 'P', DO_Print);

	praat_addMenuCommand (U"Picture", U"Edit", U"Undo", 0, 'Z', DO_Undo);
	#if defined (macintosh) || defined (_WIN32)
		praat_addMenuCommand (U"Picture", U"Edit", U"-- clipboard --", 0, 0, 0);
		praat_addMenuCommand (U"Picture", U"Edit", U"Copy to clipboard", 0, 'C', DO_Copy_picture_to_clipboard);
	#endif
	praat_addMenuCommand (U"Picture", U"Edit", U"-- erase --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Edit", U"Erase all", 0, 'E', DO_Erase_all);

	praat_addMenuCommand (U"Picture", U"Margins", U"Draw inner box", 0, 0, DO_DrawInnerBox);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- text --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text left...", 0, 0, DO_Text_left);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text right...", 0, 0, DO_Text_right);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text top...", 0, 0, DO_Text_top);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text bottom...", 0, 0, DO_Text_bottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- marks every --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks left every...", 0, 0, DO_Marks_left_every);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks right every...", 0, 0, DO_Marks_right_every);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks bottom every...", 0, 0, DO_Marks_bottom_every);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks top every...", 0, 0, DO_Marks_top_every);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- one mark --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark left...", 0, 0, DO_Mark_left);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark right...", 0, 0, DO_Mark_right);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark bottom...", 0, 0, DO_Mark_bottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark top...", 0, 0, DO_Mark_top);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- marks --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks left...", 0, 1, DO_Marks_left);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks right...", 0, 1, DO_Marks_right);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks bottom...", 0, 1, DO_Marks_bottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks top...", 0, 1, DO_Marks_top);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks left...", 0, 1, DO_marksLeftLogarithmic);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks right...", 0, 1, DO_marksRightLogarithmic);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks bottom...", 0, 1, DO_marksBottomLogarithmic);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks top...", 0, 1, DO_marksTopLogarithmic);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark left...", 0, 1, DO_LogarithmicMark_left);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark right...", 0, 1, DO_LogarithmicMark_right);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark bottom...", 0, 1, DO_LogarithmicMark_bottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark top...", 0, 1, DO_LogarithmicMark_top);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- axes --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Margins", U"Axes...", 0, 0, DO_Axes);

	praat_addMenuCommand (U"Picture", U"World", U"Text...", 0, 0, DO_Text);
	praat_addMenuCommand (U"Picture", U"World", U"Text special...", 0, 0, DO_Text_special);
	praat_addMenuCommand (U"Picture", U"World", U"-- line --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Draw line...", 0, 0, DO_DrawLine);
	praat_addMenuCommand (U"Picture", U"World", U"Draw arrow...", 0, 0, DO_DrawArrow);
	praat_addMenuCommand (U"Picture", U"World", U"Draw two-way arrow...", 0, 0, DO_DrawDoubleArrow);
	praat_addMenuCommand (U"Picture", U"World", U"-- function --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Draw function...", 0, 0, DO_DrawFunction);
	praat_addMenuCommand (U"Picture", U"World", U"-- rectangle --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Draw rectangle...", 0, 0, DO_DrawRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint rectangle...", 0, 0, DO_PaintRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Draw rounded rectangle...", 0, 0, DO_DrawRoundedRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint rounded rectangle...", 0, 0, DO_PaintRoundedRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"-- arc --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Draw arc...", 0, 0, DO_DrawArc);
	praat_addMenuCommand (U"Picture", U"World", U"Draw ellipse...", 0, 0, DO_DrawEllipse);
	praat_addMenuCommand (U"Picture", U"World", U"Paint ellipse...", 0, 0, DO_PaintEllipse);
	praat_addMenuCommand (U"Picture", U"World", U"Draw circle...", 0, 0, DO_DrawCircle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint circle...", 0, 0, DO_PaintCircle);
	praat_addMenuCommand (U"Picture", U"World", U"Draw circle (mm)...", 0, 0, DO_DrawCircle_mm);
	praat_addMenuCommand (U"Picture", U"World", U"Paint circle (mm)...", 0, 0, DO_PaintCircle_mm);
	praat_addMenuCommand (U"Picture", U"World", U"-- picture --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Insert picture from file...", 0, 0, DO_InsertPictureFromFile);
	praat_addMenuCommand (U"Picture", U"World", U"-- axes --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Axes...", 0, 0, DO_Axes);
	praat_addMenuCommand (U"Picture", U"World", U"Measure", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Horizontal mm to wc...", 0, 1, DO_dxMMtoWC);
	praat_addMenuCommand (U"Picture", U"World", U"Horizontal wc to mm...", 0, 1, DO_dxWCtoMM);
	praat_addMenuCommand (U"Picture", U"World", U"Vertical mm to wc...", 0, 1, DO_dyMMtoWC);
	praat_addMenuCommand (U"Picture", U"World", U"Vertical wc to mm...", 0, 1, DO_dyWCtoMM);
	praat_addMenuCommand (U"Picture", U"World", U"-- text measure --", 0, 1, 0);
	praat_addMenuCommand (U"Picture", U"World", U"Text width (wc)...", 0, 1, DO_textWidth_wc);
	praat_addMenuCommand (U"Picture", U"World", U"Text width (mm)...", 0, 1, DO_textWidth_mm);
	praat_addMenuCommand (U"Picture", U"World", U"PostScript text width (wc)...", 0, 1, DO_textWidth_ps_wc);
	praat_addMenuCommand (U"Picture", U"World", U"PostScript text width (mm)...", 0, 1, DO_textWidth_ps_mm);

	praatButton_innerViewport = praat_addMenuCommand (U"Picture", U"Select", U"Mouse selects inner viewport", 0, praat_RADIO_FIRST, DO_MouseSelectsInnerViewport);
	praatButton_outerViewport = praat_addMenuCommand (U"Picture", U"Select", U"Mouse selects outer viewport", 0, praat_RADIO_NEXT, DO_MouseSelectsOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"-- select --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Select", U"Select inner viewport...", 0, 0, DO_SelectInnerViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"Select outer viewport...", 0, 0, DO_SelectOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"Viewport...", 0, praat_HIDDEN, DO_SelectOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"-- viewport drawing --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Select", U"Viewport text...", 0, 0, DO_ViewportText);

	praatButton_lines [Graphics_DRAWN] = praat_addMenuCommand (U"Picture", U"Pen", U"Solid line", 0, praat_RADIO_FIRST, DO_Solid_line);
	praat_addMenuCommand (U"Picture", U"Pen", U"Plain line", 0, praat_RADIO_NEXT | praat_HIDDEN, DO_Solid_line);
	praatButton_lines [Graphics_DOTTED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dotted line", 0, praat_RADIO_NEXT, DO_Dotted_line);
	praatButton_lines [Graphics_DASHED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dashed line", 0, praat_RADIO_NEXT, DO_Dashed_line);
	praatButton_lines [Graphics_DASHED_DOTTED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dashed-dotted line", 0, praat_RADIO_NEXT, DO_Dashed_dotted_line);
	praat_addMenuCommand (U"Picture", U"Pen", U"-- line width --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Pen", U"Line width...", 0, 0, DO_Line_width);
	praat_addMenuCommand (U"Picture", U"Pen", U"Arrow size...", 0, 0, DO_Arrow_size);
	praat_addMenuCommand (U"Picture", U"Pen", U"Speckle size...", 0, 0, DO_Speckle_size);
	praat_addMenuCommand (U"Picture", U"Pen", U"-- colour --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Pen", U"Colour...", 0, 0, DO_Colour);
	praatButton_black = praat_addMenuCommand (U"Picture", U"Pen", U"Black", 0, praat_CHECKBUTTON, DO_Black);
	praatButton_white = praat_addMenuCommand (U"Picture", U"Pen", U"White", 0, praat_CHECKBUTTON, DO_White);
	praatButton_red = praat_addMenuCommand (U"Picture", U"Pen", U"Red", 0, praat_CHECKBUTTON, DO_Red);
	praatButton_green = praat_addMenuCommand (U"Picture", U"Pen", U"Green", 0, praat_CHECKBUTTON, DO_Green);
	praatButton_blue = praat_addMenuCommand (U"Picture", U"Pen", U"Blue", 0, praat_CHECKBUTTON, DO_Blue);
	praatButton_yellow = praat_addMenuCommand (U"Picture", U"Pen", U"Yellow", 0, praat_CHECKBUTTON, DO_Yellow);
	praatButton_cyan = praat_addMenuCommand (U"Picture", U"Pen", U"Cyan", 0, praat_CHECKBUTTON, DO_Cyan);
	praatButton_magenta = praat_addMenuCommand (U"Picture", U"Pen", U"Magenta", 0, praat_CHECKBUTTON, DO_Magenta);
	praatButton_maroon = praat_addMenuCommand (U"Picture", U"Pen", U"Maroon", 0, praat_CHECKBUTTON, DO_Maroon);
	praatButton_lime = praat_addMenuCommand (U"Picture", U"Pen", U"Lime", 0, praat_CHECKBUTTON, DO_Lime);
	praatButton_navy = praat_addMenuCommand (U"Picture", U"Pen", U"Navy", 0, praat_CHECKBUTTON, DO_Navy);
	praatButton_teal = praat_addMenuCommand (U"Picture", U"Pen", U"Teal", 0, praat_CHECKBUTTON, DO_Teal);
	praatButton_purple = praat_addMenuCommand (U"Picture", U"Pen", U"Purple", 0, praat_CHECKBUTTON, DO_Purple);
	praatButton_olive = praat_addMenuCommand (U"Picture", U"Pen", U"Olive", 0, praat_CHECKBUTTON, DO_Olive);
	praatButton_pink = praat_addMenuCommand (U"Picture", U"Pen", U"Pink", 0, praat_CHECKBUTTON, DO_Pink);
	praatButton_silver = praat_addMenuCommand (U"Picture", U"Pen", U"Silver", 0, praat_CHECKBUTTON, DO_Silver);
	praatButton_grey = praat_addMenuCommand (U"Picture", U"Pen", U"Grey", 0, praat_CHECKBUTTON, DO_Grey);

	praat_addMenuCommand (U"Picture", U"Font", U"Font size...", 0, 0, DO_Font_size);
	praatButton_10 = praat_addMenuCommand (U"Picture", U"Font", U"10", 0, praat_CHECKBUTTON, DO_10);
	praatButton_12 = praat_addMenuCommand (U"Picture", U"Font", U"12", 0, praat_CHECKBUTTON, DO_12);
	praatButton_14 = praat_addMenuCommand (U"Picture", U"Font", U"14", 0, praat_CHECKBUTTON, DO_14);
	praatButton_18 = praat_addMenuCommand (U"Picture", U"Font", U"18", 0, praat_CHECKBUTTON, DO_18);
	praatButton_24 = praat_addMenuCommand (U"Picture", U"Font", U"24", 0, praat_CHECKBUTTON, DO_24);
	praat_addMenuCommand (U"Picture", U"Font", U"-- font ---", 0, 0, 0);
	praatButton_fonts [kGraphics_font_TIMES] = praat_addMenuCommand (U"Picture", U"Font", U"Times", 0, praat_RADIO_FIRST, DO_Times);
	praatButton_fonts [kGraphics_font_HELVETICA] = praat_addMenuCommand (U"Picture", U"Font", U"Helvetica", 0, praat_RADIO_NEXT, DO_Helvetica);
	praatButton_fonts [kGraphics_font_PALATINO] = praat_addMenuCommand (U"Picture", U"Font", U"Palatino", 0, praat_RADIO_NEXT, DO_Palatino);
	praatButton_fonts [kGraphics_font_COURIER] = praat_addMenuCommand (U"Picture", U"Font", U"Courier", 0, praat_RADIO_NEXT, DO_Courier);

	praat_addMenuCommand (U"Picture", U"Help", U"Picture window help", 0, '?', DO_PictureWindowHelp);
	praat_addMenuCommand (U"Picture", U"Help", U"About special symbols", 0, 0, DO_AboutSpecialSymbols);
	praat_addMenuCommand (U"Picture", U"Help", U"About text styles", 0, 0, DO_AboutTextStyles);
	praat_addMenuCommand (U"Picture", U"Help", U"Phonetic symbols", 0, 0, DO_PhoneticSymbols);
	praat_addMenuCommand (U"Picture", U"Help", U"-- manual --", 0, 0, 0);
	praat_addMenuCommand (U"Picture", U"Help",
		Melder_cat (U"Search ", praatP.title, U" manual..."),
		0, 'M', DO_SearchManual);

	if (! theCurrentPraatApplication -> batch) {
		width = height = resolution * 12;
		scrollWindow = GuiScrolledWindow_createShown (dialog, margin, 0, Machine_getMenuBarHeight () + margin, 0, 1, 1, 0);
		drawingArea = GuiDrawingArea_createShown (scrollWindow, width, height, nullptr, nullptr, nullptr, nullptr, nullptr, 0);
		GuiThing_show (dialog);
	}

	// TODO: Paul: deze moet VOOR de update functies anders krijgen die void_me 0x0
	praat_picture = Picture_create (drawingArea, ! theCurrentPraatApplication -> batch);	
	// READ THIS!
	Picture_setSelectionChangedCallback (praat_picture.get(), cb_selectionChanged, nullptr);
	theCurrentPraatPicture -> graphics = static_cast<Graphics> (Picture_getGraphics (praat_picture.get()));

	updatePenMenu ();
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
}

void praat_picture_prefsChanged () {
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
	Graphics_setFontSize (theCurrentPraatPicture -> graphics, theCurrentPraatPicture -> fontSize);   // so that the thickness of the selection rectangle is correct
	Picture_setMouseSelectsInnerViewport (praat_picture.get(), praat_mouseSelectsInnerViewport);
}

void praat_picture_background () {
	if (theCurrentPraatPicture != & theForegroundPraatPicture) return;   // Demo window and pictures ignore this
	if (! theCurrentPraatApplication -> batch) {
		//Picture_unhighlight (praat_picture.get());
		#if cocoa
			Picture_background (praat_picture.get());   // prevent Cocoa's very slow highlighting until woken up by Picture_foreground()
		#endif
	}
}

void praat_picture_foreground () {
	if (theCurrentPraatPicture != & theForegroundPraatPicture) return;   // Demo window and pictures ignore this
	if (! theCurrentPraatApplication -> batch) {
		#if cocoa
			Picture_foreground (praat_picture.get());   // wake up from the highlighting sleep caused by Picture_background()
		#endif
		//Picture_highlight (praat_picture.get());
	}
}

/* End of file praat_picture.cpp */
