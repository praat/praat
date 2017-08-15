/* praat_picture.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016,2017 Paul Boersma
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
		if (theCurrentPraatPicture -> font > kGraphics_font_MAX) theCurrentPraatPicture -> font = kGraphics_font_MAX;
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
DIRECT (GRAPHICS_Times)     { setFont (kGraphics_font_TIMES);     END }
DIRECT (GRAPHICS_Helvetica) { setFont (kGraphics_font_HELVETICA); END }
DIRECT (GRAPHICS_Palatino)  { setFont (kGraphics_font_PALATINO);  END }
DIRECT (GRAPHICS_Courier)   { setFont (kGraphics_font_COURIER);   END }

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

DIRECT (GRAPHICS_10) { setFontSize (10); END }
DIRECT (GRAPHICS_12) { setFontSize (12); END }
DIRECT (GRAPHICS_14) { setFontSize (14); END }
DIRECT (GRAPHICS_18) { setFontSize (18); END }
DIRECT (GRAPHICS_24) { setFontSize (24); END }
FORM (GRAPHICS_Font_size, U"Praat picture: Font size", U"Font menu") {
	NATURAL4 (fontSize, U"Font size (points)", U"10")
OK
	SET_INTEGER (U"Font size", (long) theCurrentPraatPicture -> fontSize);
DO
	setFontSize (fontSize);
END }

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

DIRECT (GRAPHICS_MouseSelectsInnerViewport) {
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw (U"Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture.get(), praat_mouseSelectsInnerViewport = true);
	}
	updateViewportMenu ();
END }

DIRECT (GRAPHICS_MouseSelectsOuterViewport) {
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw (U"Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture.get(), praat_mouseSelectsInnerViewport = false);
	}
	updateViewportMenu ();
END }

FORM (GRAPHICS_SelectInnerViewport, U"Praat picture: Select inner viewport", U"Select inner viewport...") {
	LABEL (U"", U"The viewport is the selected rectangle in the Picture window.")
	LABEL (U"", U"It is where your next drawing will appear.")
	LABEL (U"", U"The rectangle you select here will not include the margins.")
	LABEL (U"", U"")
	REAL4 (left, U"left Horizontal range (inches)", U"0.0")
	REAL4 (right, U"right Horizontal range (inches)", U"6.0")
	REAL4 (top, U"left Vertical range (inches)", U"0.0")
	REAL4 (bottom, U"right Vertical range (inches)", U"6.0")
OK
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
END }

FORM (GRAPHICS_SelectOuterViewport, U"Praat picture: Select outer viewport", U"Select outer viewport...") {
	LABEL (U"", U"The viewport is the selected rectangle in the Picture window.")
	LABEL (U"", U"It is where your next drawing will appear.")
	LABEL (U"", U"The rectangle you select here will include the margins.")
	LABEL (U"", U"")
	REAL4 (left, U"left Horizontal range (inches)", U"0.0")
	REAL4 (right, U"right Horizontal range (inches)", U"6.0")
	REAL4 (top, U"left Vertical range (inches)", U"0.0")
	REAL4 (bottom, U"right Vertical range (inches)", U"6.0")
OK
	SET_REAL (U"left Horizontal range", theCurrentPraatPicture -> x1NDC);
	SET_REAL (U"right Horizontal range", theCurrentPraatPicture -> x2NDC);
	SET_REAL (U"left Vertical range", 12 - theCurrentPraatPicture -> y2NDC);
	SET_REAL (U"right Vertical range", 12 - theCurrentPraatPicture -> y1NDC);
DO
	//if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Viewport commands are not available inside manuals.");
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
END }

FORM (GRAPHICS_ViewportText, U"Praat picture: Viewport text", U"Viewport text...") {
	RADIO4x (horizontalAlignment, U"Horizontal alignment", 2, 0)
		RADIOBUTTON (U"Left")
		RADIOBUTTON (U"Centre")
		RADIOBUTTON (U"Right")
	RADIO4x (verticalAlignment, U"Vertical alignment", 2, 0)
		RADIOBUTTON (U"Bottom")
		RADIOBUTTON (U"Half")
		RADIOBUTTON (U"Top")
	REAL4 (rotation, U"Rotation (degrees)", U"0")
	TEXTFIELD4 (text, U"text", U"")
OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	autoPraatPicture picture;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, 0, 1, 0, 1);
	Graphics_setTextAlignment (GRAPHICS, horizontalAlignment, verticalAlignment);
	Graphics_setTextRotation (GRAPHICS, rotation);
	Graphics_text (GRAPHICS, horizontalAlignment == 0 ? 0.0 : horizontalAlignment == 1 ? 0.5 : 1.0,
		verticalAlignment == 0 ? 0.0 : verticalAlignment == 1 ? 0.5 : 1.0, text);
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
END }

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
DIRECT (GRAPHICS_Solid_line)         { setLineType (Graphics_DRAWN);         END }
DIRECT (GRAPHICS_Dotted_line)        { setLineType (Graphics_DOTTED);        END }
DIRECT (GRAPHICS_Dashed_line)        { setLineType (Graphics_DASHED);        END }
DIRECT (GRAPHICS_Dashed_dotted_line) { setLineType (Graphics_DASHED_DOTTED); END }

FORM (GRAPHICS_Line_width, U"Praat picture: Line width", nullptr) {
	POSITIVE4 (lineWidth, U"Line width", U"1.0")
OK
	SET_REAL (U"Line width", theCurrentPraatPicture -> lineWidth);
DO
	{// scope
		autoPraatPicture picture;
		Graphics_setLineWidth (GRAPHICS, lineWidth);
	}
	theCurrentPraatPicture -> lineWidth = lineWidth;
END }

FORM (GRAPHICS_Arrow_size, U"Praat picture: Arrow size", nullptr) {
	POSITIVE4 (arrowSize, U"Arrow size", U"1.0")
OK
	SET_REAL (U"Arrow size", theCurrentPraatPicture -> arrowSize);
DO
	{// scope
		autoPraatPicture picture;
		Graphics_setArrowSize (GRAPHICS, arrowSize);
	}
	theCurrentPraatPicture -> arrowSize = arrowSize;
END }

FORM (GRAPHICS_Speckle_size, U"Praat picture: Speckle size", nullptr) {
	LABEL (U"", U"Here you determine the diameter (in millimetres)")
	LABEL (U"", U"of the dots that are drawn by \"speckle\" commands.")
	POSITIVE4 (speckleSize, U"Speckle size (mm)", U"1.0")
OK
	SET_REAL (U"Speckle size", theCurrentPraatPicture -> speckleSize);
DO
	{// scope
		autoPraatPicture picture;
		Graphics_setSpeckleSize (GRAPHICS, speckleSize);
	}
	theCurrentPraatPicture -> speckleSize = speckleSize;
END }

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
DIRECT (GRAPHICS_Black)   { setColour (Graphics_BLACK);   END }
DIRECT (GRAPHICS_White)   { setColour (Graphics_WHITE);   END }
DIRECT (GRAPHICS_Red)     { setColour (Graphics_RED);     END }
DIRECT (GRAPHICS_Green)   { setColour (Graphics_GREEN);   END }
DIRECT (GRAPHICS_Blue)    { setColour (Graphics_BLUE);    END }
DIRECT (GRAPHICS_Yellow)  { setColour (Graphics_YELLOW);  END }
DIRECT (GRAPHICS_Cyan)    { setColour (Graphics_CYAN);    END }
DIRECT (GRAPHICS_Magenta) { setColour (Graphics_MAGENTA); END }
DIRECT (GRAPHICS_Maroon)  { setColour (Graphics_MAROON);  END }
DIRECT (GRAPHICS_Lime)    { setColour (Graphics_LIME);    END }
DIRECT (GRAPHICS_Navy)    { setColour (Graphics_NAVY);    END }
DIRECT (GRAPHICS_Teal)    { setColour (Graphics_TEAL);    END }
DIRECT (GRAPHICS_Purple)  { setColour (Graphics_PURPLE);  END }
DIRECT (GRAPHICS_Olive)   { setColour (Graphics_OLIVE);   END }
DIRECT (GRAPHICS_Pink)    { setColour (Graphics_PINK);    END }
DIRECT (GRAPHICS_Silver)  { setColour (Graphics_SILVER);  END }
DIRECT (GRAPHICS_Grey)    { setColour (Graphics_GREY);    END }

FORM (GRAPHICS_Colour, U"Praat picture: Colour", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.0")
OK
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
END }

/***** "File" MENU *****/

FORM_READ (GRAPHICS_Picture_readFromPraatPictureFile, U"Read picture from praat picture file", 0, false) {
	Picture_readFromPraatPictureFile (praat_picture.get(), file);
END }

static void GRAPHICS_Picture_writeToEpsFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as EPS file",
		GRAPHICS_Picture_writeToEpsFile, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, true, false);
	}
}
/*FORM_SAVE (GRAPHICS_Picture_writeToEpsFile, U"Save picture as Encapsulated PostScript file", 0, U"praat.eps")
	if (! Picture_writeToEpsFile (praat_picture, fileName, true, false)) return 0;
END*/

static void GRAPHICS_Picture_writeToFontlessEpsFile_xipa (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as fontless EPS file",
		GRAPHICS_Picture_writeToFontlessEpsFile_xipa, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, false, false);
	}
}

static void GRAPHICS_Picture_writeToFontlessEpsFile_silipa (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as fontless EPS file",
		GRAPHICS_Picture_writeToFontlessEpsFile_silipa, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.eps");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture.get(), file, false, true);
	}
}

static void GRAPHICS_Picture_writeToPdfFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PDF file",
		GRAPHICS_Picture_writeToPdfFile, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.pdf");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPdfFile (praat_picture.get(), file);
		} else {
			try {
				//autoPraatPicture picture;
				autoGraphics graphics = Graphics_create_pdffile (file, 300, undefined, 10.24, undefined, 7.68);
				Graphics_play (GRAPHICS, graphics.get());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PDF file ", file, U".");
			}
		}
	}
}

static void GRAPHICS_Picture_writeToPngFile_300 (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PNG file",
		GRAPHICS_Picture_writeToPngFile_300, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.png");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPngFile_300 (praat_picture.get(), file);
		} else {
			try {
				autoGraphics graphics = Graphics_create_pngfile (file, 300, 0.0, 10.24, 0.0, 7.68);
				Graphics_play (GRAPHICS, graphics.get());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PNG file ", file, U".");
			}
		}
	}
}

static void GRAPHICS_Picture_writeToPngFile_600 (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as PNG file",
		GRAPHICS_Picture_writeToPngFile_600, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.png");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		if (theCurrentPraatPicture == & theForegroundPraatPicture) {
			Picture_writeToPngFile_600 (praat_picture.get(), file);
		} else {
			try {
				autoGraphics graphics = Graphics_create_pngfile (file, 600, 0.0, 10.24, 0.0, 7.68);
				Graphics_play (GRAPHICS, graphics.get());
			} catch (MelderError) {
				Melder_throw (U"Picture not written to PNG file ", file, U".");
			}
		}
	}
}

static void GRAPHICS_Picture_writeToPraatPictureFile (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString,
	Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
{
	static UiForm dia;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as Praat picture file",
		GRAPHICS_Picture_writeToPraatPictureFile, nullptr, invokingButtonTitle, nullptr);
	if (narg < 0) UiForm_info (dia, narg); else if (! sendingForm && ! args && ! sendingString) {
		UiOutfile_do (dia, U"praat.prapic");
	} else { MelderFile file; structMelderFile file2 { };
		if (! args && ! sendingString) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToPraatPictureFile (praat_picture.get(), file);
	}
}

#ifdef macintosh
DIRECT (GRAPHICS_Page_setup) {
	Printer_pageSetup ();
END }
#endif

DIRECT (GRAPHICS_PostScript_settings) {
	Printer_postScriptSettings ();
END }

DIRECT (GRAPHICS_Print) {
	Picture_print (praat_picture.get());
END }

#ifdef _WIN32
	static void GRAPHICS_Picture_writeToWindowsMetafile (UiForm sendingForm, int /* narg */, Stackel args, const char32 *sendingString,
		Interpreter /* interpreter */, const char32 *invokingButtonTitle, bool /* modified */, void *)
	{
		static UiForm dia;
		if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, U"Save as Windows metafile",
			GRAPHICS_Picture_writeToWindowsMetafile, nullptr, invokingButtonTitle, nullptr);
		if (! sendingForm && ! args && ! sendingString) {
			UiOutfile_do (dia, U"praat.emf");
		} else { MelderFile file; structMelderFile file2 { };
			if (! args && ! sendingString) file = UiFile_getFile (dia);
			else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
			Picture_writeToWindowsMetafile (praat_picture.get(), file);
		}
	}
#endif

#if defined (_WIN32) || defined (macintosh)
	DIRECT (GRAPHICS_Copy_picture_to_clipboard) {
		Picture_copyToClipboard (praat_picture.get());
	END }
#endif

/***** "Edit" MENU *****/

DIRECT (GRAPHICS_Undo) {
	Graphics_undoGroup (GRAPHICS);
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		Graphics_play (GRAPHICS, GRAPHICS);
	}
	Graphics_updateWs (GRAPHICS);
END }

DIRECT (GRAPHICS_Erase_all) {
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		Picture_erase (praat_picture.get());   // this kills the recording
	} else {
		Graphics_clearRecording (GRAPHICS);
		Graphics_clearWs (GRAPHICS);
		#if 1
		autoPraatPicture picture;
		Graphics_Colour colour = GRAPHICS -> colour;
		Graphics_setColour (GRAPHICS, Graphics_WHITE);
		double x1, y1, x2, y2;
		//printf ("%ld %ld %ld %ld\n", GRAPHICS -> d_x1DC, GRAPHICS -> d_y1DC, GRAPHICS -> d_x2DC, GRAPHICS -> d_y2DC);
		Graphics_DCtoWC (GRAPHICS, GRAPHICS -> d_x1DC, GRAPHICS -> d_y1DC, & x1, & y1);
		Graphics_DCtoWC (GRAPHICS, GRAPHICS -> d_x2DC, GRAPHICS -> d_y2DC, & x2, & y2);
		//printf ("%f %f %f %f\n", x1, y1, x2, y2);
		Graphics_fillRectangle (GRAPHICS, x1, x2, y1, y2);
		Graphics_setColour (GRAPHICS, colour);
		#endif
	}
END }

/***** "World" MENU *****/

FORM (GRAPHICS_Text, U"Praat picture: Text", U"Text...") {
	REAL4 (horizontalPosition, U"Horizontal position", U"0.0")
	OPTIONMENU4x (horizontalAlignment, U"Horizontal alignment", 2, 0)
		OPTION (U"Left")
		OPTION (U"Centre")
		OPTION (U"Right")
	REAL4 (verticalPosition, U"Vertical position", U"0.0")
	OPTIONMENU4x (verticalAlignment, U"Vertical alignment", 2, 0)
		OPTION (U"Bottom")
		OPTION (U"Half")
		OPTION (U"Top")
	LABEL (U"", U"Text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	GRAPHICS_NONE
		Graphics_setTextAlignment (GRAPHICS, horizontalAlignment, verticalAlignment);
		Graphics_setInner (GRAPHICS);
		Graphics_text (GRAPHICS, horizontalPosition, verticalPosition, text);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_TextSpecial, U"Praat picture: Text special", nullptr) {
	REAL4 (horizontalPosition, U"Horizontal position", U"0.0")
	OPTIONMENU4x (horizontalAlignment, U"Horizontal alignment", 2, 0)
		OPTION (U"Left")
		OPTION (U"Centre")
		OPTION (U"Right")
	REAL4 (verticalPosition, U"Vertical position", U"0.0")
	OPTIONMENU4x (verticalAlignment, U"Vertical alignment", 2, 0)
		OPTION (U"Bottom")
		OPTION (U"Half")
		OPTION (U"Top")
	OPTIONMENU_ENUM4 (font, U"Font", kGraphics_font, DEFAULT)
	NATURAL4 (fontSize, U"Font size", U"10")
	SENTENCE4 (rotation, U"Rotation (degrees or dx;dy)", U"0")
	LABEL (U"", U"Text:")
	TEXTFIELD4 (text, U"text", U"")
OK
DO
	kGraphics_font currentFont = Graphics_inqFont (GRAPHICS);
	int currentSize = Graphics_inqFontSize (GRAPHICS);
	GRAPHICS_NONE
		Graphics_setTextAlignment (GRAPHICS, horizontalAlignment, verticalAlignment);
		Graphics_setInner (GRAPHICS);
		Graphics_setFont (GRAPHICS, (kGraphics_font) font);
		Graphics_setFontSize (GRAPHICS, fontSize);
		const char32 *semicolon;
		if (!! (semicolon = str32chr (rotation, ';')))
			Graphics_setTextRotation_vector (GRAPHICS, Melder_atof (rotation), Melder_atof (semicolon + 1));
		else
			Graphics_setTextRotation (GRAPHICS, Melder_atof (rotation));
		Graphics_text (GRAPHICS, horizontalPosition, verticalPosition, text);
		Graphics_setFont (GRAPHICS, currentFont);
		Graphics_setFontSize (GRAPHICS, currentSize);
		Graphics_setTextRotation (GRAPHICS, 0.0);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawLine, U"Praat picture: Draw line", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_line (GRAPHICS, fromX, fromY, toX, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawArrow, U"Praat picture: Draw arrow", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_arrow (GRAPHICS, fromX, fromY, toX, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawDoubleArrow, U"Praat picture: Draw double arrow", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_doubleArrow (GRAPHICS, fromX, fromY, toX, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

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

FORM (GRAPHICS_DrawFunction, U"Praat picture: Draw function", nullptr) {
	LABEL (U"", U"This command assumes that the x and y axes")
	LABEL (U"", U"have been set by a Draw command or by \"Axes...\".")
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"0.0 (= all)")
	NATURAL4 (numberOfHorizontalSteps, U"Number of horizontal steps", U"1000")
	LABEL (U"", U"Formula:")
	TEXTFIELD4 (formula, U"formula", U"x^2 - x^4")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	if (numberOfHorizontalSteps < 2) return;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	if (fromX == toX) fromX = x1WC, toX = x2WC;
	autoNUMvector <double> y (1, numberOfHorizontalSteps);
	autoPraatPictureFunction function = Thing_new (PraatPictureFunction);
	function -> xmin = x1WC;
	function -> xmax = x2WC;
	function -> nx = numberOfHorizontalSteps;
	function -> x1 = fromX;
	function -> dx = (toX - fromX) / (numberOfHorizontalSteps - 1);
	Formula_compile (interpreter, function.get(), formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
	for (long i = 1; i <= numberOfHorizontalSteps; i ++) {
		Formula_Result result;
		Formula_run (1, i, & result);
		y [i] = result. result.numericResult;
	}
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_function (GRAPHICS, y.peek(), 1, numberOfHorizontalSteps, fromX, toX);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawRectangle, U"Praat picture: Draw rectangle", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_rectangle (GRAPHICS, fromX, toX, fromY, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_PaintRectangle, U"Praat picture: Paint rectangle", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
		Graphics_fillRectangle (GRAPHICS, fromX, toX, fromY, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawRoundedRectangle, U"Praat picture: Draw rounded rectangle", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	POSITIVE4 (radius, U"Radius (mm)", U"3.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_roundedRectangle (GRAPHICS, fromX, toX, fromY, toY, radius);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_PaintRoundedRectangle, U"Praat picture: Paint rounded rectangle", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	POSITIVE4 (radius, U"Radius (mm)", U"3.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
		Graphics_fillRoundedRectangle (GRAPHICS, fromX, toX, fromY, toY, radius);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawArc, U"Praat picture: Draw arc", nullptr) {
	REAL4 (centreX, U"Centre x", U"0.0")
	REAL4 (centreY, U"Centre y", U"0.0")
	POSITIVE4 (radius, U"Radius (along x)", U"1.0")
	REAL4 (fromAngle, U"From angle (degrees)", U"0.0")
	REAL4 (toAngle, U"To angle (degrees)", U"90.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_arc (GRAPHICS, centreX, centreY, radius, fromAngle, toAngle);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawEllipse, U"Praat picture: Draw ellipse", nullptr) {
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_ellipse (GRAPHICS, fromX, toX, fromY, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_PaintEllipse, U"Praat picture: Paint ellipse", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
		Graphics_fillEllipse (GRAPHICS, fromX, toX, fromY, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawCircle, U"Praat picture: Draw circle", nullptr) {
	REAL4 (centreX, U"Centre x", U"0.0")
	REAL4 (centreY, U"Centre y", U"0.0")
	POSITIVE4 (radius, U"Radius (along x)", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_circle (GRAPHICS, centreX, centreY, radius);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_PaintCircle, U"Praat picture: Paint circle", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL4 (centreX, U"Centre x", U"0.0")
	REAL4 (centreY, U"Centre y", U"0.0")
	POSITIVE4 (radius, U"Radius (along x)", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
		Graphics_fillCircle (GRAPHICS, centreX, centreY, radius);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_DrawCircle_mm, U"Praat picture: Draw circle (mm)", nullptr) {
	REAL4 (centreX, U"Centre x", U"0.0")
	REAL4 (centreY, U"Centre y", U"0.0")
	POSITIVE4 (diameter, U"Diameter (mm)", U"5.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_circle_mm (GRAPHICS, centreX, centreY, diameter);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_PaintCircle_mm, U"Praat picture: Paint circle (mm)", nullptr) {
	COLOUR (U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL4 (centreX, U"Centre x", U"0.0")
	REAL4 (centreY, U"Centre y", U"0.0")
	POSITIVE4 (diameter, U"Diameter (mm)", U"5.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_setColour (GRAPHICS, GET_COLOUR (U"Colour"));
		Graphics_fillCircle_mm (GRAPHICS, centreX, centreY, diameter);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_InsertPictureFromFile, U"Praat picture: Insert picture from file", U"Insert picture from file...") {
	LABEL (U"", U"File name:")
	TEXTFIELD4 (fileName, U"fileName", U"~/Desktop/paul.jpg")
	REAL4 (fromX, U"From x", U"0.0")
	REAL4 (toX, U"To x", U"1.0")
	REAL4 (fromY, U"From y", U"0.0")
	REAL4 (toY, U"To y", U"1.0")
	OK
DO
	GRAPHICS_NONE
		Graphics_setInner (GRAPHICS);
		Graphics_imageFromFile (GRAPHICS, fileName, fromX, toX, fromY, toY);
		Graphics_unsetInner (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_Axes, U"Praat picture: Axes", U"Axes...") {
	REAL4 (left, U"left Left and right", U"0.0")
	REAL4 (right, U"right Left and right", U"1.0")
	REAL4 (bottom, U"left Bottom and top", U"0.0")
	REAL4 (top, U"right Bottom and top", U"1.0")
OK
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	SET_REAL (U"left Left and right", x1WC);
	SET_REAL (U"right Left and right", x2WC);
	SET_REAL (U"left Bottom and top", y1WC);
	SET_REAL (U"right Bottom and top", y2WC);
DO
	if (left == right) Melder_throw (U"Left and right should not be equal.");
	if (top == bottom) Melder_throw (U"Top and bottom should not be equal.");
	GRAPHICS_NONE
		Graphics_setWindow (GRAPHICS, left, right, bottom, top);
	GRAPHICS_NONE_END
}

// MARK: Margins

DIRECT (GRAPHICS_DrawInnerBox) {
	GRAPHICS_NONE
		Graphics_drawInnerBox (GRAPHICS);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_TextLeft, U"Praat picture: Text left", U"Text left/right/top/bottom...") {
	BOOLEAN4 (farr, U"Far", true)
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	GRAPHICS_NONE
		Graphics_textLeft (GRAPHICS, farr, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_TextRight, U"Praat picture: Text right", U"Text left/right/top/bottom...") {
	BOOLEAN4 (farr, U"Far", true)
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	GRAPHICS_NONE
		Graphics_textRight (GRAPHICS, farr, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_TextTop, U"Praat picture: Text top", U"Text left/right/top/bottom...") {
	BOOLEAN4 (farr, U"Far", true)
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	GRAPHICS_NONE
		Graphics_textTop (GRAPHICS, farr, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_TextBottom, U"Praat picture: Text bottom", U"Text left/right/top/bottom...") {
	BOOLEAN4 (farr, U"Far", true)
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	GRAPHICS_NONE
		Graphics_textBottom (GRAPHICS, farr, text);
	GRAPHICS_NONE_END
}

#define FIELDS_MARKS_EVERY  \
	POSITIVE4 (units, U"Units", U"1.0") \
	POSITIVE4 (distance, U"Distance", U"0.1") \
	BOOLEAN4 (writeNumbers, U"Write numbers", true) \
	BOOLEAN4 (drawTicks, U"Draw ticks", true) \
	BOOLEAN4 (drawDottedLines, U"Draw dotted lines", true)

FORM (GRAPHICS_MarksLeftEvery, U"Praat picture: Marks left every...", U"Marks left/right/top/bottom every...") {
	FIELDS_MARKS_EVERY
	OK
DO
	GRAPHICS_NONE
		Graphics_marksLeftEvery (GRAPHICS, units, distance, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksRightEvery, U"Praat picture: Marks right every...", U"Marks left/right/top/bottom every...") {
	FIELDS_MARKS_EVERY
	OK
DO
	GRAPHICS_NONE
		Graphics_marksRightEvery (GRAPHICS, units, distance, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksBottomEvery, U"Praat picture: Marks bottom every...", U"Marks left/right/top/bottom every...") {
	FIELDS_MARKS_EVERY
	OK
DO
	GRAPHICS_NONE
		Graphics_marksBottomEvery (GRAPHICS, units, distance, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksTopEvery, U"Praat picture: Marks top every...", U"Marks left/right/top/bottom every...") {
	FIELDS_MARKS_EVERY
	OK
DO
	GRAPHICS_NONE
		Graphics_marksTopEvery (GRAPHICS, units, distance, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

#define FIELDS_MARKS  \
	NATURAL4 (numberOfMarks, U"Number of marks", U"6") \
	BOOLEAN4 (writeNumbers, U"Write numbers", true) \
	BOOLEAN4 (drawTicks, U"Draw ticks", true) \
	BOOLEAN4 (drawDottedLines, U"Draw dotted lines", true)

FORM (GRAPHICS_MarksLeft, U"Praat picture: Marks left", U"Marks left/right/top/bottom...") {
	FIELDS_MARKS
	OK
DO
	GRAPHICS_NONE
		if (numberOfMarks < 2) Melder_throw (U"The number of marks should be at least 2.");
		Graphics_marksLeft (GRAPHICS, numberOfMarks, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksRight, U"Praat picture: Marks right", U"Marks left/right/top/bottom...") {
	FIELDS_MARKS
	OK
DO
	GRAPHICS_NONE
		if (numberOfMarks < 2) Melder_throw (U"The number of marks should be at least 2.");
		Graphics_marksRight (GRAPHICS, numberOfMarks, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksBottom, U"Praat picture: Marks bottom", U"Marks left/right/top/bottom...") {
	FIELDS_MARKS
	OK
DO
	GRAPHICS_NONE
		if (numberOfMarks < 2) Melder_throw (U"The number of marks should be at least 2.");
		Graphics_marksBottom (GRAPHICS, numberOfMarks, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_MarksTop, U"Praat picture: Marks top", U"Marks left/right/top/bottom...") {
	FIELDS_MARKS
	OK
DO
	GRAPHICS_NONE
		if (numberOfMarks < 2) Melder_throw (U"The number of marks should be at least 2.");
		Graphics_marksTop (GRAPHICS, numberOfMarks, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

#define FIELDS_MARKS_LOGARITHMIC  \
	NATURAL4 (marksPerDecade, U"Marks per decade", U"3") \
	BOOLEAN4 (writeNumbers, U"Write numbers", true) \
	BOOLEAN4 (drawTicks, U"Draw ticks", true) \
	BOOLEAN4 (drawDottedLines, U"Draw dotted lines", true)

FORM (GRAPHICS_LogarithmicMarksLeft, U"Praat picture: Logarithmic marks left", U"Logarithmic marks left/right/top/bottom...") {
	FIELDS_MARKS_LOGARITHMIC
	OK
DO
	GRAPHICS_NONE
		Graphics_marksLeftLogarithmic (GRAPHICS, marksPerDecade, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_LogarithmicMarksRight, U"Praat picture: Logarithmic marks right", U"Logarithmic marks left/right/top/bottom...") {
	FIELDS_MARKS_LOGARITHMIC
	OK
DO
	GRAPHICS_NONE
		Graphics_marksRightLogarithmic (GRAPHICS, marksPerDecade, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_LogarithmicMarksBottom, U"Praat picture: Logarithmic marks bottom", U"Logarithmic marks left/right/top/bottom...") {
	FIELDS_MARKS_LOGARITHMIC
	OK
DO
	GRAPHICS_NONE
		Graphics_marksBottomLogarithmic (GRAPHICS, marksPerDecade, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_LogarithmicMarksTop, U"Praat picture: Logarithmic marks top", U"Logarithmic marks left/right/top/bottom...") {
	FIELDS_MARKS_LOGARITHMIC
	OK
DO
	GRAPHICS_NONE
		Graphics_marksTopLogarithmic (GRAPHICS, marksPerDecade, writeNumbers, drawTicks, drawDottedLines);
	GRAPHICS_NONE_END
}

static void sortBoundingBox (double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	double temp;
	if (*x1WC > *x2WC) temp = *x1WC, *x1WC = *x2WC, *x2WC = temp;
	if (*y1WC > *y2WC) temp = *y1WC, *y1WC = *y2WC, *y2WC = temp;
}

FORM (GRAPHICS_OneMarkLeft, U"Praat picture: One mark left", U"One mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"0.0")
	BOOLEAN4 (writeNumber, U"Write number", true)
	BOOLEAN4 (drawTick, U"Draw tick", true)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", true)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw (U"\"Position\" must be between ", y1WC, U" and ", y2WC, U".");
	GRAPHICS_NONE
		Graphics_markLeft (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneMarkRight, U"Praat picture: One mark right", U"One mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"0.0")
	BOOLEAN4 (writeNumber, U"Write number", true)
	BOOLEAN4 (drawTick, U"Draw tick", true)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", true)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw (U"\"Position\" must be between ", y1WC, U" and ", y2WC, U".");
	GRAPHICS_NONE
		Graphics_markRight (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneMarkTop, U"Praat picture: One mark top", U"One mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"0.0")
	BOOLEAN4 (writeNumber, U"Write number", true)
	BOOLEAN4 (drawTick, U"Draw tick", true)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", true)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;   // WHY?
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw (U"\"Position\" must be between ", x1WC, U" and ", x2WC, U".");
	GRAPHICS_NONE
		Graphics_markTop (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneMarkBottom, U"Praat picture: One mark bottom", U"One mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"0.0")
	BOOLEAN4 (writeNumber, U"Write number", true)
	BOOLEAN4 (drawTick, U"Draw tick", true)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", true)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw (U"\"Position\" must be between ", x1WC, U" and ", x2WC, U".");
	GRAPHICS_NONE
		Graphics_markBottom (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneLogarithmicMarkLeft, U"Praat picture: One logarithmic mark left", U"One logarithmic mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"1.0")
	BOOLEAN4 (writeNumber, U"Write number", 1)
	BOOLEAN4 (drawTick, U"Draw tick", 1)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw (U"\"Position\" must be between ", pow (10, y1WC), U" and ", pow (10, y2WC), U".");
	GRAPHICS_NONE
		Graphics_markLeftLogarithmic (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneLogarithmicMarkRight, U"Praat picture: One logarithmic mark right", U"One logarithmic mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"1.0")
	BOOLEAN4 (writeNumber, U"Write number", 1)
	BOOLEAN4 (drawTick, U"Draw tick", 1)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dy;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw (U"\"Position\" must be between ", pow (10, y1WC), U" and ", pow (10, y2WC), U".");
	GRAPHICS_NONE
		Graphics_markRightLogarithmic (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneLogarithmicMarkTop, U"Praat picture: One logarithmic mark top", U"One logarithmic mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"1.0")
	BOOLEAN4 (writeNumber, U"Write number", 1)
	BOOLEAN4 (drawTick, U"Draw tick", 1)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw (U"\"Position\" must be between ", pow (10, x1WC), U" and ", pow (10, x2WC), U".");
	GRAPHICS_NONE
		Graphics_markTopLogarithmic (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_OneLogarithmicMarkBottom, U"Praat picture: One logarithmic mark bottom", U"One logarithmic mark left/right/top/bottom...") {
	REAL4 (position, U"Position", U"1.0")
	BOOLEAN4 (writeNumber, U"Write number", 1)
	BOOLEAN4 (drawTick, U"Draw tick", 1)
	BOOLEAN4 (drawDottedLine, U"Draw dotted line", 1)
	LABEL (U"", U"Draw text:")
	TEXTFIELD4 (text, U"text", U"")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC, dx;
	{// scope
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw (U"\"Position\" must be between ", pow (10, x1WC), U" and ", pow (10, x2WC), U".");
	GRAPHICS_NONE
		Graphics_markBottomLogarithmic (GRAPHICS, position, writeNumber, drawTick, drawDottedLine, text);
	GRAPHICS_NONE_END
}

FORM (GRAPHICS_HorizontalMmToWorldCoordinates, U"Compute horizontal distance in world coordinates", nullptr) {
	REAL4 (distance, U"Distance (mm)", U"10.0")
	OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dxMMtoWC (GRAPHICS, distance);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END }

FORM (GRAPHICS_HorizontalWorldCoordinatesToMm, U"Compute horizontal distance in millimetres", nullptr) {
	REAL4 (distance, U"Distance (wc)", U"0.1")
	OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, distance);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END }

FORM (GRAPHICS_VerticalMmToWorldCoordinates, U"Compute vertical distance in world coordinates", nullptr) {
	REAL4 (distance, U"Distance (mm)", U"10.0")
	OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dyMMtoWC (GRAPHICS, distance);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END }

FORM (GRAPHICS_VerticalWorldCoordinatesToMm, U"Compute vertical distance in millimetres", nullptr) {
	REAL4 (distance, U"Distance (wc)", U"1.0")
	OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dyWCtoMM (GRAPHICS, distance);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END }

FORM (GRAPHICS_TextWidth_worldCoordinates, U"Text width in world coordinates", nullptr) {
	TEXTFIELD4 (text, U"text", U"Hello world")
	OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth (GRAPHICS, text);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END }

FORM (GRAPHICS_TextWidth_mm, U"Text width in millimetres", nullptr) {
	TEXTFIELD4 (text, U"text", U"Hello world")
	OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, Graphics_textWidth (GRAPHICS, text));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END }

FORM (GRAPHICS_PostScriptTextWidth_worldCoordinates, U"PostScript text width in world coordinates", nullptr) {
	RADIO4x (phoneticFont, U"Phonetic font", 1, 0)
		RADIOBUTTON (U"XIPA")
		RADIOBUTTON (U"SILIPA")
	TEXTFIELD4 (text, U"text", U"Hello world")
	OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth_ps (GRAPHICS, text, phoneticFont);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, U"(world coordinates)");
END }

FORM (GRAPHICS_PostScriptTextWidth_mm, U"PostScript text width in millimetres", nullptr) {
	RADIO4x (phoneticFont, U"Phonetic font", 1, 0)
		RADIOBUTTON (U"XIPA")
		RADIOBUTTON (U"SILIPA")
	TEXTFIELD4 (text, U"text", U"Hello world")
	OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_textWidth_ps_mm (GRAPHICS, text, phoneticFont);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, U"mm");
END }

DIRECT (HELP_SearchManual_Picture) { Melder_search (); END }
DIRECT (HELP_PictureWindowHelp) { HELP (U"Picture window") }
DIRECT (HELP_AboutSpecialSymbols) { HELP (U"Special symbols") }
DIRECT (HELP_AboutTextStyles) { HELP (U"Text styles") }
DIRECT (HELP_PhoneticSymbols) { HELP (U"Phonetic symbols") }
DIRECT (GRAPHICS_Picture_settings_report) {
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
		double hDC = labs (y2DC - y1DC) / (y2wNDC - y1wNDC);
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
END }


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
		#elif motif
			XtMapWidget (dialog -> d_xmShell);
			XMapRaised (XtDisplay (dialog -> d_xmShell), XtWindow (dialog -> d_xmShell));
		#elif cocoa
			GuiThing_show (dialog);
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

static autoDaata pictureRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 2) return autoDaata ();
	if (strnequ (header, "PraatPictureFile", 16)) {
		Picture_readFromPraatPictureFile (praat_picture.get(), file);
		return Thing_new (Daata);   // a dummy
	}
	return autoDaata ();
}

void praat_picture_init () {
	GuiScrolledWindow scrollWindow;
	GuiDrawingArea drawingArea = nullptr;
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

	praat_addMenuCommand (U"Picture", U"File", U"Picture info", nullptr, 0, GRAPHICS_Picture_settings_report);
	praat_addMenuCommand (U"Picture", U"File",   U"Picture settings report", U"*Picture info", praat_DEPRECATED_2007, GRAPHICS_Picture_settings_report);
	praat_addMenuCommand (U"Picture", U"File", U"-- save --", nullptr, 0, nullptr);
	#if defined (macintosh) || defined (UNIX)
		praat_addMenuCommand (U"Picture", U"File", U"Save as PDF file...", nullptr, 'S', GRAPHICS_Picture_writeToPdfFile);
		praat_addMenuCommand (U"Picture", U"File",   U"Write to PDF file...", U"*Save as PDF file...", praat_DEPRECATED_2011, GRAPHICS_Picture_writeToPdfFile);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Save as 300-dpi PNG file...", nullptr, 0, GRAPHICS_Picture_writeToPngFile_300);
	#if defined (_WIN32)
		praat_addMenuCommand (U"Picture", U"File", U"Save as 600-dpi PNG file...", nullptr, 'S', GRAPHICS_Picture_writeToPngFile_600);
	#endif
	#if defined (macintosh) || defined (UNIX)
		praat_addMenuCommand (U"Picture", U"File", U"Save as 600-dpi PNG file...", nullptr, 0, GRAPHICS_Picture_writeToPngFile_600);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Save as EPS file", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Picture", U"File", U"PostScript settings...", nullptr, praat_DEPTH_1 | praat_NO_API, GRAPHICS_PostScript_settings);
		praat_addMenuCommand (U"Picture", U"File", U"Save as EPS file...", nullptr, 1, GRAPHICS_Picture_writeToEpsFile);
		praat_addMenuCommand (U"Picture", U"File",   U"Write to EPS file...", U"*Save as EPS file...", praat_DEPTH_1 | praat_DEPRECATED_2011, GRAPHICS_Picture_writeToEpsFile);
		praat_addMenuCommand (U"Picture", U"File", U"Save as fontless EPS file (XIPA)...", nullptr, 1, GRAPHICS_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (U"Picture", U"File",   U"Write to fontless EPS file (XIPA)...", U"*Save as fontless EPS file (XIPA)...", praat_DEPTH_1 | praat_DEPRECATED_2011, GRAPHICS_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (U"Picture", U"File", U"Save as fontless EPS file (SILIPA)...", nullptr, 1, GRAPHICS_Picture_writeToFontlessEpsFile_silipa);
		praat_addMenuCommand (U"Picture", U"File",   U"Write to fontless EPS file (SILIPA)...", U"*Save as fontless EPS file (SILIPA)...", praat_DEPTH_1 | praat_DEPRECATED_2011, GRAPHICS_Picture_writeToFontlessEpsFile_silipa);
	#ifdef _WIN32
		praat_addMenuCommand (U"Picture", U"File", U"Save as Windows metafile...", nullptr, 0, GRAPHICS_Picture_writeToWindowsMetafile);
		praat_addMenuCommand (U"Picture", U"File",   U"Write to Windows metafile...", U"*Save as Windows metafile...", praat_DEPRECATED_2011, GRAPHICS_Picture_writeToWindowsMetafile);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"-- praat picture file --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"File", U"Read from praat picture file...", nullptr, 0, GRAPHICS_Picture_readFromPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File", U"Save as praat picture file...", nullptr, 0, GRAPHICS_Picture_writeToPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File",   U"Write to praat picture file...", U"*Save as praat picture file...", praat_DEPRECATED_2011, GRAPHICS_Picture_writeToPraatPictureFile);
	praat_addMenuCommand (U"Picture", U"File", U"-- print --", nullptr, 0, nullptr);
	#if defined (macintosh)
		praat_addMenuCommand (U"Picture", U"File", U"Page setup...", nullptr, praat_NO_API, GRAPHICS_Page_setup);
	#endif
	praat_addMenuCommand (U"Picture", U"File", U"Print...", nullptr, 'P' | praat_NO_API, GRAPHICS_Print);

	praat_addMenuCommand (U"Picture", U"Edit", U"Undo", nullptr, 'Z' | praat_NO_API, GRAPHICS_Undo);
	#if defined (macintosh) || defined (_WIN32)
		praat_addMenuCommand (U"Picture", U"Edit", U"-- clipboard --", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Picture", U"Edit", U"Copy to clipboard", nullptr, 'C' | praat_NO_API, GRAPHICS_Copy_picture_to_clipboard);
	#endif
	praat_addMenuCommand (U"Picture", U"Edit", U"-- erase --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Edit", U"Erase all", nullptr, 'E', GRAPHICS_Erase_all);

	praat_addMenuCommand (U"Picture", U"Margins", U"Draw inner box", nullptr, 0, GRAPHICS_DrawInnerBox);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- text --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text left...", nullptr, 0, GRAPHICS_TextLeft);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text right...", nullptr, 0, GRAPHICS_TextRight);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text top...", nullptr, 0, GRAPHICS_TextTop);
	praat_addMenuCommand (U"Picture", U"Margins", U"Text bottom...", nullptr, 0, GRAPHICS_TextBottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- marks every --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks left every...", nullptr, 0, GRAPHICS_MarksLeftEvery);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks right every...", nullptr, 0, GRAPHICS_MarksRightEvery);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks bottom every...", nullptr, 0, GRAPHICS_MarksBottomEvery);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks top every...", nullptr, 0, GRAPHICS_MarksTopEvery);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- one mark --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark left...", nullptr, 0, GRAPHICS_OneMarkLeft);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark right...", nullptr, 0, GRAPHICS_OneMarkRight);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark bottom...", nullptr, 0, GRAPHICS_OneMarkBottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"One mark top...", nullptr, 0, GRAPHICS_OneMarkTop);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- marks --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks left...", nullptr, 1, GRAPHICS_MarksLeft);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks right...", nullptr, 1, GRAPHICS_MarksRight);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks bottom...", nullptr, 1, GRAPHICS_MarksBottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"Marks top...", nullptr, 1, GRAPHICS_MarksTop);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks left...", nullptr, 1, GRAPHICS_LogarithmicMarksLeft);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks right...", nullptr, 1, GRAPHICS_LogarithmicMarksRight);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks bottom...", nullptr, 1, GRAPHICS_LogarithmicMarksBottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"Logarithmic marks top...", nullptr, 1, GRAPHICS_LogarithmicMarksTop);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark left...", nullptr, 1, GRAPHICS_OneLogarithmicMarkLeft);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark right...", nullptr, 1, GRAPHICS_OneLogarithmicMarkRight);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark bottom...", nullptr, 1, GRAPHICS_OneLogarithmicMarkBottom);
	praat_addMenuCommand (U"Picture", U"Margins", U"One logarithmic mark top...", nullptr, 1, GRAPHICS_OneLogarithmicMarkTop);
	praat_addMenuCommand (U"Picture", U"Margins", U"-- axes --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Margins", U"Axes...", nullptr, 0, GRAPHICS_Axes);

	praat_addMenuCommand (U"Picture", U"World", U"Text...", nullptr, 0, GRAPHICS_Text);
	praat_addMenuCommand (U"Picture", U"World", U"Text special...", nullptr, 0, GRAPHICS_TextSpecial);
	praat_addMenuCommand (U"Picture", U"World", U"-- line --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Draw line...", nullptr, 0, GRAPHICS_DrawLine);
	praat_addMenuCommand (U"Picture", U"World", U"Draw arrow...", nullptr, 0, GRAPHICS_DrawArrow);
	praat_addMenuCommand (U"Picture", U"World", U"Draw two-way arrow...", nullptr, 0, GRAPHICS_DrawDoubleArrow);
	praat_addMenuCommand (U"Picture", U"World", U"-- function --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Draw function...", nullptr, 0, GRAPHICS_DrawFunction);
	praat_addMenuCommand (U"Picture", U"World", U"-- rectangle --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Draw rectangle...", nullptr, 0, GRAPHICS_DrawRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint rectangle...", nullptr, 0, GRAPHICS_PaintRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Draw rounded rectangle...", nullptr, 0, GRAPHICS_DrawRoundedRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint rounded rectangle...", nullptr, 0, GRAPHICS_PaintRoundedRectangle);
	praat_addMenuCommand (U"Picture", U"World", U"-- arc --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Draw arc...", nullptr, 0, GRAPHICS_DrawArc);
	praat_addMenuCommand (U"Picture", U"World", U"Draw ellipse...", nullptr, 0, GRAPHICS_DrawEllipse);
	praat_addMenuCommand (U"Picture", U"World", U"Paint ellipse...", nullptr, 0, GRAPHICS_PaintEllipse);
	praat_addMenuCommand (U"Picture", U"World", U"Draw circle...", nullptr, 0, GRAPHICS_DrawCircle);
	praat_addMenuCommand (U"Picture", U"World", U"Paint circle...", nullptr, 0, GRAPHICS_PaintCircle);
	praat_addMenuCommand (U"Picture", U"World", U"Draw circle (mm)...", nullptr, 0, GRAPHICS_DrawCircle_mm);
	praat_addMenuCommand (U"Picture", U"World", U"Paint circle (mm)...", nullptr, 0, GRAPHICS_PaintCircle_mm);
	praat_addMenuCommand (U"Picture", U"World", U"-- picture --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Insert picture from file...", nullptr, 0, GRAPHICS_InsertPictureFromFile);
	praat_addMenuCommand (U"Picture", U"World", U"-- axes --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Axes...", nullptr, 0, GRAPHICS_Axes);
	praat_addMenuCommand (U"Picture", U"World", U"Measure", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Horizontal mm to world coordinates...", nullptr, 1, GRAPHICS_HorizontalMmToWorldCoordinates);
	praat_addMenuCommand (U"Picture", U"World",   U"Horizontal mm to wc...", U"*Horizontal mm to world coordinates...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_HorizontalMmToWorldCoordinates);
	praat_addMenuCommand (U"Picture", U"World", U"Horizontal world coordinates to mm...", nullptr, 1, GRAPHICS_HorizontalWorldCoordinatesToMm);
	praat_addMenuCommand (U"Picture", U"World",   U"Horizontal wc to mm...", U"*Horizontal world coordinates to mm...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_HorizontalWorldCoordinatesToMm);
	praat_addMenuCommand (U"Picture", U"World", U"Vertical mm to world coordinates...", nullptr, 1, GRAPHICS_VerticalMmToWorldCoordinates);
	praat_addMenuCommand (U"Picture", U"World",   U"Vertical mm to wc...", U"*Vertical mm to world coordinates...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_VerticalMmToWorldCoordinates);
	praat_addMenuCommand (U"Picture", U"World", U"Vertical world coordinates to mm...", nullptr, 1, GRAPHICS_VerticalWorldCoordinatesToMm);
	praat_addMenuCommand (U"Picture", U"World",   U"Vertical wc to mm...", U"*Vertical world coordinates to mm...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_VerticalWorldCoordinatesToMm);
	praat_addMenuCommand (U"Picture", U"World", U"-- text measure --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Picture", U"World", U"Text width (world coordinates)...", nullptr, 1, GRAPHICS_TextWidth_worldCoordinates);
	praat_addMenuCommand (U"Picture", U"World",   U"Text width (wc)...", U"*Text width (world coordinates)...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_TextWidth_worldCoordinates);
	praat_addMenuCommand (U"Picture", U"World", U"Text width (mm)...", nullptr, 1, GRAPHICS_TextWidth_mm);
	praat_addMenuCommand (U"Picture", U"World", U"PostScript text width (world coordinates)...", nullptr, 1, GRAPHICS_PostScriptTextWidth_worldCoordinates);
	praat_addMenuCommand (U"Picture", U"World",   U"PostScript text width (wc)...", U"*PostScript text width (world coordinates)...", praat_DEPTH_1 | praat_DEPRECATED_2016, GRAPHICS_PostScriptTextWidth_worldCoordinates);
	praat_addMenuCommand (U"Picture", U"World", U"PostScript text width (mm)...", nullptr, 1, GRAPHICS_PostScriptTextWidth_mm);

	praatButton_innerViewport = praat_addMenuCommand (U"Picture", U"Select", U"Mouse selects inner viewport", nullptr, praat_RADIO_FIRST | praat_NO_API, GRAPHICS_MouseSelectsInnerViewport);
	praatButton_outerViewport = praat_addMenuCommand (U"Picture", U"Select", U"Mouse selects outer viewport", nullptr, praat_RADIO_NEXT | praat_NO_API, GRAPHICS_MouseSelectsOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"-- select --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Select", U"Select inner viewport...", nullptr, 0, GRAPHICS_SelectInnerViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"Select outer viewport...", nullptr, 0, GRAPHICS_SelectOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"Viewport...", U"*Select outer viewport...", praat_DEPRECATED_2004, GRAPHICS_SelectOuterViewport);
	praat_addMenuCommand (U"Picture", U"Select", U"-- viewport drawing --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Select", U"Viewport text...", nullptr, 0, GRAPHICS_ViewportText);

	praatButton_lines [Graphics_DRAWN] = praat_addMenuCommand (U"Picture", U"Pen", U"Solid line", nullptr, praat_RADIO_FIRST, GRAPHICS_Solid_line);
	praat_addMenuCommand (U"Picture", U"Pen", U"Plain line", nullptr, praat_RADIO_NEXT | praat_DEPRECATED_2006, GRAPHICS_Solid_line);
	praatButton_lines [Graphics_DOTTED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dotted line", nullptr, praat_RADIO_NEXT, GRAPHICS_Dotted_line);
	praatButton_lines [Graphics_DASHED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dashed line", nullptr, praat_RADIO_NEXT, GRAPHICS_Dashed_line);
	praatButton_lines [Graphics_DASHED_DOTTED] = praat_addMenuCommand (U"Picture", U"Pen", U"Dashed-dotted line", nullptr, praat_RADIO_NEXT, GRAPHICS_Dashed_dotted_line);
	praat_addMenuCommand (U"Picture", U"Pen", U"-- line width --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Pen", U"Line width...", nullptr, 0, GRAPHICS_Line_width);
	praat_addMenuCommand (U"Picture", U"Pen", U"Arrow size...", nullptr, 0, GRAPHICS_Arrow_size);
	praat_addMenuCommand (U"Picture", U"Pen", U"Speckle size...", nullptr, 0, GRAPHICS_Speckle_size);
	praat_addMenuCommand (U"Picture", U"Pen", U"-- colour --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Pen", U"Colour...", nullptr, 0, GRAPHICS_Colour);
	praatButton_black = praat_addMenuCommand (U"Picture", U"Pen", U"Black", nullptr, praat_CHECKBUTTON, GRAPHICS_Black);
	praatButton_white = praat_addMenuCommand (U"Picture", U"Pen", U"White", nullptr, praat_CHECKBUTTON, GRAPHICS_White);
	praatButton_red = praat_addMenuCommand (U"Picture", U"Pen", U"Red", nullptr, praat_CHECKBUTTON, GRAPHICS_Red);
	praatButton_green = praat_addMenuCommand (U"Picture", U"Pen", U"Green", nullptr, praat_CHECKBUTTON, GRAPHICS_Green);
	praatButton_blue = praat_addMenuCommand (U"Picture", U"Pen", U"Blue", nullptr, praat_CHECKBUTTON, GRAPHICS_Blue);
	praatButton_yellow = praat_addMenuCommand (U"Picture", U"Pen", U"Yellow", nullptr, praat_CHECKBUTTON, GRAPHICS_Yellow);
	praatButton_cyan = praat_addMenuCommand (U"Picture", U"Pen", U"Cyan", nullptr, praat_CHECKBUTTON, GRAPHICS_Cyan);
	praatButton_magenta = praat_addMenuCommand (U"Picture", U"Pen", U"Magenta", nullptr, praat_CHECKBUTTON, GRAPHICS_Magenta);
	praatButton_maroon = praat_addMenuCommand (U"Picture", U"Pen", U"Maroon", nullptr, praat_CHECKBUTTON, GRAPHICS_Maroon);
	praatButton_lime = praat_addMenuCommand (U"Picture", U"Pen", U"Lime", nullptr, praat_CHECKBUTTON, GRAPHICS_Lime);
	praatButton_navy = praat_addMenuCommand (U"Picture", U"Pen", U"Navy", nullptr, praat_CHECKBUTTON, GRAPHICS_Navy);
	praatButton_teal = praat_addMenuCommand (U"Picture", U"Pen", U"Teal", nullptr, praat_CHECKBUTTON, GRAPHICS_Teal);
	praatButton_purple = praat_addMenuCommand (U"Picture", U"Pen", U"Purple", nullptr, praat_CHECKBUTTON, GRAPHICS_Purple);
	praatButton_olive = praat_addMenuCommand (U"Picture", U"Pen", U"Olive", nullptr, praat_CHECKBUTTON, GRAPHICS_Olive);
	praatButton_pink = praat_addMenuCommand (U"Picture", U"Pen", U"Pink", nullptr, praat_CHECKBUTTON, GRAPHICS_Pink);
	praatButton_silver = praat_addMenuCommand (U"Picture", U"Pen", U"Silver", nullptr, praat_CHECKBUTTON, GRAPHICS_Silver);
	praatButton_grey = praat_addMenuCommand (U"Picture", U"Pen", U"Grey", nullptr, praat_CHECKBUTTON, GRAPHICS_Grey);

	praat_addMenuCommand (U"Picture", U"Font", U"Font size...", nullptr, 0, GRAPHICS_Font_size);
	praatButton_10 = praat_addMenuCommand (U"Picture", U"Font", U"10", nullptr, praat_CHECKBUTTON | praat_NO_API, GRAPHICS_10);
	praatButton_12 = praat_addMenuCommand (U"Picture", U"Font", U"12", nullptr, praat_CHECKBUTTON | praat_NO_API, GRAPHICS_12);
	praatButton_14 = praat_addMenuCommand (U"Picture", U"Font", U"14", nullptr, praat_CHECKBUTTON | praat_NO_API, GRAPHICS_14);
	praatButton_18 = praat_addMenuCommand (U"Picture", U"Font", U"18", nullptr, praat_CHECKBUTTON | praat_NO_API, GRAPHICS_18);
	praatButton_24 = praat_addMenuCommand (U"Picture", U"Font", U"24", nullptr, praat_CHECKBUTTON | praat_NO_API, GRAPHICS_24);
	praat_addMenuCommand (U"Picture", U"Font", U"-- font ---", nullptr, 0, nullptr);
	praatButton_fonts [kGraphics_font_TIMES] = praat_addMenuCommand (U"Picture", U"Font", U"Times", nullptr, praat_RADIO_FIRST, GRAPHICS_Times);
	praatButton_fonts [kGraphics_font_HELVETICA] = praat_addMenuCommand (U"Picture", U"Font", U"Helvetica", nullptr, praat_RADIO_NEXT, GRAPHICS_Helvetica);
	praatButton_fonts [kGraphics_font_PALATINO] = praat_addMenuCommand (U"Picture", U"Font", U"Palatino", nullptr, praat_RADIO_NEXT, GRAPHICS_Palatino);
	praatButton_fonts [kGraphics_font_COURIER] = praat_addMenuCommand (U"Picture", U"Font", U"Courier", nullptr, praat_RADIO_NEXT, GRAPHICS_Courier);

	praat_addMenuCommand (U"Picture", U"Help", U"Picture window help", nullptr, '?', HELP_PictureWindowHelp);
	praat_addMenuCommand (U"Picture", U"Help", U"About special symbols", nullptr, 0, HELP_AboutSpecialSymbols);
	praat_addMenuCommand (U"Picture", U"Help", U"About text styles", nullptr, 0, HELP_AboutTextStyles);
	praat_addMenuCommand (U"Picture", U"Help", U"Phonetic symbols", nullptr, 0, HELP_PhoneticSymbols);
	praat_addMenuCommand (U"Picture", U"Help", U"-- manual --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Picture", U"Help",
		Melder_cat (U"Search ", praatP.title, U" manual..."),
		nullptr, 'M', HELP_SearchManual_Picture);

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
	theCurrentPraatPicture -> graphics = static_cast<Graphics> (Picture_peekGraphics (praat_picture.get()));

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
